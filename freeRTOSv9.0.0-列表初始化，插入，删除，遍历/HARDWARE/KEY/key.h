#ifndef ALLKEY_H
#define ALLKEY_H
#include "stm32f4xx.h"
#include "sys.h"

//过去四个按键的状态：1，没被按下；0，被按下
#define KEY0 PAin(0)
#define KEY1 PEin(2)
#define KEY2 PEin(3)
#define KEY3 PEin(4)

//四个按键被按下的标志
#define KEY0_PRESD 1
#define KEY1_PRESD 2
#define KEY2_PRESD 3
#define KEY3_PRESD 4

void AllKeyInit(void);
u8 KEY_Scan(u8 mode);

#endif
