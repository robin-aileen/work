
//#include "includes.h"
#include "bsp.h"

#define BSP_IO_GLOBAL



void bsp_io_Config(void)
{
    //relay PC0~PC7
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

		GPIO_SetBits( GPIOC, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 );	 // turn off all relay
	
		//RS485 PD0,PD1,PD3,PD4
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_3 | GPIO_Pin_4;	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

		//GPIO_SetBits( GPIOD, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_3 | GPIO_Pin_4 );	 // Rs485 txd mode
		GPIO_ResetBits( GPIOD, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_3 | GPIO_Pin_4 );  // Rs485 rxd mode
		//GPIO_SetBits( GPIOD, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_3 | GPIO_Pin_4 );	 // Rs485 txd mode
		//GPIO_ResetBits( GPIOD, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_3 | GPIO_Pin_4 );  // Rs485 rxd mode
	
		//led PE8~PE14
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOE, ENABLE); //xxx

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

		GPIO_SetBits( GPIOE, GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 );	 // turn off all led
	
		//beep PE15
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOE, ENABLE); //xxx

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

		GPIO_SetBits( GPIOE, GPIO_Pin_15 );	 // turn off beep
		//GPIO_ResetBits( GPIOE, GPIO_Pin_15 );
	
		//EAT1~EAT8 : PD8~PD15
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);
 		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 

		GPIO_Init(GPIOD, &GPIO_InitStructure);
	
		//TEST : PE0~PE7
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);
 		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 

		GPIO_Init(GPIOE, &GPIO_InitStructure);
	
}

//u8 Key_Scan(GPIO_TypeDef* GPIOx,u16 GPIO_Pin)
//{			
//		/*?足2a那?﹞?車D～∩?邦～∩?? */
//   	if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == KEY_ON ) 
//	  {	   
//	    return 	KEY_ON;	
////	 	 /*?車那㊣????*/
////	  	Delay(10000);		
////	   		if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == KEY_ON )  
////					{	 
//////						/*米豕∩y～∩?邦那赤﹞? */
//////						while(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == KEY_ON);   
////			 			return 	KEY_ON;	 
////					}
////			else
////					return KEY_OFF;
//		}
//	else
//		return KEY_OFF;
//}

void BuzzerRing( void )
{
	//if( ( ~GetSwNum( ) ) & 0x80 )
	{
	    BEEP_CTRL( BEEP_ON );
		//BEEP( 1 );//FIO0SET |= Buzzer;
		//OSTimeDly( 20 );
		BEEP_CTRL( BEEP_OFF );
		//BEEP( 0 );//FIO0CLR |= Buzzer;
	}
}

void BuzzerRingN( uint8_t n )
{
	uint8_t i;
	for( i=0; i<n; i++ )
	{
	    BuzzerRing( );
        //OSTimeDly( 20 );
	}
}

