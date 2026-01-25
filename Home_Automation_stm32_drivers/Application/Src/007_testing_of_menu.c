/*
 * 007_testing_of_menu.c
 *
 *  Created on: Jan 25, 2026
 *      Author: Rahul B.
 */

/*
    To make this work, we will treat the system as 
    having three main "States": STANDBY, AUTHENTICATING, and ACTIVE.

    🛠️ Hardware Requirements
    LEDs: White (Standby), Green (Success), Red (Error).
    Inputs: Push Button (Start), 4x4 Keypad (PIN).
    Feedback: Buzzer, UART Terminal (Display).

    🧠 System Logic Flow
    STANDBY Mode (System Off):
    White LED: Blinks (1s ON, 3s OFF).
    The MCU is waiting for the Button Interrupt (EXTI13).

    AUTHENTICATION Mode:
    Green LED: ON (System is "Awake").
    UART: Displays "Enter 4-Digit PIN:".
    Keypad: Collects 4 keys. You have 3 attempts.

    SUCCESS / ACTIVE Mode:
    UART: Shows the Menu (1-9 for Hall LEDs, etc.).
    Wait for user commands.
    FAILURE / LOCKOUT Mode:
    Red LED + Buzzer: Toggle (1s ON, 1s OFF) for 3 seconds.
    System: Shuts down and returns to STANDBY.
*/

#include "bsp_init.h"
#include "bsp_keypad.h"

/* ===== Private Macros & Constants ===== */
#define MASTER_PIN          "1234"

/* ===== Global Variables ===== */
USART_Handle_t usart2_handle;
SystemState_t current_state = STATE_STANDBY;
volatile uint8_t system_start_flag = 0; // Triggered by Button Interrupt

/* ===== Function Prototypes ===== */
void handle_standby(void);
uint8_t handle_authentication(void);
void handle_active_menu(void);
void handle_lockout(void);
void delay_ms(uint32_t ms);

/* ================= MAIN APPLICATION ================= */
int main(void) {
    app_init();

    UART_Printf(">> [BOOT] The Citadel System Initialized...\r\n");

    while(1) {
        switch(current_state) {

            case STATE_STANDBY:
                handle_standby();
                if(system_start_flag) {
                    system_start_flag = 0; // Reset flag
                    current_state = STATE_AUTHENTICATING;
                }
                break;

            case STATE_AUTHENTICATING:
                // Green LED ON (System Awake)
                GPIO_WriteToOutputPin(LED_PORT, LED_GREEN_PIN, GPIO_PIN_SET);

                if(handle_authentication()) {
                    current_state = STATE_ACTIVE_MENU;
                } else {
                    current_state = STATE_LOCKOUT;
                }
                break;

            case STATE_ACTIVE_MENU:
                handle_active_menu();
                // Return to standby after logout or timeout
                current_state = STATE_STANDBY;
                break;

            case STATE_LOCKOUT:
                handle_lockout();
                current_state = STATE_STANDBY;
                break;
        }
    }
}

/* ================= STATE HANDLERS ================= */

/**
 * @brief STANDBY: White LED Blinks 1s ON, 3s OFF
 */
void handle_standby(void) {
    // Using PA7 as White LED (Example)
    GPIO_WriteToOutputPin(GPIOA, LED_WHITE_PIN, GPIO_PIN_SET);
    delay_ms(1000);
    GPIO_WriteToOutputPin(GPIOA, LED_WHITE_PIN, GPIO_PIN_RESET);

    // Check for button press during the 3s "off" period
    for(int i=0; i<30; i++) {
        if(system_start_flag) return;
        delay_ms(100);
    }
}

/**
 * @brief AUTHENTICATION: Collects PIN and verifies
 */
uint8_t handle_authentication(void) {
    char input_pin[PIN_LENGTH + 1];
    uint8_t attempt_count = 0;

    UART_Printf("\r\n>> SYSTEM AWAKE. Please Authenticate.\r\n");

    while(attempt_count < MAX_LOGIN_ATTEMPTS) {
        uint8_t char_count = 0;
        UART_Printf("Enter 4-Digit PIN (Attempt %d/%d): ", attempt_count + 1, MAX_LOGIN_ATTEMPTS);

        while(char_count < PIN_LENGTH) {
            char key = Keypad_GetKey();
            if(key != KEYPAD_NO_KEY && key >= '0' && key <= '9') {
                input_pin[char_count++] = key;
                UART_Printf("*"); // Masking
            }
        }
        input_pin[PIN_LENGTH] = '\0';

        if(strcmp(input_pin, MASTER_PIN) == 0) {
            UART_Printf("\r\n>> [SUCCESS] Access Granted.\r\n");
            return 1;
        } else {
            attempt_count++;
            UART_Printf("\r\n>> [ERROR] Invalid PIN.\r\n");
        }
    }
    return 0;
}

/**
 * @brief ACTIVE MENU: Control Hall LEDs
 */
void handle_active_menu(void) {
    UART_Printf("\r\n--- THE CITADEL MAIN MENU ---\r\n");
    UART_Printf("1-9: Toggle Hall LEDs\r\n");
    UART_Printf("#: Logout\r\n");

    while(1) {
        char key = Keypad_GetKey();
        if(key != KEYPAD_NO_KEY) {
            if(key == '#') {
                UART_Printf(">> Logging out...\r\n");
                GPIO_WriteToOutputPin(LED_PORT, LED_GREEN_PIN, GPIO_PIN_RESET);
                return;
            } else if (key >= '1' && key <= '9') {
                UART_Printf(">> Hall LED %c Toggled.\r\n", key);
                // Logic to toggle specific Hall LEDs would go here
            }
        }
    }
}

/**
 * @brief LOCKOUT: Red LED + Buzzer toggle
 */
void handle_lockout(void) {
    UART_Printf(">> [ALERT] Security Lockout Active!\r\n");
    GPIO_WriteToOutputPin(LED_PORT, LED_GREEN_PIN, GPIO_PIN_RESET);

    for(int i = 0; i < 3; i++) {
        GPIO_WriteToOutputPin(LED_PORT, LED_RED_PIN, GPIO_PIN_SET);
        GPIO_WriteToOutputPin(BUZZER_PORT, BUZZER_PIN, GPIO_PIN_SET);
        delay_ms(1000);
        GPIO_WriteToOutputPin(LED_PORT, LED_RED_PIN, GPIO_PIN_RESET);
        GPIO_WriteToOutputPin(BUZZER_PORT, BUZZER_PIN, GPIO_PIN_RESET);
        delay_ms(1000);
    }
}

/* ================= INTERRUPT HANDLERS ================= */

void EXTI15_10_IRQHandler(void) {
    // Debounce delay
    for(volatile uint32_t i=0; i<100000; i++);

    if(GPIO_ReadFromInputPin(WAKEUP_BTN_PORT, WAKEUP_BTN_PIN) == 0) {
        system_start_flag = 1; // Signal main loop to wake up
    }
    GPIO_IRQHandling(WAKEUP_BTN_PIN);
}

/* ================= HELPER FUNCTIONS ================= */

void delay_ms(uint32_t ms) {
    // Approximate delay for 16MHz Clock
    for(uint32_t i = 0; i < (ms * 1000); i++);
}



