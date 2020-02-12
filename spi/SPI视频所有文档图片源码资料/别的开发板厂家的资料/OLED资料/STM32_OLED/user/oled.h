#ifndef __OLED_H
#define __OLED_H

/* includes */
#include "stm32f10x.h"
#include "stm32f10x_conf.h"

/* defines */
#define     RCC_APB2Periph_OLED_PORT        RCC_APB2Periph_GPIOA

#define     OLED_PORT                       GPIOA

#define     OLED_RST_PIN                    GPIO_Pin_9

#define     OLED_RST_L                      GPIO_ResetBits(GPIOA, GPIO_Pin_9)
#define     OLED_RST_H                      GPIO_SetBits(GPIOA, GPIO_Pin_9)

#define     OLED_DC_PIN                     GPIO_Pin_8

#define	    OLED_DC_L                       GPIO_ResetBits(GPIOA, GPIO_Pin_8)
#define     OLED_DC_H                       GPIO_SetBits(GPIOA, GPIO_Pin_8)

#define  	OLED_SCK_PIN					GPIO_Pin_5
#define		OLED_SDA_PIN					GPIO_Pin_7


void LCD_DLY_ms(unsigned int ms);

/* OLED_WriteByte */
void OLED_WB(uint8_t data);
void LCD_Init(void);
void LCD_CLS(void);
void LCD_Fill(unsigned char dat);


#endif
