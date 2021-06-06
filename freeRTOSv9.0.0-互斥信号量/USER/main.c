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

//�����ź������
SemaphoreHandle_t MutexSemaphore;	//�����ź������

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
//�ͷţ�+1������ ����>  �ź�����Ч/������
//��ȡ��-1������ ����>  �ź�����Ч/���п�
//
//
///////////////////�����ź���//////////////////////
//1.�����ź�������ӵ�����ȼ��̳еĶ�ֵ�ź����������ڻ�����ʡ�
//2.��һ�������ź�����һ�������������ȡ�󣬴�ʱһ�������ȼ�����Ҳ����
//��ȡ��������ź����ͻᱻ�������������ȼ�����ͻὫ�����ȼ���������ȼ����������Լ���ͬ���ȼ��ϡ�������̾������ȼ��̳�
//3.�����ź���������ȫ���������ȼ���ת���⣬���Ǿ����ܼ���
//4.�����ź������������жϷ�����
//
//
//////////////////////////////////////�����ź�����������///////////////////////////////////
//����ԭ�ͣ�SemaphoreHandle_t xSemaphoreCreateMutex( void );
//��������
//����ֵ��
//NULL������ʧ��
//�����ź�������������ɹ�
//��ע����̬�����ڴ淽��������
//�˺��������ǵ���xQueueCreateMutex�����������С�
//�����ɹ����ź�������Ч��
//
//
//����ԭ�ͣ�SemaphoreHandle_t xSemaphoreCreateMutexStatic( StaticSemaphore_t *pxMutexBuffer );
//������
//pxMutexBuffer ���˲�������ָ��һ��StaticSemaphore_t���ͽṹ����
//����ֵ��
//NULL������ʧ��
//�ź�������������ɹ�
//��ע����̬���������ڴ洴����
//�˺��������ǵ���xQueueCreateMutexStatic�����������С�
//�������ź�������Ч�ġ�
//
///////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////�����ź����Ļ�ȡ////////////////////////////////////////
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

///////////////////////////////////�����ź������ͷ�//////////////////////////////////////////
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

	//��̬�������������ź���
	MutexSemaphore = xSemaphoreCreateMutex();
	
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
		xSemaphoreTake(MutexSemaphore,portMAX_DELAY);		//��ȡ�ź����������Ч������
		printf("low task Running!\r\n");
		
		for(times = 0;times<20000000;times++)				//ģ������ȼ�����ռ���ź���
		{
			taskYIELD();															//�����ڼ��ڿ��Ա��������ȼ�������ռ
		}
		
		xSemaphoreGive(MutexSemaphore);									//�ͷ��ź���
		
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
		xSemaphoreTake(MutexSemaphore,portMAX_DELAY);	//��ȡ�ź����������Ч������
		printf("high task Running!\r\n");
		xSemaphoreGive(MutexSemaphore);								//�ͷ��ź���
		
		F_delay_ms(500);
	}
	
}
	


//����4
void task4(void *pvParameters)
{
	vTaskDelete(NULL);
}
