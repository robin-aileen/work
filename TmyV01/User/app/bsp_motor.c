// =====================================================================================================================
// | File Name		: MOTOR.h
// | Summary 		: 
// | VersMOTORn 	: 1.00 
// | CPU	 		: STM32F103
// | Compile 		：keil
// | Author  		：Robin
// | Start Date 	：2019-03-16
// | Finish Date 	：
// =====================================================================================================================

//#include "includes.h"

#define MOTOR_GLOBAL
#include "bsp.h"
#include "stm32f10x.h"
#include "bsp_motor.h"
#include "hmi_driver.h"

//uint16_t BEEP_TIMER[5]     = { 200, 100, 50, 33, 17 };
//uint8_t MOTOR_SPEED_DISPLAY[5][7] = { "慢慢速\0",    "慢速  \0", "中速  \0",   "快速  \0", "最快速\0" };
//uint16_t MOTOR_SPEED_CYCLE[5]     = { CYCLE_SLOWEST, CYCLE_SLOW, CYCLE_NORMAL, CYCLE_FAST, CYCLE_FASTEST };
  
uint16_t BEEP_TIMER[7]     = { 200, 100, 50, 33, 17, 20, 20 };

uint8_t MOTOR_SPEED_DISPLAY[7][9] = { "慢慢速  \0",    "慢速    \0", "中速    \0",   "快速    \0", "自动排空\0", "回吸    \0", "顶杆检查\0" };
uint16_t MOTOR_SPEED_CYCLE[7]     = { CYCLE_SLOWEST, CYCLE_SLOW, CYCLE_NORMAL, CYCLE_FAST, CYCLE_ZDPK, CYCLE_HX, CYCLE_DGJC };
                                
uint16_t MOTOR_SPEED_INDEX[4][3]={{ SLOWEST, SLOWEST, SLOWEST },
                                  { SLOW,    SLOW,    SLOW    },
                                  { SLOW,    NORMAL,  FAST    },
                                  { SLOWEST, FAST,    FASTEST }};

uint8_t DIS_PLAY[][3][7] = { {"慢慢速\0", "慢慢速\0", "慢慢速\0"},
                             {"慢速  \0", "慢速  \0", "慢速  \0"}, 
                             {"慢速  \0", "中速  \0", "快速  \0"},
                             {"慢慢速\0", "快速  \0", "最快速\0"} };
                                                                
/***定时器1主模式***/
void TIM1_config(uint32_t Cycle)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_TIM1 , ENABLE); 

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;                   //TIM1_CH4 PA11 //GPIO_Pin_9; //TIM1_CH2 PA9
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;             //复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    TIM_TimeBaseStructure.TIM_Period = Cycle-1;                                                   
    TIM_TimeBaseStructure.TIM_Prescaler =71;                    //设置用来作为TIMx时钟频率除数的预分频值                                                     
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //设置时钟分割：TDTS= Tck_tim            
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM向上计数模式
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;            //重复计数，一定要=0！！！
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);                                       

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;          //选择定时器模式：TIM脉冲宽度调制模式1       
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
    TIM_OCInitStructure.TIM_Pulse = Cycle/2-1;                    //设置待装入捕获寄存器的脉冲值                                   
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;      //输出极性       

    TIM_OC4Init(TIM1, &TIM_OCInitStructure);  
    
//    TIM_SelectMasterSlaveMode(TIM1, TIM_MasterSlaveMode_Enable);
//    TIM_SelectOutputTrigger(TIM1, TIM_TRGOSource_Update);
//    
//
////    TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);
////    TIM_ARRPreloadConfig(TIM1, ENABLE);    
    
    TIM_ARRPreloadConfig(TIM1,ENABLE);
    TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_CtrlPWMOutputs(TIM1, ENABLE);   //高级定时器一定要加上，主输出使能
    
    TIM_Cmd(TIM1, DISABLE);
    //TIM_ARRPreloadConfig(TIM1,DISABLE);
    //TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Disable);
    
     //使能指定的TIM中断
    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;       
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;     
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
    NVIC_Init(&NVIC_InitStructure);
                                                                     
}
/***定时器2从模式***/
void TIM2_config(uint32_t PulseNum)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure; 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseStructure.TIM_Period = 4800-1;//100000-1; //100ms
    TIM_TimeBaseStructure.TIM_Prescaler = 10000-1;//71;    
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);  
       
    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
    //使能TIM定时器
    TIM_Cmd(TIM2, ENABLE);
    
   // NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;        
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;     
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
    NVIC_Init(&NVIC_InitStructure);
}
void Pulse_outputT1T2(uint32_t Cycle,uint32_t PulseNum)
{
//    TIM2_config(PulseNum);
//    TIM_Cmd(TIM2, ENABLE);
//    TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
//    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
    TIM1_config(Cycle);
    
    //TIM_Cmd(TIM1, ENABLE);
    //TIM_CtrlPWMOutputs(TIM1, ENABLE);   //高级定时器一定要加上，主输出使能
}


