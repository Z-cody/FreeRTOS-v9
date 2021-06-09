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
#include "event_groups.h"

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

//�������ȼ�
#define EVENTSETBIT_TASK_PRIO		2
//�����ջ��С	
#define EVENTSETBIT_STK_SIZE 		256  
//������
TaskHandle_t EVENTSETBIT_Handler;
//������
void eventsetbit_task(void *pvParameters);

//�������ȼ�
#define EVENTGROUP_TASK_PRIO		3
//�����ջ��С	
#define EVENTGROUP_STK_SIZE 		256  
//������
TaskHandle_t EVENTGROUP_Handler;
//������
void eventgroup_task(void *pvParameters);

//�������ȼ�
#define EVENTQUERY_TASK_PRIO		4
//�����ջ��С	
#define EVENTQUERY_STK_SIZE 		128  
//������
TaskHandle_t EVENTQUERY_Handler;
//������
void eventquery_task(void *pvParameters);

//����4
//�������ȼ�
#define TASK4_PRIO		2
//�����ջ��С	
#define task4_STK_SIZE 		128  
//������
TaskHandle_t TASK4_Handler;
//������
void task4(void *pvParameters);



///////////////////////////�¼���־��///////////////////////////////
//�¼�λ�洢����һ���޷��ŵ�EventBits_t���ͱ�����
//��configUSE_16_BIT_TICKSΪ0ʱ��EventBits_t��һ��32λ�������ͣ����ñ�־λ24λ��Ĭ�ϣ�
//��configUSE_16_BIT_TICKSΪ1ʱ��EventBits_t��һ��16λ�������ͣ����ñ�־λ8λ
/////////////////////////////////////////////////////////////////////
//
/**
*	�����¼���־λ����
*
*	��̬��������
*	����ԭ�ͣ�EventGroupHandle_t xEventGroupCreate( void )
*	��������
*	����ֵ��
*		NULL	���¼���־�鴴��ʧ��
*		����ֵ�������ɹ����¼���־����
*	��ע�������ɹ�֮���¼���־������洢���¼���־λȫ������
*
*
*	��̬��������
*	����ԭ�ͣ�EventGroupHandle_t xEventGroupCreateStatic( StaticEventGroup_t *pxEventGroupBuffer )
*	������
*	pxEventGroupBuffer:����ָ��һ��StaticEventGroup_t���͵ı��������������¼���־��ṹ��
*	����ֵ��
*		NULL	���¼���־�鴴��ʧ��
*		����ֵ�������ɹ����¼���־����
*	��ע�������ɹ�֮���¼���־������洢���¼���־λȫ������
*/

/**
*	�����¼���־λ����
*
*	��־λ��һ������
*	����ԭ�ͣ�EventBits_t xEventGroupSetBits( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet )
*	������
*		xEventGroup��Ҫ�������¼���־����
*		uxBitsToSet����Ҫ��һ���¼�λ
*	����ֵ��ָ���¼�λ��һ֮���¼���־��
*	��ע��
*
*
*	��־λ��һ����(�ж�)��
*	����ԭ�ͣ�BaseType_t xEventGroupSetBitsFromISR( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet, BaseType_t *pxHigherPriorityTaskWoken )
*	������
*		xEventGroup��Ҫ�������¼���־����
*		uxBitsToSet����Ҫ��һ���¼�λ
*		pxHigherPriorityTaskWoken������˳��˺������Ƿ���������л�������ֵΪpdTRUE��ʱ�����˳��жϷ�����֮ǰһ��Ҫ����һ�������л���
*	����ֵ��
*		pdPASS	:�¼�λ��1�ɹ�
*		pdFALSE	:�¼�λ��1ʧ��
*	��ע���˺�����xEventGroupSetBits���жϰ棬ע�ⲻ�᷵���¼���־��
*
*
*	��־λ���㺯����
*	����ԭ�ͣ�EventBits_t xEventGroupClearBits( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToClear )
*	������
*		xEventGroup��Ҫ�������¼���־����
*		uxBitsToClear����Ҫ��һ���¼�λ
*	����ֵ��ָ���¼�λ��һ֮���¼���־��
*	��ע��
*
*
*	��־λ��һ����(�ж�)��
*	����ԭ�ͣ�BaseType_t xEventGroupSetBitsFromISR( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet, BaseType_t *pxHigherPriorityTaskWoken )
*	������
*		xEventGroup��Ҫ�������¼���־����
*		uxBitsToSet����Ҫ��һ���¼�λ
*		pxHigherPriorityTaskWoken������˳��˺������Ƿ���������л�������ֵΪpdTRUE��ʱ�����˳��жϷ�����֮ǰһ��Ҫ����һ�������л���
*	����ֵ��ָ���¼�λ��һ֮���¼���־��
*	��ע��
*		pdPASS	:�¼�λ��1�ɹ�
*		pdFALSE	:�¼�λ��1ʧ��
*/

