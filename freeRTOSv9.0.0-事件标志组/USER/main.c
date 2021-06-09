#include "stm32f4xx.h"
#include "led.h"
#include "key.h"
#include "exti.h"
#include "delay.h"
#include "tim.h"
#include "pwm.h"
#include "sys.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

//任务创建任务参数
//任务优先级
#define START_TASK_PRIO		1
//任务堆栈大小	
#define START_STK_SIZE 		128  
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void start_task(void *pvParameters);

//测试任务
//任务优先级
#define test_TASK_PRIO		2
//任务堆栈大小	
#define test_STK_SIZE 		128  
//任务句柄
TaskHandle_t testTask_Handler;
//任务函数
void test(void *pvParameters);

//任务优先级
#define EVENTSETBIT_TASK_PRIO		2
//任务堆栈大小	
#define EVENTSETBIT_STK_SIZE 		256  
//任务句柄
TaskHandle_t EVENTSETBIT_Handler;
//任务函数
void eventsetbit_task(void *pvParameters);

//任务优先级
#define EVENTGROUP_TASK_PRIO		3
//任务堆栈大小	
#define EVENTGROUP_STK_SIZE 		256  
//任务句柄
TaskHandle_t EVENTGROUP_Handler;
//任务函数
void eventgroup_task(void *pvParameters);

//任务优先级
#define EVENTQUERY_TASK_PRIO		4
//任务堆栈大小	
#define EVENTQUERY_STK_SIZE 		128  
//任务句柄
TaskHandle_t EVENTQUERY_Handler;
//任务函数
void eventquery_task(void *pvParameters);

//任务4
//任务优先级
#define TASK4_PRIO		2
//任务堆栈大小	
#define task4_STK_SIZE 		128  
//任务句柄
TaskHandle_t TASK4_Handler;
//任务函数
void task4(void *pvParameters);



///////////////////////////事件标志组///////////////////////////////
//事件位存储在以一个无符号的EventBits_t类型变量中
//当configUSE_16_BIT_TICKS为0时，EventBits_t是一个32位数据类型，可用标志位24位（默认）
//当configUSE_16_BIT_TICKS为1时，EventBits_t是一个16位数据类型，可用标志位8位
/////////////////////////////////////////////////////////////////////
//
/**
*	创建事件标志位函数
*
*	动态方法创建
*	函数原型：EventGroupHandle_t xEventGroupCreate( void )
*	参数：无
*	返回值：
*		NULL	：事件标志组创建失败
*		其他值：创建成功的事件标志组句柄
*	备注：创建成功之后，事件标志组句柄里存储的事件标志位全部清零
*
*
*	静态方法创建
*	函数原型：EventGroupHandle_t xEventGroupCreateStatic( StaticEventGroup_t *pxEventGroupBuffer )
*	参数：
*	pxEventGroupBuffer:参数指向一个StaticEventGroup_t类型的变量，用来保存事件标志组结构体
*	返回值：
*		NULL	：事件标志组创建失败
*		其他值：创建成功的事件标志组句柄
*	备注：创建成功之后，事件标志组句柄里存储的事件标志位全部清零
*/

/**
*	设置事件标志位函数
*
*	标志位置一函数：
*	函数原型：EventBits_t xEventGroupSetBits( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet )
*	参数：
*		xEventGroup：要操作的事件标志组句柄
*		uxBitsToSet：需要置一的事件位
*	返回值：指定事件位置一之后事件标志组
*	备注：
*
*
*	标志位置一函数(中断)：
*	函数原型：BaseType_t xEventGroupSetBitsFromISR( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet, BaseType_t *pxHigherPriorityTaskWoken )
*	参数：
*		xEventGroup：要操作的事件标志组句柄
*		uxBitsToSet：需要置一的事件位
*		pxHigherPriorityTaskWoken：标记退出此函数后是否进行任务切换。当此值为pdTRUE的时候，在退出中断服务函数之前一定要进行一次任务切换。
*	返回值：
*		pdPASS	:事件位置1成功
*		pdFALSE	:事件位置1失败
*	备注：此函数是xEventGroupSetBits的中断版，注意不会返回事件标志组
*
*
*	标志位清零函数：
*	函数原型：EventBits_t xEventGroupClearBits( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToClear )
*	参数：
*		xEventGroup：要操作的事件标志组句柄
*		uxBitsToClear：需要置一的事件位
*	返回值：指定事件位置一之后事件标志组
*	备注：
*
*
*	标志位置一函数(中断)：
*	函数原型：BaseType_t xEventGroupSetBitsFromISR( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet, BaseType_t *pxHigherPriorityTaskWoken )
*	参数：
*		xEventGroup：要操作的事件标志组句柄
*		uxBitsToSet：需要置一的事件位
*		pxHigherPriorityTaskWoken：标记退出此函数后是否进行任务切换。当此值为pdTRUE的时候，在退出中断服务函数之前一定要进行一次任务切换。
*	返回值：指定事件位置一之后事件标志组
*	备注：
*		pdPASS	:事件位置1成功
*		pdFALSE	:事件位置1失败
*/

//
//
////////////////////////////////////////////////////////
EventGroupHandle_t EventGroupHandler;	//事件标志组句柄

#define EVENTBIT_0	(1<<0)				//事件位
#define EVENTBIT_1	(1<<1)
#define EVENTBIT_2	(1<<2)
#define EVENTBIT_ALL	(EVENTBIT_0|EVENTBIT_1|EVENTBIT_2)
////////////////////////////////////////////////////////

