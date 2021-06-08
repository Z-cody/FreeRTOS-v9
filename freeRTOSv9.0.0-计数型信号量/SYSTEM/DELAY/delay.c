#include "delay.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用OS,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "FreeRTOS.h"					//FreeRTOS使用	 
#include "task.h"
u16 F_ms = 1;									//configTICK_RATE_HZ是FreeRTOS的系统频率（最小节拍），设定为1000Hz（可自定义），即1ms计数1
#endif
//////////////////////////////////////////////////////////////////////////////////

u16 my_us = 21;  	//1us计数21个数
u16 my_ms = 21000;  //1ms计数21000个数

extern void xPortSysTickHandler(void);
//systick中断服务函数,使用OS时用到
void SysTick_Handler(void)
{  
    if(xTaskGetSchedulerState()!=taskSCHEDULER_NOT_STARTED)//系统已经运行
    {
        xPortSysTickHandler();	
    }
}

//初始化定时器的时钟频率
void Delay_Init(void)
{
	//选择外部时钟源 168MHZ/8 = 21MHZ  1us计21个数
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	SysTick->CTRL |= (0x01<<1);						//开启中断
	
}


/*******************
以下函数能使用于裸机程序的延时，
不适用与带有操作系统的程序中，
因为操作系统的任务调度会使任务脱离延时
********************/
//cortex-M4的滴答定时器的reload寄存器是一个24位寄存器
//最多只可计数16777243
//延时us    798915 
void delay_us(u32 nus)
{
	u32 temp;
	
	
	//重装值寄存器
	SysTick->LOAD = my_us*nus-1;  
	//计数器置为0
	SysTick->VAL = 0x00;
	//启动定时器
	SysTick->CTRL |= (0x01<<0);
	
	do
	{
		temp = SysTick->CTRL;     
			//判断是否启动定时器 //第十六位1，则退出
	}while( (temp & (0x01<<0)) && !(temp&(0x01<<16)));
	
	
	//关闭定时器
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


//延时ms    最大延时值798.915 
void delay_ms(u32 nms)
{
	u32 temp;
	
	
	//重装值寄存器
	SysTick->LOAD = my_ms*nms-1;  
	//计数器置为0
	SysTick->VAL = 0x00;
	//启动定时器
	SysTick->CTRL |= (0x01<<0);
					
	
	do
	{
		temp = SysTick->CTRL;     
			//判断是否启动定时器 //第十六位1，则退出
	}while( (temp & (0x01<<0)) && !(temp&(0x01<<16)));
	
	
	//关闭定时器
	SysTick->CTRL &= ~(0x01<<0);	

}

//秒级延时
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
以下延时函数加入vTaskDelay
可用于FreeRTOS的任务延时
**************************/
#if SYSTEM_SUPPORT_OS
//1000Hz的系统节拍无法满足纳秒级的延时
//实际上1ms以下的延时使用普通延时已经足够
//但如果追求更精确的纳秒延时则需要修改configTICK_RATE_HZ的值
//纳秒级
void F_delay_ns(u32 us)
{
	
}

//毫秒级
void F_delay_ms(u32 ms)
{
//	if(xTaskGetSchedulerState()!=taskSCHEDULER_NOT_STARTED)//系统已经运行
//	{		
//		if(ms>=F_ms)						//延时的时间大于OS的最少时间周期 
//		{ 
//   			vTaskDelay(ms/F_ms);	 		//FreeRTOS延时
//		}
//		ms%=F_ms;						//OS已经无法提供这么小的延时了,采用普通方式延时    
//	}
//	delay_us((u32)(ms*1000));				//普通方式延时
	
	if(xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
	{
		if(ms >= F_ms)				//延时时间必须大于系统的最小节拍
		{
			vTaskDelay(ms/F_ms);
		}
		else									//如果系统的延时无法满足，则使用嘀嗒定时器
		{
			ms %= F_ms;
			delay_ms(ms);
		}
	}
}

//秒级
void F_delay_s(u32 s)
{
	if(xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
	{
		vTaskDelay(s*1000/F_ms);
	}
}
#endif
