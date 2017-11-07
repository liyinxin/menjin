#ifndef __DOOR_H
#define __DOOR_H

#define DOOR_Pin   GPIO_Pin_4
#define DOOR_Port  GPIOA
#define DOOR_Clock RCC_APB2Periph_GPIOA


void Door_GPIO_Configuration(void);
#endif
