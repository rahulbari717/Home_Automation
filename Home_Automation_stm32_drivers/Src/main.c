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

#include "bsp_init.h"
#include "bsp_keypad.h"
#include "bsp_i2c_oled.h"
#include "bsp_lcd.h"
#include "bsp_adc_sensors.h"
#include "bsp_bluetooth.h"

/* ===== Global Variables ===== */
extern USART_Handle_t usart2_handle;
RTC_Handle_t rtc_handle;
SystemState_t current_state = STATE_STANDBY;
volatile uint8_t system_start_flag = 0;

// Sensor readings
typedef struct {
    float temperature_ds18b20;
    uint8_t temp_alarm_triggered;
    uint16_t ldr1_value;
    uint16_t ldr2_value;
    uint8_t motion_detected;
    uint32_t last_activity_time;
} SensorData_t;

SensorData_t sensors = {0};

// System statistics
typedef struct {
    uint32_t total_logins;
    uint32_t failed_attempts;
    uint32_t lockout_events;
    uint32_t uptime_seconds;
} SystemStats_t;

SystemStats_t stats = {0};

/* ===== Function Prototypes ===== */
void hardware_init(void);
void DS18B20_Init(void);
float DS18B20_ReadTemp(void);
void KY028_Init(void);
uint8_t KY028_CheckAlarm(void);

void update_oled_status(void);
void update_lcd_display(const char *line1, const char *line2);
void log_event(const char *event);

void handle_standby(void);
uint8_t handle_authentication(void);
void handle_active_menu(void);
void handle_lockout(void);

void rtc_setup_interactive(void);
void automation_logic(void);

void delay_ms(uint32_t ms);


/* ================= MAIN APPLICATION ================= */
int main(void) {
    hardware_init();
    
    log_event("SYSTEM BOOT COMPLETE");
    log_event("The Citadel Control Center v2.0");
    
    // Display splash screen
    BSP_OLED_Clear();
    OLED_Printf(15, 10, "THE CITADEL");
    OLED_Printf(5, 30, "Control Center v2");
    OLED_Printf(20, 50, "Initializing...");
    BSP_OLED_Update();
    
    update_lcd_display("THE CITADEL", "Booting System..");
    delay_ms(2000);
    
    // RTC Setup
    rtc_setup_interactive();
    
    uint32_t last_temp_read = 0;
    uint32_t last_oled_update = 0;
    uint32_t last_automation_check = 0;
    uint32_t current_tick = 0;
    
    log_event("ENTERING MAIN CONTROL LOOP");
    
    while(1) {
        current_tick = stats.uptime_seconds * 1000; // Simplified tick counter
        
        // Periodic temperature reading
        if(current_tick - last_temp_read >= TEMP_UPDATE_INTERVAL) {
            sensors.temperature_ds18b20 = DS18B20_ReadTemp();
            sensors.temp_alarm_triggered = KY028_CheckAlarm();
            
            if(sensors.temp_alarm_triggered) {
                log_event("ALERT: Temperature threshold exceeded!");
                GPIO_WriteToOutputPin(BUZZER_PORT, BUZZER_PIN, GPIO_PIN_SET);
                delay_ms(200);
                GPIO_WriteToOutputPin(BUZZER_PORT, BUZZER_PIN, GPIO_PIN_RESET);
            }
            
            last_temp_read = current_tick;
        }
        
        // Periodic automation check
        if(current_tick - last_automation_check >= 1000) {
            automation_logic();
            last_automation_check = current_tick;
            stats.uptime_seconds++;
        }
        
        // State machine
        switch(current_state) {
            case STATE_STANDBY:
                handle_standby();
                if(system_start_flag) {
                    system_start_flag = 0;
                    log_event("WAKEUP: User initiated authentication");
                    current_state = STATE_AUTHENTICATING;
                }
                
                // Update OLED in standby
                if(current_tick - last_oled_update >= OLED_UPDATE_INTERVAL) {
                    update_oled_status();
                    last_oled_update = current_tick;
                }
                break;
                
            case STATE_AUTHENTICATING:
                GPIO_WriteToOutputPin(LED_PORT, LED_GREEN_PIN, GPIO_PIN_SET);
                update_lcd_display("AUTHENTICATION", "Enter PIN:");
                
                if(handle_authentication()) {
                    stats.total_logins++;
                    log_event("AUTH SUCCESS: User authenticated");
                    current_state = STATE_ACTIVE_MENU;
                } else {
                    stats.failed_attempts++;
                    stats.lockout_events++;
                    log_event("AUTH FAILED: Invalid credentials");
                    current_state = STATE_LOCKOUT;
                }
                break;
                
            case STATE_ACTIVE_MENU:
                update_oled_status();
                handle_active_menu();
                log_event("SESSION END: User logged out");
                current_state = STATE_STANDBY;
                break;
                
            case STATE_LOCKOUT:
                handle_lockout();
                current_state = STATE_STANDBY;
                break;
        }
    }
}

