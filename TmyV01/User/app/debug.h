
/*
*********************************************************************************************************
*
*	ģ������ : debugģ��
*	�ļ����� : debug.h
*	��    �� : V1.0
*   M  C  U  : STM32F103RBT6
*   �� �� �� : IAR-Arm 8.22.2
*
*	˵    �� : 
*              
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*		V1.0    2018-07-12 robin   ��ʽ����
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

#define DEBUG_EN        1 //����ʹ�� 0�������� 1������

#define DEBUG_DISABLE   0 //���ʹ��debug���
#define DEBUG_ENABLE    1 //�����ֹdebug���

//#if DEBUG_EN == 0
//
//    #define DEBUG_BLE       0
//    #define DEBUG_AT        0
//    #define DEBUG_USR       0
//#else
    
    #define DEBUG_BLE       0//DEBUG_ENABLE //
    #define DEBUG_AT        1//DEBUG_ENABLE //
    #define DEBUG_USR       1//DEBUG_ENABLE //

//#endif

DEBUG_EXT uint8_t lg_debug_en;

DEBUG_EXT void debug_en( uint8_t en );
DEBUG_EXT void debug_ble( const char * _Format, ...);
DEBUG_EXT void debug_at( const char * _Format, ...);
DEBUG_EXT void debug_usr( const char * _Format, ...);
DEBUG_EXT void debug_usr_hex( uint8_t *hexbuf, uint16_t len );

#endif

/******************************************************* END OF FILE ************************************************************/
