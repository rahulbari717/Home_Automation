/*
 * main.c
 *
 *  Created on: Jan 12, 2026
 *      Author: Rahul B.
 *  Description: Secure Smart Home Hub - Main Application
 *               STM32F446RE with 4x4 Keypad and 16x2 LCD
 */

#include "lcd.h"
#include "led.h"
#include "buzzer.h"
#include "button.h"

int main(void)
{
    
    LED_Init();
    Buzzer_Init();
    WakeupButton_Init();
    // LCD_Init();
    // Test 1: LED Init

//    Buzzer_Beep(3, 1000);
//
//    for(int i =0; i<3; i++){
//            LED_RedOn();
//            LED_GreenOff();
//            delay(1000);
//            LED_RedOff();
//            LED_GreenOn();
//            delay(1000);
//
//        }
//    LED_GreenOff();

    // Test 2: LCD Init
    
    // LCD_Clear();
    // LCD_PrintLine(0, "LCD Test OK");

    // Test 3: Buzzer
    
     for(int i =0; i<3; i++){
             Buzzer_On();
             delay(1000);
             Buzzer_Off();
             delay(1000);
         }

    // Test 4: Button
    
    // LCD_Clear();
    
    // LCD_PrintLine(0, "Press Button");
//    while(1) {
//        if(WakeupButton_IsPressed()) {
//            // LCD_PrintLine(1, "Button Pressed!");
//            LED_GreenOn();
//        } else {
//            LED_GreenOff();
//        }
//    }

    while (1);
}

// #include "stm32f446xx.h"
// #include "stm32f446xx_gpio_driver.h"
// #include "lcd.h"
// #include "keypad.h"
// #include "menu.h"
// #include "user_auth.h"
// #include "room_control.h"
// #include "config.h"
// #include "main.h"
// #include <string.h>
// #include <stdio.h>

// /* ===== GLOBAL STATE ===== */
// static SystemState_t system_state = STATE_DEEP_SLEEP;
// static volatile uint8_t wakeup_flag = 0;

// /* ===== MAIN ENTRY POINT ===== */
// int main(void)
// {
//     System_Init();

//     while(1)
//     {
//         switch(system_state)
//         {
//             case STATE_DEEP_SLEEP:
//                 State_DeepSleep();
//                 break;

//             case STATE_AUTHENTICATION:
//                 State_Authentication();
//                 break;

//             case STATE_MENU_NAVIGATION:
//                 State_MenuNavigation();
//                 break;

//             case STATE_LOCKOUT:
//                 State_Lockout();
//                 break;

//             default:
//                 system_state = STATE_DEEP_SLEEP;
//                 break;
//         }
//     }

//     return 0;
// }

// /* ===== SYSTEM INITIALIZATION ===== */
// void System_Init(void)
// {
//     Hardware_Init();
//     UserAuth_Init();
//     Menu_Init();
//     Room_Init();

//     // Display welcome message
//     LCD_Clear();
//     LCD_PrintLine(0, "Smart Home Hub");
//     LCD_PrintLine(1, "Initializing...");
//     LCD_DelayMs(2000);

//     LCD_Clear();
//     LCD_PrintLine(0, "System Ready");
//     LCD_PrintLine(1, "Press Button");
//     LCD_DelayMs(1500);

//     system_state = STATE_DEEP_SLEEP;
// }

// void Hardware_Init(void)
// {
//     LED_Init();
//     Buzzer_Init();
//     WakeupButton_Init();
//     LCD_Init();
//     Keypad_Init();
// }

// /* ===== STATE MACHINE IMPLEMENTATIONS ===== */

// void State_DeepSleep(void)
// {
//     // Turn off LCD
//     LCD_Clear();
//     LCD_SendCommand(LCD_CMD_DISPLAY_OFF);

//     // Turn off all LEDs
//     GPIO_WriteToOutputPin(LED_PORT, LED_GREEN_PIN, GPIO_PIN_RESET);
//     GPIO_WriteToOutputPin(LED_PORT, LED_RED_PIN, GPIO_PIN_RESET);

//     // Flash red LED every 5 seconds (low power indicator)
//     while(!wakeup_flag) {
//         LED_Flash(LED_PORT, LED_RED_PIN, 1, 100);
//         LCD_DelayMs(5000);
//     }

