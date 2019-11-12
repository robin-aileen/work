
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


#define STM32_FLASH_SIZE    128  //K
#define	STM32_SECTOR_SIZE	1024

#define EEPROM1_SIZE        1   //K 不经常更新的数据
#define EEPROM2_SIZE        2   //K 频繁更新的小量数据 调用eeprom.c
#define	EEPROM3_SIZE	    40  //K 频繁更新的大块数据


//#define EEPROM_START_ADDR   (0x08000000 + (STM32_FLASH_SIZE-EEPROM3_SIZE-EEPROM2_SIZE-EEPROM1_SIZE) * STM32_SECTOR_SIZE)	//

#define EEPROM1_START_ADDR      (0x08000000 + (STM32_FLASH_SIZE-EEPROM3_SIZE-EEPROM2_SIZE-EEPROM1_SIZE) * STM32_SECTOR_SIZE)	// EEPROM_START_ADDR
#define EEPROM2_START_ADDR      (0x08000000 + (STM32_FLASH_SIZE-EEPROM3_SIZE-EEPROM2_SIZE) * STM32_SECTOR_SIZE)		//
#define EEPROM3_START_ADDR		(0x08000000 + (STM32_FLASH_SIZE-EEPROM3_SIZE) * STM32_SECTOR_SIZE)		//
#define	EEPROM3_MAX_ADDR_OFFSET EEPROM3_SIZE*STM32_SECTOR_SIZE

//------------------------------------------------------------------------------------------------------------------------------------------

#define EEPROM2_W_OFFSET 0
#define EEPROM2_R_OFFSET 1

typedef struct
{
    uint16_t write_addr_offset;
    uint16_t read_addr_offset;
}TYPE_BKD;

SAVE2FLASH_EXT TYPE_BKD Bkd;

//------------------------------------------------------------------------------------------------------------------------------------------


#define F_BASE              0

#define BIT_LG_TIME 0x01

#define FLASH_DATA_SIZE ( sizeof(TYPE_SAVE_DATA)/2 )

//默认参数
#define TIME_NB_SLEEP       10 //nb无数据进入休眠时间，单位s
#define TIME_NB_WAKE        30 //nb自动唤醒时间，单位s
#define TIME_NB_HEART       30 //心跳时间，单位s

#define TIME_LOCK_HEART     20 //心跳时间，单位s
#define COUNT_MAX_HEART     3  //激活重发次数

#define SAVE_LG_MASK        0x001F

#define TRAIN_TPYE          'K'     //车次字母
#define TRAIN_NUM_H         0x00    //车次号高
#define TRAIN_NUM_L         0x64    //车次号低
#define MAX_ROOM_N          20      //最大车厢数

#define DLY_MOVE            0       //开车时轮询间隔，单位s
#define DLY_STOP            5       //停车时轮询间隔，单位s

#define TIME_HEART          (60*3) //列车员心跳时间，单位s，用于主动上报模式

typedef struct
{
	uint16_t	write_addr_offset;
	uint16_t	read_addr_offset;
}MSG_SAVE_ADDR;

SAVE2FLASH_EXT MSG_SAVE_ADDR	MsgSaveAddr;

typedef struct 
{
    uint16_t v_init         : 1;//处理一些一般参数，一般初始化为0
    uint16_t ip             : 1;
    uint16_t systime        : 1;//uint16_t EnKey          : 1;
    uint16_t time_sleep     : 1;
    uint16_t time_wake      : 1;
    uint16_t                : 11;
}TYPE_SAVE_LG_BIT;

typedef union
{
    TYPE_SAVE_LG_BIT bit;
    uint16_t buf;
}TYPE_SAVE_LG;

typedef struct
{
    uint8_t TrainType;//车次头字母
    uint8_t TrainNum[2];//车次
    uint8_t max_room_n;
    
    char IP[16];
    char PORT[8];
    uint8_t n_error;
    uint8_t n_reset;
    
    uint8_t dly_move;
    uint8_t dly_stop;
    
    TYPE_SAVE_LG lg;
    uint16_t io_out;
    
    uint32_t time_sleep;
    uint32_t time_wake;
    uint32_t time_wake_s;
    uint32_t time_heart;

    uint32_t Pressure[7]; //压力数值

    
}TYPE_SAVE_DATA;

typedef union
{
    TYPE_SAVE_DATA data;
    uint16_t buf[ FLASH_DATA_SIZE ];
}TYPE_FLASH_EEPROM;

SAVE2FLASH_EXT TYPE_FLASH_EEPROM data_saved;

SAVE2FLASH_EXT void SaveToFlash( void );
SAVE2FLASH_EXT uint8_t ReadFromFlash( void );
SAVE2FLASH_EXT uint8_t SaveMsgToFlash( uint8_t *data, uint8_t len );
SAVE2FLASH_EXT uint8_t ReadMsgToRam( uint8_t *data, uint8_t len );

SAVE2FLASH_EXT uint16_t msg_get_num( void );

SAVE2FLASH_EXT uint8_t bkd_save( void );
SAVE2FLASH_EXT uint8_t bkd_read( void );
#endif

/******************************************************* END OF FILE ************************************************************/
