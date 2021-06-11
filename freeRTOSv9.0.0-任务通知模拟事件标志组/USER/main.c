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

#define EVENTBIT_0	(1<<0)				//�¼�λ
#define EVENTBIT_1	(1<<1)
#define EVENTBIT_2	(1<<2)
#define EVENTBIT_ALL	(EVENTBIT_0|EVENTBIT_1|EVENTBIT_2)

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

//����3������ֵ֪ͨ����
void task3(void *pvParameters)
{
	//vTaskDelete(NULL);
	BaseType_t err;
	u32 TaskNotifyValue;
	u8 eventvalue;
	static u8 event0flag,event1flag,event2flag;
	
	for(;;)
	{
		err = xTaskNotifyWait((uint32_t 	)0x00,							//����˺���ʱ����ձ������ֵ֪ͨ
													(uint32_t 	)0xffffffff,				//�˳��˺���ʱ��ձ������ֵ֪ͨ
													(uint32_t*  )&TaskNotifyValue,	//������յ���ֵ֪ͨ
													(TickType_t	)portMAX_DELAY);		//�ڽ��յ�֪֮ͨǰ����
		
		printf("task3:���յ�ֵ֪ͨ%d\r\n",TaskNotifyValue);
		if(err == pdFALSE)
		{
			printf("task3:֪ͨ����ʧ��\r\n");
		}
		else{								//��ȡֵ֪ͨ�ɹ�
			if((TaskNotifyValue & EVENTBIT_0) != 0)					//�¼�0
			{
				event0flag = 1;
			}
			else if((TaskNotifyValue & EVENTBIT_1) != 0)		//�¼�1
			{
				event1flag = 1;
			}
			else if((TaskNotifyValue & EVENTBIT_2) != 0)		//�¼�2
			{
				event2flag = 1;
			}
			eventvalue = event0flag|(event1flag<<1)|(event2flag<<2);
			
			printf("task3:����ֵ֪ͨ%d\r\n",eventvalue);
		}

	}
	
}
	


//����4������ֵ֪ͨ����
void task4(void *pvParameters)
{
	//vTaskDelete(NULL);
	BaseType_t err;
	u8 key;
	
	while(1)
	{
		
		if(TASK3_Handler != NULL)
		{
			key = KEY_Scan(0);
			switch(key)
			{
				case KEY0_PRESD:
					err = xTaskNotify( (TaskHandle_t	) TASK3_Handler,
											 (uint32_t			) EVENTBIT_1,
											 (eNotifyAction	) eSetBits);
					break;
				case KEY1_PRESD:
					err = xTaskNotify( (TaskHandle_t	) TASK3_Handler,
											 (uint32_t			) EVENTBIT_2,
											 (eNotifyAction	) eSetBits);
					break;
			}
			
			if(err == pdFALSE)
				printf("task4:����֪ͨ����ʧ��\r\n");

		}
		
		F_delay_ms(10);
	}
}
