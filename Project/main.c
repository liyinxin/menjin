#include "stm32f10x.h"
#include "zhiwen.h"
#include "string.h"
#include "stdio.h"
#include "led.h"
#include "watchdog.h"
#include "door.h"
#include "beep.h"


/*继电器是低电平打开*/
int FeedTime=0;
unsigned char Chongqi[3]={0X11,0x22,0x33};
unsigned char Start[5]={"start"};
unsigned char Clearn[5]={"clear"};
unsigned char Ceshi[12]={0xEF,0X01,0XFF,0XFF,0XFF,0XFF,0X01,0X00,0X03,0X03,0X00,0X07};
unsigned char PASS_WORD[5]={"check"};
unsigned char DELETE[6]={"delete"};
int mi(unsigned char dat, unsigned char num)
{
  unsigned char i;
  int sum = 1;

  for(i=0; i<num; i++)
  {
    sum = sum * dat;        
  }

  return sum;
}
void SysTick_Handler()//2s Feed Watchdog
{
	FeedTime++;
	if(FeedTime==1000)
	{
		GPIO_SetBits(LED_Port,LED_Pin);
	}
	if(FeedTime==2000)
	{
		IWDG_ReloadCounter();	
		FeedTime=0;
		GPIO_ResetBits(LED_Port,LED_Pin);
	}
}
int main(void)
{
	int i,tmp,result;
	char YES[3]="yes";
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	Usart_Init();
	SysTick_Config(72000);//1秒喂一次狗;
	Watch_Dog_Config();//狗的计数时间是3秒;
	Usart2_SendString(Chongqi,1);
	LED_GPIO_Configuration();
	Door_GPIO_Configuration();
	BEEP_GPIO_Configuration();
	while(1)
	{
		if(Computure_Flag==1)
		{
			Computure_Flag=0;
			if(StringCompare(Start,ComputureCommand,5)==1)
			{
				Enter_FingerPrint();//录入指纹
			}
			else if(StringCompare(Clearn,ComputureCommand,5)==1)
				ClearAll_Finger();
			else if(StringCompare(PASS_WORD,ComputureCommand,5)==1)
				Check_PassWord();
			else if(StringCompare(DELETE,ComputureCommand,6)==1)
			{ 
				
				for(i=6;i<Length2;i++)
				{
            tmp = ComputureCommand[i]&0x0f;         //如果原数组中存放的是ascii码，直接将其转换为数字
            result+=tmp*mi(10,Length2-i-1); //1*100+2*10+3*1
			  }
				Delete_One_Finger(result);
			}
			ClearAll(ComputureCommand,Length2);
		}
    Match_Finger();
		
		
	}
}

