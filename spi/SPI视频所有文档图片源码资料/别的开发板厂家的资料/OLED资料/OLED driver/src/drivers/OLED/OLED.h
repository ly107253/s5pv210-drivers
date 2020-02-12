/*****************************************
�� �� ����LED.h
�� �� �ߣ������¿Ƽ�
�������ڣ�2012/11/08
��    ����LED128x64��ʾ���ײ�����
------------------------------------------
����ͷ�ļ���


 void LED_Init(void);
 void LED_CLS(void); 
 void LED_Set_Pos(byte x, byte y);//�������꺯��
 void LED_WrDat(uchar data);   //д���ݺ���
 void LED_Fill(byte dat);
 void LED_P6x8Char(byte x,byte y,byte ch);
 void LED_P6x8Str(byte x,byte y,byte ch[]);
 void LED_P8x16Str(byte x,byte y,byte ch[]);
 void LED_P14x16Str(byte x,byte y,byte ch[]);
 void LED_PXx16MixStr(byte x, byte y, byte ch[]);
 
void LED_PrintBMP(byte x0,byte y0,byte x1,byte y1,byte bmp[]); 
void LED_PrintValueC(uchar x, uchar y,char data);
void LED_PrintValueI(uchar x, uchar y, int data);
void LED_PrintValueF(uchar x, uchar y, float data, uchar num);
void LED_PrintPIC(uchar table[ROW][COLUMN]);
void LED_PrintPIC2(uchar table[ROW][COLUMN]);
void LED_PrintEdge(void);
void LED_Cursor(uchar cursor_column, uchar cursor_row);

*****************************************/

#ifndef __OLED_H__
#define __OLED_H__

//#include "short_w.h"
#include "common.h"

#include "gpio.h"

/*#define LED_SCL       PORTA_PA0  
#define LED_SDA       PORTA_PA1
#define LED_RST       PORTA_PA2 
#define LED_DC         PORTA_PA3 
#define LED_PORT    DDRA*/

/*****************************************
���� �� ��:��ʼ�����Ŷ���

��˵ �� ��:ֱ���������IO�Ϳ�����
*******************************************/
#define SCL_PIN PORTA,24
#define SDA_PIN PORTA,25
#define RST_PIN PORTA,26
#define DC_PIN PORTA,27
#define INIT 0
#define LED_SCL_Init  gpio_init(SCL_PIN,GPO,INIT) // ʱ�ӳ�ʼ������
#define LED_SDA_Init  gpio_init(SDA_PIN,GPO,INIT)//���ݿ�D0
#define LED_RST_Init  gpio_init(RST_PIN,GPO,INIT)//��λ���ܵ�ƽ
#define LED_DC_Init   gpio_init(DC_PIN,GPO,INIT)//ƫ�ó���
/**********************************************

���� �� ��:IO ���� OUTPUT

��˵ �� ��:ֱ���������IO�Ϳ�����
*******************************************/
#define LED_SCLH  gpio_set(SCL_PIN,HIGH)// ʱ�Ӷ��� 
#define LED_SCLL  gpio_set(SCL_PIN,LOW)

#define LED_SDAH  gpio_set(SDA_PIN,HIGH)//���ݿ�D0
#define LED_SDAL  gpio_set(SDA_PIN,LOW)

#define LED_RSTH  gpio_set(RST_PIN,HIGH)//��λ���ܵ�ƽ
#define LED_RSTL  gpio_set(RST_PIN,LOW)

#define LED_DCH   gpio_set(DC_PIN,HIGH)
#define LED_DCL   gpio_set(DC_PIN,LOW)//ƫ�ó���

/************************************************/

void  LEDPIN_Init(void);   //LED�������ų�ʼ��
void LED_Init(void);
 void LED_CLS(void);
 void LED_Set_Pos(byte x, byte y);//�������꺯��
 void LED_WrDat(u8 data);   //д���ݺ���
 void LED_P6x8Char(byte x,byte y,byte ch);
 void LED_P6x8Str(byte x,byte y,byte ch[]);
 void LED_P8x16Str(byte x,byte y,byte ch[]);
 void LED_P14x16Str(byte x,byte y,byte ch[]);
 void LED_PXx16MixStr(byte x, byte y, byte ch[]);
 void LED_PrintBMP(byte x0,byte y0,byte x1,byte y1,byte bmp[]); 
 void LED_Fill(byte dat);
 void LED_PrintValueC(u8 x, u8 y,char data);
 void LED_PrintValueI(u8 x, u8 y, int data);
 void LED_PrintValueF(u8 x, u8 y, float data, u8 num);
 void LED_PrintEdge(void);
 void LED_Cursor(u8 cursor_column, u8 cursor_row);
 void LED_PrintLine(void);

#endif