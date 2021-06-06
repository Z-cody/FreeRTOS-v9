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

//测试任务
//任务优先级
#define test_TASK_PRIO		2
//任务堆栈大小	
#define test_STK_SIZE 		128  
//任务句柄
TaskHandle_t testTask_Handler;
//任务函数
void test(void *pvParameters);

//低优先级任务
//任务优先级
#define LOW_TASK_PRIO		2
//任务堆栈大小	
#define LOW_TASK_STK_SIZE 		128  
//任务句柄
TaskHandle_t LOW_TASK_Handler;
//任务函数
void low_task(void *pvParameters);

//中优先级任务
//任务优先级
#define MIDDLE_TASK_PRIO		3
//任务堆栈大小	
#define MIDDLE_TASK_STK_SIZE 		128  
//任务句柄
TaskHandle_t MIDDLE_TASK_Handler;
//任务函数
void middle_task(void *pvParameters);

//高优先级任务
//任务优先级
#define HIGH_TASK_PRIO		4
//任务堆栈大小	
#define HIGH_TASK_STK_SIZE 		128  
//任务句柄
TaskHandle_t HIGH_TASK_Handler;
//任务函数
void high_task(void *pvParameters);

//任务4
//任务优先级
#define TASK4_PRIO		2
//任务堆栈大小	
#define task4_STK_SIZE 		128  
//任务句柄
TaskHandle_t TASK4_Handler;
//任务函数
void task4(void *pvParameters);

//互斥信号量句柄
SemaphoreHandle_t MutexSemaphore;	//互斥信号量句柄

int main(void)
{
	//NVIC优先级分组：第二组； 抢占优先级：0~3； 响应优先级：0~3； 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	Delay_Init();
	Led_Init();
	Usart1_Init(115200);
	allKey_EXTI_Init();
	
	xTaskCreate((TaskFunction_t )start_task,            //任务函数
							(const char*    )"start_task",          //任务名称
							(uint16_t       )START_STK_SIZE,        //任务堆栈大小
							(void*          )NULL,                  //传递给任务函数的参数
							(UBaseType_t    )START_TASK_PRIO,       //任务优先级
							(TaskHandle_t*  )&StartTask_Handler);   //任务句柄
							
  vTaskStartScheduler();          										//开启任务调度
							
	//return 0;
}

//FreeRTOS的信号量:SemaphoreHandle_t
//释放（+1操作） ——>  信号量有效/队列满
//获取（-1操作） ——>  信号量无效/队列空
//
//
///////////////////互斥信号量//////////////////////
//1.互斥信号量就是拥有优先级继承的二值信号量。适用于互斥访问。
//2.当一个互斥信号量被一个低优先任务获取后，此时一个高优先级任务也尝试
//获取这个互斥信号量就会被阻塞，但高优先级任务就会将低优先级任务的优先级提升到与自己相同优先级上。这个过程就是优先级继承
//3.互斥信号量不能完全地消除优先级翻转问题，而是尽可能减少
//4.互斥信号量不能用于中断服务函中
//
//
//////////////////////////////////////互斥信号量创建函数///////////////////////////////////
//函数原型：SemaphoreHandle_t xSemaphoreCreateMutex( void );
//参数：无
//返回值：
//NULL：创建失败
//互斥信号量句柄：创建成功
//备注：动态分配内存方法创建。
//此函数本质是调用xQueueCreateMutex函数创建队列。
//创建成功的信号量是有效的
//
//
//函数原型：SemaphoreHandle_t xSemaphoreCreateMutexStatic( StaticSemaphore_t *pxMutexBuffer );
//参数：
//pxMutexBuffer ：此参数用来指向一个StaticSemaphore_t类型结构变量
//返回值：
//NULL：创建失败
//信号量句柄：创建成功
//备注：静态方法分配内存创建。
//此函数本质是调用xQueueCreateMutexStatic函数创建队列。
//创建的信号量是有效的。
//
///////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////互斥信号量的获取////////////////////////////////////////
//函数原型：BaseType_t xSemaphoreTake(SemaphoreHandle_t xSemaphore,TickType_t xBlockTime)
//参数：
//xSemaphore：要获取的信号量句柄
//xBlockTime：阻塞时间
//返回值：pdTRUE，获取成功；pdFALSE，获取失败
//备注：
//关于阻塞时间：单位是系统的最小节拍。设置为0，如果队空（无效）则直接跳过；设置为portMAX_DELAY，如果队空，任务则进入延时列表（阻塞）。
//此函数的本质是调用XQueueGenericReceive()函数，对队列进行读取和减1操作。
//此函数可用于二值信号量、计数型信号量、互斥信号量。
//
//
//函数原型：BaseType_t xSemaphoreTakeFromISR(SemaphoreHandle_t xSemaphore,BaseType_t * pxHigherPriorityTaskWoken)
//参数：
//xSemaphore：要获取的信号量句柄
//pxHigherPriorityTaskWoken：标记退出此函数后是否进行任务切换。当此值为pdTRUE的时候，在退出中断服务函数之前一定要进行一次任务切换。
//返回值：pdTRUE，获取信号量成功；pdFALSE，获取信号量失败
//备注：
//该函数用于中断中获取信号量。
//本质是调用中断级出队函数xQueueReceiveFromISR(),
//不产生阻塞，如果队空则直接跳出
//////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////互斥信号量的释放//////////////////////////////////////////
//函数原型：BaseType_t xSemaphoreGive(SemaphoreHandle_t xSemaphore)
//参数：
//xSemaphore:要释放的信号量句柄
//返回值：pdPASS,释放信号量成功；errQUEUE_FULL,释放信号量失败
//备注：
//关于阻塞时间：该函数的阻塞时间由semGIVE_BLOCK_TIME决定，默认值为0，意味着如果队列满则直接跳过
//关键调用函数为xQueueGenericSend()，因此释放操作=入队操作
//该函数可用于二值信号量，计数型信号量，互斥信号量
//
//
//函数原型：BaseType_t xSemaphoreGiveFromISR(SemaphoreHandle_t xSemaphore,BaseType_t * pxHigherPriorityTaskWoken)
//参数：
//xSemaphore:要释放的信号量句柄
//pxHigherPriorityTaskWoken：标记退出此函数后是否进行任务切换。当此值为pdTRUE的时候，在退出中断服务函数之前一定要进行一次任务切换。
//返回值：pdPASS,释放信号量成功；errQUEUE_FULL,释放信号量失败
//备注：
//此函数用在中断中释放信号量
//此函数本质使用xQueueGiveFromISR()进行入队操作，此函数与xQueueGenericSendFromISR()类似。
//本函数不能用于互斥信号量的释放操作
//不会产生阻塞，如果队满则直接跳过
//////////////////////////////////////////////////////////////////////////////

