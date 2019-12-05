#include "stm32f10x.h"
#include "bsp_usart1.h"
#include "bsp_SysTick.h"
#include "bsp_esp8266.h"
#include "test.h"
#include "zhuodu.h"
#include "liuliang.h"

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
	
	

int main ( void )
{
		/* ��ʼ�� */ 
		USARTx_Config ();                                                              //��ʼ������1
		SysTick_Init ();                                                               //���� SysTick Ϊ 1ms �ж�һ�� 
		ESP8266_Init ();                                                               //��ʼ��WiFiģ��ʹ�õĽӿں�����
		zhuodu_init();
		liuliang_init();  
		
		printf("���г�ʼ���ɹ�\n");
	
		ESP8266_StaTcpClient_UnvarnishTest();
		while(1);
}

/*********************************************END OF FILE**********************/
