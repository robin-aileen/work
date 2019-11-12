
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
