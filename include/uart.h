#ifndef UART_H_
#define UART_H_

#define SR_TXE                  (1U << 7)
#define SR_RXNE                 (1U << 5)

void usart2_init(int baud_rate);
void usart2_write_char(char ch);
void usart2_write_string(char *s);
char usart2_read();
void usart2_interrupt_enable();
void dma1_channel7_init();
void usart2_dma_send(uint8_t *buf, uint16_t len);
#endif