/*
 * main.c
 *
 * Created on: Jan 25, 2026
 * Author: Rahul B.
 * 
 * Description: Home Automation Master Control System
 * 
 * Hardware Integration:
 * - DS18B20: 1-Wire Temperature Sensor (PC10) - Primary accurate temp
 * - KY-028: Digital Temperature Sensor (PC11) - Threshold alarm
 * - OLED (128x64): I2C Status Hub (Clock, Temp, Lock Status, Activity)
 * - LCD (16x2): User Console (Menu, PIN Entry, Error Logs)
 * - UART2: Black Box Logging (Full system traces & debugging)
 * - 4x4 Keypad: User input for authentication and control
 * - LEDs: White (Standby), Green (Active), Red (Alert)
 * - Buzzer: Alarm notifications
 * - LDR Sensors: Light-based automation
 * - Relays: Room control outputs
 * 
 * System Features:
 * - Real-time clock with OLED display
 * - PIN-based authentication system
 * - Temperature monitoring with dual sensors
 * - Automatic light control via LDR
 * - Motion detection integration ready
 * - Full event logging to UART
 */

#include "main.h"

/* ===== Global Variables ===== */
SystemState_t current_state = STATE_STANDBY;
volatile uint8_t system_start_flag = 0;
char g_oled_status[20] = "READY";

// Data tracking structure
typedef struct {
    float temperature;
    uint16_t ldr1;
    uint16_t ldr2;
    uint32_t last_1s_report;
    uint32_t last_3s_report;
} Citadel_Data_t;

Citadel_Data_t citadel = {0};

/* ===== Function Prototypes ===== */
void update_lcd_display(const char *line1, const char *line2);
void Handle_Remote_Commands(void);
void Handle_Menu_Logic(void);
void Broadcast_Sensors(void);
void Refresh_Displays(void);
void Log_To_UART(const char* event);


int main(void) {
    app_init(); // Initializes all hardware and sets clock to 180MHz
    Log_To_UART("SYSTEM BOOT: THE CITADEL ONLINE");

    /* Show Splash Screen */
    BSP_OLED_Clear();
    OLED_Printf(15, 10, "THE CITADEL v2");
    BSP_OLED_Update();

    update_lcd_display("THE CITADEL", "System Booting..");
    BSP_Delay_ms(2000);

    while(1) {
        /* Task 1: Periodic Reporting (1s LDR, 3s Temp) */
        Broadcast_Sensors();

        /* Task 2: Process Bluetooth Commands */
        Handle_Remote_Commands();

        /* Task 3: State Machine Logic */
        switch(current_state) {
            case STATE_STANDBY:
                BSP_LED_On(LED_WHITE_PIN);
                if(system_start_flag) {
                    system_start_flag = 0;
                    current_state = STATE_ACTIVE_MENU;
                    Log_To_UART("STATE: ACTIVE MENU");
                }
                break;

            case STATE_ACTIVE_MENU:
                BSP_LED_Off(LED_WHITE_PIN);
                BSP_LED_On(LED_GREEN_PIN);
                Handle_Menu_Logic();
                break;

            default:
                current_state = STATE_STANDBY;
                break;
        }

        /* Task 4: UI Updates */
        Refresh_Displays();
    }
}

/**
 * @brief Logic for periodic reporting to terminals
 */
void Broadcast_Sensors(void) {
    uint32_t current_time = g_BSP_DelayComplete; // Rough time tracking from TIM7 ISR

    // 1-Second Report: LDR values to UART and Bluetooth
    if (current_time - citadel.last_1s_report >= 1000) {
        citadel.ldr1 = BSP_Sensor_ReadLDR(SENSOR_LDR1_CHANNEL);
        citadel.ldr2 = BSP_Sensor_ReadLDR(SENSOR_LDR2_CHANNEL);

        UART_Printf("[DATA] LDR1: %d, LDR2: %d\r\n", citadel.ldr1, citadel.ldr2);
        BSP_Bluetooth_Printf("LDR1: %d, LDR2: %d\r\n", citadel.ldr1, citadel.ldr2);
        citadel.last_1s_report = current_time;
    }

    // 3-Second Report: DS18B20 to Bluetooth
    if (current_time - citadel.last_3s_report >= 3000) {
        citadel.temperature = BSP_DS18B20_ReadTemp();
        BSP_Bluetooth_Printf("TEMP: %.2f C\r\n", citadel.temperature);
        citadel.last_3s_report = current_time;
    }
}

/**
 * @brief Handle ACTUATORS via Serial
 */
void Handle_Remote_Commands(void) {
    if (BSP_Bluetooth_DataAvailable()) {
        uint8_t cmd = BSP_Bluetooth_ReceiveByte();
        UART_Printf("[SERIAL_CMD] RX: %c\r\n", cmd);

        switch(cmd) {
            case 'R': BSP_LED_On(LED_RED_PIN);    strcpy(g_oled_status, "RED: ON"); break;
            case 'r': BSP_LED_Off(LED_RED_PIN);   strcpy(g_oled_status, "RED: OFF"); break;
            case 'B': BSP_Buzzer_On();            strcpy(g_oled_status, "BEEP: ON"); break;
            case 'b': BSP_Buzzer_Off();           strcpy(g_oled_status, "BEEP: OFF"); break;
            case 'W': BSP_LED_Toggle(LED_WHITE_PIN); break;
        }
    }
}

/**
 * @brief Menu system: 1-4 Relays, # Standby
 */
void Handle_Menu_Logic(void) {
    char key = Keypad_GetKey();
    if (key != KEYPAD_NO_KEY) {
        UART_Printf("[KEYPAD] Key: %c\r\n", key);

        if (key >= '1' && key <= '4') {
            uint8_t pin = RELAY1_PIN + (key - '1');
            BSP_Relay_Toggle(pin);
            Log_To_UART("RELAY TOGGLED");
        } else if (key == '#') {
            current_state = STATE_STANDBY;
            Log_To_UART("LOGOUT TO STANDBY");
        }
    }
}

void Refresh_Displays(void) {
    BSP_OLED_Clear();
    OLED_Printf(0, 0,  "== DASHBOARD ==");
    OLED_Printf(0, 25, "TEMP: %.1f C", citadel.temperature);
    OLED_Printf(0, 50, "STAT: %s", g_oled_status);
    BSP_OLED_Update();
}

void Log_To_UART(const char* event) {
    UART_Printf("[LOG] %s\r\n", event);
}

/* ISR Mapping */
void EXTI15_10_IRQHandler(void) {
    BSP_Button_IRQHandling();
}

void BSP_Button_Callback(void) {
    system_start_flag = 1;
}

void update_lcd_display(const char *line1, const char *line2) {
    BSP_LCD_SendCommand(LCD_CMD_CLEAR);
    BSP_Delay_ms(2); // HD44780 clear command needs extra time
    BSP_LCD_SetCursor(0, 0);
    BSP_LCD_PrintString((char*)line1);
    BSP_LCD_SetCursor(1, 0);
    BSP_LCD_PrintString((char*)line2);
}