//开始任务任务函数
void start_task(void *pvParameters)
{
	taskENTER_CRITICAL();           //进入临界区

	//动态方法创建互斥信号量
	MutexSemaphore = xSemaphoreCreateMutex();
	
	//创建测试任务
	xTaskCreate((TaskFunction_t )test,     	
							(const char*    )"test task",   	
							(uint16_t       )test_STK_SIZE, 
							(void*          )NULL,				
							(UBaseType_t    )test_TASK_PRIO,	
							(TaskHandle_t*  )&testTask_Handler);  

	//创建低优先级任务
	xTaskCreate((TaskFunction_t )low_task,     	
							(const char*    )"low p task",   	
							(uint16_t       )LOW_TASK_STK_SIZE, 
							(void*          )NULL,				
							(UBaseType_t    )LOW_TASK_PRIO,	
							(TaskHandle_t*  )&LOW_TASK_Handler);
							
	//创建中等优先级任务
	xTaskCreate((TaskFunction_t )middle_task,     	
							(const char*    )"middle p task",   	
							(uint16_t       )MIDDLE_TASK_STK_SIZE, 
							(void*          )NULL,				
							(UBaseType_t    )MIDDLE_TASK_PRIO,	
							(TaskHandle_t*  )&MIDDLE_TASK_Handler);
							
	//创建高优先级任务
	xTaskCreate((TaskFunction_t )high_task,     	
							(const char*    )"high p task",   	
							(uint16_t       )HIGH_TASK_STK_SIZE, 
							(void*          )NULL,				
							(UBaseType_t    )HIGH_TASK_PRIO,	
							(TaskHandle_t*  )&HIGH_TASK_Handler);
							
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

//低优先级任务：系统调度便获取信号量，比高优先级任务早
void low_task(void * pvParameters)
{
	//vTaskDelete(NULL);
	static u32 times;
	
	for(;;)
	{
		xSemaphoreTake(MutexSemaphore,portMAX_DELAY);		//获取信号量，如果无效则阻塞
		printf("low task Running!\r\n");
		
		for(times = 0;times<20000000;times++)				//模拟低优先级任务占用信号量
		{
			taskYIELD();															//运行期间内可以被更高优先级任务抢占
		}
		
		xSemaphoreGive(MutexSemaphore);									//释放信号量
		
		F_delay_ms(1000);	
	}
}

//中等优先级任务：模拟运行中但不占用信号量的任务
void middle_task(void * pvParameters)
{
	//vTaskDelete(NULL);
	for(;;)
	{
		printf("middle task Running!\r\n");
		F_delay_ms(1000);
	}
}

//高优先级任务:在获取信号量的时间上慢于低优先级任务
void high_task(void *pvParameters)
{
	//vTaskDelete(NULL);
	for(;;)
	{
		F_delay_ms(500);
		
		printf("high task Pend Sem\r\n");
		xSemaphoreTake(MutexSemaphore,portMAX_DELAY);	//获取信号量，如果无效则阻塞
		printf("high task Running!\r\n");
		xSemaphoreGive(MutexSemaphore);								//释放信号量
		
		F_delay_ms(500);
	}
	
}
	


//任务4
void task4(void *pvParameters)
{
	vTaskDelete(NULL);
}
