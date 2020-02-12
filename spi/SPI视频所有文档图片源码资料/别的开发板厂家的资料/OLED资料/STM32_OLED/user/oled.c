#include "oled.h"
#include "AsciiLib.h"
//#include "HzLib.h"


#define     XLevelL		    0x00
#define     XLevelH		    0x10
#define     XLevel		    ((XLevelH&0x0F)*16+XLevelL)
#define     Max_Column	    128
#define     Max_Row		    64
#define	    Brightness	    0xCF



#define     X_WIDTH         128
#define     Y_WIDTH         64
#define		Page			8
void OLED_WB(uint8_t data)
{
    /* Loop while DR register in not emplty */
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    /* Send byte through the SPI2 peripheral */
    SPI_I2S_SendData(SPI1, data);
}
/*******************一个字节数据写入***********************/
void LCD_WrDat(unsigned char dat)
{
    OLED_DC_H;
	OLED_WB(dat);
}

/********************一条指令写入**********************/
void LCD_WrCmd(unsigned char cmd)
{
    OLED_DC_L;
    OLED_WB(cmd);
}

/**********************设置显示位置**********************/
void LCD_Set_Pos(unsigned char x, unsigned char y)
{
	/* Page addressing mode */
    LCD_WrCmd(0xb0+(y&0x07));/* set page start address */
    LCD_WrCmd(x&0x0f);/* set lower nibble of the column address */
    LCD_WrCmd(((x&0xf0)>>4)|0x10); /* set higher nibble of the column address */
}

/**********************写满屏数据**********************/
void LCD_Fill(unsigned char bmp_dat)
{
	unsigned char y,x;

    LCD_WrCmd(0x20);//-Set Page Addressing Mode (0x00/0x01/0x02)
    LCD_WrCmd(0x00);//

	LCD_WrCmd(0x21);//-Set Column Address
	LCD_WrCmd(0x00);
	LCD_WrCmd(0x7f);

	LCD_WrCmd(0x22);//-Set Page Address
	LCD_WrCmd(0x00);
	LCD_WrCmd(0x07);

	LCD_DLY_ms(1);/* 等待内部稳定   */

    for(y=0;y<Page;y++)
    {
    	for(x=0;x<X_WIDTH;x++)
    	{
    		LCD_WrDat(bmp_dat);
    	}
    }
//    LCD_WrCmd(0xaf);//--turn off oled panel
}

/*********************清屏函数***********************/
void LCD_CLS(void)
{
	unsigned char y,x;
	for(y=0;y<8;y++)
	{
		LCD_WrCmd(0xb0+y);
		LCD_WrCmd(0x01);
		LCD_WrCmd(0x10);
		for(x=0;x<X_WIDTH;x++)
		LCD_WrDat(0);
		LCD_DLY_ms(200);
	}
}

/*********************延时函数***********************/
void LCD_DLY_ms(unsigned int ms)
{
    unsigned int a;
    while(ms)
    {
        a=1335;
        while(a--);
        ms--;
    }
    return;
}


/*********************12864初始化***********************/
void LCD_Init(void)
{
	OLED_RST_L;
	LCD_DLY_ms(50);
	OLED_RST_H;
	//从上电到下面开始初始化要有足够的时间，即等待RC复位完毕

    LCD_WrCmd(0xae);//--turn off oled panel

    LCD_WrCmd(0xa8);//--set multiplex ratio(1 to 64)
    LCD_WrCmd(0x3f);//--1/64 duty
    LCD_WrCmd(0xd3);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
    LCD_WrCmd(0x00);//-not offset
    LCD_WrCmd(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
    LCD_WrCmd(0xa0);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
    LCD_WrCmd(0xc0);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
    LCD_WrCmd(0xda);//--set com pins hardware configuration
    LCD_WrCmd(0x12);
    LCD_WrCmd(0x81);//--set contrast control register
    LCD_WrCmd(0xcf); // Set SEG Output Current Brightness
    LCD_WrCmd(0xa4);// Disable Entire Display On (0xa4/0xa5)
    LCD_WrCmd(0xa6);// Disable Inverse Display On (0xa6/a7)
    LCD_WrCmd(0xd5);//--set display clock divide ratio/oscillator frequency
    LCD_WrCmd(0x80);//--set divide ratio, Set Clock as 100 Frames/Sec
    LCD_WrCmd(0x8d);//--set Charge Pump enable/disable
    LCD_WrCmd(0x14);//--set(0x10) disable
    LCD_WrCmd(0xaf);//--turn on oled panel

    LCD_WrCmd(0xd9);//--set pre-charge period
    LCD_WrCmd(0xf8);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock

    LCD_WrCmd(0xdb);//--set vcomh
    LCD_WrCmd(0x40);//Set VCOM Deselect Level

    LCD_Fill(0x00);  //初始清屏
}

