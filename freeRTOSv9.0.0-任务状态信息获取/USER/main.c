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

//任务4
//任务优先级
#define TASK4_PRIO		2
//任务堆栈大小	
#define task4_STK_SIZE 		128  
//任务句柄
TaskHandle_t TASK4_Handler;
//任务函数
void task4(void *pvParameters);

char InfoBuffer[1000];				//保存信息的数组

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
	u32 totalRunTime;
	UBaseType_t ArraySize,x;
	TaskStatus_t *StatusArray;
	
	
	//一：uxTaskGetSystemState的使用
	//函数原型：UBaseType_t uxTaskGetSystemState( TaskStatus_t * const pxTaskStatusArray, 
	//																						const UBaseType_t uxArraySize, 
	//																						uint32_t * const pulTotalRunTime )
	//需要configUSE_TRACE_FACILITY定义为1。
	//返回值：当前任务状态的个数（一个任务对应一个状态），与函数uxTaskGetNumberOfTasks()的返回值相同，如果uxArraySize太小，有可能为0。
	//pxTaskStatusArray：结构体TaskStatus_t的数组，每一个任务的TaskStatus_t都会拷贝一份，存储在数组中，因此数组定义必须足够大。
	//uxArraySize：数组pxTaskStatusArray的长度，即元素的个数。
	//pulTotalRunTime：系统总运行时间。
	//注意：此函数用于获取系统中所有任务的TaskStatus_t，里面存储着任务的基本信息，此函数比较耗时，最好在专用任务中调用
	printf("/********第一步：函数uxTaskGetSystemState()的使用**********/\r\n");
	//获取当前任务数量
	ArraySize = uxTaskGetNumberOfTasks();
	//为任务信息结构体申请内存
	StatusArray = pvPortMalloc(sizeof(TaskStatus_t)*ArraySize);
	
	if(StatusArray != NULL)
	{
		ArraySize = uxTaskGetSystemState((TaskStatus_t*   )StatusArray,		//存储任务信息数组
																		 (UBaseType_t     )ArraySize,			//存储任务信息数组长度
																		 (uint32_t*       )&totalRunTime);//系统总运行时间
		
		for(x=0;x<ArraySize;x++)
		{
			printf("TaskName:%s\t\t\r\nTaskNumber:%d\r\nCurrenState:%d\r\nPriority:%d\r\nRunTime:%d\r\n\r\n",
								StatusArray[x].pcTaskName,
								StatusArray[x].xTaskNumber,
								StatusArray[x].eCurrentState,
								StatusArray[x].uxCurrentPriority,
								StatusArray[x].ulRunTimeCounter);
		}
	}
	
	//释放内存
	vPortFree(StatusArray);
	printf("/**************************结束***************************/\r\n\r\n\r\n");
	
	//二：vTaskGetInfo()的使用
	//函数原型：void vTaskGetInfo( TaskHandle_t xTask, 
	//														 TaskStatus_t *pxTaskStatus, 
	//														 BaseType_t xGetFreeStackSpace, 
	//													   eTaskState eState )
	//需要configUSE_TRACE_FACILITY定义为1。
	//返回值：无
	//xTask：需要调查的任务的任务句柄。
	//pxTaskStatus：指向被调查任务的TaskStatus_t的指针。
	//xGetFreeStackSpace：任务运行以来任务堆栈剩余的历史最小值。此值调查需要耗费时间，因此可以定义为pdFALSE跳过这个步骤，反之定义为pdTRUE。
	//eState：任务状态。这个值可以由用户自己赋值，这样函数执行时便会跳过任务状态获取，减少运行时间，反之也可以定义为eInvalid，这样函数就会执行任务状态获取
	//注意：这个函数是通过任务的句柄来获取任务的TaskStatus_t，这个函数的特点是快速便捷，可以根据需求减少运行时间，可以适当在任务中调动
	TaskHandle_t TaskHandle;
	TaskStatus_t TaskStatus;
	
	printf("/************第二步：函数vTaskGetInfo()的使用**************/\r\n");
	TaskHandle = xTaskGetHandle("start_task");				//根据pcTaskName获取任务句柄
	
	vTaskGetInfo((TaskHandle_t	)TaskHandle,					//需要获取信息任务句柄
							 (TaskStatus_t*	)&TaskStatus,					//存储任务信息的任务结构体
							 (BaseType_t		)pdTRUE,							//允许统计任务堆栈历史最小剩余大小
							 (eTaskState		)eInvalid);						//函数自己获取任务的运行状态
	
	//打印任务信息
	printf("任务名:                %s\r\n",TaskStatus.pcTaskName);
	printf("任务编号:              %d\r\n",(int)TaskStatus.xTaskNumber);
	printf("任务壮态:              %d\r\n",TaskStatus.eCurrentState);
	printf("任务当前优先级:        %d\r\n",(int)TaskStatus.uxCurrentPriority);
	printf("任务基优先级:          %d\r\n",(int)TaskStatus.uxBasePriority);
	printf("任务堆栈基地址:        %#x\r\n",(int)TaskStatus.pxStackBase);
	printf("任务堆栈历史剩余最小值:%d\r\n",TaskStatus.usStackHighWaterMark);
	printf("/**************************结束***************************/\r\n\r\n\r\n");
	
	//三：eTaskGetState()的使用
	//函数原型：eTaskState eTaskGetState( TaskHandle_t xTask )
	//INCLUDE_eTaskGetState必须为1
	//返回值：eTaskState枚举类型
	//xTask：需要调查的任务的任务句柄
	//注意：此函数通过任务句柄直接获取任务的运行状态，返回值是eTaskState枚举类型，因此返回值是0-5。
	eTaskState TaskState;
	TaskHandle = xTaskGetHandle("the task3");
	TaskState = eTaskGetState(TaskHandle);
	printf("/***********第三步：函数eTaskGetState()的使用*************/\r\n");
	switch(TaskState)
	{
		case eRunning:
			printf("任务壮态值:%d,对应的壮态为:eRunning\r\n",TaskState);
			break;
		case eReady:
			printf("任务壮态值:%d,对应的壮态为:eReady\r\n",TaskState);
			break;
		case eBlocked:
			printf("任务壮态值:%d,对应的壮态为:eBlocked\r\n",TaskState);
			break;
		case eSuspended:
			printf("任务壮态值:%d,对应的壮态为:eSuspended\r\n",TaskState);
			break;
		case eDeleted:
			printf("任务壮态值:%d,对应的壮态为:eDeleted\r\n",TaskState);
			break;
		case eInvalid:
			printf("任务壮态值:%d,对应的壮态为:eInvalid\r\n",TaskState);
			break;
		default:break;
	}
	printf("/**************************结束**************************/\r\n\r\n\r\n");
	
	//四：vTaskList()的使用
	//函数原型：void vTaskList( char * pcWriteBuffer )
	//返回值：无
	//pcWriteBuffer：字符串形式存储所有任务的任务信息
	//注意：这个任务会获取当前系统所以任务的任务信息，并且与字符串的形式绘制成表格，包含信息：Name（任务名称），
	//State（任务状态），Priority（任务优先级），Stack（任务堆栈历史剩余最小值），Num（任务编号）
	printf("/*************第四步：函数vTaskList()的使用*************/\r\n");
	vTaskList(InfoBuffer);							//获取所有任务的信息
	printf("Name\t\t-State\tPriority\tStack\tNum\r\n");
	printf("*******************************************\r\n");
	printf("%s\r\n",InfoBuffer);					//通过串口打印所有任务的信息
	printf("/**************************结束**************************/\r\n\r\n\r\n");
	
	for(;;)
	{
		F_delay_s(1);
	}
	
}

//任务4
void task4(void *pvParameters)
{
	
}
