#include "usart.h"
#include "stdio.h"

//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  	
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
int _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数   printf 是一个宏
int fputc(int ch, FILE *f)
{ 	
	USART_SendData(USART1,ch);  //通过串口发送数据
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);      
	return ch;
}
#endif 

//串口发送缓存区 	
__align(8) u8 USART3_TX_BUF[USART3_MAX_SEND_LEN]; 	//发送缓冲,最大USART2_MAX_SEND_LEN字节
//串口接收缓存区 	
u8 USART3_RX_BUF[USART3_MAX_RECV_LEN]; 							//接收缓冲,最大USART2_MAX_RECV_LEN个字节.


/*******************************
串口3：

引脚说明：

PB10  -- U3_TX
PB11  -- U3_RX

*******************************/

void Usart3_Init(int BaudRate)
{
	
	GPIO_InitTypeDef  GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef  NVIC_InitStruct;
	
	
	//A.  串口是挂载在 APB1 下面的外设，所以使能函数为
	//使能 USART1 时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	//B.  使用的是串口 3，串口 3 对应着芯片引脚 PB10,PB11 需要使能PB的时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE); 

	//2)  设置引脚复用器映射
	//PB10 复用为 USART3
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3); 
	//PB11 复用为 USART3
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3);

	
	
	GPIO_InitStruct.GPIO_Pin 	= GPIO_Pin_10 | GPIO_Pin_11; 	//GPIOA9 与 GPIOA10
	GPIO_InitStruct.GPIO_Mode 	= GPIO_Mode_AF;							//配置IO口复用功能
	GPIO_InitStruct.GPIO_Speed 	= GPIO_Speed_50MHz; 				//速度 50MHz
	GPIO_InitStruct.GPIO_OType 	= GPIO_OType_PP; 						//推挽复用输出
	GPIO_InitStruct.GPIO_PuPd 	= GPIO_PuPd_UP; 						//上拉
	//初始化 PB10，PB11
	GPIO_Init(GPIOB,&GPIO_InitStruct); 
	
	
	
	USART_InitStruct.USART_BaudRate 	= BaudRate;																	//波特率 115200;
	USART_InitStruct.USART_WordLength 	= USART_WordLength_8b;										//字长为 8 位数据格式
	USART_InitStruct.USART_StopBits 	= USART_StopBits_1;													//一个停止位
	USART_InitStruct.USART_Parity 		= USART_Parity_No;													//无奇偶校验位
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; 	//无硬件控制流
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;									//收发模式
	//初始化串口
	USART_Init(USART3, &USART_InitStruct); 
	
	
	NVIC_InitStruct.NVIC_IRQChannel 					= USART3_IRQn;  //中断通道
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority	=3;			//抢占优先级 3
	NVIC_InitStruct.NVIC_IRQChannelSubPriority 			=3;  			//响应优先级 3
	NVIC_InitStruct.NVIC_IRQChannelCmd 					= ENABLE; 		//IRQ 通道使能
	//根据指定的参数初始化 VIC 寄存器
	NVIC_Init(&NVIC_InitStruct);  
	
	//接收中断
	USART_ITConfig(USART3, USART_IT_RXNE ,ENABLE);	
	
	//中断使能
	USART_Cmd(USART3, ENABLE);

	//中断使能
	USART_Cmd(USART3, ENABLE);

}

