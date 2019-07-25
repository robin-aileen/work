/*
*********************************************************************************************************
*
*	ģ������ : ADC����ģ��
*	�ļ����� : bsp_adc.c
*	��    �� : V1.0
*   M  C  U  : STM32F103RBT6
*   �� �� �� : IAR-Arm 8.22.2
*
*	˵    �� : ADC����������ص�ѹ
*
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*		V1.0    2018-03-20 robin    ADC�ɼ�Ϊ����ģʽ��T3�жϴ�����ÿ��һ�Σ����������˲�����;
*	Copyright (C), 2019-2029
*
*********************************************************************************************************
*/
#include "bsp.h"

/* ADC��Ӧ��RCCʱ�� */

#define RCC_ALL_ADC 	(RCC_APB2Periph_GPIOC)

#define GPIO_PORT_ADC_EN    GPIOC
#define GPIO_PIN_ADC_EN	    GPIO_Pin_1
#define	GPIO_PORT_AD_DAT    GPIOC
#define GPIO_PIN_ADC_DAT    GPIO_Pin_0

#define	MAX_POWER		25200
#define	MIN_POWER		16500

//// ����һ���������ADCת��ֵ 
//__IO uint16_t ADC1_Value;

 /*
*********************************************************************************************************
*	�� �� ��: ADC1_InSigEn
*	����˵��: ADC�ź�Դ����ʹ��
*	��    ��:
*	�� �� ֵ: ��
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

	/* ��GPIOʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_AHBPeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);
	
	ADC1_InSigEn(ADC_OFF);

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//�������ģʽ
	
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_ADC_EN;
	GPIO_Init(GPIO_PORT_ADC_EN, &GPIO_InitStructure);
    
	ADC1_InSigEn( ADC_ON );
}

static void ADC1_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	/* ʹ�� ADC1 and GPIOC clock */
	RCC_AHBPeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);
    
	/* ����PA6Ϊģ������(ADC Channel14) */
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_ADC_DAT;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIO_PORT_AD_DAT, &GPIO_InitStructure);
}
 
 
 
 
static void ADC1_Mode_Config(void)
{
    ADC_InitTypeDef ADC_InitStructure;
	
	/* ʹ�� ADC1 and GPIOC clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    /* ����ADC1, ����DMA, ��������� */
	//ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	
	//����ת������ʱ��T3����
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; //ADC ����ģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE; //��ͨ��ģʽ
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; //����ת��ģʽ
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO;//�ⲿ��������ΪTIM3
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//ADC �����Ҷ���
    
	ADC_InitStructure.ADC_NbrOfChannel = 1;//˳����й���ת����ADC ͨ������Ŀ
	ADC_Init(ADC1, &ADC_InitStructure);

	
	
//	//����ת��ģʽ
//	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
//	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	
	
//    //����ת������ʱ��T3����
//	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;            //��������ת��ģʽ  ENABLE;
//	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO; //�ⲿ��������ΪTIM3
//	
//	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
//	//ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Backward;
//	ADC_Init(ADC1, &ADC_InitStructure);
    
    
	//RCC_ADCCLKConfig(RCC_ADCCLK_PCLK_Div4);
	
    /* ����ADC1 ����ͨ��14 channel9 configuration */
	//ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_55Cycles5);//ADC_ChannelConfig(ADC1, ADC_Channel_1, ADC_SampleTime_55_5Cycles); //ADC_SampleTime_239_5Cycles
    ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_55Cycles5);
	/* ʧ�� ADC1 */
	ADC_Cmd(ADC1, DISABLE);
	
	/* ADC У׼ */
	//ADC_GetCalibrationFactor(ADC1);
	//while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADCAL));//?
	
	// ADC2 ת�����������жϣ����жϷ�������ж�ȡת��ֵ
    ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);//???
    
	/* ʹ�� ADC1 */
	ADC_Cmd(ADC1, ENABLE);
	
	/* ʹ��ADC1 ��λУ׼�Ĵ��� */
	ADC_ResetCalibration(ADC1);
	/* ���ADC1�ĸ�λ�Ĵ��� */
	while(ADC_GetResetCalibrationStatus(ADC1));

	/* ����ADC1У׼ */
	ADC_StartCalibration(ADC1);
	/* ���У׼�Ƿ���� */
	while(ADC_GetCalibrationStatus(ADC1));

	/* ADC1 �����������ת�� */
	ADC_SoftwareStartConvCmd( ADC1, ENABLE );
}
 
 
 
static void ADC_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
	// ���ȼ�����
    //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
 
    // �����ж����ȼ�
    NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;//ADC1_IRQn;
    //NVIC_InitStructure.NVIC_IRQChannelPriority = 0x03;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
  
//    // ADC2 ת�����������жϣ����жϷ�������ж�ȡת��ֵ
//    ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);

}
 
void ADC1_2_IRQHandler(void)
{	
    uint16_t adc_v;
    
	if (ADC_GetITStatus(ADC1,ADC_IT_EOC)==SET) 
	{
		// ��ȡADC��ת��ֵ
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
    if( AdcValue == 0 ) //��Чֵ
    {
        return;
    }else
    if( err<FILTER_N )
    {
        ;//δ�ﵽ��������FILTER_N��������Ǽ�ʱ����
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
