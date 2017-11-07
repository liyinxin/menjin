#ifndef __ZHIWEN_H
#define __ZHIWEN_H

#include "stm32f10x.h"

extern uint8_t FingerPageID;
extern int Test ;
extern unsigned char Clear_ku[12];
extern unsigned char Search_ku[17];
extern unsigned char GR_Get_Img[12];
extern unsigned char GR_Templete_Num[12];
extern unsigned char GR_Search[17];
extern unsigned char GR_Search_0_9[17];
extern unsigned char GR_Img_To_Buffer1[13];
extern unsigned char GR_Img_To_Buffer2[13];
extern unsigned char GR_Reg_Model[12];
extern unsigned char GR_Delet_All_Model[12];
extern unsigned char GR_Save_Finger[15];
extern unsigned char GR_Delete_Model[16];
extern unsigned char GR_Chek[16];
extern unsigned char Receive[100];
extern unsigned char Stop_Flag;
extern unsigned char XiaBiao,Length1,Length2,Match_Flag;
extern unsigned char Reg_Model[12];
extern unsigned char Finger[12];
extern unsigned char Gen_Picture[12];
extern unsigned char Error[1];
extern unsigned char Right[1];
extern unsigned char Receive_Ok;
extern unsigned char R[1],E[1];
extern unsigned char Store_Finger[12],MATCH[10];
extern unsigned char ComputureCommand[100],n;
extern unsigned char Computure_Flag;
extern unsigned char TuiChu;
void Usart_Init(void);

void Usart_SendString(unsigned char p[],unsigned int n);
void Usart2_SendString(unsigned char p[],unsigned int n);
void Usart3_SendString(unsigned char p[],unsigned int n);
uint8_t StringCompare(unsigned char p1[],unsigned char p2[],unsigned char length);
void DMA_InitConfig(void);
void Enter_FingerPrint(void);
void Delay(u32 Ucount);
void Match_Finger(void);
void ClearAll_Finger(void);
void GR_Save_Finger_Weizhi(u16 PageID);
void Check_PassWord(void);
void ClearAll(unsigned char P[],unsigned length);
void Delete_One_Finger(u16 PageID);
void Change_PageID(void);
#endif

