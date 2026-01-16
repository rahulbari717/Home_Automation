# The Citadel: Driver Development Roadmap & Implementation Plan

This document outlines the step-by-step plan for building the drivers and modules for "The Citadel" distributed embedded system. We will follow a **Bottom-Up Approach**: building low-level drivers first, testing them in isolation, and then integrating them.

---

## 🏗️ 1. Project Structure Overview

We will maintain a clean separation between **Hardware Abstraction Layer (HAL/Drivers)** and **Application Logic**. We will use shared folders to avoid duplicating code.

```text
Home_Automation
├── docs
│   ├── FRS.md
│   ├── High-Level-Design.md
│   ├── Low-Level-Design.md
│   └── roadmap_plan.md
├── Home_Automation_stm32_drivers
│   ├── Application
│   │   ├── Inc
│   │   └── Src
│   │       ├── 001_LedToggle.c
│   │       ├── 002_ledButton.c
│   │       ├── 003_buttonInterrupt.c
│   │       ├── 004_buttonInterruptRFT.c
│   │       ├── 005_UART_TX.c
│   │       ├── 006_Keypad.c
│   │       └── 007_rtc_uart.c
│   ├── BSP
│   │   ├── Inc
│   │   │   ├── config.h
│   │   │   └── keypad.h
│   │   └── Src
│   │       └── keypad.c
│   ├── Drivers
│   │   ├── Inc
│   │   │   ├── stm32f446xx_fault_handler.h
│   │   │   ├── stm32f446xx_gpio_driver.h
│   │   │   ├── stm32f446xx.h
│   │   │   ├── stm32f446xx_i2c_driver.h
│   │   │   ├── stm32f446xx_rcc_driver.h
│   │   │   ├── stm32f446xx_rtc_driver.h
│   │   │   ├── stm32f446xx_spi_driver.h
│   │   │   └── stm32f446xx_usart_driver.h
│   │   └── Src
│   │       ├── stm32f446xx_fault_handler.c
│   │       ├── stm32f446xx_gpio_driver.c
│   │       ├── stm32f446xx_i2c_driver.c
│   │       ├── stm32f446xx_rcc_driver.c
│   │       ├── stm32f446xx_rtc_driver.c
│   │       ├── stm32f446xx_spi_driver.c
│   │       └── stm32f446xx_usart_driver.c
│   ├── Home_Automation_stm32_drivers.launch
│   ├── Inc
│   ├── Src
│   │   ├── main.c
│   │   ├── syscalls.c
│   │   └── sysmem.c
│   ├── Startup
│   │   └── startup_stm32f446retx.s
│   ├── STM32F446RETX_FLASH.ld
│   └── STM32F446RETX_RAM.ld
├── LICENSE
└── README.md
```

---

## 🚦 Project Status Dashboard

| Layer | Component | Status | Description |
| :--- | :--- | :--- | :--- |
| **Driver** | Fault Handling | ✅ Done | HardFault, BusFault, UsageFault decoding |
| **Driver** | Core Drivers | ✅ Done | GPIO, RCC, UART, SPI, I2C, RTC |
| **Driver** | Analog / Timing | 🚧 In Progress | ADC, Timers, PWM,  |
| **BSP** | Digital IO | 🟡 Partial | Keypad done. LED/Buzzer pending |
| **BSP** | Displays | 🛑 Pending | OLED (I2C), LCD (Parallel) |
| **App** | Logic | 🟡 Partial | CLI Debugging, RTC Calendar |

---

## 1. 🏎️ MCAL: Peripheral Drivers (Register Level)
*Low-level drivers implementing the STM32F4 Reference Manual specifications.*

### ✅ Completed Drivers
* **RCC Driver** (`stm32f446xx_rcc_driver.h`)
  - [x] HSE/HSI Clock Configuration
  - [x] Peripheral Clock Control (AHB/APB)
  - [x] PLL Config (System Clock Speed)

