#include "delay.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��OS,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "FreeRTOS.h"					//FreeRTOSʹ��	 
#include "task.h"
u16 F_ms = 1;									//configTICK_RATE_HZ��FreeRTOS��ϵͳƵ�ʣ���С���ģ����趨Ϊ1000Hz�����Զ��壩����1ms����1
#endif
//////////////////////////////////////////////////////////////////////////////////

u16 my_us = 21;  	//1us����21����
u16 my_ms = 21000;  //1ms����21000����

extern void xPortSysTickHandler(void);
//systick�жϷ�����,ʹ��OSʱ�õ�
void SysTick_Handler(void)
{  
    if(xTaskGetSchedulerState()!=taskSCHEDULER_NOT_STARTED)//ϵͳ�Ѿ�����
    {
        xPortSysTickHandler();	
    }
}

//��ʼ����ʱ����ʱ��Ƶ��
void Delay_Init(void)
{
	//ѡ���ⲿʱ��Դ 168MHZ/8 = 21MHZ  1us��21����
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	SysTick->CTRL |= (0x01<<1);						//�����ж�
	
}


/*******************
���º�����ʹ��������������ʱ��
����������в���ϵͳ�ĳ����У�
��Ϊ����ϵͳ��������Ȼ�ʹ����������ʱ
********************/
//cortex-M4�ĵδ�ʱ����reload�Ĵ�����һ��24λ�Ĵ���
//���ֻ�ɼ���16777243
//��ʱus    798915 
void delay_us(u32 nus)
{
	u32 temp;
	
	
	//��װֵ�Ĵ���
	SysTick->LOAD = my_us*nus-1;  
	//��������Ϊ0
	SysTick->VAL = 0x00;
	//������ʱ��
	SysTick->CTRL |= (0x01<<0);
	
	do
	{
		temp = SysTick->CTRL;     
			//�ж��Ƿ�������ʱ�� //��ʮ��λ1�����˳�
	}while( (temp & (0x01<<0)) && !(temp&(0x01<<16)));
	
	
	//�رն�ʱ��
	SysTick->CTRL &= ~(0x01<<0);	

}

void delay_mns(u32 mns)
{
	int i = 0;
	for(;i<mns;i++)
	{
		delay_us(1000);
	}
}


//��ʱms    �����ʱֵ798.915 
void delay_ms(u32 nms)
{
	u32 temp;
	
	
	//��װֵ�Ĵ���
	SysTick->LOAD = my_ms*nms-1;  
	//��������Ϊ0
	SysTick->VAL = 0x00;
	//������ʱ��
	SysTick->CTRL |= (0x01<<0);
					
	
	do
	{
		temp = SysTick->CTRL;     
			//�ж��Ƿ�������ʱ�� //��ʮ��λ1�����˳�
	}while( (temp & (0x01<<0)) && !(temp&(0x01<<16)));
	
	
	//�رն�ʱ��
	SysTick->CTRL &= ~(0x01<<0);	

}

//�뼶��ʱ
void delay_s(u32 ns)
{
	u16 i;
	
	for(i=0; i<ns; i++)
	{
		delay_ms(500);
		delay_ms(500);
	}

}


/*************************
������ʱ��������vTaskDelay
������FreeRTOS��������ʱ
**************************/
#if SYSTEM_SUPPORT_OS
//1000Hz��ϵͳ�����޷��������뼶����ʱ
//ʵ����1ms���µ���ʱʹ����ͨ��ʱ�Ѿ��㹻
//�����׷�����ȷ��������ʱ����Ҫ�޸�configTICK_RATE_HZ��ֵ
//���뼶
void F_delay_ns(u32 us)
{
	
}

//���뼶
void F_delay_ms(u32 ms)
{
//	if(xTaskGetSchedulerState()!=taskSCHEDULER_NOT_STARTED)//ϵͳ�Ѿ�����
//	{		
//		if(ms>=F_ms)						//��ʱ��ʱ�����OS������ʱ������ 
//		{ 
//   			vTaskDelay(ms/F_ms);	 		//FreeRTOS��ʱ
//		}
//		ms%=F_ms;						//OS�Ѿ��޷��ṩ��ôС����ʱ��,������ͨ��ʽ��ʱ    
//	}
//	delay_us((u32)(ms*1000));				//��ͨ��ʽ��ʱ
	
	if(xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
	{
		if(ms >= F_ms)				//��ʱʱ��������ϵͳ����С����
		{
			vTaskDelay(ms/F_ms);
		}
		else									//���ϵͳ����ʱ�޷����㣬��ʹ����શ�ʱ��
		{
			ms %= F_ms;
			delay_ms(ms);
		}
	}
}

//�뼶
void F_delay_s(u32 s)
{
	if(xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
	{
		vTaskDelay(s*1000/F_ms);
	}
}
#endif
