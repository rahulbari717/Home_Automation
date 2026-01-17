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
  - [ ] `led.c` (Status Indication) is remaing ... imp
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

### TO do : 
1) LCD 16*2 display
2) oled display 
3) clk and pll
4) timer 
5) timer input capture 
6) timer output capture 
7) pwm 
8) rtc : calender (date and time), alarm, interrupt 
9) can 
10) Low power modes 
11) RTOS 

