#include "config.h"
#include "oled.h"
#include <stdio.h>

void USART_Print(void);

int main(void)
{

    //automatically added by CoIDE
//	GPIO_Configuration();
//	GPIO_SetBits(GPIOA,GPIO_Pin_1);

	//automatically added by CoIDE
//	USART_Print();
//	GPIO_ResetBits(GPIOA,GPIO_Pin_1);

	SPI_Configuration();

	GPIO_SetBits(GPIOA,GPIO_Pin_4);
	LCD_DLY_ms(500);
	GPIO_ResetBits(GPIOA,GPIO_Pin_4);
	LCD_DLY_ms(500);
	LCD_Init();

	LCD_Fill(0xff);

	while(1)
		{
			LCD_Fill(0x00);
			GPIO_SetBits(GPIOA,GPIO_Pin_4);
			LCD_DLY_ms(500);
			LCD_Fill(0xaa);
			GPIO_ResetBits(GPIOA,GPIO_Pin_4);
			LCD_DLY_ms(500);
		}
}
void USART_Print(void)
{
    USART_Configuration();

    /* Output a message on Hyperterminal using printf function */
    printf("Hello, World!\n\r");
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif
