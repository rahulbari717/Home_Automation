/*
 * room_control.h
 *
 *  Created on: Jan 12, 2026
 *      Author: Rahul B.
 *  Description: Room device control (lights, fans, AC, etc.)
 */

#ifndef INC_ROOM_CONTROL_H_
#define INC_ROOM_CONTROL_H_

#include <stdint.h>
#include "config.h"

/* ===== DEVICE IDENTIFIERS ===== */
typedef enum {
    // Hall
    DEVICE_HALL_LIGHT = 0,
    DEVICE_HALL_FAN,
    DEVICE_HALL_TV,

    // Kitchen
    DEVICE_KITCHEN_LIGHT,
    DEVICE_KITCHEN_EXHAUST,
    DEVICE_KITCHEN_COFFEE,

    // Master Bedroom
    DEVICE_MBED_LAMP,
    DEVICE_MBED_AC,
    DEVICE_MBED_BLINDS,
    DEVICE_MBED_FAN,

    // Guest Bedroom
    DEVICE_GBED_LAMP,
    DEVICE_GBED_AC,
    DEVICE_GBED_BLINDS,
    DEVICE_GBED_FAN,

    // Garden
    DEVICE_GARDEN_SPRINKLER,
    DEVICE_GARDEN_LIGHT,
    DEVICE_GARDEN_FOUNTAIN,

    DEVICE_COUNT
} DeviceID_t;

/* ===== DEVICE STATE ===== */
typedef struct {
    uint8_t is_on;              // 1 = ON, 0 = OFF
    uint8_t level;              // For dimmers (0-100%) or fan speed (0-4)
    int8_t temperature;         // For AC control (-128 to 127°C)
} DeviceState_t;

/* ===== API FUNCTIONS ===== */

/**
 * @brief Initialize all room control GPIO pins
 */
void Room_Init(void);

/**
 * @brief Toggle device ON/OFF
 * @param device Device identifier
 */
void Room_ToggleDevice(DeviceID_t device);

/**
 * @brief Turn device ON
 * @param device Device identifier
 */
void Room_TurnOn(DeviceID_t device);

/**
 * @brief Turn device OFF
 * @param device Device identifier
 */
void Room_TurnOff(DeviceID_t device);

/**
 * @brief Get device state
 * @param device Device identifier
 * @return Pointer to device state structure
 */
DeviceState_t* Room_GetDeviceState(DeviceID_t device);

/**
 * @brief Control dimmer (PWM) - For night lamps
 * @param device Device identifier
 */
void Room_ControlDimmer(DeviceID_t device);

/**
 * @brief Control AC temperature
 * @param device Device identifier (AC device)
 */
void Room_ControlAC(DeviceID_t device);

/**
 * @brief Control fan speed (0-4)
 * @param device Device identifier (fan device)
 */
void Room_ControlFanSpeed(DeviceID_t device);

/**
 * @brief Turn off all devices (emergency/logout)
 */
void Room_AllOff(void);

#endif /* INC_ROOM_CONTROL_H_ */
