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
//释放（+1操作） ——>  二值信号量有效/队列满
//获取（-1操作） ——>  二值信号量无效/队列空
//
//
//
//////////////////////////////////////二值信号量创建函数///////////////////////////////////
//函数原型：SemaphoreHandle_t xSemaphoreCreateBinary()；
//参数：无
//返回值：NULL，创建失败；二值信号量句柄，创建成功
//备注：动态方法创建。新版函数。创建的信号量是无效的。此函数本质是调用xQueueGenericCreate函数创建一个长度为1的队列。
//
//
//函数原型：void vSemaphoreCreateBinary(SemaphoreHandle_t xSemaphore);
//参数：
//xSemaphore：信号量句柄，创建成功；NULL，创建失败
//返回值：无
//备注：动态方法创建。旧版函数。创建的信号量是有效的。此函数本质是调用xQueueGenericCreate函数创建一个长度为1的队列。
//
//
//函数原型：SemaphoreHandle_t xSemaphoreCreateBinaryStatic(StaticSemaphore_t * pxSemaphoreBuffer)
//参数:
//pxSemaphoreBuffer：用来保存指向StaticSemaphore_t类型的指针，用来保存信号量结构体
//返回值：无
//备注：静态方法创建信号量。此函数本质是调用xQueueGenericCreate函数创建一个长度为1的队列
///////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////二值信号量的获取////////////////////////////////////////
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

///////////////////////////////////二值信号量的释放//////////////////////////////////////////
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
	vTaskDelete(NULL);
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

//任务3：接收任务
void task3(void *pvParameters)
{
	//vTaskDelete(NULL);
	u32 NotifyValue;
	for(;;)
	{
		NotifyValue = ulTaskNotifyTake(pdTRUE,portMAX_DELAY);		//获取任务通知，如果任务的ulNotifiedValue=0,则阻塞，任务通过后将ulNotifiedValue清零
		if(NotifyValue == 1)
		{
			printf("Task3 getNotice\r\n");
		}
		else
		{
			F_delay_ms(10);
		}
	}
	
}
	


//任务4
void task4(void *pvParameters)
{
	vTaskDelete(NULL);
}
