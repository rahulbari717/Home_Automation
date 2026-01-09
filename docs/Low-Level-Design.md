# Low-Level Design (LLD): The Citadel System
**Version:** 1.0  
**Date:** January 2026  
**Author:** Rahul Bari.  
**Status:** Implementation Ready

---

## 1. Document Purpose

This Low-Level Design (LLD) provides **implementation-ready specifications** for developers. It includes:
- Detailed state machines
- API function signatures
- Memory layouts
- Register configurations
- Code examples
- Interrupt handling logic

---

## 2. STM32F446RE Master Node - Detailed Design

### 2.1 FreeRTOS Task Implementation

#### Task 1: vTaskControl (High Priority)

**Purpose:** Real-time control loop for safety-critical operations

**Pseudocode:**
```c
void vTaskControl(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(10); // 10ms period
    
    for(;;) {
        // 1. Read all safety sensors
        bool fire_detected = HAL_GPIO_ReadPin(FIRE_SENSOR_PORT, FIRE_SENSOR_PIN);
        bool gas_leak = (adc_read_gas_sensor() > GAS_THRESHOLD);
        bool intrusion = (pir_sensor_read() || radar_sensor_read());
        
        // 2. State machine update
        safety_state_machine_update(fire_detected, gas_leak, intrusion);
        
        // 3. Actuate outputs
        if (fire_detected) {
            can_send_emergency_frame(CAN_ID_UNLOCK_ALL_DOORS);
            buzzer_activate(BUZZER_FIRE_PATTERN);
        }
        
        // 4. Feed watchdog
        HAL_IWDG_Refresh(&hiwdg);
        
        // Delay until next period
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}
```

**Stack Allocation:** 512 bytes (monitored via `uxTaskGetStackHighWaterMark()`)

---

#### Task 2: vTaskComms (Medium Priority)

**Purpose:** Handle all communication buses (CAN, SPI, UART)

**Pseudocode:**
```c
void vTaskComms(void *pvParameters) {
    can_frame_t tx_frame;
    spi_telemetry_packet_t spi_data;
    
    for(;;) {
        // 1. Check CAN TX queue
        if (xQueueReceive(can_tx_queue, &tx_frame, 0) == pdTRUE) {
            HAL_CAN_AddTxMessage(&hcan1, &tx_frame.header, tx_frame.data, &can_tx_mailbox);
        }
        
        // 2. Prepare SPI telemetry packet
        spi_data.temperature = dht11_get_temperature();
        spi_data.humidity = dht11_get_humidity();
        spi_data.light_levels[0] = adc_read_ldr(0);
        // ... (populate all sensor data)
        
        // 3. DMA transfer to ESP32 Pico
        HAL_SPI_Transmit_DMA(&hspi2, (uint8_t*)&spi_data, sizeof(spi_data));
        
        // 4. Check UART RX from ESP32-CAM
        uart_process_camera_events();
        
        vTaskDelay(pdMS_TO_TICKS(50)); // 50ms period
    }
}
```

---

#### Task 3: vTaskUI (Low Priority)

**Purpose:** Update 16×2 LCD and process keypad input

**Pseudocode:**
```c
void vTaskUI(void *pvParameters) {
    char lcd_line1[17], lcd_line2[17];
    
    for(;;) {
        // 1. Format LCD strings
        snprintf(lcd_line1, 17, "Temp:%02dC Hum:%02d%%", get_temperature(), get_humidity());
        snprintf(lcd_line2, 17, "Mode:%s Safe:%s", get_mode_string(), is_safe() ? "OK" : "ALERT");
        
        // 2. Update LCD
        lcd_clear();
        lcd_set_cursor(0, 0);
        lcd_print(lcd_line1);
        lcd_set_cursor(1, 0);
        lcd_print(lcd_line2);
        
        // 3. Scan keypad
        char key = keypad_scan();
        if (key != 0) {
            process_keypad_input(key);
        }
        
        vTaskDelay(pdMS_TO_TICKS(500)); // 500ms update
    }
}
```

---

