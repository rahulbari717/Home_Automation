/*
 * bsp_lcd.h
 *
 * Description: HD44780 LCD Driver (4-bit mode) for "The Citadel"
 * Created on: Jan 24, 2026
 * Author: Rahul B.
 */

#ifndef INC_BSP_LCD_H_
#define INC_BSP_LCD_H_

#include "stm32f446xx_gpio_driver.h"
#include "config.h" // Includes your pin definitions

// LCD Commands (HD44780 Standard)
#define LCD_CMD_CLEAR           0x01
#define LCD_CMD_RETURN_HOME     0x02
#define LCD_CMD_ENTRY_MODE      0x06
#define LCD_CMD_DISPLAY_ON      0x0C
#define LCD_CMD_FUNCTION_SET    0x28 // 4-bit mode, 2 lines, 5x8 font

/* Initializes GPIOs and LCD controller */
void BSP_LCD_Init(void);

/* Sends a command byte (e.g., Clear Screen) */
void BSP_LCD_SendCommand(uint8_t cmd);

/* Sends a data byte (ASCII character) */
void BSP_LCD_SendData(uint8_t data);

/* Prints a full string */
void BSP_LCD_PrintString(char *message);

/* Moves cursor to specific row (0/1) and column (0-15) */
void BSP_LCD_SetCursor(uint8_t row, uint8_t col);


#endif /* INC_BSP_LCD_H_ */
