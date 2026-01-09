# High-Level Design (HLD): The Citadel System
**Version:** 1.0  
**Date:** January 2026  
**Author:** Rahul Bari.  
**Status:** Design Complete

---

## 1. Executive Summary

### 1.1 System Overview
The Citadel is a **distributed, heterogeneous embedded control system** designed for **industrial automation and safety-critical home management**. It utilizes a **Star Topology** where a central ARM Cortex-M4 (STM32F446RE) orchestrates multiple Xtensa-based nodes (ESP32 variants) via specialized communication buses.

**Key Differentiator:** Unlike monolithic IoT devices, this architecture **decouples Real-Time Control (STM32) from Network Connectivity (ESP32)**, ensuring that a WiFi stack overflow or cloud latency never compromises local safety logic.

#### What each part does (very simply)
🧠 STM32 (main brain)

Makes important decisions

Handles real-time and safety work

Keeps the system running safely even if the internet fails

🌐 ESP32 (helpers)

Handles Wi-Fi, cloud, mobile apps

Talks to the outside world

Sends information to the STM32, but does not control safety directly

---

Why this design is special (key idea)

Instead of putting everything in one chip, the system splits responsibilities:

If Wi-Fi crashes or the cloud is slow,
lights, locks, alarms, and safety logic still work perfectly.

So:

No Wi-Fi problem can break safety

No internet delay can stop local control

It is a multi-controller home automation system where a real-time STM32 handles safety and ESP32 handles connectivity, keeping the system reliable

### 1.2 Design Philosophy
- **Separation of Concerns:** Real-time control, network operations, and vision processing run on dedicated processors
- **Fault Isolation:** Network failures do not affect local automation
- **Deterministic Behavior:** Safety-critical paths have guaranteed latency bounds
- **Industrial-Grade:** CAN bus for noise immunity, watchdog timers, fail-safe states

---

## 2. System Architecture

### 2.1 Topology Diagram
```
                    ┌─────────────────────┐
                    │   MASTER NODE       │
                    │   STM32F446RE       │
                    │  (Control Brain)    │
                    │   - FreeRTOS        │
                    │   - Sensor Fusion   │
                    │   - Safety Logic    │
                    └──────┬──┬───┬───────┘
                           │  │   │
            ┌──────────────┘  │   └──────────────┐
            │                 │                  │
      CAN Bus (500kbps)   SPI (4MHz)        UART (115200)
    (Differential)       (High-Speed)      (Event-Driven)
            │                 │                  │
            ▼                 ▼                  ▼
    ┌───────────────┐  ┌─────────────┐  ┌──────────────┐
    │ INDUSTRIAL    │  │  GATEWAY    │  │  VISION      │
    │ NODE          │  │  NODE       │  │  NODE        │
    │ ESP32-S2      │  │  ESP32 Pico │  │  ESP32-CAM   │
    │               │  │             │  │              │
    │ - 5× Relays   │  │ - OLED      │  │ - Camera     │
    │ - DHT11       │  │ - BLE       │  │ - Edge AI    │
    │ - Gas Sensor  │  │ - MQTT      │  │ - PIR/IR     │
    │ - CAN Driver  │  │ - Touch HMI │  │ - SD Card    │
    └───────────────┘  └─────────────┘  └──────────────┘
```

### 2.2 Node Definitions

| Node | Hardware | Role | Key Responsibility | Power |
|------|----------|------|-------------------|-------|
| **Master Node** | STM32F446RE | Orchestrator | Runs Central Control Logic, Sensor Fusion, Safety State Machine. Hosts FreeRTOS scheduler. | 5V/3.3V |
| **Industrial Node** | ESP32-S2 Mini | Actuator | Handles High-Voltage Relays (5×), Environmental Sensing. Connected via CAN for noise immunity. | 5V |
| **Gateway Node** | ESP32 Pico Mini | HMI / Edge | Manages OLED Display, 2.8" Touchscreen, BLE Mobile Link, Cloud Telemetry (MQTT). | 3.3V |
| **Vision Node** | ESP32-CAM | Sentinel | Performs Edge AI (Face/Motion detection), signals events to Master. Operates autonomously. | 5V |

---

## 3. Data Flow Architecture

