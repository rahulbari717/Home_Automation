# The Citadel: Driver Development Roadmap & Implementation Plan

This document outlines the step-by-step plan for building the drivers and modules for "The Citadel" distributed embedded system. We will follow a **Bottom-Up Approach**: building low-level drivers first, testing them in isolation, and then integrating them.

---

## 🏗️ 1. Project Structure Overview

We will maintain a clean separation between **Hardware Abstraction Layer (HAL/Drivers)** and **Application Logic**. We will use shared folders to avoid duplicating code.

```text
firmware/
├── common/                         # Shared Headers (Protocol definitions like system_packets.h)
│
├── common_drivers/                 # Platform Agnostic Drivers (STM32 & ESP32)
│   ├── leds/                       # LED control logic
│   ├── buzzer/                     # Piezo buzzer control
│   ├── ldr/                        # Light Dependent Resistor logic
│   ├── keypad/                     # 4x4 Matrix Keypad logic
│   ├── relay/                      # Relay module control logic
│   ├── ir/                         # Infrared sensor logic
│   ├── pir/                        # Passive Infrared (Motion) sensor logic
│   ├── ultrasonic/                 # HC-SR04 Distance sensor logic
│   ├── servo/                      # Servo motor position control
│   ├── rcwl-0516/                  # Microwave radar motion sensor logic
│   ├── ssd1306/                    # OLED Display driver (I2C)
│   ├── lcd1602/                    # Character LCD driver (I2C/Parallel)
│   └── ili9341/                    # TFT Display driver (SPI)
│
├── esp32-common-components/        # Shared ESP-IDF Components (S2, Pico, Cam)
│   ├── esp_rtos/                   # FreeRTOS wrappers
│   ├── esp_menuconfig/             # Kconfig settings
│   ├── esp_memory/                 # Heap/PSRAM manager
│   ├── esp_storage/                # NVS & SPIFFS wrapper
│   ├── esp_gpio/                   # GPIO wrapper
│   ├── esp_adc/                    # ADC wrapper
│   ├── esp_dac/                    # DAC wrapper
│   ├── esp_pwm/                    # LEDC / PWM wrapper
│   ├── esp_uart/                   # UART wrapper
│   ├── esp_spi/                    # SPI wrapper
│   ├── esp_i2c/                    # I2C wrapper
│   ├── esp_can/                    # TWAI (CAN) Driver
│   ├── esp_wifi_ntp_mqtt/          # WiFi, NTP & MQTT Client
│   └── esp_bt_ble/                 # Bluetooth/BLE wrapper
│
├── app_main/                       # Main application entry point for ESP32 nodes
│
├── stm32-master/                   # STM32 F446RE (The Brain)
│   ├── drivers/                    # Register-level drivers (.c/.h)
│   └── app/                        # Application & Test code
│
├── esp32-s2-industrial/            # ESP32-S2 (Uses esp32-common-components)
├── esp32-pico-gateway/             # ESP32 Pico (Uses esp32-common-components)
└── esp32-cam-vision/               # ESP32-CAM (Camera specific code)

```

## 🧠 2. STM32 Master Node (STM32F446RE)

Focus: Bare-Metal Register Programming & FreeRTOS Integration.

### Phase 2.1: Foundation & GPIO
Goal: Create the base header and control pins.

[ ] Setup: Create drivers/stm32f446re.h.

[ ] GPIO Driver: Create drivers/gpio_driver.h & .c.

[ ] Test App: Create app/test_gpio.c.

[ ] Blink generic LED, Control Buzzer.

[ ] Read Push Button.

[ ] Test Relay driver.

### Phase 2.2: System & Timing
Goal: Accurate timing without HAL.

[ ] Clock Driver: Create drivers/rcc_driver.h & .c.

[ ] rcc_setup_180mhz(): Configure PLL, Flash Latency.

[ ] SysTick Driver: Create drivers/systick_driver.h & .c.

[ ] systick_init(): 1ms interrupt.

[ ] delay_ms(uint32_t ms): Blocking delay.

### Phase 2.3: Serial Communication
Goal: Debugging & Vision Node link.

[ ] UART Driver: drivers/uart_driver.h & .c.

[ ] SPI Driver: drivers/spi_driver.h & .c.

[ ] I2C Driver: drivers/i2c_driver.h & .c.

