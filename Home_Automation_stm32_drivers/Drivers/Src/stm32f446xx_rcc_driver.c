/*
 * stm32f446xx_rcc_driver.c
 *
 *  Created on: Dec 4, 2025
 *      Author: Rahul B.
 */

#include "stm32f446xx_rcc_driver.h"

uint16_t AHB_Prescaler[] = {2,4,8,16,64,128,256,512};
uint8_t APB_Prescaler[] = {2,4,8,16};

/* Timeout value in milliseconds */
#define HSE_TIMEOUT_VALUE          100U
#define HSI_TIMEOUT_VALUE          2U
#define LSE_TIMEOUT_VALUE          5000U
#define PLL_TIMEOUT_VALUE          2U
#define CLOCKSWITCH_TIMEOUT_VALUE  5000U

/*
 * Private Helper Functions
 */

static RCC_StatusTypeDef RCC_EnableHSE(uint32_t state);
static RCC_StatusTypeDef RCC_EnableHSI(uint32_t state);
static RCC_StatusTypeDef RCC_EnableLSE(uint32_t state);
static RCC_StatusTypeDef RCC_EnableLSI(uint32_t state);
static RCC_StatusTypeDef RCC_EnablePLL(void);
static RCC_StatusTypeDef RCC_DisablePLL(void);
static void RCC_Delay(uint32_t ms);

/*********************************************************************
 * @fn              - RCC_OscConfig
 * @brief           - Configures oscillators (HSE, HSI, LSE, LSI, PLL)
 * @param[in]       - RCC_OscInitStruct: Pointer to oscillator config structure
 * @return          - RCC_OK, RCC_ERROR, or RCC_TIMEOUT
 * @note            - Main API for oscillator configuration
 ********************************************************************/
RCC_StatusTypeDef RCC_OscConfig(RCC_OscInitTypeDef *RCC_OscInitStruct)
{
    RCC_StatusTypeDef status = RCC_OK;
    uint32_t sysclk_source;

    if (RCC_OscInitStruct == NULL)
        return RCC_ERROR;

    /* Get current system clock source */
    sysclk_source = (RCC->CFGR >> 2) & 0x3;

    /*------------------------------- HSE Configuration ------------------------*/
    if (((RCC_OscInitStruct->OscillatorType) & RCC_OSCILLATORTYPE_HSE) == RCC_OSCILLATORTYPE_HSE)
    {
        /* Check if HSE is being used as system clock */
        if (sysclk_source == 1) /* HSE is SYSCLK */
        {
            if (RCC_OscInitStruct->HSEState == RCC_HSE_OFF)
                return RCC_ERROR; /* Cannot turn off HSE while it's the system clock */
        }
        else
        {
            /* Configure HSE */
            status = RCC_EnableHSE(RCC_OscInitStruct->HSEState);
            if (status != RCC_OK)
                return status;
        }
    }

    /*------------------------------- HSI Configuration ------------------------*/
    if (((RCC_OscInitStruct->OscillatorType) & RCC_OSCILLATORTYPE_HSI) == RCC_OSCILLATORTYPE_HSI)
    {
        /* Check if HSI is being used as system clock or PLL source */
        if (sysclk_source == 0) /* HSI is SYSCLK */
        {
            if (RCC_OscInitStruct->HSIState == RCC_HSI_OFF)
                return RCC_ERROR;

            /* Adjust HSI calibration */
            RCC->CR &= ~(0x1F << 3); /* Clear HSITRIM */
            RCC->CR |= ((RCC_OscInitStruct->HSICalibrationValue) << 3);
        }
        else
        {
            /* Configure HSI */
            status = RCC_EnableHSI(RCC_OscInitStruct->HSIState);
            if (status != RCC_OK)
                return status;

            /* Set calibration value */
            if (RCC_OscInitStruct->HSIState == RCC_HSI_ON)
            {
                RCC->CR &= ~(0x1F << 3);
                RCC->CR |= ((RCC_OscInitStruct->HSICalibrationValue) << 3);
            }
        }
    }

    /*------------------------------- LSE Configuration ------------------------*/
    if (((RCC_OscInitStruct->OscillatorType) & RCC_OSCILLATORTYPE_LSE) == RCC_OSCILLATORTYPE_LSE)
    {
        /* Enable write access to Backup domain */
        RCC->APB1ENR |= (1 << 28); /* Enable PWR clock */
        PWR->CR |= (1 << 8);       /* Enable backup domain access */

        status = RCC_EnableLSE(RCC_OscInitStruct->LSEState);
        if (status != RCC_OK)
            return status;
    }

    /*------------------------------- LSI Configuration ------------------------*/
    if (((RCC_OscInitStruct->OscillatorType) & RCC_OSCILLATORTYPE_LSI) == RCC_OSCILLATORTYPE_LSI)
    {
        status = RCC_EnableLSI(RCC_OscInitStruct->LSIState);
        if (status != RCC_OK)
            return status;
    }

    /*------------------------------- PLL Configuration ------------------------*/
    if (RCC_OscInitStruct->PLL.PLLState != RCC_PLL_NONE)
    {
        /* Check if PLL is being used as system clock */
        if (sysclk_source == 2) /* PLL is SYSCLK */
        {
            if (RCC_OscInitStruct->PLL.PLLState == RCC_PLL_OFF)
                return RCC_ERROR; /* Cannot turn off PLL while it's the system clock */
        }
        else
        {
            if (RCC_OscInitStruct->PLL.PLLState == RCC_PLL_OFF)
            {
                status = RCC_DisablePLL();
                if (status != RCC_OK)
                    return status;
            }
            else if (RCC_OscInitStruct->PLL.PLLState == RCC_PLL_ON)
            {
                /* Disable PLL before reconfiguring */
                status = RCC_DisablePLL();
                if (status != RCC_OK)
                    return status;

                /* Configure PLL */
                RCC->PLLCFGR = (RCC_OscInitStruct->PLL.PLLM << 0)  |
                               (RCC_OscInitStruct->PLL.PLLN << 6)  |
                               (((RCC_OscInitStruct->PLL.PLLP >> 1) - 1) << 16) |
                               (RCC_OscInitStruct->PLL.PLLSource)  |
                               (RCC_OscInitStruct->PLL.PLLQ << 24) |
                               (RCC_OscInitStruct->PLL.PLLR << 28);

                /* Enable PLL */
                status = RCC_EnablePLL();
                if (status != RCC_OK)
                    return status;
            }
        }
    }

    return RCC_OK;
}

