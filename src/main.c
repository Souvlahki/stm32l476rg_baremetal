#include <stdint.h>
#include "stm32l476xx.h"

#include "gpio.h"
#include "uart.h"
#include "exti.h"
#include "common.h"

#define LED_PIN         (5)
#define BAUDRATE        (9600)

static void exti_callback();
// static void usart2_send(char *, uint8_t);
static int strlen(char *);

volatile char *tx_buf;
volatile uint8_t tx_len;
volatile uint8_t tx_idx;
char msg[] = "button is pressed\n";

int main() {
    usart2_init(BAUDRATE);

    dma1_channel7_init();
    pc13_exti_init();

    RCC -> AHB2ENR |= GPIOAEN;
    gpio_set_pin_mode(GPIOA, LED_PIN, GPIO_MODE_OUTPUT);
    while(1) {
    }
}

static int strlen(char *string) {
    int i = 0;

    while(string[i]) {
        i++;
    }

    return i;
}

static void exti_callback() {
    usart2_dma_send((uint8_t *)msg,(uint16_t) strlen(msg));
    gpio_toggle_pin(GPIOA, LED_PIN);
}

// static void usart2_send(char *buf, uint8_t len) {
//     tx_buf = buf;
//     tx_len = len;
//     tx_idx = 0;

//     USART2 -> CR1 |= TXEIE;
// }



void EXTI15_10_IRQHandler() {
    if(EXTI -> PR1 & LINE13) {
       EXTI -> PR1 = LINE13; // clear  
       exti_callback();
    }
}


void DMA1_Channel7_IRQHandler() {
    if(DMA1 -> ISR & (1U << 25)) {
        DMA1-> IFCR = (1U << 25);


    }
}   