[ ] CAN Driver: drivers/can_driver.h & .c.

[ ] RTC Driver: drivers/rtc_driver.h & .c.

[ ] Test App: app/test_comms.c -> Loopback & Packet tests.

### Phase 2.4: Advanced Peripherals

[ ] ADC Driver: drivers/adc_driver.c (Single conversion).

[ ] DAC Driver: drivers/dac_driver.c (Signal generation).

[ ] PWM Driver: drivers/timer_pwm_driver.c (TIM2/3 OC mode).

[ ] Watchdog: drivers/iwdg_driver.c (Enable IWDG).

### Phase 2.5: RTOS Integration

[ ] RTOS Driver: drivers/rtos_driver.c (Task, Mutex, Semaphore, Queue wrappers).

## 🖥️ 3. Platform Agnostic Drivers (Shared)

Focus: Drivers that work on BOTH STM32 and ESP32 (Logic separated from hardware). Location: firmware/common_drivers/

[ ] OLED Driver: ssd1306/ (I2C Logic).

[ ] LCD 16x2 Driver: lcd1602/ (Character Logic).

[ ] TFT Driver: ili9341/ (SPI Logic).

[ ] Sensor Logic: pir/, relay/, ultrasonic/, etc. 

## 🏭 4. ESP32 Common Components (Shared)
Focus: Wrappers for ESP-IDF features (Reusable across S2, Pico, Cam).

### Phase 4.1: Basic IO & System
Location: firmware/esp32-common-components/

[ ] GPIO Component: esp_gpio/.

[ ] Memory Component: esp_memory/.

[ ] Storage Component: esp_storage/ (NVS).

[ ] ADC Component: esp_adc/.

[ ] DAC Component: esp_dac/.

[ ] PWM Component: esp_pwm/.

### Phase 4.2: Serial Communication
Location: firmware/esp32-common-components/

[ ] UART Component: esp_uart/.

[ ] I2C Component: esp_i2c/.

[ ] SPI Component: esp_spi/.

[ ] CAN Component: esp_can/ (TWAI Driver).

### Phase 4.3: Wireless & Cloud
Location: firmware/esp32-common-components/

[ ] WiFi Manager: esp_wifi_ntp_mqtt/ (Station Mode).

[ ] MQTT Client: esp_wifi_ntp_mqtt/ (ThingSpeak/HiveMQ).

[ ] BLE Server: esp_bt_ble/ (NimBLE Stack).

## 👁️ 5. ESP32-CAM Vision Node (Specific)
Focus: Camera hardware specific code.

### Phase 5.1: Camera Setup
[ ] Camera Driver: camera_config.c (OV2640 Setup).

[ ] Framebuffer: Capture and serialization.

### Phase 5.2: Vision Logic
[ ] Face Detection: Integration with ESP-WHO.

[ ] UART Link: Send "FACE_DETECTED" to STM32.

[ ] SD Card: Save JPEG images.

## 🧪 6. Integration Testing Plan

Each board has its own test/ or app/ folder, but final integration happens here.

Ping Test: STM32 sends CAN message -> ESP32-S2 toggles LED.

Data Path Test: Sensor (ESP32-S2) -> CAN -> STM32 -> SPI -> Pico -> OLED.

Safety Test: Disconnect CAN wire -> ESP32-S2 should turn off all relays (Watchdog).

Full System: Trigger Face Detect -> Unlock Relay + Log to MQTT.

---

## 📊 Priority 

🎯 Recommended Development Sequence

### Sprint 1 (Weeks 1-2): HIGH PRIORITY

STM32 GPIO + Clock + SysTick
UART driver + loopback test
Basic LED/Buzzer control

### Sprint 2 (Weeks 3-4): HIGH PRIORITY

I2C + SPI drivers
CAN driver + packet protocol
Communication loopback tests

### Sprint 3 (Weeks 5-6): MEDIUM PRIORITY

ADC/PWM/DAC drivers
ESP32 GPIO/Memory components
Platform agnostic sensor drivers

### Sprint 4 (Weeks 7-8): MEDIUM PRIORITY

ESP32 UART/I2C/SPI components
OLED/LCD/TFT display drivers
Watchdog implementation

### Sprint 5 (Weeks 9-10): LOW PRIORITY

