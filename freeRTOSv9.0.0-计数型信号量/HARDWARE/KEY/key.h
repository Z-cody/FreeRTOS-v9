#ifndef ALLKEY_H
#define ALLKEY_H
#include "stm32f4xx.h"
#include "sys.h"

//��ȥ�ĸ�������״̬��1��û�����£�0��������
#define KEY0 PAin(0)
#define KEY1 PEin(2)
#define KEY2 PEin(3)
#define KEY3 PEin(4)

//�ĸ����������µı�־
#define KEY0_PRESD 1
#define KEY1_PRESD 2
#define KEY2_PRESD 3
#define KEY3_PRESD 4

void AllKeyInit(void);
u8 KEY_Scan(u8 mode);

#endif
