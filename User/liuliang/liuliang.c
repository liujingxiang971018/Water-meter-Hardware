#include "liuliang.h"
#include "stm32f10x.h"
#include "stdio.h"
#include "string.h"
#include "bsp_usart1.h"
#include "bsp_SysTick.h"
#include "stm32f10x_iwdg.h"
#include "stm32f10x_dbgmcu.h"
#include "stm32f10x_exti.h"

static char SendBufLen=0;              //���ݷ��ͻ���������
static char SendLen=0;                 //Ҫ���͵����ݳ���
volatile char SendDataOk =1;           //���ڷ�����ɱ�־λ��������ɺ�=1
char SendBuf[30];                      //���ݷ��ͻ�����
char Date[20];                          
int Pulse1,Pulse2;
int Flow,Speed;
char ASC[]={'0','1','2','3','4','5','6','7','8','9'};

/*��������StartIwdg 
*��  ������
*����ֵ����
*��  �����������Ź� ���Ź� LSI = 40KHZ  6.4S
*/
void StartIwdg( void )
{
	DBGMCU_Config( DBGMCU_IWDG_STOP,ENABLE);     //����ʱ�رտ��Ź�ʱ��
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);// Enable write access to IWDG_PR and IWDG_RLR registers 
	IWDG_SetPrescaler( IWDG_Prescaler_256 );     // IWDG counter clock: LSI/32 = 40K/256= 6.4ms
	IWDG_SetReload( 3126 );                      //0-0xfff( 4095 )    6.4s / 6.4ms = 1000
	IWDG_ReloadCounter();  
	IWDG_Enable( );
}

/******************************************************
* ������: TIM2_Config
* ��  ������
* ����ֵ����
* ��  ��������ϵͳ������ʱ,ÿ1ms����һ���ж� 
*         TIM_Period / Auto Reload Register(ARR) = 1000   TIM_Prescaler--71 
*         �ж�����Ϊ = 1/(72MHZ /72) * 1000 = 1ms
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

  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);//IO�ڵ�ʱ������ 

    //��ʼ�� �ⲿ�ж�-->GPIOC.0, �������� 
  	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0; 
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
  	GPIO_Init(GPIOC, &GPIO_InitStructure); 
    /* Enable AFIO clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	
    //��GPIOC.0      �ж����Լ��жϳ�ʼ������ 
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource0); //ѡ����Ӧ������Ϊ�ж��� 
   
	  /* Configure EXTI10 line */
  	EXTI_InitStructure.EXTI_Line=EXTI_Line0;	   //����Ϊ�ⲿ�ж���0 
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	 //�����ⲿ�ж���Ϊ�ж����� 
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;	 //�½��ش��� 
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;	 //ʹ���ж� 
  	EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ��� 
   

   /* Enable and set EXTI3 Interrupt to the lowest priority */
  	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;	//ʹ�ܰ���S2���ڵ��ⲿ�ж�ͨ�� 
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//��ռ���ȼ�2  
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	//�����ȼ�2  
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	//ʹ���ⲿ�ж�ͨ�� 
  	NVIC_Init(&NVIC_InitStructure);  	  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ��� 

} 








/***************************************************************************
*��������UsartNVIC_Config
*��  ����IRQChannel�ж�������    PreemptionPriority�����ȼ�  SubPriority�����ȼ�
*����ֵ����
*��  �����Դ����ж����ȼ���������
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
*��������USART1_IRQHandler
*��  ������
*����ֵ����
*��  �����жϴ�����
*/
void USART1_IRQHandler( void )
{ 
	
  if(USART_GetITStatus(USART1, USART_IT_TC) == SET)//�����ж�
  {   
			
		  //���һ�η����� �����־�����ڽ���
		  USART_ClearFlag(USART1,USART_FLAG_TC);
		 
			if(SendLen==0)//�������
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
*��������ComSend
*��  ����Buf�����͵����ݰ�  Len���ݳ���
*����ֵ����
*��  ���������ݸ��Ƶ������ͻ�����
*/
//������һ��һ����
void ComSend( u8 *Buf, u8 Len )
{	
	
  if(SendDataOk==1)//�ϴεķ�����ɱ�־
  {	
		SendDataOk=0; 
		//���ݿ�����������
		memcpy( SendBuf, Buf, Len );
		SendLen=Len; 
    SendBufLen=0;	
		//�ȷ���һ�����������ж� ʣ�µ������жϷ���
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
		//UsartNVIC_Config( USART1_IRQn, 4, 0);          //�����ж����ȼ�
	
		//USART_ITConfig(USART1, USART_IT_TC, ENABLE);  
}
char *liuliang_cmd()//
{
					 Delay_ms(1000);
				   Speed=Pulse1/6;//speedΪˮ����
				   Pulse1=0;//С����
				   Date[0]=ASC[Speed%10000/1000];
				   Date[1]=ASC[Speed%1000/100];
				   Date[2]=0x2e;//С����
				   Date[3]=ASC[Speed%100/10];
				   Date[4]=ASC[Speed%10];				
				   Date[5]=0x20;//�ո�

           Flow=Pulse2;//FlowΪˮ������
				   Date[6] =ASC[Flow%1000000/100000];
				   Date[7] =ASC[Flow%100000/10000];				
				   Date[8] =ASC[Flow%10000/1000];
				   Date[9]=ASC[Flow%1000/100];
				   Date[10]=0x2e;
				   Date[11]=ASC[Flow%100/10];
				   Date[12]=ASC[Flow%10];		
				   Date[13]='\0';//�ո�
				  return Date;
}
void EXTI0_IRQHandler(void) 
{ 
    u8 i;
  	if(EXTI_GetITStatus(EXTI_Line0) != RESET)	  //���ָ����EXT0��·������������� 
    {	   
       Pulse1++;
		   i++;
		   if(i>6)
			 {
				 i=0;
				 Pulse2++; 
			 }
    } 
    EXTI_ClearITPendingBit(EXTI_Line0);  //���EXTI0��·����λ 
} 
