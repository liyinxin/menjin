#ifndef __BEEP_H
#define __BEEP_H

#define BEEP_Pin   GPIO_Pin_1
#define BEEP_Port  GPIOA
#define BEEP_Clock RCC_APB2Periph_GPIOA

void BEEP_GPIO_Configuration(void);
#endif
