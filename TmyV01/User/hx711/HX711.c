#define HX711_GLOBAL
#include "bsp.h"

/*Include---------------------------*/
#include "bsp.h"//#include"STM32f10x_lib.h"                //�������е�ͷ�ļ�
#include <stdio.h> 

#define delay_us( us ) bsp_DelayUS( us )

//----------------��������--------------------
//void Delay_MS(u16 dly);
//void delay_us(u16 dly1);
//void RCC_Configuration(void);
//void GPIO_Configuration(void);
//unsigned long Read_HX711(void);
//void USART_Configuration(void);
//int fputc(int ch,FILE *f);
//int GetKey (void);
//USART_InitTypeDef USART_InitStructure;
//ErrorStatus HSEStartUpStatus;

//int main_sample(void)
//{
//    u32 weigh1;
//    float weigh2;
//    #IFdef DEBUG
//    debug();
//    #endif

//    //------------��ʼ��------------
//    RCC_Configuration();
//    GPIO_Configuration();
//    USART_Configuration( );
//    while(1)
//    {
//      Delay_MS(1000);
//      weigh2=Read_HX711();
//      weigh2=weigh2/83886.08; 13495563 7254    5V:7231 13512133 16735228 8375450
//      weigh1=weigh2*2000;
//      GPIO_SetBits(GPIOA,GPIO_Pin_11);
//      printf("%d\n",weigh1);
//    }
//                
//}

///*******************************************************************************
//* Function Name  : Delay_Ms
//* Description    : delay 1 ms.
//* Input          : dly(ms)
//* Output         : None
//* Return         : None
//*******************************************************************************/
//void Delay_MS(u16 dly)
//{
//        u16 i,j;
//        for(i=0;i<dly;i++)
//        for(j=1000;j>0;j--);
//}
//void delay_us(u16 dly1)
//{
//        u16 i;
//        for(i=dly1;i>0;i--);
//}

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

//unsigned long HX711_Read(void)  //��HX711оƬ��������ݡ� 
//{ 
//    unsigned long val = 0; 
//    unsigned char i = 0; 
//    
//    val = 0;
//    
//    HX711_DATA_1;   //DOUT=1 
//    HX711_SCK_0;    //SCK=0
//    
//    while( HX711_DATA_CHK ); //�ȴ�DOUT=0  
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
//    HX711_SCK_1; 
//    delay_us(1); 
//    val=val^0x800000;//��25�������½�����ʱ��ת������
//    HX711_SCK_0; //
//    //HX711_DATA_1; 
//    delay_us(1);  
//    
//    return val;
//}   

//unsigned long HX711_Read_B32(void)  //��HX711оƬ��������ݡ� 
unsigned long HX711_Read(void)
{ 
    unsigned long val = 0; 
    unsigned char i = 0; 
    
    val = 0;
    
    HX711_DATA_1;   //DOUT=1 
    HX711_SCK_0;    //SCK=0
    
    while( HX711_DATA_CHK ); //�ȴ�DOUT=0  
    delay_us(1); 
    for(i=0;i<24;i++) 
    { 
        HX711_SCK_1;    //SCK=1 
        val=val<<1; 
        //delay_us(1);  
        HX711_SCK_0;    //SCK=0 
        if( HX711_DATA_CHK ) //DOUT=1 
        val++; 
       //delay_us(1); 
    } 
    HX711_SCK_1; 
    delay_us(1); 
    HX711_SCK_0; //
    delay_us(1); 
    HX711_SCK_1; 
    delay_us(1); 
    val=val^0x800000;//��25�������½�����ʱ��ת������
    HX711_SCK_0; //
    //HX711_DATA_1; 
    delay_us(1);  
    
    return val;
}  

//unsigned long Read_HX711(void)  //��HX711оƬ��������ݡ� 
//{ 
//    unsigned long val = 0; 
//    unsigned char i = 0; 
//
//    GPIO_SetBits(GPIOB,GPIO_Pin_11);    //DOUT=1 
//    GPIO_ResetBits(GPIOB,GPIO_Pin_12);    //SCK=0
//    
//    while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11));   //�ȴ�DOUT=0  
//    delay_us(1); 
//    for(i=0;i<24;i++) 
//    { 
//        GPIO_SetBits(GPIOB,GPIO_Pin_12);    //SCK=1 
//        val=val<<1; 
//        delay_us(1);  
//        GPIO_ResetBits(GPIOB,GPIO_Pin_12);    //SCK=0 
//        if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11))   //DOUT=1 
//        val++; 
//        delay_us(1); 
//    } 
//    GPIO_SetBits(GPIOB,GPIO_Pin_12); 
//    delay_us(1); 
//    GPIO_ResetBits(GPIOB,GPIO_Pin_12); 
//    delay_us(1);  
//    return val;  
//}   

///*******************************************************************************
//* Function Name  : RCC_Configuration
//* Description    : Configures the different system clocks.
//* Input          : None
//* Output         : None
//* Return         : None
//*******************************************************************************/
//void RCC_Configuration(void)
//{
//        //----------ʹ���ⲿRC����-----------
//        RCC_DeInit();                        //��ʼ��Ϊȱʡֵ
//        RCC_HSEConfig(RCC_HSE_ON);        //ʹ���ⲿ�ĸ���ʱ�� 
//        while(RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET);        //�ȴ��ⲿ����ʱ��ʹ�ܾ���
//        FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);   
//    FLASH_SetLatency(FLASH_Latency_2); 
//        RCC_HCLKConfig(RCC_SYSCLK_Div1);                //HCLK = SYSCLK
//        RCC_PCLK2Config(RCC_HCLK_Div1);                        //PCLK2 =  HCLK
//        RCC_PCLK1Config(RCC_HCLK_Div2);                        //PCLK1 = HCLK/2
//        RCC_PLLConfig(RCC_PLLSource_HSE_Div1,RCC_PLLMul_9);        //PLLCLK = 8MHZ * 9 =72MHZ
//        RCC_PLLCmd(ENABLE);                        //Enable PLLCLK
//
//        while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);        //Wait till PLLCLK is ready
//    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);        //Select PLL as system clock
//        while(RCC_GetSYSCLKSource()!=0x08);                //Wait till PLL is used as system clock source        
//        //---------����Ӧ����ʱ��--------------------
//        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB,ENABLE);        //ʹ��APB2�����GPIOA��ʱ��        
//                  //��������ʱ��
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
//
//                 
//}

