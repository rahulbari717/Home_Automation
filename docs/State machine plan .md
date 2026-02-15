# State Machine Implementation Plan

## 1. STATE MACHINE OVERVIEW

### State Enumeration (Extended)
```c
typedef enum {
    STATE_STANDBY,           // White LED blinking, waiting for input
    STATE_AUTHENTICATING,    // Green LED ON, PIN entry
    STATE_ACTIVE_MENU,       // Menu navigation
    STATE_LOCKOUT,           // Red LED + Buzzer alarm (failed login)
    STATE_ERROR              // System error - auto-restart after timeout
} SystemState_t;

```

### Sub-State Enumeration for Menu Navigation
```c
typedef enum {
    SUB_MENU_MAIN,
    SUB_MENU_RELAYS,
    SUB_MENU_LEDS
} MenuState_t;
```

---

## 2. DISPLAY ALLOCATION

### OLED (128x64) - State & Status Display
**Purpose:** Show current system state and critical information

**Display Format:**
```
┌─────────────────────┐
│ STATE: STANDBY      │
│ User: [None]        │
└─────────────────────┘
```

**States to Display:**
- Current SystemState_t value
- Logged-in username
- System uptime/timestamp
- Temperature (DS18B20)
- Motion detected indicator
- Security status

### LCD (16x2) - Sensor Readings & Controls
**Purpose:** Interactive display for sensor data and device control

**Display Modes:**

**Mode 1: Sensor Dashboard**
```
LDR1:450 LDR2:320
Mot:YES
```

**Mode 2: Device Control**
```
>LED Green: ON
 Relay1: OFF
```

**Mode 3: Menu Navigation**
```
>Sensors
 Controls
```

---

## 3. DETAILED STATE MACHINE LOGIC

### STATE_STANDBY
**Entry Actions:**
- Turn off all LEDs except White LED (dim indicator)
- Clear LCD: Display "System Ready"
- OLED: Show "STATE: STANDBY"
- Start motion detection (RCWL-0516)

**Active Operations:**
- Monitor RCWL motion sensor
- Monitor IR proximity sensors
- Check for keypad input
- Check for Bluetooth commands
- Monitor wakeup button (PC13)

**Transition Conditions:**
- Motion detected OR Wakeup button → STATE_AUTHENTICATING
- Bluetooth command received → STATE_AUTHENTICATING

**UART Output:**
```
[STANDBY] System in standby mode. Waiting for user input...
[STANDBY] Motion: NO | IR1: NO | IR2: NO
```

---

### STATE_AUTHENTICATING
**Entry Actions:**
- Turn on White LED (full brightness)
- LCD: Display "Enter PIN:"
- OLED: Show "STATE: AUTH"
- Start login attempt counter

**Active Operations:**
- Read keypad input (4-digit PIN)
- Display asterisks on LCD for each digit
- Compare with stored user PINs
- Bluetooth PIN input option

**Transition Conditions:**
- Valid PIN → STATE_ACTIVE_MENU
- Invalid PIN (< 3 attempts) → Retry
- Invalid PIN (3 attempts) → STATE_LOCKOUT
- Timeout (30 sec) → STATE_STANDBY

**UART Output:**
```
[AUTH] User attempting login...
[AUTH] PIN entered: ****
[AUTH] Authentication successful! User: Rahul
```

---

### STATE_ACTIVE_MENU
**Entry Actions:**
- Turn on Green LED
- LCD: Display main menu
- OLED: Show "STATE: MENU" + username
- Buzzer: Short beep (success)

**Menu Structure:**
```
Main Menu:
1. Sensor Monitor
2. Device Controls
3. Settings
4. Logout
```

**Active Operations:**
- Keypad navigation (2=Up, 8=Down, 5=Enter, *=Back)
- Display menu options on LCD
- Update sensor readings in background
- UART command processing

**Transition Conditions:**
- Select "Sensor Monitor" → STATE_SENSOR_MONITOR
- Select "Device Controls" → STATE_CONTROL_DEVICES
- Select "Settings" → STATE_SETTINGS
- Press # OR Timeout → STATE_STANDBY

**UART Output:**
```
[MENU] User: Rahul | Menu: MAIN
[MENU] Option selected: Sensor Monitor
```

---

### STATE_SENSOR_MONITOR
**Entry Actions:**
- LCD: Clear and prepare for sensor display
- OLED: Show "STATE: SENSORS"
- Start continuous sensor reading