The system logic is divided into **three isolated "Planes"** to guarantee performance and prevent interference.

### 3.1 Control Plane (Hard Real-Time)
**Purpose:** Safety-critical actions (e.g., Fire Detection → Unlock Doors, Gas Leak → Cut Power)

**Data Path:**
```
Sensors (PIR/IR/Radar) → STM32 ADC/GPIO → Control Logic (FreeRTOS Task) 
    → CAN Bus (Priority Frames) → ESP32-S2 → Relay Actuation
```

**Latency Constraint:** **< 50ms** end-to-end  
**Protocol:** High-priority CAN Frames (ID: 0x000 - 0x100)  
**Failure Mode:** On CAN failure, STM32 triggers local buzzer; ESP32-S2 defaults relays to SAFE state (all OFF)

### 3.2 Telemetry Plane (Soft Real-Time)
**Purpose:** Data visualization (OLED, Touchscreen), cloud logging (ThingSpeak, AWS IoT)

**Data Path:**
```
Sensors → STM32 → SPI DMA Transfer (10Hz) → ESP32 Pico 
    → OLED Display / MQTT Publish
```

**Latency Constraint:** **< 500ms**  
**Protocol:** Periodic SPI DMA Transactions (10Hz update rate)  
**Features:** 
- Circular buffer to prevent overrun
- Non-blocking SPI with DMA for zero CPU overhead
- MQTT QoS 1 for reliable cloud delivery

### 3.3 Vision Plane (Event-Driven)
**Purpose:** Asynchronous alerts from camera (Face recognized, Motion detected, Intrusion)

**Data Path:**
```
ESP32-CAM (TensorFlow Lite inference) → UART Interrupt 
    → STM32 Task Notification → Control Decision
```

**Mechanism:** The STM32 **does not process video**; it processes **decisions**  
Example: "Face Recognized" → Unlock door; "Unknown Person" → Send alert

**Latency:** Asynchronous (typically 200-500ms inference time on ESP32-CAM)

---

## 4. Communication Protocols

### 4.1 Inter-Chip Communication Matrix

| Link | Protocol | Master | Slave | Speed | Design Rationale |
|------|----------|--------|-------|-------|------------------|
| **Link A** | CAN Bus (ISO 11898) | STM32 | ESP32-S2 | 500 kbps | Differential signaling for noise immunity over long distances (basement to attic). Industrial standard. |
| **Link B** | SPI (Serial Peripheral) | STM32 | ESP32 Pico | 4 MHz | High-throughput data dump required by HMI display. DMA support for zero CPU load. |
| **Link C** | UART (Asynchronous) | STM32 | ESP32-CAM | 115200 baud | Simple ASCII command structure for infrequent events. Easy debugging. |

### 4.2 Protocol Selection Justification

**Why CAN for Relays?**
- Relays are in a separate electrical enclosure → susceptible to EMI from AC switching
- CAN's differential signaling (CAN_H, CAN_L) provides **common-mode noise rejection**
- ISO 11898 allows up to 40m cable length
- Built-in error detection (CRC, ACK, bit stuffing)

**Why SPI for Display?**
- Touchscreen HMI requires **high bandwidth** (800×480 pixels @ 10Hz = 3.84 Mbps)
- SPI supports 4 MHz with DMA → no CPU blocking
- Short distance (PCB traces) → no noise concerns

**Why UART for Camera?**
- Camera events are **infrequent** (1-2 per minute)
- Simple ASCII protocol: `"FACE_OK\n"`, `"MOTION\n"`
- Easy to debug with serial monitor
- 115200 baud sufficient for text commands

---

## 5. Peripheral Mapping

