#include "beep.h"
#include "stm32f10x.h"

void BEEP_GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(BEEP_Clock,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = BEEP_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(BEEP_Port,&GPIO_InitStructure);
	
	GPIO_ResetBits(BEEP_Port,BEEP_Pin);
}
