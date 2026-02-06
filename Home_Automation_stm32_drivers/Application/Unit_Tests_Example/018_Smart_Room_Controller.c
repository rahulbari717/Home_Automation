/*
 * 018_Smart_Room_Controller.c
 *
 * Created on: Jan 19, 2026
 * Author: Rahul B.

 * Smart Room Controller - Main Application
 * Integrates: GPIO, UART, RTC, ADC, TIMER, KEYPAD, IWDG.

 * Application Logic:

 * System Start: Initializes all peripherals and sets a default time (RTC).
 * Security (Keypad): You must press 'A' on the keypad to "Arm" the system.
 * Automation (ADC & GPIO): Once armed, if the room gets Dark (LDR value), the Light (LED) turns ON automatically.
 * Reliability (IWDG & Fault): The Watchdog prevents hanging, and Fault handlers are enabled.
 * Logging (UART): Sends status updates to your serial terminal every second.
 */

#include "main.h"
#include "bsp_adc_sensors.h"
#include "keypad.h"

// --- Global Handles ---
USART_Handle_t usart2_handle;
RTC_Handle_t rtc_handle;

// --- Macros ---
#define LDR_THRESHOLD  3500

// --- Function Prototypes ---
void App_Init(void);  
void GPIO_LED_Init(void);
void USART2_Init_Setup(void);
void RTC_Setup(void);
void UART_Printf(const char *format, ...);

int main(void)
{
    // 1. Single Call to Initialize Everything
    App_Init();

    // 2. Application Variables
    uint8_t system_armed = 0; // 0 = OFF, 1 = ON
    uint16_t ldr_val = 0;
    char key_pressed;
    uint8_t h, m, s; // For RTC

    // 3. Main Loop
    while(1)
    {
        // --- A. Refresh Watchdog (Critical) ---
        IWDG_RELOAD();

        // --- B. Read Inputs ---
        key_pressed = Keypad_GetKey();
        ldr_val = BSP_Sensor_ReadLDR();

        // --- C. Logic: Keypad Control ---

        if(key_pressed != KEYPAD_NO_KEY)
        {
            UART_Printf(">> [INPUT] Key: %c\r\n", key_pressed);

            if(key_pressed == 'A') {
                system_armed = 1;
                UART_Printf(">> [MODE] System ARMED (Auto-Light ON)\r\n");
            }
            else if(key_pressed == 'B') {
                system_armed = 0;
                // Force Light OFF
                GPIO_WriteToOutputPin(GPIOA, GPIO_PIN_NO_5, GPIO_PIN_RESET);
                UART_Printf(">> [MODE] System DISARMED\r\n");
            }
        }

        // --- D. Logic: Automation (ADC + GPIO) ---
        if(system_armed)
        {
            if(ldr_val > LDR_THRESHOLD) {
                // It is Dark -> Turn Light ON
                GPIO_WriteToOutputPin(GPIOA, GPIO_PIN_NO_5, GPIO_PIN_SET);
            } else {
                // It is Bright -> Turn Light OFF
                GPIO_WriteToOutputPin(GPIOA, GPIO_PIN_NO_5, GPIO_PIN_RESET);
            }
        }

        // --- D. Status Logging ---
        TIMER_DelayMs(TIM2, 1000); // 1 Second Delay

        // Get Time
        RTC_GetTime(&rtc_handle, &h, &m, &s);

        UART_Printf("[%02d:%02d:%02d] Mode: %s | Light: %d | LED: %s\r\n",
                    h, m, s,
                    (system_armed ? "ARMED" : "OFF"),
                    ldr_val,
                    (GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_5) ? "ON" : "OFF")
                   );
    }
}

/* * ============================================================================
 * MASTER INIT FUNCTION
 * Comment/Uncomment lines here to Enable/Disable features
 * ============================================================================
 */
void App_Init(void)
{
    // 1. Clock & Timers (Critical - Do not comment out)
    // SystemClock_Config_HSE_180MHz(); // Enable if you have the PLL code
    TIMER_DelayInit();                  // Required for delays

    // 2. Debugging & Logging
    USART2_Init_Setup();     // Enable UART for printf style debugging

    // 3. User Interface
    GPIO_LED_Init();         // Enable Onboard LED (PA5)
    // Keypad_Init();        // Enable 4x4 Keypad (Commented out if not attached)

    // 4. Sensors
    BSP_Sensors_Init();      // Enable ADC for LDR/Temp sensors

    // 5. System Safety & Time
    Fault_Init();            // Enable HardFault handlers
    RTC_Setup();             // Enable Real Time Clock

    // 6. Watchdog (Safety Mechanism)
    // WARNING: If enabled, you MUST call IWDG_RELOAD() in main loop or system resets.
    IWDG_Init_2s();          // Initialize Watchdog with 2s timeout
    IWDG_START();            // Start counting down

    // Initial Log
    UART_Printf(">> [INIT] All Peripherals Initialized.\r\n");
}

// ============================================================================
// Driver Initialization & Helper Functions
// ============================================================================

void GPIO_LED_Init(void)
{
    // Initialize PA5 (Onboard LED)
    GPIO_Handle_t GpioLed;
    GpioLed.pGPIOx = GPIOA;
    GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_5;
    GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
    GpioLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    GpioLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

    GPIO_Init(&GpioLed);
}

void USART2_Init_Setup(void)
{
    // 1. Configure GPIO Pins for UART (PA2=TX, PA3=RX)
    GPIO_Handle_t usart_gpios;
    usart_gpios.pGPIOx = GPIOA;
    usart_gpios.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
    usart_gpios.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    usart_gpios.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;
    usart_gpios.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    usart_gpios.GPIO_PinConfig.GPIO_PinAltFunMode = 7;

    usart_gpios.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_2; // TX
    GPIO_Init(&usart_gpios);

    usart_gpios.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_3; // RX
    GPIO_Init(&usart_gpios);

    // 2. Configure UART Peripheral
    usart2_handle.pUSARTx = USART2;
    usart2_handle.USART_Config.USART_Baud = USART_STD_BAUD_115200;
    usart2_handle.USART_Config.USART_HWFlowControl = USART_HW_FLOW_CTRL_NONE;
    usart2_handle.USART_Config.USART_Mode = USART_MODE_TXRX;
    usart2_handle.USART_Config.USART_NoOfStopBits = USART_STOPBITS_1;
    usart2_handle.USART_Config.USART_WordLength = USART_WORDLEN_8BITS;
    usart2_handle.USART_Config.USART_ParityControl = USART_PARITY_DISABLE;

    USART_Init(&usart2_handle);
    USART_PeripheralControl(USART2, ENABLE);
}

void RTC_Setup(void)
{
    rtc_handle.pRTC = RTC;
    RTC_Init(&rtc_handle);

    // Set Default Start Time: 12:00:00
    // Using signature from your 007_rtc_uart.c: (handle, h, m, s)
    RTC_SetTime(&rtc_handle, 12, 00, 00);

    // Set Default Date: 1st Jan 2026, Thursday (4)
    RTC_SetDate(&rtc_handle, 26, 1, 1, 4);
}