### 2.2 CAN Bus Protocol Specification

#### Frame Format

| Byte | Field | Description |
|------|-------|-------------|
| 0-1 | CAN ID (11-bit) | Message identifier (see ID table below) |
| 2 | Command | Action code (0x01 = SET, 0x02 = GET, 0x03 = EVENT) |
| 3 | Target | Device/relay number (0x00-0x04) |
| 4-7 | Data | Payload (e.g., relay state, sensor value) |

#### CAN ID Allocation

| ID Range | Purpose | Priority |
|----------|---------|----------|
| 0x000-0x0FF | Safety-critical (fire, gas, intrusion) | Highest |
| 0x100-0x1FF | Control commands (relay actuation) | High |
| 0x200-0x2FF | Telemetry (sensor readings) | Medium |
| 0x300-0x3FF | Diagnostics (heartbeat, status) | Low |

#### Example: Unlock Door Command

```c
void can_send_unlock_door(uint8_t door_id) {
    CAN_TxHeaderTypeDef header;
    uint8_t data[8] = {0};
    
    header.StdId = 0x101; // Control command
    header.IDE = CAN_ID_STD;
    header.RTR = CAN_RTR_DATA;
    header.DLC = 8;
    
    data[0] = 0x01; // Command: SET
    data[1] = door_id; // Target relay
    data[2] = 0x01; // State: ON (unlock)
    
    HAL_CAN_AddTxMessage(&hcan1, &header, data, &can_tx_mailbox);
}
```

---

### 2.3 SPI DMA Transfer Implementation

**Data Structure:**
```c
typedef struct {
    float temperature;      // 4 bytes
    float humidity;         // 4 bytes
    uint16_t light_levels[5]; // 10 bytes (5 LDRs)
    uint16_t gas_ppm;       // 2 bytes
    uint16_t distance_cm;   // 2 bytes
    uint8_t pir_status;     // 1 byte
    uint8_t ir_status;      // 1 byte
    uint8_t radar_status;   // 1 byte
    uint8_t reserved[7];    // Padding to 32 bytes
} __attribute__((packed)) spi_telemetry_packet_t;
```

**DMA Configuration:**
```c
void spi_dma_init(void) {
    // Configure SPI2 for Master Mode
    hspi2.Instance = SPI2;
    hspi2.Init.Mode = SPI_MODE_MASTER;
    hspi2.Init.Direction = SPI_DIRECTION_2LINES;
    hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi2.Init.NSS = SPI_NSS_SOFT;
    hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16; // 4 MHz
    hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
    HAL_SPI_Init(&hspi2);
    
    // Configure DMA for SPI2_TX
    hdma_spi2_tx.Instance = DMA1_Stream4;
    hdma_spi2_tx.Init.Channel = DMA_CHANNEL_0;
    hdma_spi2_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_spi2_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_spi2_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_spi2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_spi2_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_spi2_tx.Init.Mode = DMA_NORMAL;
    hdma_spi2_tx.Init.Priority = DMA_PRIORITY_MEDIUM;
    HAL_DMA_Init(&hdma_spi2_tx);
    
    __HAL_LINKDMA(&hspi2, hdmatx, hdma_spi2_tx);
}
```

**Callback:**
```c
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) {
    if (hspi->Instance == SPI2) {
        // SPI transfer complete - ready for next packet
        spi_transfer_complete = true;
    }
}
```

---

### 2.4 UART Protocol for ESP32-CAM

**Frame Format:** ASCII commands terminated with `\n`

| Command | Direction | Meaning | Example |
|---------|-----------|---------|---------|
| `FACE_OK` | CAM → STM32 | Known face detected | `FACE_OK:John\n` |
| `FACE_UNKNOWN` | CAM → STM32 | Unknown face | `FACE_UNKNOWN\n` |
| `MOTION` | CAM → STM32 | Motion detected | `MOTION:HIGH\n` |
| `HEARTBEAT` | CAM → STM32 | Alive signal | `HEARTBEAT\n` |
| `CAPTURE` | STM32 → CAM | Take photo | `CAPTURE\n` |
| `STREAM_START` | STM32 → CAM | Start video stream | `STREAM_START\n` |

