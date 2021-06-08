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
                (const char*    )"this is a test task",   	
                (uint16_t       )test_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )test_TASK_PRIO,	
                (TaskHandle_t*  )&testTask_Handler);  

		//创建任务1
    xTaskCreate((TaskFunction_t )task1,     	
                (const char*    )"this is the task1",   	
                (uint16_t       )task1_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )TASK1_PRIO,	
                (TaskHandle_t*  )&TASK1_Handler);
								
		//创建任务2
    xTaskCreate((TaskFunction_t )task2,     	
                (const char*    )"this is the task2",   	
                (uint16_t       )task2_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )TASK2_PRIO,	
                (TaskHandle_t*  )&TASK2_Handler);
								
		//创建任务3
    xTaskCreate((TaskFunction_t )task3,     	
                (const char*    )"this is the task3",   	
                (uint16_t       )task3_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )TASK3_PRIO,	
                (TaskHandle_t*  )&TASK3_Handler);
     
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}

//任务1
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

void task3(void *pvParamters)
{
	//vTaskDelete(NULL);
	u8 key;
	while(1)
	{
		key = KEY_Scan(0);
		//printf("key =%d\r\n",key);
		switch(key)
		{
			case KEY0_PRESD:
				vTaskSuspend(TASK1_Handler);
				break;
			case KEY1_PRESD:
				vTaskResume(TASK1_Handler);
				break;
		
		}
		
		F_delay_ms(100);
	
	}
	printf("here!!\r\n");
//	for(;;)
//	{
//		if(KEY0 == 1)
//		{
//			printf("KEY0 off\r\n");
//		}
//		if(KEY0 == 0)
//		{
//			printf("KEY0 on\r\n");
//		}
//		
//		F_delay_s(1);
//	}

}