/*********************************************************************
 * @fn              - RCC_ClockConfig
 * @brief           - Configures system clocks (SYSCLK, HCLK, PCLK1, PCLK2)
 * @param[in]       - RCC_ClkInitStruct: Pointer to clock config structure
 * @param[in]       - FLatency: Flash latency value
 * @return          - RCC_OK, RCC_ERROR, or RCC_TIMEOUT
 * @note            - Main API for clock configuration
 ********************************************************************/
RCC_StatusTypeDef RCC_ClockConfig(RCC_ClkInitTypeDef *RCC_ClkInitStruct, uint32_t FLatency)
{
    uint32_t tickstart = 0;

    if (RCC_ClkInitStruct == NULL)
        return RCC_ERROR;

    /* Set Flash Latency based on voltage range and frequency */
    FLASH->ACR &= ~(0xF << 0); /* Clear latency bits */
    FLASH->ACR |= FLatency;

    /* Enable Flash prefetch, instruction cache, and data cache */
    FLASH->ACR |= (1 << 8) | (1 << 9) | (1 << 10);

    /* Verify Flash latency was set correctly */
    if ((FLASH->ACR & 0xF) != FLatency)
        return RCC_ERROR;

    /*-------------------------- HCLK Configuration --------------------------*/
    if (((RCC_ClkInitStruct->ClockType) & RCC_CLOCKTYPE_HCLK) == RCC_CLOCKTYPE_HCLK)
    {
        RCC->CFGR &= ~(0xF << 4); /* Clear HPRE bits */
        RCC->CFGR |= RCC_ClkInitStruct->AHBCLKDivider;
    }

    /*-------------------------- SYSCLK Configuration ------------------------*/
    if (((RCC_ClkInitStruct->ClockType) & RCC_CLOCKTYPE_SYSCLK) == RCC_CLOCKTYPE_SYSCLK)
    {
        /* Check PLL is ready if using it as SYSCLK source */
        if (RCC_ClkInitStruct->SYSCLKSource == RCC_SYSCLKSOURCE_PLLCLK)
        {
            if ((RCC->CR & (1 << 25)) == 0) /* Check PLLRDY */
                return RCC_ERROR;
        }
        /* Check HSE is ready if using it as SYSCLK source */
        else if (RCC_ClkInitStruct->SYSCLKSource == RCC_SYSCLKSOURCE_HSE)
        {
            if ((RCC->CR & (1 << 17)) == 0) /* Check HSERDY */
                return RCC_ERROR;
        }
        /* Check HSI is ready if using it as SYSCLK source */
        else if (RCC_ClkInitStruct->SYSCLKSource == RCC_SYSCLKSOURCE_HSI)
        {
            if ((RCC->CR & (1 << 1)) == 0) /* Check HSIRDY */
                return RCC_ERROR;
        }

        /* Set System Clock Switch */
        RCC->CFGR &= ~(0x3 << 0); /* Clear SW bits */
        RCC->CFGR |= RCC_ClkInitStruct->SYSCLKSource;

        /* Wait for system clock switch */
        tickstart = 0;
        while (((RCC->CFGR >> 2) & 0x3) != RCC_ClkInitStruct->SYSCLKSource)
        {
            if (tickstart++ > CLOCKSWITCH_TIMEOUT_VALUE)
                return RCC_TIMEOUT;
            RCC_Delay(1);
        }
    }

    /*-------------------------- PCLK1 Configuration -------------------------*/
    if (((RCC_ClkInitStruct->ClockType) & RCC_CLOCKTYPE_PCLK1) == RCC_CLOCKTYPE_PCLK1)
    {
        RCC->CFGR &= ~(0x7 << 10); /* Clear PPRE1 bits */
        RCC->CFGR |= RCC_ClkInitStruct->APB1CLKDivider;
    }

    /*-------------------------- PCLK2 Configuration -------------------------*/
    if (((RCC_ClkInitStruct->ClockType) & RCC_CLOCKTYPE_PCLK2) == RCC_CLOCKTYPE_PCLK2)
    {
        RCC->CFGR &= ~(0x7 << 13); /* Clear PPRE2 bits */
        RCC->CFGR |= ((RCC_ClkInitStruct->APB2CLKDivider) << 3);
    }

    return RCC_OK;
}

