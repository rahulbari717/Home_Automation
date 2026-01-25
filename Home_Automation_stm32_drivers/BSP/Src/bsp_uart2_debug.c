/*
 * bsp_uart2_debug.c
 *
 *  Created on: Jan 16, 2026
 *      Author: lenovo
 */


#include "bsp_uart2_debug.h"
#include "bsp_init.h"

void UART_Printf(const char *format, ...) {
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    USART_SendData(&usart2_handle, (uint8_t*)buffer, strlen(buffer));
    va_end(args);
}

uint8_t UART_ReceiveByte(void)
{
    uint8_t data;
    USART_ReceiveData(&usart2_handle, &data, 1);
    return data;
}

void Debug_ClearScreen(void)
{
    // VT100 Escape codes to clear screen and move cursor home
    UART_Printf("\033[2J\033[H");
}

/* ================= FAULT TRIGGERS ================= */

// 1. Divide by Zero
// Calculates 10 / 0. Requires DIV_0_TRP bit enabled in SCB->CCR.
void Trigger_DivideByZero(void)
{
    volatile int a = 10;
    volatile int b = 0;
    volatile int c;
    c = a / b; // CRASH: UsageFault (DIVBYZERO)
    (void)c;   // Suppress unused warning
}

// 2. Unaligned Access
// Tries to read a 32-bit word from an address not divisible by 4.
void Trigger_UnalignedAccess(void)
{
    volatile uint32_t *p;
    uint8_t temp_buffer[8]; // Array on stack
    // Pointer math creates an unaligned address (e.g. 0x...1)
    p = (uint32_t*)((uint32_t)temp_buffer + 1);
    volatile uint32_t val = *p; // CRASH: UsageFault (UNALIGNED)
    (void)val;
}

// 3. Invalid Instruction
// Jumps to an address containing invalid executable code.
void Trigger_InvalidInstruction(void)
{
    // Function pointer to valid SRAM address (0x20000001 sets Thumb bit)
    // content at that address is 0x0000 (invalid instruction)
    void (*bad_func)(void) = (void (*)(void))0x20000001;
    bad_func(); // CRASH: UsageFault (UNDEFINSTR)
}

// 4. Bus Fault
// Accessing a reserved memory address.
void Trigger_BusFault(void)
{
    volatile uint32_t *p = (uint32_t*)0xFFFFFFFF;
    volatile uint32_t val = *p; // CRASH: BusFault
    (void)val;
}

/* NEW IMPLEMENTATION */
USART_Handle_t* Debug_GetHandle(void)
{
    return &usart2_handle;
}
