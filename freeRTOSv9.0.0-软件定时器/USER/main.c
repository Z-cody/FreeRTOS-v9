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
#include "timers.h"

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

//��ʱ����������
//�������ȼ�
#define TIMERCONTROL_TASK_PRIO	2
//�����ջ��С	
#define TIMERCONTROL_STK_SIZE 	256  
//������
TaskHandle_t TimerControlTask_Handler;
//������
void timercontrol_task(void *pvParameters);


TimerHandle_t 	AutoReloadTimer_Handle;			//���ڶ�ʱ�����
TimerHandle_t		OneShotTimer_Handle;				//���ζ�ʱ�����

void AutoReloadCallback(TimerHandle_t xTimer); 	//���ڶ�ʱ���ص�����
void OneShotCallback(TimerHandle_t xTimer);		//���ζ�ʱ���ص�����

////////////////FreeRTOS�������ʱ��//////////////////////
//1.�����ʱ���Ļ��������������Եش���ĳЩ������Щ��������ص�����
//2.�����ʱ���ľ�ȷ�Բ���Ӳ����ʱ��
//3.��ʹ����Ӳ����ʱ�������ã�����һЩ�Ծ���Ҫ�󲻸ߵĻص�����
//4.�ص������в����Ե������������ĺ���

//�����ʱ����Ϊ���ζ�ʱ�������ڶ�ʱ��
//���ζ�ʱ����һ������֮��ִ��һ�λص�������֮��ʱ�����ر�
//���ڶ�ʱ������ʱ���������֮����Զ��������ص�����Ҳһ���ִ��



//////////////////////////////�����ʱ�����API///////////////////////////////
//
/////////////////////��ʱ������///////////////////
//������ TimerHandle_t xTimerCreate(	const char * const pcTimerName,
//																		const TickType_t xTimerPeriodInTicks,
//																		const UBaseType_t uxAutoReload,
//																		void * const pvTimerID,
//																		TimerCallbackFunction_t pxCallbackFunction )
//������
//pcTimerName:��ʱ�������֣������������pcName����
//xTimerPeriodInTicks�������ʱ���Ķ�ʱ�����ڣ���λ��ϵͳ��С���ġ�
//uxAutoReload: pdTRUE���������ڶ�ʱ��     pdFALSE�����óɵ��ζ�ʱ��
//pvTimerID����ʱ��ID���ڻص������и��ݶ�ʱ��ID����ͬ�Ķ�ʱ��
//pxCallbackFunction���ص������ĺ�����
//��ע����̬����������ʱ��
//
//
//
//������ TimerHandle_t xTimerCreateStatic(	const char * const pcTimerName,
//																					const TickType_t xTimerPeriodInTicks,
//																					const UBaseType_t uxAutoReload,
//																					void * const pvTimerID,
//																					TimerCallbackFunction_t pxCallbackFunction,
//																					StaticTimer_t *pxTimerBuffer )
//������
//pcTimerName:��ʱ�������֣������������pcName����
//xTimerPeriodInTicks�������ʱ���Ķ�ʱ�����ڣ���λ��ϵͳ��С���ġ�
//uxAutoReload: pdTRUE���������ڶ�ʱ��     pdFALSE�����óɵ��ζ�ʱ��
//pvTimerID����ʱ��ID���ڻص������и��ݶ�ʱ��ID����ͬ�Ķ�ʱ��
//pxCallbackFunction���ص������ĺ�����
//pxTimerBuffer:ָ��һ��StaticTimer_t�ṹ�������ָ��
//��ע����̬����������ʱ��
////////////////////////////////////////////////////////////////////////////////
//
//
/////////////////////��ʱ����/////////////////////
//������ BaseType_t xTimerStart( TimerHandle_t xTimer, TickType_t xTicksToWait );
//������
//xTimer: Ҫ�����Ķ�ʱ�����
//xTicksToWait�� ��������ʱ��
//��ע��
//����ִ�еĺ�����xTimerGenericCommand()
//��ʱ������ʵ��������ʱ����������﷢��һ��tmrCOMMAND_START���
//�������������������õ�����ʱ����еȴ�
//�ú��������������п�����ʱ��
//����ʱ�Ѿ�������ִ�д˺���Ч����xTimerReset��һ����
//����ֵ��
//pdTRUE:��ʱ�������ɹ�
//pdFALSE:��ʱ������ʧ��
//
//
//������ BaseType_t xTimerStartFromISR( 	TimerHandle_t xTimer,
// 																				BaseType_t *pxHigherPriorityTaskWoken );
//������
//xTimer: Ҫ�����Ķ�ʱ�����
//pxHigherPriorityTaskWoken: ����˳��˺������Ƿ���������л�������ֵΪpdTRUE��ʱ�����˳��жϷ�����֮ǰһ��Ҫ����һ�������л���
//��ע��
//����ִ�еĺ�����xTimerGenericCommand()
//��ʱ������ʵ��������ʱ����������﷢��һ��tmrCOMMAND_START���
//�˺���û������ʱ�䣬���������ֱ������
//�ú����������ж��п�����ʱ��
//����ʱ�Ѿ�������ִ�д˺���Ч����xTimerReset��һ����
//pdTRUE:��ʱ�������ɹ�
//pdFALSE:��ʱ������ʧ��
//////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////��ʱ���ر�//////////////////////////
//������ BaseType_t xTimerStop( TimerHandle_t xTimer, TickType_t xTicksToWait );
//������
//xTimer: Ҫ�رյĶ�ʱ�����
//xTicksToWait�� ��������ʱ��
//��ע��
//����ִ�еĺ�����xTimerGenericCommand()
//��ʱ������ʵ��������ʱ����������﷢��һ��tmrCOMMAND_STOP���
//�������������������õ�����ʱ����еȴ�
//�ú��������������йرն�ʱ��
//pdTRUE:��ʱ���رճɹ�
//pdFALSE:��ʱ���ر�ʧ��
//
//
//������ BaseType_t xTimerStopFromISR( 	TimerHandle_t xTimer,
 //																			BaseType_t *pxHigherPriorityTaskWoken );
