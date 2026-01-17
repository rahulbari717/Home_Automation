/*
 * stm32f446xx_iwdg_driver.c
 *
 *  Created on: Jan 17, 2026
 *      Author: Rahul B.
 */

#include "stm32f446xx_iwdg_driver.h"

/* Timeout value for waiting IWDG update (in loops) */
#define IWDG_UPDATE_TIMEOUT     50000

/* Prescaler division factors */
static const uint16_t IWDG_Prescaler_Table[7] = {4, 8, 16, 32, 64, 128, 256};

/*********************************************************************
 * @fn              - IWDG_Init
 * @brief           - Initializes the IWDG with specified configuration
 * @param[in]       - pIWDGHandle: Pointer to IWDG handle structure
 * @return          - IWDG_OK, IWDG_ERROR, or IWDG_TIMEOUT
 * @note            - Does NOT start the watchdog - call IWDG_Start() separately
 *                  - Once started, IWDG cannot be stopped except by reset
 ********************************************************************/
IWDG_StatusTypeDef IWDG_Init(IWDG_Handle_t *pIWDGHandle)
{
    if (pIWDGHandle == NULL)
        return IWDG_ERROR;

    /* Validate prescaler value */
    if (pIWDGHandle->IWDG_Config.IWDG_Prescaler > IWDG_PRESCALER_256)
        return IWDG_ERROR;

    /* Validate reload value (12-bit max) */
    if (pIWDGHandle->IWDG_Config.IWDG_Reload > IWDG_RELOAD_MAX)
        return IWDG_ERROR;

    /* Enable write access to IWDG_PR and IWDG_RLR registers */
    pIWDGHandle->pIWDGx->KR = IWDG_KEY_WRITE_ACCESS;

    /* Configure prescaler */
    pIWDGHandle->pIWDGx->PR = pIWDGHandle->IWDG_Config.IWDG_Prescaler;

    /* Configure reload value */
    pIWDGHandle->pIWDGx->RLR = pIWDGHandle->IWDG_Config.IWDG_Reload;

    /* Wait for registers to be updated */
    if (IWDG_WaitForUpdate() != IWDG_OK)
        return IWDG_TIMEOUT;

    return IWDG_OK;
}

/*********************************************************************
 * @fn              - IWDG_Start
 * @brief           - Starts the Independent Watchdog
 * @return          - None
 * @note            - WARNING: Once started, IWDG CANNOT be stopped!
 *                  - Only a system reset can stop it
 *                  - Must call IWDG_Reload() periodically to prevent reset
 ********************************************************************/
void IWDG_Start(void)
{
    /* Enable IWDG - This starts the countdown! */
    IWDG->KR = IWDG_KEY_ENABLE;
}

/*********************************************************************
 * @fn              - IWDG_Reload
 * @brief           - Reloads the IWDG counter (feeds the watchdog)
 * @return          - None
 * @note            - This function MUST be called before timeout
 *                  - Also known as "kicking" or "feeding" the watchdog
 ********************************************************************/
void IWDG_Reload(void)
{
    /* Reload IWDG counter with RLR value */
    IWDG->KR = IWDG_KEY_RELOAD;
}

/*********************************************************************
 * @fn              - IWDG_CalculateTimeout
 * @brief           - Calculates timeout period in milliseconds
 * @param[in]       - Prescaler: Prescaler value (0-6)
 * @param[in]       - Reload: Reload value (0-4095)
 * @return          - Timeout in milliseconds
 * @note            - Formula: Timeout = (Prescaler_Div * Reload) / LSI_Freq
 *                  - Uses typical LSI frequency of 32kHz
 ********************************************************************/
uint32_t IWDG_CalculateTimeout(uint8_t Prescaler, uint16_t Reload)
{
    uint32_t timeout_ms;
    uint16_t prescaler_div;

    if (Prescaler > IWDG_PRESCALER_256)
        return 0;

    prescaler_div = IWDG_Prescaler_Table[Prescaler];

    /* Calculate timeout in milliseconds
     * Timeout = (prescaler_div * reload) / LSI_Freq
     * Timeout_ms = (prescaler_div * reload * 1000) / LSI_Freq
     */
    timeout_ms = ((uint32_t)prescaler_div * (uint32_t)Reload * 1000) / LSI_FREQ_TYP;

    return timeout_ms;
}

/*********************************************************************
 * @fn              - IWDG_ConfigureTimeout
 * @brief           - Configures IWDG for a specific timeout period
 * @param[in]       - TimeoutMs: Desired timeout in milliseconds
 * @return          - IWDG_OK or IWDG_ERROR
 * @note            - Automatically calculates best prescaler and reload values
 *                  - Does NOT start the watchdog
 ********************************************************************/
IWDG_StatusTypeDef IWDG_ConfigureTimeout(uint32_t TimeoutMs)
{
    IWDG_Handle_t hiwdg;
    uint8_t prescaler;
    uint32_t reload_value;
    uint16_t prescaler_div;

    /* Try each prescaler from smallest to largest */
    for (prescaler = IWDG_PRESCALER_4; prescaler <= IWDG_PRESCALER_256; prescaler++)
    {
        prescaler_div = IWDG_Prescaler_Table[prescaler];

        /* Calculate required reload value
         * Reload = (TimeoutMs * LSI_Freq) / (prescaler_div * 1000)
         */
        reload_value = (TimeoutMs * LSI_FREQ_TYP) / (prescaler_div * 1000);

        /* Check if reload value fits in 12-bit register */
        if (reload_value <= IWDG_RELOAD_MAX)
        {
            /* Valid configuration found */
            hiwdg.pIWDGx = IWDG;
            hiwdg.IWDG_Config.IWDG_Prescaler = prescaler;
            hiwdg.IWDG_Config.IWDG_Reload = (uint16_t)reload_value;

            return IWDG_Init(&hiwdg);
        }
    }

    /* Timeout too large - cannot configure */
    return IWDG_ERROR;
}

