/*
	����ʹ��˵����
	1.�������֧��oledģ����SPI�µĿ��Ƽ�4��ͨ�ţ�����֧��������
����ģʽ��oled�ĳ��ÿ���������ļ�����ͼƬ
	2.���޸����������ʵ������޸���Ӧ��·�����Ŷ��壬Ĭ�������:
		ʱ���ߣ�PA0  �����ߣ�PA1  ����/����ѡ���ߣ�PA2  ��λ�ߣ�PA3
		ʱ�ӺͶ˿ڵ��������޸������޸ģ�Ĭ���¶�����Ϊ���������
	3.�ӿ�˵������ʹ��OLEDǰ�����ʼ��OLED,����Ϊ��OLED_Init();
��ʱ����õ����������ṩ���½ӿڣ�
			Clear_Color; //����Ļ��Ϊ��ɫ�� 					
			Clear_Black; //����Ļ��Ϊȫ����
��ʾ����ʱ�����ú����� OLED_Show_FONT_16x16(unsigned char X,unsigned char Y,unsigned char Num);
�ú�������˵����X��ʾ�ڼ���(OLED��16*16������ʾ�¿�����ʾ4�У�8��)��YΪ�ڼ��У�NumΪ�ֿ��еڼ�
�����֣��������������±궼��1��ʼ������Ӧ����Ҫ������鿴�ֿ⣬��"oled.c"�д򿪰�����ͷ�ļ���
"oled_word_library.h" ���ֿ��н���ȡ��ģ����õ���Ӧ�ĺ��֣��������Ӧģʽ�Ŀ���ֱ��ʹ�ã�

���º����ӿڵ�ʹ�ö���Ҫ��"oled_word_library.h"�д���Ӧ�ĺ궨�����ʹ�ã�
	a. ��ʾ8*16��С�ַ��Ľӿڣ�
			OLED_Show_FONT_8x16(unsigned char x , unsigned char y , unsigned char Data[]);
�˽ӿڵı���˵����x: ��(1~4)   y: ��(1~16)   Data[]ΪҪ��ʾ���ַ����������ʾΪ4��16��;
	b. ��ʾ6*8��С�ַ��ӿڣ�
			void OLED_Show_FONT_6x8(unsigned char x , unsigned char y , unsigned char Data[]);
�˽ӿڵı�������һ��������һ����������Χ�ɴﵽ��8��21�У�
	c. ��ʾͼƬ�ӿڣ�
				void OLED_Show_PICTURE_128x64(unsigned char x0 , unsigned char y0,
								              unsigned char x1 , unsigned char y1,unsigned int Picture_Index);
�˺�������˵����
		( x0 , y0 ) ��ʼ���꣬x0: ��ʼ��(1~8)   y0: ��ʼ��(0~128);
		( x1 , y1 ) �յ����꣬x1: �յ���(1~8)   y1: �յ���(0~128);
		���и��ݹ淶ʹ�ã������淶ʹ��ʱû���ṩ�������ܣ�

*/
#ifndef __OLED_H__
#define __OLED_H__

/****************************************************************************/
/////////////////////////////////////////////////////////////////////////////
////		                        �޸���                 			  ///////
/////////////////////////////////////////////////////////////////////////////
/*------------OLED���Ŷ���------------*/
		//OLEDʱ���ߣ�
		#define OLED_CLK PAout(0)
		
		//OLED�����ߣ�
		#define OLED_DAT PAout(1)
		
		//OLED����/����ѡ���ߣ�
		#define OLED_DC	 PAout(2)
		/////
		//���ݣ�����ѡ��
		#define OLED_COMMAND 0
		#define OLED_DATA    1
		
		//OLED��λ�ߣ�
		#define OLED_RST PAout(3)


//������Ӧ�˿�ʱ�Ӳ����ö˿�
#define OLED_SET_GPIO { RCC->APB2ENR|=1<<2;GPIOA->CRL&=0xffff0000;GPIOA->CRL|=0xffff3333; GPIOA->ODR|=0x0c0; }///
					   //ʹ��ʱ�ӣ�         Ԥ��0					���������           DC.RST��1��DAT.CMD��0��

/* ���Ҫ�޸ĵĵط��϶࣬���Բ������¶��壬��ͬ����'\'�����ӣ�����궨�������
#define OLED_SET_GPIO { \
						 RCC->APB2ENR|=1<<2; \
						 GPIOA->CRL&=0xffff0000; \
						 GPIOA->CRL|=0xffff3333; \
						 GPIOA->ODR|=0x0c0; \
 					  }
*/

/*-------------------------------------------���ϸ���ʵ�����и���----------------------------------------------------*/


//���ⶨ��,���������ģ�
#define OLED_PAGE 8 //oled�Դ�Ϊ8ҳ��ÿҳ128���ֽڣ�
#define OLED_X 128	//oled����
#define OLED_Y 64	//oled��
#define Clear_Color OLED_Screen(0xff)	//��������ɫ��
#define Clear_Black OLED_Screen(0x00)	//��������ɫ��



//OLED��ʼ��������
void OLED_Init(void);

//д���������
void OLED_WriteCommand(unsigned char Cmd);

//д�����ݺ�����
void OLED_WriteByte(unsigned char Data);

//ȫ������������
void OLED_Screen(unsigned char Val);

//�������ú�����
void OLED_SetPosition(unsigned char x , unsigned char y);




//16*16 ����ģʽ��ʾ������
void OLED_Show_FONT_16x16(unsigned char x , unsigned char y , unsigned char Num);

#if FONT_8x16_MODE //�������˸ú꣬���ʾ8*16 ASCII���׼����ģʽ������
	//8*16 ����ģʽ��ʾ������
	void OLED_Show_FONT_8x16(unsigned char x , unsigned char y , unsigned char Data[]);
////////////////////////////////////////////////////////////////////////////////////////
#endif


#if FONT_6x8_MODE //�������˸ú꣬���ʾ6*8 ASCII���׼����ģʽ������
	//6*8 ����ģʽ��ʾ������
	 void OLED_Show_FONT_6x8(unsigned char x , unsigned char y , unsigned char Data[]);
////////////////////////////////////////////////////////////////////////////////////////
#endif


#if PICTURE_MODE  //�������˸ú꣬���ʾͼƬ��ʾģʽ������
	//���128*64 ͼƬģʽ��ʾ������
	void OLED_Show_PICTURE_128x64(unsigned char x0 , unsigned char y0,
								  unsigned char x1 , unsigned char y1,unsigned int Picture_Index);
//////////////////////////////////////////////////////////////////////////////////////////////////
#endif



#endif










