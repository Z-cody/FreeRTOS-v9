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

//�����ȼ�����
//�������ȼ�
#define LOW_TASK_PRIO		2
//�����ջ��С	
#define LOW_TASK_STK_SIZE 		128  
//������
TaskHandle_t LOW_TASK_Handler;
//������
void low_task(void *pvParameters);

//�����ȼ�����
//�������ȼ�
#define MIDDLE_TASK_PRIO		3
//�����ջ��С	
#define MIDDLE_TASK_STK_SIZE 		128  
//������
TaskHandle_t MIDDLE_TASK_Handler;
//������
void middle_task(void *pvParameters);

//�����ȼ�����
//�������ȼ�
#define HIGH_TASK_PRIO		4
//�����ջ��С	
#define HIGH_TASK_STK_SIZE 		128  
//������
TaskHandle_t HIGH_TASK_Handler;
//������
void high_task(void *pvParameters);

//����4
//�������ȼ�
#define TASK4_PRIO		2
//�����ջ��С	
#define task4_STK_SIZE 		128  
//������
TaskHandle_t TASK4_Handler;
//������
void task4(void *pvParameters);

//��ֵ�ź������
SemaphoreHandle_t BinarySemaphore;	//��ֵ�ź������

int main(void)
{
	//NVIC���ȼ����飺�ڶ��飻 ��ռ���ȼ���0~3�� ��Ӧ���ȼ���0~3�� 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	Delay_Init();
	Led_Init();
	Usart1_Init(115200);
	allKey_EXTI_Init();
	
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

	//��̬����������ֵ�ź���
	BinarySemaphore = xSemaphoreCreateBinary();
	//��ֵ�ź��������ɹ��Ժ�Ҫ���ͷ�һ�£����ź�����Ч
	if(BinarySemaphore != NULL)xSemaphoreGive(BinarySemaphore);
	
	//������������
	xTaskCreate((TaskFunction_t )test,     	
							(const char*    )"test task",   	
							(uint16_t       )test_STK_SIZE, 
							(void*          )NULL,				
							(UBaseType_t    )test_TASK_PRIO,	
							(TaskHandle_t*  )&testTask_Handler);  

	//���������ȼ�����
	xTaskCreate((TaskFunction_t )low_task,     	
							(const char*    )"low p task",   	
							(uint16_t       )LOW_TASK_STK_SIZE, 
							(void*          )NULL,				
							(UBaseType_t    )LOW_TASK_PRIO,	
							(TaskHandle_t*  )&LOW_TASK_Handler);
							
	//�����е����ȼ�����
	xTaskCreate((TaskFunction_t )middle_task,     	
							(const char*    )"middle p task",   	
							(uint16_t       )MIDDLE_TASK_STK_SIZE, 
							(void*          )NULL,				
							(UBaseType_t    )MIDDLE_TASK_PRIO,	
							(TaskHandle_t*  )&MIDDLE_TASK_Handler);
							
	//���������ȼ�����
	xTaskCreate((TaskFunction_t )high_task,     	
							(const char*    )"high p task",   	
							(uint16_t       )HIGH_TASK_STK_SIZE, 
							(void*          )NULL,				
							(UBaseType_t    )HIGH_TASK_PRIO,	
							(TaskHandle_t*  )&HIGH_TASK_Handler);
							
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

//�����ȼ�����ϵͳ���ȱ��ȡ�ź������ȸ����ȼ�������
void low_task(void * pvParameters)
{
	//vTaskDelete(NULL);
	static u32 times;
	
	for(;;)
	{
		xSemaphoreTake(BinarySemaphore,portMAX_DELAY);		//��ȡ�ź����������Ч������
		printf("low task Running!\r\n");
		
		for(times = 0;times<20000000;times++)				//ģ������ȼ�����ռ���ź���
		{
			taskYIELD();															//�����ڼ��ڿ��Ա��������ȼ�������ռ
		}
		
		xSemaphoreGive(BinarySemaphore);									//�ͷ��ź���
		
		F_delay_ms(1000);	
	}
}

//�е����ȼ�����ģ�������е���ռ���ź���������
void middle_task(void * pvParameters)
{
	//vTaskDelete(NULL);
	for(;;)
	{
		printf("middle task Running!\r\n");
		F_delay_ms(1000);
	}
}

//�����ȼ�����:�ڻ�ȡ�ź�����ʱ�������ڵ����ȼ�����
void high_task(void *pvParameters)
{
	//vTaskDelete(NULL);
	for(;;)
	{
		F_delay_ms(500);
		
		printf("high task Pend Sem\r\n");
		xSemaphoreTake(BinarySemaphore,portMAX_DELAY);	//��ȡ�ź����������Ч������
		printf("high task Running!\r\n");
		xSemaphoreGive(BinarySemaphore);								//�ͷ��ź���
		
		F_delay_ms(500);
	}
	
}
	


//����4
void task4(void *pvParameters)
{
	vTaskDelete(NULL);
}
