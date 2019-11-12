
/*
*********************************************************************************************************
*
*	模块名称 : 定时器驱动模块
*	文件名称 : bsp_simole_timer.c
*	版    本 : V1.0
*   M  C  U  : STM32F103RBT6
*   编 译 器 : IAR-Arm 8.22.2
*
*	说    明 : 1秒定时器
*              
*	修改记录 :
*		版本号  日期        作者     说明
*		V1.0    2018-05-15 robin   正式发布
*
*	Copyright (C), 2018-2028
*
*********************************************************************************************************
*/
#define TIMER_GLOBAL

#include "bsp.h"
#include "bsp_motor.h"
#include "hmi_driver.h"

//100ms 定时
void timer2_init( void )
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    //打开相应的时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);

    //清除TIM2的中断待处理位:TIM 中断源
    TIM_ClearITPendingBit(TIM2,TIM_IT_Update);

    //设置定时器TIM_TimeBaseInit初始化定时器的时钟基数
//    TIM_TimeBaseStructure.TIM_Period = 480-1;//2000 -1;
//    TIM_TimeBaseStructure.TIM_Prescaler = 10000-1;//36000 -1;
    TIM_TimeBaseStructure.TIM_Period = 10000-1;//2000 -1;
    TIM_TimeBaseStructure.TIM_Prescaler = 71;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    //使能指定的TIM中断
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE );
		
		//TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);  //选择TIM2的update事件更新为触发源 用于ADC采集
		
    //使能TIM定时器
    TIM_Cmd(TIM2, ENABLE);

    //设置NVIC
    //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    //NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

//1s 定时
void timer3_init( void )
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    //打开相应的时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);

    //清除TIM3的中断待处理位:TIM 中断源
    TIM_ClearITPendingBit(TIM3,TIM_IT_Update);

    //设置定时器TIM_TimeBaseInit初始化定时器的时钟基数
    TIM_TimeBaseStructure.TIM_Period = 4800-1;//2000 -1;
    TIM_TimeBaseStructure.TIM_Prescaler = 10000-1;//36000 -1;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    //使能指定的TIM中断
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE );
		
		TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);  //选择TIM3的update事件更新为触发源 用于ADC采集
	
    //使能TIM定时器
    TIM_Cmd(TIM3, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;        
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;     
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
    NVIC_Init(&NVIC_InitStructure);
    
}

//100ms 定时
void TimeInit( void )
{
    timer2_init( );
    timer3_init( );
    
}

void TIM2_IRQHandler( void )
{
    //判断是否发生中断
    if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
    {
        //清除EXTI线路挂起位
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

//中断函数
//1s 中断
void TIM3_IRQHandler( void )
{
    //判断是否发生中断
    if(TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)
    {
        //清除EXTI线路挂起位
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
