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

//�������ź������
SemaphoreHandle_t CountSemaphore;	//�������ź���

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

//FreeRTOS���ź���:SemaphoreHandle_t
//�ͷţ�+1������ ����>  ��ֵ�ź�����Ч/������
//��ȡ��-1������ ����>  ��ֵ�ź�����Ч/���п�
//
//
//
//////////////////////////////////////��ֵ���ź�����������///////////////////////////////////
//����ԭ�ͣ�SemaphoreHandle_t xSemaphoreCreateCounting( UBaseType_t uxMaxCount, UBaseType_t uxInitialCount );
//������
//uxMaxCount:������ֵ��������ֵ�������ֵ������ͷŲ�������ʧ��
//uxInitialCount������ֵ��ʼֵ
//����ֵ��
//NULL:����ʧ��
//�������ź�������������ɹ�
//��ע����̬�����ڴ淽���������˺��������ǵ���xQueueCreateCountingSemaphore()�������С�
//
//
//
//����ԭ�ͣ�SemaphoreHandle_t xSemaphoreCreateCountingStatic( UBaseType_t uxMaxCount, 
//																														UBaseType_t uxInitialCount, 
//																														StaticSemaphore_t *pxSemaphoreBuffer );
//������
//uxMaxCount:������ֵ��������ֵ�������ֵ������ͷŲ�������ʧ��
//uxInitialCount������ֵ��ʼֵ
//pxSemaphoreBuffer��ָ��һ��StaticSemaphore_t�ṹ�������ָ��
//����ֵ��
//NULL:����ʧ��
//�������ź�������������ɹ�
//��ע����̬�����������˺��������ǵ���xQueueCreateCountingSemaphoreStatic()�������С�
///////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////�������ź����Ļ�ȡ////////////////////////////////////////
//����ԭ�ͣ�BaseType_t xSemaphoreTake(SemaphoreHandle_t xSemaphore,TickType_t xBlockTime)
//������
//xSemaphore��Ҫ��ȡ���ź������
//xBlockTime������ʱ��
//����ֵ��pdTRUE����ȡ�ɹ���pdFALSE����ȡʧ��
//��ע��
//��������ʱ�䣺��λ��ϵͳ����С���ġ�����Ϊ0������ӿգ���Ч����ֱ������������ΪportMAX_DELAY������ӿգ������������ʱ�б���������
//�˺����ı����ǵ���XQueueGenericReceive()�������Զ��н��ж�ȡ�ͼ�1������
//�˺��������ڶ�ֵ�ź������������ź����������ź�����
//
//
//����ԭ�ͣ�BaseType_t xSemaphoreTakeFromISR(SemaphoreHandle_t xSemaphore,BaseType_t * pxHigherPriorityTaskWoken)
//������
//xSemaphore��Ҫ��ȡ���ź������
//pxHigherPriorityTaskWoken������˳��˺������Ƿ���������л�������ֵΪpdTRUE��ʱ�����˳��жϷ�����֮ǰһ��Ҫ����һ�������л���
//����ֵ��pdTRUE����ȡ�ź����ɹ���pdFALSE����ȡ�ź���ʧ��
//��ע��
//�ú��������ж��л�ȡ�ź�����
//�����ǵ����жϼ����Ӻ���xQueueReceiveFromISR(),
//����������������ӿ���ֱ������
//////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////�������ź������ͷ�//////////////////////////////////////////
//����ԭ�ͣ�BaseType_t xSemaphoreGive(SemaphoreHandle_t xSemaphore)
//������
//xSemaphore:Ҫ�ͷŵ��ź������
//����ֵ��pdPASS,�ͷ��ź����ɹ���errQUEUE_FULL,�ͷ��ź���ʧ��
//��ע��
//��������ʱ�䣺�ú���������ʱ����semGIVE_BLOCK_TIME������Ĭ��ֵΪ0����ζ�������������ֱ������
//�ؼ����ú���ΪxQueueGenericSend()������ͷŲ���=��Ӳ���
//�ú��������ڶ�ֵ�ź������������ź����������ź���
//
//
//����ԭ�ͣ�BaseType_t xSemaphoreGiveFromISR(SemaphoreHandle_t xSemaphore,BaseType_t * pxHigherPriorityTaskWoken)
//������
//xSemaphore:Ҫ�ͷŵ��ź������
//pxHigherPriorityTaskWoken������˳��˺������Ƿ���������л�������ֵΪpdTRUE��ʱ�����˳��жϷ�����֮ǰһ��Ҫ����һ�������л���
//����ֵ��pdPASS,�ͷ��ź����ɹ���errQUEUE_FULL,�ͷ��ź���ʧ��
//��ע��
//�˺��������ж����ͷ��ź���
//�˺�������ʹ��xQueueGiveFromISR()������Ӳ������˺�����xQueueGenericSendFromISR()���ơ�
//�������������ڻ����ź������ͷŲ���
//����������������������ֱ������
//////////////////////////////////////////////////////////////////////////////

//��ʼ����������
void start_task(void *pvParameters)
{
	taskENTER_CRITICAL();           //�����ٽ���

	//��̬���������������ź���
	CountSemaphore = xSemaphoreCreateCounting(255,0);	//������ֵ��255  ��ʼ����ֵ��0
	
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

//����3:�ͷż������ź���
void task3(void *pvParameters)
{
	//vTaskDelete(NULL);
	BaseType_t err = pdFALSE;
	u8 semavalue;
	u8 key;
	for(;;)
	{
		key = KEY_Scan(0);					//ɨ�谴��
		if(CountSemaphore != NULL)
		{
			switch(key)
			{
				case KEY0_PRESD:
					err = xSemaphoreGive(CountSemaphore);	//�ͷż������ź���
					if(err == pdFALSE)
					{
						printf("�ź����ͷ�ʧ��!!!\r\n");
					}
					semavalue = uxSemaphoreGetCount(CountSemaphore);
					printf("����ֵ�ͷź�:%d\r\n",semavalue);
					break;
			}
		}
		F_delay_ms(10);
	}
	
}
	


//����4:��ȡ�������ź���
void task4(void *pvParameters)
{
	//vTaskDelete(NULL);
  u8 semavalue;
	for(;;)
	{
		xSemaphoreTake(CountSemaphore,portMAX_DELAY);			//��ȡ�������ź������������ֵΪ0��������
		semavalue = uxSemaphoreGetCount(CountSemaphore);	//ȡ���ź�������ֵ
		printf("����ֵ��ȡ��:%d\r\n",semavalue);
		F_delay_ms(10);
	}
}
