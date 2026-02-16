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

## 2. Functional Requirements

### 2.1 Security & Access (The Gatekeeper)
* **PIN Authentication:** Users must enter a 4-digit code.
* **Lockout Logic:** System triggers a 30-second lockout after 3 failed attempts.
* **Intrusion Logic:** While in **LOCKED** state, triggering any proximity sensor (IR) activates the buzzer and logs an alert.

### 2.2 Automation & UI (The Commander)
* **Menu System:** A hierarchical FSM navigated via keypad:
  * `2` (Up) | `8` (Down) | `5` (Select) | `*` (Back) | `#` (Logout). 

### 2.3 🧠 System State Machine (FSM) Modes

### 2.3.1 STANDBY Mode (System Idle)
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

### 2.3.2 AUTHENTICATION Mode (Security Gate)
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

### 2.3.3 ACTIVE Mode (The Commander)
User has full control over home automation peripherals.

- **Visual Indicator**
  - Green LED remains **ON**

- **Menu Navigation**
  - `2` Up | `8` Down | `5` Select | `*` Back
  - Controls Relays (PB12–PB15)

- **Transition Out**
  - `#` Logout → **STANDBY Mode**

---

### 2.3.4 FAILURE / STANDBY Mode (Security Breach)

- **Indicators**
  - Red LED (PA6) + Buzzer (PA4)
  - **1s ON / 1s OFF**, repeated **3 times**

- **Duration**
  - 30 seconds (production)
  - 3 seconds (testing)

---

**END OF REQUIREMENTS**
