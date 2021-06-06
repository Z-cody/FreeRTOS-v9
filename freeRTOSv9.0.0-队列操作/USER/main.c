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

#define MESSAGE_Q_NUM	1				//消息队列的队列项数量
QueueHandle_t Key_Queue;			//消息队列句柄

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

	//动态方法创建消息队列
	Key_Queue = xQueueCreate(MESSAGE_Q_NUM,sizeof(u8));
	
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

//任务3
void task3(void *pvParameters)
{
	//vTaskDelete(NULL);
	u8 key,i=0;
	BaseType_t err;
	while(1)
	{
		key=KEY_Scan(0);            							//扫描按键
		if((Key_Queue!=NULL)&&(key))   						//消息队列Key_Queue创建成功,并且按键被按下
		{
				err=xQueueSend(Key_Queue,&key,10);		//发送按键值,等待时间：10个节拍
				if(err==errQUEUE_FULL)   							
				{
						printf("队列Key_Queue已满，数据发送失败!\r\n");
				}
		}

		vTaskDelay(10);                           //延时10ms，也就是10个时钟节拍	
	}
}
	


//任务4
void task4(void *pvParameters)
{
	u8 num,key;
	while(1)
	{
        if(Key_Queue!=NULL)
        {
            if(xQueueReceive(Key_Queue,&key,portMAX_DELAY))//请求消息Key_Queue，完全阻塞
            {
                switch(key)
                {
                    case KEY0_PRESD:		
                        printf("按键1被按下\r\n");
                        break;
                    case KEY1_PRESD:		
                        printf("按键2被按下\r\n");
                        break;
                }
            }
        } 
		vTaskDelay(10);      //延时10ms，也就是10个时钟节拍	
	}
}
