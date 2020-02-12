/******************************************************************************

          ��Ȩ���� (C), 2011-2012, �����¿Ƽ�(http://aldsz.taobao.com/)

 ******************************************************************************
  �� �� ��   : system_funtion.c
  �� �� ��   : v2.0
  ��    ��   : Andy
  ��������   : 2012-12-1
  ����޸�   : 
  ��������   : оƬ�ڲ����ܺ���
  �޸���ʷ   :
  1.��    ��   : 2012-11-12
    ��    ��   : Andy
    �޸�����   : �����ļ�
    
  2.��    ��   : 2012-12-1
    ��    ��   : Andy
    �޸�����   : ����DFLASH���ܺ���

******************************************************************************/

/*----------------------------------------------------------------------------*
 * ����ͷ�ļ�                                                                 *
 *----------------------------------------------------------------------------*/
#include "system_funtion.h"

/*----------------------------------------------------------------------------*
 * �궨��                                                                     *
 *----------------------------------------------------------------------------*/
#define FLASH_DFLASH_GLOBAL_ADDR_BASE   0x0010
#define FLASH_DFLASH_LOCAL_ADDR_BASE    0x0800
#define FLASH_DFLASH_PAGE_SIZE          0x0400  // 1Kbytes
/*----------------------------------------------------------------------------*
 * ö�ٶ���                                                                     *
 *----------------------------------------------------------------------------*/
enum tagSystemFlashCmd
{
    FLASH_ERASE_VERIFY_ALL_BLOCKS     = 0x01,
    FLASH_ERASE_VERIFY_BLOCK          = 0x02,
    FLASH_ERASE_VERIFY_PFLASH_SECTION = 0x03,
    FLASH_READ_ONCE                   = 0x04,
    FLASH_PROGRAM_PFLASH              = 0x06,
    FLASH_PROGRAM_ONCE                = 0x07,
    FLASH_ERASE_ALL_BLOCKS            = 0x08,
    FLASH_ERASE_FLASH_BLOCK           = 0x09,
    FLASH_ERASE_PFLASH_SECTOR         = 0x0A,
    FLASH_UNSECURE_FLASH              = 0x0B,
    FLASH_VERIFY_BACKDOOR_ACCESS_KEY  = 0x0C,
    FLASH_SET_USER_MARGIN_LEVEL       = 0x0D,
    FLASH_SET_FIELD_MARGIN_LEVEL      = 0x0E,
    FLASH_ERASE_VERIFY_DFLASH_SECTION = 0x10,
    FLASH_PROGRAM_DFLASH              = 0x11,
    FLASH_ERASE_DFLASH_SECTOR         = 0x12,
}SystemFlashCmd_E;

/*----------------------------------------------------------------------------*
 * ȫ�ֱ���                                                                   *
 *----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*
 * �ڲ�����ԭ��                                                               *
 *----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*
 * �ⲿ����ԭ��                                                               *
 *----------------------------------------------------------------------------*/

/*****************************************************************************
 �� �� ��  : PLL_Init
 ��������  : ��ʼ�����໷
 �������  : NONE  
 �������  : NONE
 �� �� ֵ  : NONE
*****************************************************************************/
VOID SYSTEM_PLL_Init(VOID) 
{
    CLKSEL &= 0x7f;       //set OSCCLK as sysclk
    PLLCTL &= 0x8F;       //Disable PLL circuit
    CRGINT &= 0xDF;

    #if (SYSTEM_BUS_CLOCK == 40000000) 
    SYNR = 0x44;
    #elif (SYSTEM_BUS_CLOCK == 64000000)
    SYNR = 0xc7;     
    #elif (SYSTEM_BUS_CLOCK == 24000000)
    SYNR = 0x42;
    #endif 

    REFDV  = 0xC1;            //PLLCLK=2��OSCCLK��(SYNR+1)/(REFDV+1)��128MHz ,fbus=32M
    PLLCTL = PLLCTL | 0x70;   //Enable PLL circuit
    
    asm NOP;
    asm NOP;
    
    while (!(CRGFLG & 0x08)); //PLLCLK is Locked already
    CLKSEL |= 0x80;           //set PLLCLK as sysclk

    return;
}

