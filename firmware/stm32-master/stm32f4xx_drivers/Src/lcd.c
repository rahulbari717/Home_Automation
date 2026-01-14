///*
// * lcd.c
// *
// *  Created on: Jan 12, 2026
// *      Author: Rahul B.
// */
//
//
//
//#include "lcd.h"
//#include "stm32f446xx_gpio_driver.h"
//#include <string.h>
//#include <stdio.h>
//
//// --- Helper Functions (Made Public to fix Linker Error) ---
//
//// Calibrated for 16MHz
//void LCD_DelayUs(uint32_t us)
//{
//    uint32_t count = us * 4;
//    for(uint32_t i = 0; i < count; i++) {
//        __asm("NOP");
//    }
//}
//
//void LCD_DelayMs(uint32_t ms)
//{
//    for(uint32_t i = 0; i < ms; i++) {
//        LCD_DelayUs(1000);
//    }
//}
//
//// REMOVED 'static' keyword here
//void LCD_EnablePulse(void)
//{
//    GPIO_WriteToOutputPin(LCD_CTRL_PORT, LCD_EN_PIN, GPIO_PIN_SET);
//    LCD_DelayUs(20);
//    GPIO_WriteToOutputPin(LCD_CTRL_PORT, LCD_EN_PIN, GPIO_PIN_RESET);
//    LCD_DelayUs(50);
//}
//
//// REMOVED 'static' keyword here
//void LCD_SendNibble(uint8_t nibble)
//{
//    GPIO_WriteToOutputPin(LCD_DATA_PORT, LCD_D4_PIN, (nibble >> 0) & 0x01);
//    GPIO_WriteToOutputPin(LCD_DATA_PORT, LCD_D5_PIN, (nibble >> 1) & 0x01);
//    GPIO_WriteToOutputPin(LCD_DATA_PORT, LCD_D6_PIN, (nibble >> 2) & 0x01);
//    GPIO_WriteToOutputPin(LCD_DATA_PORT, LCD_D7_PIN, (nibble >> 3) & 0x01);
//
//    LCD_EnablePulse();
//}
//
//// --- Public Functions ---
//
//void LCD_Init(void)
//{
//    GPIO_Handle_t lcd_pin;
//    memset(&lcd_pin, 0, sizeof(lcd_pin)); // Clear garbage
//
//    // Enable Clocks
//    GPIO_PeriClockControl(LCD_CTRL_PORT, ENABLE);
//    GPIO_PeriClockControl(LCD_DATA_PORT, ENABLE);
//
//    // Common Config
//    lcd_pin.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
//    lcd_pin.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
//    lcd_pin.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
//    lcd_pin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
//
//    // Init Control Pins
//    lcd_pin.pGPIOx = LCD_CTRL_PORT;
//    lcd_pin.GPIO_PinConfig.GPIO_PinNumber = LCD_RS_PIN;
//    GPIO_Init(&lcd_pin);
//    lcd_pin.GPIO_PinConfig.GPIO_PinNumber = LCD_EN_PIN;
//    GPIO_Init(&lcd_pin);
//
//    // Init Data Pins
//    lcd_pin.pGPIOx = LCD_DATA_PORT;
//    lcd_pin.GPIO_PinConfig.GPIO_PinNumber = LCD_D4_PIN;
//    GPIO_Init(&lcd_pin);
//    lcd_pin.GPIO_PinConfig.GPIO_PinNumber = LCD_D5_PIN;
//    GPIO_Init(&lcd_pin);
//    lcd_pin.GPIO_PinConfig.GPIO_PinNumber = LCD_D6_PIN;
//    GPIO_Init(&lcd_pin);
//    lcd_pin.GPIO_PinConfig.GPIO_PinNumber = LCD_D7_PIN;
//    GPIO_Init(&lcd_pin);
//
//    // Initialization Sequence
//    LCD_DelayMs(50);
//
//    GPIO_WriteToOutputPin(LCD_CTRL_PORT, LCD_RS_PIN, GPIO_PIN_RESET);
//    GPIO_WriteToOutputPin(LCD_CTRL_PORT, LCD_EN_PIN, GPIO_PIN_RESET);
//
//    // Reset Sequence
//    LCD_SendNibble(0x03); LCD_DelayMs(5);
//    LCD_SendNibble(0x03); LCD_DelayUs(150);
//    LCD_SendNibble(0x03); LCD_DelayUs(150);
//    LCD_SendNibble(0x02); LCD_DelayUs(150);
//
//    // Setup
//    LCD_SendCommand(LCD_CMD_4BIT_2LINE);
//    LCD_SendCommand(LCD_CMD_DISPLAY_ON);
//    LCD_SendCommand(LCD_CMD_CLEAR);
//    LCD_DelayMs(2);
//    LCD_SendCommand(LCD_CMD_ENTRY_MODE);
//}
//
//void LCD_SendCommand(uint8_t cmd)
//{
//    GPIO_WriteToOutputPin(LCD_CTRL_PORT, LCD_RS_PIN, GPIO_PIN_RESET);
//    LCD_SendNibble((cmd >> 4) & 0x0F);
//    LCD_SendNibble(cmd & 0x0F);
//
//    if(cmd == LCD_CMD_CLEAR || cmd == LCD_CMD_HOME) LCD_DelayMs(2);
//    else LCD_DelayUs(50);
//}
//
//void LCD_SendData(uint8_t data)
//{
//    GPIO_WriteToOutputPin(LCD_CTRL_PORT, LCD_RS_PIN, GPIO_PIN_SET);
//    LCD_SendNibble((data >> 4) & 0x0F);
//    LCD_SendNibble(data & 0x0F);
//    LCD_DelayUs(50);
//}
//
//void LCD_PrintString(const char *str)
//{
//    while(*str) LCD_SendData(*str++);
//}
//
//void LCD_Clear(void)
//{
//    LCD_SendCommand(LCD_CMD_CLEAR);
//    LCD_DelayMs(2);
//}
//
//void LCD_SetCursor(uint8_t row, uint8_t col)
//{
//    uint8_t address = (row == 0) ? LCD_LINE1_ADDR : LCD_LINE2_ADDR;
//    address += col;
//    LCD_SendCommand(LCD_CMD_SET_CURSOR | address);
//}
//
//void LCD_PrintLine(uint8_t row, const char *str)
//{
//    LCD_SetCursor(row, 0);
//    char buffer[17] = "                ";
//    uint8_t len = strlen(str);
//    if(len > 16) len = 16;
//    memcpy(buffer, str, len);
//    for(uint8_t i = 0; i < 16; i++) LCD_SendData(buffer[i]);
//}
//
//void LCD_DisplayMenu(const char *line1, const char *line2, uint8_t cursor_on_line1)
//{
//    char display_line1[17];
//    char display_line2[17];
//
//    if(cursor_on_line1) snprintf(display_line1, 17, ">%-15s", line1);
//    else snprintf(display_line1, 17, " %-15s", line1);
//
//    if(!cursor_on_line1) snprintf(display_line2, 17, ">%-15s", line2);
//    else snprintf(display_line2, 17, " %-15s", line2);
//
//    LCD_PrintLine(0, display_line1);
//    LCD_PrintLine(1, display_line2);
//}
//
//void LCD_CreateCustomChar(uint8_t location, uint8_t pattern[8])
//{
//    location &= 0x07;
//    LCD_SendCommand(0x40 | (location << 3));
//    for(uint8_t i = 0; i < 8; i++) LCD_SendData(pattern[i]);
//}


//------------------------------------------------------------------------------------------------------------------------


/*
 * Src/lcd.c
 * Full Version - 16MHz Calibrated
 */

#include "lcd.h"
#include "stm32f446xx_gpio_driver.h"
#include <string.h>
#include <stdio.h>

extern void UART_Printf(const char *format, ...);

// --- HELPER FUNCTIONS ---

void LCD_DelayUs(uint32_t us)
{
    uint32_t count = us * 4;
    for(uint32_t i = 0; i < count; i++) { __asm("NOP"); }
}

void LCD_DelayMs(uint32_t ms)
{
    for(uint32_t i = 0; i < ms; i++) { LCD_DelayUs(1000); }
}

void LCD_EnablePulse(void)
{
    GPIO_WriteToOutputPin(LCD_CTRL_PORT, LCD_EN_PIN, GPIO_PIN_SET);
    LCD_DelayUs(20);
    GPIO_WriteToOutputPin(LCD_CTRL_PORT, LCD_EN_PIN, GPIO_PIN_RESET);
    LCD_DelayUs(50);
}

void LCD_SendNibble(uint8_t nibble)
{
	// 1. Explicitly RESET all data pins first
	    GPIO_WriteToOutputPin(LCD_DATA_PORT, LCD_D4_PIN, GPIO_PIN_RESET);
	    GPIO_WriteToOutputPin(LCD_DATA_PORT, LCD_D5_PIN, GPIO_PIN_RESET);
	    GPIO_WriteToOutputPin(LCD_DATA_PORT, LCD_D6_PIN, GPIO_PIN_RESET);
	    GPIO_WriteToOutputPin(LCD_DATA_PORT, LCD_D7_PIN, GPIO_PIN_RESET);

    GPIO_WriteToOutputPin(LCD_DATA_PORT, LCD_D4_PIN, (nibble >> 0) & 0x01);
    GPIO_WriteToOutputPin(LCD_DATA_PORT, LCD_D5_PIN, (nibble >> 1) & 0x01);
    GPIO_WriteToOutputPin(LCD_DATA_PORT, LCD_D6_PIN, (nibble >> 2) & 0x01);
    GPIO_WriteToOutputPin(LCD_DATA_PORT, LCD_D7_PIN, (nibble >> 3) & 0x01);
    LCD_EnablePulse();
}

