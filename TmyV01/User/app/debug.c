
/*
*********************************************************************************************************
*
*	模块名称 : debug模块
*	文件名称 : debug.c
*	版    本 : V1.0
*   M  C  U  : STM32F103RBT6
*   编 译 器 : IAR-Arm 8.22.2
*
*	说    明 : 
*              
*	修改记录 :
*		版本号  日期        作者     说明
*		V1.0    2018-07-12 robin   正式发布
*
*	Copyright (C), 2018-2028
*
*********************************************************************************************************
*/

#define DEBUG_GLOBAL

#include "includes.h"


void debug_en( uint8_t en )
{
    lg_debug_en = en;
}

void debug_usr_hex( uint8_t *hexbuf, uint16_t len )
{
    if( lg_debug_en ) 
    {
        uint16_t i;
        
        for( i=0; i<len; i++ )
        {
            comSendCharHex( COM1, hexbuf[i] );
        }
    }
}

void debug_at( const char * _Format, ...)
{
    #if DEBUG_AT == 1
    
    if( lg_debug_en & 0x02 ) printf( _Format );
        
    #endif
   ;
}

void debug_ble( const char * _Format, ...)
{
    #if DEBUG_BLE == 1
    
    if( lg_debug_en & 0x04 ) printf( _Format );
        
    #endif
   ;
}

void debug_usr( const char * _Format, ...)
{
    #if DEBUG_USR == 1
    
    if( lg_debug_en & 0x08 ) printf( _Format );
        
    #endif
   ;
}

/******************************************************* END OF FILE ************************************************************/
