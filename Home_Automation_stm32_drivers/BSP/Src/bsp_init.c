/*
 * bsp_init.c
 *
 * Created on: Jan 24, 2026
 * Author: Rahul B.
 * Description: Board Support Package - System Initialization
 */

#include "bsp_init.h"
#include "bsp_i2c_oled.h"
#include "bsp_lcd.h"
#include "bsp_uart2_debug.h"
#include "bsp_adc_sensors.h"
#include "bsp_keypad.h"

/* ===== Global Handle Declarations ===== */
USART_Handle_t bt_handle;
extern I2C_Handle_t g_OledI2cHandle;
USART_Handle_t usart2_handle;

/* ===== Helper Function Declarations ===== */
static void led_init(void);
static void buzzer_init(void);
static void LDR_init(void);
static void Relay_Init(void);
static void RCWL_Init(void);
void BSP_Sensors_Init(void);

/* ===== RCC Configuration (Simplified - Using your actual driver) ===== */
void RCC_Config_MaxSpeed(void) {
    // Using your existing RCC driver types
    RCC_OscInitTypeDef osc_config;
    RCC_ClkInitTypeDef clk_config;
    
    // Configure HSE + PLL for maximum speed
    osc_config.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    osc_config.HSEState = RCC_HSE_ON;
    osc_config.PLL.PLLState = RCC_PLL_ON;
    osc_config.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    osc_config.PLL.PLLM = 4;
    osc_config.PLL.PLLN = 180;
    osc_config.PLL.PLLP = 2;
    osc_config.PLL.PLLQ = 7;
    
    RCC_OscConfig(&osc_config);
    
    // Configure system clocks
    clk_config.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | 
                            RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    clk_config.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    clk_config.AHBCLKDivider = RCC_SYSCLK_DIV1;
    clk_config.APB1CLKDivider = RCC_HCLK_DIV4;
    clk_config.APB2CLKDivider = RCC_HCLK_DIV2;
    
    RCC_ClockConfig(&clk_config, RCC_FLASH_LATENCY_5WS);
}

/* ===== LED Initialization ===== */
static void led_init(void) {
    GPIO_Handle_t led_pin;
    
    led_pin.pGPIOx = LED_PORT;
    led_pin.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
    led_pin.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_LOW;
    led_pin.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    led_pin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
    
    // Green LED
    led_pin.GPIO_PinConfig.GPIO_PinNumber = LED_GREEN_PIN;
    GPIO_Init(&led_pin);
    
    // Red LED
    led_pin.GPIO_PinConfig.GPIO_PinNumber = LED_RED_PIN;
    GPIO_Init(&led_pin);
    
    // White LED
    led_pin.GPIO_PinConfig.GPIO_PinNumber = LED_WHITE_PIN;
    GPIO_Init(&led_pin);
}

/* ===== Buzzer Initialization ===== */
static void buzzer_init(void) {
    GPIO_Handle_t buzzer_pin;
    
    buzzer_pin.pGPIOx = BUZZER_PORT;
    buzzer_pin.GPIO_PinConfig.GPIO_PinNumber = BUZZER_PIN;
    buzzer_pin.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
    buzzer_pin.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_LOW;
    buzzer_pin.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    buzzer_pin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
    GPIO_Init(&buzzer_pin);
}

