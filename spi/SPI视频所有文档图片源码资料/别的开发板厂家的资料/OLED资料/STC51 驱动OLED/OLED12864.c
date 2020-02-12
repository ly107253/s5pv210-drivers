/********************************************
OLED�����Գ���-STC&89C52 
*********************************************************/
#include "REG51.h"
#include "intrins.h"
#include "codetab.h"
#include "LQ12864.h"

/********************��STC12C5608AD  11MHZ z=1ʱ��ȷ��ʱ1ms��*******************/
void delayms(unsigned int z)
{
	unsigned int x,y;
	for(x=z; x>0; x--)
		for(y=1848; y>0; y--);
}
/*********************������************************************/
void main(void) 
{
	unsigned char i;   
	LCD_Init(); //oled ��ʼ��  
	while(1)
	{
		//LCD_Fill(0xff); //��ȫ�� 
		//LCD_Fill(0x00); //��ȫ�� 
		for(i=0; i<8; i++)
		{
			LCD_P16x16Ch(i*16,0,i);  //������ʾ
			LCD_P16x16Ch(i*16,2,i+8);
			LCD_P16x16Ch(i*16,4,i+16);
			LCD_P16x16Ch(i*16,6,i+24);
		} 
		delayms(4000); 
		LCD_CLS();   
	        LCD_P8x16Str(44,0,"saler");
	        LCD_P8x16Str(20,2,"OLED DISPLAY");
	        LCD_P8x16Str(8,4,"TEL:13088850665");
	        LCD_P6x8Str(20,6,"ald16888@163.com");
	        LCD_P6x8Str(20,7,"2013-01-10 18:18");
		delayms(4000);  
		LCD_CLS();
		Draw_BMP(0,0,128,8,BMP);  //ͼƬ��ʾ
		delayms(4000);  
	}
	
}
