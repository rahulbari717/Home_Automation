# Functional Requirements Specification (FRS): The Citadel System
**Version:** 1.0  
**Date:** January 2026  
**Author:** Rahul Bari.  
**Status:** Approved for Development

---

## 1. Document Overview

### 1.1 Purpose
This Functional Requirements Specification (FRS) defines **what** the Citadel system must do from a user and stakeholder perspective. It serves as the contract between business requirements and technical implementation.

### 1.2 Scope
The Citadel is a **distributed embedded control system** for:
- **Smart Home Automation:** Lighting, climate, security
- **Industrial Safety Monitoring:** Fire, gas leak, intrusion detection
- **Remote Monitoring:** Cloud dashboards, mobile app control

### 1.3 Intended Audience
- Product Managers
- System Architects
- Test Engineers
- Compliance Officers
- End Users

---

## 2. System Features & User Stories

### 2.1 Feature: Home Security & Surveillance

#### REQ-SEC-001: Face Recognition Access Control
**Priority:** HIGH  
**User Story:** *"As a homeowner, I want the system to unlock my door automatically when it recognizes my face, so I don't need to carry keys."*

**Acceptance Criteria:**
1. System shall recognize registered faces with ≥95% accuracy in daylight
2. Face recognition shall complete within 2 seconds
3. System shall store up to 20 face embeddings locally
4. Unknown faces shall trigger a photo capture and mobile alert
5. System shall support PIN code fallback if face recognition fails

**Test Cases:**
- TC-SEC-001-01: Verify door unlocks for registered user using pin on keypad and 16*2 LCD display, oled status of door open or closed. 
- TC-SEC-001-02: Verify door unlocks for registered user
- TC-SEC-001-03: Verify door remains locked for unknown person
- TC-SEC-001-04: Verify alert sent when unknown person detected, buzzer on.

---

#### REQ-SEC-002: Multi-Sensor Intrusion Detection
**Priority:** HIGH  
**User Story:** *"As a security manager, I want the system to detect intruders using multiple sensors, so I have redundancy and fewer false alarms."*

**Acceptance Criteria:**
1. System shall use PIR (×2), IR, and RCWL-0516 radar sensors
2. Intrusion shall be confirmed only if 2+ sensors trigger within 5 seconds
3. System shall sound buzzer and send MQTT alert within 3 seconds
4. System shall capture 5 photos at 1 FPS and store to SD card also send to cloud
5. System shall distinguish between pets (<30 cm height) and humans

**Test Cases:**
- TC-SEC-002-01: Verify no false alarm from single sensor
- TC-SEC-002-02: Verify alarm triggers on multi-sensor confirmation
- TC-SEC-002-03: Verify photos stored with correct timestamp
- TC-SEC-002-04: Verify pet immunity (30 cm threshold)

---

### 2.2 Feature: Safety-Critical Monitoring

#### REQ-SAFE-001: Fire Detection & Emergency Response
**Priority:** CRITICAL  
**User Story:** *"As a building occupant, I want the system to detect fire and automatically unlock all exits, so I can evacuate safely."*

**Acceptance Criteria:**
1. System shall detect fire using flame sensor (KY-026) and camera (visual smoke)
2. Upon fire detection, system shall:
   - Unlock ALL doors within 50 ms
   - Sound continuous alarm (105 dB buzzer)
   - Cut power to non-essential relays (prevent electrical fire spread)
   - Send emergency CAN broadcast to all nodes
   - Publish MQTT alert with "FIRE" priority
3. System shall log fire event to EEPROM with timestamp
4. System shall enter "Emergency Mode" until manual reset

**Test Cases:**
- TC-SAFE-001-01: Simulate flame sensor trigger, verify door unlock <50 ms
- TC-SAFE-001-02: Verify buzzer activates at 105 dB (measured)
- TC-SAFE-001-03: Verify MQTT message reaches cloud within 5s
- TC-SAFE-001-04: Verify system logs event to EEPROM

---

#### REQ-SAFE-002: Gas Leak Detection & Ventilation
**Priority:** CRITICAL  
**User Story:** *"As a homeowner, I want the system to detect gas leaks and activate ventilation, preventing explosions."*

