#include "stm32f10x.h"
#include "watchdog.h"
#include "stm32f10x_iwdg.h"



void Watch_Dog_Config(void)
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	
	IWDG_SetPrescaler(IWDG_Prescaler_32);//4S计时时间
	
	IWDG_SetReload(((3000*1250)/1000)-1);
	
	IWDG_ReloadCounter();
	
	IWDG_Enable();
}

