#ifndef __HX711_H
#define __HX711_H

#ifndef HX711_GLOBAL
#define HX711_EXT	extern
#else
#define HX711_EXT
#endif

//Includes ---------------------------------------------------------------------
#include "stm32f10x.h"

//#include"STM32f10x_lib.h"                //�������е�ͷ�ļ�
//#include <stdio.h> 

HX711_EXT unsigned long PressureTable[7];

//----------------��������--------------------

HX711_EXT void HX711_Init( void );
HX711_EXT unsigned long HX711_Read( void ); //��HX711оƬ��������ݡ�
HX711_EXT void PRE_Adjust( uint8_t n, unsigned long p );
HX711_EXT uint8_t PRE_GetLevel( unsigned long p );

#endif

/******************************************************* END OF FILE ************************************************************/