#include "exti.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/******************************************
引脚说明：

key0 -- S1 -- PA0
key1 -- S2 -- PE2
key2 -- S3 -- PE3
key3 -- S4 -- PE4

按键未按下引脚为高电平
按键按下引脚为低电平

初始化四个按键，并配置中断
中断设置为下降沿触发

*******************************************/

void allKey_EXTI_Init(void)
{
	EXTI_InitTypeDef  EXTI_InitStruct;
	NVIC_InitTypeDef  NVIC_InitStruct;
	
	//使能SYSCFG时钟： 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	//初始化所有按键
	AllKeyInit();	
	
	//设置IO口与中断线的映射关系。
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource2);	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource3);	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource4);	
	
	//S1
	EXTI_InitStruct.EXTI_Line		  = EXTI_Line0;							//中断线0
	EXTI_InitStruct.EXTI_Mode		  = EXTI_Mode_Interrupt;		//中断模式
	EXTI_InitStruct.EXTI_LineCmd	= ENABLE;								  //中断线使能
	EXTI_InitStruct.EXTI_Trigger	= EXTI_Trigger_Falling;		//下降沿触发
	//初始化线上中断，设置触发条件等。
  EXTI_Init(&EXTI_InitStruct);	
	
	//S2
	EXTI_InitStruct.EXTI_Line		= EXTI_Line2;							//中断线2
	//初始化线上中断，设置触发条件等。
  EXTI_Init(&EXTI_InitStruct);
	
	//S3
	EXTI_InitStruct.EXTI_Line		= EXTI_Line3;							//中断线3
	//初始化线上中断，设置触发条件等。
  EXTI_Init(&EXTI_InitStruct);
	
	//S4
	EXTI_InitStruct.EXTI_Line		= EXTI_Line4;							//中断线4
	//初始化线上中断，设置触发条件等。
  EXTI_Init(&EXTI_InitStruct);
	
	//S1
	NVIC_InitStruct.NVIC_IRQChannel										= EXTI0_IRQn;    	// 中断通道，中断通道在stm32f4xx.h查阅
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority	= 6;							//抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority				= 0;							//响应优先级
	NVIC_InitStruct.NVIC_IRQChannelCmd								= ENABLE;         //使能
	//配置中断分组（NVIC），并使能中断。
  NVIC_Init(&NVIC_InitStruct);
	
	//S2
	NVIC_InitStruct.NVIC_IRQChannel										= EXTI2_IRQn; 
	//配置中断分组（NVIC），并使能中断。
  NVIC_Init(&NVIC_InitStruct);
	
	//S3
	NVIC_InitStruct.NVIC_IRQChannel										= EXTI3_IRQn; 
	//配置中断分组（NVIC），并使能中断。
  NVIC_Init(&NVIC_InitStruct);
	
	//S4
	NVIC_InitStruct.NVIC_IRQChannel										= EXTI4_IRQn; 
	//配置中断分组（NVIC），并使能中断。
  NVIC_Init(&NVIC_InitStruct);
	
}	


//注意，以下中断服务函数中存在延时函数，因为是从裸机程序中移植来的
//在带有os的程序中，禁止在中断服务函数中调用任何形式的延时函数

extern SemaphoreHandle_t BinarySemaphore;	//二值信号量句柄
//S1
void EXTI0_IRQHandler(void)
{
	
	//软件消抖
//	if(EXTI_GetITStatus(EXTI_Line0) == SET)
//	{
//		delay_ms(100);
//		if(EXTI_GetITStatus(EXTI_Line0) == SET)
//		{
//			delay_ms(100);
//			//判断是否按住不动
//			while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0) == Bit_RESET);
//			//松开
//			
//			
//		}
//	}
	
//	BaseType_t YieldRequired;
//	
//	delay_ms(50);						//消抖
//	if(KEY3==0)
//	{
//		YieldRequired=xTaskResumeFromISR(TASK1_Handler);//恢复任务2
//		printf("恢复任务1的运行!\r\n");
//		if(YieldRequired==pdTRUE)
//		{
//			/*如果函数xTaskResumeFromISR()返回值为pdTRUE，那么说明要恢复的这个
//			任务的任务优先级等于或者高于正在运行的任务(被中断打断的任务),所以在
//			退出中断的时候一定要进行上下文切换！*/
//			portYIELD_FROM_ISR(YieldRequired);
//		}
//	}

	if(KEY0 == 0)
	{
		printf("S1\r\n");
	}
	BaseType_t xHigherPriorityTaskWoken;
	
	if(BinarySemaphore != NULL)			//如果二值信号量不为空
	{
		xSemaphoreGiveFromISR(BinarySemaphore,&xHigherPriorityTaskWoken);	//释放二值信号量
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);//如果需要的话进行一次任务切换
	}
	//清除中断标志位
	EXTI_ClearITPendingBit(EXTI_Line0);
	
}


//S2
void EXTI2_IRQHandler(void)
{
	
	//物理消抖
//	if(EXTI_GetITStatus(EXTI_Line2) == SET)
//	{
//		delay_ms(100);
//		if(EXTI_GetITStatus(EXTI_Line2) == SET)
//		{
//			delay_ms(100);
//			while(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2) == Bit_RESET);
//			//松开
//			
//			//printf("s2\n");
//			//恢复任务2

//			
//			
//			
//		}
//	}
	
	//清除中断标志位
	EXTI_ClearITPendingBit(EXTI_Line2);

}


//S3
void EXTI3_IRQHandler(void)
{

	//软件消抖
//	if(EXTI_GetITStatus(EXTI_Line3) == SET)
//	{
//		delay_ms(100);
//		if(EXTI_GetITStatus(EXTI_Line3) == SET)
//		{
//			delay_ms(100);
//			while(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3) == Bit_RESET);
//			//松开
//			
//			//printf("s3\n");

//		
//		}

//	}
	
//	BaseType_t YieldRequired;
//	
//	delay_mns(50);						//消抖
//	if(KEY2==0)
//	{
//		YieldRequired=xTaskResumeFromISR(TASK1_Handler);//恢复任务2
//		printf("恢复任务1的运行!\r\n");
//		if(YieldRequired==pdTRUE)
//		{
//			/*如果函数xTaskResumeFromISR()返回值为pdTRUE，那么说明要恢复的这个
//			任务的任务优先级等于或者高于正在运行的任务(被中断打断的任务),所以在
//			退出中断的时候一定要进行上下文切换！*/
//			//portYIELD_FROM_ISR(YieldRequired);
//		}
//	}
	//清除中断标志位
	EXTI_ClearITPendingBit(EXTI_Line3);

}

//S4
void EXTI4_IRQHandler(void)
{
	//物理消抖
//	if(EXTI_GetITStatus(EXTI_Line4) == SET)
//	{
//		delay_ms(100);
//		if(EXTI_GetITStatus(EXTI_Line4) == SET)
//		{
//			delay_ms(100);
//			//判断是否按着
//			//while(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4) == Bit_RESET);
//			//松开
//			
//			//printf("s4\n");

//			
//			
//		}
//	}
	
	//清除中断标志位
	EXTI_ClearITPendingBit(EXTI_Line4);

}


