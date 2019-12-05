#include "stm32f10x.h"
#include "bsp_usart1.h"
#include "bsp_SysTick.h"
#include "bsp_esp8266.h"
#include "test.h"
#include "zhuodu.h"
#include "liuliang.h"

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
	
	

int main ( void )
{
		/* 初始化 */ 
		USARTx_Config ();                                                              //初始化串口1
		SysTick_Init ();                                                               //配置 SysTick 为 1ms 中断一次 
		ESP8266_Init ();                                                               //初始化WiFi模块使用的接口和外设
		zhuodu_init();
		liuliang_init();  
		
		printf("所有初始化成功\n");
	
		ESP8266_StaTcpClient_UnvarnishTest();
		while(1);
}

/*********************************************END OF FILE**********************/
