#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// --- DEFINITIONS FOR NOTIFICATION BITS ---
// Bit 0 for Temperature, Bit 1 for Humidity
#define EVENT_TEMP_READY  (1 << 0) // 0x01
#define EVENT_HUM_READY   (1 << 1) // 0x02

// --- HANDLES ---
// We only strictly need the handle of the receiver so others can send to it
TaskHandle_t write_bus_handle = NULL;
TaskHandle_t temp_handle = NULL;
TaskHandle_t hum_handle = NULL;

BaseType_t status; 

// --- PROTOTYPES ---
static void temp_task(void *pvParameters); 
static void hum_task(void *pvParameters);
static void write_bus_task(void *pvParameters);

// ====================================================
// 1. SENDER TASK: TEMPERATURE
// ====================================================
static void temp_task(void *pvParameters){
    for(;;){
        // Simulate reading sensor...
        vTaskDelay(1000 / portTICK_PERIOD_MS); // Read every 1 seconds

        // Notify the Write Bus
        // eSetBits: This sets Bit 0 (0x01) inside the Write Bus task's notification value.
        // It does NOT overwrite Bit 1 if it is already set.
        xTaskNotify(write_bus_handle, EVENT_TEMP_READY, eSetBits);
    }
    vTaskDelete(NULL);
}

// ====================================================
// 2. SENDER TASK: HUMIDITY
// ====================================================
static void hum_task(void *pvParameters){
    for(;;){
        // Simulate reading sensor...
        vTaskDelay(2000 / portTICK_PERIOD_MS); // Read every 2 seconds

        // Notify the Write Bus
        // eSetBits: This sets Bit 1 (0x02) inside the Write Bus task's notification value.
        xTaskNotify(write_bus_handle, EVENT_HUM_READY, eSetBits);
    }
    vTaskDelete(NULL);
}

// ====================================================
// 3. RECEIVER TASK: WRITE BUS (The Central Logger)
// ====================================================
static void write_bus_task(void *pvParameters){
    uint32_t ulNotificationValue;

    for(;;){
        // Wait forever (portMAX_DELAY) until ANY notification comes in.
        // 0x00: Don't clear bits on entry.
        // 0xFFFFFFFF: Clear all bits on exit (reset value to 0 after we read it).
        // &ulNotificationValue: Store the bits that were set here.
        xTaskNotifyWait(0x00, 0xFFFFFFFF, &ulNotificationValue, portMAX_DELAY);

        // Check which bit was set
        if ((ulNotificationValue & EVENT_TEMP_READY) != 0) {
            printf("[Write Bus] Received: Temperature Data = 25.5 C\n");
        }

        if ((ulNotificationValue & EVENT_HUM_READY) != 0) {
            printf("[Write Bus] Received: Humidity Data = 60.0 %%\n");
        }
    }
    vTaskDelete(NULL);
}

// ====================================================
// APP MAIN
// ====================================================
void app_main(void)
{
    // 1. Create the Write Bus Task FIRST
    // It must exist so the handles are valid when other tasks try to notify it.
    status = xTaskCreate(write_bus_task, "write_bus", 2048, NULL, 2, &write_bus_handle);
    configASSERT(status == pdPASS);

    // 2. Create Temp Task
    status = xTaskCreate(temp_task, "temp_task", 2048, NULL, 2, &temp_handle);
    configASSERT(status == pdPASS);

    // 3. Create Humidity Task
    status = xTaskCreate(hum_task, "hum_task", 2048, NULL, 2, &hum_handle);
    configASSERT(status == pdPASS);
    
    printf("System Started. Waiting for notifications...\n");
}