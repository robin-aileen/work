
/*
*********************************************************************************************************
*
*	ģ������ : �˲���
*	�ļ����� : filter.h
*	��    �� : V1.0
*   M  C  U  : STM32F103RBT6
*   �� �� �� : IAR-Arm 8.22.2
*
*	˵    �� : �˲���
*              
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*		V1.0    2019-03-19 robin   ��λֵ�˲���, ��λֵƽ���˲���
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


//#define FILTER_TYPE_ZWZ //��λֵ�˲���
#define FILTER_TYPE_ZZPJ //��λֵƽ���˲������ֳƷ��������ƽ���˲�����

#define FILTER_N 11 //��λֵ�˲�������������

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
