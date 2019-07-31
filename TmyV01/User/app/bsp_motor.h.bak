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

#ifndef MOTOR_H
#define MOTOR_H 

#ifndef MOTOR_GLOBAL 
#define MOTOR_EXT	extern 
#else 
#define MOTOR_EXT 
#endif 

#define MOTOR_SIMULATION    1 //模拟运行标志，模拟运行时候，把PWM管脚映射到没有用的管脚，这样既可以不让电机运动又不影响计数

#define MAX_STEP_18ML 244094 //最大步数 244094.08
#define MAX_STEP_22ML 307086 //最大步数 307086.08

//MOTOR_EXT uint16_t BEEP_TIMER[5];
//MOTOR_EXT uint8_t MOTOR_SPEED_DISPLAY[5][7];
//MOTOR_EXT uint16_t MOTOR_SPEED_CYCLE[5];

MOTOR_EXT uint16_t BEEP_TIMER[7];
MOTOR_EXT uint8_t MOTOR_SPEED_DISPLAY[7][9];
MOTOR_EXT uint16_t MOTOR_SPEED_CYCLE[7];
                                
MOTOR_EXT uint16_t MOTOR_SPEED_INDEX[4][3];

//1.8ml行程46.5mm 2.2ml行程
//速度        转速(转/分)     蜂鸣频率(次/秒)
//慢慢速      3.325           0.5
//慢速        6.65            1
//中速        15.5839         2
//快速        37.7296         3
//最快速      150.9           6

//回吸        70              5
//自动排空    70              5
//顶杆检查    246.0629

//自动给药 慢慢速、慢速 5秒后提示“自动给药”，再2秒后解除
//自动排空 5.7414转后停止
//二次排空 2.05转后停止
                  
#define CYCLE_SLOWEST   2819//2819
#define CYCLE_SLOW      1409
#define CYCLE_NORMAL    601
#define CYCLE_FAST      248
#define CYCLE_FASTEST   62

#define CYCLE_ZDPK      134 //准备 自动排空
#define CYCLE_HX        134 //回吸
#define CYCLE_DGJC      38 //顶杆检查 快升降 CYCLE_KSJ

//#define CYCLE_ZB 134 //准备 自动排空
//#define CYCLE_HX 134 //回吸
//#define CYCLE_KSJ 38 //快升降

//#define CYCLE_SLOWEST 2000
//#define CYCLE_SLOW    1000
//#define CYCLE_NORMAL  500
//#define CYCLE_FAST    100
//#define CYCLE_FASTEST 30
//-----------------------------------------------------------------------------------------------------
#define UP    0
#define DOWN  1

//PB11
#define MOTOR_DIR(a)	if (a)	\
					GPIO_SetBits(GPIOA,GPIO_Pin_12);\
					else		\
					GPIO_ResetBits(GPIOA,GPIO_Pin_12)
					
//-----------------------------------------------------------------------------------------------------

typedef enum
{
	SPEED_1=0,
	SPEED_2,
	SPEED_3,
	SPEED_4,
	SPEED_5
}ENUM_SPEED_LEVEL;

//MOTOR_EXT uint16_t MOTOR_SPEED_18[5]; //1.8ml药瓶速度
//MOTOR_EXT uint16_t MOTOR_SPEED_22[5]; //2.2ml药瓶速度

#define MOTOR_MAX_STEP 65536 //最大步数

//电机速度档位
//typedef enum
//{
//    //STOP=0,      //停止
//	SLOWEST=0,      //慢慢速
//	SLOW,           //常慢速
//	NORMAL,         //中速
//	FAST,           //快速
//	FASTEST         //最快速
//}ENUM_SPEED;
typedef enum
{
    //STOP=0,       //停止
	SLOWEST=0,      //慢慢速
	SLOW,           //常慢速
	NORMAL,         //中速
	FAST,           //快速
	FASTEST,        //最快速
	ZDPK,           //自动排空
	HX,             //回吸
	DGJC            //顶杆检查
}ENUM_SPEED;

//电机速度组合档位
typedef enum
{
    //GEAR_STANDBY = 0,       //待机
	GEAR_SLOWEST=0,           //慢慢速
	GEAR_SLOW,              //常慢速
	GEAR_SLOW_NORMAL_FAST,  //慢中快速
	GEAR_SLOW_FAST_FASTEST, //慢快最快速
}ENUM_GEAR; 

typedef struct
{
    uint8_t dir         :1;
    uint8_t alarm       :1;
    uint8_t powerlow    :1;
    uint8_t move        :1;
    uint8_t stop        :1;
    uint8_t zero        :1;
    uint8_t t_pulse     :1;
        
    uint8_t             :1;
}TYPE_MOTOR_STATE_BIT;

typedef union
{
    TYPE_MOTOR_STATE_BIT bit;
    uint8_t all;
}TYPE_MOTOR_STATE;

typedef struct //info
{
    uint8_t speed;//电机运行速度
    uint8_t gear;//速度档位
    TYPE_MOTOR_STATE state;//电机运行状态
    uint32_t counter_step; //电机步数
    uint32_t max_step; //电机最大步数
    uint32_t counter_hx_step; //回吸步数
    uint32_t max_hx_step; //回吸最大步数
    uint32_t timer;//电机运行时间
    uint32_t timer_t_pulse; // t型滤波记时
    
}TYPE_STRUCT_MOTOR;
MOTOR_EXT TYPE_STRUCT_MOTOR Motor;

//typedef union
//{
//    TYPE_STRUCT_MOTOR Para;
//    uint8_t Buf[sizeof(TYPE_STRUCT_MOTOR)];
//}TYPE_MOTOR;
//
//MOTOR_EXT TYPE_MOTOR Motor;
//void TIM4_config(uint32_t Cycle);
//void TIM3_config(uint32_t PulseNum);
//void Pulse_output(uint32_t Cycle,uint32_t PulseNum);
//void Pulse_outputT1T2(uint32_t Cycle,uint32_t PulseNum);

MOTOR_EXT void MOTOR_Init( uint32_t Cycle );
//void MOTOR_SetSpeed(uint32_t speed);//
MOTOR_EXT void MOTOR_SetSpeed(uint32_t speed);
MOTOR_EXT void MOTOR_Start( void );
MOTOR_EXT void MOTOR_Stop( void );
MOTOR_EXT void MOTOR_SetDir( uint8_t dir );
MOTOR_EXT void MOTOR_ClrStep( void );

//void pulse_set_f(uint32_t Cycle);
//void pulse_start( void );
//void pulse_stop( void );

#endif
