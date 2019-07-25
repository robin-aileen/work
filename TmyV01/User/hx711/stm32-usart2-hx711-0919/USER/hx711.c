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
//HX711 AD����
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
	value = ReadCount();//min��ʼֵΪ 0�����û���ȸ�ֵ����󽫵��� min Ϊ 0
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
* �� �� ��         : delay_us
* ��������           : ��ʱ��������ʱus
* ��    ��         : i
* ��    ��         : ��
*******************************************************************************/
void delay_us(u32 i)
{
    u32 temp;
    SysTick->LOAD=9*i;         //������װ��ֵ, 72MHZʱ
    SysTick->CTRL=0X01;        //ʹ�ܣ����������޶����������ⲿʱ��Դ
    SysTick->VAL=0;            //���������
    do
    {
        temp=SysTick->CTRL;    //��ȡ��ǰ������ֵ
    }
    while((temp&0x01)&&(!(temp&(1<<16))));  //�ȴ�ʱ�䵽��
    SysTick->CTRL=0;    //�رռ�����
    SysTick->VAL=0;     //��ռ�����
}

/*******************************************************************************
* �� �� ��         : delay_ms
* ��������           : ��ʱ��������ʱms
* ��    ��         : i
* ��    ��         : ��
*******************************************************************************/
void delay_ms(u32 i)
{
    u32 temp;
    SysTick->LOAD=9000*i;      //������װ��ֵ, 72MHZʱ
    SysTick->CTRL=0X01;        //ʹ�ܣ����������޶����������ⲿʱ��Դ
    SysTick->VAL=0;            //���������
    do
    {
     temp=SysTick->CTRL;       //��ȡ��ǰ������ֵ
    }
    while((temp&0x01)&&(!(temp&(1<<16))));    //�ȴ�ʱ�䵽��
    SysTick->CTRL=0;    //�رռ�����
    SysTick->VAL=0;        //��ռ�����
}
