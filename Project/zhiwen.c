#include "zhiwen.h"
#include "stm32f10x_dma.h"
#include "stdio.h"
#include "door.h"
#include "beep.h"
#include "stdlib.h"
static uint8_t FingerPageID;
int Test ;
unsigned char Clear_ku[12]={0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x0D,0x00,0x11};//������е�ָ��
unsigned char Search_ku[17]={0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x08,0x04,0x01,0x00,0x00,0x00,0x80,0x00,0x8E};
unsigned char GR_Get_Img[12] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x01,0x00,0x05};    //��ȡָ��ͼ��
unsigned char GR_Templete_Num[12] ={0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x1D,0x00,0x21 }; //��ȡģ������
unsigned char GR_Search[17]={0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x08,0x04,0x01,0x00,0x00,0x03,0xA1,0x00,0xB2}; //����ָ�� ������Χ 0-929
unsigned char GR_Search_0_9[17]={0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x08,0x04,0x01,0x00,0x00,0x00,0x09,0x00,0x17}; //����0-9��ָ��
unsigned char GR_Img_To_Buffer1[13]={0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x04,0x02,0x01,0x00,0x08}; //��ͼ�����BUFF1
unsigned char GR_Img_To_Buffer2[13]={0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x04,0x02,0x02,0x00,0x09}; //��ͼ�����BUFF2
unsigned char GR_Reg_Model[12]={0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x05,0x00,0x09}; //��buff1��buff2�ϳ�����ģ��
unsigned char GR_Delet_All_Model[12]={0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x0D,0x00,0x11};//ɾ��ָ��ģ���������ָ��
unsigned char GR_Save_Finger[15]={0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x06,0x06,0x01,0X00,0X00,0x00,0x0E};//��buff2�е�����ָ�Ʒŵ�ָ��λ��
unsigned char GR_Delete_Model[16]={0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x07,0x0C,0x00,0x00,0x00,0x01,0x00,0x15}; //ɾ��ָ����ģ��
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


/************ָ��ƥ��ɹ�:����0XEE��
             */
void GR_Save_Finger_Weizhi(u16 PageID)//�����������Ҫ�����ָ�Ƶ�ַ��һ�����㹫ʽ
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
	NVIC_InitStruction.NVIC_IRQChannelPreemptionPriority= 0;//��ռ���ȼ�3
	NVIC_InitStruction.NVIC_IRQChannelSubPriority = 0;		//�����ȼ�3
	NVIC_InitStruction.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStruction);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
//	NVIC_InitStruction.NVIC_IRQChannel = USART1_IRQn;
//	NVIC_InitStruction.NVIC_IRQChannelPreemptionPriority= 0;//��ռ���ȼ�3
//	NVIC_InitStruction.NVIC_IRQChannelSubPriority = 1;		//�����ȼ�3
//	NVIC_InitStruction.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
//	NVIC_Init(&NVIC_InitStruction);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
//	
	
	//DMA���ȼ�        
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  NVIC_InitStruction.NVIC_IRQChannel= DMA1_Channel4_IRQn; 
  NVIC_InitStruction.NVIC_IRQChannelPreemptionPriority= 0; 
  NVIC_InitStruction.NVIC_IRQChannelSubPriority= 2; 
  NVIC_InitStruction.NVIC_IRQChannelCmd= ENABLE; 
  NVIC_Init(&NVIC_InitStruction);
	
	
	NVIC_InitStruction.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStruction.NVIC_IRQChannelPreemptionPriority= 0;//��ռ���ȼ�3
	NVIC_InitStruction.NVIC_IRQChannelSubPriority = 2;		//�����ȼ�3
	NVIC_InitStruction.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStruction);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
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
void Usart_SendString(uint8_t p[],unsigned int n)//����һ����һ���ַ�����
{
	int i;
	for(i=0;i<n;i++)
	{
		USART_SendData(USART3,p[i]);
		while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
	}
}
void Usart2_SendString(uint8_t p[],unsigned int n)//����2����һ���ַ�����
{
	int i;
	for(i=0;i<n;i++)
	{
		USART_SendData(USART2,p[i]);
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
	}
}
void USART3_IRQHandler(void)//�����ָ��ģ��ķ����ж�
{
	int clear;
	
	if(USART_GetITStatus(USART3,USART_IT_RXNE) != RESET)
	{
		Receive[XiaBiao] = USART_ReceiveData(USART3);//����ָ��ģ�鴫�����Ϣ
		XiaBiao++;//��Ϊ�������Ϣ�ĳ��ȵ�һ���ж�
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);
	}
	if(USART_GetFlagStatus(USART3,USART_FLAG_IDLE) != RESET)//��������������е�
	{
		clear = USART3->SR;
		clear = USART3->DR;
		Length1 = XiaBiao;
		XiaBiao=0;//һ��Ҫ����;
		Stop_Flag=1;
		
		//USART_ClearITPendingBit(USART1,USART_FLAG_IDLE);
	}
			
}

