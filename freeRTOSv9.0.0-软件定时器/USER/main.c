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
#include "timers.h"

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

//任务1
//任务优先级
#define TASK1_PRIO		2
//任务堆栈大小	
#define task1_STK_SIZE 		128  
//任务句柄
TaskHandle_t TASK1_Handler;
//任务函数
void task1(void *pvParameters);

//任务2
//任务优先级
#define TASK2_PRIO		2
//任务堆栈大小	
#define task2_STK_SIZE 		128  
//任务句柄
TaskHandle_t TASK2_Handler;
//任务函数
void task2(void *pvParameters);

//任务3
//任务优先级
#define TASK3_PRIO		2
//任务堆栈大小	
#define task3_STK_SIZE 		128  
//任务句柄
TaskHandle_t TASK3_Handler;
//任务函数
void task3(void *pvParameters);

//任务4
//任务优先级
#define TASK4_PRIO		2
//任务堆栈大小	
#define task4_STK_SIZE 		128  
//任务句柄
TaskHandle_t TASK4_Handler;
//任务函数
void task4(void *pvParameters);

//定时器控制任务
//任务优先级
#define TIMERCONTROL_TASK_PRIO	2
//任务堆栈大小	
#define TIMERCONTROL_STK_SIZE 	256  
//任务句柄
TaskHandle_t TimerControlTask_Handler;
//任务函数
void timercontrol_task(void *pvParameters);


TimerHandle_t 	AutoReloadTimer_Handle;			//周期定时器句柄
TimerHandle_t		OneShotTimer_Handle;				//单次定时器句柄

void AutoReloadCallback(TimerHandle_t xTimer); 	//周期定时器回调函数
void OneShotCallback(TimerHandle_t xTimer);		//单次定时器回调函数

////////////////FreeRTOS的软件定时器//////////////////////
//1.软件定时器的基本功能是周期性地处理某些任务，这些任务叫做回调函数
//2.软件定时器的精确性不如硬件定时器
//3.常使用与硬件定时器不够用，或者一些对精度要求不高的回调函数
//4.回调函数中不可以调用引起阻塞的函数

//软件定时器分为单次定时器和周期定时器
//单次定时器：一个周期之后执行一次回调函数，之后定时器便会关闭
//周期定时器：定时器会个周期之后会自动重启，回调函数也一遍遍执行



//////////////////////////////软件定时器相关API///////////////////////////////
//
/////////////////////定时器创建///////////////////
//函数： TimerHandle_t xTimerCreate(	const char * const pcTimerName,
//																		const TickType_t xTimerPeriodInTicks,
//																		const UBaseType_t uxAutoReload,
//																		void * const pvTimerID,
//																		TimerCallbackFunction_t pxCallbackFunction )
//参数：
//pcTimerName:定时器的名字，作用与任务的pcName类似
//xTimerPeriodInTicks：软件定时器的定时器周期，单位是系统最小节拍。
//uxAutoReload: pdTRUE：设置周期定时器     pdFALSE：设置成单次定时器
//pvTimerID：定时器ID。在回调函数中根据定时器ID处理不同的定时器
//pxCallbackFunction：回调函数的函数名
//备注：动态方法创建定时器
//
//
//
//函数： TimerHandle_t xTimerCreateStatic(	const char * const pcTimerName,
//																					const TickType_t xTimerPeriodInTicks,
//																					const UBaseType_t uxAutoReload,
//																					void * const pvTimerID,
//																					TimerCallbackFunction_t pxCallbackFunction,
//																					StaticTimer_t *pxTimerBuffer )
//参数：
//pcTimerName:定时器的名字，作用与任务的pcName类似
//xTimerPeriodInTicks：软件定时器的定时器周期，单位是系统最小节拍。
//uxAutoReload: pdTRUE：设置周期定时器     pdFALSE：设置成单次定时器
//pvTimerID：定时器ID。在回调函数中根据定时器ID处理不同的定时器
//pxCallbackFunction：回调函数的函数名
//pxTimerBuffer:指向一个StaticTimer_t结构体变量的指针
//备注：动态方法创建定时器
////////////////////////////////////////////////////////////////////////////////
//
//
/////////////////////定时开启/////////////////////
//函数： BaseType_t xTimerStart( TimerHandle_t xTimer, TickType_t xTicksToWait );
//参数：
//xTimer: 要开启的定时器句柄
//xTicksToWait： 设置阻塞时间
//备注：
//真正执行的函数是xTimerGenericCommand()
//定时器开启实际上往定时器命令队列里发送一条tmrCOMMAND_START命令，
//如果队列满，则根据设置的阻塞时间进行等待
//该函数用于在任务中开启定时器
//当定时已经开启，执行此函数效果与xTimerReset是一样的
//返回值：
//pdTRUE:定时器开启成功
//pdFALSE:定时器开启失败
//
//
//函数： BaseType_t xTimerStartFromISR( 	TimerHandle_t xTimer,
// 																				BaseType_t *pxHigherPriorityTaskWoken );
//参数：
//xTimer: 要开启的定时器句柄
//pxHigherPriorityTaskWoken: 标记退出此函数后是否进行任务切换。当此值为pdTRUE的时候，在退出中断服务函数之前一定要进行一次任务切换。
//备注：
//真正执行的函数是xTimerGenericCommand()
//定时器开启实际上往定时器命令队列里发送一条tmrCOMMAND_START命令，
//此函数没有阻塞时间，如果队满则直接跳出
//该函数用于在中断中开启定时器
//当定时已经开启，执行此函数效果与xTimerReset是一样的
//pdTRUE:定时器开启成功
//pdFALSE:定时器开启失败
//////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////定时器关闭//////////////////////////
//函数： BaseType_t xTimerStop( TimerHandle_t xTimer, TickType_t xTicksToWait );
//参数：
//xTimer: 要关闭的定时器句柄
//xTicksToWait： 设置阻塞时间
//备注：
//真正执行的函数是xTimerGenericCommand()
//定时器开启实际上往定时器命令队列里发送一条tmrCOMMAND_STOP命令，
//如果队列满，则根据设置的阻塞时间进行等待
//该函数用于在任务中关闭定时器
//pdTRUE:定时器关闭成功
//pdFALSE:定时器关闭失败
//
//
//函数： BaseType_t xTimerStopFromISR( 	TimerHandle_t xTimer,
 //																			BaseType_t *pxHigherPriorityTaskWoken );
