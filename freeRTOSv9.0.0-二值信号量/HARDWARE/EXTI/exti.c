#include "exti.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/******************************************
����˵����

key0 -- S1 -- PA0
key1 -- S2 -- PE2
key2 -- S3 -- PE3
key3 -- S4 -- PE4

����δ��������Ϊ�ߵ�ƽ
������������Ϊ�͵�ƽ

��ʼ���ĸ��������������ж�
�ж�����Ϊ�½��ش���

*******************************************/

void allKey_EXTI_Init(void)
{
	EXTI_InitTypeDef  EXTI_InitStruct;
	NVIC_InitTypeDef  NVIC_InitStruct;
	
	//ʹ��SYSCFGʱ�ӣ� 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	//��ʼ�����а���
	AllKeyInit();	
	
	//����IO�����ж��ߵ�ӳ���ϵ��
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource2);	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource3);	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource4);	
	
	//S1
	EXTI_InitStruct.EXTI_Line		  = EXTI_Line0;							//�ж���0
	EXTI_InitStruct.EXTI_Mode		  = EXTI_Mode_Interrupt;		//�ж�ģʽ
	EXTI_InitStruct.EXTI_LineCmd	= ENABLE;								  //�ж���ʹ��
	EXTI_InitStruct.EXTI_Trigger	= EXTI_Trigger_Falling;		//�½��ش���
	//��ʼ�������жϣ����ô��������ȡ�
  EXTI_Init(&EXTI_InitStruct);	
	
	//S2
	EXTI_InitStruct.EXTI_Line		= EXTI_Line2;							//�ж���2
	//��ʼ�������жϣ����ô��������ȡ�
  EXTI_Init(&EXTI_InitStruct);
	
	//S3
	EXTI_InitStruct.EXTI_Line		= EXTI_Line3;							//�ж���3
	//��ʼ�������жϣ����ô��������ȡ�
  EXTI_Init(&EXTI_InitStruct);
	
	//S4
	EXTI_InitStruct.EXTI_Line		= EXTI_Line4;							//�ж���4
	//��ʼ�������жϣ����ô��������ȡ�
  EXTI_Init(&EXTI_InitStruct);
	
	//S1
	NVIC_InitStruct.NVIC_IRQChannel										= EXTI0_IRQn;    	// �ж�ͨ�����ж�ͨ����stm32f4xx.h����
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority	= 6;							//��ռ���ȼ�
	NVIC_InitStruct.NVIC_IRQChannelSubPriority				= 0;							//��Ӧ���ȼ�
	NVIC_InitStruct.NVIC_IRQChannelCmd								= ENABLE;         //ʹ��
	//�����жϷ��飨NVIC������ʹ���жϡ�
  NVIC_Init(&NVIC_InitStruct);
	
	//S2
	NVIC_InitStruct.NVIC_IRQChannel										= EXTI2_IRQn; 
	//�����жϷ��飨NVIC������ʹ���жϡ�
  NVIC_Init(&NVIC_InitStruct);
	
	//S3
	NVIC_InitStruct.NVIC_IRQChannel										= EXTI3_IRQn; 
	//�����жϷ��飨NVIC������ʹ���жϡ�
  NVIC_Init(&NVIC_InitStruct);
	
	//S4
	NVIC_InitStruct.NVIC_IRQChannel										= EXTI4_IRQn; 
	//�����жϷ��飨NVIC������ʹ���жϡ�
  NVIC_Init(&NVIC_InitStruct);
	
}	


//ע�⣬�����жϷ������д�����ʱ��������Ϊ�Ǵ������������ֲ����
//�ڴ���os�ĳ����У���ֹ���жϷ������е����κ���ʽ����ʱ����

