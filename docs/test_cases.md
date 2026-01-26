# Acceptance Test Plan  
## The Citadel Home Automation System

This document provides a comprehensive step-by-step validation guide for the **Citadel Home Automation System**.  
It is designed to verify:

- Hardware integrity  
- Software state transitions  
- Precise timing for all integrated peripherals  
- Visual pin-level correctness based on `config.h`

---

## 1. Pre-Test Configuration

Ensure your environment matches these hardware and software specifications before starting the tests.

---

## 2. Complete Peripheral Pin Mapping & Visualization

Use this table to **visually trace and validate each physical pin** during testing.

| Peripheral              | MCU Port | Pin No | Signal Name        | Mode / AF       | Expected Behavior                              |
|-------------------------|----------|--------|--------------------|------------------|-----------------------------------------------|
| LDR Sensor 1            | GPIOA    | PA0    | ADC1_IN0           | Analog Input     | ADC value 0–4095                              |
| LDR Sensor 2            | GPIOA    | PA1    | ADC1_IN1           | Analog Input     | ADC value 0–4095                              |
| UART2 TX (Debug)       | GPIOA    | PA2    | USART2_TX          | AF7              | Serial TX output                              |
| UART2 RX (Debug)       | GPIOA    | PA3    | USART2_RX          | AF7              | Serial RX input                               |
| Buzzer                  | GPIOA    | PA4    | BUZZER             | GPIO Output      | High = ON, Low = OFF                          |
| LED Green               | GPIOA    | PA5    | LED_GREEN          | GPIO Output      | High = ON                                     |
| LED Red                 | GPIOA    | PA6    | LED_RED            | GPIO Output      | High = ON                                     |
| LED White               | GPIOA    | PA7    | LED_WHITE          | GPIO Output      | High = ON                                     |
| DS18B20 Data            | GPIOA    | PA8    | 1-Wire Data        | GPIO Open-Drain  | Temp read pulse train                         |
| Bluetooth TX            | GPIOA    | PA9    | USART1_TX          | AF7              | Serial TX output                              |
| Bluetooth RX            | GPIOA    | PA10   | USART1_RX          | AF7              | Serial RX input                               |
| Keypad Row 0            | GPIOB    | PB0    | R0                 | GPIO Output      | Scan line toggles                             |
| Keypad Row 1            | GPIOB    | PB1    | R1                 | GPIO Output      | Scan line toggles                             |
| Keypad Row 2            | GPIOB    | PB2    | R2                 | GPIO Output      | Scan line toggles                             |
| Keypad Row 3            | GPIOB    | PB3    | R3                 | GPIO Output      | Scan line toggles                             |
| Keypad Column 0         | GPIOB    | PB4    | C0                 | GPIO Input PU    | Reads LOW on key press                       |
| Keypad Column 1         | GPIOB    | PB5    | C1                 | GPIO Input PU    | Reads LOW on key press                       |
| Keypad Column 2         | GPIOB    | PB6    | C2                 | GPIO Input PU    | Reads LOW on key press                       |
| Keypad Column 3         | GPIOB    | PB7    | C3                 | GPIO Input PU    | Reads LOW on key press                       |
| OLED SCL                | GPIOB    | PB8    | I2C1_SCL           | AF4              | Clock pulses ~100 kHz                         |
| OLED SDA                | GPIOB    | PB9    | I2C1_SDA           | AF4              | Data line toggles                             |
| RCWL Radar Sensor       | GPIOB    | PB10   | RCWL_OUT           | GPIO Input       | High on motion                               |
| KY-028 Digital          | GPIOB    | PB11   | TEMP_ALARM         | GPIO Input       | High when threshold crossed                  |
| Relay 1                 | GPIOB    | PB12   | RELAY1             | GPIO Output      | High = ON                                     |
| Relay 2                 | GPIOB    | PB13   | RELAY2             | GPIO Output      | High = ON                                     |
| Relay 3                 | GPIOB    | PB14   | RELAY3             | GPIO Output      | High = ON                                     |
| Relay 4                 | GPIOB    | PB15   | RELAY4             | GPIO Output      | High = ON                                     |
| LCD RS                  | GPIOC    | PC0    | LCD_RS             | GPIO Output      | Register Select                              |
| LCD EN                  | GPIOC    | PC1    | LCD_EN             | GPIO Output      | Enable pulse                                 |
| LCD D4                  | GPIOC    | PC2    | LCD_D4             | GPIO Output      | Data nibble                                  |
| LCD D5                  | GPIOC    | PC3    | LCD_D5             | GPIO Output      | Data nibble                                  |
| LCD D6                  | GPIOC    | PC4    | LCD_D6             | GPIO Output      | Data nibble                                  |
| LCD D7                  | GPIOC    | PC5    | LCD_D7             | GPIO Output      | Data nibble                                  |
| IR Sensor 1             | GPIOC    | PC6    | IR1                | GPIO Input       | High on obstacle                             |
| IR Sensor 2             | GPIOC    | PC7    | IR2                | GPIO Input       | High on obstacle                             |
| Wakeup Button           | GPIOC    | PC13   | WAKEUP_BTN         | GPIO Input + EXTI| Falling edge interrupt                      |

---

