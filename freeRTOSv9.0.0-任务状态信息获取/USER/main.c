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

//���񴴽��������
//�������ȼ�
#define START_TASK_PRIO		1
//�����ջ��С	
#define START_STK_SIZE 		128  
//������
TaskHandle_t StartTask_Handler;
//������
void start_task(void *pvParameters);

//��������
//�������ȼ�
#define test_TASK_PRIO		2
//�����ջ��С	
#define test_STK_SIZE 		128  
//������
TaskHandle_t testTask_Handler;
//������
void test(void *pvParameters);

//����1
//�������ȼ�
#define TASK1_PRIO		2
//�����ջ��С	
#define task1_STK_SIZE 		128  
//������
TaskHandle_t TASK1_Handler;
//������
void task1(void *pvParameters);

//����2
//�������ȼ�
#define TASK2_PRIO		2
//�����ջ��С	
#define task2_STK_SIZE 		128  
//������
TaskHandle_t TASK2_Handler;
//������
void task2(void *pvParameters);

//����3
//�������ȼ�
#define TASK3_PRIO		2
//�����ջ��С	
#define task3_STK_SIZE 		128  
//������
TaskHandle_t TASK3_Handler;
//������
void task3(void *pvParameters);

//����4
//�������ȼ�
#define TASK4_PRIO		2
//�����ջ��С	
#define task4_STK_SIZE 		128  
//������
TaskHandle_t TASK4_Handler;
//������
void task4(void *pvParameters);

char InfoBuffer[1000];				//������Ϣ������

int main(void)
{
	//NVIC���ȼ����飺�ڶ��飻 ��ռ���ȼ���0~3�� ��Ӧ���ȼ���0~3�� 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	Delay_Init();
	Led_Init();
	Usart1_Init(115200);
	AllKeyInit();
	
	xTaskCreate((TaskFunction_t )start_task,            //������
							(const char*    )"start_task",          //��������
							(uint16_t       )START_STK_SIZE,        //�����ջ��С
							(void*          )NULL,                  //���ݸ��������Ĳ���
							(UBaseType_t    )START_TASK_PRIO,       //�������ȼ�
							(TaskHandle_t*  )&StartTask_Handler);   //������
							
  vTaskStartScheduler();          										//�����������
							
	//return 0;
}

//��ʼ����������
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //�����ٽ���
    //������������
    xTaskCreate((TaskFunction_t )test,     	
                (const char*    )"test task",   	
                (uint16_t       )test_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )test_TASK_PRIO,	
                (TaskHandle_t*  )&testTask_Handler);  

		//��������1
    xTaskCreate((TaskFunction_t )task1,     	
                (const char*    )"the task1",   	
                (uint16_t       )task1_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )TASK1_PRIO,	
                (TaskHandle_t*  )&TASK1_Handler);
								
		//��������2
    xTaskCreate((TaskFunction_t )task2,     	
                (const char*    )"the task2",   	
                (uint16_t       )task2_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )TASK2_PRIO,	
                (TaskHandle_t*  )&TASK2_Handler);
								
		//��������3
    xTaskCreate((TaskFunction_t )task3,     	
                (const char*    )"the task3",   	
                (uint16_t       )task3_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )TASK3_PRIO,	
                (TaskHandle_t*  )&TASK3_Handler);
								
		//��������4
    xTaskCreate((TaskFunction_t )task4,     	
                (const char*    )"the task4",   	
                (uint16_t       )task4_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )TASK4_PRIO,	
                (TaskHandle_t*  )&TASK4_Handler);
     
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}

//����test
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

//����1
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

//����2
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

