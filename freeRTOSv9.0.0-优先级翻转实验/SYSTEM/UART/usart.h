#ifndef __USART_H
#define __USART_H
#include "stm32f4xx.h"

#define USART3_MAX_RECV_LEN		800					//�����ջ����ֽ���
#define USART3_MAX_SEND_LEN		200					//����ͻ����ֽ���
#define USART3_RX_EN 			1								//0,������;1,����.

void Usart3_Init(int BaudRate);
void Usart2_Init(int BaudRate);
void Usart1_Init(int BaudRate);


#endif