## 3. Phase 1: System Boot & Initialization

**Objective:**  
Verify the master boot sequence and entry into Standby mode.

| Step | Action                          | Observation        | Expected Result                                               |
|------|----------------------------------|--------------------|---------------------------------------------------------------|
| 1.1  | Apply power / Reset board        | Immediate          | White LED (PA7) turns ON                                      |
| 1.2  | Observe UART2 Terminal           | T = 0 s            | Terminal prints: `[LOG] SYSTEM BOOT: THE CITADEL ONLINE`      |
| 1.3  | Observe OLED                     | T = 0–2 s          | Splash screen: `"THE CITADEL v2"`                             |
| 1.4  | Observe LCD                      | T = 0–2 s          | Displays: `System Booting..`                                 |
| 1.5  | Wait for initialization          | T = 2 s            | OLED shows dashboard with live Temp and LDR values           |

---

## 4. Phase 2: Autonomous Reporting (Telemetrics)

**Objective:**  
Confirm that non-blocking background tasks report data at correct intervals.

| Step | Metric         | Timing      | Expected Observation                                           |
|------|----------------|-------------|----------------------------------------------------------------|
| 2.1  | LDR Reporting  | Every 1 s   | UART2 & BT show LDR1 (PA0) and LDR2 (PA1) raw values            |
| 2.2  | Temp Reporting | Every 3 s   | Bluetooth prints: `TEMP: XX.XX C`                              |
| 2.3  | OLED Refresh   | Continuous | Dashboard updates without flicker or blocking key presses     |

---

## 5. Phase 3: Remote Actuator Control (Bluetooth)

**Objective:**  
Verify serial command processing and OLED status feedback.

| CMD | Terminal Action       | Hardware Response                      | OLED Status Text |
|-----|-----------------------|----------------------------------------|------------------|
| `R` | Send `R` via BT       | Red LED (PA6) turns ON                 | RED ON           |
| `r` | Send `r` via BT       | Red LED (PA6) turns OFF                | RED OFF          |
| `G` | Send `G` via BT       | Green LED (PA5) turns ON               | GRN ON           |
| `g` | Send `g` via BT       | Green LED (PA5) turns OFF              | GRN OFF          |
| `B` | Send `B` via BT       | Buzzer (PA4) sounds continuously       | BEEP ON          |
| `b` | Send `b` via BT       | Buzzer (PA4) turns OFF                 | BEEP OFF         |
| `W` | Send `W` via BT       | White LED (PA7) toggles state          | N/A              |

---

## 6. Phase 4: Local Interface & Relay Logic

**Objective:**  
Verify relay control and state transitions via keypad.

| Step | Action                     | Expected Result                                                                 | Observation |
|------|----------------------------|----------------------------------------------------------------------------------|-------------|
| 4.1  | Press Blue Button (PC13)   | White LED OFF; Green LED ON; LCD: `CITADEL ACTIVE`                              | [ ]         |
| 4.2  | Press Keypad `1`           | Relay 1 (PB12) toggles; UART2: `RELAY TOGGLED`                                 | [ ]         |
| 4.3  | Press Keypad `2`           | Relay 2 (PB13) toggles; UART2: `RELAY TOGGLED`                                 | [ ]         |
| 4.4  | Press Keypad `3`           | Relay 3 (PB14) toggles; UART2: `RELAY TOGGLED`                                 | [ ]         |
| 4.5  | Press Keypad `4`           | Relay 4 (PB15) toggles; UART2: `RELAY TOGGLED`                                 | [ ]         |
| 4.6  | Press Keypad `#`           | Green LED OFF; White LED ON; Returns to Standby                                | [ ]         |

---

## 7. Precise Timing & Bug Verification

**Objective:**  
Verify that specific timing logic fixes are working correctly.

- **LCD Stability**  
  Verify PC0–PC5 toggle cleanly and the LCD does not show garbled text.  
  Confirms the 2 ms delay after `LCD_CMD_CLEAR` is effective.

- **Buzzer Response**  
  Toggle PA4 using `B` then `b`.  
  Ensure immediate response without blocking delay.

- **ADC Polling**  
  Cover LDR1 (PA0).  
  Ensure terminal value updates within 1 second.  
  Confirms `ADC_SR_EOC` polling is working.

- **I2C Communications**  
  Probe PB8 (SCL) and PB9 (SDA).  
  Ensure OLED updates without system hang.  
  Confirms correct 7-bit addressing for `0x3C`.

- **EXTI Wakeup Button**  
  Press PC13.  
  Ensure system transitions from Standby to Active.  
  Confirms EXTI15_10 IRQ mapping.


## For Future Sensor Expansion (Free & Safe Pins)

### 🔵 GPIOA  
**Used:** PA0–PA10  

**Free (Safe):**  
- PA11  
- PA12  
- PA15  

**Avoid:**  
- PA13 (SWDIO)  
- PA14 (SWCLK)

---

### 🟢 GPIOB  
**Used:** PB0–PB15  

**Free:**  
- None

---

### 🟣 GPIOC  
**Used:** PC0–PC7, PC13  

**Free (Safe):**  
- PC8  
- PC9  
- PC10  
- PC11  
- PC12
---

## End of Test Plan