/* ================= HARDWARE INITIALIZATION ================= */
void hardware_init(void) {
    // Core system init
    app_init();
    TIMER_DelayInit();
    
    // Display initialization
    BSP_OLED_Init();
    BSP_LCD_Init();
    
    // Sensor initialization
    DS18B20_Init();
    KY028_Init();
    
    // Clear displays
    BSP_OLED_Clear();
    BSP_OLED_Update();
    BSP_LCD_SendCommand(LCD_CMD_CLEAR);
}

/* ================= DS18B20 FUNCTIONS ================= */
void DS18B20_Init(void) {
    // Configure PC10 as open-drain output with pull-up
    GPIO_Handle_t ds18b20_pin;
    ds18b20_pin.pGPIOx = DS18B20_PORT;
    ds18b20_pin.GPIO_PinConfig.GPIO_PinNumber = DS18B20_PIN;
    ds18b20_pin.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
    ds18b20_pin.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    ds18b20_pin.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD;
    ds18b20_pin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;
    GPIO_Init(&ds18b20_pin);
    
    log_event("DS18B20 Initialized on PC10");
}

float DS18B20_ReadTemp(void) {
    // Simplified 1-Wire communication (implement full protocol in production)
    // This is a placeholder - full implementation requires precise timing
    
    // For now, return simulated value
    // TODO: Implement complete 1-Wire protocol:
    // 1. Reset pulse
    // 2. Presence detect
    // 3. Skip ROM command (0xCC)
    // 4. Convert T command (0x44)
    // 5. Read scratchpad (0xBE)
    
    static float simulated_temp = 25.0f;
    simulated_temp += (float)(rand() % 20 - 10) / 10.0f; // ±1°C variation
    
    if(simulated_temp < 20.0f) simulated_temp = 20.0f;
    if(simulated_temp > 30.0f) simulated_temp = 30.0f;
    
    return simulated_temp;
}

/* ================= KY-028 FUNCTIONS ================= */
void KY028_Init(void) {
    // Digital output from KY-028 comparator
    GPIO_Handle_t ky028_pin;
    ky028_pin.pGPIOx = KY028_DIGITAL_PORT;
    ky028_pin.GPIO_PinConfig.GPIO_PinNumber = KY028_DIGITAL_PIN;
    ky028_pin.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
    ky028_pin.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    ky028_pin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;
    GPIO_Init(&ky028_pin);
    
    log_event("KY-028 Temperature Alarm Initialized on PC11");
}

uint8_t KY028_CheckAlarm(void) {
    // Digital pin goes LOW when temperature exceeds threshold
    return !GPIO_ReadFromInputPin(KY028_DIGITAL_PORT, KY028_DIGITAL_PIN);
}

/* ================= DISPLAY UPDATE FUNCTIONS ================= */
void update_oled_status(void) {
    uint8_t h, m, s, day, month, year, weekday;
    char temp_str[20];
    char status_str[20];
    
    // Read RTC
    RTC_GetTime(&rtc_handle, &h, &m, &s);
    RTC_GetDate(&rtc_handle, &year, &month, &day, &weekday);
    
    // Clear and draw new content
    BSP_OLED_Clear();
    
    // Header
    OLED_Printf(0, 0, "== CITADEL v2 ==");
    
    // Time
    OLED_Printf(25, 15, "%02d:%02d:%02d", h, m, s);
    
    // Date
    OLED_Printf(15, 25, "%02d/%02d/20%02d", day, month, year);
    
    // Temperature
    sprintf(temp_str, "Temp: %.1fC", sensors.temperature_ds18b20);
    OLED_Printf(5, 38, temp_str);
    
    // System status
    switch(current_state) {
        case STATE_STANDBY:
            strcpy(status_str, "Status: STANDBY");
            break;
        case STATE_AUTHENTICATING:
            strcpy(status_str, "Status: AUTH...");
            break;
        case STATE_ACTIVE_MENU:
            strcpy(status_str, "Status: ACTIVE");
            break;
        case STATE_LOCKOUT:
            strcpy(status_str, "Status: LOCKED");
            break;
    }
    OLED_Printf(0, 50, status_str);
    
    // Alarm indicator
    if(sensors.temp_alarm_triggered) {
        OLED_Printf(100, 50, "[!]");
    }
    
    BSP_OLED_Update();
}

void update_lcd_display(const char *line1, const char *line2) {
    BSP_LCD_SetCursor(0, 0);
    BSP_LCD_PrintString((char*)line1);
    BSP_LCD_SetCursor(1, 0);
    BSP_LCD_PrintString((char*)line2);
}

