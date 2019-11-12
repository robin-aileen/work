
/*
*********************************************************************************************************
*
*	ģ������ : IAP����ģ��
*	�ļ����� : save2flash.h
*	��    �� : V1.0
*   M  C  U  : STM32F103RBT6
*   �� �� �� : IAR-Arm 8.22.2
*
*	˵    �� : IAP��EEPROM
*              
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*		V1.0    2018-05-01 robin   ��ʽ����
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

//Ĭ�ϲ���
//#define TIME_LOCK_WAKE      (24*60*60) //�����Զ�����ʱ�䣬��λs
#define TIME_LOCK_WAKE      (24*60) //�����Զ�����ʱ�䣬��λ ����
#define TIME_LOCK_SLEEP     20 //�������ݽ�������ʱ�䣬��λs
#define TIME_NB_SLEEP       20 //nb�����ݽ�������ʱ�䣬��λs
#define THR_LOWPOWER        10 //�͵������ޣ���λ%

#define TIME_LOCK_HEART     20 //����ʱ�䣬��λs
#define COUNT_MAX_HEART     3  //�����ط�����

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
    
    uint8_t PWD[7]; //����
    uint8_t TOKEN[5];//����
    uint8_t KEY[17];//��Կ
    uint8_t NAME[16];//�㲥���� �15���ֽ�
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
