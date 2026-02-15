/*
 * bsp_button.c
 *
 * Created on: Feb 09, 2026
 * Author: Rahul Bari
 * Description: Button & IR Sensor BSP Layer with ISR support
 */

#include "bsp_button.h"
#include "bsp_delay.h"

/* ========================================================================
   PC13 BUTTON INITIALIZATION (Green LED Trigger)
   ======================================================================== */
void BSP_Button_Init(void)
{
    GPIO_Handle_t btn = {0};

    // Enable clock for Port C
    GPIO_PeriClockControl(GPIOC, ENABLE);

    // Configure PC13
    btn.pGPIOx = WAKEUP_BTN_PORT;
    btn.GPIO_PinConfig.GPIO_PinNumber = WAKEUP_BTN_PIN;
    btn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IT_FT;  // Falling edge
    btn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    btn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;  // External pull-up

    GPIO_Init(&btn);

    // Configure NVIC
    GPIO_IRQPriorityConfig(WAKEUP_BTN_IRQ, NVIC_IRQ_PRI15);
    GPIO_IRQInterruptConfig(WAKEUP_BTN_IRQ, ENABLE);
}

/* ========================================================================
   PC8 BUTTON INITIALIZATION (Red LED Trigger)
   ======================================================================== */
void PC8_Button_Init(void)
{
    GPIO_Handle_t pc8_btn = {0};

    // Enable clock for Port C
    GPIO_PeriClockControl(GPIOC, ENABLE);

    // Configure PC8 as input with falling edge trigger
    pc8_btn.pGPIOx = GPIOC;
    pc8_btn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_8;
    pc8_btn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IT_FT;  // Falling edge
    pc8_btn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    pc8_btn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;  // Internal pull-up

    GPIO_Init(&pc8_btn);

    // Configure NVIC for EXTI9_5 (handles PC5-PC9)

    GPIO_IRQPriorityConfig(EXTI9_5_IRQn, NVIC_IRQ_PRI15);
    GPIO_IRQInterruptConfig(EXTI9_5_IRQn, ENABLE);
}

/* ========================================================================
   PC6 IR SENSOR INITIALIZATION (White LED Trigger)
   ======================================================================== */
void PC6_IR_Init(void)
{
    GPIO_Handle_t pc6_ir = {0};

    // Enable clock for Port C
    GPIO_PeriClockControl(GPIOC, ENABLE);

    // Configure PC6 as input with rising edge trigger
    // IR sensor outputs HIGH when motion is detected
    pc6_ir.pGPIOx = GPIOC;
    pc6_ir.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_6;
    pc6_ir.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IT_FT;  // Falling edge
    pc6_ir.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    pc6_ir.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PD;  // Pull-down (sensor pulls HIGH)

    GPIO_Init(&pc6_ir);

    // Configure NVIC for EXTI9_5 (same line as PC8)
//
//    GPIO_IRQPriorityConfig(EXTI9_5_IRQn, NVIC_IRQ_PRI15);
//    GPIO_IRQInterruptConfig(EXTI9_5_IRQn, ENABLE);
}

/* ========================================================================
   UTILITY FUNCTIONS
   ======================================================================== */

/**
 * @brief Read PC13 button state (blocking)
 * @return BSP_BUTTON_PRESSED (0) or BSP_BUTTON_RELEASED (1)
 */
uint8_t BSP_Button_Read(void)
{
    return GPIO_ReadFromInputPin(WAKEUP_BTN_PORT, WAKEUP_BTN_PIN);
}

/**
 * @brief Get button state with proper logic
 * @return BSP_BUTTON_PRESSED or BSP_BUTTON_RELEASED
 */
uint8_t BSP_Button_GetState(void)
{
    // PC13 is active LOW (0 = pressed, 1 = released)
    if (GPIO_ReadFromInputPin(WAKEUP_BTN_PORT, WAKEUP_BTN_PIN) == GPIO_PIN_RESET) {
        return BSP_BUTTON_PRESSED;
    } else {
        return BSP_BUTTON_RELEASED;
    }
}

/**
 * @brief IRQ Handling for PC13 button
 * Call this from EXTI15_10_IRQHandler
 */
void BSP_Button_IRQHandling(void)
{
    // Clear the EXTI pending bit
    GPIO_IRQHandling(WAKEUP_BTN_PIN);

    // Call user callback
    BSP_Button_Callback();
}

/**
 * @brief Weak callback function
 * User can override this in main.c
 */
__attribute__((weak)) void BSP_Button_Callback(void)
{
    // Default implementation - do nothing
    // User overrides this in main.c
}
