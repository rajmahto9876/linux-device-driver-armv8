#ifndef __UART_H_
#define __UART_H_

#define UART_BASE       0x09000000UL

#define UART_DR         (UART_BASE + 0x00)
#define UART_FR         (UART_BASE + 0x18)
#define UART_IMSC       (UART_BASE + 0x38)
#define UART_RIS        (UART_BASE + 0x3C)
#define UART_MIS        (UART_BASE + 0x40)
#define UART_ICR        (UART_BASE + 0x44)

#define UART_FR_RXFE    (1 << 4)
#define UART_FR_TXFF    (1 << 5)

#define UARTDR  		(*(volatile unsigned int *)(UART_BASE + 0x00))
#define UARTFR  		(*(volatile unsigned int *)(UART_BASE + 0x18))

#define UARTFR_TXFF 	(1 << 5)

/* Receive interrupt */
#define UART_INT_RX     (1 << 4)
#define UART_INT_RT   	(1 << 6)   // receive timeout

#define UART_IRQ 		33  //Map with GIC

#define UART_LCR_H  (UART_BASE + 0x2C)
#define UART_CR     (UART_BASE + 0x30)
#define UART_LCR_FEN    (1 << 4)
#define UART_LCR_WLEN8  (3 << 5)
#define UART_CR_UARTEN  (1 << 0)
#define UART_CR_TXE     (1 << 8)
#define UART_CR_RXE     (1 << 9)


int uart_rx();
int uart_init(void);
void uart_tx(unsigned char ch);
void uart_tx_string(char *s);

void uart_irq_enable(void);
void uart_irq_handler(void);
#endif /* _UART_H_ */