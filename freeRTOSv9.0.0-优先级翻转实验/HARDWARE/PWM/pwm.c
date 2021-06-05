#include "pwm.h"

/***********************************
引脚说明：

LED0 -- PF9 --TIM14_CH1(CH1:通道1)
TIM14 -- APB1 (84MHZ)


***********************************/



void Pwm_PF9_Init(void)
{
	//GPIO变量
	GPIO_InitTypeDef 			GPIO_InitStruct;
	TIM_TimeBaseInitTypeDef  	TIM_TimeBaseInitStruct;
	TIM_OCInitTypeDef  			TIM_OCInitStruct;
	
	//使能定时器14时钟：
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);
	//使能GPIOF时钟：
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	
	
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_9;				//引脚9
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;             	//复用功能
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;				//推挽输出
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;			//快速
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;				//带上拉电阻
	//初始化IO口为复用功能输出。函数：
	GPIO_Init(GPIOF, &GPIO_InitStruct);

	//GPIOF9复用映射到定时器14
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource9,GPIO_AF_TIM14);
	
	
	TIM_TimeBaseInitStruct.TIM_Prescaler	= 84-1;   			//84分频  84MHZ/84 = 1MHZ
	TIM_TimeBaseInitStruct.TIM_Period		= 1000-1; 			//在1MHZ下，计1000个数，用时1ms
	TIM_TimeBaseInitStruct.TIM_CounterMode	= TIM_CounterMode_Up; //向上计数
	TIM_TimeBaseInitStruct.TIM_ClockDivision= TIM_CKD_DIV1;       //分频因子
	//（2）初始化定时器，配置ARR,PSC。
	TIM_TimeBaseInit(TIM14,&TIM_TimeBaseInitStruct);		

	
	TIM_OCInitStruct.TIM_OCMode		= TIM_OCMode_PWM1;  	//PWM1模#式
	TIM_OCInitStruct.TIM_OCPolarity	= TIM_OCPolarity_Low; 	//低电平
	TIM_OCInitStruct.TIM_OutputState= TIM_OutputState_Enable; //通道使能	
	//初始化输出比较参数:OC1 -->输出通道1
	TIM_OC1Init(TIM14, &TIM_OCInitStruct);
	
	
	
	//使能预装载寄存器：  OC1 -->输出通道1
	TIM_OC1PreloadConfig(TIM14, TIM_OCPreload_Enable); 
	//（7）使能自动重装载的预装载寄存器允许位	
	TIM_ARRPreloadConfig(TIM14,ENABLE);
	
	
	//（8）使能定时器。
	TIM_Cmd(TIM14, ENABLE);
}
