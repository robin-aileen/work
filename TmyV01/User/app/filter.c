
/*
*********************************************************************************************************
*
*	模块名称 : 滤波法
*	文件名称 : filter.c
*	版    本 : V1.0
*   M  C  U  : STM32F103RBT6
*   编 译 器 : IAR-Arm 8.22.2
*
*	说    明 : 滤波法
*              
*	修改记录 :
*		版本号  日期        作者     说明
*		V1.0    2019-03-19 robin   中位值滤波法, 中位值平均滤波法
*
*	Copyright (C), 2019-2039
*
*********************************************************************************************************
*/


#define FILTER_GLOBAL

#include "includes.h"//#include "bsp.h"

//中位值滤波法  
//    A、方法：  
//    连续采样N次（N取奇数）  
//    把N次采样值按大小排列  
//    取中间值为本次有效值  
//    B、优点：  
//    能有效克服因偶然因素引起的波动干扰  
//    对温度、液位的变化缓慢的被测参数有良好的滤波效果  
//    C、缺点：  
//    对流量、速度等快速变化的参数不宜

/* N值可根据实际情况调整  
排序采用冒泡法*/  
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

//中位值平均滤波法（又称防脉冲干扰平均滤波法）  
//A、方法：  
//相当于“中位值滤波法”+“算术平均滤波法”  
//连续采样N个数据，去掉一个最大值和一个最小值  
//然后计算N-2个数据的算术平均值  
//N值的选取：3~14  
//B、优点：  
//融合了两种滤波法的优点  
//对于偶然出现的脉冲性干扰，可消除由于脉冲干扰所引起的采样值偏差  
//C、缺点：  
//测量速度较慢，和算术平均滤波法一样  
//比较浪费RAM 

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
