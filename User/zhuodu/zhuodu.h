#ifndef __ZHUODU_H
#define __ZHUODU_H
#define ADC1_DR_Address (u32)0x4001244c

#include "stm32f10x.h"
#include <stdio.h>
#include <string.h>

void gpio_config(void);
void adc_config(void);
void zhuodu_init(void);
float zhuodu_cmd(void);
#endif
