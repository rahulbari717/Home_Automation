/*
 * room_control.c
 *
 *  Created on: Jan 12, 2026
 *      Author: Rahul B.
 */

#include "room_control.h"
#include "stm32f446xx_gpio_driver.h"
#include "lcd.h"
#include "keypad.h"
#include <stdio.h>
#include <string.h>

/* ===== DEVICE STATE STORAGE ===== */
static DeviceState_t device_states[DEVICE_COUNT];

/* ===== DEVICE PIN MAPPING ===== */
typedef struct {
    GPIO_RegDef_t *port;
    uint8_t pin;
    const char *name;
} DevicePin_t;

static const DevicePin_t device_pins[DEVICE_COUNT] = {
    // Hall
    {ROOM_CTRL_PORT, HALL_LIGHT_PIN, "Hall Light"},
    {ROOM_CTRL_PORT, HALL_FAN_PIN, "Hall Fan"},
    {ROOM_CTRL_PORT, HALL_TV_PIN, "Hall TV"},

    // Kitchen
    {ROOM_CTRL_PORT, KITCHEN_LIGHT_PIN, "Kitchen Light"},
    {ROOM_CTRL_PORT, KITCHEN_EXHAUST_PIN, "Kitchen Exhaust"},
    {ROOM_CTRL_PORT, KITCHEN_COFFEE_PIN, "Coffee Maker"},

    // Master Bedroom
    {ROOM_CTRL_PORT, MBED_LAMP_PIN, "MBed Lamp"},
    {ROOM_CTRL_PORT, MBED_AC_PIN, "MBed AC"},
    {LED_PORT, GPIO_PIN_NO_8, "MBed Blinds"}, // Using available pins
    {ROOM_CTRL_PORT, MBED_FAN_PIN, "MBed Fan"},

    // Guest Bedroom
    {ROOM_CTRL_PORT, GBED_LAMP_PIN, "GBed Lamp"},
    {ROOM_CTRL_PORT, GBED_AC_PIN, "GBed AC"},
    {LED_PORT, GPIO_PIN_NO_9, "GBed Blinds"},
    {ROOM_CTRL_PORT, GBED_FAN_PIN, "GBed Fan"},

    // Garden
    {ROOM_CTRL_PORT, GARDEN_SPRINKLER_PIN, "Sprinkler"},
    {ROOM_CTRL_PORT, GARDEN_LIGHT_PIN, "Garden Light"},
    {ROOM_CTRL_PORT, GARDEN_FOUNTAIN_PIN, "Fountain"}
};

/* ===== HELPER FUNCTIONS ===== */

static void DisplayDeviceStatus(DeviceID_t device)
{
    char line1[17], line2[17];

    snprintf(line1, 17, "%s", device_pins[device].name);

    if(device_states[device].is_on) {
        if(device_states[device].level > 0) {
            snprintf(line2, 17, "ON - Level:%d%%", device_states[device].level);
        } else if(device_states[device].temperature != 0) {
            snprintf(line2, 17, "ON - Temp:%dC", device_states[device].temperature);
        } else {
            snprintf(line2, 17, "Status: ON");
        }
    } else {
        snprintf(line2, 17, "Status: OFF");
    }

    LCD_Clear();
    LCD_PrintLine(0, line1);
    LCD_PrintLine(1, line2);
    LCD_DelayMs(1500);
}

/* ===== PUBLIC API IMPLEMENTATION ===== */

void Room_Init(void)
{
    GPIO_Handle_t gpio_device;

    // Enable clocks
    GPIO_PeriClockControl(ROOM_CTRL_PORT, ENABLE);
    GPIO_PeriClockControl(LED_PORT, ENABLE);

    // Configure all device pins as outputs
    gpio_device.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
    gpio_device.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    gpio_device.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    gpio_device.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

    for(uint8_t i = 0; i < DEVICE_COUNT; i++) {
        gpio_device.pGPIOx = device_pins[i].port;
        gpio_device.GPIO_PinConfig.GPIO_PinNumber = device_pins[i].pin;
        GPIO_Init(&gpio_device);

        // Initialize to OFF
        GPIO_WriteToOutputPin(device_pins[i].port, device_pins[i].pin, GPIO_PIN_RESET);

        // Initialize state
        device_states[i].is_on = 0;
        device_states[i].level = 0;
        device_states[i].temperature = 24; // Default AC temp
    }
}

