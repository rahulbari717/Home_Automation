///*
// * main.c
// *
// *  Created on: Jan 12, 2026
// *      Author: Rahul B.
// *  Description: Secure Smart Home Hub - Main Application
// *               STM32F446RE with 4x4 Keypad and 16x2 LCD
// */
//
///************* Include headers ************************************************/
//
//#include "stm32f446xx.h"
//#include "lcd.h"
//#include "keypad.h"
//#include "menu.h"
//#include "user_auth.h"
//#include "room_control.h"
//#include "config.h"
//#include "main.h"
//#include "led.h"
//
///* ===== GLOBAL STATE ===== */
//static SystemState_t system_state = STATE_DEEP_SLEEP;
//static volatile uint8_t wakeup_flag = 0;
//
//// Handle structure for USART2
//USART_Handle_t usart2_handle;
//
//// External Delay function
//extern void delay_ms(uint32_t ms);
//
///* ===== DEBUG HELPER FUNCTION ===== */
//// This function acts like printf but sends to Minicom
//void UART_Printf(const char *format, ...)
//{
//    char buffer[128]; // Buffer for the message
//    va_list args;
//    va_start(args, format);
//
//    // Convert format + variables into a string
//    vsnprintf(buffer, sizeof(buffer), format, args);
//
//    // Send via UART (Blocking)
//    USART_SendData(&usart2_handle, (uint8_t*)buffer, strlen(buffer));
//
//    va_end(args);
//}
////
////int main(void)
////{
////    // 1. Init System
////    System_Init();
////
////    UART_Printf(">> [MAIN] System Init Complete. Entering Main Loop.\r\n");
////
////    while(1)
////    {
////    	switch(system_state)
////        {
////        	case STATE_DEEP_SLEEP:
////				UART_Printf(">> [STATE] Switching to DEEP SLEEP\r\n");
////				State_DeepSleep();
////				break;
////
////			case STATE_AUTHENTICATION:
////				UART_Printf(">> [STATE] Switching to AUTHENTICATION\r\n");
////				State_Authentication();
////				break;
////
////			case STATE_MENU_NAVIGATION:
////				UART_Printf(">> [STATE] Switching to MENU\r\n");
////				State_MenuNavigation();
////				break;
////
////			case STATE_LOCKOUT:
////				UART_Printf(">> [STATE] Switching to LOCKOUT\r\n");
////				State_Lockout();
////				break;
////
////			default:
////				UART_Printf(">> [ERROR] Unknown State! Resetting to Sleep.\r\n");
////				system_state = STATE_DEEP_SLEEP;
////				break;
////		}
////	}
////
////    return 0;
////}
//
//int main(void)
//{
//    // 1. Initialize Hardware
//    LED_Init();
//    LCD_Init();
//
//    // 2. Initial Pattern
//    LCD_Clear();
//    LCD_PrintLine(0, "TESTING...");
//    LCD_PrintLine(1, "Watch Text!");
//
//    delay_ms(2000);
//
//    while(1)
//    {
//        // STEP 1: Turn Display OFF (Text should vanish)
//        // If you have UART_Printf enabled: UART_Printf("Turning Display OFF\r\n");
//        LCD_SendCommand(LCD_CMD_DISPLAY_OFF);
//
//        LED_RedOn(); // LED Red indicates "OFF" state
//        delay_ms(2000);
//
//        // STEP 2: Turn Display ON (Text should reappear)
//        // If you have UART_Printf enabled: UART_Printf("Turning Display ON\r\n");
//        LCD_SendCommand(LCD_CMD_DISPLAY_ON);
//
//        LED_RedOff(); // LED Off indicates "ON" state
//        delay_ms(2000);
//    }
//}
//
///* ===== SYSTEM INITIALIZATION ===== */
//void System_Init(void)
//{
//    Hardware_Init();
//
//    // Enable UART for Debugging immediately
//    USART_PeripheralControl(USART2, ENABLE);
//    UART_Printf("\r\n\r\n=== REBOOTING SYSTEM ===\r\n");
//    UART_Printf(">> [INIT] Hardware Initialized.\r\n");
//
//    UserAuth_Init();
//    Menu_Init();
//    Room_Init();
//    UART_Printf(">> [INIT] Logic Modules Initialized.\r\n");
//
//    // Display welcome message
//    LCD_Clear();
//    LCD_PrintLine(0, "Smart Home Hub");
//    LCD_PrintLine(1, "Initializing...");
//    delay_ms(2000);
//
//    LCD_Clear();
//    LCD_PrintLine(0, "System Ready");
//    LCD_PrintLine(1, "Press Button");
//    delay_ms(1500);
//
//    system_state = STATE_DEEP_SLEEP;
//}
//
//void Hardware_Init(void)
//{
//    USART2_GPIOInit();
//    USART2_Init(); // Init UART first to enable debug prints early
//
//    LED_Init();
//    Buzzer_Init();
//    WakeupButton_Init();
//    LCD_Init();
//    Keypad_Init();
//}
//
///* ===== USART CONFIGURATION ===== */
//void USART2_GPIOInit(void)
//{
//    GPIO_Handle_t usart_gpios;
//    usart_gpios.pGPIOx = GPIOA;
//    usart_gpios.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
//    usart_gpios.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
//    usart_gpios.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;
//    usart_gpios.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
//    usart_gpios.GPIO_PinConfig.GPIO_PinAltFunMode = 7;
//
//    usart_gpios.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_2; // TX
//    GPIO_Init(&usart_gpios);
//    usart_gpios.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_3; // RX
//    GPIO_Init(&usart_gpios);
//}
//
//void USART2_Init(void)
//{
//    usart2_handle.pUSARTx = USART2;
//    usart2_handle.USART_Config.USART_Baud = USART_STD_BAUD_115200;
//    usart2_handle.USART_Config.USART_HWFlowControl = USART_HW_FLOW_CTRL_NONE;
//    usart2_handle.USART_Config.USART_Mode = USART_MODE_ONLY_TX;
//    usart2_handle.USART_Config.USART_NoOfStopBits = USART_STOPBITS_1;
//    usart2_handle.USART_Config.USART_WordLength = USART_WORDLEN_8BITS;
//    usart2_handle.USART_Config.USART_ParityControl = USART_PARITY_DISABLE;
//    USART_Init(&usart2_handle);
//}
//
///* ===== STATE MACHINE IMPLEMENTATIONS ===== */
//
//void State_DeepSleep(void)
//{
//    UART_Printf(">> [SLEEP] Display OFF. Waiting for Wakeup Flag...\r\n");
//
//    LCD_Clear();
//    LCD_SendCommand(LCD_CMD_DISPLAY_OFF);
//
//    GPIO_WriteToOutputPin(LED_PORT, LED_GREEN_PIN, GPIO_PIN_RESET);
//    GPIO_WriteToOutputPin(LED_PORT, LED_RED_PIN, GPIO_PIN_RESET);
//
//    // Loop until flag is set by ISR
//    while(!wakeup_flag) {
//        // Heartbeat print to show system hasn't crashed
//        // UART_Printf(".");
//        LED_Flash(LED_PORT, LED_RED_PIN, 1, 100);
//
//
//        // WARNING: A 5s blocking delay here makes the button unresponsive for 5s at a time
//        // Better to check flag more often.
//        for(int i=0; i<50; i++) {
//             delay_ms(100);
//             if(wakeup_flag) break; // Exit immediately if button pressed
//        }
//    }
//
//    UART_Printf("\r\n>> [SLEEP] WAKEUP DETECTED!\r\n");
//
//    wakeup_flag = 0; // Clear flag
//
//    LCD_SendCommand(LCD_CMD_DISPLAY_ON);
//    LCD_Clear();
//
//    system_state = STATE_AUTHENTICATION;
//}
//
//void State_Authentication(void)
//{
//    char pin[PIN_LENGTH + 1];
//    int8_t user_id;
//    uint8_t attempt;
//
//    UserAuth_ClearLoginAttempts();
//
//    for(attempt = 0; attempt < MAX_LOGIN_ATTEMPTS; attempt++) {
//        UART_Printf(">> [AUTH] Login Attempt %d/%d. Waiting for Keypad...\r\n", attempt + 1, MAX_LOGIN_ATTEMPTS);
//
//        LCD_Clear();
//        LCD_PrintLine(0, "Enter PIN:");
//        LCD_SetCursor(1, 0);
//
//        // This function blocks until 4 keys are pressed
//        Keypad_GetNumericInput(pin, PIN_LENGTH, 1);
//        UART_Printf(">> [AUTH] PIN Entered: %s\r\n", pin);
//
//        user_id = UserAuth_Login(pin);
//
//        if(user_id >= 0) {
//            User_t *user = UserAuth_GetUser(user_id);
//            UART_Printf(">> [AUTH] SUCCESS! User: %s\r\n", user->username);
//
//            LCD_Clear();
//            LCD_PrintLine(0, "Welcome!");
//            LCD_PrintLine(1, user->username);
//
//            GPIO_WriteToOutputPin(LED_PORT, LED_GREEN_PIN, GPIO_PIN_SET);
//            Buzzer_Beep(1, 200);
//            delay_ms(2000);
//
//            system_state = STATE_MENU_NAVIGATION;
//            return;
//        } else {
//            UART_Printf(">> [AUTH] FAILED. Invalid PIN.\r\n");
//
//            LCD_Clear();
//            LCD_PrintLine(0, "Invalid PIN!");
//            char msg[17];
//            snprintf(msg, 17, "Attempt %d/%d", attempt + 1, MAX_LOGIN_ATTEMPTS);
//            LCD_PrintLine(1, msg);
//
////            LED_Flash(LED_PORT, LED_RED_PIN, 3, 200);
//            LED_Flash(LED_PORT, LED_RED_PIN, 1, 100);
//            Buzzer_Beep(3, 200);
//            delay_ms(2000);
//        }
//    }
//
//    UART_Printf(">> [AUTH] Too many failures. LOCKING SYSTEM.\r\n");
//    system_state = STATE_LOCKOUT;
//}
//
//void State_MenuNavigation(void)
//{
//    UART_Printf(">> [MENU] Entering Menu Loop.\r\n");
//
//    // Note: If Menu_Run crashes, check logic inside menu.c
//    Menu_Run();
//
//    UART_Printf(">> [MENU] Logged Out.\r\n");
//
//    LCD_Clear();
//    LCD_PrintLine(0, "Logging Out...");
//    LCD_PrintLine(1, "Goodbye!");
//
//    GPIO_WriteToOutputPin(LED_PORT, LED_GREEN_PIN, GPIO_PIN_RESET);
//    delay_ms(2000);
//
//    UserAuth_Logout();
//    system_state = STATE_DEEP_SLEEP;
//}
//
//void State_Lockout(void)
//{
//    UART_Printf(">> [LOCK] System Lockout for 20s.\r\n");
//
//    LCD_Clear();
//    LCD_PrintLine(0, "LOCKOUT MODE!");
//    LCD_PrintLine(1, "Wait 20 sec...");
//
//    for(uint8_t i = 0; i < 20; i++) {
//        GPIO_ToggleOutputPin(LED_PORT, LED_RED_PIN);
//        GPIO_ToggleOutputPin(BUZZER_PORT, BUZZER_PIN);
//        delay_ms(1000);
//        UART_Printf(">> [LOCK] Time remaining: %ds\r\n", 20-i);
//    }
//
//    GPIO_WriteToOutputPin(LED_PORT, LED_RED_PIN, GPIO_PIN_RESET);
//    GPIO_WriteToOutputPin(BUZZER_PORT, BUZZER_PIN, GPIO_PIN_RESET);
//
//    system_state = STATE_DEEP_SLEEP;
//}
//
///* ===== INTERRUPT HANDLERS ===== */
//
//void EXTI15_10_IRQHandler(void)
//{
//    // WARNING: Do not put heavy print functions here, it might crash.
//    // We send 1 character just to confirm ISR entry.
//    USART_SendData(&usart2_handle, (uint8_t*)"!", 1);
//
//    // Use the assembly/loop delay here to avoid SysTick conflicts inside ISR
//    // LCD_DelayMs might be unsafe if it uses SysTick and priorities aren't set right
//    for(int i=0; i<500000; i++); // Simple debounce
//
//    GPIO_IRQHandling(WAKEUP_BTN_PIN);
//
//    if(GPIO_ReadFromInputPin(WAKEUP_BTN_PORT, WAKEUP_BTN_PIN) == 0) {
//        wakeup_flag = 1;
//        // ISR exit print
//        USART_SendData(&usart2_handle, (uint8_t*)"#", 1);
//    }
//}