/*****************************************************************************
 �� �� ��  : SYSTEM_ATD_Init
 ��������  : оƬ�ڲ���ģת��ģ���ʼ��
 �������  : NONE  
 �������  : NONE
 �� �� ֵ  : NONE
*****************************************************************************/
VOID SYSTEM_ATD_Init(VOID)
{
    ATD0CTL2 = 0xc0;  //����A/D, ��������, �޵ȴ�ģʽ, ��ֹ�ⲿ����, �жϽ�ֹ
    ATD0CTL1_SRES = 0;  //ѡ��8λģ��ת��
    ATD0CTL3 = 0x88;  //ÿ��ֻת��һ��ͨ��,No FIFO, Freezeģʽ�¼���ת��
    ATD0CTL4 = 0x01;  //����ADģ��ʱ��Ƶ��

    return;
}

/*****************************************************************************
 �� �� ��  : SYSTEM_ATD_Convert
 ��������  : оƬ�ڲ���ģת��
 �������  : VOID  
 �������  : NONE
 �� �� ֵ  : UCHAR8 ��ģת����� 0-255
*****************************************************************************/
UCHAR8 SYSTEM_ATD_Convert(VOID) 
{
    UCHAR8 ucATDResult;
    
    ATD0CTL5 = 0x06;    //ת��AD06
    while (!ATD0STAT0_SCF);
    ucATDResult = ATD0DR0L;

    return ucATDResult;
}

/*****************************************************************************
 �� �� ��  : SYSTEM_TIME_Delay
 ��������  : ��ʱ���� 
 �������  : USHORT16 usTime
 �������  : NONE
 �� �� ֵ  : NONE
*****************************************************************************/
VOID SYSTEM_TIME_Delay(USHORT16 usTime)
{
    ULONG32 i;
    USHORT16 j;
    
    for (i = (ULONG32)100 * usTime; i > 0; i--)
    {
        for(j = 210; j > 0; j--)
        {
        }
    }
    
    return;
}
  
/*****************************************************************************
 �� �� ��  : SYSTEM_DFLASH_Init
 ��������  : оƬ�ڲ�DFlash��ʼ��
 �������  : NONE  
 �������  : NONE
 �� �� ֵ  : NONE
*****************************************************************************/
VOID SYSTEM_DFLASH_Init(VOID)  
{    
    while (!FSTAT_CCIF);      //������FLASH�Ĵ���д��ǰ���豣֤CCIF��Ϊ0���ϴ��������
    FCLKDIV = 0x0F;           //�ⲿ����Ϊ16M.FLASHʱ�Ӳ�����1M����������ֲ�  
    FCNFG   = 0x00;           //��ֹ�ж�  
    while (!FCLKDIV_FDIVLD);  //�ȴ�ʱ�����óɹ�

    return;
}  

/*****************************************************************************
 �� �� ��  : SYSTEM_DFLASH_Erase
 ��������  : ����DFLASH��һ������
             S12XS128��DFLASH����32��������ÿ����265 bytes����8Kbytes
             ����  ��ʼ��ַ�ĵ�16λ
               0:  0x0000     1:  0x0100     2:  0x0200     3:  0x0300
               4:  0x0400     5:  0x0500     6:  0x0600     7:  0x0700
               8:  0x0800     9:  0x0900    10:  0x0A00    11:  0x0B00
              12:  0x0C00    13:  0x0D00    14:  0x0E00    15:  0x0F00
              16:  0x1000    17:  0x1100    18:  0x1200    19:  0x1300
              20:  0x1400    21:  0x1500    22:  0x1600    23:  0x1700
              24:  0x1800    25:  0x1900    26:  0x1A00    27:  0x1B00
              28:  0x1C00    29:  0x1D00    30:  0x1E00    31:  0x1F00

 �������  : USHORT16 usAddress ��ѡ��������ʼ��ַ�ĵ�16λ
 �������  : NONE
 �� �� ֵ  : NONE
*****************************************************************************/
VOID SYSTEM_DFLASH_Erase(USHORT16 usAddress)
{
    while (!FSTAT_CCIF);     //������FLASH�Ĵ���д��ǰ���豣֤CCIF��Ϊ0���ϴ��������
    if (1 == FSTAT_ACCERR)   //�ж϶�д�����־
    {
        FSTAT_ACCERR = 1;    //�����־λ
    }
    if (1 == FSTAT_FPVIOL)   //�жϱ���Υ����־
    {
        FSTAT_FPVIOL = 1;    //�����־λ
    }
    
    FCCOBIX_CCOBIX = 0x00; 
    FCCOB = ((USHORT16)FLASH_ERASE_DFLASH_SECTOR << 8) | FLASH_DFLASH_GLOBAL_ADDR_BASE; //д������͸�λ��ַ  
    FCCOBIX_CCOBIX = 0x01;   
    FCCOB = usAddress;       //д���16λ��ַ 
    
    FSTAT_CCIF=1;            //����ִ������  
    while (!FSTAT_CCIF);     //�ȴ�ִ�����  

    return;
}  

