#include "key.h"
#include "delay.h"

/**********************************************
key0 -- S1 -- PA0
key1 -- S2 -- PE2
key2 -- S3 -- PE3
key3 -- S4 -- PE4

**********************************************/

void AllKeyInit(void)
{
	//使能A组引脚的时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	
	//使能E组引脚的时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
	
	//创建GPIO_InitTypeDef结构体
	GPIO_InitTypeDef gpio;
	
	//初始化GPIO_InitTypeDef结构体
	gpio.GPIO_Pin   = GPIO_Pin_0;					//设置引脚PA0
	gpio.GPIO_Mode  = GPIO_Mode_IN;				//输入模式
	gpio.GPIO_PuPd  = GPIO_PuPd_UP;				//上拉
	gpio.GPIO_Speed = GPIO_High_Speed;		//高速
	
	//初始化引脚PA0,S1
	GPIO_Init(GPIOA,&gpio);
	
	//初始化引脚PE2,S2
	gpio.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOE,&gpio);
	
	//初始化引脚PE3,S3
	gpio.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOE,&gpio);
	
	//初始化引脚PE4,S4
	gpio.GPIO_Pin = GPIO_Pin_4;
	GPIO_Init(GPIOE,&gpio);

}

//按键扫描函数
//返回0，没有任何按键按下;返回标志，表示那个按键被按下
//mode:0,不支持连续按;1,支持连续按;
//注意此函数有响应优先级,KEY0>KEY1>KEY2>KEY3
u8 KEY_Scan(u8 mode)
{
//	static u8 key_up=1;     //按键松开标志
//    if(mode==1)key_up=1;    //支持连按
//    if(key_up && (KEY0==0||KEY1==0||KEY2==0||KEY3==0))
//    {
//        delay_ms(10);
//        key_up=0;
//        if(KEY0==0)       return KEY0_PRESD;
//        else if(KEY1==0)  return KEY1_PRESD;
//        else if(KEY2==0)  return KEY2_PRESD;
//				else if(KEY3==0)  return KEY3_PRESD;			
//    }else if(KEY0==1&&KEY1==1&&KEY2==1&&KEY3==1)key_up=1;
//    return 0;   //无按键按下
		
		if(KEY0==0||KEY1==0||KEY2==0||KEY3==0)
		{
			if(KEY0==0)
			{
				return KEY0_PRESD;
			}
			else if(KEY1==0)
			{
				return KEY1_PRESD;
			}
			else if(KEY2_PRESD)
			{
				return KEY2_PRESD;
			}
			else if(KEY3_PRESD)
			{
				return KEY3_PRESD;
			}
			
		}
		else
			{
				return 0;
			}
}