**Acceptance Criteria:**
1. System shall use MQ-2 gas sensor (detect LPG, propane, methane)
2. Gas threshold: >1000 ppm triggers alarm
3. Upon gas detection, system shall:
   - Cut power to ignition sources (relays for stove, heater)
   - Activate exhaust fan relay (if connected)
   - Sound pulsing alarm (2 Hz beep pattern)
   - Display "GAS LEAK - EVACUATE" on LCD
4. System shall auto-reset after gas level <500 ppm for 60 seconds

**Test Cases:**
- TC-SAFE-002-01: Inject 1200 ppm gas, verify relay cutoff
- TC-SAFE-002-02: Verify alarm pattern (2 Hz measured with oscilloscope)
- TC-SAFE-002-03: Verify auto-reset after 60s at safe level

---

### 2.3 Feature: Environmental Monitoring

#### REQ-ENV-001: Temperature & Humidity Tracking
**Priority:** MEDIUM  
**User Story:** *"As a greenhouse manager, I want to monitor temperature and humidity, so I can maintain optimal growing conditions."*

**Acceptance Criteria:**
1. System shall measure temperature using DHT11 sensor (±2°C accuracy)
2. System shall measure humidity using DHT11 (±5% accuracy)
3. Data shall update every 5 seconds on OLED display
4. Data shall publish to MQTT every 30 seconds
5. System shall trigger alerts if:
   - Temperature < 10°C or > 35°C
   - Humidity < 30% or > 80%

**Test Cases:**
- TC-ENV-001-01: Verify OLED updates every 5s (stopwatch)
- TC-ENV-001-02: Heat sensor to 40°C, verify alert triggers
- TC-ENV-001-03: Verify MQTT payload format matches specification

---

#### REQ-ENV-002: Light Level Adaptive Control
**Priority:** LOW  
**User Story:** *"As an office manager, I want lights to auto-adjust based on natural sunlight, saving energy."*

**Acceptance Criteria:**
1. System shall use 5× LDR sensors in different zones
2. System shall calculate average light level (0-1023 ADC)
3. If average light < 300, activate relay for zone lighting
4. If average light > 700, deactivate relay
5. System shall provide manual override via keypad (press '9')

**Test Cases:**
- TC-ENV-002-01: Cover all LDRs, verify relay activates
- TC-ENV-002-02: Illuminate LDRs >700, verify relay deactivates
- TC-ENV-002-03: Verify manual override works

---

### 2.4 Feature: User Interface & Control

#### REQ-UI-001: Local LCD Status Display
**Priority:** MEDIUM  
**User Story:** *"As a technician, I want to see system status on a local display, without needing WiFi."*

**Acceptance Criteria:**
1. LCD shall display 2 lines of 16 characters each
2. Line 1 format: `"Temp:25C Hum:60%"`
3. Line 2 format: `"Mode:AUTO Safe:OK"`
4. LCD shall update every 500 ms
5. If any sensor fails, display `"SENSOR_ERR"` for that sensor

**Test Cases:**
- TC-UI-001-01: Verify LCD updates within 500 ms
- TC-UI-001-02: Disconnect DHT11, verify "SENSOR_ERR" displayed
- TC-UI-001-03: Verify text formatting matches specification

---

#### REQ-UI-002: OLED Graphical Dashboard
**Priority:** MEDIUM  
**User Story:** *"As a power user, I want a graphical display with charts, so I can see trends at a glance."*

**Acceptance Criteria:**
1. OLED shall be 128×64 pixels (SSD1306)
2. Display shall show:
   - Temperature (numeric + bar graph)
   - Humidity (numeric + bar graph)
   - Distance sensor reading
   - Motion status (icon)
3. Display shall update every 500 ms
4. Display shall auto-dim after 60s of no motion (save power)

**Test Cases:**
- TC-UI-002-01: Verify bar graph scales correctly (0-100%)
- TC-UI-002-02: Verify auto-dim activates after 60s
- TC-UI-002-03: Verify display wakes on motion

---

#### REQ-UI-003: Touchscreen HMI Control Panel
**Priority:** HIGH  
**User Story:** *"As a facility manager, I want a touchscreen to control relays and view logs, like a professional control panel."*

**Acceptance Criteria:**
1. Touchscreen shall be 2.8" (240×320 pixels)
2. Home screen shall show:
   - 5× relay buttons (toggle ON/OFF)
   - Temperature & humidity widgets
   - Last 5 events in scrollable list
3. Relay buttons shall provide visual feedback (green=ON, gray=OFF)
4. Touch response time shall be <200 ms
5. Screen shall lock after 5 minutes, unlock with PIN

