/*
 * keypad.c
 *
 *  Created on: Jan 12, 2026
 *      Author: Rahul B.
 */

#include "keypad.h"
#include "stm32f446xx_gpio_driver.h"
#include "lcd.h"

/* ===== KEYPAD KEY MAP ===== */
static const char keypad_map[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

static const uint8_t row_pins[4] = {
    KEYPAD_R0_PIN, KEYPAD_R1_PIN, KEYPAD_R2_PIN, KEYPAD_R3_PIN
};

static const uint8_t col_pins[4] = {
    KEYPAD_C0_PIN, KEYPAD_C1_PIN, KEYPAD_C2_PIN, KEYPAD_C3_PIN
};

/* ===== PRIVATE HELPER FUNCTIONS ===== */

void Keypad_Debounce(void)
{
    LCD_DelayMs(DEBOUNCE_DELAY_MS);
}

/* ===== PUBLIC API IMPLEMENTATION ===== */

void Keypad_Init(void)
{
    GPIO_Handle_t keypad_pin;

    // Enable clock
    GPIO_PeriClockControl(KEYPAD_ROW_PORT, ENABLE);
    GPIO_PeriClockControl(KEYPAD_COL_PORT, ENABLE);

    // Configure rows as outputs (push-pull, initially HIGH)
    keypad_pin.pGPIOx = KEYPAD_ROW_PORT;
    keypad_pin.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
    keypad_pin.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    keypad_pin.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    keypad_pin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

    for(uint8_t i = 0; i < 4; i++) {
        keypad_pin.GPIO_PinConfig.GPIO_PinNumber = row_pins[i];
        GPIO_Init(&keypad_pin);
        GPIO_WriteToOutputPin(KEYPAD_ROW_PORT, row_pins[i], GPIO_PIN_SET);
    }

    // Configure columns as inputs with pull-up
    keypad_pin.pGPIOx = KEYPAD_COL_PORT;
    keypad_pin.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
    keypad_pin.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    keypad_pin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;

    for(uint8_t i = 0; i < 4; i++) {
        keypad_pin.GPIO_PinConfig.GPIO_PinNumber = col_pins[i];
        GPIO_Init(&keypad_pin);
    }
}

char Keypad_GetKey(void)
{
    for(uint8_t row = 0; row < 4; row++) {
        // Set current row LOW, others HIGH
        for(uint8_t r = 0; r < 4; r++) {
            if(r == row) {
                GPIO_WriteToOutputPin(KEYPAD_ROW_PORT, row_pins[r], GPIO_PIN_RESET);
            } else {
                GPIO_WriteToOutputPin(KEYPAD_ROW_PORT, row_pins[r], GPIO_PIN_SET);
            }
        }

        LCD_DelayUs(10); // Small delay for signal to stabilize

        // Check columns
        for(uint8_t col = 0; col < 4; col++) {
            if(GPIO_ReadFromInputPin(KEYPAD_COL_PORT, col_pins[col]) == GPIO_PIN_RESET) {
                // Key pressed
                Keypad_Debounce();

                // Wait for key release
                while(GPIO_ReadFromInputPin(KEYPAD_COL_PORT, col_pins[col]) == GPIO_PIN_RESET);
                Keypad_Debounce();

                // Set all rows HIGH again
                for(uint8_t r = 0; r < 4; r++) {
                    GPIO_WriteToOutputPin(KEYPAD_ROW_PORT, row_pins[r], GPIO_PIN_SET);
                }

                return keypad_map[row][col];
            }
        }
    }

    // Set all rows HIGH again
    for(uint8_t r = 0; r < 4; r++) {
        GPIO_WriteToOutputPin(KEYPAD_ROW_PORT, row_pins[r], GPIO_PIN_SET);
    }

    return KEY_NONE;
}

char Keypad_WaitForKey(void)
{
    char key = KEY_NONE;
    while(key == KEY_NONE) {
        key = Keypad_GetKey();
        LCD_DelayMs(10); // Polling delay
    }
    return key;
}

uint8_t Keypad_GetNumericInput(char *buffer, uint8_t max_length, uint8_t mask_display)
{
    uint8_t index = 0;
    char key;

    while(index < max_length) {
        key = Keypad_WaitForKey();

        // Check if it's a digit
        if(key >= '0' && key <= '9') {
            buffer[index++] = key;

            // Display on LCD
            if(mask_display) {
                LCD_SendData('*');
            } else {
                LCD_SendData(key);
            }
        }
        // Backspace functionality
        else if(key == '*' && index > 0) {
            index--;
            LCD_SendCommand(0x10); // Move cursor left
            LCD_SendData(' ');     // Clear character
            LCD_SendCommand(0x10); // Move cursor left again
        }
        // Enter key to finish
        else if(key == '#') {
            break;
        }
    }

    buffer[index] = '\0';
    return index;
}
