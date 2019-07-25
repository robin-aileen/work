/*
*********************************************************************************************************
*
*	模块名称 : ADC驱动模块
*	文件名称 : bsp_adc.c
*	版    本 : V1.0
*   M  C  U  : STM32F103RBT6
*   编 译 器 : IAR-Arm 8.22.2
*
*	说    明 : ADC驱动，检测电池电压
*
*	修改记录 :
*		版本号  日期        作者     说明
*		V1.0    2018-03-20 robin    ADC采集为单次模式，T3中断触发（每秒一次），并增加滤波处理;
*	Copyright (C), 2019-2029
*
*********************************************************************************************************
*/
#include "bsp.h"

/* ADC对应的RCC时钟 */

#define RCC_ALL_ADC 	(RCC_APB2Periph_GPIOC)

#define GPIO_PORT_ADC_EN    GPIOC
#define GPIO_PIN_ADC_EN	    GPIO_Pin_1
#define	GPIO_PORT_AD_DAT    GPIOC
#define GPIO_PIN_ADC_DAT    GPIO_Pin_0

#define	MAX_POWER		25200
#define	MIN_POWER		16500

//// 定义一个变量存放ADC转换值 
//__IO uint16_t ADC1_Value;

 /*
*********************************************************************************************************
*	函 数 名: ADC1_InSigEn
*	功能说明: ADC信号源输入使能
*	形    参:
*	返 回 值: 无
*********************************************************************************************************
*/
void ADC1_InSigEn( uint8_t onoff )
{
    
    if (onoff == ADC_OFF)
	{
		GPIO_PORT_ADC_EN->BRR = GPIO_PIN_ADC_EN;
	}else
	{
		GPIO_PORT_ADC_EN->BSRR = GPIO_PIN_ADC_EN;
	}
}


void ADC1_InSigEnInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* 打开GPIO时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_AHBPeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);
	
	ADC1_InSigEn(ADC_OFF);

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//推挽输出模式
	
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_ADC_EN;
	GPIO_Init(GPIO_PORT_ADC_EN, &GPIO_InitStructure);
    
	ADC1_InSigEn( ADC_ON );
}

static void ADC1_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	/* 使能 ADC1 and GPIOC clock */
	RCC_AHBPeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);
    
	/* 配置PA6为模拟输入(ADC Channel14) */
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_ADC_DAT;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIO_PORT_AD_DAT, &GPIO_InitStructure);
}
 
 
 
 
static void ADC1_Mode_Config(void)
{
    ADC_InitTypeDef ADC_InitStructure;
	
	/* 使能 ADC1 and GPIOC clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    /* 配置ADC1, 不用DMA, 用软件触发 */
	//ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	
	//单次转换，定时器T3触发
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; //ADC 独立模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE; //单通道模式
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; //单次转换模式
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO;//外部触发设置为TIM3
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//ADC 数据右对齐
    
	ADC_InitStructure.ADC_NbrOfChannel = 1;//顺序进行规则转换的ADC 通道的数目
	ADC_Init(ADC1, &ADC_InitStructure);

	
	
//	//连续转换模式
//	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
//	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	
	
//    //单次转换，定时器T3触发
//	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;            //禁用连续转换模式  ENABLE;
//	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO; //外部触发设置为TIM3
//	
//	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
//	//ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Backward;
//	ADC_Init(ADC1, &ADC_InitStructure);
    
    
	//RCC_ADCCLKConfig(RCC_ADCCLK_PCLK_Div4);
	
    /* 配置ADC1 规则通道14 channel9 configuration */
	//ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_55Cycles5);//ADC_ChannelConfig(ADC1, ADC_Channel_1, ADC_SampleTime_55_5Cycles); //ADC_SampleTime_239_5Cycles
    ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_55Cycles5);
	/* 失能 ADC1 */
	ADC_Cmd(ADC1, DISABLE);
	
	/* ADC 校准 */
	//ADC_GetCalibrationFactor(ADC1);
	//while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADCAL));//?
	
	// ADC2 转换结束产生中断，在中断服务程序中读取转换值
    ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);//???
    
	/* 使能 ADC1 */
	ADC_Cmd(ADC1, ENABLE);
	
	/* 使能ADC1 复位校准寄存器 */
	ADC_ResetCalibration(ADC1);
	/* 检查ADC1的复位寄存器 */
	while(ADC_GetResetCalibrationStatus(ADC1));

	/* 启动ADC1校准 */
	ADC_StartCalibration(ADC1);
	/* 检查校准是否结束 */
	while(ADC_GetCalibrationStatus(ADC1));

	/* ADC1 常规软件启动转换 */
	ADC_SoftwareStartConvCmd( ADC1, ENABLE );
}
 
 
 
static void ADC_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
	// 优先级分组
    //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
 
    // 配置中断优先级
    NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;//ADC1_IRQn;
    //NVIC_InitStructure.NVIC_IRQChannelPriority = 0x03;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
  
//    // ADC2 转换结束产生中断，在中断服务程序中读取转换值
//    ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);

}
 
void ADC1_2_IRQHandler(void)
{	
    uint16_t adc_v;
    
	if (ADC_GetITStatus(ADC1,ADC_IT_EOC)==SET) 
	{
		// 读取ADC的转换值
		adc_v = ADC_GetConversionValue(ADC1);
		FILTER_Push( adc_v );//ADC1_Value = FILTER_Push( adc_v );
	}
	ADC_ClearITPendingBit(ADC1,ADC_IT_EOC);
}
 
void ADC1_Init(void)
{
    ADC1_InSigEnInit();
	ADC1_GPIO_Config();
	ADC1_Mode_Config();
	ADC_NVIC_Config();
	FILTER_Clear( );
}

void ADC1_GetV(void)
{
    uint8_t err;
    uint16_t AdcValue = 0;
    
	float voltage;
	
    err = FILTER_Get( &AdcValue );
    if( AdcValue == 0 ) //无效值
    {
        return;
    }else
    if( err<FILTER_N )
    {
        ;//未达到采样总数FILTER_N，输出的是即时数据
    }
    //printf("FILTER_Get%d %ld\r\n", err, AdcValue );
    
    voltage = ((float)AdcValue / 4096)*3.3*(ADC_R1+ADC_R2)/ADC_R2;

    
	Tmy.power_v = (uint16_t)( voltage*1000 );
	
    //printf("voltage:%f power_v:%d\r\n", voltage, Tmy.power_v );
    
	if( Tmy.power_v > MAX_POWER )
	{
		Tmy.power_v = MAX_POWER;
	}
	else if( Tmy.power_v <= MIN_POWER )
	{
		Tmy.power_v = MIN_POWER;
	}
    
	Tmy.power_percent = (uint8_t)( (Tmy.power_v - MIN_POWER) * 100 / (MAX_POWER - MIN_POWER) );
	if( Tmy.power_percent < 20 )
	{
		//Tmy.alarm.bit.power = 1;
	}else
	{
	    //Tmy.alarm.bit.power = 0;
	}
	printf("power_v:%d power_percent:%d\r\n", Tmy.power_v, Tmy.power_percent );
}

/******************************************************* END OF FILE ************************************************************/