void log_event(const char *event) {
    uint8_t h, m, s;
    RTC_GetTime(&rtc_handle, &h, &m, &s);
    
    UART_Printf("[%02d:%02d:%02d] %s\r\n", h, m, s, event);
}

/* ================= STATE HANDLERS ================= */
void handle_standby(void) {
    // White LED blinks: 1s ON, 3s OFF
    GPIO_WriteToOutputPin(LED_PORT, LED_WHITE_PIN, GPIO_PIN_SET);
    delay_ms(1000);
    GPIO_WriteToOutputPin(LED_PORT, LED_WHITE_PIN, GPIO_PIN_RESET);
    
    // Check for button press during off period
    for(int i = 0; i < 30; i++) {
        if(system_start_flag) return;
        delay_ms(100);
    }
}

uint8_t handle_authentication(void) {
    char input_pin[PIN_LENGTH + 1];
    uint8_t attempt_count = 0;
    
    UART_Printf("\r\n>> AUTHENTICATION REQUIRED\r\n");
    update_lcd_display("ENTER PIN:", "Attempt 1/3");
    
    while(attempt_count < MAX_LOGIN_ATTEMPTS) {
        uint8_t char_count = 0;
        UART_Printf("Enter PIN (Attempt %d/%d): ", attempt_count + 1, MAX_LOGIN_ATTEMPTS);
        
        // Update LCD
        char lcd_line[17];
        sprintf(lcd_line, "Attempt %d/3", attempt_count + 1);
        update_lcd_display("ENTER PIN:", lcd_line);
        
        while(char_count < PIN_LENGTH) {
            char key = Keypad_GetKey();
            if(key != KEYPAD_NO_KEY && key >= '0' && key <= '9') {
                input_pin[char_count++] = key;
                UART_Printf("*");
                
                // Show asterisks on LCD
                BSP_LCD_SetCursor(1, char_count - 1);
                BSP_LCD_SendData('*');
            }
        }
        input_pin[PIN_LENGTH] = '\0';
        
        if(strcmp(input_pin, MASTER_PIN) == 0) {
            UART_Printf("\r\n>> ACCESS GRANTED\r\n");
            update_lcd_display("ACCESS GRANTED", "Welcome!");
            delay_ms(1000);
            return 1;
        } else {
            attempt_count++;
            UART_Printf("\r\n>> INVALID PIN\r\n");
            update_lcd_display("INVALID PIN", "Try again");
            delay_ms(1000);
        }
    }
    
    return 0;
}

void handle_active_menu(void) {
    char menu_buffer[17];
    
    UART_Printf("\r\n=== CITADEL CONTROL MENU ===\r\n");
    UART_Printf("1-4: Toggle Relays\r\n");
    UART_Printf("5: View Sensors\r\n");
    UART_Printf("6: System Stats\r\n");
    UART_Printf("7: Temperature Log\r\n");
    UART_Printf("#: Logout\r\n\r\n");
    
    update_lcd_display("MAIN MENU", "1-7:Opt #:Logout");
    
    while(1) {
        char key = Keypad_GetKey();
        
        if(key != KEYPAD_NO_KEY) {
            if(key == '#') {
                UART_Printf(">> Logging out...\r\n");
                GPIO_WriteToOutputPin(LED_PORT, LED_GREEN_PIN, GPIO_PIN_RESET);
                update_lcd_display("LOGGING OUT", "Goodbye!");
                delay_ms(1000);
                return;
                
            } else if(key >= '1' && key <= '4') {
                // Toggle relay
                uint8_t relay_pin = GPIO_PIN_NO_12 + (key - '1');
                GPIO_ToggleOutputPin(RELAY_PORT, relay_pin);
                
                sprintf(menu_buffer, "Relay %c Toggled", key);
                UART_Printf(">> %s\r\n", menu_buffer);
                update_lcd_display("RELAY CONTROL", menu_buffer);
                log_event(menu_buffer);
                delay_ms(500);
                
            } else if(key == '5') {
                // Sensor view
                UART_Printf("\r\n=== SENSOR READINGS ===\r\n");
                UART_Printf("Temperature: %.1f C\r\n", sensors.temperature_ds18b20);
                UART_Printf("LDR1: %d\r\n", sensors.ldr1_value);
                UART_Printf("LDR2: %d\r\n", sensors.ldr2_value);
                UART_Printf("Temp Alarm: %s\r\n", sensors.temp_alarm_triggered ? "ACTIVE" : "OK");
                
                sprintf(menu_buffer, "Temp: %.1fC", sensors.temperature_ds18b20);
                update_lcd_display("SENSOR VIEW", menu_buffer);
                delay_ms(3000);
                
            } else if(key == '6') {
                // System stats
                UART_Printf("\r\n=== SYSTEM STATISTICS ===\r\n");
                UART_Printf("Total Logins: %lu\r\n", stats.total_logins);
                UART_Printf("Failed Attempts: %lu\r\n", stats.failed_attempts);
                UART_Printf("Lockout Events: %lu\r\n", stats.lockout_events);
                UART_Printf("Uptime: %lu sec\r\n", stats.uptime_seconds);
                
                sprintf(menu_buffer, "Logins: %lu", stats.total_logins);
                update_lcd_display("SYSTEM STATS", menu_buffer);
                delay_ms(3000);
                
            } else if(key == '7') {
                // Temperature log
                update_lcd_display("TEMP LOG", "See UART Output");
                UART_Printf("\r\n=== TEMPERATURE LOG ===\r\n");
                UART_Printf("Current: %.1f C\r\n", sensors.temperature_ds18b20);
                UART_Printf("Alarm Threshold: %d C\r\n", TEMP_ALARM_THRESHOLD);
                UART_Printf("Alarm Status: %s\r\n", sensors.temp_alarm_triggered ? "TRIGGERED" : "NORMAL");
                delay_ms(3000);
            }
            
            // Return to main menu display
            update_lcd_display("MAIN MENU", "1-7:Opt #:Logout");
        }
        
        // Update OLED status while in menu
        static uint32_t last_status_update = 0;
        if(stats.uptime_seconds - last_status_update >= 2) {
            update_oled_status();
            last_status_update = stats.uptime_seconds;
        }
    }
}