/*********************************************************************
 * @fn              - RCC_EnableHSE
 * @brief           - Enables or disables HSE oscillator
 ********************************************************************/
static RCC_StatusTypeDef RCC_EnableHSE(uint32_t state)
{
    uint32_t tickstart = 0;

    if (state == RCC_HSE_ON || state == RCC_HSE_BYPASS)
    {
        if (state == RCC_HSE_BYPASS)
            RCC->CR |= (1 << 18); /* HSEBYP */
        else
            RCC->CR &= ~(1 << 18);

        RCC->CR |= (1 << 16); /* HSEON */

        /* Wait for HSE ready */
        while (!(RCC->CR & (1 << 17)))
        {
            if (tickstart++ > HSE_TIMEOUT_VALUE)
                return RCC_TIMEOUT;
            RCC_Delay(1);
        }
    }
    else if (state == RCC_HSE_OFF)
    {
        RCC->CR &= ~(1 << 16); /* Clear HSEON */
        RCC->CR &= ~(1 << 18); /* Clear HSEBYP */

        /* Wait for HSE to stop */
        while ((RCC->CR & (1 << 17)))
        {
            if (tickstart++ > HSE_TIMEOUT_VALUE)
                return RCC_TIMEOUT;
            RCC_Delay(1);
        }
    }

    return RCC_OK;
}

/*********************************************************************
 * @fn              - RCC_EnableHSI
 * @brief           - Enables or disables HSI oscillator
 ********************************************************************/
static RCC_StatusTypeDef RCC_EnableHSI(uint32_t state)
{
    uint32_t tickstart = 0;

    if (state == RCC_HSI_ON)
    {
        RCC->CR |= (1 << 0); /* HSION */

        /* Wait for HSI ready */
        while (!(RCC->CR & (1 << 1)))
        {
            if (tickstart++ > HSI_TIMEOUT_VALUE)
                return RCC_TIMEOUT;
            RCC_Delay(1);
        }
    }
    else if (state == RCC_HSI_OFF)
    {
        RCC->CR &= ~(1 << 0); /* Clear HSION */

        /* Wait for HSI to stop */
        while ((RCC->CR & (1 << 1)))
        {
            if (tickstart++ > HSI_TIMEOUT_VALUE)
                return RCC_TIMEOUT;
            RCC_Delay(1);
        }
    }

    return RCC_OK;
}

/*********************************************************************
 * @fn              - RCC_EnableLSE
 * @brief           - Enables or disables LSE oscillator
 ********************************************************************/
static RCC_StatusTypeDef RCC_EnableLSE(uint32_t state)
{
    uint32_t tickstart = 0;

    if (state == RCC_LSE_ON || state == RCC_LSE_BYPASS)
    {
        if (state == RCC_LSE_BYPASS)
            RCC->BDCR |= (1 << 2); /* LSEBYP */
        else
            RCC->BDCR &= ~(1 << 2);

        RCC->BDCR |= (1 << 0); /* LSEON */

        /* Wait for LSE ready */
        while (!(RCC->BDCR & (1 << 1)))
        {
            if (tickstart++ > LSE_TIMEOUT_VALUE)
                return RCC_TIMEOUT;
            RCC_Delay(1);
        }
    }
    else if (state == RCC_LSE_OFF)
    {
        RCC->BDCR &= ~(1 << 0); /* Clear LSEON */
        RCC->BDCR &= ~(1 << 2); /* Clear LSEBYP */
    }

    return RCC_OK;
}