**Test Cases:**
- TC-UI-003-01: Tap relay button, verify visual feedback <200 ms
- TC-UI-003-02: Verify relay state changes via CAN bus
- TC-UI-003-03: Verify screen locks after 5 min
- TC-UI-003-04: Enter correct PIN, verify unlock

---

#### REQ-UI-004: Keypad Manual Control
**Priority:** MEDIUM  
**User Story:** *"As a maintenance worker, I want a keypad for manual control when the touchscreen fails."*

**Acceptance Criteria:**
1. Keypad shall be 4×4 matrix (16 keys)
2. Key mappings:
   - 1-5: Toggle relays 1-5
   - 6: Cycle display modes
   - 7: Mute buzzer
   - 8: Manual mode toggle
   - 9: Override lights ON
   - 0: Override lights OFF
   - *: Enter PIN mode
   - #: Confirm/Enter
3. PIN shall be 4 digits, configurable via EEPROM
4. Failed PIN attempts (3×) shall lock keypad for 60s

**Test Cases:**
- TC-UI-004-01: Press '1', verify relay 1 toggles
- TC-UI-004-02: Enter correct PIN, verify acceptance
- TC-UI-004-03: Enter wrong PIN 3×, verify 60s lockout

---

### 2.5 Feature: Remote Connectivity

#### REQ-CONN-001: MQTT Cloud Telemetry
**Priority:** HIGH  
**User Story:** *"As a remote manager, I want to see system data on my phone via ThingSpeak, even when I'm traveling."*

**Acceptance Criteria:**
1. System shall publish to MQTT broker (ThingSpeak) every 30s
2. Payload shall include:
   - Temperature (field1)
   - Humidity (field2)
   - Distance (field3)
   - Gas level (field4)
   - Motion status (field5)
3. System shall use TLS 1.2 encryption
4. If WiFi disconnects, system shall buffer last 10 messages
5. On reconnect, system shall flush buffer (QoS 1)

**Test Cases:**
- TC-CONN-001-01: Verify MQTT publish every 30s (Wireshark capture)
- TC-CONN-001-02: Disconnect WiFi, verify buffer stores 10 msgs
- TC-CONN-001-03: Reconnect WiFi, verify buffer flush
- TC-CONN-001-04: Verify TLS certificate validation

---

#### REQ-CONN-002: BLE Mobile App Control
**Priority:** MEDIUM  
**User Story:** *"As a homeowner, I want to control my home from my phone via Bluetooth, when WiFi is down."*

**Acceptance Criteria:**
1. System shall advertise BLE service with UUID `0x180A`
2. Characteristics:
   - Relay Control (UUID `0x2A57`): Write 1 byte (relay ID + state)
   - Sensor Data (UUID `0x2A58`): Read 20 bytes (all sensor values)
3. BLE shall use PIN pairing (6-digit code)
4. Range: ≥10 meters (line-of-sight)
5. Connection timeout: 60 seconds if no activity

**Test Cases:**
- TC-CONN-002-01: Pair phone with correct PIN
- TC-CONN-002-02: Write to relay characteristic, verify relay toggles
- TC-CONN-002-03: Read sensor characteristic, verify data format
- TC-CONN-002-04: Verify connection drops after 60s idle

---

#### REQ-CONN-003: OTA Firmware Updates
**Priority:** MEDIUM  
**User Story:** *"As a developer, I want to update firmware wirelessly, so I don't need physical access."*

**Acceptance Criteria:**
1. System shall support OTA updates via WiFi (HTTP/HTTPS)
2. Firmware images shall be signed with RSA-2048
3. System shall verify signature before flashing
4. On update failure, system shall rollback to previous firmware
5. Update progress shall be shown on OLED (0-100%)
6. System shall reboot automatically after successful update

**Test Cases:**
- TC-CONN-003-01: Upload valid signed firmware, verify success
- TC-CONN-003-02: Upload invalid signature, verify rejection
- TC-CONN-003-03: Interrupt update mid-flash, verify rollback
- TC-CONN-003-04: Verify OLED shows progress bar

---

### 2.6 Feature: Data Logging & Analytics

#### REQ-LOG-001: Event Logging to EEPROM
**Priority:** MEDIUM  
**User Story:** *"As a compliance officer, I want all safety events logged permanently, for audit trails."*