//----------------------------------------------------------------------------------------------------------------

/*
 * main.c - HEAVY DEBUG VERSION
 */

/*
 * Src/main.c
 * FULL VERSION - No missing functions
 */

#include "stm32f446xx.h"
#include "lcd.h"
#include "keypad.h"
#include "menu.h"
#include "user_auth.h"
#include "room_control.h"
#include "config.h"
#include "main.h"
#include "stm32f446xx_usart_driver.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

/* ===== GLOBAL STATE ===== */
static SystemState_t system_state = STATE_DEEP_SLEEP;
static volatile uint8_t wakeup_flag = 0;

USART_Handle_t usart2_handle;
extern void delay_ms(uint32_t ms);

/* ===== DEBUG FUNCTION ===== */
void UART_Printf(const char *format, ...)
{
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    USART_SendData(&usart2_handle, (uint8_t*)buffer, strlen(buffer));
    va_end(args);
}

/* ===== PROTOTYPES ===== */
void USART2_GPIOInit(void);
void USART2_Init(void);

/* ===== MAIN ===== */
int main(void)
{
    // 1. Init UART First for Debugging
    USART2_GPIOInit();
    USART2_Init();
    USART_PeripheralControl(USART2, ENABLE);

    UART_Printf("\r\n\r\n========================================\r\n");
    UART_Printf(">> [BOOT] System Power On.\r\n");

    // 2. Init Hardware
    UART_Printf(">> [INIT] Initializing LED & Buzzer...\r\n");
    LED_Init();
    Buzzer_Init();

    UART_Printf(">> [INIT] Initializing Button...\r\n");
    WakeupButton_Init();

    UART_Printf(">> [INIT] Initializing LCD...\r\n");
    LCD_Init();

    UART_Printf(">> [INIT] Initializing Keypad...\r\n");
    Keypad_Init();

    UART_Printf(">> [INIT] Initializing Logic...\r\n");
    UserAuth_Init();
    Menu_Init();
    Room_Init();

    UART_Printf(">> [MAIN] Init Complete. Starting State Machine.\r\n");

    while(1)
    {
        switch(system_state)
        {
            case STATE_DEEP_SLEEP:
                State_DeepSleep();
                break;

            case STATE_AUTHENTICATION:
                State_Authentication();
                break;

            case STATE_MENU_NAVIGATION:
                State_MenuNavigation();
                break;

            case STATE_LOCKOUT:
                State_Lockout();
                break;

            default:
                UART_Printf(">> [FATAL] Unknown State! Resetting.\r\n");
                system_state = STATE_DEEP_SLEEP;
                break;
        }
    }
}

