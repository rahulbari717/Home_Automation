/*
 * bsp_i2c_oled.h
 *
 * Created on: Jan 23, 2026
 * Author: Rahul B.
 * Description: Board Support Package for SSD1306 OLED (128x64) via I2C
 */

#ifndef BSP_I2C_OLED_H_
#define BSP_I2C_OLED_H_

#include "stm32f446xx.h"
#include "stm32f446xx_i2c_driver.h"
#include "stm32f446xx_gpio_driver.h"

// --- Configuration ---
#define OLED_I2C_ADDR       0x3C // 7-bit Address (0x78 if 8-bit)
#define OLED_WIDTH          128
#define OLED_HEIGHT         64

// --- Helper Macros ---
// Control byte: Co = 0, D/C = 0 -> 0x00 (Command)
// Control byte: Co = 0, D/C = 1 -> 0x40 (Data)
#define OLED_CONTROL_CMD    0x00
#define OLED_CONTROL_DATA   0x40

// --- Function Prototypes ---

/* Initializes I2C1 GPIOs and the SSD1306 Display */
void BSP_OLED_Init(void);

/* Clears the screen buffer (does not update display immediately) */
void BSP_OLED_Clear(void);

/* Pushes the buffer to the actual OLED screen via I2C */
void BSP_OLED_Update(void);

/* Draws a character at x,y */
void BSP_OLED_DrawChar(uint8_t x, uint8_t y, char c);

/* Draws a string at x,y */
void BSP_OLED_PrintString(uint8_t x, uint8_t y, char *str);

/* Draws a single pixel (0 = OFF, 1 = ON) */
void BSP_OLED_DrawPixel(uint8_t x, uint8_t y, uint8_t state);

/* Draws a string at x,y */
void BSP_OLED_PrintString(uint8_t x, uint8_t y, char *str);

/* NEW: Printf style function for OLED */
/* Usage: OLED_Printf(0, 0, "Temp: %d", temp_val); */
void OLED_Printf(uint8_t x, uint8_t y, const char *format, ...);

/* NEW: Function to draw a full bitmap */
void BSP_OLED_DrawBitmap(const uint8_t *bitmap);
#endif /* BSP_I2C_OLED_H_ */
