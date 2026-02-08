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



void dma1_channel7_init(void) {
    RCC->AHB1ENR |= DMA1EN;

    DMA1_Channel7->CCR &= ~DMA_CCR_EN;

    DMA1->IFCR = DMA_IFCR_CGIF7;

    /* USART2_TX request */
    DMA1_CSELR->CSELR &= ~(0xF << 24);
    DMA1_CSELR->CSELR |=  (2U << 24);

    /* Configure channel */
    DMA1_Channel7->CCR =
          DMA_CCR_MINC      // memory increment
        | DMA_CCR_DIR       // mem → periph
        | DMA_CCR_TCIE;     // transfer complete IRQ

    NVIC_EnableIRQ(DMA1_Channel7_IRQn);
}


static void usart2_dma_send(uint8_t *buf, uint16_t len) {
    if (len == 0) return;

    DMA1_Channel7->CCR &= ~DMA_CCR_EN;
    DMA1->IFCR = DMA_IFCR_CGIF7;

    DMA1_Channel7->CMAR  = (uint32_t)buf;
    DMA1_Channel7->CPAR  = (uint32_t)&USART2->TDR;
    DMA1_Channel7->CNDTR = len;

    USART2->CR3 |= USART_CR3_DMAT;
    DMA1_Channel7->CCR |= DMA_CCR_EN;
}