//
//
////////////////////////////////////////////////////////
EventGroupHandle_t EventGroupHandler;	//�¼���־����

#define EVENTBIT_0	(1<<0)				//�¼�λ
#define EVENTBIT_1	(1<<1)
#define EVENTBIT_2	(1<<2)
#define EVENTBIT_ALL	(EVENTBIT_0|EVENTBIT_1|EVENTBIT_2)
////////////////////////////////////////////////////////

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

	//�����¼���־��
	EventGroupHandler=xEventGroupCreate();	 //�����¼���־��
	
	//������������
	xTaskCreate((TaskFunction_t )test,     	
							(const char*    )"test task",   	
							(uint16_t       )test_STK_SIZE, 
							(void*          )NULL,				
							(UBaseType_t    )test_TASK_PRIO,	
							(TaskHandle_t*  )&testTask_Handler);  

	////����-�����¼�λ������
	xTaskCreate((TaskFunction_t )eventsetbit_task,     	
							(const char*    )"eventsetbit_task",   	
							(uint16_t       )EVENTSETBIT_STK_SIZE, 
							(void*          )NULL,				
							(UBaseType_t    )EVENTSETBIT_TASK_PRIO,	
							(TaskHandle_t*  )&EVENTSETBIT_Handler);
							
	//����-�¼���־�鴦������
	xTaskCreate((TaskFunction_t )eventgroup_task,     	
							(const char*    )"eventgroup_taskk",   	
							(uint16_t       )EVENTGROUP_STK_SIZE, 
							(void*          )NULL,				
							(UBaseType_t    )EVENTGROUP_TASK_PRIO,	
							(TaskHandle_t*  )&EVENTGROUP_Handler);
							
	//���������ȼ�����
	xTaskCreate((TaskFunction_t )eventquery_task,     	
							(const char*    )"eventquery_task",   	
							(uint16_t       )EVENTQUERY_STK_SIZE, 
							(void*          )NULL,				
							(UBaseType_t    )EVENTQUERY_TASK_PRIO,	
							(TaskHandle_t*  )&EVENTQUERY_Handler);
							
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

//�����¼�λ������
void eventsetbit_task(void *pvParameters)
{
	//vTaskDelete(NULL);
	u8 key;
	while(1)
	{
		if(EventGroupHandler!=NULL)
		{
			key=KEY_Scan(0);
			switch(key)
			{
				case KEY0_PRESD:
					xEventGroupSetBits(EventGroupHandler,EVENTBIT_1);	//����0x0010
					break;
				case KEY1_PRESD:
					xEventGroupSetBits(EventGroupHandler,EVENTBIT_2);	//����0x0100
					break;	
			}
		}
    vTaskDelay(10); //��ʱ10ms��Ҳ����10��ʱ�ӽ���
	}
}

//�¼���־�鴦������
void eventgroup_task(void * pvParameters)
{
	//vTaskDelete(NULL);
	EventBits_t EventValue;
	while(1)
	{

		if(EventGroupHandler!=NULL)
		{
			//�ȴ��¼����е���Ӧ�¼�λ
			EventValue=xEventGroupWaitBits((EventGroupHandle_t	)EventGroupHandler,		//���
																		 (EventBits_t					)EVENTBIT_1|EVENTBIT_2,				//�ȴ�0x0111
																		 (BaseType_t					)pdTRUE,							//�¼�λ����
																		 (BaseType_t					)pdTRUE,							//���ж�
																		 (TickType_t					)portMAX_DELAY);			//�����ȴ�
			printf("�¼����¼���־���ֵ:%d\r\n",EventValue);
			
		}
		else
		{
			vTaskDelay(10); //��ʱ10ms��Ҳ����10��ʱ�ӽ���
		}
	}
}

//�¼���ѯ����
void eventquery_task(void *pvParameters)
{	
	//vTaskDelete(NULL);
	EventBits_t NewValue,LastValue;
	while(1)
	{
		if(EventGroupHandler!=NULL)
		{
			NewValue=xEventGroupGetBits(EventGroupHandler);	//��ȡ�¼����
			if(NewValue!=LastValue)
			{
				LastValue=NewValue;
				printf("��ѯ���¼���־���ֵ:%d\r\n",NewValue);
			}
		}
		vTaskDelay(50); //��ʱ50ms��Ҳ����50��ʱ�ӽ���
	}
}
	


//����4
void task4(void *pvParameters)
{
	vTaskDelete(NULL);
	for(;;)
	{
		printf("KEY0 = %d\r\n",KEY0);
		printf("KEY1 = %d\r\n",KEY1);
		printf("KEY2 = %d\r\n",KEY2);
		printf("KEY3 = %d\r\n",KEY3);
		printf("\r\n\r\n");
		F_delay_ms(500);
	}
}
