/******************************************************************************

          ��Ȩ���� (C), 2011-2012, �����¿Ƽ�(http://aldsz.taobao.com/)

 ******************************************************************************
  �� �� ��   : main.c
  �� �� ��   : v1.0
  ��    ��   : Andy
  ��������   : 2012-11-10
  ����޸�   : 
  ��������   : OLED��ʾ����
               ˵��: 
               ----------------------------------------------------------------
               G    ��Դ��
               VCC  ��5V��3.3v��Դ
               D0   PORTA_PA0 ��Ϊ LED_SCL
               D1   PORTA_PA1 ��Ϊ LED_SDA
               RST  PORTA_PA2 ��Ϊ LED_RST
               DC   PORTA_PA3 ��Ϊ LED_DC 
               CS   �ѽӵأ����ý�                     
               ----------------------------------------------------------------
  �޸���ʷ   :
  1.��    ��   : 2012-11-10
    ��    ��   : Andy
    �޸�����   : �����ļ�

******************************************************************************/

/*----------------------------------------------------------------------------*
 * ����ͷ�ļ�                                                                 *
 *----------------------------------------------------------------------------*/
#include<string.h>
#include"common.h"
#include"system_funtion.h"
#include"oled.h"

/*----------------------------------------------------------------------------*
 * �궨��                                                                     *
 *----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*
 * ȫ�ֱ���                                                                   *
 *----------------------------------------------------------------------------*/
//�˲����ڴ�ܴ�ʹ��ʱ�밴��Ҫ����
UCHAR8 g_aucImageBuf[32][LED_MAX_COLUMN_NUM];

/*----------------------------------------------------------------------------*
 * �ڲ�����ԭ��                                                               *
 *----------------------------------------------------------------------------*/
VOID MAIN_Init(VOID);
VOID MAIN_OLED_Display(VOID);

/*----------------------------------------------------------------------------*
 * �ⲿ����ԭ��                                                               *
 *----------------------------------------------------------------------------*/

VOID main(void) 
{
    MAIN_Init();

    MAIN_OLED_Display();
    
    return;
}

VOID MAIN_Init(VOID)
{
    SYSTEM_PLL_Init();
    LED_Init();

    return;
}

VOID MAIN_OLED_Display(VOID)
{
    USHORT16 i, j;

    LED_P14x16Str(25, 0, (UCHAR8*)"�����¿Ƽ�");
    LED_P8x16Str(0, 2, (UCHAR8*)" http://aldsz.taobao.com");
    SYSTEM_TIME_Delay(2000);

    LED_Fill(0x00);    
    LED_P14x16Str(30, 1, (UCHAR8*)"��Ļ����");

    for (j = 0; j <= 100; j++)
    {
        SYSTEM_TIME_Delay(40);
        LED_PrintChar(45, 5, (CHAR8)j);        
    }

    LED_Fill(0xFF); //����ȫ��
    SYSTEM_TIME_Delay(2000);

    LED_Fill(0x00); //����
    SYSTEM_TIME_Delay(2000);

    //ֻҪ��������Ϣ�Ž�ͼƬ������
    //Ȼ�����LED_PrintImage����������ʾʵʱ������Ϣ

    //���ͼƬ����
    (VOID)memset(g_aucImageBuf, 0x00, 32 * LED_MAX_COLUMN_NUM);

    //��������ɨ��
    for (i = 0; i < 32; i++)
    {
        for (j = 0; j < LED_MAX_COLUMN_NUM; j++)
        {
            g_aucImageBuf[i][j] = LED_IMAGE_WHITE;
        }
        //�ú�����Ҫ��ͼƬ����̫�������ʹ��
        LED_PrintImage((UCHAR8*)g_aucImageBuf, 32, LED_MAX_COLUMN_NUM);
        SYSTEM_TIME_Delay(10);
    }

    //���ͼƬ����
    (VOID)memset(g_aucImageBuf, 0x00, 32 * LED_MAX_COLUMN_NUM);
    
    //��������ɨ��
    for (i = 0; i < LED_MAX_COLUMN_NUM; i++)
    {
        for (j = 0; j < 32; j++)
        {
            g_aucImageBuf[j][i] = LED_IMAGE_WHITE;
        }
        //�ú�����Ҫ��ͼƬ����̫�������ʹ��
        LED_PrintImage((UCHAR8*)g_aucImageBuf, 32, LED_MAX_COLUMN_NUM);
        SYSTEM_TIME_Delay(10);
    }
    
    LED_Fill(0x00); //����
    SYSTEM_TIME_Delay(1000);
    
    LED_P8x16Str(10, 2, (UCHAR8*)"Test Finished!");

    for (;;)
    {
        ;
    }
    
    return;
}

