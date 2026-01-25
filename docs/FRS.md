# Functional Requirements Specification (FRS)
## Project: The Citadel — Bare-Metal Home Security & Automation

**Document Number:** FRS-CITADEL-001  
**Version:** 3.0  
**Date:** January 24, 2026  
**Author:** Rahul Bari, Embedded Developer  
**Status:** Active Development  
**Classification:** Internal Use

---

## 1. Project Overview
The Citadel is a high-performance, bare-metal embedded controller designed for home security and automation. To ensure maximum efficiency and deep hardware understanding, the system bypasses HAL and RTOS, utilizing pure register-level drivers.

### 1.1 Architecture Stack
1. **Application Layer:** Finite State Machine (FSM) for Security and User Interface.
2. **BSP (Board Support Package):** Component-specific drivers (LCD, Keypad, Sensors).
3. **Driver Layer:** Low-level register access (GPIO, UART, ADC, IWDG).



### 1.2 Target Hardware
* **MCU:** STM32F446RE Nucleo-64 (Cortex-M4F)
* **Clock:** 84MHz (HSE 8MHz → PLL)
* **Memory:** 512KB Flash / 128KB SRAM

---

## 2. Hardware Mapping & Status

| Category | Component | Pins | Status |
| :--- | :--- | :--- | :--- |
| **Input** | 4x4 Matrix Keypad | PB0 - PB7 | ✅ Working |
| | User Button | PC13 | ✅ Working |
| **Display** | LCD 16x2 | PC0 - PC5 | ✅ Working |
| | OLED 128x64 | PB8, PB9 (I2C1) | ✅ Working |
| **Sensors** | LDR (Light Sensors) | PA0, PA1 (ADC1) | ✅ Working |
| | PIR / IR / RCWL | PC6 - PC10 | 🛑 Pending |
| | HC-SR04 (Ultrasonic) | PC11, PC12 | 🛑 Pending |
| **Output** | Relays (x4) | PB10, PB12 - PB14| 🛑 Pending |
| | Buzzer / LEDs | PA4 - PA6 | 🚧 In Progress |
| **Comm** | Bluetooth (HC-05) | PA9, PA10 (UART1) | 🛑 Pending |
| | Debug Console | PA2, PA3 (UART2) | ✅ Working |

---

## 3. Functional Requirements

### 3.1 Security & Access (The Gatekeeper)
* **PIN Authentication:** Users must enter a 4-digit code.
* **Lockout Logic:** System triggers a 60-second lockout after 3 failed attempts.
* **Intrusion Logic:** While in **LOCKED** state, triggering any motion sensor (PIR/RCWL) activates the buzzer and logs an alert.

### 3.2 Automation & UI (The Commander)
* **Menu System:** A hierarchical FSM navigated via keypad:
  * `2` (Up) | `8` (Down) | `5` (Select) | `0` (Back) | `#` (Logout).
* **Relay Mapping:**
  1. Living Room Light (PB10)
  2. Kitchen Exhaust (PB12)
  3. Bedroom AC (PB13)
  4. Garden Sprinkler (PB14)

### 3.3 Environmental Monitoring
* **Auto-Lighting:** If LDR values fall below the threshold (Night), activate Night Light.
* **Thermal Monitoring:** Read internal MCU temperature via ADC; display warning on LCD if > 40°C.

### 3.4 Remote Control (Bluetooth)
* Serial control via HC-05 module using a command-response protocol:
  * `AUTH <PIN>`
  * `RELAY <ID> <ON/OFF>`
  * `STATUS` (Returns system state and sensor data)

---

## 4. Technical Specifications (NFR)

| ID | Category | Requirement |
| :--- | :--- | :--- |
| **NFR-01** | Latency | Keypad/LCD UI response < 100ms. |
| **NFR-02** | Reliability | Watchdog (IWDG) reset if system hangs for > 2s. |
| **NFR-03** | Communication | UART Debug @ 115200 bps; Bluetooth @ 9600 bps. |
| **NFR-04** | Safety | All relays MUST default to OFF during logout or reset. |

---

## 5. Development Roadmap
1. **Drivers:** Complete register-level modules for Timers and PWM. ✅
2. **BSP:** Develop `bsp_bluetooth.c` and `bsp_relay.c`. 🚧
3. **Application:** Build the Menu FSM and PIN Authentication logic. 🛑
4. **Integration:** Final system testing and "The Citadel" main loop optimization. 🛑

---
**END OF REQUIREMENTS**