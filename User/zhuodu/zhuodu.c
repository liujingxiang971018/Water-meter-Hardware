#include "zhuodu.h"
#include "stm32f10x.h"
#include <stdio.h>  
#include <string.h> 
#include "bsp_SysTick.h"
//#include "stdlib.h"//用于float->char*
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"



float AD_value;

vu16 ADC_ConvertedValue;
ErrorStatus HSEStartUpStatus;

void gpio_config()
{
	GPIO_InitTypeDef GPIO_initstructure;
	//允许ADC1和GPIOC时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_ADC1,ENABLE);
	//允许DMA时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	//配置PC4为模拟输入
	GPIO_initstructure.GPIO_Mode=GPIO_Mode_AIN;
	GPIO_initstructure.GPIO_Pin=GPIO_Pin_4;
	GPIO_Init(GPIOC,&GPIO_initstructure);
}

void adc_config()
{
	DMA_InitTypeDef dma_initstructure;
	ADC_InitTypeDef adc_initstructure;
	//配置DMA通道1
	DMA_DeInit(DMA1_Channel1);
	dma_initstructure.DMA_PeripheralBaseAddr=ADC1_DR_Address;
	dma_initstructure.DMA_MemoryBaseAddr=(u32)&ADC_ConvertedValue;
	dma_initstructure.DMA_DIR=DMA_DIR_PeripheralSRC;
	dma_initstructure.DMA_BufferSize=1;
	dma_initstructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;
	dma_initstructure.DMA_MemoryInc=DMA_MemoryInc_Disable;
	dma_initstructure.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord;
	dma_initstructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;
	dma_initstructure.DMA_Mode=DMA_Mode_Circular;
	dma_initstructure.DMA_M2M=DMA_M2M_Disable;
	dma_initstructure.DMA_Priority=DMA_Priority_High;
	//允许DMA通道1
	DMA_Init(DMA1_Channel1,&dma_initstructure);
	DMA_Cmd(DMA1_Channel1,ENABLE);
	
	//配置ADC1，连续转换模式，右对齐，非外部触发
	adc_initstructure.ADC_Mode=ADC_Mode_Independent;
	adc_initstructure.ADC_ScanConvMode=ENABLE;
	adc_initstructure.ADC_ContinuousConvMode=ENABLE;
	adc_initstructure.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;
	adc_initstructure.ADC_DataAlign=ADC_DataAlign_Right;
	adc_initstructure.ADC_NbrOfChannel=1;
	ADC_Init(ADC1,&adc_initstructure);
	//adc1转换通道14为规则转换通道
	ADC_RegularChannelConfig(ADC1,ADC_Channel_14,1,ADC_SampleTime_55Cycles5);
	//允许ADC1进行DMA传送
	ADC_DMACmd(ADC1,ENABLE);
	//允许ADC1
	ADC_Cmd(ADC1,ENABLE);
	//允许ADC1复位校准寄存器
	ADC_ResetCalibration(ADC1);
	//检测ADC1校准寄存器是否复位完成
	while(ADC_GetResetCalibrationStatus(ADC1));
	//启动ADC1校准
	ADC_StartCalibration(ADC1);
	//检测ADC1校准是否完成
	while(ADC_GetCalibrationStatus(ADC1));
	//软件触发启动ADC1转换
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);
}
void zhuodu_init(void)
{
	gpio_config();
	adc_config();
}
float zhuodu_cmd(void)
{
	ADC_ConvertedValue=ADC_GetConversionValue(ADC1);
	AD_value=ADC_ConvertedValue/(4096)*5.0;//得到电压值
	
	if(AD_value>=4)
	{
		AD_value=192.5*AD_value*AD_value-2031.05*AD_value+5342.0;
	}
	else
	{
		AD_value=129.62*AD_value*AD_value-1198.97*AD_value+3064.97;
	}
	return AD_value;
}

