#include "tim.h"


/****************************************
ģ��˵����

TIM3 -- APB1 -- 84MHZ
16λ��ʱ��

*****************************************/


void Tim3_Init(void)
{
	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseInitStruct;
	NVIC_InitTypeDef  NVIC_InitStruct;
	
	
	
	//��1���ܶ�ʱ��ʱ�ӡ�
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	
//	TIM_TimeBaseInitStruct.TIM_Prescaler	= 84-1;   		//84��Ƶ  84MHZ/84 = 1MHZ
//	TIM_TimeBaseInitStruct.TIM_Period		= 1000-1; 		//��1MHZ�£���1000��������ʱ1ms
//	TIM_TimeBaseInitStruct.TIM_CounterMode	= TIM_CounterMode_Up; //���ϼ���
//	TIM_TimeBaseInitStruct.TIM_ClockDivision= TIM_CKD_DIV1;       //��Ƶ����
//	//��2����ʼ����ʱ��������ARR,PSC��
//	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStruct);
	
	TIM_TimeBaseInitStruct.TIM_Prescaler	= 8400-1;   		//84��Ƶ  84000000/8400 = 10000HZ
	TIM_TimeBaseInitStruct.TIM_Period		= 10000-1; 			//��1MHZ�£���1000��������ʱ1ms
	TIM_TimeBaseInitStruct.TIM_CounterMode	= TIM_CounterMode_Up; //���ϼ���
	TIM_TimeBaseInitStruct.TIM_ClockDivision= TIM_CKD_DIV1;       //��Ƶ����
	//��2����ʼ����ʱ��������ARR,PSC��
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStruct);	
	
	
	NVIC_InitStruct.NVIC_IRQChannel						= TIM3_IRQn;    	// �ж�ͨ�����ж�ͨ����stm32f4xx.h����
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority	= 0;				//��ռ���ȼ�
	NVIC_InitStruct.NVIC_IRQChannelSubPriority			= 2;				//��Ӧ���ȼ�
	NVIC_InitStruct.NVIC_IRQChannelCmd					= ENABLE;           //ʹ��
	//��3������ʱ���жϣ�����NVIC��
    NVIC_Init(&NVIC_InitStruct);	
	
	//��4������ TIM3_DIER  ��������ж�
	TIM_ITConfig(TIM3, TIM_IT_Update ,ENABLE);	
	
	
	//��5��ʹ�ܶ�ʱ����
	TIM_Cmd(TIM3, ENABLE);
	


}


//	��6����д�жϷ�������
void TIM3_IRQHandler(void)
{
	
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //����ж�
	{
		/*����ĳ����*/
		GPIO_ToggleBits(GPIOE, GPIO_Pin_14);	
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update); //����жϱ�־λ

}