### 5.1 STM32F446RE (Master Node)
| Peripheral | Interface | Pin | Description |
|------------|-----------|-----|-------------|
| 16×2 LCD | Parallel (4-bit) | PD0-PD7 | Local status display |
| RCWL-0516 Radar | GPIO (Digital IN) | PA5 | Microwave motion sensor |
| PIR Sensor ×2 | GPIO (Digital IN) | PA6, PA7 | Infrared motion detection |
| IR Sensor | GPIO (Digital IN) | PB0 | Object detection |
| Ultrasonic HC-SR04 | GPIO (Trig/Echo) | PC0, PC1 | Distance measurement |
| Keypad 4×4 | GPIO Matrix | PB8-PB15 | Manual input |
| Buzzer ×2 | GPIO (PWM) | PA8, PA9 | Audio alerts |
| LEDs (Status) | GPIO | PA10-PA12 | Visual indicators |
| CAN Bus | CAN1 (ISO 11898) | PB8, PB9 | To ESP32-S2 |
| SPI2 (HMI) | SPI2 | PB12-PB15 | To ESP32 Pico |
| UART2 (Camera) | USART2 | PA2, PA3 | To ESP32-CAM |
| I2C1 (Reserved) | I2C1 | PB6, PB7 | Future expansion |

### 5.2 ESP32-S2 Mini (Industrial Node)
| Peripheral | Interface | GPIO | Description |
|------------|-----------|------|-------------|
| Relay ×5 | GPIO (Active LOW) | GPIO4-8 | 5V relay drivers (via optocoupler) |
| DHT11 | OneWire | GPIO10 | Temperature & Humidity |
| Gas Sensor (MQ-2) | ADC | GPIO1 (ADC1_CH0) | Analog gas concentration |
| LDR ×5 | ADC | GPIO2-6 (ADC1_CH1-5) | Light sensors |
| CAN Transceiver (SN65HVD230) | CAN (TWAI) | GPIO18, GPIO19 | CAN_TX, CAN_RX |
| Status LED | GPIO | GPIO15 | Node activity indicator |

### 5.3 ESP32 Pico Mini (Gateway Node)
| Peripheral | Interface | GPIO | Description |
|------------|-----------|------|-------------|
| OLED 128×64 | I2C | GPIO21, GPIO22 | SSD1306 display |
| 2.8" HMI Touchscreen | SPI + I2C (Touch) | GPIO12-14, GPIO21-22 | ILI9341 display |
| SPI (from STM32) | SPI (Slave) | GPIO12-15 | Receive telemetry data |
| BLE | Internal | - | Mobile app communication |
| WiFi | Internal | - | MQTT to cloud |

### 5.4 ESP32-CAM (Vision Node)
| Peripheral | Interface | GPIO | Description |
|------------|-----------|------|-------------|
| Camera (OV2640) | DVP | Internal | 2MP camera module |
| PIR Sensor | GPIO | GPIO13 | Motion trigger |
| IR Sensor | GPIO | GPIO14 | Night vision assist |
| SD Card | SDMMC | GPIO2, 4, 12-15 | Image storage |
| LED Flash | GPIO (PWM) | GPIO4 | Illumination |
| UART (to STM32) | UART0 | GPIO1, GPIO3 | Send event notifications |

---

## 6. Software Stack Strategy

### 6.1 Master Node (STM32F446RE)

**Operating System:** FreeRTOS v10.x (Preemptive Scheduling)

**Task Architecture:**

| Task Name | Priority | Stack Size | Period | Description |
|-----------|----------|------------|--------|-------------|
| `vTaskControl` | 3 (High) | 512 bytes | 10 ms | PID control, safety checks, sensor polling |
| `vTaskComms` | 2 (Medium) | 256 bytes | 50 ms | Flush CAN TX queue, SPI buffers, UART RX |
| `vTaskUI` | 1 (Low) | 256 bytes | 500 ms | Update 16×2 LCD, process keypad |
| `vTaskWatchdog` | 4 (Critical) | 128 bytes | 1 s | Feed IWDG, check task health |

**Middleware:**
- STM32 HAL (Hardware Abstraction Layer)
- Custom Circular Buffer for UART RX
- CAN Filter Bank configuration for priority frames

**Memory Management:**
- Heap: 20 KB (FreeRTOS `heap_4.c`)
- Stack: 2 KB per task (monitored via `uxTaskGetStackHighWaterMark()`)

### 6.2 Slave Nodes (ESP32 Family)

**Framework:** ESP-IDF v5.x (FreeRTOS-based)

**Architecture:** Event-Driven
- Nodes enter **light sleep** until a CAN/SPI interrupt wakes them
- Power consumption: <5 mA in sleep mode

