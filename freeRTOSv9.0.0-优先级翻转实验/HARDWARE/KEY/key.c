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
	//ʹ��A�����ŵ�ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	
	//ʹ��E�����ŵ�ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
	
	//����GPIO_InitTypeDef�ṹ��
	GPIO_InitTypeDef gpio;
	
	//��ʼ��GPIO_InitTypeDef�ṹ��
	gpio.GPIO_Pin   = GPIO_Pin_0;					//��������PA0
	gpio.GPIO_Mode  = GPIO_Mode_IN;				//����ģʽ
	gpio.GPIO_PuPd  = GPIO_PuPd_UP;				//����
	gpio.GPIO_Speed = GPIO_High_Speed;		//����
	
	//��ʼ������PA0,S1
	GPIO_Init(GPIOA,&gpio);
	
	//��ʼ������PE2,S2
	gpio.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOE,&gpio);
	
	//��ʼ������PE3,S3
	gpio.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOE,&gpio);
	
	//��ʼ������PE4,S4
	gpio.GPIO_Pin = GPIO_Pin_4;
	GPIO_Init(GPIOE,&gpio);

}

//����ɨ�躯��
//����0��û���κΰ�������;���ر�־����ʾ�Ǹ�����������
//mode:0,��֧��������;1,֧��������;
//ע��˺�������Ӧ���ȼ�,KEY0>KEY1>KEY2>KEY3
u8 KEY_Scan(u8 mode)
{
//	static u8 key_up=1;     //�����ɿ���־
//    if(mode==1)key_up=1;    //֧������
//    if(key_up && (KEY0==0||KEY1==0||KEY2==0||KEY3==0))
//    {
//        delay_ms(10);
//        key_up=0;
//        if(KEY0==0)       return KEY0_PRESD;
//        else if(KEY1==0)  return KEY1_PRESD;
//        else if(KEY2==0)  return KEY2_PRESD;
//				else if(KEY3==0)  return KEY3_PRESD;			
//    }else if(KEY0==1&&KEY1==1&&KEY2==1&&KEY3==1)key_up=1;
//    return 0;   //�ް�������
		
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
