/*
 * config.h
 *
 *  Created on: Jan 12, 2026
 *      Author: Rahul B.
 *  Description: System-wide configuration and pin mappings
 */

#ifndef INC_CONFIG_H_
#define INC_CONFIG_H_

#include "stm32f446xx.h"

/* ===== SYSTEM CONFIGURATION ===== */
#define MAX_USERS                   (5)
#define USERNAME_LENGTH             (10)
#define PIN_LENGTH                  (4)
#define MAX_LOGIN_ATTEMPTS          (3)
#define LOCKOUT_TIME_SEC            (20)

/* ===== KEYPAD PIN CONFIGURATION ===== */
/* 4x4 Matrix Keypad Layout:
 *        C0    C1    C2    C3
 *   R0   1     2     3     A
 *   R1   4     5     6     B
 *   R2   7     8     9     C
 *   R3   *     0     #     D
 */

// Rows (Output) - Port B
#define KEYPAD_ROW_PORT         GPIOB
#define KEYPAD_R0_PIN           GPIO_PIN_NO_0
#define KEYPAD_R1_PIN           GPIO_PIN_NO_1
#define KEYPAD_R2_PIN           GPIO_PIN_NO_2
#define KEYPAD_R3_PIN           GPIO_PIN_NO_3

// Columns (Input with Pull-up) - Port B
#define KEYPAD_COL_PORT         GPIOB
#define KEYPAD_C0_PIN           GPIO_PIN_NO_4
#define KEYPAD_C1_PIN           GPIO_PIN_NO_5
#define KEYPAD_C2_PIN           GPIO_PIN_NO_6
#define KEYPAD_C3_PIN           GPIO_PIN_NO_7

/* ===== LCD 16x2 PIN CONFIGURATION (4-bit mode) ===== */
// Control Pins - Port C
#define LCD_CTRL_PORT           GPIOC
#define LCD_RS_PIN              GPIO_PIN_NO_0  // Register Select
#define LCD_EN_PIN              GPIO_PIN_NO_1  // Enable

// Data Pins (D4-D7) - Port C
#define LCD_DATA_PORT           GPIOC
#define LCD_D4_PIN              GPIO_PIN_NO_2
#define LCD_D5_PIN              GPIO_PIN_NO_3
#define LCD_D6_PIN              GPIO_PIN_NO_4
#define LCD_D7_PIN              GPIO_PIN_NO_5

/* ===== LED INDICATORS ===== */
#define LED_PORT                GPIOA
#define LED_GREEN_PIN           GPIO_PIN_NO_5   // Success/Authenticated
#define LED_RED_PIN             GPIO_PIN_NO_6   // Error/Failed

/* ===== BUZZER ===== */
#define BUZZER_PORT             GPIOA
#define BUZZER_PIN              GPIO_PIN_NO_7

/* ===== WAKEUP BUTTON ===== */
#define WAKEUP_BTN_PORT         GPIOC
#define WAKEUP_BTN_PIN          GPIO_PIN_NO_13
#define WAKEUP_BTN_IRQ          EXTI15_10_IRQn

/* 
/* ===== ROOM CONTROL RELAYS/OUTPUTS ===== 
// Using Port D for room controls
#define ROOM_CTRL_PORT          GPIOD

// Hall
#define HALL_LIGHT_PIN          GPIO_PIN_NO_0
#define HALL_FAN_PIN            GPIO_PIN_NO_1
#define HALL_TV_PIN             GPIO_PIN_NO_2

// Kitchen
#define KITCHEN_LIGHT_PIN       GPIO_PIN_NO_3
#define KITCHEN_EXHAUST_PIN     GPIO_PIN_NO_4
#define KITCHEN_COFFEE_PIN      GPIO_PIN_NO_5

// Master Bedroom
#define MBED_LAMP_PIN           GPIO_PIN_NO_6
#define MBED_AC_PIN             GPIO_PIN_NO_7
#define MBED_FAN_PIN            GPIO_PIN_NO_8

// Guest Bedroom
#define GBED_LAMP_PIN           GPIO_PIN_NO_9
#define GBED_AC_PIN             GPIO_PIN_NO_10
#define GBED_FAN_PIN            GPIO_PIN_NO_11

// Garden
#define GARDEN_SPRINKLER_PIN    GPIO_PIN_NO_12
#define GARDEN_LIGHT_PIN        GPIO_PIN_NO_13
#define GARDEN_FOUNTAIN_PIN     GPIO_PIN_NO_14 */

/* ===== TIMING CONSTANTS ===== */
#define DEBOUNCE_DELAY_MS       50
#define LCD_DELAY_US            50
#define BUZZER_BEEP_MS          200
#define SCREEN_TIMEOUT_SEC      30

/* ===== KEYPAD KEY CODES ===== */
#define KEY_UP                  '2'
#define KEY_DOWN                '8'
#define KEY_ENTER               '5'
#define KEY_BACK                '*'
#define KEY_LOGOUT              '#'
#define KEY_NONE                '\0'

/* ===== MENU NAVIGATION ===== */
#define MENU_MAX_DEPTH          3
#define MENU_ITEMS_PER_SCREEN   2

/* ===== STATE MACHINE STATES ===== */
typedef enum {
    STATE_DEEP_SLEEP,
    STATE_AUTHENTICATION,
    STATE_MENU_NAVIGATION,
    STATE_LOCKOUT
} SystemState_t;

#endif /* INC_CONFIG_H_ */