**Implementation:**
```c
#define UART_RX_BUFFER_SIZE 128
uint8_t uart_rx_buffer[UART_RX_BUFFER_SIZE];
volatile uint8_t uart_rx_index = 0;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) {
        uint8_t received_byte = uart_rx_buffer[uart_rx_index];
        
        if (received_byte == '\n') {
            // Parse command
            uart_rx_buffer[uart_rx_index] = '\0';
            parse_camera_command((char*)uart_rx_buffer);
            uart_rx_index = 0;
        } else {
            uart_rx_index = (uart_rx_index + 1) % UART_RX_BUFFER_SIZE;
        }
        
        // Re-enable interrupt
        HAL_UART_Receive_IT(&huart2, &uart_rx_buffer[uart_rx_index], 1);
    }
}

void parse_camera_command(char *cmd) {
    if (strncmp(cmd, "FACE_OK", 7) == 0) {
        // Unlock door
        can_send_unlock_door(DOOR_MAIN);
    } else if (strncmp(cmd, "MOTION", 6) == 0) {
        // Trigger alarm
        safety_state = STATE_ALERT;
    }
    // ... other commands
}
```

---

### 2.5 Safety State Machine

**States:**
```c
typedef enum {
    STATE_SAFE,        // All sensors normal
    STATE_WARNING,     // Minor anomaly detected
    STATE_ALERT,       // Safety violation (gas/fire/intrusion)
    STATE_EMERGENCY,   // Critical failure (requires evacuation)
    STATE_FAULT        // System malfunction
} safety_state_t;
```

**State Diagram:**
```
         ┌──────────┐
         │   SAFE   │◄──────┐
         └────┬─────┘       │
              │             │
       Sensor │             │ Timeout
       Anomaly│             │ (30s)
              ▼             │
         ┌──────────┐       │
         │ WARNING  ├───────┘
         └────┬─────┘
              │
         Fire/│Gas/
         Intr.│
              ▼
         ┌──────────┐
         │  ALERT   │
         └────┬─────┘
              │
      Critical│Failure
        (CAN │Down)
              ▼
         ┌──────────┐
         │EMERGENCY │
         └──────────┘
```

**Implementation:**
```c
void safety_state_machine_update(bool fire, bool gas, bool intrusion) {
    static uint32_t warning_timer = 0;
    
    switch (current_state) {
        case STATE_SAFE:
            if (fire || gas || intrusion) {
                current_state = STATE_ALERT;
                buzzer_activate(PATTERN_ALERT);
            } else if (sensor_reading_abnormal()) {
                current_state = STATE_WARNING;
                warning_timer = HAL_GetTick();
            }
            break;
            
        case STATE_WARNING:
            if (fire || gas || intrusion) {
                current_state = STATE_ALERT;
            } else if (HAL_GetTick() - warning_timer > 30000) {
                current_state = STATE_SAFE; // Timeout
            }
            break;
            
        case STATE_ALERT:
            can_send_emergency_frame(CAN_ID_ALERT);
            if (can_bus_error || watchdog_triggered) {
                current_state = STATE_EMERGENCY;
                unlock_all_doors();
            }
            break;
            
        case STATE_EMERGENCY:
            // Sound continuous alarm, unlock all exits
            buzzer_activate(PATTERN_CONTINUOUS);
            // System requires manual reset
            break;
    }
}
```

---

## 3. ESP32-S2 Industrial Node - Detailed Design

### 3.1 CAN Bus Driver (TWAI)

**Initialization:**
```c
#include "driver/twai.h"

void can_init(void) {
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(GPIO_NUM_18, GPIO_NUM_19, TWAI_MODE_NORMAL);
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
    
    // Install TWAI driver
    ESP_ERROR_CHECK(twai_driver_install(&g_config, &t_config, &f_config));
    ESP_ERROR_CHECK(twai_start());
}
```

