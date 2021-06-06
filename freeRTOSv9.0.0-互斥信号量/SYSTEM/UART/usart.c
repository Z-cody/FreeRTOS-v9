#include "usart.h"
#include "stdio.h"

//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  	
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
int _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc����   printf ��һ����
int fputc(int ch, FILE *f)
{ 	
	USART_SendData(USART1,ch);  //ͨ�����ڷ�������
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);      
	return ch;
}
#endif 

//���ڷ��ͻ����� 	
__align(8) u8 USART3_TX_BUF[USART3_MAX_SEND_LEN]; 	//���ͻ���,���USART2_MAX_SEND_LEN�ֽ�
//���ڽ��ջ����� 	
u8 USART3_RX_BUF[USART3_MAX_RECV_LEN]; 							//���ջ���,���USART2_MAX_RECV_LEN���ֽ�.


/*******************************
����3��

����˵����

PB10  -- U3_TX
PB11  -- U3_RX

*******************************/

void Usart3_Init(int BaudRate)
{
	
	GPIO_InitTypeDef  GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef  NVIC_InitStruct;
	
	
	//A.  �����ǹ����� APB1 ��������裬����ʹ�ܺ���Ϊ
	//ʹ�� USART1 ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	//B.  ʹ�õ��Ǵ��� 3������ 3 ��Ӧ��оƬ���� PB10,PB11 ��Ҫʹ��PB��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE); 

	//2)  �������Ÿ�����ӳ��
	//PB10 ����Ϊ USART3
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3); 
	//PB11 ����Ϊ USART3
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3);

	
	
	GPIO_InitStruct.GPIO_Pin 	= GPIO_Pin_10 | GPIO_Pin_11; 	//GPIOA9 �� GPIOA10
	GPIO_InitStruct.GPIO_Mode 	= GPIO_Mode_AF;							//����IO�ڸ��ù���
	GPIO_InitStruct.GPIO_Speed 	= GPIO_Speed_50MHz; 				//�ٶ� 50MHz
	GPIO_InitStruct.GPIO_OType 	= GPIO_OType_PP; 						//���츴�����
	GPIO_InitStruct.GPIO_PuPd 	= GPIO_PuPd_UP; 						//����
	//��ʼ�� PB10��PB11
	GPIO_Init(GPIOB,&GPIO_InitStruct); 
	
	
	
	USART_InitStruct.USART_BaudRate 	= BaudRate;																	//������ 115200;
	USART_InitStruct.USART_WordLength 	= USART_WordLength_8b;										//�ֳ�Ϊ 8 λ���ݸ�ʽ
	USART_InitStruct.USART_StopBits 	= USART_StopBits_1;													//һ��ֹͣλ
	USART_InitStruct.USART_Parity 		= USART_Parity_No;													//����żУ��λ
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; 	//��Ӳ��������
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;									//�շ�ģʽ
	//��ʼ������
	USART_Init(USART3, &USART_InitStruct); 
	
	
	NVIC_InitStruct.NVIC_IRQChannel 					= USART3_IRQn;  //�ж�ͨ��
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority	=3;			//��ռ���ȼ� 3
	NVIC_InitStruct.NVIC_IRQChannelSubPriority 			=3;  			//��Ӧ���ȼ� 3
	NVIC_InitStruct.NVIC_IRQChannelCmd 					= ENABLE; 		//IRQ ͨ��ʹ��
	//����ָ���Ĳ�����ʼ�� VIC �Ĵ���
	NVIC_Init(&NVIC_InitStruct);  
	
	//�����ж�
	USART_ITConfig(USART3, USART_IT_RXNE ,ENABLE);	
	
	//�ж�ʹ��
	USART_Cmd(USART3, ENABLE);

	//�ж�ʹ��
	USART_Cmd(USART3, ENABLE);

}