//����3
void task3(void *pvParameters)
{
	//vTaskDelete(NULL);
	u32 totalRunTime;
	UBaseType_t ArraySize,x;
	TaskStatus_t *StatusArray;
	
	
	//һ��uxTaskGetSystemState��ʹ��
	//����ԭ�ͣ�UBaseType_t uxTaskGetSystemState( TaskStatus_t * const pxTaskStatusArray, 
	//																						const UBaseType_t uxArraySize, 
	//																						uint32_t * const pulTotalRunTime )
	//��ҪconfigUSE_TRACE_FACILITY����Ϊ1��
	//����ֵ����ǰ����״̬�ĸ�����һ�������Ӧһ��״̬�����뺯��uxTaskGetNumberOfTasks()�ķ���ֵ��ͬ�����uxArraySize̫С���п���Ϊ0��
	//pxTaskStatusArray���ṹ��TaskStatus_t�����飬ÿһ�������TaskStatus_t���´��һ�ݣ��洢�������У�������鶨������㹻��
	//uxArraySize������pxTaskStatusArray�ĳ��ȣ���Ԫ�صĸ�����
	//pulTotalRunTime��ϵͳ������ʱ�䡣
	//ע�⣺�˺������ڻ�ȡϵͳ�����������TaskStatus_t������洢������Ļ�����Ϣ���˺����ȽϺ�ʱ�������ר�������е���
	printf("/********��һ��������uxTaskGetSystemState()��ʹ��**********/\r\n");
	//��ȡ��ǰ��������
	ArraySize = uxTaskGetNumberOfTasks();
	//Ϊ������Ϣ�ṹ�������ڴ�
	StatusArray = pvPortMalloc(sizeof(TaskStatus_t)*ArraySize);
	
	if(StatusArray != NULL)
	{
		ArraySize = uxTaskGetSystemState((TaskStatus_t*   )StatusArray,		//�洢������Ϣ����
																		 (UBaseType_t     )ArraySize,			//�洢������Ϣ���鳤��
																		 (uint32_t*       )&totalRunTime);//ϵͳ������ʱ��
		
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
	
	//�ͷ��ڴ�
	vPortFree(StatusArray);
	printf("/**************************����***************************/\r\n\r\n\r\n");
	
	//����vTaskGetInfo()��ʹ��
	//����ԭ�ͣ�void vTaskGetInfo( TaskHandle_t xTask, 
	//														 TaskStatus_t *pxTaskStatus, 
	//														 BaseType_t xGetFreeStackSpace, 
	//													   eTaskState eState )
	//��ҪconfigUSE_TRACE_FACILITY����Ϊ1��
	//����ֵ����
	//xTask����Ҫ������������������
	//pxTaskStatus��ָ�򱻵��������TaskStatus_t��ָ�롣
	//xGetFreeStackSpace�������������������ջʣ�����ʷ��Сֵ����ֵ������Ҫ�ķ�ʱ�䣬��˿��Զ���ΪpdFALSE����������裬��֮����ΪpdTRUE��
	//eState������״̬�����ֵ�������û��Լ���ֵ����������ִ��ʱ�����������״̬��ȡ����������ʱ�䣬��֮Ҳ���Զ���ΪeInvalid�����������ͻ�ִ������״̬��ȡ
	//ע�⣺���������ͨ������ľ������ȡ�����TaskStatus_t������������ص��ǿ��ٱ�ݣ����Ը��������������ʱ�䣬�����ʵ��������е���
	TaskHandle_t TaskHandle;
	TaskStatus_t TaskStatus;
	
	printf("/************�ڶ���������vTaskGetInfo()��ʹ��**************/\r\n");
	TaskHandle = xTaskGetHandle("start_task");				//����pcTaskName��ȡ������
	
	vTaskGetInfo((TaskHandle_t	)TaskHandle,					//��Ҫ��ȡ��Ϣ������
							 (TaskStatus_t*	)&TaskStatus,					//�洢������Ϣ������ṹ��
							 (BaseType_t		)pdTRUE,							//����ͳ�������ջ��ʷ��Сʣ���С
							 (eTaskState		)eInvalid);						//�����Լ���ȡ���������״̬
	
	//��ӡ������Ϣ
	printf("������:                %s\r\n",TaskStatus.pcTaskName);
	printf("������:              %d\r\n",(int)TaskStatus.xTaskNumber);
	printf("����׳̬:              %d\r\n",TaskStatus.eCurrentState);
	printf("����ǰ���ȼ�:        %d\r\n",(int)TaskStatus.uxCurrentPriority);
	printf("��������ȼ�:          %d\r\n",(int)TaskStatus.uxBasePriority);
	printf("�����ջ����ַ:        %#x\r\n",(int)TaskStatus.pxStackBase);
	printf("�����ջ��ʷʣ����Сֵ:%d\r\n",TaskStatus.usStackHighWaterMark);
	printf("/**************************����***************************/\r\n\r\n\r\n");
	
	//����eTaskGetState()��ʹ��
	//����ԭ�ͣ�eTaskState eTaskGetState( TaskHandle_t xTask )
	//INCLUDE_eTaskGetState����Ϊ1
	//����ֵ��eTaskStateö������
	//xTask����Ҫ����������������
	//ע�⣺�˺���ͨ��������ֱ�ӻ�ȡ���������״̬������ֵ��eTaskStateö�����ͣ���˷���ֵ��0-5��
	eTaskState TaskState;
	TaskHandle = xTaskGetHandle("the task3");
	TaskState = eTaskGetState(TaskHandle);
	printf("/***********������������eTaskGetState()��ʹ��*************/\r\n");
	switch(TaskState)
	{
		case eRunning:
			printf("����׳ֵ̬:%d,��Ӧ��׳̬Ϊ:eRunning\r\n",TaskState);
			break;
		case eReady:
			printf("����׳ֵ̬:%d,��Ӧ��׳̬Ϊ:eReady\r\n",TaskState);
			break;
		case eBlocked:
			printf("����׳ֵ̬:%d,��Ӧ��׳̬Ϊ:eBlocked\r\n",TaskState);
			break;
		case eSuspended:
			printf("����׳ֵ̬:%d,��Ӧ��׳̬Ϊ:eSuspended\r\n",TaskState);
			break;
		case eDeleted:
			printf("����׳ֵ̬:%d,��Ӧ��׳̬Ϊ:eDeleted\r\n",TaskState);
			break;
		case eInvalid:
			printf("����׳ֵ̬:%d,��Ӧ��׳̬Ϊ:eInvalid\r\n",TaskState);
			break;
		default:break;
	}
	printf("/**************************����**************************/\r\n\r\n\r\n");
	
	//�ģ�vTaskList()��ʹ��
	//����ԭ�ͣ�void vTaskList( char * pcWriteBuffer )
	//����ֵ����
	//pcWriteBuffer���ַ�����ʽ�洢���������������Ϣ
	//ע�⣺���������ȡ��ǰϵͳ���������������Ϣ���������ַ�������ʽ���Ƴɱ�񣬰�����Ϣ��Name���������ƣ���
	//State������״̬����Priority���������ȼ�����Stack�������ջ��ʷʣ����Сֵ����Num�������ţ�
	printf("/*************���Ĳ�������vTaskList()��ʹ��*************/\r\n");
	vTaskList(InfoBuffer);							//��ȡ�����������Ϣ
	printf("Name\t\t-State\tPriority\tStack\tNum\r\n");
	printf("*******************************************\r\n");
	printf("%s\r\n",InfoBuffer);					//ͨ�����ڴ�ӡ�����������Ϣ
	printf("/**************************����**************************/\r\n\r\n\r\n");
	
	for(;;)
	{
		F_delay_s(1);
	}
	
}

//����4
void task4(void *pvParameters)
{
	
}