**Acceptance Criteria:**
1. System shall log events to STM32 EEPROM (64 KB)
2. Log entry format (16 bytes):
   - Timestamp (4 bytes, UNIX epoch)
   - Event type (1 byte): FIRE, GAS, INTRUSION, etc.
   - Sensor values snapshot (8 bytes)
   - Checksum (1 byte, XOR)
3. Log shall store last 4000 events (circular buffer)
4. System shall provide UART command to dump logs: `GET_LOGS\n`

**Test Cases:**
- TC-LOG-001-01: Trigger 10 events, verify all logged
- TC-LOG-001-02: Fill log to 4000 entries, verify oldest overwritten
- TC-LOG-001-03: Request log dump via UART, verify format
- TC-LOG-001-04: Verify checksum validation

---

#### REQ-LOG-002: SD Card Image Storage
**Priority:** LOW  
**User Story:** *"As a security analyst, I want intrusion photos saved to SD card, for forensic analysis."*

**Acceptance Criteria:**
1. System shall save JPEGs to SD card (FAT32 formatted)
2. Filename format: `IMG_YYYYMMDD_HHMMSS.jpg`
3. Images shall be timestamped with RTC
4. System shall auto-delete images older than 7 days
5. If SD card full, overwrite oldest images

**Test Cases:**
- TC-LOG-002-01: Trigger camera, verify JPEG saved with correct filename
- TC-LOG-002-02: Verify RTC timestamp in EXIF metadata
- TC-LOG-002-03: Fill SD card, verify oldest images deleted

---

## 3. Non-Functional Requirements

### 3.1 Performance Requirements

| ID | Requirement | Target |
|----|-------------|--------|
| NFR-PERF-001 | Safety-critical response time (fire → door unlock) | <50 ms |
| NFR-PERF-002 | LCD display update rate | 500 ms (2 Hz) |
| NFR-PERF-003 | MQTT publish latency (sensor → cloud) | <5 seconds |
| NFR-PERF-004 | Face recognition inference time | <2 seconds |
| NFR-PERF-005 | Touchscreen touch response | <200 ms |
| NFR-PERF-006 | CAN bus utilization | <20% @ 500 kbps |
| NFR-PERF-007 | SPI bus utilization | <10% @ 4 MHz |

---

### 3.2 Reliability Requirements

| ID | Requirement | Target |
|----|-------------|--------|
| NFR-REL-001 | Mean Time Between Failures (MTBF) | >10,000 hours |
| NFR-REL-002 | Watchdog timer reset time | 1 second |
| NFR-REL-003 | CAN bus error recovery time | <500 ms |
| NFR-REL-004 | WiFi reconnection time | <10 seconds |
| NFR-REL-005 | System uptime (continuous operation) | >30 days |

---

### 3.3 Usability Requirements

| ID | Requirement | Target |
|----|-------------|--------|
| NFR-USE-001 | Touchscreen setup wizard completion time (first boot) | <5 minutes |
| NFR-USE-002 | Face registration process (mobile app) | <30 seconds |
| NFR-USE-003 | Keypad PIN entry time | <10 seconds |
| NFR-USE-004 | OLED readability distance | 2 meters |

---

### 3.4 Security Requirements

| ID | Requirement | Description |
|----|-------------|-------------|
| NFR-SEC-001 | MQTT encryption | TLS 1.2 with certificate validation |
| NFR-SEC-002 | BLE pairing | PIN-based (6 digits) |
| NFR-SEC-003 | Firmware signing | RSA-2048 signature |
| NFR-SEC-004 | Face embedding storage | Encrypted with AES-128 |
| NFR-SEC-005 | Keypad PIN storage | SHA-256 hash in EEPROM |

---

### 3.5 Environmental Requirements

| ID | Requirement | Range |
|----|-------------|-------|
| NFR-ENV-001 | Operating temperature | 0°C to 50°C |
| NFR-ENV-002 | Storage temperature | -20°C to 70°C |
| NFR-ENV-003 | Humidity | 10% to 90% RH (non-condensing) |
| NFR-ENV-004 | Power supply voltage | 4.5V to 5.5V (USB) |

---

## 4. Compliance & Standards

### 4.1 Regulatory Compliance