void Room_ToggleDevice(DeviceID_t device)
{
    if(device >= DEVICE_COUNT) return;

    if(device_states[device].is_on) {
        Room_TurnOff(device);
    } else {
        Room_TurnOn(device);
    }

    DisplayDeviceStatus(device);
}

void Room_TurnOn(DeviceID_t device)
{
    if(device >= DEVICE_COUNT) return;

    GPIO_WriteToOutputPin(device_pins[device].port,
                         device_pins[device].pin,
                         GPIO_PIN_SET);

    device_states[device].is_on = 1;
}

void Room_TurnOff(DeviceID_t device)
{
    if(device >= DEVICE_COUNT) return;

    GPIO_WriteToOutputPin(device_pins[device].port,
                         device_pins[device].pin,
                         GPIO_PIN_RESET);

    device_states[device].is_on = 0;
}

DeviceState_t* Room_GetDeviceState(DeviceID_t device)
{
    if(device >= DEVICE_COUNT) return NULL;
    return &device_states[device];
}

void Room_ControlDimmer(DeviceID_t device)
{
    if(device >= DEVICE_COUNT) return;

    char line1[17], line2[17];
    char key;
    uint8_t level = device_states[device].level;

    if(level == 0) level = 50; // Default to 50%

    snprintf(line1, 17, "%s", device_pins[device].name);

    while(1) {
        snprintf(line2, 17, "Level:%d%% 2/8/#", level);
        LCD_Clear();
        LCD_PrintLine(0, line1);
        LCD_PrintLine(1, line2);

        key = Keypad_WaitForKey();

        if(key == '2' && level < 100) {
            level += 10;
        } else if(key == '8' && level > 10) {
            level -= 10;
        } else if(key == '#') {
            break;
        }
    }

    device_states[device].level = level;
    if(level > 0) {
        device_states[device].is_on = 1;
        Room_TurnOn(device);
    } else {
        device_states[device].is_on = 0;
        Room_TurnOff(device);
    }

    // TODO: Implement actual PWM control for dimming
}

void Room_ControlAC(DeviceID_t device)
{
    if(device >= DEVICE_COUNT) return;

    char line1[17], line2[17];
    char key;
    int8_t temp = device_states[device].temperature;

    snprintf(line1, 17, "%s", device_pins[device].name);

    while(1) {
        snprintf(line2, 17, "Temp:%dC 2/8/#", temp);
        LCD_Clear();
        LCD_PrintLine(0, line1);
        LCD_PrintLine(1, line2);

        key = Keypad_WaitForKey();

        if(key == '2' && temp < 30) {
            temp++;
        } else if(key == '8' && temp > 16) {
            temp--;
        } else if(key == '5') {
            // Toggle ON/OFF
            if(device_states[device].is_on) {
                Room_TurnOff(device);
            } else {
                Room_TurnOn(device);
            }
        } else if(key == '#') {
            break;
        }
    }

    device_states[device].temperature = temp;
}

void Room_ControlFanSpeed(DeviceID_t device)
{
    if(device >= DEVICE_COUNT) return;

    char line1[17], line2[17];
    char key;
    uint8_t speed = device_states[device].level;

    snprintf(line1, 17, "%s", device_pins[device].name);

    while(1) {
        if(speed == 0) {
            snprintf(line2, 17, "OFF  2/8/#");
        } else {
            snprintf(line2, 17, "Speed:%d  2/8/#", speed);
        }

        LCD_Clear();
        LCD_PrintLine(0, line1);
        LCD_PrintLine(1, line2);

        key = Keypad_WaitForKey();

        if(key == '2' && speed < 4) {
            speed++;
        } else if(key == '8' && speed > 0) {
            speed--;
        } else if(key == '#') {
            break;
        }
    }

    device_states[device].level = speed;
    if(speed > 0) {
        device_states[device].is_on = 1;
        Room_TurnOn(device);
    } else {
        device_states[device].is_on = 0;
        Room_TurnOff(device);
    }
}

void Room_AllOff(void)
{
    for(uint8_t i = 0; i < DEVICE_COUNT; i++) {
        Room_TurnOff(i);
    }

    LCD_Clear();
    LCD_PrintLine(0, "All Devices");
    LCD_PrintLine(1, "Turned OFF");
    LCD_DelayMs(2000);
}
