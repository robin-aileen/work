
/*
*********************************************************************************************************
*
*	模块名称 : debug模块
*	文件名称 : debug.h
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


#ifndef _DEBUG_H
#define _DEBUG_H

#ifndef DEBUG_GLOBAL

#define DEBUG_EXT extern
#else
#define DEBUG_EXT
#endif

#define DEBUG_EN        1 //编译使能 0：不编译 1：编译

#define DEBUG_DISABLE   0 //软件使能debug输出
#define DEBUG_ENABLE    1 //软件禁止debug输出

#define DEBUG_AT        1//DEBUG_ENABLE //
#define DEBUG_USR       1//DEBUG_ENABLE //
#define DEBUG_BLE       1//

DEBUG_EXT uint8_t lg_debug_en;

//void debug_printf( uint8_t type, const char * _Format, ...);
void debug_printf( const char * _Format, ...);
void debug_ble( const char * _Format, ...);
void debug_at( const char * _Format, ...);
void debug_usr( const char * _Format, ...);
void debug_en( uint8_t en );
void debug_usr_hex( uint8_t *hexbuf, uint16_t len );

#endif

/******************************************************* END OF FILE ************************************************************/
