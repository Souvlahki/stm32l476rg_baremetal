#include "exti.h"
#include "common.h"
#include "gpio.h"

#define BUTTON_PIN          (13)

void pc13_exti_init() {
    __disable_irq();
    RCC -> AHB2ENR |= GPIOCEN;
    
    gpio_set_pin_mode(GPIOC, BUTTON_PIN, GPIO_MODE_INPUT);

    RCC -> APB2ENR |= SYSCFGEN;

    SYSCFG -> EXTICR[3] |= (2U << 4);
    EXTI -> IMR1 |= (1U << 13);
    EXTI -> FTSR1 |= (1U << 13);

    NVIC_EnableIRQ(EXTI15_10_IRQn);
    __enable_irq();
}