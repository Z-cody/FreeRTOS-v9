#ifndef __LED_H
#define __LED_H
#include "stm32f4xx.h"
#include "sys.h"

//�ĸ�LED�Ŀ��أ�1����; 0����
#define LED0	PFout(9)  	//����
#define LED1	PFout(10)  	//����
#define LED2	PEout(13)  	//����
#define LED3	PEout(14)  	//����


void Led_Init(void);

#endif
