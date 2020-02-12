#include "oled.h"
#include "sys.h"
#include "delay.h"
#include "oled_word_library.h" //OLED�ֿ⣻

//����ϳ�ʼ��OLED�����е��õ��ĺ����ں�����Զ���;
void OLED_Init(void)
{
	OLED_SET_GPIO; //����GPIOʱ�Ӻ�ģʽ,����IO�ڵĲ�ͬ�� "oled.h" �н����޸ģ�

	OLED_CLK=1;	 //����ʱ���ߣ�
    OLED_RST=0;	 //��λoled;
    delay_ms(50);
    OLED_RST=1;      //ֹͣ��λ��   
    OLED_WriteCommand(0xae);//--turn off oled panel
    OLED_WriteCommand(0x00);//---set low column address
    OLED_WriteCommand(0x10);//---set high column address
    OLED_WriteCommand(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
    OLED_WriteCommand(0x81);//--set contrast control register
    OLED_WriteCommand(0xcf); // Set SEG Output Current Brightness
    OLED_WriteCommand(0xa1);//--Set SEG/Column Mapping     
    OLED_WriteCommand(0xc8);//Set COM/Row Scan Direction  
    OLED_WriteCommand(0xa6);//--set normal display
    OLED_WriteCommand(0xa8);//--set multiplex ratio(1 to 64)
    OLED_WriteCommand(0x3f);//--1/64 duty
    OLED_WriteCommand(0xd3);//-set display offset    Shift Mapping RAM Counter (0x00~0x3F)
    OLED_WriteCommand(0x00);//-not offset
    OLED_WriteCommand(0xd5);//--set display clock divide ratio/oscillator frequency
    OLED_WriteCommand(0x80);//--set divide ratio, Set Clock as 100 Frames/Sec
    OLED_WriteCommand(0xd9);//--set pre-charge period
    OLED_WriteCommand(0xf1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    OLED_WriteCommand(0xda);//--set com pins hardware configuration
    OLED_WriteCommand(0x12);
    OLED_WriteCommand(0xdb);//--set vcomh
    OLED_WriteCommand(0x40);//Set VCOM Deselect Level
    OLED_WriteCommand(0x20);//-Set Page Addressing Mode (0x00/0x01/0x02)
    OLED_WriteCommand(0x02);//
    OLED_WriteCommand(0x8d);//--set Charge Pump enable/disable
    OLED_WriteCommand(0x14);//--set(0x10) disable
    OLED_WriteCommand(0xa4);// Disable Entire Display On (0xa4/0xa5)
    OLED_WriteCommand(0xa6);// Disable Inverse Display On (0xa6/a7) 
    OLED_WriteCommand(0xaf);//--turn on oled panel
    Clear_Color;  //��ʼ����,��ɫ��
	//Clear_Black; //��ʼ����,��ɫ
	OLED_SetPosition(0,0); //������ʼ�����ꣻ
}

void OLED_WriteCommand(unsigned char Cmd)
{
	unsigned char i,Val;
    OLED_DC = OLED_COMMAND;	//����/����ѡ����ѡ���
    for(i=0;i<8;i++) //����һ���ֽ�,���ͷ���Ӹ�λ��ʼ�� 
    { 
        OLED_CLK=0;  //����ʱ����     
        Val = Cmd & 0x80;
        if (Val == 0)
        {
            OLED_DAT = 0;
        }
        else
        {
            OLED_DAT = 1;
        }
        OLED_CLK=1;	//����ʱ���ߣ����������أ�һλ���ݱ����ͣ�
        Cmd<<=1;;        
    }     
}

void OLED_WriteByte(unsigned char Data)
{
	unsigned char i,Val;
    OLED_DC = OLED_DATA;	//����/����ѡ����ѡ���ݣ�
    for(i=0;i<8;i++) //����һ���ֽ�,���ͷ���Ӹ�λ��ʼ�� 
    { 
        OLED_CLK=0;  //����ʱ����     
        Val = Data & 0x80;
        if (Val == 0)
        {
            OLED_DAT = 0;
        }
        else
        {
            OLED_DAT = 1;
        }
        OLED_CLK=1;	//����ʱ���ߣ����������أ�һλ���ݱ����ͣ�
        Data<<=1;;        
    }     
}

//��Val = 0xff�������ṩ����Val�Ӵ��ܣ�
void OLED_Screen(unsigned char Val)
{
	unsigned char x,y; //xΪҳ���������ΪOLED_PAGE��
	for(x=0;x<OLED_PAGE;x++)
	{
		OLED_WriteCommand(0xb0+x);	//�������ü��ļ����е�ͼʾ��
		OLED_WriteCommand(0x01);
		OLED_WriteCommand(0x10);
		for(y=0;y<OLED_X;y++) { OLED_WriteByte(Val); }
	}
}

void OLED_SetPosition(unsigned char x , unsigned char y)  //yΪҳ��
{
	OLED_WriteCommand(0xb0+y);
    OLED_WriteCommand(((x&0xf0)>>4)|0x10);
    OLED_WriteCommand((x&0x0f)|0x01); 
}

/*
	16*16ģʽһ�������ܹ���Ҫ��ʾ256����,256/8=32,������һ��������32���ֽ�ȷ��;
	����ԭ������ʾģʽ����16*16ʱ��oled(128*64)������4��8�У������32�����֣�ÿ��8�����֣�
oledû�б���������ҳ�洢����Ϊһ��ҳֻ��128���ֽڣ���ÿ������32���ֽڣ���һ��ҳ����4�����֣�
ƫ������Ӧ�ֿ�����ĵڼ������֣��±�Ϊ0���Ǹ����ּ���һ��������ƫ�������ڶ�������Ҫƫ�Ƶ�һ
�����ֵ�������ƫ������16*16ģʽ��ƫ����Ϊ32���ֽڣ�
	4��8��
*/
//XΪ�У�YΪ�У�NumΪ�ֿ��еĵڼ������֣��±��1��ʼ�����ӷ���ʹ�ù淶��
void OLED_Show_FONT_16x16(unsigned char x , unsigned char y , unsigned char Num) //NΪƫ������
{
	unsigned char i;
    unsigned int adder;  //ƫ���������ü���һ������˵����	
	{
		Num--;
		x--;
		y--;
		x *= 2;
		y *= 16;  
	}//���е����ת����
	adder = 32*Num;    
    OLED_SetPosition(y , x); //һ������������ҳ��������һҳ�� 
    for(i = 0;i < 16;i++)              
    {
        OLED_WriteByte(FONT_16x16[adder]); //д���ֽڣ�   
        adder += 1;
    }      
    OLED_SetPosition(y,x + 1); //�ڶ�ҳ��   
    for(i = 0;i < 16;i++)       
    {
        OLED_WriteByte(FONT_16x16[adder]);
        adder += 1;
    }           	
}


#if FONT_8x16_MODE //�������˸ú꣬���ʾ6*8 ASCII���׼����ģʽ������
//8*16 ����ģʽ��ʾ������
/*
	 x: �У�y: ��
	128*64oled ��8*16ģʽ�¿��������ʾ64���ַ���ÿ���ַ�����ռ������ҳ�е����ݣ�
��4�У�64/4 = 16����ÿ��16���ַ���
	4��16��
*/
void OLED_Show_FONT_8x16(unsigned char x , unsigned char y , unsigned char Data[])
{
	unsigned char ascii_num = 0, //�ַ�ת����ASCII������ֿ��������ľ���λ�ã�
				  count = 0, //��ǰ�ַ�������¼�ַ�����ĳһ�����ַ����±ꣻ
				  i = 0;   //ѭ��������һ���ַ���6���ֽ���ʾ������Ҫ����д���ֽڣ�
	{
		x--;
		y--;
		x *= 2;
		y *= 8;
	}//���е����ת����
    while (Data[count]!='\0')
    {
		if(y>127)
		{
			y=0;
			x++;
			x++;  //ÿ����Ҫ����ҳ��
		}    
        ascii_num = Data[count]-32;  //�±ꣻ     
        OLED_SetPosition(y,x);  //��һҳ��  
        for(i=0;i<8;i++)
		{     
            OLED_WriteByte(FONT_8x16[ascii_num*16+i]);
		}
        OLED_SetPosition(y,x+1); //�ڶ�ҳ��   
        for(i=0;i<8;i++) 
		{    
            OLED_WriteByte(FONT_8x16[ascii_num*16+i+8]); 
		} 
        y+=8; //һ���ַ�ռ��8���㣻
        count++; //�¸��ַ���
    }	
}

#endif


#if FONT_6x8_MODE //�������˸ú꣬���ʾ6*8 ASCII���׼����ģʽ������
//6*8 ����ģʽ��ʾ������
/*
	 x: �У�y: �� 
	 ����6*8ģʽ(һ���ַ�ռ6���ֽ�)���� 128*64��oled��������ʾ170���ַ�,ÿ���ַ�
����Ҫ��һҳ����ʾ���ʿ�����ʾ8��(ҳ)��170/8=21����ÿ�п�����ʾ21���ַ���������Ҫ˵
�����ǣ�ÿ����ʾ21���ֽں��������㣬��ÿ��128����ֻ�õ���126���㣻
	8��21��
*/
void OLED_Show_FONT_6x8(unsigned char x , unsigned char y , unsigned char Data[])
{
	unsigned char ascii_num = 0, //�ַ�ת����ASCII������ֿ��������ľ���λ�ã�
				  count = 0, //��ǰ�ַ�������¼�ַ�����ĳһ�����ַ����±ꣻ
				  i = 0;   //ѭ��������һ���ַ���6���ֽ���ʾ������Ҫ����д���ֽڣ�
	{
		x--;
		y--; //���������������ǽ�Ϊ���ϵķ�ʽ����������
			//Ҫת�����磺xΪ1ʱʵ�ʱ�ʾ��һ�У�����������ǵ�0��
		y *= 6;
	}//���е����ת����
	while(Data[count] != '\0') //�ַ�����û������
	{
		if(y>=126)  //���һ�����꣬������һ�е���ʼλ��
		{
			y = 0;  //��ʼλ��
			x++;	//��һҳ(��)��
		}
		ascii_num = Data[count] - 32; //�ַ�ת�����ֿ��Ӧ�������ʼ���ꣻ
		OLED_SetPosition(y,x); //������ʾ�㣻
		for(i=0;i<6;i++) //д��һ���ַ���
		{
			OLED_WriteByte(FONT_6x8[ascii_num][i]);	
		}
		y += 6; //һ��6���ֽڣ�
		count++;		
	}	
}

#endif

#if PICTURE_MODE  //�������˸ú꣬���ʾͼƬ��ʾģʽ������
	//128*64 ͼƬģʽ��ʾ������
	/*
		( x0 , y0 ) ��ʼ���꣬x0: ��ʼ��(1~8)   y0: ��ʼ��(0~128);
		( x1 , y1 ) �յ����꣬x1: �յ���(1~8)   y1: �յ���(0~128);
		���и��ݹ淶ʹ�ã������淶ʹ��ʱû���ṩ�������ܣ�
	*/
	void OLED_Show_PICTURE_128x64(unsigned char x0 , unsigned char y0,
								  unsigned char x1 , unsigned char y1,unsigned int Picture_Index)
	{
		unsigned char x,y; //��¼ͼƬ��ǰ������ꣻ
		unsigned int count = Picture_Index; //��¼�ڼ���ͼƬ����ʼ�±�,(ͼƬ�ɺܶ��ֽ����)��
		{
			x0--;
			y0--;
			x1--;
			y1--;
		}
		for(x=x0;x<=x1;x++) //�У�
		{
			/* �а����У� */
			OLED_SetPosition(y0,x); //x�е���ʼλ�ã�
			for(y=y0;y<=y1;y++) //�У�
			{
				OLED_WriteByte(PICTURE_128x64[count++]);	
			}
		}	
	}
////////////////////////////////////////////////////////////////////////////////////////////
#endif



