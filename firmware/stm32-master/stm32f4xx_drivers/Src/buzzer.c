/*
 * buzzer.c
 *
 *  Created on: Jan 12, 2026
 *      Author: Rahul B.
 */

#include "stm32f446xx_gpio_driver.h"  // For GPIO_Handle_t, GPIO functions
#include "config.h"                    // For pin definitions (WAKEUP_BTN_PORT, etc.)
#include <stdint.h>                    // For uint8_t, uint16_t

#include "buzzer.h"
#include "lcd.h"                       

void Buzzer_Init(void)
{
    GPIO_Handle_t buzzer;
    memset(&buzzer, 0, sizeof(buzzer));

    GPIO_PeriClockControl(BUZZER_PORT, ENABLE);

    buzzer.pGPIOx = BUZZER_PORT;
    buzzer.GPIO_PinConfig.GPIO_PinNumber = BUZZER_PIN;
    buzzer.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
    buzzer.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    buzzer.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    buzzer.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

    GPIO_Init(&buzzer);
    GPIO_WriteToOutputPin(BUZZER_PORT, BUZZER_PIN, GPIO_PIN_RESET);

}

void Buzzer_Beep(uint8_t count, uint16_t duration_ms)
{
    for(uint8_t i = 0; i < count; i++)
    {
        Buzzer_On();            // Turn ON
        delay_ms(duration_ms);  // Wait exactly duration_ms

        Buzzer_Off();           // Turn OFF
        delay_ms(duration_ms);  // Wait exactly duration_ms
    }
}

void Buzzer_On(void)
{
    GPIO_WriteToOutputPin(BUZZER_PORT, BUZZER_PIN, GPIO_PIN_SET);
}

void Buzzer_Off(void)
{
    GPIO_WriteToOutputPin(BUZZER_PORT, BUZZER_PIN, GPIO_PIN_RESET);
}