* **GPIO Driver** (`stm32f446xx_gpio_driver.h`)
  - [x] Input/Output/Alternate Functions
  - [x] Speed, Pull-up/down, Output types (Push-pull/Open-drain)
  - [x] Interrupts (Rising/Falling Edge)

* **UART Driver** (`stm32f446xx_usart_driver.h`)
  - [x] Baud Rate Calculation (Fractional/Mantissa)
  - [x] Blocking TX/RX
  - [x] Interrupt-based TX/RX (`USART_IRQHandling`)
  - [x] `printf` redirection for CLI debugging

* **SPI Driver** (`stm32f446xx_spi_driver.h`)
  - [x] Master/Slave Configuration
  - [x] Software Slave Management (SSM)
  - [x] Full-Duplex Communication
  - [x] CPOL/CPHA Mode Setup

* **I2C Driver** (`stm32f446xx_i2c_driver.h`)
  - [x] Standard Mode (100kHz) & Fast Mode (400kHz)
  - [x] ACK Control & Address Management
  - [x] Interrupt Handling (Event & Error IRQs)

* **RTC Driver** (`stm32f446xx_rtc_driver.h`)
  - [x] Clock Source Selection (LSI/LSE)
  - [x] Calendar Management (Date/Time)
  - [x] Backup Domain Reset & Protection

* **System Safety** (`stm32f446xx_fault_handler.h`)
  - [x] **SCB Implementation:** System Control Block register definitions
  - [x] **Crash Forensics:** Stack dumping on HardFault/UsageFault

### 🚧 Drivers in Development (Next Sprints)
- [ ] **ADC Driver:** Single Channel Polling & EOC Interrupts
- [ ] **Timer Driver:** Basic microsecond delays (TIM6)
- [ ] **PWM Driver:** Output Compare mode for Motor/LED control
- [ ] **Watchdog Driver:** IWDG implementation for system reliability

---

## 2. 🧱 BSP: Board Support Package
*Hardware Abstraction Layer. Decouples application logic from specific pins/ports.*

### ✅ Completed Modules
- [x] **Matrix Keypad** (`keypad.c`) 
  - [x] Row/Column Scanning Algorithm
  - [x] Blocking Input

### 🚧 Modules in Development
- [ ] **Basic IO:**
  - [ ] `led.c` (Status Indication)
  - [ ] `buzzer.c` (PWM Tone Generation)
  - [ ] `relay.c` (Active Low Logic)
  - [ ] `button.c` (Software Debouncing)
- [ ] **Displays:**
  - [ ] `lcd1602.c` (4-bit Parallel Mode)
  - [ ] `ssd1306.c` (OLED I2C Framebuffer)

---

## 3. 🧠 Application Layer
*Business logic and System Integration.*

### ✅ Completed Apps
- [x] **System Boot:** Logging boot messages via UART
- [x] **RTC Calendar:** Interactive Time/Date setting via Terminal

### 🚧 Upcoming Features (The "Citadel" Logic)
- [ ] **Security System:**
  - User Authentication (PIN Entry)
  - Lockout Logic (3 failed attempts)
- [ ] **Home Control:**
  - Menu Navigation System (using Keypad + Display)
  - Device Control (Relays/PWM)
---


