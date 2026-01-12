/*
 * 006_i2c_master_tx.c
 *
 *  Created on: Dec 26, 2025
 *  Author: Rahul B.
 *
 *  Description: I2C Master sends "Hello World" to slave when button is pressed
 *
 *  Pin Configuration:
 *  ==================
 *  PB6 -> I2C1_SCL  (AF4)
 *  PB7 -> I2C1_SDA  (AF4)
 *  PC13 -> User Button (Active LOW)
 *
 *  I2C Settings:
 *  =============
 *  - Speed: 100 kHz (Standard Mode)
 *  - Slave Address: 0x68 (example - change as needed)
 *  - 7-bit addressing
 */

#include "stm32f446xx.h"
#include <string.h>

// Global I2C handle
I2C_Handle_t I2C1Handle;

// Slave device address (change this to match your slave device)
#define SLAVE_ADDR  0x68


uint8_t some_data[] = "We are testing I2C master TX \n";

/*********************************************************************
 * @fn              - delay
 *
 * @brief           - Simple delay for button debouncing
 *********************************************************************/
void delay(void)
{
    for(uint32_t i = 0; i < 500000/2; i++);
}

/*********************************************************************
 * @fn              - I2C1_GPIOInits
 *
 * @brief           - Initialize PB6 (SCL) and PB7 (SDA) for I2C1
 *
 * @Note            - Uses Alternate Function 4 (AF4) for I2C
 *                    Open-drain output with pull-up resistors
 *********************************************************************/
void I2C1_GPIOInits(void)
{
    GPIO_Handle_t I2CPins;
    memset(&I2CPins, 0, sizeof(I2CPins));

    I2CPins.pGPIOx = GPIOB;
    I2CPins.GPIO_PinConfig.GPIO_PinMode        = GPIO_MODE_ALTFN;
    I2CPins.GPIO_PinConfig.GPIO_PinAltFunMode  = GPIO_AF4_I2C;      // AF4 for I2C
    I2CPins.GPIO_PinConfig.GPIO_PinOPType      = GPIO_OP_TYPE_OD;   // Open Drain (required for I2C)
    I2CPins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;       // Pull-up enabled
    I2CPins.GPIO_PinConfig.GPIO_PinSpeed       = GPIO_SPEED_FAST;

    // SCL -> PB6
    I2CPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_6;
    GPIO_Init(&I2CPins);

    // SDA -> PB7
    I2CPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_7;
    GPIO_Init(&I2CPins);
}

/*********************************************************************
 * @fn              - I2C1_Inits
 *
 * @brief           - Initialize I2C1 peripheral in master mode
 *
 * @Note            - Standard mode (100 kHz)
 *                    ACK enabled
 *********************************************************************/
void I2C1_Inits(void)
{
    memset(&I2C1Handle, 0, sizeof(I2C1Handle));

    I2C1Handle.pI2Cx = I2C1;
    I2C1Handle.I2C_Config.I2C_AckControl    = I2C_ACK_ENABLE;
    I2C1Handle.I2C_Config.I2C_DeviceAddress = MY_ADDRESS;              // Own address (not important for master-only)
    I2C1Handle.I2C_Config.I2C_SCLSpeed      = I2C_SCL_SPEED_SM;  // 100 kHz (Standard Mode)
    I2C1Handle.I2C_Config.I2C_FMDutyCycle   = I2C_FM_DUTY_2;     // Not used in SM

    I2C_Init(&I2C1Handle);
}

/*********************************************************************
 * @fn              - GPIO_ButtonInit
 *
 * @brief           - Initialize user button on PC13
 *
 * @Note            - Button is active LOW (pressed = 0)
 *********************************************************************/
void GPIO_ButtonInit(void)
{
    GPIO_Handle_t GpioButton;
    memset(&GpioButton, 0, sizeof(GpioButton));

    GpioButton.pGPIOx = GPIOC;
    GpioButton.GPIO_PinConfig.GPIO_PinNumber      = GPIO_PIN_NO_13;
    GpioButton.GPIO_PinConfig.GPIO_PinMode        = GPIO_MODE_IN;
    GpioButton.GPIO_PinConfig.GPIO_PinSpeed       = GPIO_SPEED_FAST;
    GpioButton.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

    GPIO_PeriClockControl(GPIOC, ENABLE);
    GPIO_Init(&GpioButton);
}

/*********************************************************************
 * @fn              - main
 *
 * @brief           - Main application loop
 *
 * @Description     - Waits for button press, then sends "Hello World"
 *                    via I2C to slave device at address SLAVE_ADDR
 *********************************************************************/
int main(void)
{
    // 1. Initialize button
    GPIO_ButtonInit();

    // 2. Initialize GPIO pins for I2C1
    I2C1_GPIOInits();

    // 3. Initialize I2C1 peripheral
    I2C1_Inits();

    // 4. Enable I2C1 peripheral
    I2C_PeripheralControl(I2C1, ENABLE);



    // Main loop
    while(1)
    {
        // Check if button is pressed (active LOW)
        if(GPIO_ReadFromInputPin(GPIOC, GPIO_PIN_NO_13) == BTN_PRESSED)
        {
            // Debounce delay
            delay();
            I2C_MasterSendData(&I2C1Handle, (uint8_t*)some_data, strlen((char*)(some_data)), SLAVE_ADDR, I2C_ENABLE_SR);

        }
//        else
//        {
//            // Button released, clear flag
//            button_pressed_flag = 0;
//        }
    }

    return 0;
}
