#include "liuliang.h"
#include "stm32f10x.h"
#include "stdio.h"
#include "string.h"
#include "bsp_usart1.h"
#include "bsp_SysTick.h"
#include "stm32f10x_iwdg.h"
#include "stm32f10x_dbgmcu.h"
#include "stm32f10x_exti.h"

static char SendBufLen=0;              //数据发送缓冲区长度
static char SendLen=0;                 //要发送的数据长度
volatile char SendDataOk =1;           //串口发送完成标志位，发送完成后=1
char SendBuf[30];                      //数据发送缓冲区
char Date[20];                          
int Pulse1,Pulse2;
int Flow,Speed;
char ASC[]={'0','1','2','3','4','5','6','7','8','9'};

/*函数名：StartIwdg 
*参  数：无
*返回值：无
*描  述：启动看门狗 看门狗 LSI = 40KHZ  6.4S
*/
void StartIwdg( void )
{
	DBGMCU_Config( DBGMCU_IWDG_STOP,ENABLE);     //调试时关闭看门狗时钟
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);// Enable write access to IWDG_PR and IWDG_RLR registers 
	IWDG_SetPrescaler( IWDG_Prescaler_256 );     // IWDG counter clock: LSI/32 = 40K/256= 6.4ms
	IWDG_SetReload( 3126 );                      //0-0xfff( 4095 )    6.4s / 6.4ms = 1000
	IWDG_ReloadCounter();  
	IWDG_Enable( );
}

/******************************************************
* 函数名: TIM2_Config
* 参  数：无
* 返回值：无
* 描  述：用于系统心跳计时,每1ms进入一次中断 
*         TIM_Period / Auto Reload Register(ARR) = 1000   TIM_Prescaler--71 
*         中断周期为 = 1/(72MHZ /72) * 1000 = 1ms
*/
void TIM2_Config( void )
{
  	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  	NVIC_InitTypeDef NVIC_InitStructure;
	
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	
	
  	/* Timer Base configuration */
  	TIM_TimeBaseStructure.TIM_Period = 1000;          
  	TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;       
  	TIM_TimeBaseStructure.TIM_ClockDivision = 0x00;    
  	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
  	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);  
  	
  	/* Enable the TIM2 gloabal Interrupt */
  	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 13;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);  	
	
  	/* Enable timer update interrupt */
  	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
  	/* Enable timer counter */
  	TIM_Cmd(TIM2,ENABLE);
}

void EXTIX_Init(void) 
{ 
  	GPIO_InitTypeDef GPIO_InitStructure; 
 	  EXTI_InitTypeDef EXTI_InitStructure; 
 	  NVIC_InitTypeDef NVIC_InitStructure; 

  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);//IO口的时钟配置 

    //初始化 外部中断-->GPIOC.0, 上拉输入 
  	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0; 
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
  	GPIO_Init(GPIOC, &GPIO_InitStructure); 
    /* Enable AFIO clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	
    //的GPIOC.0      中断线以及中断初始化配置 
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource0); //选择相应引脚作为中短线 
   
	  /* Configure EXTI10 line */
  	EXTI_InitStructure.EXTI_Line=EXTI_Line0;	   //配置为外部中断线0 
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	 //配置外部中断线为中断请求 
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;	 //下降沿触发 
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;	 //使能中断 
  	EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器 
   

   /* Enable and set EXTI3 Interrupt to the lowest priority */
  	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;	//使能按键S2所在的外部中断通道 
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//抢占优先级2  
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	//子优先级2  
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	//使能外部中断通道 
  	NVIC_Init(&NVIC_InitStructure);  	  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器 

} 








/***************************************************************************
*函数名：UsartNVIC_Config
*参  数：IRQChannel中断向量号    PreemptionPriority主优先级  SubPriority从优先级
*返回值：无
*描  述：对串口中断优先级进行配置
*/
void UsartNVIC_Config( u8 IRQChannel, u8 PreemptionPriority, u8 SubPriority )
{
    NVIC_InitTypeDef    NVIC_InitStructure;
    /* Enable the USART2 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = IRQChannel;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = PreemptionPriority;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = SubPriority;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init( &NVIC_InitStructure );
}



/*********************************************************
*函数名：USART1_IRQHandler
*参  数：无
*返回值：无
*描  述：中断处理函数
*/
void USART1_IRQHandler( void )
{ 
	
  if(USART_GetITStatus(USART1, USART_IT_TC) == SET)//发送中断
  {   
			
		  //最后一次发完了 清除标志不会在进入
		  USART_ClearFlag(USART1,USART_FLAG_TC);
		 
			if(SendLen==0)//发送完成
      {
				SendDataOk=1;
			} 		
			else
			{	
				USART_SendData(USART1, SendBuf[SendBufLen]);	
        SendBufLen++;
				SendLen--;
			}				

	}	
}

/*********************************************************
*函数名：ComSend
*参  数：Buf待发送的数据包  Len数据长度
*返回值：无
*描  述：把数据复制到待发送缓冲区
*/
//缓冲区一个一个发
void ComSend( u8 *Buf, u8 Len )
{	
	
  if(SendDataOk==1)//上次的发送完成标志
  {	
		SendDataOk=0; 
		//数据拷贝到缓冲区
		memcpy( SendBuf, Buf, Len );
		SendLen=Len; 
    SendBufLen=0;	
		//先发第一个数据引发中断 剩下的数据中断发送
    USART_SendData(USART1, SendBuf[SendBufLen]);	
    SendBufLen++;
		SendLen--;	
	}
}

void liuliang_init(void)
{
		//SysTick_Init();
	  //Delay_ms(100);
	  NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
		//StartIwdg( );
		//TIM2_Config( );
		EXTIX_Init();
		//UsartNVIC_Config( USART1_IRQn, 4, 0);          //接收中断优先级
	
		//USART_ITConfig(USART1, USART_IT_TC, ENABLE);  
}
char *liuliang_cmd()//
{
					 Delay_ms(1000);
				   Speed=Pulse1/6;//speed为水流速
				   Pulse1=0;//小数点
				   Date[0]=ASC[Speed%10000/1000];
				   Date[1]=ASC[Speed%1000/100];
				   Date[2]=0x2e;//小数点
				   Date[3]=ASC[Speed%100/10];
				   Date[4]=ASC[Speed%10];				
				   Date[5]=0x20;//空格

           Flow=Pulse2;//Flow为水流总量
				   Date[6] =ASC[Flow%1000000/100000];
				   Date[7] =ASC[Flow%100000/10000];				
				   Date[8] =ASC[Flow%10000/1000];
				   Date[9]=ASC[Flow%1000/100];
				   Date[10]=0x2e;
				   Date[11]=ASC[Flow%100/10];
				   Date[12]=ASC[Flow%10];		
				   Date[13]='\0';//空格
				  return Date;
}
void EXTI0_IRQHandler(void) 
{ 
    u8 i;
  	if(EXTI_GetITStatus(EXTI_Line0) != RESET)	  //检查指定的EXT0线路触发请求发生与否 
    {	   
       Pulse1++;
		   i++;
		   if(i>6)
			 {
				 i=0;
				 Pulse2++; 
			 }
    } 
    EXTI_ClearITPendingBit(EXTI_Line0);  //清除EXTI0线路挂起位 
} 