**RX Task:**
```c
void can_rx_task(void *pvParameters) {
    twai_message_t rx_msg;
    
    while (1) {
        if (twai_receive(&rx_msg, pdMS_TO_TICKS(1000)) == ESP_OK) {
            // Parse message
            if (rx_msg.identifier == 0x101) { // Relay control
                uint8_t relay_id = rx_msg.data[1];
                uint8_t state = rx_msg.data[2];
                relay_set(relay_id, state);
            }
        }
    }
}
```

---

### 3.2 Relay Driver with Optocoupler

**Hardware Interface:**
```c
#define RELAY1_GPIO GPIO_NUM_4
#define RELAY2_GPIO GPIO_NUM_5
#define RELAY3_GPIO GPIO_NUM_6
#define RELAY4_GPIO GPIO_NUM_7
#define RELAY5_GPIO GPIO_NUM_8

const gpio_num_t relay_pins[5] = {RELAY1_GPIO, RELAY2_GPIO, RELAY3_GPIO, RELAY4_GPIO, RELAY5_GPIO};

void relay_init(void) {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << RELAY1_GPIO) | (1ULL << RELAY2_GPIO) | 
                        (1ULL << RELAY3_GPIO) | (1ULL << RELAY4_GPIO) | 
                        (1ULL << RELAY5_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);
    
    // All relays OFF (active LOW)
    for (int i = 0; i < 5; i++) {
        gpio_set_level(relay_pins[i], 1);
    }
}

void relay_set(uint8_t relay_id, bool state) {
    if (relay_id < 5) {
        gpio_set_level(relay_pins[relay_id], !state); // Active LOW
    }
}
```

---

### 3.3 DHT11 Sensor Driver

```c
#include "driver/rmt.h"

#define DHT11_GPIO GPIO_NUM_10
#define DHT11_TIMEOUT_US 1000

typedef struct {
    float temperature;
    float humidity;
} dht11_data_t;

bool dht11_read(dht11_data_t *data) {
    uint8_t raw_data[5] = {0};
    
    // 1. Send start signal
    gpio_set_direction(DHT11_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(DHT11_GPIO, 0);
    vTaskDelay(pdMS_TO_TICKS(20));
    gpio_set_level(DHT11_GPIO, 1);
    ets_delay_us(30);
    gpio_set_direction(DHT11_GPIO, GPIO_MODE_INPUT);
    
    // 2. Wait for DHT11 response
    if (!wait_for_level(DHT11_GPIO, 0, DHT11_TIMEOUT_US)) return false;
    if (!wait_for_level(DHT11_GPIO, 1, DHT11_TIMEOUT_US)) return false;
    if (!wait_for_level(DHT11_GPIO, 0, DHT11_TIMEOUT_US)) return false;
    
    // 3. Read 40 bits of data
    for (int i = 0; i < 40; i++) {
        if (!wait_for_level(DHT11_GPIO, 1, DHT11_TIMEOUT_US)) return false;
        
        uint32_t high_time = measure_pulse_width(DHT11_GPIO, 1);
        raw_data[i / 8] <<= 1;
        if (high_time > 40) raw_data[i / 8] |= 1;
        
        if (!wait_for_level(DHT11_GPIO, 0, DHT11_TIMEOUT_US)) return false;
    }
    
    // 4. Verify checksum
    uint8_t checksum = raw_data[0] + raw_data[1] + raw_data[2] + raw_data[3];
    if (checksum != raw_data[4]) return false;
    
    // 5. Parse data
    data->humidity = raw_data[0];
    data->temperature = raw_data[2];
    
    return true;
}
```

---

## 4. ESP32 Pico Mini Gateway Node - Detailed Design

### 4.1 SPI Slave Configuration

