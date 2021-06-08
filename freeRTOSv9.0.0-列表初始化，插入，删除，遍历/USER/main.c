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

//����һ���б�
List_t TestList;
//���������б���
ListItem_t ListItem1;
ListItem_t ListItem2;
ListItem_t ListItem3;


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
                (const char*    )"this is a test task",   	
                (uint16_t       )test_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )test_TASK_PRIO,	
                (TaskHandle_t*  )&testTask_Handler);  

		//��������1
    xTaskCreate((TaskFunction_t )task1,     	
                (const char*    )"this is the task1",   	
                (uint16_t       )task1_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )TASK1_PRIO,	
                (TaskHandle_t*  )&TASK1_Handler);
								
		//��������2
    xTaskCreate((TaskFunction_t )task2,     	
                (const char*    )"this is the task2",   	
                (uint16_t       )task2_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )TASK2_PRIO,	
                (TaskHandle_t*  )&TASK2_Handler);
								
		//��������3
    xTaskCreate((TaskFunction_t )task3,     	
                (const char*    )"this is the task3",   	
                (uint16_t       )task3_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )TASK3_PRIO,	
                (TaskHandle_t*  )&TASK3_Handler);
     
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
void task3(void *pvParamters)
{
	//vTaskDelete(NULL);
	//һ����ʼ���б���б���
	vListInitialise(&TestList);
	vListInitialiseItem(&ListItem1);
	vListInitialiseItem(&ListItem2);
	vListInitialiseItem(&ListItem3);
	
	//�б��ֵ
	ListItem1.xItemValue = 40;
	ListItem2.xItemValue = 60;
	ListItem3.xItemValue = 50;
	
	//������ӡ�б���б���ĵ�ַ
	printf("/*******************�б���б����ַ*******************/\r\n");
	printf("��Ŀ                              ��ַ				          \r\n");
	printf("TestList                          %#x					          \r\n",(int)&TestList);
	printf("TestList->pxIndex                 %#x				           	\r\n",(int)TestList.pxIndex);
	printf("TestList->xListEnd                %#x                   \r\n",(int)(&TestList.xListEnd));
	printf("ListItem1                         %#x                   \r\n",(int)(&ListItem1));
	printf("ListItem2                         %#x                   \r\n",(int)(&ListItem2));
	printf("ListItem3                         %#x                   \r\n",(int)(&ListItem3));
	printf("/************************����**************************/\r\n");
	printf("����S1�����б������ListItem1\r\n\r\n\r\n");
	while(KEY_Scan(0) != KEY0_PRESD)F_delay_ms(10);								//�ȴ�����S1���ǰ���
	
	//��������б���1������ӡ�����б���pxNext��pxPrevious��ַ
	vListInsert(&TestList,&ListItem1);														//�����б���1
	printf("/******************����б���ListItem1*****************/\r\n");
	printf("��Ŀ                              ��ַ				          \r\n");
	printf("TestList->xListEnd->pxNext        %#x                   \r\n",(int)TestList.xListEnd.pxNext);
	printf("ListItem1->pxNext                 %#x                   \r\n",(int)ListItem1.pxNext);
	printf("/*******************ǰ�������ӷָ���********************/\r\n");
	printf("TestList->xListEnd->pxPrevious    %#x                   \r\n",(int)TestList.xListEnd.pxPrevious);
	printf("ListItem1->pxPrevious             %#x                   \r\n",(int)ListItem1.pxPrevious);
	printf("/************************����**************************/\r\n");
	printf("����S1�����б������ListItem2\r\n\r\n\r\n");
	while(KEY_Scan(0) != KEY0_PRESD)F_delay_ms(10);								//�ȴ�����S1���ǰ���
	
	//�ģ�����б���2������ӡ�����б���pxNext��pxPrevious��ַ
	vListInsert(&TestList,&ListItem2);														//�����б���1
	printf("/******************����б���ListItem2*****************/\r\n");
	printf("��Ŀ                              ��ַ				          \r\n");
	printf("TestList->xListEnd->pxNext        %#x                   \r\n",(int)TestList.xListEnd.pxNext);
	printf("ListItem1->pxNext                 %#x                   \r\n",(int)ListItem1.pxNext);
	printf("ListItem2->pxNext                 %#x                   \r\n",(int)ListItem2.pxNext);
	printf("/*******************ǰ�������ӷָ���********************/\r\n");
	printf("TestList->xListEnd->pxPrevious    %#x                   \r\n",(int)TestList.xListEnd.pxPrevious);
	printf("ListItem1->pxPrevious             %#x                   \r\n",(int)ListItem1.pxPrevious);
	printf("ListItem2->pxPrevious             %#x                   \r\n",(int)ListItem2.pxPrevious);
	printf("/************************����**************************/\r\n");
	printf("����S1�����б������ListItem3\r\n\r\n\r\n");
	while(KEY_Scan(0) != KEY0_PRESD)F_delay_ms(10);								//�ȴ�����S1���ǰ���
	
	//�壺����б���3������ӡ�����б���pxNext��pxPrevious��ַ
	vListInsert(&TestList,&ListItem3);														//�����б���1
	printf("/******************����б���ListItem3*****************/\r\n");
	printf("��Ŀ                              ��ַ				          \r\n");
	printf("TestList->xListEnd->pxNext        %#x                   \r\n",(int)TestList.xListEnd.pxNext);
	printf("ListItem1->pxNext                 %#x                   \r\n",(int)ListItem1.pxNext);
	printf("ListItem2->pxNext                 %#x                   \r\n",(int)ListItem2.pxNext);
	printf("ListItem3->pxNext                 %#x                   \r\n",(int)ListItem3.pxNext);
	printf("/*******************ǰ�������ӷָ���********************/\r\n");
	printf("TestList->xListEnd->pxPrevious    %#x                   \r\n",(int)TestList.xListEnd.pxPrevious);
	printf("ListItem1->pxPrevious             %#x                   \r\n",(int)ListItem1.pxPrevious);
	printf("ListItem2->pxPrevious             %#x                   \r\n",(int)ListItem2.pxPrevious);
  printf("ListItem3->pxPrevious             %#x                   \r\n",(int)ListItem3.pxPrevious);
	printf("/************************����**************************/\r\n");
	printf("����S1ɾ���б���ListItem2\r\n\r\n\r\n");
	while(KEY_Scan(0) != KEY0_PRESD)F_delay_ms(10);								//�ȴ�����S1���ǰ���
	
	//����ɾ���б���2
	uxListRemove(&ListItem2);
	printf("/******************ɾ���б���ListItem2*****************/\r\n");
	printf("��Ŀ                              ��ַ				          \r\n");
	printf("TestList->xListEnd->pxNext        %#x                   \r\n",(int)TestList.xListEnd.pxNext);
	printf("ListItem1->pxNext                 %#x                   \r\n",(int)ListItem1.pxNext);
	printf("ListItem3->pxNext                 %#x                   \r\n",(int)ListItem3.pxNext);
	printf("/*******************ǰ�������ӷָ���********************/\r\n");
	printf("TestList->xListEnd->pxPrevious    %#x                   \r\n",(int)TestList.xListEnd.pxPrevious);
	printf("ListItem1->pxPrevious             %#x                   \r\n",(int)ListItem1.pxPrevious);
  printf("ListItem3->pxPrevious             %#x                   \r\n",(int)ListItem3.pxPrevious);
	printf("/************************����**************************/\r\n");
	printf("����S1��ĩβ����б���ListItem2\r\n\r\n\r\n");
	while(KEY_Scan(0) != KEY0_PRESD)F_delay_ms(10);								//�ȴ�����S1���ǰ���
	
	//�ߣ���ĩβ����б���2
	TestList.pxIndex = TestList.pxIndex->pxNext;		//ʹpxIndexָ���һ�Ҳ����ListItem1
	vListInsertEnd(&TestList,&ListItem2);\
	printf("/******************��ĩβ����б���ListItem2*****************/\r\n");
	printf("��Ŀ                              ��ַ				          \r\n");
	printf("TestList->xListEnd->pxNext        %#x                   \r\n",(int)TestList.xListEnd.pxNext);
	printf("ListItem2->pxNext                 %#x                   \r\n",(int)ListItem2.pxNext);
	printf("ListItem1->pxNext                 %#x                   \r\n",(int)ListItem1.pxNext);
	printf("ListItem3->pxNext                 %#x                   \r\n",(int)ListItem3.pxNext);
	printf("/*******************ǰ�������ӷָ���********************/\r\n");
	printf("TestList->xListEnd->pxPrevious    %#x                   \r\n",(int)TestList.xListEnd.pxPrevious);
	printf("ListItem2->pxPrevious             %#x                   \r\n",(int)ListItem2.pxPrevious);
	printf("ListItem1->pxPrevious             %#x                   \r\n",(int)ListItem1.pxPrevious);
  printf("ListItem3->pxPrevious             %#x                   \r\n",(int)ListItem3.pxPrevious);
	printf("/************************����**************************/\r\n");
	
	for(;;)
	{
		F_delay_s(1);
	}
	
}