RTOS wrapper integration
WiFi + MQTT + BLE modules
ESP32-CAM setup

### Sprint 6 (Weeks 11-12): LOW PRIORITY

Vision processing integration
Full system integration tests
Documentation & release

## pins

Perfect. Below is **ONE combined wiring diagram in text form**, with **only pins**, all modules together.
Assuming again: **STM32F446RE**, **ESP32-CAM (AI-Thinker)**, **ESP32-S2 Mini**, **SSD1306 OLED**, **16×2 LCD (4-bit)**, **4×4 Keypad**.

No extra explanation. Just connections.

---

# 🔗 COMBINED PIN CONNECTION DIAGRAM

---

## 1️⃣ STM32 ↔ ESP32-CAM (UART)

### STM32F446RE

* PA9  → USART1_TX
* PA10 → USART1_RX
* GND  → GND

### ESP32-CAM

* GPIO3 (U0R) ← RX
* GPIO1 (U0T) → TX
* GND → GND

---

## 2️⃣ STM32 ↔ ESP32-S2 Mini (CAN via Transceiver)

### STM32F446RE (BxCAN)

* PB9 → CAN_TX
* PB8 → CAN_RX
* GND → GND

### CAN Transceiver (SN65HVD230 / MCP2551)

* TXD ← STM32 CAN_TX
* RXD → STM32 CAN_RX
* CANH ↔ CANH
* CANL ↔ CANL

### ESP32-S2 Mini (TWAI)

* GPIO5 → TWAI_TX
* GPIO6 → TWAI_RX
* GND → GND

---

## 3️⃣ STM32 ↔ ESP32-CAM (SPI)

### STM32F446RE (SPI1)

* PA5 → SCK
* PA6 → MISO
* PA7 → MOSI
* PA4 → CS
* GND → GND

### ESP32-CAM

* GPIO14 → SCK
* GPIO12 → MISO
* GPIO13 → MOSI
* GPIO15 → CS
* GND → GND

---

## 4️⃣ STM32 ↔ OLED Display (I²C – SSD1306)

### STM32F446RE

* PB8 → I2C1_SCL
* PB9 → I2C1_SDA
* 3.3V → VCC
* GND → GND

### OLED

* SCL
* SDA
* VCC
* GND

---

## 5️⃣ STM32 ↔ LCD 16×2 (4-bit mode, no I²C)

### STM32F446RE

* PA0 → RS
* PA1 → EN
* PA2 → D4
* PA3 → D5
* PA4 → D6
* PA5 → D7
* GND → RW
* 5V  → VCC
* GND → GND

### LCD

* RS
* EN
* D4–D7
* RW
* VCC
* GND

---

## 6️⃣ STM32 ↔ 4×4 Keypad

### STM32F446RE

* PC0 → Row1
* PC1 → Row2
* PC2 → Row3
* PC3 → Row4
* PC4 → Col1
* PC5 → Col2
* PC6 → Col3
* PC7 → Col4

### Keypad

* R1–R4
* C1–C4

---

## 🔋 POWER (IMPORTANT)

* ESP32-CAM → **External 5V**
* STM32, ESP32-S2 → 3.3V
* **All GNDs COMMON**

---


# flow of code for stm32 

The Core Concept
The device acts as the central brain for a house. It has two distinct personalities:

The Gatekeeper (Security Mode): It is defensive. The system is locked, the screen is off (to save power), and it waits for an authorized user.

The Commander (Control Mode): Once unlocked, it becomes a dashboard. You can navigate menus to turn lights/fans on or off (simulated by LEDs) and check status.

Input: 4x4 Keypad (Used for PIN entry AND Menu Navigation).

Display: 16x2 LCD (Shows "Locked", "Enter PIN", "Living Room", "Light: ON").

Security Feedback: Red LED (Locked/Error), Green LED (Unlocked/Success), Buzzer (Alarm/Clicks).

Device Simulation: Use 2 extra LEDs to represent:

LED 3: Living Room Light.

LED 4: Garage Door / Kitchen Fan.
1) create code for keypad LCD stm32, with led and buzzer combination for pin (password code)
https://github.com/rahulbari717/User_management_system



2) oled display with rtc date and time 

3) then i2c, spi, can code 

4) then focus on esp32 idf codes.

5) 