#include "door.h"
#include "stm32f10x.h"

void Door_GPIO_Configuration(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(DOOR_Clock,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = DOOR_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DOOR_Port,&GPIO_InitStructure);
	GPIO_SetBits(DOOR_Port,DOOR_Pin);
}
