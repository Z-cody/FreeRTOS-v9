#include "led.h"

/*********************
控制灯亮灭

输出低电平（0），灯亮
输出高电平（1），灯灭

PF9 --led0
PF10--led1
PE13--led2
PE14--led3

*********************/


void Led_Init(void)
{
	//GPIO变量
	GPIO_InitTypeDef GPIO_InitStruct;
	
	//使能（打开）GPIO E组时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	//使能（打开）GPIO F组时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	
	
	//配置PF9、 PF10组引脚
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_9|GPIO_Pin_10;    //引脚9和引脚10
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;             //输出模式
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;				//推挽输出
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;			//快速
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;				//带上拉电阻
	//初始化PF组引脚
	GPIO_Init(GPIOF,&GPIO_InitStruct);
	
	
	
	//配置PE组引脚
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_13|GPIO_Pin_14;	//引脚13和引脚14
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;				//输出模式
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;				//推挽输出
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;			//快速 
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;				//带上拉电阻
	//初始化PE组引脚
	GPIO_Init(GPIOE,&GPIO_InitStruct);
	
	//关闭所有的灯
	GPIO_SetBits(GPIOF,GPIO_Pin_9);
	GPIO_SetBits(GPIOF,GPIO_Pin_10);
	GPIO_SetBits(GPIOE,GPIO_Pin_13);
	GPIO_SetBits(GPIOE,GPIO_Pin_14);
	

}