//������
//xTimer: Ҫ�رյĶ�ʱ�����
//pxHigherPriorityTaskWoken: ����˳��˺������Ƿ���������л�������ֵΪpdTRUE��ʱ�����˳��жϷ�����֮ǰһ��Ҫ����һ�������л���
//��ע��
//����ִ�еĺ�����xTimerGenericCommand()
//��ʱ������ʵ��������ʱ����������﷢��һ��tmrCOMMAND_STOP���
//�˺���û������ʱ�䣬���������ֱ������
//�ú����������ж��йرն�ʱ��
//pdTRUE:��ʱ���رճɹ�
//pdFALSE:��ʱ���ر�ʧ��
////////////////////////////////////////////////////////////////
//
//
/////////////////////////////////��ʱ������///////////////////////
//������ BaseType_t xTimerReset( TimerHandle_t xTimer, TickType_t xTicksToWait );
//������
//xTimer: Ҫ�����Ķ�ʱ�����
//xTicksToWait�� ��������ʱ��
//��ע��
//����ִ�еĺ�����xTimerGenericCommand()
//��ʱ������ʵ��������ʱ����������﷢��һ��tmrCOMMAND_RESET���
//�������������������õ�����ʱ����еȴ�
//�ú���������������������ʱ��
//����ʱ���Ѿ�������ִ�д˺���Ч����xTimerReset��һ���ģ�
//�����ʱ��δ������ִ�д˺���û��Ч��
//����ֵ��
//pdTRUE:��ʱ�������ɹ�
//pdFALSE:��ʱ������ʧ��
//
//
//������ BaseType_t xTimerResetFromISR( 	TimerHandle_t xTimer,
// 																				BaseType_t *pxHigherPriorityTaskWoken );
//������
//xTimer: Ҫ�����Ķ�ʱ�����
//pxHigherPriorityTaskWoken: ����˳��˺������Ƿ���������л�������ֵΪpdTRUE��ʱ�����˳��жϷ�����֮ǰһ��Ҫ����һ�������л���
//��ע��
//����ִ�еĺ�����xTimerGenericCommand()
//��ʱ������ʵ��������ʱ����������﷢��һ��tmrCOMMAND_RESET���
//�˺���û������ʱ�䣬���������ֱ������
//�ú����������ж���������ʱ��
//����ʱ�Ѿ�������ִ�д˺���Ч����xTimerReset��һ����
//�����ʱ��δ������ִ�д˺���û��Ч��
//pdTRUE:��ʱ�������ɹ�
//pdFALSE:��ʱ������ʧ��
//////////////////////////////////////////////////////////////////////////



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
	
	//����������ڶ�ʱ��
	//���ڶ�ʱ��������1s(1000��ʱ�ӽ���)������ģʽ
  AutoReloadTimer_Handle=xTimerCreate((const char*		)"AutoReloadTimer",
																			(TickType_t			)1000,
																			(UBaseType_t		)pdTRUE,
																			(void*				)1,
																			(TimerCallbackFunction_t)AutoReloadCallback); 
  //�������ζ�ʱ��
	//���ζ�ʱ��������2s(2000��ʱ�ӽ���)������ģʽ
	OneShotTimer_Handle=xTimerCreate((const char*			)"OneShotTimer",
																	 (TickType_t			)2000,
																	 (UBaseType_t			)pdFALSE,
																	 (void*					)2,
																	 (TimerCallbackFunction_t)OneShotCallback); 
							
	//������ʱ����������
  xTaskCreate((TaskFunction_t )timercontrol_task,             
							(const char*    )"timercontrol_task",           
							(uint16_t       )TIMERCONTROL_STK_SIZE,        
							(void*          )NULL,                  
							(UBaseType_t    )TIMERCONTROL_TASK_PRIO,        
							(TaskHandle_t*  )&TimerControlTask_Handler);
	 
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
	//vTaskDelete(NULL);
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
	vTaskDelete(NULL);
	
}