/*********************************************************************
 * @fn              - RCC_EnableLSI
 * @brief           - Enables or disables LSI oscillator
 ********************************************************************/
static RCC_StatusTypeDef RCC_EnableLSI(uint32_t state)
{
    uint32_t tickstart = 0;

    if (state == RCC_LSI_ON)
    {
        RCC->CSR |= (1 << 0); /* LSION */

        /* Wait for LSI ready */
        while (!(RCC->CSR & (1 << 1)))
        {
            if (tickstart++ > HSI_TIMEOUT_VALUE)
                return RCC_TIMEOUT;
            RCC_Delay(1);
        }
    }
    else if (state == RCC_LSI_OFF)
    {
        RCC->CSR &= ~(1 << 0); /* Clear LSION */
    }

    return RCC_OK;
}

/*********************************************************************
 * @fn              - RCC_EnablePLL
 * @brief           - Enables PLL
 ********************************************************************/
static RCC_StatusTypeDef RCC_EnablePLL(void)
{
    uint32_t tickstart = 0;

    RCC->CR |= (1 << 24); /* PLLON */

    /* Wait for PLL ready */
    while (!(RCC->CR & (1 << 25)))
    {
        if (tickstart++ > PLL_TIMEOUT_VALUE)
            return RCC_TIMEOUT;
        RCC_Delay(1);
    }

    return RCC_OK;
}

/*********************************************************************
 * @fn              - RCC_DisablePLL
 * @brief           - Disables PLL
 ********************************************************************/
static RCC_StatusTypeDef RCC_DisablePLL(void)
{
    uint32_t tickstart = 0;

    RCC->CR &= ~(1 << 24); /* Clear PLLON */

    /* Wait for PLL to stop */
    while ((RCC->CR & (1 << 25)))
    {
        if (tickstart++ > PLL_TIMEOUT_VALUE)
            return RCC_TIMEOUT;
        RCC_Delay(1);
    }

    return RCC_OK;
}

/*********************************************************************
 * @fn              - RCC_Delay
 * @brief           - Simple delay function (approximate)
 ********************************************************************/
static void RCC_Delay(uint32_t ms)
{
    volatile uint32_t count = ms * 4000; /* Approximate delay */
    while(count--);
}

/*********************************************************************
 * @fn              - RCC_GetHCLKFreq
 * @brief           - Returns AHB clock (HCLK) frequency
 ********************************************************************/
uint32_t RCC_GetHCLKFreq(void)
{
    uint32_t systemClk, hclk;
    uint32_t clksrc, temp, ahbp;

    clksrc = (RCC->CFGR >> 2) & 0x3;

    if(clksrc == 0)
        systemClk = HSI_CLK_VALUE;
    else if(clksrc == 1)
        systemClk = HSE_CLK_VALUE;
    else if(clksrc == 2)
        systemClk = RCC_GetPLLOutputClock();

    temp = (RCC->CFGR >> 4) & 0xF;
    if(temp < 8)
        ahbp = 1;
    else
        ahbp = AHB_Prescaler[temp - 8];

    hclk = systemClk / ahbp;

    return hclk;
}

/*********************************************************************
 * Legacy Functions (Preserved for backward compatibility)
 ********************************************************************/

void RCC_SetSystemClock_HSE(uint32_t targetFreq, uint32_t mode)
{
    RCC_OscInitTypeDef osc = {0};
    RCC_ClkInitTypeDef clk = {0};

    osc.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    osc.HSEState = mode;
    osc.PLL.PLLState = RCC_PLL_NONE;

    RCC_OscConfig(&osc);

    clk.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    clk.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
    clk.AHBCLKDivider = RCC_SYSCLK_DIV1;
    clk.APB1CLKDivider = RCC_HCLK_DIV1;
    clk.APB2CLKDivider = RCC_HCLK_DIV1;

    RCC_ClockConfig(&clk, RCC_FLASH_LATENCY_0WS);
}