| Standard | Description | Applicability |
|----------|-------------|---------------|
| **ISO 13849** | Safety of machinery - Safety-related parts of control systems | Fire/gas safety features |
| **IEC 61508** | Functional safety of electrical/electronic systems | Safety integrity level (SIL 2 target) |
| **EN 54** | Fire detection and alarm systems | Fire sensor accuracy |
| **UL 2017** | Standard for general-purpose signaling devices | Buzzer sound level (105 dB) |

### 4.2 Communication Standards

| Standard | Description | Usage |
|----------|-------------|-------|
| **ISO 11898** | CAN bus protocol | Industrial node communication |
| **IEEE 802.15.1** | Bluetooth specification | BLE mobile app |
| **MQTT v3.1.1** | Message queuing protocol | Cloud telemetry |

---

## 5. Use Cases

### 5.1 Use Case: Morning Routine Automation

**Actor:** Homeowner  
**Preconditions:** System armed in "Night Mode"

**Main Flow:**
1. At 6:00 AM, RTC triggers wake-up sequence
2. System gradually increases relay-controlled lights (PWM dimming)
3. PIR sensors detect presence in bedroom
4. Coffee maker relay activates (connected to Relay 4)
5. OLED displays weather forecast (fetched via MQTT)
6. System transitions to "Day Mode" (reduced sensitivity)

**Postconditions:** Home ready for occupant

---

### 5.2 Use Case: Emergency Evacuation

**Actor:** Building Occupant  
**Preconditions:** Fire detected by flame sensor

**Main Flow:**
1. Flame sensor triggers interrupt on STM32
2. STM32 broadcasts CAN frame `0x000` (highest priority)
3. All ESP32-S2 relays unlock doors within 50 ms
4. Continuous 105 dB alarm sounds
5. ESP32-CAM captures evacuation footage
6. MQTT alert sent to fire department (if configured)
7. System enters "Emergency Mode" (requires manual reset)

**Postconditions:** All exits unlocked, alarm sounding

---

## 6. Test Plan Summary

### 6.1 Test Phases

| Phase | Duration | Focus |
|-------|----------|-------|
| **Unit Testing** | 2 weeks | Individual function/module verification |
| **Integration Testing** | 2 weeks | Inter-node communication (CAN/SPI/UART) |
| **System Testing** | 1 week | End-to-end scenarios (use cases) |
| **Acceptance Testing** | 1 week | Stakeholder validation |
| **Field Testing** | 2 weeks | Real-world deployment (beta) |

### 6.2 Test Coverage Targets

- Code Coverage: ≥80%
- Branch Coverage: ≥70%
- Safety-Critical Paths: 100%

---

## 7. Traceability Matrix

| Requirement | HLD Section | LLD Section | Test Case |
|-------------|-------------|-------------|-----------|
| REQ-SEC-001 | 3.3 Vision Plane | 5.2 Face Detection | TC-SEC-001-01 |
| REQ-SAFE-001 | 3.1 Control Plane | 2.5 State Machine | TC-SAFE-001-01 |
| REQ-ENV-001 | 2.1 Node Definitions | 3.3 DHT11 Driver | TC-ENV-001-01 |
| REQ-UI-003 | 2.2 ESP32 Pico | 4.1 SPI Slave | TC-UI-003-01 |
| REQ-CONN-001 | 3.2 Telemetry Plane | 4.3 MQTT Client | TC-CONN-001-01 |

---

## 8. Glossary

| Term | Definition |
|------|------------|
| **CAN** | Controller Area Network (ISO 11898) |
| **DMA** | Direct Memory Access (zero-CPU data transfer) |
| **FreeRTOS** | Real-Time Operating System (preemptive multitasking) |
| **MQTT** | Message Queue Telemetry Transport (IoT protocol) |
| **OTA** | Over-The-Air (wireless firmware updates) |
| **SPI** | Serial Peripheral Interface (synchronous bus) |
| **UART** | Universal Asynchronous Receiver-Transmitter |

---

## 9. Assumptions & Constraints

### 9.1 Assumptions
- Stable 5V power supply available (MB102 module)
- WiFi network available for cloud features
- SD card formatted as FAT32 (≥8 GB)

### 9.2 Constraints
- STM32 has 128 KB RAM (limits camera buffer size)
- ESP32-CAM inference limited to 2 FPS (TensorFlow Lite)
- CAN bus max length: 40 meters @ 500 kbps

---

**END OF FUNCTIONAL REQUIREMENTS SPECIFICATION**