//����4
void task4(void *pvParameters)
{
	vTaskDelete(NULL);
}

void timercontrol_task(void * pvParameters)
{
	u8 key,num;
	while(1)
	{
		//ֻ��������ʱ���������ɹ��˲��ܶ�����в���
		if((AutoReloadTimer_Handle!=NULL)&&(OneShotTimer_Handle!=NULL))
		{
			key = KEY_Scan(0);
			switch(key)
			{
				case KEY0_PRESD:     //��key_up���µĻ������ڶ�ʱ��
					xTimerStart(AutoReloadTimer_Handle,0);	//�������ڶ�ʱ��
					printf("������ʱ��1\r\n");
					break;
				case KEY1_PRESD:		//��key0���µĻ��򿪵��ζ�ʱ��
					xTimerStart(OneShotTimer_Handle,0);		//�������ζ�ʱ��
					printf("������ʱ��2\r\n");
					break;
				case KEY2_PRESD:		//��key1���»��͹رն�ʱ��
					xTimerStop(AutoReloadTimer_Handle,0); 	//�ر����ڶ�ʱ��
					xTimerStop(OneShotTimer_Handle,0); 		//�رյ��ζ�ʱ��
					printf("�رն�ʱ��1��2\r\n");
					break;	
			}
		}
    vTaskDelay(10); //��ʱ10ms��Ҳ����10��ʱ�ӽ���
	}
}

//���ڶ�ʱ���Ļص�����
void AutoReloadCallback(TimerHandle_t xTimer)
{
	static u8 tmr1_num=0;
	tmr1_num++;									//���ڶ�ʱ��ִ�д�����1
	printf("���ڶ�ʱ���ص�����ִ�д���:%d\r\n",tmr1_num);
}

//���ζ�ʱ���Ļص�����
void OneShotCallback(TimerHandle_t xTimer)
{
	static u8 tmr2_num = 0;
	tmr2_num++;		//���ڶ�ʱ��ִ�д�����1
  printf("���ζ�ʱ�����н������ص�����ִ�д���:%d\r\n",tmr2_num);
}