void handle_lockout(void) {
    UART_Printf(">> [SECURITY LOCKOUT] Too many failed attempts!\r\n");
    update_lcd_display("SECURITY LOCKOUT", "Wait 5 seconds");
    
    GPIO_WriteToOutputPin(LED_PORT, LED_GREEN_PIN, GPIO_PIN_RESET);
    
    for(int i = 0; i < 5; i++) {
        GPIO_WriteToOutputPin(LED_PORT, LED_RED_PIN, GPIO_PIN_SET);
        GPIO_WriteToOutputPin(BUZZER_PORT, BUZZER_PIN, GPIO_PIN_SET);
        delay_ms(500);
        
        GPIO_WriteToOutputPin(LED_PORT, LED_RED_PIN, GPIO_PIN_RESET);
        GPIO_WriteToOutputPin(BUZZER_PORT, BUZZER_PIN, GPIO_PIN_RESET);
        delay_ms(500);
    }
    
    update_lcd_display("LOCKOUT END", "System Reset");
    delay_ms(1000);
}

/* ================= AUTOMATION LOGIC ================= */
void automation_logic(void) {
    // Read LDR sensors
    sensors.ldr1_value = BSP_Sensor_ReadLDR(SENSOR_LDR1_CHANNEL);
    sensors.ldr2_value = BSP_Sensor_ReadLDR(SENSOR_LDR2_CHANNEL);
    
    // Auto light control based on LDR
    if(sensors.ldr1_value > 3500) {
        // Dark - turn on relay 1
        GPIO_WriteToOutputPin(RELAY_PORT, GPIO_PIN_NO_12, GPIO_PIN_SET);
    } else {
        GPIO_WriteToOutputPin(RELAY_PORT, GPIO_PIN_NO_12, GPIO_PIN_RESET);
    }
}

/* ================= RTC SETUP ================= */
void rtc_setup_interactive(void) {
    rtc_handle.pRTC = RTC;
    RTC_Init(&rtc_handle);
    
    UART_Printf("\r\n=== RTC SETUP ===\r\n");
    UART_Printf("Using default time: 10:30:00, 25/01/2026\r\n");
    
    // Set default time
    RTC_SetTime(&rtc_handle, 10, 30, 0);
    RTC_SetDate(&rtc_handle, 26, 1, 25, 7); // Sunday
    
    update_lcd_display("RTC CONFIGURED", "Time: 10:30:00");
    delay_ms(2000);
}

/* ================= INTERRUPT HANDLERS ================= */
void EXTI15_10_IRQHandler(void) {
    // Debounce
    for(volatile uint32_t i = 0; i < 100000; i++);
    
    if(GPIO_ReadFromInputPin(WAKEUP_BTN_PORT, WAKEUP_BTN_PIN) == 0) {
        system_start_flag = 1;
    }
    
    GPIO_IRQHandling(WAKEUP_BTN_PIN);
}

/* ================= HELPER FUNCTIONS ================= */
void delay_ms(uint32_t ms) {
    for(uint32_t i = 0; i < (ms * 1600); i++);
}
