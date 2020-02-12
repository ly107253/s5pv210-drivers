#ifndef __CONFIG_H
#define __CONFIG_H

/* includes */
#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "oled.h"



/* config for leds */
void GPIO_Configuration(void);
/* config for usart1 */
void USART_Configuration(void);
/* config for oled */
void SPI_Configuration(void);




#endif
