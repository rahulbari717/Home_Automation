/*
 * main.c
 *
 * Created on: Jan 28, 2026
 * Author: Rahul B.
 * Description: Main Application Entry Point
 */

#include "state_machine.h"
#include "bsp_uart2_debug.h"

int main(void)
{
    // Initialize the state machine and all peripherals
    StateMachine_Init();
    // Start your requested peripheral test here
//    Run_Complete_Peripheral_Test();

    UART_Printf("\r\n[MAIN] Entering main loop...\r\n");

    // Main loop - Run state machine forever
    while (1) {
        StateMachine_Run();
    }

    return 0;
}

/**
 * @brief Helper function to clear LCD and print two lines
 */
void update_lcd_display(const char *line1, const char *line2) {
    BSP_LCD_SendCommand(LCD_CMD_CLEAR); //
    BSP_Delay_ms(2); // Wait for clear to finish
    BSP_LCD_SetCursor(0, 0);
    BSP_LCD_PrintString((char*)line1);
    BSP_LCD_SetCursor(1, 0);
    BSP_LCD_PrintString((char*)line2);
}
