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
  * @brief  ESP8266 ��Sta Tcp Client��͸��
  * @param  ��
  * @retval ��
  */
void ESP8266_StaTcpClient_UnvarnishTest ( void )
{
		uint8_t ucStatus;

		float zhuodu;
		char zhuodu_str[10];
		char *zhuodu_ptr=zhuodu_str;//*zhuo_ptr���ڱ�ʾ�Ƕ�ֵ
	
		char liuliang[14];
		char *liuliang_ptr=liuliang;//*liuliang_ptr���ڱ�ʾˮ���ٺ�ˮ������
	
		static char mac[18];
		static char *mac_ptr=mac;//*mac_ptr���ڱ�ʾÿ��������MAC��ַ����������������Ψһ���
	
		char ptr[100]={0};//�洢�����͵��ַ���
		static uint16_t count=0;
		char c_count;//�������ݵ����
		int i=0;
	
		macESP8266_CH_ENABLE();//����ESP8266wifiģ������
		ESP8266_AT_Test ();//1s AT����
		ESP8266_Net_Mode_Choose ( STA );//ѡ��STAģʽ
		while ( ! ESP8266_JoinAP ( macUser_ESP8266_ApSsid, macUser_ESP8266_ApPwd ) );	//4s ����·����
		mac_ptr=ESP8266_MACGET();//1s ��ȡmac��ַ
		ESP8266_Enable_MultipleId ( DISABLE );//0.5s ��·����
		while ( !	ESP8266_Link_Server ( enumTCP, macUser_ESP8266_TcpServer_IP, macUser_ESP8266_TcpServer_Port, Single_ID_0 ) );//2s ���ӷ�����
		while ( ! ESP8266_UnvarnishSend () );//0.5s ����͸��ģʽ

		printf ( "\r\n���� ESP8266 ���\r\n" );
	
		while ( 1 )
		{		
			zhuodu=zhuodu_cmd();//��ȡ�Ƕ�ֵ
			//�Ƕ�����ת������
			sprintf(zhuodu_ptr,"%d.%02d",(unsigned int)zhuodu,(unsigned int)((zhuodu-(unsigned int)zhuodu)*100));
		
			liuliang_ptr =liuliang_cmd();//1s  ��ȡÿ������ٺ�ˮ������	
		
			count++;
			if(count==10)
			{
				count=1;
			}
			c_count='0'+count;
				
			printf("%d %s %s %s\n",count,mac_ptr,zhuodu_ptr,liuliang_ptr);//������������
				
			//�ϲ���һ���ַ���	
		
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
		
			ESP8266_SendString ( ENABLE, ptr, 0, Single_ID_0 );               //1s ������Ϣ��������
		
			Delay_ms ( 1000 );                                                 //1s
		
			if ( ucTcpClosedFlag )                                             //����Ƿ�ʧȥ����
			{
				ESP8266_ExitUnvarnishSend ();                                    //1s �˳�͸��ģʽ
			
				do ucStatus = ESP8266_Get_LinkStatus ();                         //0.5s ��ȡ����״̬
				while ( ! ucStatus );
				
				if ( ucStatus == 4 )                                             //ȷ��ʧȥ���Ӻ�����
				{
					printf ( "\r\n���������ȵ�ͷ����� ......\r\n" );
					
					while ( ! ESP8266_JoinAP ( macUser_ESP8266_ApSsid, macUser_ESP8266_ApPwd ) );//4s
				
					mac_ptr=ESP8266_MACGET();//1s
				
					while ( !	ESP8266_Link_Server ( enumTCP, macUser_ESP8266_TcpServer_IP, macUser_ESP8266_TcpServer_Port, Single_ID_0 ) );//2s
				
					printf ( "\r\n�����ȵ�ͷ������ɹ�\r\n" );

				}	
				while ( ! ESP8266_UnvarnishSend () );		//0.5s
			}

	}
	
		
}



	

		
	

		
//		while(1)
//		{
//				printf("��ʼ��ѭ��\n");
//			
//				zhuodu=zhuodu_cmd();
//				//�Ƕ�����ת������
//				sprintf(zhuodu_ptr,"%d.%02d",(unsigned int)zhuodu,(unsigned int)((zhuodu-(unsigned int)zhuodu)*100));
//				//printf("ת���ɹ�,%s\r\n",zhuodu_ptr);
//		
//				liuliang_ptr=liuliang_cmd();
//				//printf("liuliang is %s\n",liuliang_ptr);
//		
				
		
		
	
