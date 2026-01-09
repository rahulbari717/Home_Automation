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