**Sensor Display Cycle (LCD):**

**Screen 1: Light Sensors**
```
LDR1: 450 (45%)
LDR2: 320 (32%)
```

**Screen 2: Temperature**
```
Temp: 25.3°C
Status: NORMAL
```

**Screen 3: Motion/Proximity**
```
Motion: DETECTED
IR1:Y  
```

**Active Operations:**
- Read LDR1 (PA0/ADC1_CH0) every 500ms
- Read LDR2 (PA1/ADC1_CH1) every 500ms
- Read IR1 (PC6) continuously
- Cycle through screens every 1 seconds
- Keypad: 2/8 for manual screen change

**Transition Conditions:**
- Press * → STATE_ACTIVE_MENU
- Press # → STATE_STANDBY

**UART Output (Continuous):**
```
[SENSOR] LDR1: 450  | LDR2: 320 
[SENSOR]  IR1: YES 

```

---

### STATE_CONTROL_DEVICES
**Entry Actions:**
- LCD: Display device control menu
- OLED: Show "STATE: CONTROL"
- Load current device states

**Control Menu Structure:**
```
1. LED Control
   - Green (PA5)
   - Red (PA6)
   - White (PA7)

2. Buzzer (PA4)

3. Relays
   - Relay1 (PB12)
   - Relay2 (PB13)
   - Relay3 (PB14)
   - Relay4 (PB15)

4. LDR Auto Mode

```

**LCD Display Format:**
```
>LED Green: ON
 LED Red: OFF
```

**Active Operations:**
- Navigate with keypad (2=Up, 8=Down)
- Toggle with 5 (Enter)
- Immediate UART feedback
- Update LCD status
- Visual/audio confirmation (buzzer beep)

**LDR Auto Mode Logic:**
```c
if (LDR1_value > 4000) {  // Dark
    Relay1_ON();  // Room light
}
if (LDR2_value > 4000) {
    Relay2_ON();   // lights on 
}
```

**Transition Conditions:**
- Press * → STATE_ACTIVE_MENU
- Press # → STATE_STANDBY

**UART Output:**
```
[CONTROL] LED Green: ON
[CONTROL] Relay1 activated
[CONTROL] Auto-mode enabled for LDR control
[CONTROL] LDR1=150 -> Lights ON (Auto)
```

---

### STATE_SETTINGS
**Entry Actions:**
- LCD: Display settings menu
- OLED: Show "STATE: SETTINGS"

**Settings Options:**
```
1. Change PIN
2. Add User
3. Remove User
4. Display Settings
   - Brightness
   - Update Rate
5. System Info
```

**Active Operations:**
- Navigate settings menu
- Modify user database
- Adjust system parameters
- Display system information

**UART Output:**
```
[SETTINGS] Current users: 3/5
[SETTINGS] PIN changed successfully
[SETTINGS] Display update rate: 500ms
```

---

### STATE_LOCKOUT
**Entry Actions:**
- Turn on Red LED (blinking)
- Buzzer: Continuous alarm (3 beeps)
- LCD: "LOCKED OUT!"
- OLED: "STATE: LOCKOUT"
- Disable all controls

**Active Operations:**
- Countdown timer (5 seconds)
- Ignore all keypad input
- Bluetooth emergency unlock option
- Log security event

**Transition Conditions:**
- Timer expires → STATE_STANDBY
- Master PIN via Bluetooth → STATE_STANDBY

**UART Output:**
```
[LOCKOUT] !!! SECURITY ALERT !!!
[LOCKOUT] Failed login attempts: 3
[LOCKOUT] System locked for 5 seconds
[LOCKOUT] Lockout timer: 4...3...2...1...
[LOCKOUT] System unlocked
```

---

## 4. SWITCH-CASE IMPLEMENTATION STRUCTURE

### Main State Machine
```c
void StateMachine_Run(void) {
    switch (currentState) {
        case STATE_STANDBY:
            Handle_Standby();
            break;
            
        case STATE_AUTHENTICATING:
            Handle_Authentication();
            break;
            
        case STATE_ACTIVE_MENU:
            Handle_ActiveMenu();
            break;
            
        case STATE_SENSOR_MONITOR:
            Handle_SensorMonitor();
            break;
            
        case STATE_CONTROL_DEVICES:
            Handle_ControlDevices();
            break;
            
        case STATE_SETTINGS:
            Handle_Settings();
            break;
            
        case STATE_LOCKOUT:
            Handle_Lockout();
            break;
            
        default:
            currentState = STATE_STANDBY;
            break;
    }
}
```