/*******************************
串口2：
引脚说明：

PA:2  -- U2_TX
PA:3  -- U2_RX

*******************************/
void Usart2_Init(int BaudRate)
{
	
	GPIO_InitTypeDef  GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef  NVIC_InitStruct;
	
	
	//A.  串口2是挂载在 APB1 下面的外设，所以使能函数为
	//使能 USART2 时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	//B.  使用的是串口 2，串口 2 对应着芯片引脚 PA2,PA3 需要使能PA的时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); 

	//2)  设置引脚复用器映射
	//PA2 复用为 USART2
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2); 
	//PA3 复用为 USART2
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2);

	
	
	GPIO_InitStruct.GPIO_Pin 		= GPIO_Pin_2 | GPIO_Pin_3; 		//GPIOA2 与 GPIOA3
	GPIO_InitStruct.GPIO_Mode 	= GPIO_Mode_AF;								//配置IO口复用功能
	GPIO_InitStruct.GPIO_Speed 	= GPIO_Speed_50MHz; 					//速度 50MHz
	GPIO_InitStruct.GPIO_OType 	= GPIO_OType_PP; 							//推挽复用输出
	GPIO_InitStruct.GPIO_PuPd 	= GPIO_PuPd_UP; 							//上拉
	//初始化 PA2，PA3
	GPIO_Init(GPIOA,&GPIO_InitStruct); 
	
	
	
	USART_InitStruct.USART_BaudRate 		= BaudRate;									//波特率 9600,适用于蓝牙;
	USART_InitStruct.USART_WordLength 	= USART_WordLength_8b;	//字长为 8 位数据格式
	USART_InitStruct.USART_StopBits 		= USART_StopBits_1;			//一个停止位
	USART_InitStruct.USART_Parity 			= USART_Parity_No;			//无奇偶校验位
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //无硬件控制流
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	//初始化串口
	USART_Init(USART2, &USART_InitStruct); 
	
	
	NVIC_InitStruct.NVIC_IRQChannel 									= USART2_IRQn;  //中断通道
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority	=3;							//抢占优先级 3
	NVIC_InitStruct.NVIC_IRQChannelSubPriority 				=3;  						//响应优先级 3
	NVIC_InitStruct.NVIC_IRQChannelCmd 								= ENABLE; 			//IRQ 通道使能
	//根据指定的参数初始化 VIC 寄存器
	NVIC_Init(&NVIC_InitStruct);  
	
	//接收中断
	USART_ITConfig(USART2, USART_IT_RXNE ,ENABLE);	
	
	//中断使能
	USART_Cmd(USART2, ENABLE);	

}

/*******************************
串口1
引脚说明：

PA9  -- U1_TX
PA10 -- U1_RX

*******************************/

void Usart1_Init(int BaudRate)
{
	
	GPIO_InitTypeDef  GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef  NVIC_InitStruct;
	
	
	//A.  串口是挂载在 APB2 下面的外设，所以使能函数为
	//使能 USART1 时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	//B.  使用的是串口 1，串口 1 对应着芯片引脚 PA9,PA10 需要使能PA的时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); 

	//2)  设置引脚复用器映射
	//PA9 复用为 USART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); 
	//PA10 复用为 USART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);

	
	
	GPIO_InitStruct.GPIO_Pin 	= GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9 与 GPIOA10
	GPIO_InitStruct.GPIO_Mode 	= GPIO_Mode_AF;				//配置IO口复用功能
	GPIO_InitStruct.GPIO_Speed 	= GPIO_Speed_50MHz; 		//速度 50MHz
	GPIO_InitStruct.GPIO_OType 	= GPIO_OType_PP; 			//推挽复用输出
	GPIO_InitStruct.GPIO_PuPd 	= GPIO_PuPd_UP; 			//上拉
	//初始化 PA9，PA10
	GPIO_Init(GPIOA,&GPIO_InitStruct); 
	
	
	
	USART_InitStruct.USART_BaudRate 	= BaudRate;					//波特率 115200;
	USART_InitStruct.USART_WordLength 	= USART_WordLength_8b;		//字长为 8 位数据格式
	USART_InitStruct.USART_StopBits 	= USART_StopBits_1;			//一个停止位
	USART_InitStruct.USART_Parity 		= USART_Parity_No;			//无奇偶校验位
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //无硬件控制流
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	//初始化串口
	USART_Init(USART1, &USART_InitStruct); 
	
	
	NVIC_InitStruct.NVIC_IRQChannel 					= USART1_IRQn;  //中断通道
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority	=3;				//抢占优先级 3
	NVIC_InitStruct.NVIC_IRQChannelSubPriority 			=3;  			//响应优先级 3
	NVIC_InitStruct.NVIC_IRQChannelCmd 					= ENABLE; 		//IRQ 通道使能
	//根据指定的参数初始化 VIC 寄存器
	NVIC_Init(&NVIC_InitStruct);  
	
	//接收中断
	USART_ITConfig(USART1, USART_IT_RXNE ,ENABLE);	
	
	//中断使能
	USART_Cmd(USART1, ENABLE);	
	

}


