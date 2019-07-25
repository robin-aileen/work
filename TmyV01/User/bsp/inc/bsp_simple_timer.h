
/*
*********************************************************************************************************
*
*	模块名称 : 定时器驱动模块
*	文件名称 : bsp_simole_timer.h
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


#ifndef _TIMER_H
#define _TIMER_H

#ifndef TIMER_GLOBAL 
#define TIMER_EXT	extern 
#else 
#define TIMER_EXT
#endif

TIMER_EXT uint8_t timer_pedal_up;
TIMER_EXT uint8_t timer_pedal_down;

void TimeInit( void );

#endif

/******************************************************* END OF FILE ************************************************************/