/* ===== STATE FUNCTIONS ===== */

void State_DeepSleep(void)
{
    UART_Printf(">> [SLEEP] Display OFF. Waiting for Button...\r\n");
    LCD_Clear();
    LCD_SendCommand(LCD_CMD_DISPLAY_OFF);

    // Turn off LEDs
    GPIO_WriteToOutputPin(LED_PORT, LED_GREEN_PIN, GPIO_PIN_RESET);
    GPIO_WriteToOutputPin(LED_PORT, LED_RED_PIN, GPIO_PIN_RESET);

    wakeup_flag = 0;

    while(!wakeup_flag) {
        // Blink Red LED slowly to show system is alive
        LED_Flash(LED_PORT, LED_RED_PIN, 1, 100);

        // Wait loop checking flag frequently
        for(int i=0; i<30; i++) {
             delay_ms(100);
             if(wakeup_flag) break;
        }
    }

    UART_Printf(">> [SLEEP] Waking Up!\r\n");
    wakeup_flag = 0;
    LCD_SendCommand(LCD_CMD_DISPLAY_ON);
    LCD_Clear();

    system_state = STATE_AUTHENTICATION;
}

void State_Authentication(void)
{
    UART_Printf(">> [AUTH] Enter PIN State.\r\n");
    GPIO_WriteToOutputPin(LED_PORT, LED_GREEN_PIN, GPIO_PIN_SET); // Green LED indicates active

    char pin[PIN_LENGTH + 1];
    int8_t user_id;
    uint8_t attempt;

    UserAuth_ClearLoginAttempts();

    for(attempt = 0; attempt < MAX_LOGIN_ATTEMPTS; attempt++) {
        LCD_Clear();
        LCD_PrintLine(0, "Enter PIN:");
        LCD_SetCursor(1, 0);

        UART_Printf(">> [AUTH] Waiting for Keypad...\r\n");
        Keypad_GetNumericInput(pin, PIN_LENGTH, 1);
        UART_Printf(">> [AUTH] Input: %s\r\n", pin);

        user_id = UserAuth_Login(pin);

        if(user_id >= 0) {
            User_t *user = UserAuth_GetUser(user_id);
            UART_Printf(">> [AUTH] Success! User: %s\r\n", user->username);

            LCD_Clear();
            LCD_PrintLine(0, "Welcome!");
            LCD_PrintLine(1, user->username);
            delay_ms(2000);

            system_state = STATE_MENU_NAVIGATION;
            return;
        } else {
            UART_Printf(">> [AUTH] Failed.\r\n");
            LCD_Clear();
            LCD_PrintLine(0, "Invalid PIN!");
            LED_Flash(LED_PORT, LED_RED_PIN, 2, 200);
            delay_ms(1000);
        }
    }

    system_state = STATE_LOCKOUT;
}