/* ===== LDR/ADC Initialization ===== */
static void LDR_init(void) {
    GPIO_Handle_t ldr_pins;
    ADC_Handle_t citadel_adc;

    GPIOA_PCLK_EN();
    ADC1_PCLK_EN();

    memset(&ldr_pins, 0, sizeof(ldr_pins));
    ldr_pins.pGPIOx = SENSOR_GPIO_PORT;
    ldr_pins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
    ldr_pins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

    ldr_pins.GPIO_PinConfig.GPIO_PinNumber = SENSOR_LDR1_PIN;
    GPIO_Init(&ldr_pins);

    ldr_pins.GPIO_PinConfig.GPIO_PinNumber = SENSOR_LDR2_PIN;
    GPIO_Init(&ldr_pins);

    memset(&citadel_adc, 0, sizeof(citadel_adc));
    citadel_adc.pADCx = SENSOR_ADC;
    citadel_adc.ADC_Config.ADC_Resolution = ADC_RES_12BIT;
    citadel_adc.ADC_Config.ADC_ContinuousMode = ADC_CONT_DISABLE;
    citadel_adc.ADC_Config.ADC_DataAlign = ADC_ALIGN_RIGHT;
    citadel_adc.ADC_Config.ADC_ScanMode = ADC_SCAN_DISABLE;
    citadel_adc.ADC_Config.ADC_Prescaler = ADC_PRESCALER_DIV4;
    
    ADC_Init(&citadel_adc);
}

/* ===== Button Initialization ===== */
void button_init(void) {
    GPIO_Handle_t GpioButton;

    memset(&GpioButton, 0, sizeof(GpioButton));
    GpioButton.pGPIOx = WAKEUP_BTN_PORT;
    GpioButton.GPIO_PinConfig.GPIO_PinNumber = WAKEUP_BTN_PIN;
    GpioButton.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IT_RFT;
    GpioButton.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    GpioButton.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

    GPIO_Init(&GpioButton);

    GPIO_IRQPriorityConfig(EXTI15_10_IRQn, NVIC_IRQ_PRI15);
    GPIO_IRQInterruptConfig(EXTI15_10_IRQn, ENABLE);
}

/* ===== USART2 GPIO Initialization ===== */
void USART2_GPIOInit(void) {
    GPIO_Handle_t usart_gpios;

    usart_gpios.pGPIOx = USART_VCP_PORT;
    usart_gpios.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
    usart_gpios.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    usart_gpios.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;
    usart_gpios.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    usart_gpios.GPIO_PinConfig.GPIO_PinAltFunMode = USART_VCP_AF;

    usart_gpios.GPIO_PinConfig.GPIO_PinNumber = USART_TX_PIN;
    GPIO_Init(&usart_gpios);

    usart_gpios.GPIO_PinConfig.GPIO_PinNumber = USART_RX_PIN;
    GPIO_Init(&usart_gpios);
}

/* ===== USART2 Peripheral Initialization ===== */
void USART2_Init(void) {
    usart2_handle.pUSARTx = USART_VCP;
    usart2_handle.USART_Config.USART_Baud = USART_STD_BAUD_115200;
    usart2_handle.USART_Config.USART_HWFlowControl = USART_HW_FLOW_CTRL_NONE;
    usart2_handle.USART_Config.USART_Mode = USART_MODE_TXRX;
    usart2_handle.USART_Config.USART_NoOfStopBits = USART_STOPBITS_1;
    usart2_handle.USART_Config.USART_WordLength = USART_WORDLEN_8BITS;
    usart2_handle.USART_Config.USART_ParityControl = USART_PARITY_DISABLE;

    USART_Init(&usart2_handle);
    USART_PeripheralControl(USART2, ENABLE);
}

/* ===== Bluetooth Initialization ===== */
void Bluetooth_Init(void) {
    GPIO_Handle_t bt_pins;
    
    GPIOA_PCLK_EN();
    USART1_PCLK_EN();

    memset(&bt_pins, 0, sizeof(bt_pins));
    bt_pins.pGPIOx = BT_USART_PORT;
    bt_pins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
    bt_pins.GPIO_PinConfig.GPIO_PinAltFunMode = BT_USART_AF;
    bt_pins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    bt_pins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;
    bt_pins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

    bt_pins.GPIO_PinConfig.GPIO_PinNumber = BT_TX_PIN;
    GPIO_Init(&bt_pins);
    
    bt_pins.GPIO_PinConfig.GPIO_PinNumber = BT_RX_PIN;
    GPIO_Init(&bt_pins);

    bt_handle.pUSARTx = BT_USART;
    bt_handle.USART_Config.USART_Baud = BT_BAUD_RATE;
    bt_handle.USART_Config.USART_Mode = USART_MODE_TXRX;
    bt_handle.USART_Config.USART_NoOfStopBits = USART_STOPBITS_1;
    bt_handle.USART_Config.USART_WordLength = USART_WORDLEN_8BITS;
    bt_handle.USART_Config.USART_ParityControl = USART_PARITY_DISABLE;
    
    USART_Init(&bt_handle);
    USART_PeripheralControl(BT_USART, ENABLE);
}

