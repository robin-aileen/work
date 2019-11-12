/*
*********************************************************************************************************
*
*	模块名称 : ADC驱动模块
*	文件名称 : bsp_adc.h
*	版    本 : V1.0
*   M  C  U  : STM32F103RBT6
*   编 译 器 : IAR-Arm 8.22.2
*
*	说    明 : ADC驱动，检测电池电压
*
*	修改记录 :
*		版本号  日期        作者     说明
*		V1.0    2018-05-01 robin   正式发布
*
*	Copyright (C), 2018-2028
*
*********************************************************************************************************
*/

#ifndef __BSP_ADC_INT_H
#define __BSP_ADC_INT_H

#define ADC_OFF     0
#define ADC_ON      1

#define ADC_R1  75//100000
#define ADC_R2  10//200000

//extern uint16_t g_usAdcValue;
//extern uint8_t	g_powerpercent;

/* 供外部调用的函数声明 */
void ADC1_InSigEn( uint8_t onoff );
void ADC1_Init(void);
void ADC1_GetV(void);

#endif

/******************************************************* END OF FILE ************************************************************/