void State_MenuNavigation(void)
{
    UART_Printf(">> [MENU] Running Menu System.\r\n");
    Menu_Run();

    UART_Printf(">> [MENU] Logged Out.\r\n");
    LCD_Clear();
    LCD_PrintLine(0, "Logging Out...");
    delay_ms(1500);

    UserAuth_Logout();
    system_state = STATE_DEEP_SLEEP;
}

void State_Lockout(void)
{
    UART_Printf(">> [LOCK] System Lockout (20s).\r\n");
    LCD_Clear();
    LCD_PrintLine(0, "SYSTEM LOCKED");
    LCD_PrintLine(1, "Wait 20s...");

    for(uint8_t i = 0; i < 20; i++) {
        GPIO_ToggleOutputPin(LED_PORT, LED_RED_PIN);
        delay_ms(1000);
    }

    system_state = STATE_DEEP_SLEEP;
}

/* ===== HARDWARE INIT FUNCTIONS ===== */

void USART2_GPIOInit(void)
{
    GPIO_Handle_t usart_gpios;
    usart_gpios.pGPIOx = GPIOA;
    usart_gpios.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
    usart_gpios.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    usart_gpios.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;
    usart_gpios.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    usart_gpios.GPIO_PinConfig.GPIO_PinAltFunMode = 7;

    usart_gpios.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_2; // TX
    GPIO_Init(&usart_gpios);
    usart_gpios.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_3; // RX
    GPIO_Init(&usart_gpios);
}

void USART2_Init(void)
{
    usart2_handle.pUSARTx = USART2;
    usart2_handle.USART_Config.USART_Baud = USART_STD_BAUD_115200;
    usart2_handle.USART_Config.USART_HWFlowControl = USART_HW_FLOW_CTRL_NONE;
    usart2_handle.USART_Config.USART_Mode = USART_MODE_ONLY_TX;
    usart2_handle.USART_Config.USART_NoOfStopBits = USART_STOPBITS_1;
    usart2_handle.USART_Config.USART_WordLength = USART_WORDLEN_8BITS;
    usart2_handle.USART_Config.USART_ParityControl = USART_PARITY_DISABLE;
    USART_Init(&usart2_handle);
}

/* ===== INTERRUPT HANDLER ===== */
void EXTI15_10_IRQHandler(void)
{
    // Simple debounce delay
    for(int i=0; i<500000; i++);

    GPIO_IRQHandling(WAKEUP_BTN_PIN);

    if(GPIO_ReadFromInputPin(WAKEUP_BTN_PORT, WAKEUP_BTN_PIN) == 0) {
        wakeup_flag = 1;
        USART_SendData(&usart2_handle, (uint8_t*)"[ISR_BTN]\r\n", 11);
    }
}
