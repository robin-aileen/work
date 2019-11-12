#include "hx711.h"
unsigned long Data[12];
void GPIO_INIT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);  
	/************************ CLK  PA 9 ************************/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOA,GPIO_Pin_9);
	/*********************** ADDO  PA 10 **************************/
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}
//HX711 AD读数
unsigned long ReadCount(void)
{
	unsigned long Count = 0;
	unsigned char i;
	GPIO_ResetBits(GPIOA,GPIO_Pin_9); 
	delay_us(1); 
	while( GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_10) );
	for (i = 0; i < 24; i++)
		{
			GPIO_SetBits(GPIOA,GPIO_Pin_9);
			Count = Count << 1;
			delay_us(1); 
			GPIO_ResetBits(GPIOA,GPIO_Pin_9);
			if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_10)) 
			Count++;
			delay_us(1); 
		}		
	GPIO_SetBits(GPIOA,GPIO_Pin_9);
	delay_us(1);
	Count = Count^0x800000;			
	GPIO_ResetBits(GPIOA,GPIO_Pin_9);
	delay_us(1);	
	return(Count);
}
unsigned long FilterCount(void)
{
	unsigned long value,sum,max,min;
	unsigned char n;
	value = ReadCount();//min初始值为 0，如果没有先赋值，最后将导致 min 为 0
	max = value;
	min = value;
	for(n = 0; n < 22; n++)
	{
		value = ReadCount();
		if(value > max)max = value;
		if(value < min)min = value;
		sum += value;
	}
	value =(sum - max - min)/20;
	return(value);
}
/*******************************************************************************
* 函 数 名         : delay_us
* 函数功能           : 延时函数，延时us
* 输    入         : i
* 输    出         : 无
*******************************************************************************/
void delay_us(u32 i)
{
    u32 temp;
    SysTick->LOAD=9*i;         //设置重装数值, 72MHZ时
    SysTick->CTRL=0X01;        //使能，减到零是无动作，采用外部时钟源
    SysTick->VAL=0;            //清零计数器
    do
    {
        temp=SysTick->CTRL;    //读取当前倒计数值
    }
    while((temp&0x01)&&(!(temp&(1<<16))));  //等待时间到达
    SysTick->CTRL=0;    //关闭计数器
    SysTick->VAL=0;     //清空计数器
}

/*******************************************************************************
* 函 数 名         : delay_ms
* 函数功能           : 延时函数，延时ms
* 输    入         : i
* 输    出         : 无
*******************************************************************************/
void delay_ms(u32 i)
{
    u32 temp;
    SysTick->LOAD=9000*i;      //设置重装数值, 72MHZ时
    SysTick->CTRL=0X01;        //使能，减到零是无动作，采用外部时钟源
    SysTick->VAL=0;            //清零计数器
    do
    {
     temp=SysTick->CTRL;       //读取当前倒计数值
    }
    while((temp&0x01)&&(!(temp&(1<<16))));    //等待时间到达
    SysTick->CTRL=0;    //关闭计数器
    SysTick->VAL=0;        //清空计数器
}
