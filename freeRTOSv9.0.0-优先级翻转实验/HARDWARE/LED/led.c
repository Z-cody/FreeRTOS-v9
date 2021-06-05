#include "led.h"

/*********************
���Ƶ�����

����͵�ƽ��0��������
����ߵ�ƽ��1��������

PF9 --led0
PF10--led1
PE13--led2
PE14--led3

*********************/


void Led_Init(void)
{
	//GPIO����
	GPIO_InitTypeDef GPIO_InitStruct;
	
	//ʹ�ܣ��򿪣�GPIO E��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	//ʹ�ܣ��򿪣�GPIO F��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	
	
	//����PF9�� PF10������
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_9|GPIO_Pin_10;    //����9������10
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;             //���ģʽ
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;				//�������
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;			//����
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;				//����������
	//��ʼ��PF������
	GPIO_Init(GPIOF,&GPIO_InitStruct);
	
	
	
	//����PE������
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_13|GPIO_Pin_14;	//����13������14
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;				//���ģʽ
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;				//�������
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;			//���� 
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;				//����������
	//��ʼ��PE������
	GPIO_Init(GPIOE,&GPIO_InitStruct);
	
	//�ر����еĵ�
	GPIO_SetBits(GPIOF,GPIO_Pin_9);
	GPIO_SetBits(GPIOF,GPIO_Pin_10);
	GPIO_SetBits(GPIOE,GPIO_Pin_13);
	GPIO_SetBits(GPIOE,GPIO_Pin_14);
	

}
