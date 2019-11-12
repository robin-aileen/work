#define HX711_GLOBAL
#include "bsp.h"

/*Include---------------------------*/
#include "bsp.h"//#include"STM32f10x_lib.h"                //包含所有的头文件
#include <stdio.h> 

//#define delay_us( us ) bsp_DelayUS( us )

#define HX711_PORT      GPIOB
#define HX711_PIN_DATA  GPIO_Pin_9
#define HX711_PIN_SCK   GPIO_Pin_8

#define HX711_DATA_0 GPIO_ResetBits( HX711_PORT, HX711_PIN_DATA )
#define HX711_DATA_1 GPIO_SetBits( HX711_PORT, HX711_PIN_DATA )

#define HX711_SCK_0 GPIO_ResetBits( HX711_PORT, HX711_PIN_SCK )
#define HX711_SCK_1 GPIO_SetBits( HX711_PORT, HX711_PIN_SCK )

#define HX711_DATA_CHK GPIO_ReadInputDataBit( HX711_PORT, HX711_PIN_DATA )

void HX711_Init( void )
{
    //CLK:PB5  CLR:PE11  Data:PE10
    GPIO_InitTypeDef        GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = HX711_PIN_DATA;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(HX711_PORT,&GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = HX711_PIN_SCK; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;           
    GPIO_Init(HX711_PORT,&GPIO_InitStructure);

}

//unsigned long HX711_Read_B32(void)  //读HX711芯片输出的数据。 
unsigned long HX711_Read(void)
{ 
    unsigned long val = 0; 
    unsigned char i = 0; 
    
    val = 0;
    
    HX711_DATA_1;   //DOUT=1 
    HX711_SCK_0;    //SCK=0
    
    while( HX711_DATA_CHK ); //等待DOUT=0  
    delay_us(1); 
    for(i=0;i<24;i++) 
    { 
        HX711_SCK_1;    //SCK=1 
        val=val<<1;   
        HX711_SCK_0;    //SCK=0 
        if( HX711_DATA_CHK ) //DOUT=1 
        val++; 
    } 
    HX711_SCK_1; 
    delay_us(1); 
    val=val^0x800000;//第25个脉冲下降沿来时，转换数据
    HX711_SCK_0; //
    //HX711_DATA_1; 
    delay_us(1);  
    
    return val;
}  
//
//unsigned long HX711_Read(void)
//{ 
//    unsigned long val = 0; 
//    unsigned char i = 0; 
//    
//    val = 0;
//    
//    HX711_DATA_1;   //DOUT=1 
//    HX711_SCK_0;    //SCK=0
//    
//    while( HX711_DATA_CHK ); //等待DOUT=0  
//    delay_us(1); 
//    for(i=0;i<24;i++) 
//    { 
//        HX711_SCK_1;    //SCK=1 
//        val=val<<1; 
//        //delay_us(1);  
//        HX711_SCK_0;    //SCK=0 
//        if( HX711_DATA_CHK ) //DOUT=1 
//        val++; 
//       //delay_us(1); 
//    } 
////    HX711_SCK_1; 
////    delay_us(1); 
////    HX711_SCK_0; //
////    delay_us(1); 
//    HX711_SCK_1; 
//    delay_us(1); 
//    val=val^0x800000;//第25个脉冲下降沿来时，转换数据
//    HX711_SCK_0; //
//    //HX711_DATA_1; 
//    delay_us(1);  
//    
//    return val;
//}  
//3,5,7,9,11,11.5
typedef enum
{
    KG0=0,
	KG3,
	KG5,
	KG6,
	KG7,
	KG11,
	KG11_5
}ENUM_PRESSURE;

//unsigned long PressureTable[7];


//设置压力校准值
void PRE_Adjust( uint8_t n, unsigned long p )
{
    PressureTable[ n ] = p;
    //save2flash
    data_saved.data.Pressure[n] = p;
    
    SaveToFlash();
}


//得到压力所在等级
uint8_t PRE_GetLevel( unsigned long p )
{
    uint8_t i;
    
    for(i=0; i<7; i++)
    {
        if( *( PressureTable + i ) >= p ) return i;
    }
    
    return 6;
}
