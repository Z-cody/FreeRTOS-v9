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

//二值信号量句柄
SemaphoreHandle_t BinarySemaphore;	//二值信号量句柄

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


//开始任务任务函数
void start_task(void *pvParameters)
{
	taskENTER_CRITICAL();           //进入临界区

	//动态方法创建二值信号量
	BinarySemaphore = xSemaphoreCreateBinary();
	//二值信号量创建成功以后要先释放一下，是信号量有效
	if(BinarySemaphore != NULL)xSemaphoreGive(BinarySemaphore);
	
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
		xSemaphoreTake(BinarySemaphore,portMAX_DELAY);		//获取信号量，如果无效则阻塞
		printf("low task Running!\r\n");
		
		for(times = 0;times<20000000;times++)				//模拟低优先级任务占用信号量
		{
			taskYIELD();															//运行期间内可以被更高优先级任务抢占
		}
		
		xSemaphoreGive(BinarySemaphore);									//释放信号量
		
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
		xSemaphoreTake(BinarySemaphore,portMAX_DELAY);	//获取信号量，如果无效则阻塞
		printf("high task Running!\r\n");
		xSemaphoreGive(BinarySemaphore);								//释放信号量
		
		F_delay_ms(500);
	}
	
}
	


//任务4
void task4(void *pvParameters)
{
	vTaskDelete(NULL);
}