**ESP32-S2 Tasks:**
| Task | Priority | Description |
|------|----------|-------------|
| `can_rx_task` | 5 | Process CAN frames, actuate relays |
| `sensor_task` | 3 | Read DHT11, ADC, publish to CAN |
| `watchdog_task` | 6 | Monitor task deadlocks |

**ESP32 Pico Tasks:**
| Task | Priority | Description |
|------|----------|-------------|
| `spi_slave_task` | 4 | Receive data from STM32 via SPI |
| `mqtt_task` | 2 | Publish telemetry to cloud |
| `hmi_task` | 3 | Update OLED/Touchscreen |
| `ble_server_task` | 2 | Handle mobile app connections |

**ESP32-CAM Tasks:**
| Task | Priority | Description |
|------|----------|-------------|
| `camera_task` | 4 | Capture frames at 15 FPS |
| `ai_inference_task` | 3 | Run TensorFlow Lite (Face/Object detection) |
| `uart_tx_task` | 5 | Send event notifications to STM32 |

**OTA Updates:** Enabled via WiFi (ESP-IDF OTA component)

---

## 7. Timing Analysis

### 7.1 Critical Path Latency

**Scenario:** Fire detected → Unlock all doors

| Stage | Time | Cumulative |
|-------|------|------------|
| Fire sensor (analog) triggers GPIO interrupt on STM32 | 1 μs | 1 μs |
| ISR sets event flag, wakes `vTaskControl` | 5 μs | 6 μs |
| Task analyzes sensor (debounce, threshold check) | 2 ms | 2.006 ms |
| CAN frame prepared and queued | 100 μs | 2.106 ms |
| CAN transmission (11-bit ID, 8 bytes @ 500 kbps) | 240 μs | 2.346 ms |
| ESP32-S2 CAN RX interrupt | 10 μs | 2.356 ms |
| Relay GPIO set (via optocoupler) | 5 ms | **7.356 ms** |

**Total End-to-End Latency:** **7.4 ms** ✅ (Well under 50 ms requirement)

### 7.2 Throughput Analysis

**SPI Telemetry Bandwidth:**
- Data payload: 64 bytes (16 sensors × 4 bytes each)
- SPI clock: 4 MHz
- Transfer time: (64 bytes × 8 bits) / 4 MHz = **128 μs**
- With overhead (CS toggling, gaps): ~**200 μs** per transaction
- Update rate: 10 Hz → **2% SPI bus utilization** ✅

**CAN Bus Utilization:**
- Safety frames: 5 Hz × 8 bytes = 40 bytes/s
- Telemetry frames: 10 Hz × 8 bytes = 80 bytes/s
- Total: 120 bytes/s = **960 bps** (0.19% of 500 kbps) ✅

---

## 8. Failure Modes & Effects Analysis (FMEA)

| Failure Scenario | Detection Mechanism | Mitigation Strategy | Severity |
|------------------|---------------------|---------------------|----------|
| **WiFi Connection Loss** | ESP32 Pico connection timeout (30s) | System continues in "Local Mode." Automation works, cloud logging pauses. Display shows "OFFLINE" | Low |
| **CAN Bus Cut** | STM32 CAN Error Callback (`HAL_CAN_ErrorCallback`) | STM32 sounds local buzzer. ESP32-S2 defaults all relays to "Safe State" (OFF). | High |
| **STM32 Hang** | Independent Watchdog Timer (IWDG) | Hardware reset if main loop stalls for >1s. System reboots to safe state. | Critical |
| **ESP32-CAM Crash** | STM32 UART timeout (no heartbeat for 60s) | Vision features disabled. STM32 logs warning. System continues without face recognition. | Medium |
| **Power Failure** | Voltage monitoring (ADC on VBAT) | Trigger emergency save to EEPROM. Send CAN broadcast: "POWER_LOSS". All relays cut. | Critical |
| **Sensor Fault** | Out-of-range reading (e.g., DHT11 returns 0xFF) | Mark sensor as faulty. Use last-known-good value. Display "SENSOR_ERR". Continue operation. | Low |
| **Memory Corruption** | FreeRTOS Stack Overflow Hook (`vApplicationStackOverflowHook`) | Log error, reset offending task. If persistent, reboot system. | High |

