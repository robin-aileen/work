//io.h

// =====================================================================================================================
// | File Name		: io.h
// | Summary 			: 
// | Version 			: 1.00 
// | CPU	 				: STM32103F
// | Compile 			：keil
// | Author  			：Robin
// | Start Date 	：2019-03-10
// | Finish Date 	：
// =====================================================================================================================

#ifndef BSP_IO_H
#define BSP_IO_H 

#ifndef BSP_IO_GLOBAL 
#define BSP_IO_EXT	extern 
#else 
#define BSP_IO_EXT 
#endif 

////PC--------------------------------------------------------------------------------------------------------------------
//#define RLY_ON      1
//#define RLY_OFF     0
//#define RLY_PORT    GPIOC
//
//#define RLY1   GPIO_Pin_0 //PC0
//#define RLY2   GPIO_Pin_1 //PC1
//#define RLY3   GPIO_Pin_2 //PC2
//#define RLY4   GPIO_Pin_3 //PC3
//#define RLY5   GPIO_Pin_4 //PC4
//#define RLY6   GPIO_Pin_5 //PC5
//#define RLY7   GPIO_Pin_6 //PC6
//
//#define RLY_CTRL( p, s )	if( s )\
//					GPIO_SetBits( RLY_PORT, p );\
//					else		\
//					GPIO_ResetBits( RLY_PORT, p)

//PB--------------------------------------------------------------------------------------------------------------------

#define BEEP_ON      0
#define BEEP_OFF     1
#define BEEP_PORT    GPIOB

#define BEEP   GPIO_Pin_0 //

#define BEEP_CTRL( s )	if( s )\
					GPIO_SetBits( BEEP_PORT, BEEP );\
					else		\
					GPIO_ResetBits( BEEP_PORT, BEEP)

//#define BEEP(a)	BEEP_CTRL( s )

//XG:NET LINK RESET
#define LCD_PORT  	GPIOB
#define LCD_ON			1
#define LCD_OFF			0

#define LCD_EN			GPIO_Pin_1 	//
#define LCD_CTRL( p, s )	if( s )\
					GPIO_SetBits( LCD_PORT, p );\
					else		\
					GPIO_ResetBits( LCD_PORT, p)

//XG:NET LINK RESET
#define XG_PORT    	GPIOB

#define XG_NET    	GPIO_Pin_5 	//
#define XG_RESET    GPIO_Pin_6 	//
#define XG_LINK    	GPIO_Pin_7	//

#define XG_CHK( s )	GPIO_ReadInputDataBit( XG_PORT, s )
#define XG_CTRL( p, s )	if( s )\
					GPIO_SetBits( XG_PORT, p );\
					else		\
					GPIO_ResetBits( XG_PORT, p)

//DET:
#define DET_PORT    	GPIOB

#define DET_PEDAL    	GPIO_Pin_13 //
#define DET_ZERO    	GPIO_Pin_14 //
#define DET_SYRINGE  	GPIO_Pin_15	//

#define DET_CHK( s )	GPIO_ReadInputDataBit( DET_PORT, s )

//PC--------------------------------------------------------------------------------------------------------------------
//KEY1~KEY8
#define KEY_PORT    	GPIOC

#define KEY1    			GPIO_Pin_0 //
#define KEY2    			GPIO_Pin_1 //
#define KEY3    			GPIO_Pin_2 //
#define KEY4    			GPIO_Pin_3 //
#define KEY5    			GPIO_Pin_4 //
#define KEY6    			GPIO_Pin_5 //
#define KEY7    			GPIO_Pin_6 //
#define KEY8    			GPIO_Pin_7 //

#define KEY_CHK( s )	GPIO_ReadInputDataBit( KEY_PORT, s )


////PD--------------------------------------------------------------------------------------------------------------------
////RS485
//#define EAT_PORT    GPIOD
//
//#define EAT1    GPIO_Pin_8 	//PD8
//#define EAT2    GPIO_Pin_9 	//PD9
//#define EAT3    GPIO_Pin_10	//PD10
//#define EAT4    GPIO_Pin_11	//PD11
//#define EAT5    GPIO_Pin_12	//PD12
//#define EAT6    GPIO_Pin_13	//PD13
//#define EAT7    GPIO_Pin_14	//PD14
//#define EAT8    GPIO_Pin_15	//PD15
//
////if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == KEY_ON ) 
//    
//#define EAT_CHK( s )	GPIO_ReadInputDataBit( EAT_PORT, s )
//
////PE0~PE8---------------------------------------------------------------------------------------------------------------
////TEST
//#define TEST_PORT    GPIOE
//
//#define POW1A   GPIO_Pin_0 //PE0
//#define POW1B   GPIO_Pin_1 //PE1
//#define POW1C   GPIO_Pin_2 //PE2
//#define POW2A   GPIO_Pin_3 //PE3
//#define POW2B   GPIO_Pin_4 //PE4
//#define POW2C   GPIO_Pin_5 //PE5
//#define MOVE    GPIO_Pin_6 //PE6 //原理图上这两个位置标反了
//#define DOOR    GPIO_Pin_7 //PE7 //原理图上这两个位置标反了
//
//#define TEST_CHK( s )	GPIO_ReadInputDataBit( TEST_PORT, s )


//PE8~PE9---------------------------------------------------------------------------------------------------------------
//LED BEEP



//#define ON  1
//#define OFF 0
//
//#define LED1(a)	if (a)	\
//					GPIO_SetBits(GPIOE,GPIO_Pin_8);\
//					else		\
//					GPIO_ResetBits(GPIOE,GPIO_Pin_8)
//
//#define LED2(a)	if (a)	\
//					GPIO_SetBits(GPIOE,GPIO_Pin_9);\
//					else		\
//					GPIO_ResetBits(GPIOE,GPIO_Pin_9)
//
//#define LED3(a)	if (a)	\
//					GPIO_SetBits(GPIOE,GPIO_Pin_10);\
//					else		\
//					GPIO_ResetBits(GPIOE,GPIO_Pin_10)
//
//#define LED4(a)	if (a)	\
//					GPIO_SetBits(GPIOE,GPIO_Pin_11);\
//					else		\
//					GPIO_ResetBits(GPIOE,GPIO_Pin_11)
//
//#define LED5(a)	if (a)	\
//					GPIO_SetBits(GPIOE,GPIO_Pin_12);\
//					else		\
//					GPIO_ResetBits(GPIOE,GPIO_Pin_12)
//
//#define LED6(a)	if (a)	\
//					GPIO_SetBits(GPIOE,GPIO_Pin_13);\
//					else		\
//					GPIO_ResetBits(GPIOE,GPIO_Pin_13)
//
//#define LED7(a)	if (a)	\
//					GPIO_SetBits(GPIOE,GPIO_Pin_14);\
//					else		\
//					GPIO_ResetBits(GPIOE,GPIO_Pin_14)
//
//#define BEEP(a)	if (a)	\
//					GPIO_SetBits(GPIOE,GPIO_Pin_15);\
//					else		\
//					GPIO_ResetBits(GPIOE,GPIO_Pin_15)


					
BSP_IO_EXT void bsp_io_Config(void);
BSP_IO_EXT void BuzzerRing( void ) ;
BSP_IO_EXT void BuzzerRingN( uint8_t n ) ;

#endif
