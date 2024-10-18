#include "send.h"
#include "ir_uart.h"

void send (char chr) {
    ir_uart_putc(chr);  
}