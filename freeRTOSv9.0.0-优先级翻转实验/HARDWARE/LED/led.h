#ifndef __LED_H
#define __LED_H
#include "stm32f4xx.h"
#include "sys.h"

//四个LED的开关：1，关; 0，关
#define LED0	PFout(9)  	//开灯
#define LED1	PFout(10)  	//开灯
#define LED2	PEout(13)  	//开灯
#define LED3	PEout(14)  	//开灯


void Led_Init(void);

#endif
