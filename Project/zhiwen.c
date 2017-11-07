#include "zhiwen.h"
#include "stm32f10x_dma.h"
#include "stdio.h"
#include "door.h"
#include "beep.h"
#include "stdlib.h"
static uint8_t FingerPageID;
int Test ;
unsigned char Clear_ku[12]={0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x0D,0x00,0x11};//清除所有的指纹
unsigned char Search_ku[17]={0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x08,0x04,0x01,0x00,0x00,0x00,0x80,0x00,0x8E};
unsigned char GR_Get_Img[12] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x01,0x00,0x05};    //获取指纹图像
unsigned char GR_Templete_Num[12] ={0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x1D,0x00,0x21 }; //获取模板总数
unsigned char GR_Search[17]={0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x08,0x04,0x01,0x00,0x00,0x03,0xA1,0x00,0xB2}; //搜索指纹 搜索范围 0-929
unsigned char GR_Search_0_9[17]={0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x08,0x04,0x01,0x00,0x00,0x00,0x09,0x00,0x17}; //搜索0-9号指纹
unsigned char GR_Img_To_Buffer1[13]={0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x04,0x02,0x01,0x00,0x08}; //将图像放入BUFF1
unsigned char GR_Img_To_Buffer2[13]={0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x04,0x02,0x02,0x00,0x09}; //将图像放入BUFF2
unsigned char GR_Reg_Model[12]={0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x05,0x00,0x09}; //将buff1跟buff2合成特征模板
unsigned char GR_Delet_All_Model[12]={0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x0D,0x00,0x11};//删除指纹模板里的所有指纹
unsigned char GR_Save_Finger[15]={0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x06,0x06,0x01,0X00,0X00,0x00,0x0E};//将buff2中的特征指纹放到指定位置
unsigned char GR_Delete_Model[16]={0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x07,0x0C,0x00,0x00,0x00,0x01,0x00,0x15}; //删除指定的模板
unsigned char GR_Chek[16]={0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x07,0x13,0x00,0x00,0x00,0x00,0x00,0x1B};
unsigned char Receive[100];
unsigned char Stop_Flag=0;
unsigned char XiaBiao=0,Length1=0,Length2=0;
unsigned char Finger[12]={0XEF,0X01,0XFF,0XFF,0XFF,0XFF,0X07,0X00,0X03,0X00,0X00,0X0A};
unsigned char Gen_Picture[12]={0XEF,0X01,0XFF,0XFF,0XFF,0XFF,0X07,0X00,0X03,0X00,0X00,0X0A};
unsigned char Error[1]={0XAA};
unsigned char Right[1]={0XBB};
unsigned char Receive_Ok=0;
unsigned char R[1]={0XCC},E[1]={0XDD};
unsigned char Reg_Model[12]={0XEF,0X01,0XFF,0XFF,0XFF,0XFF,0X07,0X00,0X03,0X00,0X00,0X0A};
unsigned char Store_Finger[12]={0XEF,0X01,0XFF,0XFF,0XFF,0XFF,0X07,0X00,0X03,0X00,0X00,0X0A};
unsigned char ComputureCommand[100]={0},n=0;
unsigned char Computure_Flag=0,Match_Flag=0;
unsigned char Pass[1]={0XEE},NotPass[1]={0XCC},MATCH[10]={0XEF,0X01,0XFF,0XFF,0XFF,0XFF,0X07,0X00,0X07,0X00};
unsigned char TuiChu=0;
unsigned char Hex[100]={0};
unsigned char GG[1] = {0x11};


/************指纹匹配成功:发送0XEE；
             */
