/*
 * 013_RCC_complete_function.c
 *
 * Created on: Jan 17, 2026
 * Author: Rahul B.
 * Description: Complete examples using RCC_OscConfig and RCC_ClockConfig APIs
 */

#include "stm32f446xx.h"

void gpio_init(void)
{
    GPIO_Handle_t GpioLed;
    GpioLed.pGPIOx = GPIOA;
    GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_5;
    GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
    GpioLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_LOW;
    GpioLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

    GPIO_Init(&GpioLed);
}

/* ============================================================================
   EXAMPLE 1: Configure 8MHz HSE using new API
   ============================================================================ */
void Example1_HSE_8MHz(void)
{
    RCC_OscInitTypeDef RCC_OscInit = {0};
    RCC_ClkInitTypeDef RCC_ClkInit = {0};

    /* Configure HSE Oscillator */
    RCC_OscInit.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInit.HSEState = RCC_HSE_BYPASS; /* For Nucleo ST-Link */
    RCC_OscInit.PLL.PLLState = RCC_PLL_NONE;

    if (RCC_OscConfig(&RCC_OscInit) != RCC_OK)
    {
        /* Error handling */
        while(1);
    }

    /* Configure System Clocks */
    RCC_ClkInit.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
                            RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInit.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
    RCC_ClkInit.AHBCLKDivider = RCC_SYSCLK_DIV1;   /* HCLK = 8MHz */
    RCC_ClkInit.APB1CLKDivider = RCC_HCLK_DIV1;    /* PCLK1 = 8MHz */
    RCC_ClkInit.APB2CLKDivider = RCC_HCLK_DIV1;    /* PCLK2 = 8MHz */

    if (RCC_ClockConfig(&RCC_ClkInit, RCC_FLASH_LATENCY_0WS) != RCC_OK)
    {
        /* Error handling */
        while(1);
    }

    /* Initialize Timer and GPIO */
    TIMER_DelayInit();
    gpio_init();

    while(1)
    {
        GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_5);
        TIMER_DelayMs(TIM2, 1000);
    }
}

/* ============================================================================
   EXAMPLE 2: Configure 16MHz HSI using new API
   ============================================================================ */
void Example2_HSI_16MHz(void)
{
    RCC_OscInitTypeDef RCC_OscInit = {0};
    RCC_ClkInitTypeDef RCC_ClkInit = {0};

    /* Configure HSI Oscillator */
    RCC_OscInit.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInit.HSIState = RCC_HSI_ON;
    RCC_OscInit.HSICalibrationValue = 16; /* Default calibration */
    RCC_OscInit.PLL.PLLState = RCC_PLL_NONE;

    if (RCC_OscConfig(&RCC_OscInit) != RCC_OK)
    {
        while(1);
    }

    /* Configure System Clocks */
    RCC_ClkInit.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
                            RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInit.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInit.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInit.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInit.APB2CLKDivider = RCC_HCLK_DIV1;

    if (RCC_ClockConfig(&RCC_ClkInit, RCC_FLASH_LATENCY_0WS) != RCC_OK)
    {
        while(1);
    }

    TIMER_DelayInit();
    gpio_init();

    while(1)
    {
        GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_5);
        TIMER_DelayMs(TIM2, 500);
    }
}

/* ============================================================================
   EXAMPLE 3: Configure 84MHz using PLL (HSE as source)
   ============================================================================ */
void Example3_PLL_84MHz(void)
{
    RCC_OscInitTypeDef RCC_OscInit = {0};
    RCC_ClkInitTypeDef RCC_ClkInit = {0};

    /* Configure HSE and PLL */
    RCC_OscInit.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInit.HSEState = RCC_HSE_BYPASS;
    RCC_OscInit.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInit.PLL.PLLSource = RCC_PLLSOURCE_HSE;

    /* PLL Configuration: (8MHz / 8) * 168 / 2 = 84MHz */
    RCC_OscInit.PLL.PLLM = 8;   /* Division factor for input */
    RCC_OscInit.PLL.PLLN = 168; /* Multiplication factor */
    RCC_OscInit.PLL.PLLP = 2;   /* Division factor for main output */
    RCC_OscInit.PLL.PLLQ = 7;   /* Division for USB/SDIO */

    if (RCC_OscConfig(&RCC_OscInit) != RCC_OK)
    {
        while(1);
    }

    /* Configure System Clocks */
    RCC_ClkInit.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
                            RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInit.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInit.AHBCLKDivider = RCC_SYSCLK_DIV1;    /* HCLK = 84MHz */
    RCC_ClkInit.APB1CLKDivider = RCC_HCLK_DIV2;     /* PCLK1 = 42MHz */
    RCC_ClkInit.APB2CLKDivider = RCC_HCLK_DIV1;     /* PCLK2 = 84MHz */

    if (RCC_ClockConfig(&RCC_ClkInit, RCC_FLASH_LATENCY_2WS) != RCC_OK)
    {
        while(1);
    }

    TIMER_DelayInit();
    gpio_init();

    while(1)
    {
        GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_5);
        TIMER_DelayMs(TIM2, 250);
    }
}