```c
void spi_slave_init(void) {
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = GPIO_NUM_13,
        .miso_io_num = GPIO_NUM_12,
        .sclk_io_num = GPIO_NUM_14,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };
    
    spi_slave_interface_config_t slave_cfg = {
        .mode = 0,
        .spics_io_num = GPIO_NUM_15,
        .queue_size = 3,
        .flags = 0,
    };
    
    ESP_ERROR_CHECK(spi_slave_initialize(SPI2_HOST, &bus_cfg, &slave_cfg, SPI_DMA_CH_AUTO));
}

void spi_slave_task(void *pvParameters) {
    spi_telemetry_packet_t rx_data;
    spi_slave_transaction_t trans;
    
    memset(&trans, 0, sizeof(trans));
    trans.length = sizeof(spi_telemetry_packet_t) * 8; // bits
    trans.rx_buffer = &rx_data;
    
    while (1) {
        ESP_ERROR_CHECK(spi_slave_transmit(SPI2_HOST, &trans, portMAX_DELAY));
        
        // Process received telemetry
        update_oled_display(&rx_data);
        publish_to_mqtt(&rx_data);
    }
}
```

---

### 4.2 OLED Display Driver (SSD1306)

```c
#include "ssd1306.h"

void oled_init(void) {
    i2c_master_init(I2C_NUM_0, GPIO_NUM_21, GPIO_NUM_22, 400000);
    ssd1306_init(I2C_NUM_0, 0x3C); // I2C address 0x3C
}

void update_oled_display(spi_telemetry_packet_t *data) {
    char line[32];
    
    ssd1306_clear_screen(I2C_NUM_0);
    
    snprintf(line, 32, "Temp: %.1f C", data->temperature);
    ssd1306_display_text(I2C_NUM_0, 0, line, 16, false);
    
    snprintf(line, 32, "Humidity: %.1f%%", data->humidity);
    ssd1306_display_text(I2C_NUM_0, 1, line, 16, false);
    
    snprintf(line, 32, "Distance: %d cm", data->distance_cm);
    ssd1306_display_text(I2C_NUM_0, 2, line, 16, false);
    
    snprintf(line, 32, "Status: %s", data->pir_status ? "MOTION" : "CLEAR");
    ssd1306_display_text(I2C_NUM_0, 3, line, 16, false);
}
```

---

### 4.3 MQTT Client

```c
#include "mqtt_client.h"

#define MQTT_BROKER_URI "mqtts://mqtt.thingspeak.com:8883"

esp_mqtt_client_handle_t mqtt_client;

void mqtt_init(void) {
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = MQTT_BROKER_URI,
        .credentials.username = "YOUR_MQTT_USERNAME",
        .credentials.authentication.password = "YOUR_MQTT_API_KEY",
    };
    
    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_start(mqtt_client);
}

void publish_to_mqtt(spi_telemetry_packet_t *data) {
    char payload[256];
    snprintf(payload, 256, 
             "field1=%.2f&field2=%.2f&field3=%d&field4=%d",
             data->temperature, data->humidity, data->distance_cm, data->gas_ppm);
    
    esp_mqtt_client_publish(mqtt_client, "channels/YOUR_CHANNEL_ID/publish", payload, 0, 1, 0);
}
```

---

## 5. ESP32-CAM Vision Node - Detailed Design

### 5.1 Camera Initialization

```c
#include "esp_camera.h"

#define CAM_PIN_PWDN    32
#define CAM_PIN_RESET   -1
#define CAM_PIN_XCLK    0
#define CAM_PIN_SIOD    26
#define CAM_PIN_SIOC    27

static camera_config_t camera_config = {
    .pin_pwdn  = CAM_PIN_PWDN,
    .pin_reset = CAM_PIN_RESET,
    .pin_xclk = CAM_PIN_XCLK,
    .pin_sscb_sda = CAM_PIN_SIOD,
    .pin_sscb_scl = CAM_PIN_SIOC,
    // ... (D0-D7 pins)
    .xclk_freq_hz = 20000000,
    .pixel_format = PIXFORMAT_JPEG,
    .frame_size = FRAMESIZE_VGA,
    .jpeg_quality = 12,
    .fb_count = 1
};

void camera_init(void) {
    esp_err_t err = esp_camera_init(&camera_config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Camera init failed with error 0x%x", err);
        return;
    }
}
```

---

### 5.2 Face Detection Task