int main(void)
{
	//NVIC优先级分组：第二组； 抢占优先级：0~3； 响应优先级：0~3； 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	Delay_Init();
	Led_Init();
	Usart1_Init(115200);
	AllKeyInit();
	
	xTaskCreate((TaskFunction_t )start_task,            //任务函数
							(const char*    )"start_task",          //任务名称
							(uint16_t       )START_STK_SIZE,        //任务堆栈大小
							(void*          )NULL,                  //传递给任务函数的参数
							(UBaseType_t    )START_TASK_PRIO,       //任务优先级
							(TaskHandle_t*  )&StartTask_Handler);   //任务句柄
							
  vTaskStartScheduler();          										//开启任务调度
							
	//return 0;
}


//开始任务任务函数
void start_task(void *pvParameters)
{
	taskENTER_CRITICAL();           //进入临界区

	//创建事件标志组
	EventGroupHandler=xEventGroupCreate();	 //创建事件标志组
	
	//创建测试任务
	xTaskCreate((TaskFunction_t )test,     	
							(const char*    )"test task",   	
							(uint16_t       )test_STK_SIZE, 
							(void*          )NULL,				
							(UBaseType_t    )test_TASK_PRIO,	
							(TaskHandle_t*  )&testTask_Handler);  

	////创建-设置事件位的任务
	xTaskCreate((TaskFunction_t )eventsetbit_task,     	
							(const char*    )"eventsetbit_task",   	
							(uint16_t       )EVENTSETBIT_STK_SIZE, 
							(void*          )NULL,				
							(UBaseType_t    )EVENTSETBIT_TASK_PRIO,	
							(TaskHandle_t*  )&EVENTSETBIT_Handler);
							
	//创建-事件标志组处理任务
	xTaskCreate((TaskFunction_t )eventgroup_task,     	
							(const char*    )"eventgroup_taskk",   	
							(uint16_t       )EVENTGROUP_STK_SIZE, 
							(void*          )NULL,				
							(UBaseType_t    )EVENTGROUP_TASK_PRIO,	
							(TaskHandle_t*  )&EVENTGROUP_Handler);
							
	//创建高优先级任务
	xTaskCreate((TaskFunction_t )eventquery_task,     	
							(const char*    )"eventquery_task",   	
							(uint16_t       )EVENTQUERY_STK_SIZE, 
							(void*          )NULL,				
							(UBaseType_t    )EVENTQUERY_TASK_PRIO,	
							(TaskHandle_t*  )&EVENTQUERY_Handler);
							
	//创建任务4
	xTaskCreate((TaskFunction_t )task4,     	
							(const char*    )"the task4",   	
							(uint16_t       )task4_STK_SIZE, 
							(void*          )NULL,				
							(UBaseType_t    )TASK4_PRIO,	
							(TaskHandle_t*  )&TASK4_Handler);
	 
	vTaskDelete(StartTask_Handler); //删除开始任务
	taskEXIT_CRITICAL();            //退出临界区
}

//任务test
void test(void *pvParameters)
{
	//vTaskDelete(NULL);
	while(1)
	{
		
		LED0 = 0;
		F_delay_s(1);
		
		LED0 = 1;
		LED1 = 0;
		
		F_delay_s(1);
		LED1 = 1;
		LED2 = 0;
		
		F_delay_s(1);
		LED2 = 1;
		LED3 = 0;
		
		F_delay_s(1);
		LED3 = 1;
	}
		
}

//设置事件位的任务
void eventsetbit_task(void *pvParameters)
{
	//vTaskDelete(NULL);
	u8 key;
	while(1)
	{
		if(EventGroupHandler!=NULL)
		{
			key=KEY_Scan(0);
			switch(key)
			{
				case KEY0_PRESD:
					xEventGroupSetBits(EventGroupHandler,EVENTBIT_1);	//设置0x0010
					break;
				case KEY1_PRESD:
					xEventGroupSetBits(EventGroupHandler,EVENTBIT_2);	//设置0x0100
					break;	
			}
		}
    vTaskDelay(10); //延时10ms，也就是10个时钟节拍
	}
}

//事件标志组处理任务
void eventgroup_task(void * pvParameters)
{
	//vTaskDelete(NULL);
	EventBits_t EventValue;
	while(1)
	{

		if(EventGroupHandler!=NULL)
		{
			//等待事件组中的相应事件位
			EventValue=xEventGroupWaitBits((EventGroupHandle_t	)EventGroupHandler,		//句柄
																		 (EventBits_t					)EVENTBIT_1|EVENTBIT_2,				//等待0x0111
																		 (BaseType_t					)pdTRUE,							//事件位清零
																		 (BaseType_t					)pdTRUE,							//与判断
																		 (TickType_t					)portMAX_DELAY);			//阻塞等待
			printf("事件：事件标志组的值:%d\r\n",EventValue);
			
		}
		else
		{
			vTaskDelay(10); //延时10ms，也就是10个时钟节拍
		}
	}
}

//事件查询任务
void eventquery_task(void *pvParameters)
{	
	//vTaskDelete(NULL);
	EventBits_t NewValue,LastValue;
	while(1)
	{
		if(EventGroupHandler!=NULL)
		{
			NewValue=xEventGroupGetBits(EventGroupHandler);	//获取事件组的
			if(NewValue!=LastValue)
			{
				LastValue=NewValue;
				printf("查询：事件标志组的值:%d\r\n",NewValue);
			}
		}
		vTaskDelay(50); //延时50ms，也就是50个时钟节拍
	}
}
	


//任务4
void task4(void *pvParameters)
{
	vTaskDelete(NULL);
	for(;;)
	{
		printf("KEY0 = %d\r\n",KEY0);
		printf("KEY1 = %d\r\n",KEY1);
		printf("KEY2 = %d\r\n",KEY2);
		printf("KEY3 = %d\r\n",KEY3);
		printf("\r\n\r\n");
		F_delay_ms(500);
	}
}
