#include <stdint.h>
#include "stm32l476xx.h"

#include "gpio.h"
#include "uart.h"
#include "exti.h"
#include "common.h"

#define LED_PIN         (5)
#define BAUDRATE        (9600)

static void exti_callback();
static void usart2_send(char *, uint8_t);
static int strlen(char *);

volatile char *tx_buf;
volatile uint8_t tx_len;
volatile uint8_t tx_idx;
char msg[] = "button is pressed\n";

int main() {
    pc13_exti_init();
    usart2_init(BAUDRATE);
    usart2_interrupt_enable();
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
    usart2_send(msg, strlen(msg));
    gpio_toggle_pin(GPIOA, LED_PIN);
}

static void usart2_send(char *buf, uint8_t len) {
    tx_buf = buf;
    tx_len = len;
    tx_idx = 0;

    USART2 -> CR1 |= TXEIE;
}



void EXTI15_10_IRQHandler() {
    if(EXTI -> PR1 & LINE13) {
       EXTI -> PR1 = LINE13; // clear  
       exti_callback();
    }
}

void USART2_IRQHandler() {
    if(USART2 -> ISR & SR_TXE) {
        if(tx_idx < tx_len) {
            USART2 -> TDR = tx_buf[tx_idx];
            tx_idx++;
        } else {
            USART2 -> CR1 &=~ TXEIE;
        }
    }
}