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

//定义一个列表
List_t TestList;
//定义三个列表项
ListItem_t ListItem1;
ListItem_t ListItem2;
ListItem_t ListItem3;


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
void task3(void *pvParamters)
{
	//vTaskDelete(NULL);
	//一：初始化列表和列表项
	vListInitialise(&TestList);
	vListInitialiseItem(&ListItem1);
	vListInitialiseItem(&ListItem2);
	vListInitialiseItem(&ListItem3);
	
	//列表项赋值
	ListItem1.xItemValue = 40;
	ListItem2.xItemValue = 60;
	ListItem3.xItemValue = 50;
	
	//二：打印列表和列表项的地址
	printf("/*******************列表和列表项地址*******************/\r\n");
	printf("项目                              地址				          \r\n");
	printf("TestList                          %#x					          \r\n",(int)&TestList);
	printf("TestList->pxIndex                 %#x				           	\r\n",(int)TestList.pxIndex);
	printf("TestList->xListEnd                %#x                   \r\n",(int)(&TestList.xListEnd));
	printf("ListItem1                         %#x                   \r\n",(int)(&ListItem1));
	printf("ListItem2                         %#x                   \r\n",(int)(&ListItem2));
	printf("ListItem3                         %#x                   \r\n",(int)(&ListItem3));
	printf("/************************结束**************************/\r\n");
	printf("按下S1进行列表项插入ListItem1\r\n\r\n\r\n");
	while(KEY_Scan(0) != KEY0_PRESD)F_delay_ms(10);								//等待按键S1被那按下
	
	//三：添加列表项1，并打印所有列表项pxNext和pxPrevious地址
	vListInsert(&TestList,&ListItem1);														//插入列表项1
	printf("/******************添加列表项ListItem1*****************/\r\n");
	printf("项目                              地址				          \r\n");
	printf("TestList->xListEnd->pxNext        %#x                   \r\n",(int)TestList.xListEnd.pxNext);
	printf("ListItem1->pxNext                 %#x                   \r\n",(int)ListItem1.pxNext);
	printf("/*******************前后向连接分割线********************/\r\n");
	printf("TestList->xListEnd->pxPrevious    %#x                   \r\n",(int)TestList.xListEnd.pxPrevious);
	printf("ListItem1->pxPrevious             %#x                   \r\n",(int)ListItem1.pxPrevious);
	printf("/************************结束**************************/\r\n");
	printf("按下S1进行列表项插入ListItem2\r\n\r\n\r\n");
	while(KEY_Scan(0) != KEY0_PRESD)F_delay_ms(10);								//等待按键S1被那按下
	
	//四：添加列表项2，并打印所有列表项pxNext和pxPrevious地址
	vListInsert(&TestList,&ListItem2);														//插入列表项1
	printf("/******************添加列表项ListItem2*****************/\r\n");
	printf("项目                              地址				          \r\n");
	printf("TestList->xListEnd->pxNext        %#x                   \r\n",(int)TestList.xListEnd.pxNext);
	printf("ListItem1->pxNext                 %#x                   \r\n",(int)ListItem1.pxNext);
	printf("ListItem2->pxNext                 %#x                   \r\n",(int)ListItem2.pxNext);
	printf("/*******************前后向连接分割线********************/\r\n");
	printf("TestList->xListEnd->pxPrevious    %#x                   \r\n",(int)TestList.xListEnd.pxPrevious);
	printf("ListItem1->pxPrevious             %#x                   \r\n",(int)ListItem1.pxPrevious);
	printf("ListItem2->pxPrevious             %#x                   \r\n",(int)ListItem2.pxPrevious);
	printf("/************************结束**************************/\r\n");
	printf("按下S1进行列表项插入ListItem3\r\n\r\n\r\n");
	while(KEY_Scan(0) != KEY0_PRESD)F_delay_ms(10);								//等待按键S1被那按下
	
	//五：添加列表项3，并打印所有列表项pxNext和pxPrevious地址
	vListInsert(&TestList,&ListItem3);														//插入列表项1
	printf("/******************添加列表项ListItem3*****************/\r\n");
	printf("项目                              地址				          \r\n");
	printf("TestList->xListEnd->pxNext        %#x                   \r\n",(int)TestList.xListEnd.pxNext);
	printf("ListItem1->pxNext                 %#x                   \r\n",(int)ListItem1.pxNext);
	printf("ListItem2->pxNext                 %#x                   \r\n",(int)ListItem2.pxNext);
	printf("ListItem3->pxNext                 %#x                   \r\n",(int)ListItem3.pxNext);
	printf("/*******************前后向连接分割线********************/\r\n");
	printf("TestList->xListEnd->pxPrevious    %#x                   \r\n",(int)TestList.xListEnd.pxPrevious);
	printf("ListItem1->pxPrevious             %#x                   \r\n",(int)ListItem1.pxPrevious);
	printf("ListItem2->pxPrevious             %#x                   \r\n",(int)ListItem2.pxPrevious);
  printf("ListItem3->pxPrevious             %#x                   \r\n",(int)ListItem3.pxPrevious);
	printf("/************************结束**************************/\r\n");
	printf("按下S1删除列表项ListItem2\r\n\r\n\r\n");
	while(KEY_Scan(0) != KEY0_PRESD)F_delay_ms(10);								//等待按键S1被那按下
	
	//六：删除列表项2
	uxListRemove(&ListItem2);
	printf("/******************删除列表项ListItem2*****************/\r\n");
	printf("项目                              地址				          \r\n");
	printf("TestList->xListEnd->pxNext        %#x                   \r\n",(int)TestList.xListEnd.pxNext);
	printf("ListItem1->pxNext                 %#x                   \r\n",(int)ListItem1.pxNext);
	printf("ListItem3->pxNext                 %#x                   \r\n",(int)ListItem3.pxNext);
	printf("/*******************前后向连接分割线********************/\r\n");
	printf("TestList->xListEnd->pxPrevious    %#x                   \r\n",(int)TestList.xListEnd.pxPrevious);
	printf("ListItem1->pxPrevious             %#x                   \r\n",(int)ListItem1.pxPrevious);
  printf("ListItem3->pxPrevious             %#x                   \r\n",(int)ListItem3.pxPrevious);
	printf("/************************结束**************************/\r\n");
	printf("按下S1在末尾添加列表项ListItem2\r\n\r\n\r\n");
	while(KEY_Scan(0) != KEY0_PRESD)F_delay_ms(10);								//等待按键S1被那按下
	
	//七：在末尾添加列表项2
	TestList.pxIndex = TestList.pxIndex->pxNext;		//使pxIndex指向后一项，也就是ListItem1
	vListInsertEnd(&TestList,&ListItem2);\
	printf("/******************在末尾添加列表项ListItem2*****************/\r\n");
	printf("项目                              地址				          \r\n");
	printf("TestList->xListEnd->pxNext        %#x                   \r\n",(int)TestList.xListEnd.pxNext);
	printf("ListItem2->pxNext                 %#x                   \r\n",(int)ListItem2.pxNext);
	printf("ListItem1->pxNext                 %#x                   \r\n",(int)ListItem1.pxNext);
	printf("ListItem3->pxNext                 %#x                   \r\n",(int)ListItem3.pxNext);
	printf("/*******************前后向连接分割线********************/\r\n");
	printf("TestList->xListEnd->pxPrevious    %#x                   \r\n",(int)TestList.xListEnd.pxPrevious);
	printf("ListItem2->pxPrevious             %#x                   \r\n",(int)ListItem2.pxPrevious);
	printf("ListItem1->pxPrevious             %#x                   \r\n",(int)ListItem1.pxPrevious);
  printf("ListItem3->pxPrevious             %#x                   \r\n",(int)ListItem3.pxPrevious);
	printf("/************************结束**************************/\r\n");
	
	for(;;)
	{
		F_delay_s(1);
	}
	
}
