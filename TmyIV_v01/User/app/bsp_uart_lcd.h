// =====================================================================================================================
// | File Name		: bsp_uart_lcd.h
// | Summary 	    : 
// | Version 	    : 1.00 
// | CPU	 	    : STM32103F
// | Compile 	    ��keil
// | Author  	    ��Robin
// | Start Date 	��2019-03-10
// | Finish Date 	��
// =====================================================================================================================

#ifndef ULCD_H
#define ULCD_H 

#ifndef ULCD_GLOBAL 
#define ULCD_EXT	extern 
#else 
#define ULCD_EXT 
#endif 

#define ULCD_COM COM2

#define ID_HX   3   //����
#define ID_DIR  4   //����
#define ID_CMS  5   //������
#define ID_MMS  6   //������
#define ID_MZK  7   //���п���
#define ID_MKZ  8   //���������
#define ID_FIX  9   //���˼��
#define ID_1ML8 10  //ע��������1.8ml
#define ID_2ML2 11  //ע��������2.2ml

#define RCC_ULCD 	(RCC_APB2Periph_GPIOA)

#define GPIO_PORT_ULCD  GPIOA
#define GPIO_PIN_ULCD_POWEN GPIO_Pin_0
#define GPIO_PIN_ULCD_BUSY  GPIO_Pin_1

#define ULCD_POW_ON     GPIO_SetBits( GPIO_PORT_ULCD, GPIO_PIN_ULCD_POWEN )
#define ULCD_POW_OFF    GPIO_SetBits( GPIO_PORT_ULCD, GPIO_PIN_ULCD_POWEN )

#define ULCD_P_HEAD 0xEE
#define ULCD_P_TAIL "\xFF\xFC\xFF\xFF"

ULCD_EXT uint8_t lg_lcd_init;
ULCD_EXT uint8_t dg_sta_befor;

ULCD_EXT uint32_t timer_lcd_update;
ULCD_EXT uint32_t timer_beep;
ULCD_EXT uint16_t lcd_prosser; 
ULCD_EXT void ULCD_Init(void);
ULCD_EXT void ULCD_Task( void );
ULCD_EXT void LCD_ParaInit( void );
ULCD_EXT uint16_t LCD_GetScreenId( void );

#endif