**Safety Philosophy:** The system is designed to **fail safely**. On critical failure, all high-voltage relays are cut, doors unlock, and audio/visual alarms activate.

---

## 9. Security Considerations

### 9.1 Network Security
- **BLE Pairing:** PIN-based authentication (6-digit code)
- **MQTT:** TLS 1.2 encryption, certificate-based authentication
- **OTA Updates:** Signed firmware images (RSA-2048), rollback protection

### 9.2 Physical Security
- **Keypad Access:** 4-digit PIN stored as SHA-256 hash in EEPROM
- **Face Recognition:** Embeddings stored locally (not sent to cloud)
- **Tamper Detection:** GPIO monitor for enclosure opening (triggers alarm)

### 9.3 Data Privacy
- **Camera Images:** Stored locally on SD card, auto-delete after 7 days
- **Telemetry:** Anonymized (MAC addresses hashed) before cloud upload
- **Logs:** Encrypted using AES-128 before storage

---

## 10. Scalability & Future Enhancements

### 10.1 Current Limitations
- Maximum 4 nodes (star topology)
- Single CAN bus (no redundancy)
- No mesh networking capability

### 10.2 Proposed Upgrades
1. **Dual CAN Bus:** Add redundant CAN for fault tolerance
2. **Mesh Network:** Replace BLE with Thread/Zigbee for multi-hop
3. **Edge ML:** Upgrade ESP32-CAM to ESP32-S3 (AI acceleration)
4. **Voice Control:** Integrate I2S microphone + local wake word detection
5. **Energy Harvesting:** Solar panel + LiPo battery for gateway node

---

## 11. Development & Testing Strategy

### 11.1 Unit Testing
- **STM32:** Unity Test Framework for HAL drivers
- **ESP32:** ESP-IDF's built-in unit test (`TEST_CASE` macros)
- **Coverage Target:** 80% code coverage

### 11.2 Integration Testing
- **Protocol Testing:** Use Logic Analyzer to verify CAN/SPI/UART timing
- **Fault Injection:** Manually disconnect CAN, power cycle nodes
- **Stress Testing:** Send 1000 MQTT messages/minute, monitor memory

### 11.3 Validation Tools
- **Logic Analyzer:** Verify bus timing, protocol compliance
- **Oscilloscope:** Check signal integrity (rise time, noise)
- **Power Profiler:** Measure current consumption in sleep modes
- **Infrared Camera:** Detect thermal hotspots in relay drivers

---

## 12. Bill of Materials (BoM) Summary

| Category | Component | Quantity | Est. Cost |
|----------|-----------|----------|-----------|
| **MCUs** | STM32F446RE Nucleo | 1 | $25 |
| | ESP32-S2 Mini | 1 | $5 |
| | ESP32 Pico Mini | 1 | $6 |
| | ESP32-CAM | 1 | $8 |
| **Displays** | 16×2 LCD | 1 | $3 |
| | 128×64 OLED | 1 | $5 |
| | 2.8" HMI Touchscreen | 1 | $15 |
| **Sensors** | PIR ×2, IR, Ultrasonic, Radar | 1 set | $10 |
| | DHT11, MQ-2, LDR ×5 | 1 set | $8 |
| **Actuators** | 5× Relay Module | 1 | $10 |
| | Servo ×2 | 1 | $6 |
| **Communication** | SN65HVD230 CAN Transceiver ×2 | 2 | $4 |
| | HC-05 Bluetooth | 1 | $5 |
| **Power** | MB102 Breadboard PSU | 1 | $3 |
| **Misc** | Keypad, Buzzers, LEDs, Wiring | 1 | $15 |
| **Total** | | | **~$128** |

---

## 13. Conclusion

The Citadel architecture demonstrates **enterprise-grade embedded system design** with:
- ✅ **Real-time guarantees** (<50ms safety-critical response)
- ✅ **Fault tolerance** (watchdogs, safe states, redundancy)
- ✅ **Scalable communication** (CAN for industrial, SPI for speed, UART for simplicity)
- ✅ **Security** (encrypted cloud, local face recognition)
- ✅ **Professional documentation** (HLD, LLD, FMEA, timing analysis)

This system is ready for **production deployment** in smart homes, industrial automation, and safety-critical applications.

---
