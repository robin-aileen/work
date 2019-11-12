
/*
*********************************************************************************************************
*
*	模块名称 : 滤波法
*	文件名称 : filter.h
*	版    本 : V1.0
*   M  C  U  : STM32F103RBT6
*   编 译 器 : IAR-Arm 8.22.2
*
*	说    明 : 滤波法
*              
*	修改记录 :
*		版本号  日期        作者     说明
*		V1.0    2019-03-19 robin   中位值滤波法, 中位值平均滤波法
*
*	Copyright (C), 2019-2039
*
*********************************************************************************************************
*/


#ifndef _FILTER_H
#define _FILTER_H

#ifndef FILTER_GLOBAL

#define FILTER_EXT extern
#else
#define FILTER_EXT
#endif


//#define FILTER_TYPE_ZWZ //中位值滤波法
#define FILTER_TYPE_ZZPJ //中位值平均滤波法（又称防脉冲干扰平均滤波法）

#define FILTER_N 11 //中位值滤波法必须是奇数

#define FILTER_DATA_TYPE uint16_t

typedef struct
{
    uint8_t i_in;
    uint8_t lg_ok;
    FILTER_DATA_TYPE Buf[ FILTER_N ];
    
}TYPE_FILTER;

FILTER_EXT TYPE_FILTER FilterTem;

FILTER_EXT void FILTER_Clear( void );
FILTER_EXT uint8_t FILTER_Push( FILTER_DATA_TYPE data );
FILTER_EXT uint8_t FILTER_Get( FILTER_DATA_TYPE *data );

#endif

/******************************************************* END OF FILE ************************************************************/
