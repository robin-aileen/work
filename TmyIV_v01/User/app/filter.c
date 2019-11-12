
/*
*********************************************************************************************************
*
*	ģ������ : �˲���
*	�ļ����� : filter.c
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


#define FILTER_GLOBAL

#include "includes.h"//#include "bsp.h"

//��λֵ�˲���  
//    A��������  
//    ��������N�Σ�Nȡ������  
//    ��N�β���ֵ����С����  
//    ȡ�м�ֵΪ������Чֵ  
//    B���ŵ㣺  
//    ����Ч�˷���żȻ��������Ĳ�������  
//    ���¶ȡ�Һλ�ı仯�����ı�����������õ��˲�Ч��  
//    C��ȱ�㣺  
//    ���������ٶȵȿ��ٱ仯�Ĳ�������

/* Nֵ�ɸ���ʵ���������  
�������ð�ݷ�*/  
void FILTER_Clear( void )  
{
    FilterTem.i_in = 0;
    FilterTem.lg_ok = 0;
}

uint8_t FILTER_Push( FILTER_DATA_TYPE data ) 
{  

    FilterTem.Buf[ FilterTem.i_in++ ] = data;
    
    if( FilterTem.i_in > FILTER_N-1 ) 
    {
        FilterTem.i_in = 0;
        FilterTem.lg_ok = 1;
    }
        
    //if( FilterTem.i_in > FILTER_N-1 ) FilterTem.lg_ok = 1;
    return FilterTem.i_in;
}


#ifdef FILTER_TYPE_ZWZ

uint8_t FILTER_Get( FILTER_DATA_TYPE *data )  
{  
    FILTER_DATA_TYPE temp, value_buf[FILTER_N];  
    uint8_t count, i, j;  
    
    if( !FilterTem.lg_ok ) 
    {
        if( FilterTem.i_in == 0 ) return 0;
        *data = FilterTem.Buf[ FilterTem.i_in-1 ];
        return FilterTem.i_in;//FilterTem.lg_ok;
    }
    
    for ( count=0; count<FILTER_N; count++ )  
    {   
        value_buf[count] = FilterTem.Buf[ count ];
    }
    
    for (j=0;j<FILTER_N-1;j++)  
    {  
        for (i=0;i<FILTER_N-j;i++)  
       {  
             if ( value_buf[i]>value_buf[i+1] )  
             {  
                   temp = value_buf[i];  
                   value_buf[i] = value_buf[i+1];  
                   value_buf[i+1] = temp;  
             }  
        }  
     }
     
     *data = value_buf[(FILTER_N-1)/2];
     return FilterTem.i_in;//FilterTem.lg_ok;
}

#endif

#ifdef FILTER_TYPE_ZZPJ

//��λֵƽ���˲������ֳƷ��������ƽ���˲�����  
//A��������  
//�൱�ڡ���λֵ�˲�����+������ƽ���˲�����  
//��������N�����ݣ�ȥ��һ�����ֵ��һ����Сֵ  
//Ȼ�����N-2�����ݵ�����ƽ��ֵ  
//Nֵ��ѡȡ��3~14  
//B���ŵ㣺  
//�ں��������˲������ŵ�  
//����żȻ���ֵ������Ը��ţ������������������������Ĳ���ֵƫ��  
//C��ȱ�㣺  
//�����ٶȽ�����������ƽ���˲���һ��  
//�Ƚ��˷�RAM 

uint8_t FILTER_Get( FILTER_DATA_TYPE *data )    
{  
    char count, i, j;
    FILTER_DATA_TYPE temp, value_buf[FILTER_N];  
    
    uint32_t sum=0;  
    
    if( !FilterTem.lg_ok ) 
    {
        if( FilterTem.i_in == 0 ) return 0;
        *data = FilterTem.Buf[ FilterTem.i_in-1 ];
        return FilterTem.i_in;//FilterTem.lg_ok;
    }
    
    for (count=0; count<FILTER_N; count++)  
    {  
        value_buf[count] = FilterTem.Buf[ count ];
    }  
    for (j=0;j<FILTER_N-1;j++)  
    {  
        for (i=0;i<FILTER_N-j;i++)  
        {  
             if ( value_buf[i]>value_buf[i+1] )  
            {  
                 temp = value_buf[i];  
                 value_buf[i] = value_buf[i+1];  
                 value_buf[i+1] = temp;  
            }  
         }  
    }  
    for(count=1;count<FILTER_N-1;count++)  
    sum += value_buf[count];
    
    *data = (FILTER_DATA_TYPE)(sum/(FILTER_N-2));
     return FilterTem.i_in;
     
} 

#endif

/******************************************************* END OF FILE ************************************************************/
