#ifndef __USART_H
#define __USART_H
#include "stm32f4xx.h"

#define USART3_MAX_RECV_LEN		800					//最大接收缓存字节数
#define USART3_MAX_SEND_LEN		200					//最大发送缓存字节数
#define USART3_RX_EN 			1								//0,不接收;1,接收.

void Usart3_Init(int BaudRate);
void Usart2_Init(int BaudRate);
void Usart1_Init(int BaudRate);


#endif

