/*
 * 026_sensor_array.c
 *
 * Created on: Jan 24, 2026
 * Author: Rahul B.
 * Description: Test all motion/proximity sensors
 */

#include "main.h"
#include "bsp_sensors.h"
#include "bsp_uart2_debug.h"
#include "stm32f446xx_timer_driver.h"

int main(void)
{
    Debug_Init();
    BSP_Sensors_Motion_Init();
    TIMER_DelayInit();
    
    UART_Printf("\r\n===== Sensor Array Test =====\r\n");
    
    while(1)
    {
        uint8_t rcwl = BSP_Sensor_Read_RCWL();

        uint8_t ir1  = BSP_Sensor_Read_IR1();
        uint8_t ir2  = BSP_Sensor_Read_IR2();
        
        UART_Printf("RCWL:%d  IR1:%d IR2:%d \r\n",
                    rcwl, ir1, ir2);
        
        TIMER_DelayMs(TIM2, 500);
    }
}