### Sub-Menu Handler Example
```c
void Handle_ControlDevices(void) {
    char key = Keypad_GetKey();
    
    switch (controlMenuState) {
        case MENU_LED_CONTROL:
            switch (key) {
                case KEY_ENTER:
                    Toggle_LED();
                    UART_SendString("[CONTROL] LED toggled\r\n");
                    LCD_Update();
                    break;
                case KEY_UP:
                    Navigate_Up();
                    break;
                case KEY_DOWN:
                    Navigate_Down();
                    break;
                case KEY_BACK:
                    controlMenuState = MENU_CONTROLS;
                    break;
            }
            break;
            
        case MENU_RELAY_CONTROL:
            // Similar structure
            break;
            
        case MENU_BUZZER_CONTROL:
            // Similar structure
            break;
    }
}
```

---

## 5. UART COMMUNICATION PROTOCOL

### Command Format
```
CMD:<command>,PARAM:<value>\r\n
```

### Supported Commands
```c
// Read sensors
CMD:READ,PARAM:LDR1
CMD:READ,PARAM:LDR2
CMD:READ,PARAM:TEMP
CMD:READ,PARAM:MOTION
CMD:READ,PARAM:ALL

// Control devices
CMD:SET,PARAM:LED_GREEN_ON
CMD:SET,PARAM:LED_GREEN_OFF
CMD:SET,PARAM:RELAY1_ON
CMD:SET,PARAM:BUZZER_BEEP

// System commands
CMD:AUTH,PARAM:1234
CMD:LOGOUT
CMD:STATUS
```

### Response Format
```c
// Success
OK:<data>\r\n

// Error
ERR:<error_message>\r\n

// Data stream
DATA:LDR1=450,LDR2=320,TEMP=25.3,MOTION=YES\r\n
```

### Continuous Monitoring Mode
```c
// Enable periodic updates
CMD:MONITOR,PARAM:START

// Response every 1 second
[MONITOR] LDR1:450 LDR2:320 TEMP:25.3 M:YES IR1:NO IR2:NO
[MONITOR] LED_G:ON LED_R:OFF R1:ON R2:OFF R3:OFF R4:OFF
```

---

## 6. SENSOR READING SCHEDULE

### Timing Table
| Sensor | Update Rate | Display |
|--------|-------------|---------|
| LDR1 (PA0) | 500ms | LCD + UART |
| LDR2 (PA1) | 500ms | LCD + UART |
| DS18B20 (PA8) | 5000ms | OLED + LCD + UART |
| RCWL (PB10) | Interrupt | All |
| IR1 (PC6) | 100ms | LCD + UART |
| IR2 (PC8) | 100ms | LCD + UART |

### Priority System
1. **Critical:** Motion detection (security)
2. **High:** Temperature alarms
3. **Medium:** LDR-based automation
4. **Low:** Display updates

---

## 7. LED INDICATOR LOGIC

### LED States
```c
// Standby mode
White LED: DIM (PWM 10%)

// Authenticating
White LED: FULL (100%)

// Active/Logged in
Green LED: ON

// Error/Warning
Red LED: ON or BLINKING


```

---

## 8. BUZZER PATTERNS

```c
typedef enum {
    BEEP_SUCCESS,      // 1 short beep (500ms)
    BEEP_ERROR,        // 3 short beeps (1000 ms each)
    BEEP_ALARM,        // 5 times Continuous (1s on, 0.5s off)
} BuzzerPattern_t;
```

**Usage:**
- Login success: BEEP_SUCCESS
- Invalid PIN: BEEP_ERROR
- Lockout: BEEP_ALARM
- Motion detected: BEEP_SUCCESS
- Device toggled: BEEP_SUCCESS

---

---

## 10. IMPLEMENTATION ROADMAP

### Phase 1: Core State Machine
- [ ] Implement basic state transitions
- [ ] Add UART debug output for each state
- [ ] Test STANDBY ↔ AUTHENTICATING ↔ ACTIVE_MENU

