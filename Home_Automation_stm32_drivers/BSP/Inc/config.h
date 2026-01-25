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
#define LOCKOUT_TIME_SEC            (5)
#define MASTER_PIN                  "1234"
#define TEMP_UPDATE_INTERVAL        5000  // ms
#define OLED_UPDATE_INTERVAL        1000  // ms
#define LCD_UPDATE_INTERVAL         500   // ms


/* ===== LDR SENSORS (ADC) ===== */
#define SENSOR_GPIO_PORT        	GPIOA
#define SENSOR_LDR1_PIN         	GPIO_PIN_NO_0  // ADC1_IN0
#define SENSOR_LDR2_PIN         	GPIO_PIN_NO_1  // ADC1_IN1
#define SENSOR_ADC              	ADC1
#define SENSOR_LDR1_CHANNEL     	0              // ADC Channel 0
#define SENSOR_LDR2_CHANNEL     	1              // ADC Channel 1

/* ===== USART2 (ST-LINK Virtual COM Port) ===== */
// Port A - Reserved for debugging/programming
#define USART_VCP_PORT          	GPIOA
#define USART_TX_PIN            	GPIO_PIN_NO_2  // USART2_TX (AF7)
#define USART_RX_PIN            	GPIO_PIN_NO_3  // USART2_RX (AF7)
#define USART_VCP               	USART2
#define USART_VCP_AF            	7

/* ===== BUZZER ===== */
#define BUZZER_PORT         		GPIOA
#define BUZZER_PIN          		GPIO_PIN_NO_4

/* ===== LED INDICATORS ===== */
#define LED_PORT            		GPIOA
#define LED_GREEN_PIN       		GPIO_PIN_NO_5
#define LED_RED_PIN         		GPIO_PIN_NO_6
#define LED_WHITE_PIN         		GPIO_PIN_NO_7

/* ===== DS18B20 Configuration ===== */
#define DS18B20_PORT                GPIOA
#define DS18B20_PIN                 GPIO_PIN_NO_8

/* ===== BLUETOOTH HC-05 (USART1) ===== */
#define BT_USART_PORT           	GPIOA
#define BT_TX_PIN               	GPIO_PIN_NO_9   // USART1_TX (AF7)
#define BT_RX_PIN               	GPIO_PIN_NO_10  // USART1_RX (AF7)
#define BT_USART                	USART1
#define BT_USART_AF             	7
#define BT_BAUD_RATE            	9600

/* ===== KEYPAD PIN CONFIGURATION ===== */
/* 4x4 Matrix Keypad Layout:
 *        C0    C1    C2    C3
 *   R0   1     2     3     A
 *   R1   4     5     6     B
 *   R2   7     8     9     C
 *   R3   *     0     #     D
 */

// Rows (Output) - Port B
#define KEYPAD_ROW_PORT             GPIOB
#define KEYPAD_R0_PIN               GPIO_PIN_NO_0
#define KEYPAD_R1_PIN               GPIO_PIN_NO_1
#define KEYPAD_R2_PIN               GPIO_PIN_NO_2
#define KEYPAD_R3_PIN               GPIO_PIN_NO_3

// Columns (Input with Pull-up) - Port B
#define KEYPAD_COL_PORT             GPIOB
#define KEYPAD_C0_PIN               GPIO_PIN_NO_4
#define KEYPAD_C1_PIN               GPIO_PIN_NO_5
#define KEYPAD_C2_PIN               GPIO_PIN_NO_6
#define KEYPAD_C3_PIN               GPIO_PIN_NO_7

/* ===== OLED DISPLAY (128x64, I2C1) ===== */
#define OLED_I2C_PORT               GPIOB
#define OLED_SCL_PIN                GPIO_PIN_NO_8  // I2C1_SCL (AF4)
#define OLED_SDA_PIN                GPIO_PIN_NO_9  // I2C1_SDA (AF4)
#define OLED_I2C                    I2C1
#define OLED_I2C_AF                 4              // Alternate Function 4

/* ===== MOTION/PROXIMITY SENSORS ===== */
// RCWL-0516 Microwave Radar (3.3V output - SAFE)
#define RCWL_PORT               	GPIOB
#define RCWL_PIN                	GPIO_PIN_NO_10

/* ===== KY-028 Configuration ===== */
#define KY028_DIGITAL_PORT          GPIOB
#define KY028_DIGITAL_PIN           GPIO_PIN_NO_11
#define TEMP_ALARM_THRESHOLD        28  // Degrees Celsius

// Relays (with transistor buffers)
#define RELAY_PORT                  GPIOB
#define RELAY1_PIN                  GPIO_PIN_NO_12
#define RELAY2_PIN                  GPIO_PIN_NO_13
#define RELAY3_PIN                  GPIO_PIN_NO_14
#define RELAY4_PIN                  GPIO_PIN_NO_15

/* ===== LCD 16x2 PIN CONFIGURATION (4-bit mode) ===== */
// Control Pins - Port C
#define LCD_CTRL_PORT               GPIOC
#define LCD_RS_PIN                  GPIO_PIN_NO_0  // Register Select
#define LCD_EN_PIN                  GPIO_PIN_NO_1  // Enable

// Data Pins (D4-D7) - Port C
#define LCD_DATA_PORT               GPIOC
#define LCD_D4_PIN                  GPIO_PIN_NO_2
#define LCD_D5_PIN                  GPIO_PIN_NO_3
#define LCD_D6_PIN                  GPIO_PIN_NO_4
#define LCD_D7_PIN                  GPIO_PIN_NO_5

// IR Proximity Sensors (powered by 3.3V - SAFE)
#define IR1_PORT                    GPIOC
#define IR1_PIN                     GPIO_PIN_NO_6
#define IR2_PORT                    GPIOC
#define IR2_PIN                     GPIO_PIN_NO_7

/* ===== WAKEUP BUTTON ===== */
#define WAKEUP_BTN_PORT             GPIOC
#define WAKEUP_BTN_PIN              GPIO_PIN_NO_13
#define WAKEUP_BTN_IRQ              EXTI15_10_IRQn

/* ===== TIMING CONSTANTS ===== */
#define DEBOUNCE_DELAY_MS           50
#define LCD_DELAY_US                50
#define BUZZER_BEEP_MS              200
#define SCREEN_TIMEOUT_SEC          30

/* ===== KEYPAD KEY CODES ===== */
#define KEY_UP                      '2'
#define KEY_DOWN                    '8'
#define KEY_ENTER                   '5'
#define KEY_BACK                    '*'
#define KEY_LOGOUT                  '#'
#define KEY_NONE                    '\0'

/* ===== MENU NAVIGATION ===== */
#define MENU_MAX_DEPTH              3
#define MENU_ITEMS_PER_SCREEN       2

/* ===== State Machine Enumeration ===== */
typedef enum {
    STATE_STANDBY,
    STATE_AUTHENTICATING,
    STATE_ACTIVE_MENU,
    STATE_LOCKOUT
} SystemState_t;

#endif /* INC_CONFIG_H_ */
