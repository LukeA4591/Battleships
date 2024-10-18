/** @file   send.c
    @author Luke Armstrong, Tyla Holmes
    @date   18 Oct 2024
    @brief  Module that controls functionality for sending a char through IR*/

#include "send.h"
#include "ir_uart.h"

/** 
Sends one char through IR
*/
void send (char chr) {
    ir_uart_putc(chr);  
}