```c
#include "fd_forward.h" // ESP-WHO library

void ai_inference_task(void *pvParameters) {
    camera_fb_t *fb = NULL;
    
    while (1) {
        fb = esp_camera_fb_get();
        if (!fb) {
            ESP_LOGE(TAG, "Camera capture failed");
            continue;
        }
        
        // Run face detection
        dl_matrix3du_t *image_matrix = dl_matrix3du_alloc(1, fb->width, fb->height, 3);
        fmt2rgb888(fb->buf, fb->len, fb->format, image_matrix->item);
        
        box_array_t *detected = face_detect(image_matrix, NULL);
        
        if (detected && detected->len > 0) {
            // Face detected!
            uart_send_command("FACE_DETECTED\n");
            
            // TODO: Face recognition
        }
        
        // Cleanup
        dl_matrix3du_free(image_matrix);
        esp_camera_fb_return(fb);
        
        vTaskDelay(pdMS_TO_TICKS(500)); // 2 FPS
    }
}
```

---

## 6. Memory Layout & Optimization

### 6.1 STM32 Flash Layout

| Address | Size | Content |
|---------|------|---------|
| 0x0800 0000 | 16 KB | Vector Table + Bootloader |
| 0x0800 4000 | 400 KB | Application Code |
| 0x0807 0000 | 64 KB | Configuration (EEPROM emulation) |
| 0x0808 0000 | 32 KB | Reserved (OTA partition) |

### 6.2 RAM Usage

**STM32F446RE (128 KB SRAM):**
- FreeRTOS Heap: 20 KB
- Task Stacks: 2 KB × 4 = 8 KB
- BSS (global variables): 10 KB
- DMA Buffers: 4 KB
- Free: ~86 KB

**ESP32 (520 KB SRAM):**
- WiFi Stack: 60 KB
- MQTT Client: 10 KB
- Camera Buffer: 100 KB
- Free: ~350 KB

---

## 7. Power Management

### 7.1 ESP32 Sleep Modes

```c
void enter_light_sleep(uint32_t sleep_time_ms) {
    esp_sleep_enable_timer_wakeup(sleep_time_ms * 1000); // Convert to μs
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_13, 1); // Wake on PIR
    esp_light_sleep_start();
}
```

**Power Consumption:**
- Active (WiFi ON): 160 mA
- Light Sleep: 0.8 mA
- Deep Sleep: 10 μA

---

## 8. Error Handling & Logging

### 8.1 STM32 Error Codes

```c
typedef enum {
    ERR_NONE = 0,
    ERR_CAN_TIMEOUT = 0x01,
    ERR_SPI_FAULT = 0x02,
    ERR_UART_OVERRUN = 0x03,
    ERR_SENSOR_FAULT = 0x04,
    ERR_WATCHDOG_RESET = 0x05,
    ERR_MEMORY_CORRUPT = 0x06
} error_code_t;

void log_error(error_code_t code, const char *msg) {
    // Write to EEPROM
    eeprom_write_error(code, HAL_GetTick(), msg);
    
    // Send to cloud
    char log_msg[128];
    snprintf(log_msg, 128, "ERROR_%02X:%s", code, msg);
    spi_send_to_gateway(log_msg);
}
```

---

## 9. Testing & Debugging

### 9.1 Unit Test Example (STM32)

```c
#include "unity.h"

void test_can_frame_parsing(void) {
    can_frame_t frame;
    frame.id = 0x101;
    frame.data[0] = 0x01; // SET command
    frame.data[1] = 0x02; // Relay 2
    frame.data[2] = 0x01; // ON
    
    relay_command_t cmd = parse_can_frame(&frame);
    
    TEST_ASSERT_EQUAL(CMD_SET, cmd.type);
    TEST_ASSERT_EQUAL(2, cmd.relay_id);
    TEST_ASSERT_TRUE(cmd.state);
}
```

---

## 10. Revision History

| Version | Date | Author | Changes |
|---------|------|--------|---------|
| 1.0 | Jan 2026 | Rahul Bari | Initial LLD document |

---