uint8_t StringCompare(unsigned char p1[],unsigned char p2[],unsigned char length)//�Ƚ����������С�Ƿ�����ͬ������
{
	int i=0;
	while(1)
	{
		if(p1[i] != p2[i])//��ȥ��'\0'�жϽ���������
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
void Enter_FingerPrint(void)//�����¼��ָ�ƵĽӿ�,ֻҪ�����˽ӿ�,�ͱ����¼��ɹ�,�����һֱ��ʾ��¼ȡָ��;
{
	int Ok=0;//����¼��buffer�ĸ���;
	Usart2_SendString("ABCDEF",6);
	while(Computure_Flag == 0);
	if(Computure_Flag == 1)
	{
		Computure_Flag=0;
		Change_PageID();
		while(1)
		{
			Usart_SendString(GR_Get_Img,12);//���͸�ָ��ģ��
			Delay(0X41FFF0);	//�����ʱ�Ǹ����ڽ������ݵ�һЩʱ��(���˾��û��Ǽ�һ�±ȽϺ�)
			//Usart2_SendString(Receive,Length1);//���ص��Զ�1
			/*�ж��Ƿ�����ָ���µĴ���***/
			if(StringCompare(Receive,Finger,Length1)==1)//�ɼ�ָ�Ƴɹ�
			{
					//Usart2_SendString(Right,1);//0XBB;�ɼ�ָ�Ƴɹ��Ļ�,����һ���ɹ��ı�־BB
					Receive_Ok = 1;//�ɼ�����ָ�Ƴɹ��ı�־							
			}
			else
			{
					Usart2_SendString(Error,1);//0XAA;
					Ok=0;  //�������˼�����´�ͷ��ʼ�ɼ�����;
			}
		/*����ǰ�����ָ�Ժ�Ĵ���****/
			if(Receive_Ok ==1)//������ݲɼ���
			{
				Receive_Ok = 0;//����ָ���µı�־
				if(Ok==0)//(Ok�ǵڼ���BUffer��ı�־)ָ�ƴ���Buffer1��
					Usart_SendString(GR_Img_To_Buffer1,13);//������������ָ�������;
				else if(Ok==1)
					Usart_SendString(GR_Img_To_Buffer2,13);//ָ�ƴ���Buffer2��	
				Delay(0X40FFF0);	//����ʱ�ȴ�ָ��ģ�����Ӧ(��һ��һ��Ҫ�ȴ�,������Ӧ��û���,��ͷ�����,��ʹ���ݶ�ʧ��;
				//Usart2_SendString(Receive,Length1);//����������ֵ������ص����ݴ�������2
				if(StringCompare(Receive,Gen_Picture,Length1)==1)//����ͼ������
				{
					Usart2_SendString(Right,1);//0XBB;�ɹ��Ļ�������һ����־λ
					Ok++;//��¼�ɹ��Ĵ���
				}
				else
				{
					Usart2_SendString(GG,1);//0XAA;û�ɹ���ʱ�򣬱���ԭ��Ok;
					Ok=0;//ֻҪ����;ʧ����,��ȥ���¿�ʼ����,ֱ��¼��ɹ�Ϊֹ
				}
			}
		/***��������������ֱ�浽Buffe1��Buffer2���Ժ�Ĳ���***/
			if(Ok==2)
			{
				Usart_SendString(GR_Reg_Model,12);//��������ģ��
				Delay(0X40FFF0);	//����ʱ
				//Usart2_SendString(Receive,Length1);//3
				if(StringCompare(Reg_Model,Receive,Length1)==1)
				{
						GR_Save_Finger_Weizhi(FingerPageID);
						Usart_SendString(GR_Save_Finger,15);
						Delay(0X40FFFF);//����ʱ
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
						Usart2_SendString(E,1);//����һ��¼�����ָ�ƴ���ı�־
						Ok=0;//���¿�ʼ�ɼ�����
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
void USART2_IRQHandler(void)//����ǿͻ��˷���ָ��ķ����ж�
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
		n=0;//һ��Ҫ����;
		Computure_Flag = 1;
		//USART_ClearITPendingBit(USART2,USART_FLAG_IDLE);
	}
			
}
void ClearAll(unsigned char P[],unsigned length)//����Ļ�������ȫ�������
{
	int i;
	for(i=0;i<length;i++)
	{
		P[i]='0';
	}
}
void Match_Finger(void)//ƥ�����ָ�Ƶ�
{
	  Usart_SendString(GR_Get_Img,12);//���͸�ָ��ģ��
	  Delay(0X40FFFF);
		/*�ж��Ƿ�����ָ���µĴ���***/
	  if(StringCompare(Receive,Finger,Length1)==1)//�ɼ�ָ�Ƴɹ�
		{
				  Receive_Ok = 1;//�ɼ�����ָ�Ƴɹ��ı�־						
		}
//		else 
//			Usart2_SendString(Receive,Length1);//����������ж�ʧ�ܵ�ԭ���ָ��;
//		//��ָ��ģ�鷢�͵����ݣ����͸��ң���ȥ������;
		if(Receive_Ok == 1)//������ݲɼ���
		{
			Receive_Ok = 0;//����ָ���µı�־
			Usart_SendString(GR_Img_To_Buffer1,13);//������������ָ�������;
      Delay(0X40FFFF);			
			if(StringCompare(Receive,Gen_Picture,Length1)==1)//����ͼ������
			{
				  Match_Flag=1;
			}
			if(Match_Flag==1)
			{
				Match_Flag=0;
				Usart_SendString(GR_Search,17);//���������û�������
				Delay(0X40FFFF);
				 //Usart2_SendString(Receive,Length1);//���յ���Ӧ������16λ��
				 if(StringCompare(Receive,MATCH,10)==1)
				 {
					   //Usart2_SendString(Pass,1);//ƥ��ɹ���ʱ��������0XEE;
					 GPIO_ResetBits(GPIOA,GPIO_Pin_4);
					 Delay(0xFFFF);
					 GPIO_SetBits(GPIOA,GPIO_Pin_4);
				 }
				 else
				 {
					 Usart2_SendString(NotPass,1);//ƥ�䲻�ɹ��ɹ���ʱ��������0XCC;
					 GPIO_SetBits(BEEP_Port,BEEP_Pin);
					 Delay(0XFFF);
					 GPIO_ResetBits(BEEP_Port,BEEP_Pin);
				 }
		  }
		}
}

void ClearAll_Finger(void)//�������ָ������ֵ��
{
	Usart_SendString(GR_Delet_All_Model,12);
	Delay(0XFFFF);
	Usart2_SendString(Receive,Length1);
}
void Check_PassWord(void)//�������������
{
	Usart_SendString(GR_Chek,16);
	Delay(0XFFFF);
	Usart2_SendString(Receive,Length1);
}

void Delete_One_Finger(u16 PageID)//һ��ֻɾ��һ��ָ��,���ָ�Ƶ�λ���ɿͷ��˷���
{
	GR_Delet_Finger_Weizhi(PageID);
  Usart_SendString(GR_Delete_Model,16);
	Delay(0X40FFFF);
	Usart2_SendString(Receive,Length1);
}