void GR_Save_Finger_Weizhi(u16 PageID)//这个是设置你要保存的指纹地址的一个计算公式
{
       unsigned long temp = 0;
		   unsigned char i;

       GR_Save_Finger[11] = PageID/0XFF;
       GR_Save_Finger[12] = PageID%0XFF;
           
		   for(i=6;i<13;i++)   
		   	   temp = temp + GR_Save_Finger[i]; 
		   GR_Save_Finger[13]=(temp & 0x00FF00) ;//>> 8; 
		   GR_Save_Finger[14]= temp & 0x0000FF;
}
void GR_Delet_Finger_Weizhi(u16 PageID)
{
	unsigned long temp = 0;
	unsigned char i;

  GR_Delete_Model[10] = PageID/256;
  GR_Delete_Model[11] = PageID%256;
          
	for(i=6;i<14;i++)   
	    temp = temp + GR_Delete_Model[i]; 
  GR_Delete_Model[14]=(temp & 0x00FF00) >> 8; 
  GR_Delete_Model[15]= temp & 0x0000FF;
}
void Usart_Init(void)
{
	USART_InitTypeDef USART_InitStruction;
	GPIO_InitTypeDef GPIO_InitStruction;
	NVIC_InitTypeDef NVIC_InitStruction;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	
	NVIC_InitStruction.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStruction.NVIC_IRQChannelPreemptionPriority= 0;//抢占优先级3
	NVIC_InitStruction.NVIC_IRQChannelSubPriority = 0;		//子优先级3
	NVIC_InitStruction.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStruction);	//根据指定的参数初始化VIC寄存器
	
//	NVIC_InitStruction.NVIC_IRQChannel = USART1_IRQn;
//	NVIC_InitStruction.NVIC_IRQChannelPreemptionPriority= 0;//抢占优先级3
//	NVIC_InitStruction.NVIC_IRQChannelSubPriority = 1;		//子优先级3
//	NVIC_InitStruction.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
//	NVIC_Init(&NVIC_InitStruction);	//根据指定的参数初始化VIC寄存器
//	
	
	//DMA优先级        
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  NVIC_InitStruction.NVIC_IRQChannel= DMA1_Channel4_IRQn; 
  NVIC_InitStruction.NVIC_IRQChannelPreemptionPriority= 0; 
  NVIC_InitStruction.NVIC_IRQChannelSubPriority= 2; 
  NVIC_InitStruction.NVIC_IRQChannelCmd= ENABLE; 
  NVIC_Init(&NVIC_InitStruction);
	
	
	NVIC_InitStruction.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStruction.NVIC_IRQChannelPreemptionPriority= 0;//抢占优先级3
	NVIC_InitStruction.NVIC_IRQChannelSubPriority = 2;		//子优先级3
	NVIC_InitStruction.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStruction);	//根据指定的参数初始化VIC寄存器
	
	GPIO_InitStruction.GPIO_Pin = GPIO_Pin_10;//TX
	GPIO_InitStruction.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruction.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruction);
	
	GPIO_InitStruction.GPIO_Pin = GPIO_Pin_11;//RX
	GPIO_InitStruction.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruction.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruction);
	
	
//	GPIO_InitStruction.GPIO_Pin = GPIO_Pin_9;//TX
//	GPIO_InitStruction.GPIO_Mode = GPIO_Mode_AF_PP;
//	GPIO_InitStruction.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOA,&GPIO_InitStruction);
//	
//	GPIO_InitStruction.GPIO_Pin = GPIO_Pin_10;//RX
//	GPIO_InitStruction.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//	GPIO_InitStruction.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOA,&GPIO_InitStruction);
//	
	GPIO_InitStruction.GPIO_Pin = GPIO_Pin_2;//TX
	GPIO_InitStruction.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruction.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruction);
	
	GPIO_InitStruction.GPIO_Pin = GPIO_Pin_3;//RX
	GPIO_InitStruction.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruction.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruction);
	
	
	USART_InitStruction.USART_BaudRate = 57600;
	USART_InitStruction.USART_WordLength = USART_WordLength_8b;
	USART_InitStruction.USART_StopBits = USART_StopBits_1;
	USART_InitStruction.USART_Parity = USART_Parity_No;
	USART_InitStruction.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStruction.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	//USART_Init(USART1,&USART_InitStruction);
	USART_Init(USART2,&USART_InitStruction);
	USART_Init(USART3,&USART_InitStruction);
	
	//USART_Cmd(USART1,ENABLE);
	USART_Cmd(USART2,ENABLE);
	USART_Cmd(USART3,ENABLE);
	
	//USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	//USART_ITConfig(USART1,USART_IT_IDLE,ENABLE);
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
	USART_ITConfig(USART2,USART_IT_IDLE,ENABLE);
	USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);
	USART_ITConfig(USART3,USART_IT_IDLE,ENABLE);
}
void Delay(u32 Ucount)
{
	while(Ucount--);
}
void DMA_InitConfig(void)
{

}
void Usart_SendString(uint8_t p[],unsigned int n)//串口一发送一个字符数据
{
	int i;
	for(i=0;i<n;i++)
	{
		USART_SendData(USART3,p[i]);
		while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
	}
}
void Usart2_SendString(uint8_t p[],unsigned int n)//串口2发送一个字符数组
{
	int i;
	for(i=0;i<n;i++)
	{
		USART_SendData(USART2,p[i]);
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
	}
}
void USART3_IRQHandler(void)//这个是指纹模块的服务中断
{
	int clear;
	
	if(USART_GetITStatus(USART3,USART_IT_RXNE) != RESET)
	{
		Receive[XiaBiao] = USART_ReceiveData(USART3);//接收指纹模块传输的消息
		XiaBiao++;//作为你接收消息的长度的一个判断
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);
	}
	if(USART_GetFlagStatus(USART3,USART_FLAG_IDLE) != RESET)//这个函数可以运行的
	{
		clear = USART3->SR;
		clear = USART3->DR;
		Length1 = XiaBiao;
		XiaBiao=0;//一定要清零;
		Stop_Flag=1;
		
		//USART_ClearITPendingBit(USART1,USART_FLAG_IDLE);
	}
			
}