/* ============================================================================
   EXAMPLE 4: Configure 180MHz Maximum Speed (HSE + PLL)
   ============================================================================ */
void Example4_PLL_180MHz_Max(void)
{
    RCC_OscInitTypeDef RCC_OscInit = {0};
    RCC_ClkInitTypeDef RCC_ClkInit = {0};

    /* Enable Power Control Clock and configure voltage scaling */
    RCC->APB1ENR |= (1 << 28);
    PWR->CR |= (3 << 14); /* Voltage Scale 1 for maximum frequency */

    /* Configure HSE and PLL */
    RCC_OscInit.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInit.HSEState = RCC_HSE_BYPASS;
    RCC_OscInit.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInit.PLL.PLLSource = RCC_PLLSOURCE_HSE;

    /* PLL Configuration: (8MHz / 8) * 360 / 2 = 180MHz */
    RCC_OscInit.PLL.PLLM = 8;
    RCC_OscInit.PLL.PLLN = 360;
    RCC_OscInit.PLL.PLLP = 2;
    RCC_OscInit.PLL.PLLQ = 7;

    if (RCC_OscConfig(&RCC_OscInit) != RCC_OK)
    {
        while(1);
    }

    /* Configure System Clocks */
    RCC_ClkInit.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
                            RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInit.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInit.AHBCLKDivider = RCC_SYSCLK_DIV1;    /* HCLK = 180MHz */
    RCC_ClkInit.APB1CLKDivider = RCC_HCLK_DIV4;     /* PCLK1 = 45MHz */
    RCC_ClkInit.APB2CLKDivider = RCC_HCLK_DIV2;     /* PCLK2 = 90MHz */

    if (RCC_ClockConfig(&RCC_ClkInit, RCC_FLASH_LATENCY_5WS) != RCC_OK)
    {
        while(1);
    }

    TIMER_DelayInit();
    gpio_init();

    while(1)
    {
        GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_5);
        TIMER_DelayMs(TIM2, 100);
    }
}

/* ============================================================================
   EXAMPLE 5: Using Helper Macros (Simplified Configuration)
   ============================================================================ */
void Example5_Using_Macros(void)
{
    /* Using helper macros from header file */
    RCC_OscInitTypeDef RCC_OscInit = RCC_OSC_HSE_8MHZ_BYPASS();
    RCC_ClkInitTypeDef RCC_ClkInit = RCC_CLK_CONFIG_DEFAULT();

    /* Just set the clock source */
    RCC_ClkInit.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;

    if (RCC_OscConfig(&RCC_OscInit) != RCC_OK)
    {
        while(1);
    }

    if (RCC_ClockConfig(&RCC_ClkInit, RCC_FLASH_LATENCY_0WS) != RCC_OK)
    {
        while(1);
    }

    TIMER_DelayInit();
    gpio_init();

    while(1)
    {
        GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_5);
        TIMER_DelayMs(TIM2, 1000);
    }
}

/* ============================================================================
   EXAMPLE 6: Runtime Clock Switching (HSI -> HSE)
   ============================================================================ */
void Example6_Runtime_Clock_Switch(void)
{
    RCC_OscInitTypeDef RCC_OscInit = {0};
    RCC_ClkInitTypeDef RCC_ClkInit = {0};

    /* Start with HSI (default) */
    gpio_init();

    /* Blink fast on HSI (16MHz) */
    for(int i = 0; i < 10; i++)
    {
        GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_5);
        TIMER_DelayMs(TIM2, 100);
    }

    /* Switch to HSE */
    RCC_OscInit.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInit.HSEState = RCC_HSE_BYPASS;
    RCC_OscInit.PLL.PLLState = RCC_PLL_NONE;

    RCC_OscConfig(&RCC_OscInit);

    RCC_ClkInit.ClockType = RCC_CLOCKTYPE_SYSCLK;
    RCC_ClkInit.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;

    RCC_ClockConfig(&RCC_ClkInit, RCC_FLASH_LATENCY_0WS);

    /* Re-initialize timer for new clock frequency */
    TIMER_DelayInit();

    /* Blink slower on HSE (8MHz) */
    while(1)
    {
        GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_5);
        TIMER_DelayMs(TIM2, 500);
    }
}

/* ============================================================================
   MAIN FUNCTION - Select which example to run
   ============================================================================ */
int main(void)
{
    /* Uncomment the example you want to run */

//    Example1_HSE_8MHz();           /* 8MHz HSE */
    // Example2_HSI_16MHz();       /* 16MHz HSI */
    // Example3_PLL_84MHz();       /* 84MHz PLL */
    Example4_PLL_180MHz_Max();  /* 180MHz Maximum */
    // Example5_Using_Macros();    /* Using helper macros */
    // Example6_Runtime_Clock_Switch(); /* Runtime switching */

    return 0;
}
