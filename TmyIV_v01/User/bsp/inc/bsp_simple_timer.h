
/*
*********************************************************************************************************
*
*	ģ������ : ��ʱ������ģ��
*	�ļ����� : bsp_simole_timer.h
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


#ifndef _TIMER_H
#define _TIMER_H

#ifndef TIMER_GLOBAL 
#define TIMER_EXT	extern 
#else 
#define TIMER_EXT
#endif

TIMER_EXT uint8_t timer_pedal_up;
TIMER_EXT uint8_t timer_pedal_down;
TIMER_EXT uint8_t timer_dg_init; //���˳�ʼ��ʱ��
TIMER_EXT uint8_t timer_ble_netledon; //����

void TimeInit( void );

#endif

/******************************************************* END OF FILE ************************************************************/