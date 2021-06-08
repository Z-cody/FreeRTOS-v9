#ifndef __DELAY_H
#define __DELAY_H
#include "stm32f4xx.h"
#include "sys.h"

void Delay_Init(void);

void delay_us(u32 nus);
void delay_mns(u32 mns);
void delay_ms(u32 nms);
void delay_s(u32 ns);

//如果使用到FreeRTOS系统
#if SYSTEM_SUPPORT_OS
void F_delay_us(u32 us);
void F_delay_ms(u32 ms);
void F_delay_s(u32 s);
#endif

#endif

