
/*
*********************************************************************************************************
*
*	ģ������ : ��ʱ������ģ��
*	�ļ����� : bsp_simole_timer.c
*	��    �� : V1.0
*   M  C  U  : STM32F103RBT6
*   �� �� �� : IAR-Arm 8.22.2
*
*	˵    �� : 1�붨ʱ��
*              
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*		V1.0    2018-05-15 robin   ��ʽ����
*
*	Copyright (C), 2018-2028
*
*********************************************************************************************************
*/
#define TIMER_GLOBAL

#include "bsp.h"
#include "bsp_motor.h"
#include "hmi_driver.h"

//100ms ��ʱ
void timer2_init( void )
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    //����Ӧ��ʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);

    //���TIM2���жϴ�����λ:TIM �ж�Դ
    TIM_ClearITPendingBit(TIM2,TIM_IT_Update);

    //���ö�ʱ��TIM_TimeBaseInit��ʼ����ʱ����ʱ�ӻ���
//    TIM_TimeBaseStructure.TIM_Period = 480-1;//2000 -1;
//    TIM_TimeBaseStructure.TIM_Prescaler = 10000-1;//36000 -1;
    TIM_TimeBaseStructure.TIM_Period = 10000-1;//2000 -1;
    TIM_TimeBaseStructure.TIM_Prescaler = 71;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    //ʹ��ָ����TIM�ж�
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE );
		
		//TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);  //ѡ��TIM2��update�¼�����Ϊ����Դ ����ADC�ɼ�
		
    //ʹ��TIM��ʱ��
    TIM_Cmd(TIM2, ENABLE);

    //����NVIC
    //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    //NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

//1s ��ʱ
void timer3_init( void )
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    //����Ӧ��ʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);

    //���TIM3���жϴ�����λ:TIM �ж�Դ
    TIM_ClearITPendingBit(TIM3,TIM_IT_Update);

    //���ö�ʱ��TIM_TimeBaseInit��ʼ����ʱ����ʱ�ӻ���
    TIM_TimeBaseStructure.TIM_Period = 4800-1;//2000 -1;
    TIM_TimeBaseStructure.TIM_Prescaler = 10000-1;//36000 -1;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    //ʹ��ָ����TIM�ж�
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE );
		
		TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);  //ѡ��TIM3��update�¼�����Ϊ����Դ ����ADC�ɼ�
	
    //ʹ��TIM��ʱ��
    TIM_Cmd(TIM3, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;        
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;     
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
    NVIC_Init(&NVIC_InitStructure);
    
}

//100ms ��ʱ
void TimeInit( void )
{
    timer2_init( );
    timer3_init( );
    
}

void TIM2_IRQHandler( void )
{
    //�ж��Ƿ����ж�
    if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
    {
        //���EXTI��·����λ
        TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
        
        if( timer_lcd_update ) timer_lcd_update--;
        if( timer_beep ) timer_beep--;
        
        bsp_KeyScan();

        if( GPIO_ReadInputData( GPIO_PORT_BLE_LINK ) & GPIO_PIN_BLE_LINK )
        {
            if(timer_ble_netledon > 80)
            {
                Ble.state.bit.link = 1;
            }else
            {
                timer_ble_netledon++;
            }
        }else
        {
            timer_ble_netledon = 0;
            Ble.state.bit.link = 0;
        }
    }
}

//�жϺ���
//1s �ж�
void TIM3_IRQHandler( void )
{
    //�ж��Ƿ����ж�
    if(TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)
    {
        //���EXTI��·����λ
        TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
        
        if( timer_pedal_up ) timer_pedal_up++;
	    if( timer_pedal_down ) timer_pedal_down++;
        if( timer_dg_init ) timer_dg_init--;

        // if( GPIO_ReadInputData( GPIO_PORT_BLE_LINK ) & GPIO_PIN_BLE_LINK )
        // {
        //     if(timer_ble_netledon > 2)
        //     {
        //         Ble.state.bit.link = 1;
        //     }else
        //     {
        //         timer_ble_netledon++;
        //     }
        // }else
        // {
        //     timer_ble_netledon = 0;
        //     Ble.state.bit.link = 0;
        // }
    }
}

/******************************************************* END OF FILE ************************************************************/