### Phase 2: Sensor Integration
- [ ] ADC configuration for LDR1, LDR2
- [ ] DS18B20 one-wire protocol
- [ ] GPIO interrupts for RCWL, IR sensors
- [ ] OLED display state information
- [ ] LCD display sensor readings

### Phase 3: Device Control
- [ ] LED control functions
- [ ] Relay control with safety checks
- [ ] Buzzer pattern generation
- [ ] LDR auto-mode implementation

### Phase 4: Menu System
- [ ] Keypad input handling
- [ ] Menu navigation logic
- [ ] LCD menu display
- [ ] Settings persistence (EEPROM/Flash)

### Phase 5: Communication
- [ ] UART command parser
- [ ] Bluetooth integration
- [ ] Continuous monitoring mode
- [ ] Emergency override commands

### Phase 6: Testing & Refinement
- [ ] State transition testing
- [ ] Sensor accuracy validation
- [ ] Timing verification
- [ ] Security testing
- [ ] User acceptance testing

---

## 11. EXAMPLE UART OUTPUT SESSION

```
[SYSTEM] STM32 Home Automation System v1.0
[SYSTEM] Initializing peripherals...
[SYSTEM] UART: OK | ADC: OK | I2C: OK | GPIO: OK
[STANDBY] System ready. Waiting for user...
[STANDBY] Motion: NO | IR1: NO | IR2: NO | Temp: 24.8°C

[MOTION DETECTED!]
[STANDBY] -> [AUTH] Transition triggered by motion sensor

[AUTH] Please enter PIN...
[AUTH] PIN: *
[AUTH] PIN: **
[AUTH] PIN: ***
[AUTH] PIN: ****
[AUTH] Verifying credentials...
[AUTH] Login successful! Welcome, Rahul

[AUTH] -> [MENU] User authenticated

[MENU] === Main Menu ===
[MENU] 1. Sensor Monitor
[MENU] 2. Device Controls
[MENU] 3. Settings
[MENU] 4. Logout
[MENU] Selection: Sensor Monitor

[MENU] -> [SENSOR] Entering sensor monitoring mode

[SENSOR] === Live Sensor Data ===
[SENSOR] LDR1: 450 (45%) | LDR2: 320 (32%)
[SENSOR] Temperature: 25.3°C | Status: NORMAL
[SENSOR] Motion: YES | IR1: NO | IR2: NO
[SENSOR] -------------------------
[SENSOR] LDR1: 448 (45%) | LDR2: 318 (32%)
[SENSOR] Temperature: 25.3°C | Status: NORMAL
[SENSOR] Motion: NO | IR1: YES | IR2: NO

[SENSOR] User pressed BACK
[SENSOR] -> [MENU] Returning to main menu

[MENU] Selection: Device Controls
[MENU] -> [CONTROL] Entering device control mode

[CONTROL] === Device Control ===
[CONTROL] LED Green: ON
[CONTROL] LED Red: OFF
[CONTROL] LED White: ON (Auto)
[CONTROL] Relay1: ON | Relay2: OFF | Relay3: OFF | Relay4: OFF
[CONTROL] User toggled Relay1
[CONTROL] Relay1: OFF
[CONTROL] Command executed successfully

[CONTROL] User pressed LOGOUT
[CONTROL] -> [STANDBY] User logged out

[STANDBY] System returned to standby mode
[STANDBY] Goodbye, Rahul!
```

---

## 12. ERROR HANDLING

### Error States
```c
typedef enum {
    ERR_NONE,
    ERR_SENSOR_TIMEOUT,
    ERR_ADC_FAIL,
    ERR_I2C_TIMEOUT,
    ERR_INVALID_USER,
    ERR_DEVICE_FAULT
} ErrorCode_t;
```

### Recovery Actions
- ADC failure → Retry 3 times, then use last known value
- I2C timeout → Re-initialize bus, log error
- Sensor timeout → Display "SENSOR ERR" on LCD
- Invalid state → Force return to STANDBY

---

## SUMMARY

This plan provides a complete state machine architecture with:
✅ Clear state definitions and transitions
✅ OLED for state display only
✅ LCD for sensor readings and device control
✅ Comprehensive UART monitoring
✅ Switch-case implementation structure
✅ Sensor integration schedule
✅ Device control logic
✅ Security features

**Next Steps:**
1. Review and approve this plan
2. Create .c/.h files for each module
3. Implement incrementally following the roadmap
4. Test each phase before moving forward