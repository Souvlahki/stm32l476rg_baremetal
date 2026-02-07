#include "stm32l476xx.h"

#include "uart.h"
#include "gpio.h"
#include "common.h"


#define USART2EN                (1U << 17)
#define TE                      (1U << 3)
#define UE                      (1U << 0)
#define RE                      (1U << 2)

#define TX_PIN                  (2)
#define RX_PIN                  (3)

#define TXEIE                   (1U << 7)
#define RXNEIE                  (1U << 5)

void usart2_init(int baud_rate) {
    // enable clock for gpioa
    RCC -> AHB2ENR |= GPIOAEN;

    // set the usart2 tx and rx pins
    gpio_set_pin_mode(GPIOA, TX_PIN, GPIO_MODE_AF);
    GPIOA -> AFR[0] |= (7U << 8);
    
    gpio_set_pin_mode(GPIOA, RX_PIN, GPIO_MODE_AF);
    GPIOA -> AFR[0] |= (7 << 12);

    // enable clock for uart
    RCC -> APB1ENR1 |= USART2EN;
    
    // configure brr
    // since the prescaler is 1 by default the apb1 frequency is the same as the system frequency
    USART2 -> BRR = SYS_FREQ / baud_rate;
    

    // enable uart, transmit, receive
    USART2 -> CR1 |= UE;
    USART2 -> CR1 |= TE;
    USART2 -> CR1 |= RE;

}

void usart2_write_char(char ch) {
    while(!(USART2 -> ISR & SR_TXE));

    USART2 -> TDR = ch;
}

void usart2_write_string(char *s) {
    while(*s) {
        usart2_write_char(*s++);
    }
}

char usart2_read() {
    while(!(USART2 -> ISR & SR_RXNE));

    return USART2 -> RDR & 0xFF;
}

void usart2_interrupt_enable() {
    USART2 -> CR1 |= RXNEIE;
    NVIC_EnableIRQ(USART2_IRQn);
}
