#ifndef __LED_H
#define __LED_H

#define LED_Pin    GPIO_Pin_13
#define LED_Port   GPIOC
#define LED_Clock  RCC_APB2Periph_GPIOC

void LED_GPIO_Configuration(void);

#endif
