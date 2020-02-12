#include "config.h"

void GPIO_Configuration(void)
{

    /* Initialize Leds mounted on STM32 board */
    GPIO_InitTypeDef  GPIO_InitStructure;
    /* Initialize LED which connected to PA1,2,3,4, Enable the Clock*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    /* Configure the GPIO_LED pin */


    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_4 | GPIO_Pin_3|GPIO_Pin_2|GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

}

void USART_Configuration(void)
{


	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	/* Enable USART1 and GPIOA clock                                        */
	RCC_APB2PeriphClockCmd (RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

	/* Configure USART1 Rx (PA10) as input floating                         */
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure USART1 Tx (PA9) as alternate function push-pull            */
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	 USART_InitStructure.USART_BaudRate            = 115200;
	 USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
	 USART_InitStructure.USART_StopBits            = USART_StopBits_1;
	 USART_InitStructure.USART_Parity              = USART_Parity_No ;
	 USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	 USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
	 USART_Init(USART1, &USART_InitStructure);
	 USART_Cmd(USART1, ENABLE);
}

void SPI_Configuration(void)
{
	  SPI_InitTypeDef  SPI_InitStructure;
	  GPIO_InitTypeDef GPIO_InitStructure;

	  RCC_APB2PeriphClockCmd( RCC_APB2Periph_SPI1 | RCC_APB2Periph_OLED_PORT, ENABLE);

	  /* NSS---->GPIO(LED) */
	  SPI_SSOutputCmd(SPI1, ENABLE);

	  GPIO_InitStructure.GPIO_Pin = OLED_SCK_PIN | OLED_SDA_PIN ;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_Init(OLED_PORT, &GPIO_InitStructure);

	  /* GPIO4 LED3 */
	  GPIO_InitStructure.GPIO_Pin = OLED_RST_PIN | OLED_DC_PIN | GPIO_Pin_4 ;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_Init(OLED_PORT, &GPIO_InitStructure);

	  /* SPI1 Config -------------------------------------------------------------*/
	  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
	  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	  SPI_InitStructure.SPI_CRCPolynomial = 7;
	  SPI_Init(SPI1, &SPI_InitStructure);
	  /* Enable SPI1 */
	  SPI_Cmd(SPI1, ENABLE);
}


