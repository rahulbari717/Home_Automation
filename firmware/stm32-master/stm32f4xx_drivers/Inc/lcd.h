/*
 * lcd.h
 *
 *  Created on: Jan 12, 2026
 *      Author: Rahul B.
 *  Description: 16x2 LCD driver (4-bit mode, no I2C)
 */

#ifndef INC_LCD_H_
#define INC_LCD_H_

#include <stdint.h>
#include "config.h"

/* ===== LCD COMMANDS ===== */
#define LCD_CMD_CLEAR           0x01
#define LCD_CMD_HOME            0x02
#define LCD_CMD_ENTRY_MODE      0x06  // Increment cursor
#define LCD_CMD_DISPLAY_ON      0x0C  // Display ON, Cursor OFF
#define LCD_CMD_DISPLAY_OFF     0x08
#define LCD_CMD_CURSOR_BLINK    0x0F  // Display ON, Cursor ON, Blink ON
#define LCD_CMD_CURSOR_ON       0x0E  // Display ON, Cursor ON
#define LCD_CMD_4BIT_2LINE      0x28  // 4-bit mode, 2 lines, 5x8 font
#define LCD_CMD_8BIT_1LINE      0x30
#define LCD_CMD_SET_CURSOR      0x80  // Set DDRAM address

/* ===== LCD LINE ADDRESSES ===== */
#define LCD_LINE1_ADDR          0x00
#define LCD_LINE2_ADDR          0x40

/* ===== API FUNCTIONS ===== */

/**
 * @brief Initialize LCD in 4-bit mode
 */
void LCD_Init(void);

/**
 * @brief Send command to LCD
 * @param cmd Command byte
 */
void LCD_SendCommand(uint8_t cmd);

/**
 * @brief Send data (character) to LCD
 * @param data Character to display
 */
void LCD_SendData(uint8_t data);

/**
 * @brief Print string on LCD
 * @param str Null-terminated string
 */
void LCD_PrintString(const char *str);

/**
 * @brief Clear LCD display
 */
void LCD_Clear(void);

/**
 * @brief Set cursor position
 * @param row Row number (0 or 1)
 * @param col Column number (0-15)
 */
void LCD_SetCursor(uint8_t row, uint8_t col);

/**
 * @brief Print formatted line (centered, left-aligned, etc.)
 * @param row Row number (0 or 1)
 * @param str String to print (max 16 chars)
 */
void LCD_PrintLine(uint8_t row, const char *str);

/**
 * @brief Display two lines with cursor indicator
 * @param line1 First line text
 * @param line2 Second line text
 * @param cursor_on_line1 1 if cursor on line1, 0 if on line2
 */
void LCD_DisplayMenu(const char *line1, const char *line2, uint8_t cursor_on_line1);

/**
 * @brief Create custom character
 * @param location Character location (0-7)
 * @param pattern 8-byte pattern array
 */
void LCD_CreateCustomChar(uint8_t location, uint8_t pattern[8]);

/**
 * @brief Microsecond delay (approximate)
 * @param us Microseconds to delay
 */
void LCD_DelayUs(uint32_t us);

/**
 * @brief Millisecond delay
 * @param ms Milliseconds to delay
 */
void LCD_DelayMs(uint32_t ms);

#endif /* INC_LCD_H_ */