Phase 1: Fill the "Driver" GapsYour current Drivers folder has GPIO, I2C, UART, and RTC. To support your hardware list, you need to add two critical low-level drivers:ADC Driver (stm32f446xx_adc_driver.c)Why: You have 5 LDRs. Reading analog light intensity requires the ADC peripheral.Features needed: Single conversion mode (polling is fine for LDRs), Scan mode (to read 5 channels sequentially).Timer/PWM Driver (stm32f446xx_timer_driver.c)Why: You have 2 Servo Motors and 1 Ultrasonic Sensor.Features needed:PWM Output: To drive the Servos (usually 50Hz signal).Input Capture (or Microsecond Delay): To measure the echo pulse width of the HC-SR04 ultrasonic sensor.Phase 2: Expand the "BSP" LayerYou should not clutter your main.c with raw driver calls. Use your BSP folder to create "Component Drivers." This abstracts the hardware complexity.Create the following files in BSP/Src and BSP/Inc:bsp_lcd_16x2.c: Since you are using it without I2C, you need a parallel GPIO driver (4-bit mode is recommended to save pins).Functions: LCD_Init(), LCD_SendCommand(), LCD_String().bsp_ssd1306.c: For the I2C OLED. You will implement the SSD1306 command set using your existing stm32f446xx_i2c_driver.c.Functions: OLED_Init(), OLED_DrawPixel(), OLED_UpdateScreen().bsp_sensors.c: Group your simple sensors here.Functions: LDR_GetValue(uint8_t sensor_id), PIR_GetStatus(), Radar_GetStatus().bsp_actuators.c: Group Relays and Buzzers.Functions: Relay_Control(uint8_t relay_id, uint8_t state), Buzzer_Beep(uint16_t duration_ms).Phase 3: Hardware Pin Mapping Strategy (STM32F446RE)With this many components, pin conflicts are your biggest risk. Here is a suggested allocation strategy:ComponentInterfaceDriver RequiredSuggested Pins (F446RE)5x LDRsAnalog InADCPA0, PA1, PA4, PB0, PC12x ServosPWMTimer (TIM2)PA5, PB3 (Alt Functions)HC-SR04GPIO (Trig) + Input Capture (Echo)Timer / GPIOPC2 (Trig), PC3 (Echo)2x PIRGPIO Input (Interrupt)GPIO / EXTIPC13, PC142x RadarGPIO Input (Interrupt)GPIO / EXTIPH0, PH1KeypadGPIO Matrix (4x4 or 4x3)GPIOPB12-PB15 (Rows), PC6-PC9 (Cols)LCD 16x2GPIO (Parallel)GPIOPC0-PC5 (Control + Data)OLEDI2CI2C1PB6 (SCL), PB9 (SDA)ESP32 LinkUARTUSART2PA2 (TX), PA3 (RX)4x RelaysGPIO OutputGPIOPB1, PB2, PB10, PB4Phase 4: The Application Logic (State Machine)Since you are a "bare-metal" developer, avoid a "spaghetti" main loop. Use a circular checking approach or a simple scheduler.Proposed Logic Flow for main.c:SysInit: Initialize Clocks, Enable GPIO/ADC/I2C/UART/Timers.BSP_Init: Initialize LCD, OLED, Relays (OFF), Servos (0 position).Communication Handshake: Send "STM32 Ready" to ESP32 via UART.While(1) Super Loop:Task 1: Read Inputs (Non-blocking)Scan Keypad.Check PIR/Radar GPIOs.Start ADC conversion for LDRs.Task 2: Process LogicIf (LDR < Threshold) -> Turn ON Relay 1 (Light).If (PIR Detects Motion) -> Trigger Buzzer + Send UART Alert to ESP32.If (Keypad Password Correct) -> Move Servo (Unlock Door).Task 3: Update OutputsRefresh LCD/OLED with sensor status.Task 4: UART CommsCheck if ESP32 sent a command (e.g., "Turn Relay 2 ON" via WiFi) -> Execute it.Phase 5: ESP32 Integration (The "Brain")The ESP32 will handle the IoT side.Role: It connects to WiFi/MQTT.Code: It listens to the Serial Port (connected to STM32).Action:When STM32 sends {"alert": "motion_detected"}, ESP32 pushes an MQTT message to your phone/dashboard.When you press a button on your phone, ESP32 receives MQTT and sends CMD:RELAY1:ON to STM32 over UART.Recommended Next StepYou currently lack the ADC Driver and PWM generation, which are blocking the LDRs and Servos.Would you like me to generate the stm32f446xx_adc_driver.c skeleton code or the logic for the HC-SR04 Ultrasonic sensor using the standard SysTick timer?