/*******************************
����2��
����˵����

PA:2  -- U2_TX
PA:3  -- U2_RX

*******************************/
void Usart2_Init(int BaudRate)
{
	
	GPIO_InitTypeDef  GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef  NVIC_InitStruct;
	
	
	//A.  ����2�ǹ����� APB1 ��������裬����ʹ�ܺ���Ϊ
	//ʹ�� USART2 ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	//B.  ʹ�õ��Ǵ��� 2������ 2 ��Ӧ��оƬ���� PA2,PA3 ��Ҫʹ��PA��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); 

	//2)  �������Ÿ�����ӳ��
	//PA2 ����Ϊ USART2
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2); 
	//PA3 ����Ϊ USART2
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2);

	
	
	GPIO_InitStruct.GPIO_Pin 		= GPIO_Pin_2 | GPIO_Pin_3; 		//GPIOA2 �� GPIOA3
	GPIO_InitStruct.GPIO_Mode 	= GPIO_Mode_AF;								//����IO�ڸ��ù���
	GPIO_InitStruct.GPIO_Speed 	= GPIO_Speed_50MHz; 					//�ٶ� 50MHz
	GPIO_InitStruct.GPIO_OType 	= GPIO_OType_PP; 							//���츴�����
	GPIO_InitStruct.GPIO_PuPd 	= GPIO_PuPd_UP; 							//����
	//��ʼ�� PA2��PA3
	GPIO_Init(GPIOA,&GPIO_InitStruct); 
	
	
	
	USART_InitStruct.USART_BaudRate 		= BaudRate;									//������ 9600,����������;
	USART_InitStruct.USART_WordLength 	= USART_WordLength_8b;	//�ֳ�Ϊ 8 λ���ݸ�ʽ
	USART_InitStruct.USART_StopBits 		= USART_StopBits_1;			//һ��ֹͣλ
	USART_InitStruct.USART_Parity 			= USART_Parity_No;			//����żУ��λ
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //��Ӳ��������
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	//��ʼ������
	USART_Init(USART2, &USART_InitStruct); 
	
	
	NVIC_InitStruct.NVIC_IRQChannel 									= USART2_IRQn;  //�ж�ͨ��
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority	=3;							//��ռ���ȼ� 3
	NVIC_InitStruct.NVIC_IRQChannelSubPriority 				=3;  						//��Ӧ���ȼ� 3
	NVIC_InitStruct.NVIC_IRQChannelCmd 								= ENABLE; 			//IRQ ͨ��ʹ��
	//����ָ���Ĳ�����ʼ�� VIC �Ĵ���
	NVIC_Init(&NVIC_InitStruct);  
	
	//�����ж�
	USART_ITConfig(USART2, USART_IT_RXNE ,ENABLE);	
	
	//�ж�ʹ��
	USART_Cmd(USART2, ENABLE);	

}

/*******************************
����1
����˵����

PA9  -- U1_TX
PA10 -- U1_RX

*******************************/

void Usart1_Init(int BaudRate)
{
	
	GPIO_InitTypeDef  GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef  NVIC_InitStruct;
	
	
	//A.  �����ǹ����� APB2 ��������裬����ʹ�ܺ���Ϊ
	//ʹ�� USART1 ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	//B.  ʹ�õ��Ǵ��� 1������ 1 ��Ӧ��оƬ���� PA9,PA10 ��Ҫʹ��PA��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); 

	//2)  �������Ÿ�����ӳ��
	//PA9 ����Ϊ USART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); 
	//PA10 ����Ϊ USART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);

	
	
	GPIO_InitStruct.GPIO_Pin 	= GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9 �� GPIOA10
	GPIO_InitStruct.GPIO_Mode 	= GPIO_Mode_AF;				//����IO�ڸ��ù���
	GPIO_InitStruct.GPIO_Speed 	= GPIO_Speed_50MHz; 		//�ٶ� 50MHz
	GPIO_InitStruct.GPIO_OType 	= GPIO_OType_PP; 			//���츴�����
	GPIO_InitStruct.GPIO_PuPd 	= GPIO_PuPd_UP; 			//����
	//��ʼ�� PA9��PA10
	GPIO_Init(GPIOA,&GPIO_InitStruct); 
	
	
	
	USART_InitStruct.USART_BaudRate 	= BaudRate;					//������ 115200;
	USART_InitStruct.USART_WordLength 	= USART_WordLength_8b;		//�ֳ�Ϊ 8 λ���ݸ�ʽ
	USART_InitStruct.USART_StopBits 	= USART_StopBits_1;			//һ��ֹͣλ
	USART_InitStruct.USART_Parity 		= USART_Parity_No;			//����żУ��λ
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //��Ӳ��������
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	//��ʼ������
	USART_Init(USART1, &USART_InitStruct); 
	
	
	NVIC_InitStruct.NVIC_IRQChannel 					= USART1_IRQn;  //�ж�ͨ��
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority	=3;				//��ռ���ȼ� 3
	NVIC_InitStruct.NVIC_IRQChannelSubPriority 			=3;  			//��Ӧ���ȼ� 3
	NVIC_InitStruct.NVIC_IRQChannelCmd 					= ENABLE; 		//IRQ ͨ��ʹ��
	//����ָ���Ĳ�����ʼ�� VIC �Ĵ���
	NVIC_Init(&NVIC_InitStruct);  
	
	//�����ж�
	USART_ITConfig(USART1, USART_IT_RXNE ,ENABLE);	
	
	//�ж�ʹ��
	USART_Cmd(USART1, ENABLE);	
	

}


