
/*
*********************************************************************************************************
*
*	模块名称 : IAP驱动模块
*	文件名称 : save2flash.h
*	版    本 : V1.0
*   M  C  U  : STM32F103RBT6
*   编 译 器 : IAR-Arm 8.22.2
*
*	说    明 : IAP做EEPROM
*              
*	修改记录 :
*		版本号  日期        作者     说明
*		V1.0    2018-05-01 robin   正式发布
*
*	Copyright (C), 2018-2028
*
*********************************************************************************************************
*/

#ifndef _SAVE2FLASH_H 
#define _SAVE2FLASH_H

#ifndef SAVE2FLASH_GLOBAL 
#define SAVE2FLASH_EXT	extern 
#else 
#define SAVE2FLASH_EXT 
#endif 

#include "bsp.h"
//----------------------------------------------------------------------------------------------------------------------

#define F_BASE             0

#define BIT_LG_TIME 0x01

#define FLASH_DATA_SIZE ( sizeof(TYPE_SAVE_DATA)/2 )

//默认参数
//#define TIME_LOCK_WAKE      (24*60*60) //锁无自动唤醒时间，单位s
#define TIME_LOCK_WAKE      (24*60) //锁无自动唤醒时间，单位 分钟
#define TIME_LOCK_SLEEP     20 //锁无数据进入休眠时间，单位s
#define TIME_NB_SLEEP       20 //nb无数据进入休眠时间，单位s
#define THR_LOWPOWER        10 //低电量门限，单位%

#define TIME_LOCK_HEART     20 //心跳时间，单位s
#define COUNT_MAX_HEART     3  //激活重发次数

#define SAVE_LG_MASK 0x001F

typedef struct 
{
    uint16_t ip             : 1;
    uint16_t EnKey          : 1;
    uint16_t time_sleep     : 1;
    uint16_t time_wake      : 1;
    uint16_t threshold_lowpower : 1;
    uint16_t                : 11;
}TYPE_SAVE_LG_BIT;

typedef union
{
    TYPE_SAVE_LG_BIT bit;
    uint16_t buf;
}TYPE_SAVE_LG;

typedef struct
{
    uint8_t threshold_lowpower;
    
    uint8_t PWD[7]; //密码
    uint8_t TOKEN[5];//令牌
    uint8_t KEY[17];//秘钥
    uint8_t NAME[16];//广播名称 最长15个字节
    char IP[16];
    char PORT[8];
    uint8_t EncKey[5];
    TYPE_SAVE_LG lg;
    uint32_t time_sleep;
    uint32_t time_wake;
    //uint32_t time_heart;
    
}TYPE_SAVE_DATA;

typedef union
{
    TYPE_SAVE_DATA data;
    uint16_t buf[ FLASH_DATA_SIZE ];//uint16_t buf[ FLASH_DATA_SIZE ];
}TYPE_FLASH_EEPROM;

SAVE2FLASH_EXT TYPE_FLASH_EEPROM data_saved;

SAVE2FLASH_EXT void SaveToFlash( void );
SAVE2FLASH_EXT uint8_t ReadFromFlash( void );

#endif

/******************************************************* END OF FILE ************************************************************/
