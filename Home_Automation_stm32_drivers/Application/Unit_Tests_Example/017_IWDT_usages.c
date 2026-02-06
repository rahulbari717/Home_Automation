/*
 * IWDG Driver Usage Examples for STM32F446RE
 *
 * Created on: Jan 17, 2026
 * Author: Rahul B.
 */

#include "main.h"

void gpio_init(void)
{
    GPIO_Handle_t GpioLed;
    GpioLed.pGPIOx = GPIOA;
    GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_5;
    GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
    GpioLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    GpioLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

    GPIO_Init(&GpioLed);
}

/* ============================================================================
   EXAMPLE 1: Basic Watchdog with Preset Timeout (1 second)
   ============================================================================ */
void Example1_Basic_1s_Watchdog(void)
{
    TIMER_DelayInit();
    gpio_init();

    /* Initialize IWDG with 1 second timeout */
    if (IWDG_Init_1s() != IWDG_OK)
    {
        /* Error - blink rapidly */
        while(1)
        {
            GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_5);
            TIMER_DelayMs(TIM2, 100);
        }
    }

    /* Start the watchdog - CANNOT BE STOPPED! */
    IWDG_START();

    /* Main loop - must reload watchdog before 1 second expires */
    while(1)
    {
        /* Do your work here */
    	GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_5);
        TIMER_DelayMs(TIM2, 500);

        /* CRITICAL: Reload watchdog to prevent reset */
        IWDG_RELOAD();  /* or IWDG_KICK() */
    }
}

/* ============================================================================
   EXAMPLE 2: Custom Timeout Configuration (2.5 seconds)
   ============================================================================ */
void Example2_Custom_Timeout(void)
{
    TIMER_DelayInit();
    gpio_init();

    /* Configure IWDG for 2500ms (2.5 seconds) timeout */
    if (IWDG_ConfigureTimeout(2500) != IWDG_OK)
    {
        /* Configuration failed */
        while(1);
    }

    /* Start watchdog */
    IWDG_START();

    while(1)
    {
        /* Blink LED */
        GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_5);
        TIMER_DelayMs(TIM2, 1000);

        /* Feed watchdog every 1 second (well before 2.5s timeout) */
        IWDG_REFRESH();
    }
}

/* ============================================================================
   EXAMPLE 3: Manual Configuration (500ms timeout)
   ============================================================================ */
void Example3_Manual_Config(void)
{
    IWDG_Handle_t hiwdg;

    TIMER_DelayInit();
    gpio_init();

    /* Manual configuration */
    hiwdg.pIWDGx = IWDG;
    hiwdg.IWDG_Config.IWDG_Prescaler = IWDG_PRESCALER_8;  /* /8 = 4kHz */
    hiwdg.IWDG_Config.IWDG_Reload = 1999;                 /* 500ms */

    if (IWDG_Init(&hiwdg) != IWDG_OK)
    {
        while(1);
    }

    IWDG_START();

    while(1)
    {
        GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_5);
        TIMER_DelayMs(TIM2, 200);

        IWDG_RELOAD();
    }
}

/* ============================================================================
   EXAMPLE 4: Watchdog Reset Detection
   ============================================================================ */
void Example4_Reset_Detection(void)
{
    TIMER_DelayInit();
    gpio_init();

    /* Check if reset was caused by IWDG */
    if (RCC->CSR & (1 << 29))  /* IWDGRSTF flag */
    {
        /* Watchdog reset occurred! */
        /* Blink LED rapidly 10 times to indicate watchdog reset */
        for(int i = 0; i < 10; i++)
        {
            GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_5);
            TIMER_DelayMs(TIM2, 100);
        }

        /* Clear reset flags */
        RCC->CSR |= (1 << 24);  /* RMVF - Remove reset flags */
    }

    /* Initialize and start watchdog */
    IWDG_Init_1s();
    IWDG_START();

    while(1)
    {
        /* Normal operation */
        GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_5);
        TIMER_DelayMs(TIM2, 500);
        IWDG_RELOAD();
    }
}

/* ============================================================================
   EXAMPLE 5: Demonstrating Watchdog Reset (Intentional Timeout)
   ============================================================================ */
