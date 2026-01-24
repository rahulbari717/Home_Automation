/*
 * stm32f446xx_keypad.c
 *
 *  Created on: Jan 13, 2026
 *      Author: Rahul Bari
 */

#include "keypad.h"

#include "stm32f446xx.h"
#include "stm32f446xx_gpio_driver.h"

/* ===== Keypad Character Map ===== */
static const char KEYPAD_CHARS[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

/* ===== Row Pins Array ===== */
static const uint8_t ROW_PINS[4] = {
    KEYPAD_R0_PIN, 
    KEYPAD_R1_PIN, 
    KEYPAD_R2_PIN, 
    KEYPAD_R3_PIN
};

/* ===== Column Pins Array ===== */
static const uint8_t COL_PINS[4] = {
    KEYPAD_C0_PIN, 
    KEYPAD_C1_PIN, 
    KEYPAD_C2_PIN, 
    KEYPAD_C3_PIN
};

/* ===== Simple Delay Function ===== */
void Keypad_Delay(uint32_t delay)
{
    for(uint32_t i = 0; i < delay * 1000; i++);
}

/* ===== KEYPAD INITIALIZATION ===== */
void Keypad_Init(void)
{
    GPIO_Handle_t keypad_gpio;
    
    // Enable GPIOB Clock
    GPIOB_PCLK_EN();
    
    // Configure Rows as Output (Push-Pull, No Pull-up/down)
    keypad_gpio.pGPIOx = KEYPAD_ROW_PORT;
    keypad_gpio.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
    keypad_gpio.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    keypad_gpio.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
    keypad_gpio.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    
    for(int i = 0; i < 4; i++)
    {
        keypad_gpio.GPIO_PinConfig.GPIO_PinNumber = ROW_PINS[i];
        GPIO_Init(&keypad_gpio);
        GPIO_WriteToOutputPin(KEYPAD_ROW_PORT, ROW_PINS[i], GPIO_PIN_SET); // Set all rows HIGH initially
    }
    
    // Configure Columns as Input with Pull-up
    keypad_gpio.pGPIOx = KEYPAD_COL_PORT;
    keypad_gpio.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
    keypad_gpio.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU; // Pull-up resistor
    
    for(int i = 0; i < 4; i++)
    {
        keypad_gpio.GPIO_PinConfig.GPIO_PinNumber = COL_PINS[i];
        GPIO_Init(&keypad_gpio);
    }
}

/* ===== GET KEY PRESS ===== */
char Keypad_GetKey(void)
{
    // Scan each row
    for(uint8_t row = 0; row < 4; row++)
    {
        // Set all rows HIGH
        for(uint8_t r = 0; r < 4; r++)
        {
            GPIO_WriteToOutputPin(KEYPAD_ROW_PORT, ROW_PINS[r], GPIO_PIN_SET);
        }
        
        // Set current row LOW
        GPIO_WriteToOutputPin(KEYPAD_ROW_PORT, ROW_PINS[row], GPIO_PIN_RESET);
        
        // Small delay for signal stabilization
        Keypad_Delay(1);
        
        // Check each column
        for(uint8_t col = 0; col < 4; col++)
        {
            // If column is LOW, key is pressed
            if(GPIO_ReadFromInputPin(KEYPAD_COL_PORT, COL_PINS[col]) == GPIO_PIN_RESET)
            {
                // Debounce delay
                Keypad_Delay(20);
                
                // Verify key is still pressed
                if(GPIO_ReadFromInputPin(KEYPAD_COL_PORT, COL_PINS[col]) == GPIO_PIN_RESET)
                {
                    // Wait for key release
                    while(GPIO_ReadFromInputPin(KEYPAD_COL_PORT, COL_PINS[col]) == GPIO_PIN_RESET);
                    
                    // Small delay after release
                    Keypad_Delay(20);
                    
                    // Return the pressed key
                    return KEYPAD_CHARS[row][col];
                }
            }
        }
    }
    
    return KEYPAD_NO_KEY; // No key pressed
}
