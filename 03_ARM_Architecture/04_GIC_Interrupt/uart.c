#include <stdint.h>

#include "uart.h"

/*
====================================
			PRIVATE MACROS
====================================
*/

int uart_init(void)
{
	*(volatile uint32_t *)UART_ICR = 0x7FF;                          /* clear stale interrupts */
    *(volatile uint32_t *)UART_LCR_H = UART_LCR_FEN | UART_LCR_WLEN8;
    *(volatile uint32_t *)UART_CR = UART_CR_UARTEN | UART_CR_TXE | UART_CR_RXE;
    return 0;
}

int uart_rx(void)
{
	return 0;
}

void uart_tx(unsigned char ch)
{
	while (UARTFR & UARTFR_TXFF);
    UARTDR = ch;
}

void uart_tx_string(char *ch)
{
	int i = 0;
	while(ch[i]!= '\0')
	{
		uart_tx(ch[i]);
		i++;
	}
}

void uart_irq_enable(void)
{
    /*
     * Enable RX interrupt.
     */
    *(volatile uint32_t *)UART_IMSC |= (UART_INT_RX | UART_INT_RT);
}

void uart_irq_handler(void)
{
    uint32_t mis;
    char c;

    mis = *(volatile uint32_t *)UART_MIS;
    if (mis & UART_INT_RX)
    {
        while (!(*(volatile uint32_t *)UART_FR & UART_FR_RXFE))
        {
            char c = (char)(*(volatile uint32_t *)UART_DR & 0xFF);
            uart_tx_string("Received: ");
            uart_tx(c);
            uart_tx_string("\r\n");
        }

        *(volatile uint32_t *)UART_ICR = UART_INT_RX;
    }
}