//参数：
//xTimer: 要关闭的定时器句柄
//pxHigherPriorityTaskWoken: 标记退出此函数后是否进行任务切换。当此值为pdTRUE的时候，在退出中断服务函数之前一定要进行一次任务切换。
//备注：
//真正执行的函数是xTimerGenericCommand()
//定时器开启实际上往定时器命令队列里发送一条tmrCOMMAND_STOP命令，
//此函数没有阻塞时间，如果队满则直接跳出
//该函数用于在中断中关闭定时器
//pdTRUE:定时器关闭成功
//pdFALSE:定时器关闭失败
////////////////////////////////////////////////////////////////
//
//
/////////////////////////////////定时器重启///////////////////////
//函数： BaseType_t xTimerReset( TimerHandle_t xTimer, TickType_t xTicksToWait );
//参数：
//xTimer: 要重启的定时器句柄
//xTicksToWait： 设置阻塞时间
//备注：
//真正执行的函数是xTimerGenericCommand()
//定时器开启实际上往定时器命令队列里发送一条tmrCOMMAND_RESET命令，
//如果队列满，则根据设置的阻塞时间进行等待
//该函数用于在任务中重启定时器
//当定时器已经开启，执行此函数效果与xTimerReset是一样的，
//如果定时器未开启，执行此函数没有效果
//返回值：
//pdTRUE:定时器重启成功
//pdFALSE:定时器重启失败
//
//
//函数： BaseType_t xTimerResetFromISR( 	TimerHandle_t xTimer,
// 																				BaseType_t *pxHigherPriorityTaskWoken );
//参数：
//xTimer: 要重启的定时器句柄
//pxHigherPriorityTaskWoken: 标记退出此函数后是否进行任务切换。当此值为pdTRUE的时候，在退出中断服务函数之前一定要进行一次任务切换。
//备注：
//真正执行的函数是xTimerGenericCommand()
//定时器开启实际上往定时器命令队列里发送一条tmrCOMMAND_RESET命令，
//此函数没有阻塞时间，如果队满则直接跳出
//该函数用于在中断中重启定时器
//当定时已经重启，执行此函数效果与xTimerReset是一样的
//如果定时器未开启，执行此函数没有效果
//pdTRUE:定时器开启成功
//pdFALSE:定时器开启失败
//////////////////////////////////////////////////////////////////////////



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
	
	//创建测试任务
	xTaskCreate((TaskFunction_t )test,     	
							(const char*    )"test task",   	
							(uint16_t       )test_STK_SIZE, 
							(void*          )NULL,				
							(UBaseType_t    )test_TASK_PRIO,	
							(TaskHandle_t*  )&testTask_Handler);  

	//创建任务1
	xTaskCreate((TaskFunction_t )task1,     	
							(const char*    )"the task1",   	
							(uint16_t       )task1_STK_SIZE, 
							(void*          )NULL,				
							(UBaseType_t    )TASK1_PRIO,	
							(TaskHandle_t*  )&TASK1_Handler);
							
	//创建任务2
	xTaskCreate((TaskFunction_t )task2,     	
							(const char*    )"the task2",   	
							(uint16_t       )task2_STK_SIZE, 
							(void*          )NULL,				
							(UBaseType_t    )TASK2_PRIO,	
							(TaskHandle_t*  )&TASK2_Handler);
							
	//创建任务3
	xTaskCreate((TaskFunction_t )task3,     	
							(const char*    )"the task3",   	
							(uint16_t       )task3_STK_SIZE, 
							(void*          )NULL,				
							(UBaseType_t    )TASK3_PRIO,	
							(TaskHandle_t*  )&TASK3_Handler);
							
	//创建任务4
	xTaskCreate((TaskFunction_t )task4,     	
							(const char*    )"the task4",   	
							(uint16_t       )task4_STK_SIZE, 
							(void*          )NULL,				
							(UBaseType_t    )TASK4_PRIO,	
							(TaskHandle_t*  )&TASK4_Handler);
	
	//创建软件周期定时器
	//周期定时器，周期1s(1000个时钟节拍)，周期模式
  AutoReloadTimer_Handle=xTimerCreate((const char*		)"AutoReloadTimer",
																			(TickType_t			)1000,
																			(UBaseType_t		)pdTRUE,
																			(void*				)1,
																			(TimerCallbackFunction_t)AutoReloadCallback); 
  //创建单次定时器
	//单次定时器，周期2s(2000个时钟节拍)，单次模式
	OneShotTimer_Handle=xTimerCreate((const char*			)"OneShotTimer",
																	 (TickType_t			)2000,
																	 (UBaseType_t			)pdFALSE,
																	 (void*					)2,
																	 (TimerCallbackFunction_t)OneShotCallback); 
							
	//创建定时器控制任务
  xTaskCreate((TaskFunction_t )timercontrol_task,             
							(const char*    )"timercontrol_task",           
							(uint16_t       )TIMERCONTROL_STK_SIZE,        
							(void*          )NULL,                  
							(UBaseType_t    )TIMERCONTROL_TASK_PRIO,        
							(TaskHandle_t*  )&TimerControlTask_Handler);
	 
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