//     wakeup_flag = 0;

//     // Turn on LCD
//     LCD_SendCommand(LCD_CMD_DISPLAY_ON);
//     LCD_Clear();

//     // Transition to authentication
//     system_state = STATE_AUTHENTICATION;
// }

// void State_Authentication(void)
// {
//     char pin[PIN_LENGTH + 1];
//     int8_t user_id;
//     uint8_t attempt;

//     UserAuth_ClearLoginAttempts();

//     for(attempt = 0; attempt < MAX_LOGIN_ATTEMPTS; attempt++) {
//         LCD_Clear();
//         LCD_PrintLine(0, "Enter PIN:");
//         LCD_SetCursor(1, 0);

//         // Get PIN input
//         Keypad_GetNumericInput(pin, PIN_LENGTH, 1); // Masked display

//         // Authenticate
//         user_id = UserAuth_Login(pin);

//         if(user_id >= 0) {
//             // Success
//             User_t *user = UserAuth_GetUser(user_id);

//             LCD_Clear();
//             LCD_PrintLine(0, "Welcome!");
//             LCD_PrintLine(1, user->username);

//             GPIO_WriteToOutputPin(LED_PORT, LED_GREEN_PIN, GPIO_PIN_SET);
//             Buzzer_Beep(1, 200);
//             LCD_DelayMs(2000);

//             system_state = STATE_MENU_NAVIGATION;
//             return;
//         } else {
//             // Failed
//             LCD_Clear();
//             LCD_PrintLine(0, "Invalid PIN!");

//             char msg[17];
//             snprintf(msg, 17, "Attempt %d/%d", attempt + 1, MAX_LOGIN_ATTEMPTS);
//             LCD_PrintLine(1, msg);

//             LED_Flash(LED_PORT, LED_RED_PIN, 3, 200);
//             Buzzer_Beep(3, 200);
//             LCD_DelayMs(2000);
//         }
//     }

//     // Max attempts reached - lockout
//     system_state = STATE_LOCKOUT;
// }

// void State_MenuNavigation(void)
// {
//     Menu_Run();

//     // User pressed logout (#)
//     LCD_Clear();
//     LCD_PrintLine(0, "Logging Out...");
//     LCD_PrintLine(1, "Goodbye!");

//     GPIO_WriteToOutputPin(LED_PORT, LED_GREEN_PIN, GPIO_PIN_RESET);
//     LCD_DelayMs(2000);

//     UserAuth_Logout();
//     system_state = STATE_DEEP_SLEEP;
// }

// void State_Lockout(void)
// {
//     LCD_Clear();
//     LCD_PrintLine(0, "LOCKOUT MODE!");
//     LCD_PrintLine(1, "Wait 20 sec...");

//     // Alternating buzzer and red LED for 20 seconds
//     for(uint8_t i = 0; i < 20; i++) {
//         GPIO_ToggleOutputPin(LED_PORT, LED_RED_PIN);
//         GPIO_ToggleOutputPin(BUZZER_PORT, BUZZER_PIN);
//         LCD_DelayMs(1000);
//     }

//     // Turn off buzzer and LED
//     GPIO_WriteToOutputPin(LED_PORT, LED_RED_PIN, GPIO_PIN_RESET);
//     GPIO_WriteToOutputPin(BUZZER_PORT, BUZZER_PIN, GPIO_PIN_RESET);

//     LCD_Clear();
//     LCD_PrintLine(0, "System Locked");
//     LCD_PrintLine(1, "Shutting Down");
//     LCD_DelayMs(2000);

//     system_state = STATE_DEEP_SLEEP;
// }

// /* ===== INTERRUPT HANDLERS ===== */

// void EXTI15_10_IRQHandler(void)
// {
//     LCD_DelayMs(50); // Debounce
//     GPIO_IRQHandling(WAKEUP_BTN_PIN);

//     // Check if button is still pressed (falling edge)
//     if(GPIO_ReadFromInputPin(WAKEUP_BTN_PORT, WAKEUP_BTN_PIN) == GPIO_PIN_RESET) {
//         wakeup_flag = 1;
//     }
// }