/*****************************************************************************
 �� �� ��  : SYSTEM_DFLASH_Write
 ��������  : ��оƬ�ڲ�DFlashд�����ݣ�����д�����ݵĿռ�д��ǰ�����������д��ʧ��
 �������  : USHORT16 usAddress   0x0000 - 0x1FF8
             USHORT16 *pusDataBuf ����4words(8bytes)�Ļ����ַ
 �������  : NONE
 �� �� ֵ  : NONE
*****************************************************************************/
VOID SYSTEM_DFLASH_Write(USHORT16 usAddress, USHORT16 *pusDataBuf)  
{  
    while (!FSTAT_CCIF);     //������FLASH�Ĵ���д��ǰ���豣֤CCIF��Ϊ0���ϴ��������
    if(1 == FSTAT_ACCERR)    //�ж϶�д�����־
    {
        FSTAT_ACCERR = 1;    //�����־λ
    }
    if(1 == FSTAT_FPVIOL)    //�жϱ���Υ����־
    {
        FSTAT_FPVIOL = 1;    //�����־λ
    }

    FCCOBIX_CCOBIX = 0x00;   
    FCCOB = ((USHORT16)FLASH_PROGRAM_DFLASH << 8) | FLASH_DFLASH_GLOBAL_ADDR_BASE; //д������͸�λ��ַ 
    FCCOBIX_CCOBIX = 0x01;   
    FCCOB = usAddress;       //д���16λ��ַ  
    FCCOBIX_CCOBIX = 0x02;   
    FCCOB = pusDataBuf[0];   //д���һ������  
    FCCOBIX_CCOBIX = 0x03;   
    FCCOB = pusDataBuf[1];   //д��ڶ�������  
    FCCOBIX_CCOBIX = 0x04;   
    FCCOB = pusDataBuf[2];   //д�����������  
    FCCOBIX_CCOBIX = 0x05;   
    FCCOB = pusDataBuf[3];   //д����ĸ�����  

    FSTAT_CCIF = 1;          //д��ִ������  
    while (!FSTAT_CCIF);     //�ȴ�ִ�����  

    return;
}  
  
/*****************************************************************************
 �� �� ��  : SYSTEM_DFLASH_Read
 ��������  : ��оƬ�ڲ�DFlash��������
 �������  : USHORT16 usAddress   0x0000 - 0x1FF8 
             USHORT16 *pusDataBuf ����4words(8bytes)�Ļ����ַ
 �������  : NONE
 �� �� ֵ  : NONE
*****************************************************************************/
VOID SYSTEM_DFLASH_Read(USHORT16 usAddress, USHORT16 *pusDataBuf)  
{  
    UCHAR8 ucLastEpage;
    USHORT16 *pusMapAddr;

    ucLastEpage = EPAGE;     //����EPAGE��ֵ  

    EPAGE = (UCHAR8)(usAddress >> 10); //����EPAGE  

    //����usAddressӳ�䵽CPU�ڴ�ĵ�ַ
    pusMapAddr = (USHORT16*)(FLASH_DFLASH_LOCAL_ADDR_BASE + (usAddress & (FLASH_DFLASH_PAGE_SIZE - 1))); 

    pusDataBuf[0] = *pusMapAddr;
    pusDataBuf[1] = *(pusMapAddr + 1);
    pusDataBuf[2] = *(pusMapAddr + 2);
    pusDataBuf[3] = *(pusMapAddr + 3);

    EPAGE = ucLastEpage;      //�ָ�EPAGE��ֵ  

    return;
}  