/* ===== Keypad Initialization ===== */
void Keypad_Init(void) {
    GPIO_Handle_t keypad_gpio;

    memset(&keypad_gpio, 0, sizeof(keypad_gpio));
    GPIOB_PCLK_EN();

    // Configure Rows as Output
    keypad_gpio.pGPIOx = KEYPAD_ROW_PORT;
    keypad_gpio.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
    keypad_gpio.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    keypad_gpio.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
    keypad_gpio.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

    uint8_t rows[] = {KEYPAD_R0_PIN, KEYPAD_R1_PIN, KEYPAD_R2_PIN, KEYPAD_R3_PIN};
    for(int i = 0; i < 4; i++) {
        keypad_gpio.GPIO_PinConfig.GPIO_PinNumber = rows[i];
        GPIO_Init(&keypad_gpio);
        GPIO_WriteToOutputPin(KEYPAD_ROW_PORT, rows[i], GPIO_PIN_SET); 
    }

    // Configure Columns as Input
    keypad_gpio.pGPIOx = KEYPAD_COL_PORT;
    keypad_gpio.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
    keypad_gpio.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;

    uint8_t cols[] = {KEYPAD_C0_PIN, KEYPAD_C1_PIN, KEYPAD_C2_PIN, KEYPAD_C3_PIN};
    for(int i = 0; i < 4; i++) {
        keypad_gpio.GPIO_PinConfig.GPIO_PinNumber = cols[i];
        GPIO_Init(&keypad_gpio);
    }
}

/* ===== Relay Initialization ===== */
static void Relay_Init(void) {
    GPIO_Handle_t relay_gpio;

    relay_gpio.pGPIOx = RELAY_PORT;
    relay_gpio.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
    relay_gpio.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    relay_gpio.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    relay_gpio.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

    const uint8_t relay_pins[4] = {RELAY1_PIN, RELAY2_PIN, RELAY3_PIN, RELAY4_PIN};
    for(int i = 0; i < 4; i++) {
        relay_gpio.GPIO_PinConfig.GPIO_PinNumber = relay_pins[i];
        GPIO_Init(&relay_gpio);
        GPIO_WriteToOutputPin(RELAY_PORT, relay_pins[i], GPIO_PIN_RESET);
    }
}

/* ===== RCWL Motion Sensor Initialization ===== */
static void RCWL_Init(void) {
    GPIO_Handle_t rcwl_pin;
    
    GPIOB_PCLK_EN();

    rcwl_pin.pGPIOx = RCWL_PORT;
    rcwl_pin.GPIO_PinConfig.GPIO_PinNumber = RCWL_PIN;
    rcwl_pin.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
    rcwl_pin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
    
    GPIO_Init(&rcwl_pin);
}

/* ===== Sensors Init (Called from main) ===== */
void BSP_Sensors_Init(void) {
    LDR_init();
    RCWL_Init();
}

/* ===== Master Application Initialization ===== */
void app_init(void) {
    // Comment out if you want to use default HSI clock instead of max speed
    // RCC_Config_MaxSpeed();

    // Basic peripherals
    led_init();
    button_init();
    buzzer_init();
    
    // Communication
    USART2_GPIOInit();
    USART2_Init();
    
    // Optional: Uncomment if using Bluetooth
    // Bluetooth_Init();
    
    // Input devices
    BSP_Sensors_Init();
    Keypad_Init();
    
    // Output devices
    Relay_Init();
}
