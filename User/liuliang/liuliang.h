#ifndef _LIULIANG_H
#define _LIULIANG_H
#include "stm32f10x.h"
#define TRUE   0xFF 
#define FALSE  0x00

 
#define Delay_01           1000    //每隔1s计算下流速时间 *1ms
#define Delay_02           5000    //每隔5s串口上传一次数据
#define Delay_03           1000  
 

void liuliang_init(void);
char * liuliang_cmd(void);
#endif
