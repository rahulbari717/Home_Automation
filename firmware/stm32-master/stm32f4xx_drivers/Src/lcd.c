/*
 * lcd.c
 *
 *  Created on: Jan 12, 2026
 *      Author: Rahul B.
 */

#include "lcd.h"
#include "stm32f446xx_gpio_driver.h"
#include <string.h>

/* ===== PRIVATE HELPER FUNCTIONS ===== */

static void LCD_EnablePulse(void)
{
    GPIO_WriteToOutputPin(LCD_CTRL_PORT, LCD_EN_PIN, GPIO_PIN_SET);
    LCD_DelayUs(1);
    GPIO_WriteToOutputPin(LCD_CTRL_PORT, LCD_EN_PIN, GPIO_PIN_RESET);
    LCD_DelayUs(100);
}

static void LCD_SendNibble(uint8_t nibble)
{
    GPIO_WriteToOutputPin(LCD_DATA_PORT, LCD_D4_PIN, (nibble >> 0) & 0x01);
    GPIO_WriteToOutputPin(LCD_DATA_PORT, LCD_D5_PIN, (nibble >> 1) & 0x01);
    GPIO_WriteToOutputPin(LCD_DATA_PORT, LCD_D6_PIN, (nibble >> 2) & 0x01);
    GPIO_WriteToOutputPin(LCD_DATA_PORT, LCD_D7_PIN, (nibble >> 3) & 0x01);
    LCD_EnablePulse();
}

void LCD_DelayUs(uint32_t us)
{
    // Approximate delay for 180MHz core clock
    // Adjust this multiplier based on actual clock speed
    for(uint32_t i = 0; i < (us * 45); i++) {
        __asm("NOP");
    }
}

void LCD_DelayMs(uint32_t ms)
{
    for(uint32_t i = 0; i < ms; i++) {
        LCD_DelayUs(1000);
    }
}

/* ===== PUBLIC API IMPLEMENTATION ===== */

void LCD_Init(void)
{
    GPIO_Handle_t lcd_pin;

    // Enable clocks
    GPIO_PeriClockControl(LCD_CTRL_PORT, ENABLE);
    GPIO_PeriClockControl(LCD_DATA_PORT, ENABLE);

    // Configure RS and EN as outputs
    lcd_pin.pGPIOx = LCD_CTRL_PORT;
    lcd_pin.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
    lcd_pin.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    lcd_pin.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    lcd_pin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

    lcd_pin.GPIO_PinConfig.GPIO_PinNumber = LCD_RS_PIN;
    GPIO_Init(&lcd_pin);
    lcd_pin.GPIO_PinConfig.GPIO_PinNumber = LCD_EN_PIN;
    GPIO_Init(&lcd_pin);

    // Configure D4-D7 as outputs
    lcd_pin.pGPIOx = LCD_DATA_PORT;
    lcd_pin.GPIO_PinConfig.GPIO_PinNumber = LCD_D4_PIN;
    GPIO_Init(&lcd_pin);
    lcd_pin.GPIO_PinConfig.GPIO_PinNumber = LCD_D5_PIN;
    GPIO_Init(&lcd_pin);
    lcd_pin.GPIO_PinConfig.GPIO_PinNumber = LCD_D6_PIN;
    GPIO_Init(&lcd_pin);
    lcd_pin.GPIO_PinConfig.GPIO_PinNumber = LCD_D7_PIN;
    GPIO_Init(&lcd_pin);

    // LCD Initialization sequence (4-bit mode)
    LCD_DelayMs(50); // Wait for LCD power-up

    GPIO_WriteToOutputPin(LCD_CTRL_PORT, LCD_RS_PIN, GPIO_PIN_RESET);
    GPIO_WriteToOutputPin(LCD_CTRL_PORT, LCD_EN_PIN, GPIO_PIN_RESET);

    // Initialize to 8-bit mode first
    LCD_SendNibble(0x03);
    LCD_DelayMs(5);
    LCD_SendNibble(0x03);
    LCD_DelayUs(150);
    LCD_SendNibble(0x03);
    LCD_DelayUs(150);

    // Switch to 4-bit mode
    LCD_SendNibble(0x02);
    LCD_DelayUs(150);

    // Configure LCD
    LCD_SendCommand(LCD_CMD_4BIT_2LINE);    // 4-bit, 2 lines, 5x8
    LCD_SendCommand(LCD_CMD_DISPLAY_ON);    // Display ON, cursor OFF
    LCD_SendCommand(LCD_CMD_CLEAR);         // Clear display
    LCD_DelayMs(2);
    LCD_SendCommand(LCD_CMD_ENTRY_MODE);    // Increment cursor
}

void LCD_SendCommand(uint8_t cmd)
{
    GPIO_WriteToOutputPin(LCD_CTRL_PORT, LCD_RS_PIN, GPIO_PIN_RESET);

    // Send upper nibble
    LCD_SendNibble((cmd >> 4) & 0x0F);

    // Send lower nibble
    LCD_SendNibble(cmd & 0x0F);

    if(cmd == LCD_CMD_CLEAR || cmd == LCD_CMD_HOME) {
        LCD_DelayMs(2);
    }
}

void LCD_SendData(uint8_t data)
{
    GPIO_WriteToOutputPin(LCD_CTRL_PORT, LCD_RS_PIN, GPIO_PIN_SET);

    // Send upper nibble
    LCD_SendNibble((data >> 4) & 0x0F);

    // Send lower nibble
    LCD_SendNibble(data & 0x0F);
}

void LCD_PrintString(const char *str)
{
    while(*str) {
        LCD_SendData(*str++);
    }
}

void LCD_Clear(void)
{
    LCD_SendCommand(LCD_CMD_CLEAR);
    LCD_DelayMs(2);
}

void LCD_SetCursor(uint8_t row, uint8_t col)
{
    uint8_t address = (row == 0) ? LCD_LINE1_ADDR : LCD_LINE2_ADDR;
    address += col;
    LCD_SendCommand(LCD_CMD_SET_CURSOR | address);
}

void LCD_PrintLine(uint8_t row, const char *str)
{
    LCD_SetCursor(row, 0);

    char buffer[17] = "                "; // 16 spaces + null
    uint8_t len = strlen(str);

    if(len > 16) len = 16;
    memcpy(buffer, str, len);

    for(uint8_t i = 0; i < 16; i++) {
        LCD_SendData(buffer[i]);
    }
}

void LCD_DisplayMenu(const char *line1, const char *line2, uint8_t cursor_on_line1)
{
    char display_line1[17];
    char display_line2[17];

    // Prepare line 1
    if(cursor_on_line1) {
        snprintf(display_line1, 17, ">%-15s", line1);
    } else {
        snprintf(display_line1, 17, " %-15s", line1);
    }

    // Prepare line 2
    if(!cursor_on_line1) {
        snprintf(display_line2, 17, ">%-15s", line2);
    } else {
        snprintf(display_line2, 17, " %-15s", line2);
    }

    LCD_PrintLine(0, display_line1);
    LCD_PrintLine(1, display_line2);
}

void LCD_CreateCustomChar(uint8_t location, uint8_t pattern[8])
{
    location &= 0x07; // Only 8 locations (0-7)
    LCD_SendCommand(0x40 | (location << 3));

    for(uint8_t i = 0; i < 8; i++) {
        LCD_SendData(pattern[i]);
    }
}
