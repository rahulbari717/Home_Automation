# Functional Requirements Specification (FRS)
## Project: Home Automation (complete bare-metal without any library )

**Document Number:** FRS-Home Automation
**Version:** 1.0  
**Date:** January 24, 2026  
**Author:** Rahul Bari, Embedded Developer  
**Status:** Active Development  

---

## 📋 1.  Project Overview

This project implements a complete home automation controller using **STM32F446RE Nucleo-64** microcontroller. The system features a hierarchical finite state machine (FSM) for secure access control and automated device management—all without HAL or RTOS dependencies, using pure register-level drivers for maximum efficiency.

### 1.1 Development Constraint
- Zero-Library Policy: No HAL or RTOS dependencies. All functionality is built upon custom register-level drivers (GPIO, ADC, UART, etc.).

### 1.2 Architecture Stack
1. **Application Layer:** Finite State Machine (FSM) for Security and User Interface.
2. **BSP (Board Support Package):** Component-specific drivers (LCD, Keypad, Sensors).
3. **Driver Layer:** Low-level register access (GPIO, UART, ADC).

### 1.3 Target Hardware
* **MCU:** STM32F446RE Nucleo-64 (Cortex-M4F)
* **Memory:** 512KB Flash / 128KB SRAM

---

## 📊 2. Pin Mapping & Hardware Configuration

### PORT A (Sensors & Communication)
```
PA0  → LDR1 (ADC1_CH0)           - Light sensor 1
PA1  → LDR2 (ADC1_CH1)           - Light sensor 2
PA2  → USART2_TX                 - Debug console output
PA3  → USART2_RX                 - Debug console input
PA4  → Buzzer                    - Audio feedback
PA5  → LED Green                 - Success indicator
PA6  → LED Red                   - Error/warning indicator
PA7  → LED White                 - Status/ambient light

```

### PORT B (Keypad, I2C, Relays)
```
PB0  → Keypad Row 0              - Scan line (Output)
PB1  → Keypad Row 1              - Scan line (Output)
PB2  → Keypad Row 2              - Scan line (Output)
PB3  → Keypad Row 3              - Scan line (Output)
PB4  → Keypad Col 0              - Read line (Input + Pull-up)
PB5  → Keypad Col 1              - Read line (Input + Pull-up)
PB6  → Keypad Col 2              - Read line (Input + Pull-up)
PB7  → Keypad Col 3              - Read line (Input + Pull-up)
PB8  → I2C1_SCL                  - OLED clock
PB9  → I2C1_SDA                  - OLED data
PB12 → Relay 1                   - Living room light
PB13 → Relay 2                   - Kitchen exhaust
PB14 → Relay 3                   - Bedroom AC
PB15 → Relay 4                   - Garden sprinkler
```

### PORT C (LCD & Sensors)
```
PC0  → LCD RS                    - Register select
PC1  → LCD EN                    - Enable pulse
PC2  → LCD D4                    - Data bit 4
PC3  → LCD D5                    - Data bit 5
PC4  → LCD D6                    - Data bit 6
PC5  → LCD D7                    - Data bit 7
PC6  → IR Sensor 1               - Obstacle detection
PC8  → IR Sensor 2               - Obstacle detection
PC13 → Wakeup Button             - System wakeup (EXTI)
```

---

## 3. Functional Requirements

### 3.1 Security & Access (The Gatekeeper)
* **PIN Authentication:** Users must enter a 4-digit code.
* **Lockout Logic:** System triggers a 30-second lockout after 3 failed attempts.
* **Intrusion Logic:** While in **LOCKED** state, triggering any proximity sensor (IR) activates the buzzer and logs an alert.

### 3.2 Automation & UI (The Commander)
* **Menu System:** A hierarchical FSM navigated via keypad:
  * `2` (Up) | `8` (Down) | `5` (Select) | `*` (Back) | `#` (Logout). 

### 3.3 🧠 System State Machine (FSM) Modes

### 3.3.1 STANDBY Mode (System Idle)
This is the default low-power state where the system waits for user engagement.

- **Visual Indicator**
  - White LED (PA7) blinks with a pattern of **1s ON / 3s OFF**

- **Transition Out**
  - Triggered by a **Falling Edge Interrupt** on the Wakeup Button (PC13)

- **Intrusion Logic**
  - If IR sensors detect movement:
    - Buzzer (PA4) is activated
    - Alert is logged via **USART2**

---

### 3.3.2 AUTHENTICATION Mode (Security Gate)
Once awakened, the system verifies the user's identity.

- **Visual Indicator**
  - Green LED (PA5) turns **ON** all others leds off

- **User Interface**
  - LCD / UART displays: **"Enter 4-Digit PIN:"**
  - System waits for **4 keypad inputs**

- **Logic**
  - Correct PIN → **ACTIVE Mode**
  - 3 wrong attempts → **FAILURE / LOCKOUT Mode**

---

### 3.3.3 ACTIVE Mode (The Commander)
User has full control over home automation peripherals.

- **Visual Indicator**
  - Green LED remains **ON**

- **Menu Navigation**
  - `2` Up | `8` Down | `5` Select | `*` Back
  - Controls Relays (PB12–PB15)

- **Transition Out**
  - `#` Logout → **STANDBY Mode**

- **to do** 
  - 1) ir1, ir2 ==> relay1 , relay2 control 
  - 2) ldr1, ldr2 ==> relay3 , relay4 control 
  - 3) all 4 relay control (on/off)
  - 4) ldr1, ldr2 values monitor
---

### 3.3.4 FAILURE / STANDBY Mode (Security Breach)

- **Indicators**
  - Red LED (PA6) + Buzzer (PA4)
  - **1s ON / 1s OFF**, repeated **3 times**

- **Duration**
  - 30 seconds (production)
  - 3 seconds (testing)

### 3.3.5 ERROR mode 

---

## 4 Power-ON Flow

## 🧪 Testing Checklist:

**Test in this exact order:**

1. ✅ **Power-ON** → White LED should blink (1s ON, 3s OFF)
2. ✅ **Press Button (PC13)** → Should transition to AUTH, Green LED ON
3. ✅ **Enter PIN "1234" on keypad** → Should show menu
4. ✅ **Use keys 2/8 to navigate, 5 to select** → Menu should respond
5. ✅ **Press # to logout** → Return to STANDBY
6. ✅ **Enter wrong PIN 3 times** → STANDBY mode, Red LED + Buzzer for 3s

---

```
1. SystemInit() - Clock setup (HSE/PLL) ==> all full speed high speed clk 
Get the core working: STANDBY → AUTH → ACTIVE → STANDBY

```

---

**END OF REQUIREMENTS**