//任务1
void task1(void * pvParameters)
{
	//vTaskDelete(NULL);
	int i = 0;
	for(;;)
	{
		printf("task1:i=%d\r\n",i);
		i++;
		vTaskDelay(1000);
		
	}
}

//任务2
void task2(void * pvParameters)
{
	vTaskDelete(NULL);
	int i=0;
	for(;;)
	{
		i++;
		if(i == 5)
		{
			vTaskSuspend(TASK1_Handler);
		}
		if(i == 10)
		{
			i = 0;
			vTaskResume(TASK1_Handler);
		}
		F_delay_s(1);
	}
}

//任务3
void task3(void *pvParameters)
{
	vTaskDelete(NULL);
	
}

//任务4
void task4(void *pvParameters)
{
	vTaskDelete(NULL);
}

void timercontrol_task(void * pvParameters)
{
	u8 key,num;
	while(1)
	{
		//只有两个定时器都创建成功了才能对其进行操作
		if((AutoReloadTimer_Handle!=NULL)&&(OneShotTimer_Handle!=NULL))
		{
			key = KEY_Scan(0);
			switch(key)
			{
				case KEY0_PRESD:     //当key_up按下的话打开周期定时器
					xTimerStart(AutoReloadTimer_Handle,0);	//开启周期定时器
					printf("开启定时器1\r\n");
					break;
				case KEY1_PRESD:		//当key0按下的话打开单次定时器
					xTimerStart(OneShotTimer_Handle,0);		//开启单次定时器
					printf("开启定时器2\r\n");
					break;
				case KEY2_PRESD:		//当key1按下话就关闭定时器
					xTimerStop(AutoReloadTimer_Handle,0); 	//关闭周期定时器
					xTimerStop(OneShotTimer_Handle,0); 		//关闭单次定时器
					printf("关闭定时器1和2\r\n");
					break;	
			}
		}
    vTaskDelay(10); //延时10ms，也就是10个时钟节拍
	}
}

//周期定时器的回调函数
void AutoReloadCallback(TimerHandle_t xTimer)
{
	static u8 tmr1_num=0;
	tmr1_num++;									//周期定时器执行次数加1
	printf("周期定时器回调函数执行次数:%d\r\n",tmr1_num);
}

//单次定时器的回调函数
void OneShotCallback(TimerHandle_t xTimer)
{
	static u8 tmr2_num = 0;
	tmr2_num++;		//周期定时器执行次数加1
  printf("单次定时器运行结束，回调函数执行次数:%d\r\n",tmr2_num);
}
