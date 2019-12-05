#include "test.h"
#include "bsp_esp8266.h"
#include "bsp_SysTick.h"
#include <stdio.h>  
#include <string.h>  
#include <stdbool.h>
#include "zhuodu.h"
#include "liuliang.h"


volatile uint8_t ucTcpClosedFlag = 0;



/**
  * @brief  ESP8266 （Sta Tcp Client）透传
  * @param  无
  * @retval 无
  */
void ESP8266_StaTcpClient_UnvarnishTest ( void )
{
		uint8_t ucStatus;

		float zhuodu;
		char zhuodu_str[10];
		char *zhuodu_ptr=zhuodu_str;//*zhuo_ptr用于表示浊度值
	
		char liuliang[14];
		char *liuliang_ptr=liuliang;//*liuliang_ptr用于表示水流速和水流总量
	
		static char mac[18];
		static char *mac_ptr=mac;//*mac_ptr用于表示每个仪器的MAC地址，用作区分仪器的唯一编号
	
		char ptr[100]={0};//存储待发送的字符串
		static uint16_t count=0;
		char c_count;//发送数据的序号
		int i=0;
	
		macESP8266_CH_ENABLE();//拉高ESP8266wifi模块引脚
		ESP8266_AT_Test ();//1s AT测试
		ESP8266_Net_Mode_Choose ( STA );//选择STA模式
		while ( ! ESP8266_JoinAP ( macUser_ESP8266_ApSsid, macUser_ESP8266_ApPwd ) );	//4s 连接路由器
		mac_ptr=ESP8266_MACGET();//1s 获取mac地址
		ESP8266_Enable_MultipleId ( DISABLE );//0.5s 单路连接
		while ( !	ESP8266_Link_Server ( enumTCP, macUser_ESP8266_TcpServer_IP, macUser_ESP8266_TcpServer_Port, Single_ID_0 ) );//2s 连接服务器
		while ( ! ESP8266_UnvarnishSend () );//0.5s 进入透传模式

		printf ( "\r\n配置 ESP8266 完毕\r\n" );
	
		while ( 1 )
		{		
			zhuodu=zhuodu_cmd();//获取浊度值
			//浊度类型转换函数
			sprintf(zhuodu_ptr,"%d.%02d",(unsigned int)zhuodu,(unsigned int)((zhuodu-(unsigned int)zhuodu)*100));
		
			liuliang_ptr =liuliang_cmd();//1s  获取每秒的流速和水流总量	
		
			count++;
			if(count==10)
			{
				count=1;
			}
			c_count='0'+count;
				
			printf("%d %s %s %s\n",count,mac_ptr,zhuodu_ptr,liuliang_ptr);//发到串口助手
				
			//合并成一个字符串	
		
			ptr[0]=c_count;

			ptr[1]=' ';
		
			while(*mac_ptr!='\0')
			{
				ptr[i++]=*mac_ptr++;
			}
			ptr[i++]=' ';
		
			while(*zhuodu_ptr!='\0')
			{
				ptr[i++]=*zhuodu_ptr++;	
			}
				
			ptr[i++]=' ';
				
			while(*liuliang_ptr!='\0')	
			{		
				ptr[i++]=*liuliang_ptr++;		
			}
			ptr[i]='\n';
			i=2;
			//printf("answer=%s\n",ptr);
		
			ESP8266_SendString ( ENABLE, ptr, 0, Single_ID_0 );               //1s 发送信息到服务器
		
			Delay_ms ( 1000 );                                                 //1s
		
			if ( ucTcpClosedFlag )                                             //检测是否失去连接
			{
				ESP8266_ExitUnvarnishSend ();                                    //1s 退出透传模式
			
				do ucStatus = ESP8266_Get_LinkStatus ();                         //0.5s 获取连接状态
				while ( ! ucStatus );
				
				if ( ucStatus == 4 )                                             //确认失去连接后重连
				{
					printf ( "\r\n正在重连热点和服务器 ......\r\n" );
					
					while ( ! ESP8266_JoinAP ( macUser_ESP8266_ApSsid, macUser_ESP8266_ApPwd ) );//4s
				
					mac_ptr=ESP8266_MACGET();//1s
				
					while ( !	ESP8266_Link_Server ( enumTCP, macUser_ESP8266_TcpServer_IP, macUser_ESP8266_TcpServer_Port, Single_ID_0 ) );//2s
				
					printf ( "\r\n重连热点和服务器成功\r\n" );

				}	
				while ( ! ESP8266_UnvarnishSend () );		//0.5s
			}

	}
	
		
}



	

		
	

		
//		while(1)
//		{
//				printf("开始死循环\n");
//			
//				zhuodu=zhuodu_cmd();
//				//浊度类型转换函数
//				sprintf(zhuodu_ptr,"%d.%02d",(unsigned int)zhuodu,(unsigned int)((zhuodu-(unsigned int)zhuodu)*100));
//				//printf("转换成功,%s\r\n",zhuodu_ptr);
//		
//				liuliang_ptr=liuliang_cmd();
//				//printf("liuliang is %s\n",liuliang_ptr);
//		
				
		
		
	