void RCC_SetSystemClock_HSI(void)
{
    RCC_OscInitTypeDef osc = {0};
    RCC_ClkInitTypeDef clk = {0};

    osc.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    osc.HSIState = RCC_HSI_ON;
    osc.HSICalibrationValue = 16;
    osc.PLL.PLLState = RCC_PLL_NONE;

    RCC_OscConfig(&osc);

    clk.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    clk.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    clk.AHBCLKDivider = RCC_SYSCLK_DIV1;
    clk.APB1CLKDivider = RCC_HCLK_DIV1;
    clk.APB2CLKDivider = RCC_HCLK_DIV1;

    RCC_ClockConfig(&clk, RCC_FLASH_LATENCY_0WS);
}

void SystemClock_Config_HSE_180MHz(void)
{
    RCC_OscInitTypeDef osc = {0};
    RCC_ClkInitTypeDef clk = {0};

    /* Enable Power Control Clock */
    RCC->APB1ENR |= (1 << 28);
    PWR->CR |= (3 << 14); /* Voltage scaling */

    /* Configure HSE and PLL */
    osc.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    osc.HSEState = RCC_HSE_BYPASS;
    osc.PLL.PLLState = RCC_PLL_ON;
    osc.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    osc.PLL.PLLM = 8;
    osc.PLL.PLLN = 360;
    osc.PLL.PLLP = 2;
    osc.PLL.PLLQ = 7;

    RCC_OscConfig(&osc);

    /* Configure clocks */
    clk.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    clk.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    clk.AHBCLKDivider = RCC_SYSCLK_DIV1;
    clk.APB1CLKDivider = RCC_HCLK_DIV4;
    clk.APB2CLKDivider = RCC_HCLK_DIV2;

    RCC_ClockConfig(&clk, RCC_FLASH_LATENCY_5WS);
}

/* Get Clock Value Functions */
uint32_t RCC_GetPCLK1Value(void)
{
    uint32_t systemClk = 0, pclk1;
    uint32_t clksrc, temp, ahbp, apb1p;

    clksrc = (RCC->CFGR >> 2) & 0x3;

    if(clksrc == 0)
        systemClk = HSI_CLK_VALUE;
    else if(clksrc == 1)
        systemClk = HSE_CLK_VALUE;
    else if(clksrc == 2)
        systemClk = RCC_GetPLLOutputClock();

    temp = (RCC->CFGR >> 4) & 0xF;
    if(temp < 8) ahbp = 1;
    else ahbp = AHB_Prescaler[temp - 8];

    temp = (RCC->CFGR >> 10) & 0x7;
    if(temp < 4) apb1p = 1;
    else apb1p = APB_Prescaler[temp - 4];

    pclk1 = (systemClk / ahbp) / apb1p;

    return pclk1;
}

uint32_t RCC_GetPCLK2Value(void)
{
    uint32_t systemClk = 0, pclk2;
    uint32_t clksrc, temp, ahbp, apb2p;

    clksrc = (RCC->CFGR >> 2) & 0x3;

    if(clksrc == 0)
        systemClk = HSI_CLK_VALUE;
    else if(clksrc == 1)
        systemClk = HSE_CLK_VALUE;
    else if(clksrc == 2)
        systemClk = RCC_GetPLLOutputClock();

    temp = (RCC->CFGR >> 4) & 0xF;
    if(temp < 8)
        ahbp = 1;
    else
        ahbp = AHB_Prescaler[temp - 8];

    temp = (RCC->CFGR >> 13) & 0x7;
    if(temp < 4)
        apb2p = 1;
    else
        apb2p = APB_Prescaler[temp - 4];

    pclk2 = (systemClk / ahbp) / apb2p;

    return pclk2;
}

uint32_t RCC_GetPLLOutputClock(void)
{
    uint32_t pllclk, pll_m, pll_n, pll_p;
    uint32_t temp;

    temp = RCC->PLLCFGR;

    pll_m = temp & 0x3F;
    pll_n = (temp >> 6) & 0x1FF;
    pll_p = ((temp >> 16) & 0x3) + 1;
    pll_p = pll_p * 2;

    uint32_t clk_src = (temp >> 22) & 0x1;

    if(clk_src == 0)
        pllclk = HSI_CLK_VALUE;
    else
        pllclk = HSE_CLK_VALUE;

    return (pllclk / pll_m) * pll_n / pll_p;
}

uint32_t RCC_GetSystemClock(void)
{
    uint32_t systemClk = 0;
    uint8_t clksrc = (RCC->CFGR >> 2) & 0x3;

    if(clksrc == 0)
        systemClk = HSI_CLK_VALUE;
    else if(clksrc == 1)
        systemClk = HSE_CLK_VALUE;
    else if(clksrc == 2)
        systemClk = RCC_GetPLLOutputClock();

    return systemClk;
}
