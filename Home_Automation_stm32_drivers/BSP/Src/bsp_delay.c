#include "bsp_delay.h"
#include "stm32f446xx_rcc_driver.h"

/* ===== RCC Configuration (Simplified - Using your actual driver) ===== */
void RCC_Config_MaxSpeed(void) {
    // Using your existing RCC driver types
    RCC_OscInitTypeDef osc_config;
    RCC_ClkInitTypeDef clk_config;

    // Configure HSE + PLL for maximum speed
    osc_config.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    osc_config.HSEState = RCC_HSE_ON;
    osc_config.PLL.PLLState = RCC_PLL_ON;
    osc_config.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    osc_config.PLL.PLLM = 4;
    osc_config.PLL.PLLN = 180;
    osc_config.PLL.PLLP = 2;
    osc_config.PLL.PLLQ = 7;

    RCC_OscConfig(&osc_config);

    // Configure system clocks
    clk_config.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
                            RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    clk_config.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    clk_config.AHBCLKDivider = RCC_SYSCLK_DIV1;
    clk_config.APB1CLKDivider = RCC_HCLK_DIV4;
    clk_config.APB2CLKDivider = RCC_HCLK_DIV2;

    RCC_ClockConfig(&clk_config, RCC_FLASH_LATENCY_5WS);
}

/**
 * @brief Initialize the underlying hardware timer (TIM2)
 */
void BSP_Delay_Init(void)
{
    // Calls your driver function to set up TIM2 for 1us ticks
    TIMER_DelayInit();
    RCC_Config_MaxSpeed();
}

/**
 * @brief Provides a blocking delay for a specific number of milliseconds
 */
void BSP_Delay_ms(uint32_t ms)
{
    // Uses your driver's blocking delay function
    TIMER_DelayMs(TIM2, ms);
}

/**
 * @brief 100 Millisecond Delay
 */
void BSP_Delay_100ms(void)
{
    BSP_Delay_ms(100);
}

/**
 * @brief 500 Millisecond Delay
 */
void BSP_Delay_500ms(void)
{
    BSP_Delay_ms(500);
}

/**
 * @brief 1 Second Delay
 */
void BSP_Delay_1s(void)
{
    BSP_Delay_ms(1000);
}

/**
 * @brief 3 Second Delay
 */
void BSP_Delay_3s(void)
{
    BSP_Delay_ms(3000);
}

volatile uint8_t g_BSP_DelayComplete = 1; // Start in idle state

/**
 * @brief Sets up the system at 180MHz and prepares TIM7 for interrupts.
 */
void BSP_Delay_Init_IT(void) {
    // 1. Set System to Max Speed (180MHz) using your RCC driver
    SystemClock_Config_HSE_180MHz();

    // 2. Enable Peripheral Clock for TIM7
    TIMER_PeriClockControl(TIM7, ENABLE);

    // 3. Configure NVIC for TIM7 Interrupt
    TIMER_IRQPriorityConfig(TIM7_IRQn, NVIC_IRQ_PRI0); // Highest priority for accuracy
    TIMER_IRQInterruptConfig(TIM7_IRQn, ENABLE);
}

/**
 * @brief Private helper to configure TIM7 for a specific count
 */
static void Start_Timer_Internal(uint32_t ticks, uint16_t psc) {
    g_BSP_DelayComplete = 0;

    TIM7->CR1 &= ~(1 << 0); // Stop if running
    TIM7->PSC = psc;
    TIM7->ARR = ticks - 1;

    TIM7->DIER |= (1 << 0); // Enable Update Interrupt
    TIM7->EGR  |= (1 << 0); // Force update to load PSC/ARR
    TIM7->SR   &= ~(1 << 0); // Clear flag
    TIM7->CR1  |= (1 << 0); // Start
}

void BSP_DelayUs_IT(uint32_t us) {
    // At 180MHz, APB1 Timer clock is 90MHz.
    // We set PSC to 89 so 1 tick = 1 microsecond.
    Start_Timer_Internal(us, 89);
}

void BSP_DelayMs_IT(uint32_t ms) {
    // 1 tick = 0.1ms (PSC = 8999).
    // Formula: (90,000,000 / 10,000) - 1 = 8999
    Start_Timer_Internal(ms * 10, 8999);
}

void BSP_DelaySec_IT(uint32_t sec) {
    // 1 tick = 1ms (PSC = 44999, using a slower base to fit 16-bit ARR)
    // Formula: (90,000,000 / 2000) - 1 = 44999. Ticks = 2000 per second.
    Start_Timer_Internal(sec * 2000, 44999);
}

uint8_t BSP_Delay_IsComplete(void) {
    return g_BSP_DelayComplete;
}

/**
 * @brief TIM7 ISR - Place this in your it.c file or keep here
 */
void TIM7_IRQHandler(void) {
    if (TIM7->SR & (1 << 0)) {
        TIM7->SR &= ~(1 << 0);    // Clear flag
        TIM7->CR1 &= ~(1 << 0);   // Stop timer
        g_BSP_DelayComplete = 1;  // Signal completion
    }
}

