# Smart Home Automation System - (STM32F446RE)

![Platforms](https://img.shields.io/badge/Platforms-STM32%20%7C%20ESP32-blue)
![Architecture](https://img.shields.io/badge/Architecture-Distributed%20%7C%20CAN%20%7C%20SPI-orange)
![RTOS](https://img.shields.io/badge/OS-FreeRTOS%20%7C%20ESP--IDF-purple)
![License](https://img.shields.io/github/license/rahulbari717/embedded-systems-toolkit?color=green)

**A bare-metal embedded home automation system with finite state machine architecture, featuring multi-sensor monitoring, device control, and secure keypad authentication.**


## 📋 Project Overview

This project implements a complete home automation controller using **STM32F446RE Nucleo-64** microcontroller. The system features a hierarchical finite state machine (FSM) for secure access control, real-time sensor monitoring, and automated device management—all without HAL or RTOS dependencies, using pure register-level drivers for maximum efficiency.

### Key Highlights
- ✅ **Bare-metal architecture** - Direct register access for all peripherals
- ✅ **Security-first design** - PIN authentication with lockout protection
- ✅ **State machine based** - Clean, maintainable FSM architecture
- ✅ **Dual display system** - OLED for status, LCD for interaction
- ✅ **Multi-sensor fusion** - Light, temperature, motion, proximity sensing
- ✅ **Automated control** - Smart lighting based on ambient conditions

---

## 🎯 System Architecture

### State Machine Flow

```
┌─────────────────┐
│    STANDBY      │◄────────────────┐
│  (Sleep Mode)   │                 │
└────────┬────────┘                 │
         │                          │
         │ Motion/Keypress          │ Timeout/Logout
         ▼                          │
┌─────────────────┐                 │
│ AUTHENTICATING  │────────────────►┤
│  (PIN Entry)    │    Invalid      │
└────────┬────────┘                 │
         │                          │
         │ Valid PIN                │
         ▼                          │
┌─────────────────┐                 │
│  ACTIVE_MENU    │◄───────┐        │
│ (Main Control)  │        │        │
└────────┬────────┘        │        │
         │                 │        │
         ├──► Sensor Monitor ───────┤
         ├──► Device Control ───────┤
         └──► Settings ─────────────┘
```

### State Definitions

| State | Description | Entry Condition | Exit Condition |
|-------|-------------|----------------|----------------|
| **STANDBY** | Low-power waiting mode | System boot / Logout | Motion detected / Key pressed |
| **AUTHENTICATING** | PIN verification | User trigger from STANDBY | Valid PIN / 3 failed attempts |
| **ACTIVE_MENU** | Main menu navigation | Successful authentication | User logout / Timeout |
| **SENSOR_MONITOR** | Live sensor data display | Selected from menu | Back button / Timeout |
| **CONTROL_DEVICES** | Device control interface | Selected from menu | Back button / Timeout |
| **SETTINGS** | Configuration menu | Selected from menu | Back button / Timeout |
| **LOCKOUT** | Security lockout (5 sec) | 3 failed login attempts | Timer expires |


### Input Devices
| Component | Interface | Pins | Purpose |
|-----------|-----------|------|---------|
| 4x4 Matrix Keypad | GPIO | PB0-PB7 | User input & navigation |
| User Button | GPIO + EXTI | PC13 | System wakeup |
| LDR Sensors (×2) | ADC | PA0, PA1 | Ambient light sensing |
| IR Proximity (×1) | GPIO | PC6 | Obstacle detection |

### Output Devices
| Component | Interface | Pins | Purpose |
|-----------|-----------|------|---------|
| LCD 16x2 | GPIO (4-bit) | PC0-PC5 | Interactive display |
| OLED 128x64 | I2C | PB8, PB9 | Status display |
| LEDs (×3) | GPIO | PA5-PA7 | Visual indicators |
| Relays (×4) | GPIO | PB12-PB15 | High-power device control |
| Buzzer | GPIO | PA4 | Audio feedback |

### Communication
| Interface | Pins | Purpose |
|-----------|------|---------|
| USART2 | PA2, PA3 | Debug console (115200 baud) |

---


## 📊 Complete Pin Mapping

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
PC13 → Wakeup Button             - System wakeup (EXTI)
```

---

## 🗂️ Project Structure

```
Home_Automation/
├── docs/                           # Documentation
│   ├── FRS.md                      # Functional Requirements Specification
│   ├── Project_summary.md          # Project overview
│   ├── State_machine_plan.md       # State machine design
│   ├── roadmap_plan.md             # Development roadmap
│   ├── test_cases.md               # Test plan & validation
│   ├── Quick_ref.md                # Quick reference guide
│   └── stm32f446re.jpg             # Pinout reference image
│
├── Home_Automation_stm32_drivers/  # Main firmware directory
│   ├── Application/                # Example/test programs
│   │   ├── Inc/
│   │   │   └── main.h
│   │   └── Src/
│   │       ├── 001_LedToggle.c     # Basic GPIO examples
│   │       ├── 005_UART_TX.c       # Communication examples
│   │       ├── 020_oled_i2c.c      # Display examples
│   │       └── ... (29 examples total)
│   │
│   ├── BSP/                        # Board Support Package
│   │   ├── Inc/
│   │   │   ├── bsp_init.h          # System initialization
│   │   │   ├── bsp_led.h           # LED control
│   │   │   ├── bsp_button.h        # Button handling
│   │   │   ├── bsp_buzzer.h        # Buzzer control
│   │   │   ├── bsp_relay.h         # Relay control
│   │   │   ├── bsp_keypad.h        # Keypad scanning
│   │   │   ├── bsp_lcd.h           # LCD driver (4-bit mode)
│   │   │   ├── bsp_i2c_oled.h      # OLED driver (I2C)
│   │   │   ├── bsp_sensors.h       # Sensor interfaces
│   │   │   ├── bsp_ldr.h           # LDR sensor
│   │   │   ├── bsp_ds18b20.h       # Temperature sensor
│   │   │   ├── bsp_uart2_debug.h   # Debug UART
│   │   │   ├── bsp_delay.h         # Timing utilities
│   │   │   └── config.h            # Hardware configuration
│   │   └── Src/
│   │       └── ... (BSP implementations)
│   │
│   ├── Drivers/                    # MCU Peripheral Drivers
│   │   ├── Inc/
│   │   │   ├── stm32f446xx.h       # MCU register definitions
│   │   │   ├── stm32f446xx_gpio_driver.h
│   │   │   ├── stm32f446xx_usart_driver.h
│   │   │   ├── stm32f446xx_i2c_driver.h
│   │   │   ├── stm32f446xx_adc_driver.h
│   │   │   ├── stm32f446xx_timer_driver.h
│   │   │   ├── stm32f446xx_dma_driver.h
│   │   │   ├── stm32f446xx_rtc_driver.h
│   │   │   ├── stm32f446xx_iwdg_driver.h
│   │   │   ├── stm32f446xx_rcc_driver.h
│   │   │   └── stm32f446xx_fault_handler.h
│   │   └── Src/
│   │       └── ... (Driver implementations)
│   │
│   ├── Inc/                        # Application headers
│   │   └── state_machine.h         # State machine declarations
│   │
│   ├── Src/                        # Application source
│   │   ├── main.c                  # Application entry point
│   │   ├── state_machine.c         # FSM core logic
│   │   ├── state_handlers.c        # State handler functions
│   │   ├── auth_menu.c             # Authentication & menus
│   │   ├── sensors.c               # Sensor management
│   │   ├── devices.c               # Device control
│   │   ├── display.c               # Display updates
│   │   ├── peripheral_test.c       # Hardware tests
│   │   ├── stm32f446xx_it.c        # Interrupt handlers
│   │   ├── syscalls.c              # System calls
│   │   └── sysmem.c                # Memory management
│   │
│   ├── Startup/
│   │   └── startup_stm32f446retx.s # Startup code
│   │
│   ├── STM32F446RETX_FLASH.ld      # Linker script (Flash)
│   └── STM32F446RETX_RAM.ld        # Linker script (RAM)
│
├── .gitignore                      # Git ignore rules
├── LICENSE                         # MIT License
└── README.md                       # This file
```



---

## ⚙️ Key Features

### 1. 🔐 Security & Authentication
- **PIN-based Access Control**
  - 4-digit PIN entry via keypad
  - Multiple user support (5 default users)
  - Master PIN override capability
- **Lockout Protection**
  - 3 failed attempts → 10-second security lockout
  - Buzzer alarm during lockout
  - Automatic return to standby after lockout
- **Session Management**
  - 30-second inactivity timeout
  - Secure logout with device shutdown
  - State persistence during session

### 2. 📊 Sensor Monitoring
- **Light Sensing (LDR)**
  - Dual LDR sensors (0-4095 ADC range)
  - Ambient light percentage calculation
  - Auto-lighting trigger threshold: 200 (darkness)
- **Proximity Detection**
  - IR sensor
  - Obstacle detection
  - Event logging via UART

### 3. 🎛️ Device Control
- **Manual Control Mode**
  - LED indicators (Green/Red/White)
  - 4-channel relay bank
  - Individual device toggle via menu
- **LDR Auto Mode**
  - Automatic lighting based on ambient light
  - LDR1 > 4000 → Relay1 ON
  - LDR2 > 4000 → Relay2 ON 
  - Enable/disable via control menu
- **Buzzer Patterns**
  - Success beep (short)
  - Error beep (toggle)

### 4. 📺 Display System
- **OLED (Status Display)**
  ```
  STATE: ACTIVE_MENU
  User: Rahul
  Motion: None
  ```
  - Shows current system state
  - Active user name
  - Motion sensor status
  - Updates every 1 second

- **LCD (Interactive Display)**
  - **Standby Mode:**
    ```
    System Ready
    Press Any Key
    ```
  - **Authentication:**
    ```
    Enter PIN:
    ****_
    ```
  - **Main Menu:**
    ```
    >1.Sensor Monitor
     2.Device Control
    ```
  - **Sensor Screens (Auto-cycling every 2s):**
    ```
    LDR1: 450 (45%)
    LDR2: 320 (32%)
    
    Motion: YES
    IR1:Y  
    ```
  - **Device Control:**
    ```
    >LED Green: ON
     LED Red: OFF
    ```

### 5. 🎮 Keypad Navigation

**Keypad Layout:**
```
┌───┬───┬───┬───┐
│ 1 │ 2 │ 3 │ A │
├───┼───┼───┼───┤
│ 4 │ 5 │ 6 │ B │
├───┼───┼───┼───┤
│ 7 │ 8 │ 9 │ C │
├───┼───┼───┼───┤
│ * │ 0 │ # │ D │
└───┴───┴───┴───┘
```

**Navigation Keys:**
- `2` → UP (Navigate up / Previous screen)
- `8` → DOWN (Navigate down / Next screen)
- `5` → ENTER (Select / Toggle device)
- `*` → BACK (Return to previous menu)
- `#` → LOGOUT (Return to standby & lock system)

### 6. 🔍 UART Monitoring
Complete system diagnostics via USART2 (115200 baud):
```
[STATE] STANDBY → AUTHENTICATING
[AUTH] User 'Rahul' login attempt
[AUTH] PIN verified - Access granted
[STATE] AUTHENTICATING → ACTIVE_MENU
[SENSOR] LDR1:450, LDR2:320
[DEVICE] LED_GREEN toggled ON
[CONTROL] Relay1 activated
```

---

## 🚀 Getting Started

### Prerequisites

**Hardware:**
- STM32F446RE Nucleo-64 board
- USB cable (for programming & debug UART)
- All components listed in hardware section
- Breadboard and jumper wires
- Power supply (5V for relays, 3.3V/5V for sensors)

**Software:**
- **STM32CubeIDE 1.18.0** or later
- **ARM GCC Toolchain** (bundled with CubeIDE)
- **OpenOCD** or **ST-Link Utilities** (for flashing)
- Serial terminal (PuTTY, Tera Term, or screen)

### Installation Steps

1. **Clone Repository**
   ```bash
   git clone https://github.com/rahulbari717/Home_Automation.git
   cd Home_Automation
   ```

2. **Open in STM32CubeIDE**
   - File → Open Projects from File System
   - Select `Home_Automation_stm32_drivers` folder
   - Click Finish

3. **Configure Build**
   - Right-click project → Properties
   - C/C++ Build → Settings
   - Verify optimization level:
     - Debug: `-O0` (no optimization)
     - Release: `-O2` (size optimization)

4. **Build Project**
   ```
   Project → Build Project (Ctrl+B)
   ```

5. **Flash to Board**
   ```
   Run → Debug (F11)
   ```
   Or using command line:
   ```bash
   openocd -f interface/stlink.cfg -f target/stm32f4x.cfg \
           -c "program build/Home_Automation.elf verify reset exit"
   ```

6. **Open Serial Monitor**
   ```bash
   # Linux
   minicom -D /dev/ttyACM0 -b 115200

   
   # Windows (PuTTY)
   COM Port: COMx (check Device Manager)
   Baud: 115200
   ```

---

## 📖 Usage Guide

### System Startup Sequence
```
[Power ON]
    ↓
[Initialize Peripherals]
    ↓
[Display Welcome Messages]
  OLED: "System Booting"
  LCD:  "Please Wait..."
    ↓
[White LED ON (Dim)]
    ↓
[Enter STANDBY Mode]
  OLED: "STATE: STANDBY"
  LCD:  "System Ready / Press Any Key"
    ↓
[System Ready - Waiting for trigger]
```

### Authentication Flow
```
[STANDBY] → Motion detected OR Key pressed
          → [AUTHENTICATING]
          → LCD shows: "Enter PIN: ____"
          → User enters 4-digit PIN
          → System verifies PIN against database
          
          ✓ Valid PIN:
            → Green LED ON + Success beep
            → [ACTIVE_MENU]
            → OLED shows username
            
          ✗ Invalid PIN:
            → Red LED ON + Error beep
            → Retry (max 3 attempts)
            → After 3 fails → [LOCKOUT] for 5 seconds
```

### Main Menu Navigation
```
ACTIVE_MENU:
┌────────────────────────┐
│ >1. Sensor Monitor     │  ← Current selection
│  2. Device Control     │
│  3. Settings           │
│  4. Logout             │
└────────────────────────┘

Actions:
- Press '2' to move down
- Press '8' to move up  
- Press '5' to select
- Press '*' to back
- Press '#' to logout
```

### Sensor Monitoring (Auto-Cycle Screens)
```
Screen 1 (2 seconds):
┌────────────────────────┐
│ LDR1: 450 (45%)        │
│ LDR2: 320 (32%)        │
└────────────────────────┘


Screen 3 (2 seconds):
┌────────────────────────┐
│ Motion: YES            │
│ IR1: Y                 │
└────────────────────────┘

(Cycles automatically, press '#' to exit)
```

### Device Control
```
┌────────────────────────┐
│ >LED Green: ON         │  ← Toggle with '5'
│  LED Red: OFF          │
│  LED White: OFF        │
│  Relay1: OFF           │
│  Relay2: OFF           │
│  LDR Auto: OFF         │
└────────────────────────┘

Actions:
- Navigate with '2'/'8'
- Toggle with '5' (instant beep feedback)
- Each action logged to UART
```

### LDR Auto Mode
```
When LDR Auto Mode is ENABLED:

1. System continuously reads LDR sensors

2. If LDR1 > 4000 (darkness):
   → Relay1 ON (room light)

   → UART log: "[AUTO] LDR1 dark - Lights ON"

3. If LDR1 > 4000 (darkness):
   → Relay2 ON (room light)
   → UART log: "[AUTO] LDR2 dark - Lights ON"

4. If light returns (LDR > 200):
   → LEDs/Relays OFF
   → UART log: "[AUTO] Light detected - Lights OFF"
```

---

## 👥 Default User Database

| Username | PIN  | Access Level |
|----------|------|--------------|
| Rahul    | 1234 | Full access  |
| Admin    | 0000 | Administrator|
| User1    | 1111 | Standard     |
| User2    | 2222 | Standard     |
| Guest    | 9999 | Limited      |


---

For detailed roadmap, see [`docs/roadmap_plan.md`](docs/roadmap_plan.md).

---

## 📚 Documentation

- **[Functional Requirements Specification (FRS)](docs/FRS.md)** - Complete system requirements
- **[Project Summary](docs/Project_summary.md)** - High-level overview
- **[State Machine Plan](docs/State_machine_plan.md)** - FSM design document
- **[Test Cases](docs/test_cases.md)** - Validation test plan
- **[Quick Reference](docs/Quick_ref.md)** - Pin mapping & command reference
- **[Roadmap](docs/roadmap_plan.md)** - Development timeline

---

## 🤝 Contributing

Contributions are welcome! Please:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit changes (`git commit -m 'Add amazing feature'`)
4. Push to branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

---

## 📊 Project Status

| Component | Status | Notes |
|-----------|--------|-------|
| **STM32 Core** | ✅ Complete | Bare-metal FSM working |
| **Authentication** | ✅ Complete | PIN system with lockout |
| **Sensors** | ✅ Partial | LDR + IR working |
| **Displays** | ✅ Complete | OLED + LCD working |
| **Control** | ✅ Complete | LEDs + Relays working |
| **Self-Test** | ✅ Complete | Boot diagnostics working |
| **ESP32 Gateway** | 🚧 Planned | Q2 2026 |
| **BLE/WiFi** | 🚧 Planned | Q2 2026 |
| **MQTT Cloud** | 🚧 Planned | Q3 2026 |

## 📄 License

This project is licensed under the **MIT License** - see the [LICENSE](LICENSE) file for details.

```
MIT License

Copyright (c) 2026 Rahul Bari

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

[Full license text in LICENSE file]
```

---

## 👨‍💻 Author

**Rahul Bari**  
Embedded Systems Developer

- 🌐 **LinkedIn:** [rahul-bari-embeddeddeveloper](https://linkedin.com/in/rahul-bari-embeddeddeveloper)
- 💻 **GitHub:** [@rahulbari717](https://github.com/rahulbari717)
- 📧 **Email:** rahulbari717@gmail.com

---

## ⭐ Support This Project

If you find this project helpful:

- ⭐ **Star this repository** on GitHub
- 🔀 **Fork and contribute** improvements
- 📢 **Share** with fellow embedded enthusiasts
- 💬 **Open issues** for bugs or feature requests
- 📝 **Write about it** - blog posts, tutorials, videos

---

**Last Updated:** January 30, 2026  
**Version:** 1.0  
**Platform:** STM32F446RE Nucleo-64  
**IDE:** STM32CubeIDE 1.18.0  
**OS:** Ubuntu 24.04

---

*Built with ❤️ for the embedded systems community # Smart Home Automation System - STM32F446RE
