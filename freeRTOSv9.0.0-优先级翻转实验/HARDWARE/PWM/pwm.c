#include "pwm.h"

/***********************************
����˵����

LED0 -- PF9 --TIM14_CH1(CH1:ͨ��1)
TIM14 -- APB1 (84MHZ)


***********************************/



void Pwm_PF9_Init(void)
{
	//GPIO����
	GPIO_InitTypeDef 			GPIO_InitStruct;
	TIM_TimeBaseInitTypeDef  	TIM_TimeBaseInitStruct;
	TIM_OCInitTypeDef  			TIM_OCInitStruct;
	
	//ʹ�ܶ�ʱ��14ʱ�ӣ�
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);
	//ʹ��GPIOFʱ�ӣ�
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	
	
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_9;				//����9
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;             	//���ù���
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;				//�������
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;			//����
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;				//����������
	//��ʼ��IO��Ϊ���ù��������������
	GPIO_Init(GPIOF, &GPIO_InitStruct);

	//GPIOF9����ӳ�䵽��ʱ��14
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource9,GPIO_AF_TIM14);
	
	
	TIM_TimeBaseInitStruct.TIM_Prescaler	= 84-1;   			//84��Ƶ  84MHZ/84 = 1MHZ
	TIM_TimeBaseInitStruct.TIM_Period		= 1000-1; 			//��1MHZ�£���1000��������ʱ1ms
	TIM_TimeBaseInitStruct.TIM_CounterMode	= TIM_CounterMode_Up; //���ϼ���
	TIM_TimeBaseInitStruct.TIM_ClockDivision= TIM_CKD_DIV1;       //��Ƶ����
	//��2����ʼ����ʱ��������ARR,PSC��
	TIM_TimeBaseInit(TIM14,&TIM_TimeBaseInitStruct);		

	
	TIM_OCInitStruct.TIM_OCMode		= TIM_OCMode_PWM1;  	//PWM1ģ#ʽ
	TIM_OCInitStruct.TIM_OCPolarity	= TIM_OCPolarity_Low; 	//�͵�ƽ
	TIM_OCInitStruct.TIM_OutputState= TIM_OutputState_Enable; //ͨ��ʹ��	
	//��ʼ������Ƚϲ���:OC1 -->���ͨ��1
	TIM_OC1Init(TIM14, &TIM_OCInitStruct);
	
	
	
	//ʹ��Ԥװ�ؼĴ�����  OC1 -->���ͨ��1
	TIM_OC1PreloadConfig(TIM14, TIM_OCPreload_Enable); 
	//��7��ʹ���Զ���װ�ص�Ԥװ�ؼĴ�������λ	
	TIM_ARRPreloadConfig(TIM14,ENABLE);
	
	
	//��8��ʹ�ܶ�ʱ����
	TIM_Cmd(TIM14, ENABLE);
}
