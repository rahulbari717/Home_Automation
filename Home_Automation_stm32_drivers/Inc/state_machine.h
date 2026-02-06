/*
 * state_machine.h
 *
 * Created on: Jan 28, 2026
 * Author: Rahul B.
 * Description: State Machine Header - System States and Data Structures
 */

#ifndef STATE_MACHINE_H_
#define STATE_MACHINE_H_

#include "stm32f446xx.h"
#include "config.h"
#include <stdint.h>
#include <stdbool.h>

/* ===== ERROR RECOVERY CONFIGURATION ===== */
#define ERROR_RECOVERY_TIME_SEC     10   // wait for 10 sec Time before auto-restart
#define ERROR_BLINK_INTERVAL_MS     500 // red led Fast blink for error indication

/* ===== LDR THRESHOLDS ===== */
#define LDR_DARK_THRESHOLD      4000     // above this = Dark
#define LDR_BRIGHT_THRESHOLD    3000    // below this = Bright

/* ===== STATE MACHINE ENUMERATIONS ===== */


/* ===== BUZZER PATTERNS ===== */
typedef enum {
    BEEP_SUCCESS,            // 1 short beep (200ms)
    BEEP_ERROR,              // 2 short beeps (100ms each)
    BEEP_WARNING,            // 3 short beeps
    BEEP_ALARM,              // Continuous alarm
    BEEP_MOTION              // Single long beep (500ms)
} BuzzerPattern_t;

/* ===== MENU AND CONTROL ITEMS (Simplified) ===== */
typedef enum {
    MENU_MAIN,
    MENU_SENSORS,
    MENU_CONTROLS,
    MENU_SETTINGS
} MenuState_t;

typedef enum {
    SENSOR_SCREEN_LDR,
    SENSOR_SCREEN_MOTION
} SensorScreen_t;

typedef enum {
    CONTROL_LED_GREEN,
    CONTROL_LED_RED,
    CONTROL_LED_WHITE,
    CONTROL_RELAY1,
    CONTROL_RELAY2,
    CONTROL_RELAY3,
    CONTROL_RELAY4,
    CONTROL_BUZZER,
    CONTROL_LDR_AUTO,
    CONTROL_MENU_MAX
} ControlMenuItem_t;

/**
 * @brief Sensor Data Storage
 */
typedef struct {
    uint16_t ldr1_value;        // LDR1 raw ADC value (0-4095)
    uint16_t ldr2_value;        // LDR2 raw ADC value (0-4095)
    bool ir1_detected;          // IR1 sensor state
    bool ir2_detected;          // IR2 sensor state
    uint32_t lastUpdateTime;    // Timestamp of last update
} SensorData_t;

/**
 * @brief Device State Storage
 */
typedef struct {
    bool led_green;
    bool led_red;
    bool led_white;
    bool relay1;
    bool relay2;
    bool relay3;
    bool relay4;
    bool buzzer_active;
    bool ldr_auto_mode;         // Auto control based on LDR
} DeviceStates_t;

/**
 * @brief Main System States
 */
typedef enum {
    STATE_STANDBY,          // System idle, White LED 1s ON / 3s OFF
    STATE_AUTHENTICATING,   // User login process, Green LED ON
    STATE_ACTIVE_MENU,      // Main menu navigation
    STATE_SENSOR_MONITOR,   // Monitoring LDR values
    STATE_CONTROL_DEVICES,  // Manual Relay control
    STATE_SETTINGS,         // Configuration
    STATE_LOCKOUT,          // 30s security lockout
    STATE_ERROR             // System error mode
} SystemState_t;

/* ===== ERROR TYPES ===== */
typedef enum {
    ERROR_NONE = 0,
    ERROR_SENSOR_FAULT,         // Sensor read failure
    ERROR_DISPLAY_FAULT,        // LCD/OLED communication failure
    ERROR_KEYPAD_FAULT,         // Keypad stuck or not responding
    ERROR_MEMORY_FAULT,         // Memory corruption
    ERROR_WATCHDOG_RESET,       // IWDG triggered
    ERROR_HARD_FAULT,           // MCU fault handler triggered
    ERROR_UNKNOWN               // Unclassified error
} SystemError_t;

/* ===== UPDATED SYSTEM CONTEXT ===== */
/**
 * @brief System Context - Complete State Information
 */
typedef struct {
    SystemState_t currentState;
    SystemState_t previousState;

    MenuState_t currentMenu;
    SensorScreen_t currentSensorScreen;
    ControlMenuItem_t currentControlItem;

    char currentUser[USERNAME_LENGTH];
    char pinBuffer[PIN_LENGTH + 1];
    uint8_t pinIndex;
    uint8_t loginAttempts;
    bool isAuthenticated;

    uint32_t lastActivityTime;
    uint32_t lockoutEndTime;
    uint32_t errorRecoveryTime;
    uint32_t lastSensorUpdate;
    uint32_t lastLCDUpdate;
    uint32_t lastOLEDUpdate;

    SystemError_t lastError;
    uint8_t errorCount;
    uint8_t menuCursor;
} SystemContext_t;

/* ===== GLOBAL VARIABLES (Extern Declarations) ===== */
extern SystemContext_t g_SystemContext;
extern SensorData_t g_SensorData;
extern DeviceStates_t g_DeviceStates;

/* ===== FUNCTION PROTOTYPES ===== */

/* Core State Machine */
void StateMachine_Init(void);
void StateMachine_Run(void);
void StateMachine_SetState(SystemState_t newState);

/* State Handlers */
void Handle_Standby(void);
void Handle_Authentication(void);
void Handle_ActiveMenu(void);
void Handle_Error(void);          // NEW: Error state handler
void Handle_Lockout(void);

// Error management
void System_ReportError(SystemError_t error);  // NEW: Report error and enter ERROR state
void System_ClearError(void);                  // NEW: Clear error and restart
bool System_CheckHealth(void);                 // NEW: System health check

/* Sensor Functions */
bool Sensors_Update(void);
void Sensors_DisplayOnLCD(void);
void Sensors_DisplayOnOLED(void);
void Sensors_SendUART(void);

/* Device Control Functions */
void Device_ToggleLED(uint8_t pin);
void Device_ToggleRelay(uint8_t pin);
void Device_PlayBuzzer(BuzzerPattern_t pattern);
void Device_UpdateLDRAutoMode(void);
void Device_SendStatusUART(void);

/* Menu Functions */
void Menu_Display(void);
void Menu_ProcessSelection(void);

/* Display Functions */
void Display_UpdateOLED(void);
void Display_UpdateLCD(void);
void Display_ClearAll(void);

/* Authentication Functions */
void Auth_ProcessKeyPress(char key);
bool Auth_VerifyPIN(const char* pin);
void Auth_Reset(void);

/* Utility Functions */
uint32_t GetSystemTick(void);
bool CheckTimeout(uint32_t lastTime, uint32_t interval);
uint8_t LDR_ToPercentage(uint16_t raw_value);

/* Helper for peripheral test */
void update_lcd_display(const char *line1, const char *line2);

void Run_Complete_Peripheral_Test(void);

#endif /* STATE_MACHINE_H_ */