extern SemaphoreHandle_t BinarySemaphore;	//��ֵ�ź������
//S1
void EXTI0_IRQHandler(void)
{
	
	//�������
//	if(EXTI_GetITStatus(EXTI_Line0) == SET)
//	{
//		delay_ms(100);
//		if(EXTI_GetITStatus(EXTI_Line0) == SET)
//		{
//			delay_ms(100);
//			//�ж��Ƿ�ס����
//			while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0) == Bit_RESET);
//			//�ɿ�
//			
//			
//		}
//	}
	
//	BaseType_t YieldRequired;
//	
//	delay_ms(50);						//����
//	if(KEY3==0)
//	{
//		YieldRequired=xTaskResumeFromISR(TASK1_Handler);//�ָ�����2
//		printf("�ָ�����1������!\r\n");
//		if(YieldRequired==pdTRUE)
//		{
//			/*�������xTaskResumeFromISR()����ֵΪpdTRUE����ô˵��Ҫ�ָ������
//			������������ȼ����ڻ��߸����������е�����(���жϴ�ϵ�����),������
//			�˳��жϵ�ʱ��һ��Ҫ�����������л���*/
//			portYIELD_FROM_ISR(YieldRequired);
//		}
//	}

	if(KEY0 == 0)
	{
		printf("S1\r\n");
	}
	BaseType_t xHigherPriorityTaskWoken;
	
	if(BinarySemaphore != NULL)			//�����ֵ�ź�����Ϊ��
	{
		xSemaphoreGiveFromISR(BinarySemaphore,&xHigherPriorityTaskWoken);	//�ͷŶ�ֵ�ź���
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);//�����Ҫ�Ļ�����һ�������л�
	}
	//����жϱ�־λ
	EXTI_ClearITPendingBit(EXTI_Line0);
	
}


//S2
void EXTI2_IRQHandler(void)
{
	
	//��������
//	if(EXTI_GetITStatus(EXTI_Line2) == SET)
//	{
//		delay_ms(100);
//		if(EXTI_GetITStatus(EXTI_Line2) == SET)
//		{
//			delay_ms(100);
//			while(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2) == Bit_RESET);
//			//�ɿ�
//			
//			//printf("s2\n");
//			//�ָ�����2

//			
//			
//			
//		}
//	}
	
	//����жϱ�־λ
	EXTI_ClearITPendingBit(EXTI_Line2);

}


//S3
void EXTI3_IRQHandler(void)
{

	//�������
//	if(EXTI_GetITStatus(EXTI_Line3) == SET)
//	{
//		delay_ms(100);
//		if(EXTI_GetITStatus(EXTI_Line3) == SET)
//		{
//			delay_ms(100);
//			while(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3) == Bit_RESET);
//			//�ɿ�
//			
//			//printf("s3\n");

//		
//		}

//	}
	
//	BaseType_t YieldRequired;
//	
//	delay_mns(50);						//����
//	if(KEY2==0)
//	{
//		YieldRequired=xTaskResumeFromISR(TASK1_Handler);//�ָ�����2
//		printf("�ָ�����1������!\r\n");
//		if(YieldRequired==pdTRUE)
//		{
//			/*�������xTaskResumeFromISR()����ֵΪpdTRUE����ô˵��Ҫ�ָ������
//			������������ȼ����ڻ��߸����������е�����(���жϴ�ϵ�����),������
//			�˳��жϵ�ʱ��һ��Ҫ�����������л���*/
//			//portYIELD_FROM_ISR(YieldRequired);
//		}
//	}
	//����жϱ�־λ
	EXTI_ClearITPendingBit(EXTI_Line3);

}

//S4
void EXTI4_IRQHandler(void)
{
	//��������
//	if(EXTI_GetITStatus(EXTI_Line4) == SET)
//	{
//		delay_ms(100);
//		if(EXTI_GetITStatus(EXTI_Line4) == SET)
//		{
//			delay_ms(100);
//			//�ж��Ƿ���
//			//while(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4) == Bit_RESET);
//			//�ɿ�
//			
//			//printf("s4\n");

//			
//			
//		}
//	}
	
	//����жϱ�־λ
	EXTI_ClearITPendingBit(EXTI_Line4);

}