void pulse_start( void )
{
//	TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);
//    TIM_ARRPreloadConfig(TIM1,ENABLE);
	TIM_Cmd(TIM1,ENABLE);;
}

void pulse_stop( void )
{
//	TIM_Cmd(TIM1, DISABLE);
//    TIM_ARRPreloadConfig(TIM1,DISABLE);
//    TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Disable);//TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Disable);
}

void pulse_set_f(uint32_t Cycle)
{
    TIM_SetAutoreload(TIM1,Cycle-1);
	TIM_SetCompare4(TIM1,Cycle/2-1);
    TIM_SetCounter(TIM1,Cycle/2-1);
}

void MOTOR_SetSpeed(uint32_t speed)
{
    uint32_t Cycle;
    Cycle = speed;//MOTOR_SPEED_CYCLE[ MOTOR_SPEED_INDEX[Motor.gear][speed-1] ];//Cycle = MOTOR_SPEED[Motor.gear][speed-1];
    TIM_SetAutoreload(TIM1,Cycle-1);
	TIM_SetCompare4(TIM1,Cycle/2-1);
    TIM_SetCounter(TIM1,Cycle/2-1);
    
    MOTOR_Start( );
    
    Motor.state.bit.t_pulse = 1;
    SetTextInt32(0, 17, Cycle, 0, 1);
    
}
//void MOTOR_SetSpeed(uint32_t speed)
//{
//    uint32_t Cycle;
//    //Cycle = MOTOR_SPEED[Motor.gear][speed-1];
//    Cycle = speed;
//    TIM_SetAutoreload(TIM1,Cycle-1);
//	TIM_SetCompare4(TIM1,Cycle/2-1);
//    TIM_SetCounter(TIM1,Cycle/2-1);
//    
//    MOTOR_Start( );
//}

void motor_dir_init( void )
{
    GPIO_InitTypeDef GPIO_InitStructure; //定义一个GPIO_InitTypeDef类型的结构体

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE); //开启GPIOA的外设时钟
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //选择要控制的GPIOA引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //设置引脚模式为通用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //设置引脚速率为50MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure); //调用库函数，初始化GPIOB  
    
    MOTOR_DIR( UP );//关闭所有
}

void motor_pul_init(uint32_t Cycle)
{
    TIM1_config(Cycle);
    //TIM2_config( 100000 );
}

void MOTOR_Init( uint32_t Cycle )
{
    //MOTOR_SPEED_18[]={1000,100,30,10};
    Motor.max_step = MAX_STEP_18ML;
    motor_dir_init( );
    motor_pul_init(Cycle);
    
    Motor.timer_t_pulse = 0;
}

void MOTOR_SetDir( uint8_t dir )
{
    Motor.state.bit.dir = dir;
    MOTOR_DIR( dir );
    vTaskDelay(100);
    if( dir ) SetTextValue(0,18,"下降");
    else SetTextValue(0,18,"上升");
}

//void MOTOR_UP( uint32_t f )
//{
//    
//}

//void MOTOR_DOWN( uint32_t f )
//{
//    
//}

void MOTOR_Start( void )
{
    Motor.state.bit.move = 1;
	TIM_Cmd(TIM1,ENABLE);;
}

void MOTOR_Stop( void )
{
    Motor.state.bit.move = 0;
    TIM_Cmd(TIM1, DISABLE);
}

void MOTOR_ClrStep( void )
{
    Motor.counter_step = 0;
}

void TIM1_UP_IRQHandler(void) 
{ 
    if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)     // TIM_IT_CC1
    { 
        TIM_ClearITPendingBit(TIM1, TIM_IT_Update); // 清除中断标志位
        if( Motor.state.bit.dir == UP )
        {
            if( Motor.counter_step < Motor.max_step ) Motor.counter_step++;
        }else
        {
            if( Motor.counter_step ) Motor.counter_step--;
        }
    }
}

//void TIM2_IRQHandler(void) 
//{ 
//    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)     // TIM_IT_CC1
//    { 
//        TIM_ClearITPendingBit(TIM2, TIM_IT_Update); // 清除中断标志位
//        
//        if( Motor.state.bit.t_pulse )
//        {
//            Motor.timer_t_pulse++;
//        
//            if( Motor.timer_t_pulse == 1 )
//            {
//                pulse_set_f(2000 );
//                MOTOR_Start( );
//            }else
//            if( Motor.timer_t_pulse == 2 )
//            {
//                pulse_set_f(1562 );
//            }else
//            if( Motor.timer_t_pulse == 3 )
//            {
//                pulse_set_f(1124 );
//            }else
//            if( Motor.timer_t_pulse == 4 )
//            {
//                pulse_set_f(686 );
//            }else
//            if( Motor.timer_t_pulse == 5 )
//            {
//                pulse_set_f(248 );
//                Motor.state.bit.t_pulse = 0;
//                Motor.timer_t_pulse = 0;
//            }
//        }
//        
//        
//        
//    } 
//} 