/*********************************************************************
 * @fn              - IWDG_GetFlagStatus
 * @brief           - Returns the status of IWDG flags
 * @param[in]       - IWDG_FLAG: IWDG_SR_PVU or IWDG_SR_RVU
 * @return          - FLAG_SET or FLAG_RESET
 ********************************************************************/
uint8_t IWDG_GetFlagStatus(uint16_t IWDG_FLAG)
{
    if (IWDG->SR & IWDG_FLAG)
        return FLAG_SET;

    return FLAG_RESET;
}

/*********************************************************************
 * @fn              - IWDG_WaitForUpdate
 * @brief           - Waits for IWDG register updates to complete
 * @return          - IWDG_OK or IWDG_TIMEOUT
 * @note            - Called internally after writing to PR or RLR
 ********************************************************************/
IWDG_StatusTypeDef IWDG_WaitForUpdate(void)
{
    uint32_t timeout = IWDG_UPDATE_TIMEOUT;

    /* Wait for PVU and RVU flags to be cleared */
    while ((IWDG->SR & (IWDG_SR_PVU | IWDG_SR_RVU)) && timeout--)
    {
        /* Waiting for update... */
    }

    if (timeout == 0)
        return IWDG_TIMEOUT;

    return IWDG_OK;
}

/*********************************************************************
 * Preset Timeout Configuration Functions
 ********************************************************************/

/*********************************************************************
 * @fn              - IWDG_Init_100ms
 * @brief           - Initializes IWDG with 100ms timeout
 ********************************************************************/
IWDG_StatusTypeDef IWDG_Init_100ms(void)
{
    IWDG_Handle_t hiwdg;

    hiwdg.pIWDGx = IWDG;
    hiwdg.IWDG_Config.IWDG_Prescaler = IWDG_PRESCALER_4;    /* /4 = 8kHz */
    hiwdg.IWDG_Config.IWDG_Reload = 799;                    /* ~100ms */

    return IWDG_Init(&hiwdg);
}

/*********************************************************************
 * @fn              - IWDG_Init_500ms
 * @brief           - Initializes IWDG with 500ms timeout
 ********************************************************************/
IWDG_StatusTypeDef IWDG_Init_500ms(void)
{
    IWDG_Handle_t hiwdg;

    hiwdg.pIWDGx = IWDG;
    hiwdg.IWDG_Config.IWDG_Prescaler = IWDG_PRESCALER_8;    /* /8 = 4kHz */
    hiwdg.IWDG_Config.IWDG_Reload = 1999;                   /* ~500ms */

    return IWDG_Init(&hiwdg);
}

/*********************************************************************
 * @fn              - IWDG_Init_1s
 * @brief           - Initializes IWDG with 1 second timeout
 ********************************************************************/
IWDG_StatusTypeDef IWDG_Init_1s(void)
{
    IWDG_Handle_t hiwdg;

    hiwdg.pIWDGx = IWDG;
    hiwdg.IWDG_Config.IWDG_Prescaler = IWDG_PRESCALER_8;    /* /8 = 4kHz */
    hiwdg.IWDG_Config.IWDG_Reload = 3999;                   /* ~1000ms */

    return IWDG_Init(&hiwdg);
}

/*********************************************************************
 * @fn              - IWDG_Init_2s
 * @brief           - Initializes IWDG with 2 seconds timeout
 ********************************************************************/
IWDG_StatusTypeDef IWDG_Init_2s(void)
{
    IWDG_Handle_t hiwdg;

    hiwdg.pIWDGx = IWDG;
    hiwdg.IWDG_Config.IWDG_Prescaler = IWDG_PRESCALER_16;   /* /16 = 2kHz */
    hiwdg.IWDG_Config.IWDG_Reload = 3999;                   /* ~2000ms */

    return IWDG_Init(&hiwdg);
}

/*********************************************************************
 * @fn              - IWDG_Init_5s
 * @brief           - Initializes IWDG with 5 seconds timeout
 ********************************************************************/
IWDG_StatusTypeDef IWDG_Init_5s(void)
{
    IWDG_Handle_t hiwdg;

    hiwdg.pIWDGx = IWDG;
    hiwdg.IWDG_Config.IWDG_Prescaler = IWDG_PRESCALER_32;   /* /32 = 1kHz */
    hiwdg.IWDG_Config.IWDG_Reload = 4095;                   /* ~4.095s (max for /32) */

    return IWDG_Init(&hiwdg);
}

/*********************************************************************
 * @fn              - IWDG_Init_10s
 * @brief           - Initializes IWDG with 10 seconds timeout
 ********************************************************************/
IWDG_StatusTypeDef IWDG_Init_10s(void)
{
    IWDG_Handle_t hiwdg;

    hiwdg.pIWDGx = IWDG;
    hiwdg.IWDG_Config.IWDG_Prescaler = IWDG_PRESCALER_64;   /* /64 = 500Hz */
    hiwdg.IWDG_Config.IWDG_Reload = 4095;                   /* ~8.19s (closest to 10s) */

    return IWDG_Init(&hiwdg);
}