void Example5_Intentional_Timeout(void)
{
    TIMER_DelayInit();
    gpio_init();

    /* Initialize watchdog with 500ms timeout */
    IWDG_Init_500ms();
    IWDG_START();

    /* Blink LED 5 times normally */
    for(int i = 0; i < 5; i++)
    {
        GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_5);
        TIMER_DelayMs(TIM2, 200);
        IWDG_RELOAD();  /* Keep feeding watchdog */
    }

    /* Now stop feeding the watchdog - system will reset! */
    GPIO_WriteToOutputPin(GPIOA, GPIO_PIN_NO_5, SET);

    /* This delay will cause watchdog timeout and system reset */
    TIMER_DelayMs(TIM2, 1000);  /* Wait longer than 500ms timeout */

    /* This code will never execute - system resets before reaching here */
    while(1);
}

/* ============================================================================
   EXAMPLE 6: Watchdog in Task Scheduling (Simulated RTOS)
   ============================================================================ */
void Task1(void);
void Task2(void);
void Task3(void);

void Example6_Task_Scheduling(void)
{
    TIMER_DelayInit();
    gpio_init();

    /* Initialize watchdog - must be fed by all tasks combined */
    IWDG_Init_2s();
    IWDG_START();

    while(1)
    {
        Task1();
        Task2();
        Task3();

        /* Feed watchdog after all tasks complete */
        IWDG_RELOAD();
    }
}

void Task1(void)
{
    /* Simulated task work */
    GPIO_WriteToOutputPin(GPIOA, GPIO_PIN_NO_5, SET);
    TIMER_DelayMs(TIM2, 300);
}

void Task2(void)
{
    /* Simulated task work */
    GPIO_WriteToOutputPin(GPIOA, GPIO_PIN_NO_5, RESET);
    TIMER_DelayMs(TIM2, 300);
}

void Task3(void)
{
    /* Simulated task work */
    GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_5);
    TIMER_DelayMs(TIM2, 300);
}

/* ============================================================================
   EXAMPLE 7: All Preset Timeouts Demo
   ============================================================================ */
void Example7_All_Presets(void)
{
    TIMER_DelayInit();
    gpio_init();

    /* Choose one preset timeout: */

    // IWDG_Init_100ms();   /* 100ms timeout */
    // IWDG_Init_500ms();   /* 500ms timeout */
    IWDG_Init_1s();      /* 1 second timeout */
    // IWDG_Init_2s();      /* 2 seconds timeout */
    // IWDG_Init_5s();      /* 5 seconds timeout */
    // IWDG_Init_10s();     /* 10 seconds timeout */

    IWDG_START();

    while(1)
    {
        /* Adjust delay to be less than timeout */
        GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_5);
        TIMER_DelayMs(TIM2, 500);
        IWDG_KICK();
    }
}

/* ============================================================================
   EXAMPLE 8: Timeout Calculation and Display
   ============================================================================ */
void Example8_Timeout_Calculation(void)
{
    TIMER_DelayInit();
    gpio_init();

    /* Calculate timeout for different configurations */
    uint32_t timeout_ms;

    timeout_ms = IWDG_CalculateTimeout(IWDG_PRESCALER_4, 799);
    /* timeout_ms will be approximately 100ms */
    (void)timeout_ms; /* Suppress unused warning */

    timeout_ms = IWDG_CalculateTimeout(IWDG_PRESCALER_8, 3999);
    /* timeout_ms will be approximately 1000ms */
    (void)timeout_ms;

    timeout_ms = IWDG_CalculateTimeout(IWDG_PRESCALER_64, 4095);
    /* timeout_ms will be approximately 8192ms */
    (void)timeout_ms;

    /* Use calculated timeout to configure watchdog */
    IWDG_Init_1s();
    IWDG_START();

    while(1)
    {
        GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_5);
        TIMER_DelayMs(TIM2, 500);
        IWDG_Reload();
    }
}

/* ============================================================================
   MAIN FUNCTION - Select which example to run
   ============================================================================ */
int main(void)
{
    /* Uncomment the example you want to run */

    Example1_Basic_1s_Watchdog();           /* Basic usage */
    // Example2_Custom_Timeout();           /* Custom timeout */
    // Example3_Manual_Config();            /* Manual configuration */
    // Example4_Reset_Detection();          /* Detect watchdog reset */
    // Example5_Intentional_Timeout();      /* Demo watchdog reset */
    // Example6_Task_Scheduling();          /* Task-based feeding */
    // Example7_All_Presets();              /* All preset timeouts */
    // Example8_Timeout_Calculation();      /* Timeout calculations */

    return 0;
}
