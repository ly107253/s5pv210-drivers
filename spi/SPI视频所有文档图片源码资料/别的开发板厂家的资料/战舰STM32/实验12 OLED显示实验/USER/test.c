#include "sys.h"
#include "usart.h"		
#include "delay.h"	
#include "led.h" 
#include "beep.h"	 	 
#include "key.h"	 	 
#include "exti.h"	 	 
#include "wdg.h" 	 
#include "timer.h"		 	 
#include "tpad.h"
#include "oled.h"		 	 
//ALIENTEK战舰STM32开发板实验12
//OLED显示实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司 
int main(void)
{
 	u8 t;
  	Stm32_Clock_Init(9);	//系统时钟设置
	uart_init(72,9600);	 	//串口初始化为9600
	delay_init(72);	   	 	//延时初始化 
	LED_Init();		  		//初始化与LED连接的硬件接口

 	OLED_Init();			//初始化液晶      
 	OLED_ShowString(0,0, "0.96' OLED TEST");  
 	OLED_ShowString(0,16,"ATOM@ALIENTEK");  
 	OLED_ShowString(0,32,"2012/9/5");  
 	OLED_ShowString(0,48,"ASCII:");  
 	OLED_ShowString(63,48,"CODE:");  
	OLED_Refresh_Gram();	 
	t=' ';
	while(1) 
	{		
		OLED_ShowChar(48,48,t,16,1);//显示ASCII字符	   
		OLED_Refresh_Gram();
		t++;
		if(t>'~')t=' ';
		OLED_ShowNum(103,48,t,3,16);//显示ASCII字符的码值 
		delay_ms(300);
		LED0=!LED0;			 
	}	  
}