// --- PUBLIC FUNCTIONS ---

void LCD_Init(void)
{
    UART_Printf(">> [LCD] Initializing...\r\n");
    GPIO_Handle_t lcd_pin;
    memset(&lcd_pin, 0, sizeof(lcd_pin));

    // Enable Clocks
    GPIO_PeriClockControl(LCD_CTRL_PORT, ENABLE);
    GPIO_PeriClockControl(LCD_DATA_PORT, ENABLE);

    // Common Config
    lcd_pin.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
    lcd_pin.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    lcd_pin.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    lcd_pin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

    // Init All Pins
    lcd_pin.pGPIOx = LCD_CTRL_PORT;
    lcd_pin.GPIO_PinConfig.GPIO_PinNumber = LCD_RS_PIN; GPIO_Init(&lcd_pin);
    lcd_pin.GPIO_PinConfig.GPIO_PinNumber = LCD_EN_PIN; GPIO_Init(&lcd_pin);

    lcd_pin.pGPIOx = LCD_DATA_PORT;
    lcd_pin.GPIO_PinConfig.GPIO_PinNumber = LCD_D4_PIN; GPIO_Init(&lcd_pin);
    lcd_pin.GPIO_PinConfig.GPIO_PinNumber = LCD_D5_PIN; GPIO_Init(&lcd_pin);
    lcd_pin.GPIO_PinConfig.GPIO_PinNumber = LCD_D6_PIN; GPIO_Init(&lcd_pin);
    lcd_pin.GPIO_PinConfig.GPIO_PinNumber = LCD_D7_PIN; GPIO_Init(&lcd_pin);

    // Init Sequence
    LCD_DelayMs(200);

    GPIO_WriteToOutputPin(LCD_CTRL_PORT, LCD_RS_PIN, GPIO_PIN_RESET);
    GPIO_WriteToOutputPin(LCD_CTRL_PORT, LCD_EN_PIN, GPIO_PIN_RESET);

    LCD_SendNibble(0x03); LCD_DelayMs(10);
    LCD_SendNibble(0x03); LCD_DelayMs(5);
    LCD_SendNibble(0x03); LCD_DelayMs(5);
    LCD_SendNibble(0x02); LCD_DelayMs(5);

    LCD_SendCommand(LCD_CMD_4BIT_2LINE);
    LCD_SendCommand(LCD_CMD_DISPLAY_ON);
    LCD_SendCommand(LCD_CMD_CLEAR);
    LCD_DelayMs(5);
    LCD_SendCommand(LCD_CMD_ENTRY_MODE);
}

void LCD_SendCommand(uint8_t cmd)
{
    GPIO_WriteToOutputPin(LCD_CTRL_PORT, LCD_RS_PIN, GPIO_PIN_RESET);
    LCD_SendNibble((cmd >> 4) & 0x0F);
    LCD_SendNibble(cmd & 0x0F);
    if(cmd == LCD_CMD_CLEAR || cmd == LCD_CMD_HOME) LCD_DelayMs(2);
    else LCD_DelayUs(50);
}

void LCD_SendData(uint8_t data)
{
    GPIO_WriteToOutputPin(LCD_CTRL_PORT, LCD_RS_PIN, GPIO_PIN_SET);
    LCD_SendNibble((data >> 4) & 0x0F);
    LCD_SendNibble(data & 0x0F);
    LCD_DelayUs(50);
}

void LCD_PrintString(const char *str)
{
    while(*str) LCD_SendData(*str++);
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
    char buffer[17] = "                ";
    uint8_t len = strlen(str);
    if(len > 16) len = 16;
    memcpy(buffer, str, len);
    for(uint8_t i = 0; i < 16; i++) LCD_SendData(buffer[i]);
}

void LCD_DisplayMenu(const char *line1, const char *line2, uint8_t cursor_on_line1)
{
    char display_line1[17];
    char display_line2[17];

    if(cursor_on_line1) snprintf(display_line1, 17, ">%-15s", line1);
    else snprintf(display_line1, 17, " %-15s", line1);

    if(!cursor_on_line1) snprintf(display_line2, 17, ">%-15s", line2);
    else snprintf(display_line2, 17, " %-15s", line2);

    LCD_PrintLine(0, display_line1);
    LCD_PrintLine(1, display_line2);
}

void LCD_CreateCustomChar(uint8_t location, uint8_t pattern[8])
{
    location &= 0x07;
    LCD_SendCommand(0x40 | (location << 3));
    for(uint8_t i = 0; i < 8; i++) LCD_SendData(pattern[i]);
}
