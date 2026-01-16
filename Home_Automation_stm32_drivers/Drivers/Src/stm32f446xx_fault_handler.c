/*
 * fault_handler.c
 *
 *  Created on: Jan 16, 2026
 *  Author: Rahul B.
 *  description : to test and debug
 *  Error handling
 *  System understanding
 */

#include "stm32f446xx_fault_handler.h"
/* Provided by your UART driver */
extern void UART_Printf(const char *format, ...);

/*
 * Enable configurable fault exceptions
 */
void Fault_Init(void)
{
    SCB->SHCSR |= (1 << 18); // UsageFault
    SCB->SHCSR |= (1 << 17); // BusFault
    SCB->SHCSR |= (1 << 16); // MemFault
}

/*
 * Enable specific hardware traps (Divide by Zero, Unaligned Access)
 * These are often disabled by default on Cortex-M4
 */
void Fault_Enable_Traps(void)
{
    // SCB->CCR (Configuration and Control Register)
    // Bit 4: DIV_0_TRP (Trap on Divide by Zero)
    SCB->CCR |= (1 << 4);

    // Bit 3: UNALIGN_TRP (Trap on Unaligned Access)
    SCB->CCR |= (1 << 3);
}

void Fault_Handler_C(uint32_t *stacked_regs, const char *fault_name)
{
    // ... (Keep your existing print logic here) ...
    UART_Printf("\r\n================ [CRASH DETECTED] ================\r\n");
    UART_Printf("Fault Type : %s\r\n", fault_name);
    // ... rest of the printing code ...

    __asm("BKPT #0");
    while(1);
}


// --- Assembly Wrappers ---

__attribute__((naked)) void HardFault_Handler(void)
{
    __asm volatile (
        "TST lr, #4 \n"
        "ITE EQ \n"
        "MRSEQ r0, MSP \n"
        "MRSNE r0, PSP \n"
        "MOV r1, %0 \n"  // Pass string as 2nd argument
        "B Fault_Handler_C \n"
        : : "r" ("HardFault")
    );
}

__attribute__((naked)) void BusFault_Handler(void)
{
    __asm volatile (
        "TST lr, #4 \n"
        "ITE EQ \n"
        "MRSEQ r0, MSP \n"
        "MRSNE r0, PSP \n"
        "MOV r1, %0 \n"
        "B Fault_Handler_C \n"
        : : "r" ("BusFault")
    );
}

__attribute__((naked)) void UsageFault_Handler(void)
{
    __asm volatile (
        "TST lr, #4 \n"
        "ITE EQ \n"
        "MRSEQ r0, MSP \n"
        "MRSNE r0, PSP \n"
        "MOV r1, %0 \n"
        "B Fault_Handler_C \n"
        : : "r" ("UsageFault")
    );
}

__attribute__((naked)) void MemManage_Handler(void)
{
    __asm volatile (
        "TST lr, #4 \n"
        "ITE EQ \n"
        "MRSEQ r0, MSP \n"
        "MRSNE r0, PSP \n"
        "MOV r1, %0 \n"
        "B Fault_Handler_C \n"
        : : "r" ("MemManage")
    );
}