uint8_t StringCompare(unsigned char p1[],unsigned char p2[],unsigned char length)//比较两个数组大小是否是相同的数据
{
	int i=0;
	while(1)
	{
		if(p1[i] != p2[i])//不去用'\0'判断结束不结束
		{
			return 0;
	  }
		i++;
		if(i==length)
			return 1;
	}		
}
void Change_PageID(void)
{
	uint8_t number=sizeof(ComputureCommand);
	Usart2_SendString(ComputureCommand,Length2);
	
	FingerPageID=atoi(ComputureCommand);
	Usart2_SendString(&FingerPageID,1);
}
void Enter_FingerPrint(void)//这个是录入指纹的接口,只要开启此接口,就必须得录入成功,否则会一直提示你录取指纹;
{
	int Ok=0;//就是录入buffer的个数;
	Usart2_SendString("ABCDEF",6);
	while(Computure_Flag == 0);
	if(Computure_Flag == 1)
	{
		Computure_Flag=0;
		Change_PageID();
		while(1)
		{
			Usart_SendString(GR_Get_Img,12);//发送给指纹模块
			Delay(0X41FFF0);	//这个延时是给串口接收数据的一些时间(个人觉得还是加一下比较好)
			//Usart2_SendString(Receive,Length1);//返回电脑端1
			/*判断是否有手指按下的处理***/
			if(StringCompare(Receive,Finger,Length1)==1)//采集指纹成功
			{
					//Usart2_SendString(Right,1);//0XBB;采集指纹成功的话,发送一个成功的标志BB
					Receive_Ok = 1;//采集数据指纹成功的标志							
			}
			else
			{
					Usart2_SendString(Error,1);//0XAA;
					Ok=0;  //这个的意思是重新从头开始采集数据;
			}
		/*这个是按下手指以后的处理****/
			if(Receive_Ok ==1)//如果数据采集成
			{
				Receive_Ok = 0;//有手指按下的标志
				if(Ok==0)//(Ok是第几个BUffer里的标志)指纹存在Buffer1中
					Usart_SendString(GR_Img_To_Buffer1,13);//发送生成特征指令的命令;
				else if(Ok==1)
					Usart_SendString(GR_Img_To_Buffer2,13);//指纹存在Buffer2中	
				Delay(0X40FFF0);	//加延时等待指纹模块的响应(这一个一定要等待,否则响应还没完结,你就发数据,会使数据丢失的;
				//Usart2_SendString(Receive,Length1);//将生成特征值这个返回的数据传给电脑2
				if(StringCompare(Receive,Gen_Picture,Length1)==1)//生成图像特征
				{
					Usart2_SendString(Right,1);//0XBB;成功的话给电脑一个标志位
					Ok++;//记录成功的次数
				}
				else
				{
					Usart2_SendString(GG,1);//0XAA;没成功的时候，保持原样Ok;
					Ok=0;//只要是中途失败了,就去重新开始操作,直到录入成功为止
				}
			}
		/***这个是两个特征分别存到Buffe1和Buffer2中以后的操作***/
			if(Ok==2)
			{
				Usart_SendString(GR_Reg_Model,12);//生成数据模板
				Delay(0X40FFF0);	//加延时
				//Usart2_SendString(Receive,Length1);//3
				if(StringCompare(Reg_Model,Receive,Length1)==1)
				{
						GR_Save_Finger_Weizhi(FingerPageID);
						Usart_SendString(GR_Save_Finger,15);
						Delay(0X40FFFF);//加延时
				    Usart2_SendString(Receive,Length1);//4
						if(StringCompare(Receive,Store_Finger,Length1)==1)
						{
							Usart2_SendString(R,1);
							TuiChu=1;
							//FingerPageID++;
							Usart2_SendString(&FingerPageID,1);
						}
						else
						{
							Ok=0;
						}
				}
				else
				{
						Usart2_SendString(E,1);//发送一个录入这个指纹错误的标志
						Ok=0;//重新开始采集数据
				}
			}
			if(TuiChu==1)
			{
				TuiChu=0;
				break;
			}
		}
	}
}
void USART2_IRQHandler(void)//这个是客户端发送指令的服务中断
{
	int clear;
	if(USART_GetITStatus(USART2,USART_IT_RXNE) != RESET)
	{
		ComputureCommand[n] = USART_ReceiveData(USART2);
		n++;
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);
	}
	if(USART_GetFlagStatus(USART2,USART_FLAG_IDLE) != RESET)
	{
		clear = USART2->SR;
		clear = USART2->DR;
		Length2 = n;
		n=0;//一定要清零;
		Computure_Flag = 1;
		//USART_ClearITPendingBit(USART2,USART_FLAG_IDLE);
	}
			
}
void ClearAll(unsigned char P[],unsigned length)//将你的缓冲数组全部清零的
{
	int i;
	for(i=0;i<length;i++)
	{
		P[i]='0';
	}
}
void Match_Finger(void)//匹配你的指纹的
{
	  Usart_SendString(GR_Get_Img,12);//发送给指纹模块
	  Delay(0X40FFFF);
		/*判断是否有手指按下的处理***/
	  if(StringCompare(Receive,Finger,Length1)==1)//采集指纹成功
		{
				  Receive_Ok = 1;//采集数据指纹成功的标志						
		}
//		else 
//			Usart2_SendString(Receive,Length1);//这个是用来判断失败的原因的指令;
//		//把指纹模块发送的数据，发送给我，我去分析下;
		if(Receive_Ok == 1)//如果数据采集成
		{
			Receive_Ok = 0;//有手指按下的标志
			Usart_SendString(GR_Img_To_Buffer1,13);//发送生成特征指令的命令;
      Delay(0X40FFFF);			
			if(StringCompare(Receive,Gen_Picture,Length1)==1)//生成图像特征
			{
				  Match_Flag=1;
			}
			if(Match_Flag==1)
			{
				Match_Flag=0;
				Usart_SendString(GR_Search,17);//这个传输是没有问题的
				Delay(0X40FFFF);
				 //Usart2_SendString(Receive,Length1);//接收到的应答码是16位的
				 if(StringCompare(Receive,MATCH,10)==1)
				 {
					   //Usart2_SendString(Pass,1);//匹配成功的时候发送数据0XEE;
					 GPIO_ResetBits(GPIOA,GPIO_Pin_4);
					 Delay(0xFFFF);
					 GPIO_SetBits(GPIOA,GPIO_Pin_4);
				 }
				 else
				 {
					 Usart2_SendString(NotPass,1);//匹配不成功成功的时候发送数据0XCC;
					 GPIO_SetBits(BEEP_Port,BEEP_Pin);
					 Delay(0XFFF);
					 GPIO_ResetBits(BEEP_Port,BEEP_Pin);
				 }
		  }
		}
}

void ClearAll_Finger(void)//清除所有指纹特征值的
{
	Usart_SendString(GR_Delet_All_Model,12);
	Delay(0XFFFF);
	Usart2_SendString(Receive,Length1);
}
void Check_PassWord(void)//检验握手命令的
{
	Usart_SendString(GR_Chek,16);
	Delay(0XFFFF);
	Usart2_SendString(Receive,Length1);
}

void Delete_One_Finger(u16 PageID)//一次只删除一个指纹,这个指纹的位置由客服端发送
{
	GR_Delet_Finger_Weizhi(PageID);
  Usart_SendString(GR_Delete_Model,16);
	Delay(0X40FFFF);
	Usart2_SendString(Receive,Length1);
}


