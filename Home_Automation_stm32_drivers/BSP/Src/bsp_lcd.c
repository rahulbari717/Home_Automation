/*
 * bsp_lcd.c
 *
 * Description: Implementation of 16x2 LCD Driver (4-bit)
 */

#include "bsp_lcd.h"
#include "stm32f446xx_timer_driver.h" // For delays

// Private Helper: Pulse the Enable Pin
static void LCD_EnablePulse(void)
{
    GPIO_WriteToOutputPin(LCD_CTRL_PORT, LCD_EN_PIN, 1);
    // Tiny delay (~10us) needed for LCD to latch data
    for(volatile int i=0; i<500; i++);
    GPIO_WriteToOutputPin(LCD_CTRL_PORT, LCD_EN_PIN, 0);
}

// Private Helper: Send 4 bits to D4-D7
static void LCD_Write4Bits(uint8_t nibble)
{
    GPIO_WriteToOutputPin(LCD_DATA_PORT, LCD_D4_PIN, (nibble >> 0) & 0x1);
    GPIO_WriteToOutputPin(LCD_DATA_PORT, LCD_D5_PIN, (nibble >> 1) & 0x1);
    GPIO_WriteToOutputPin(LCD_DATA_PORT, LCD_D6_PIN, (nibble >> 2) & 0x1);
    GPIO_WriteToOutputPin(LCD_DATA_PORT, LCD_D7_PIN, (nibble >> 3) & 0x1);

    LCD_EnablePulse();
}

// Private Helper: Send byte (Split into two 4-bit nibbles)
// is_data = 1 for Character, 0 for Command
static void LCD_Send(uint8_t value, uint8_t is_data)
{
    // 1. Set RS (0 = Command, 1 = Data)
    GPIO_WriteToOutputPin(LCD_CTRL_PORT, LCD_RS_PIN, is_data);

    // 2. Send High Nibble
    LCD_Write4Bits(value >> 4);

    // 3. Send Low Nibble
    LCD_Write4Bits(value & 0x0F);
}

void BSP_LCD_Init(void)
{
    // 1. Init GPIO Pins
    GPIO_Handle_t lcd_pin;
    lcd_pin.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
    lcd_pin.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    lcd_pin.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    lcd_pin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

    // Control Pins (PC0, PC1)
    lcd_pin.pGPIOx = LCD_CTRL_PORT;
    lcd_pin.GPIO_PinConfig.GPIO_PinNumber = LCD_RS_PIN; GPIO_Init(&lcd_pin);
    lcd_pin.GPIO_PinConfig.GPIO_PinNumber = LCD_EN_PIN; GPIO_Init(&lcd_pin);

    // Data Pins (PC2-PC5)
    lcd_pin.pGPIOx = LCD_DATA_PORT;
    lcd_pin.GPIO_PinConfig.GPIO_PinNumber = LCD_D4_PIN; GPIO_Init(&lcd_pin);
    lcd_pin.GPIO_PinConfig.GPIO_PinNumber = LCD_D5_PIN; GPIO_Init(&lcd_pin);
    lcd_pin.GPIO_PinConfig.GPIO_PinNumber = LCD_D6_PIN; GPIO_Init(&lcd_pin);
    lcd_pin.GPIO_PinConfig.GPIO_PinNumber = LCD_D7_PIN; GPIO_Init(&lcd_pin);

    // 2. LCD Startup Sequence (Magic Magic)
    // Wait >15ms after power up
    for(volatile int i=0; i<50000; i++);

    // Initialize in 4-bit mode strictly according to datasheet
    GPIO_WriteToOutputPin(LCD_CTRL_PORT, LCD_RS_PIN, 0);

    LCD_Write4Bits(0x03); // Wake up 1
    for(volatile int i=0; i<10000; i++);
    LCD_Write4Bits(0x03); // Wake up 2
    for(volatile int i=0; i<5000; i++);
    LCD_Write4Bits(0x03); // Wake up 3
    LCD_Write4Bits(0x02); // Set to 4-bit mode

    // 3. Configure Settings
    BSP_LCD_SendCommand(LCD_CMD_FUNCTION_SET); // 4-bit, 2 lines
    BSP_LCD_SendCommand(LCD_CMD_DISPLAY_ON);   // Display ON, Cursor OFF
    BSP_LCD_SendCommand(LCD_CMD_CLEAR);        // Clear Screen
    BSP_LCD_SendCommand(LCD_CMD_ENTRY_MODE);   // Auto-increment cursor
}

void BSP_LCD_SendCommand(uint8_t cmd) {
    LCD_Send(cmd, 0);
    // Clear command needs more time
    if(cmd == LCD_CMD_CLEAR) for(volatile int i=0; i<50000; i++);
}

void BSP_LCD_SendData(uint8_t data) {
    LCD_Send(data, 1);
}

void BSP_LCD_PrintString(char *message) {
    while (*message) {
        BSP_LCD_SendData((uint8_t)*message++);
    }
}

void BSP_LCD_SetCursor(uint8_t row, uint8_t col) {
    // 0x80 is the base command to set address
    // Row 0 starts at 0x00, Row 1 starts at 0x40
    uint8_t address = (row == 0) ? 0x00 : 0x40;
    address += col;
    BSP_LCD_SendCommand(0x80 | address);
}
