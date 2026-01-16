# Functional Requirements Specification (FRS): Home Automation 

**Version:** 2.0  
**Date:** January 2026  
**Author:** Rahul Bari.  

---

## 1. Document Overview

### 1.1 Purpose
This document defines the functional requirements for "The Citadel," a bare-metal embedded security and automation system. It aligns with the firmware architecture designed to demonstrate register-level driver mastery.

### 1.2 Scope
The Citadel is a **centralized embedded controller** (STM32F446RE) responsible for:
- **Access Control:** User authentication via PIN and Keypad.
- **Home Automation:** Control of appliances (simulated via Relays/LEDs).
- **System Monitoring:** Real-time sensor tracking (Temperature, Gas, Light).
- **Safety:** Local alarm handling and fault logging.

---

## 2. System Modes (The Core Concept)

The system operates in two distinct Finite State Machine (FSM) modes:

### 2.1 "The Gatekeeper" (Security Mode)
The default state. The system is defensive.
- **Input:** 4x4 Matrix Keypad.
- **Display:** 16x2 LCD (Shows "LOCKED").
- **Goal:** Prevent unauthorized access.

### 2.2 "The Commander" (Control Mode)
The active state after successful login.
- **Input:** Keypad used for menu navigation.
- **Display:** LCD shows menus (Rooms, Status, Settings).
- **Goal:** Allow user to toggle relays and view sensor data.

---

## 3. System Features

### 3.1 Feature: Access Control & Security

#### REQ-SEC-001: PIN Authentication
**Priority:** HIGH
**User Story:** *"As a user, I want to unlock the system using a secure code so that only authorized people can control my home."*

**Acceptance Criteria:**
1. System shall accept a 4-digit PIN input via 4x4 Keypad.
2. LCD shall mask digits as `*` during entry.
3. **Success:** Green LED ON, LCD shows "WELCOME", transition to *Commander Mode*.
4. **Failure:** Red LED blink, Buzzer beep (200ms), LCD shows "INVALID PIN".
5. **Lockout:** After 3 incorrect attempts, system shall lock keypad for 60 seconds.

#### REQ-SEC-002: Intrusion Detection
**Priority:** MEDIUM
**User Story:** *"As a user, I want an alarm to sound if a door is forced open."*

**Acceptance Criteria:**
1. System shall monitor a Reed Switch (GPIO Input) on the main door.
2. If door opens while system is LOCKED:
   - Trigger Buzzer (Continuous Alarm).
   - Log event "INTRUSION" to UART console.

---

### 3.2 Feature: Home Automation (The Commander)

#### REQ-AUTO-001: Menu Navigation
**Priority:** HIGH
**User Story:** *"As a user, I want to scroll through options to find the device I want to control."*

**Acceptance Criteria:**
1. System shall display a hierarchical menu on 16x2 LCD.
2. Navigation Keys:
   - `2`: Up
   - `8`: Down
   - `5`: Enter/Select
   - `0`: Back
3. LCD shall update within 100ms of keypress.

#### REQ-AUTO-002: Device Control
**Priority:** HIGH
**User Story:** *"As a user, I want to turn my Living Room lights on/off."*

**Acceptance Criteria:**
1. Menu shall provide options for "Living Room Light" and "Fan".
2. Selecting "ON" shall trigger the corresponding Relay (GPIO Output).
3. Status LED on the board shall reflect the Relay state.

#### REQ-AUTO-003: Ambient Lighting (PWM)
**Priority:** MEDIUM
**User Story:** *"As a user, I want the status LED to breathe softly so I know the system is alive without it being annoying."*

**Acceptance Criteria:**
1. System shall use a Timer in PWM mode to fade an LED.
2. Frequency: 1 kHz.
3. Duty cycle shall vary from 0% to 100% and back every 2 seconds.

---

### 3.3 Feature: Environmental Monitoring

#### REQ-ENV-001: Temperature Monitoring
**Priority:** MEDIUM
**User Story:** *"As a user, I want to check the current room temperature."*

**Acceptance Criteria:**
1. System shall read the internal temperature sensor (ADC).
2. Value shall be displayed on the LCD "Status" screen.
3. If Temp > 40°C, system shall trigger a "High Temp" warning on LCD.

#### REQ-ENV-002: Light Sensing
**Priority:** LOW
**User Story:** *"As a user, I want the system to know if it is dark."*

**Acceptance Criteria:**
1. System shall read an LDR (Light Dependent Resistor) via ADC polling.
2. If ADC Value < Threshold (Dark), system shall auto-enable the "Night Light" LED.

---

## 4. Non-Functional Requirements (Technical Constraints)

| ID | Category | Requirement |
|----|----------|-------------|
| **NFR-001** | **Latency** | Keypad press to LCD update must be < 100ms. |
| **NFR-002** | **Reliability** | Watchdog Timer (IWDG) must reset system if it hangs for > 2 seconds. |
| **NFR-003** | **Architecture** | Code must be strictly layered: Drivers must NOT call Application functions. |
| **NFR-004** | **Hardware** | Solution must run on STM32F446RE with no external OS (Bare Metal). |
| **NFR-005** | **Debug** | System must log critical events (Boot, Login, Error) to UART at 115200 baud. |

---

## 5. Traceability Matrix

| Requirement | Driver Required | BSP Module Required | Test Case |
| :--- | :--- | :--- | :--- |
| **REQ-SEC-001** (PIN) | GPIO | Keypad, LCD, LED | TC-AUTH-01 |
| **REQ-AUTO-003** (PWM) | Timer (PWM) | LED | TC-PWM-01 |
| **REQ-ENV-001** (Temp) | ADC | Sensor | TC-ADC-01 |
| **NFR-002** (Watchdog) | IWDG | None | TC-SYS-01 |

---

**END OF FUNCTIONAL REQUIREMENTS SPECIFICATION**