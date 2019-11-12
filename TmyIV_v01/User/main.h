// =====================================================================================================================
// | File Name		: main.h
// | Summary 		: 
// | VersMOTORn 	: 1.00 
// | CPU	 		: STM32F103
// | Compile 		：keil
// | Author  		：Robin
// | Start Date 	：2019-03-16
// | Finish Date 	：
// =====================================================================================================================

#ifndef MAIN_H
#define MAIN_H 

#ifndef MAIN_GLOBAL 
#define MAIN_EXT	extern 
#else 
#define MAIN_EXT 
#endif 

#define ONE_CYCLE_STEP	6400 //一转的步数

#define STEP_HX_ENABLE	0 //回吸有效距离
#define STEP_MAX_ZDPK   36745 //( ONE_CYCLE_STEP*3 ) //自动排空步数 5.7414转=36,744.96步~=36745步
#define STEP_MAX_ECPK   13120 //二次排空步数 2.05转 2.5mm =13120?步
#define MAX_PRESSURE    1000 //压力传感器过压值
//#define TIME_CRUISE_ENABLE	5 //按下脚踏后，巡航启动时间
//#define TIME_CRUISE_CANCLE	7 //按下脚踏后，巡航取消时间
#define TIME_ZDGY_ENABLE	5 //按下脚踏后，自动给药启动时间
#define TIME_ZDGY_CANCLE	8 //按下脚踏后，自动给药取消时间
#define DLY_PEDAL_UP_MOTOR_STOP	2 //抬起脚踏后，电机停止时间 timer_pedal_up

typedef enum 
{
    MOTOR_STOP = 0,
    MOTOR_UP,
    MOTOR_DOWN
}TYPE_MOTOR_STA;

typedef struct
{
    uint8_t hx          :1; //回吸
    uint8_t zdpk        :1; //自动排空
    uint8_t ecpk        :1; //二次排空
    uint8_t zh          :1; //1.8ml 2.2ml转换
    uint8_t fix         :1; //检修
    uint8_t px          :1; //培训
    uint8_t jjzd        :1; //紧急制动 emergency stop
	uint8_t             :1; //
}TYPE_TMY_KEY_BIT;


// typedef struct
// {
    // uint8_t dir         :1;
    // uint8_t alarm       :1;
    // uint8_t zdpk        :1;//已自动排空 //uint8_t powerlow    :1;
    // uint8_t move        :1;
    // uint8_t stop        :1;
    // uint8_t hx          :1;
	// uint8_t cruise  	:1; //定速巡航
    // uint8_t             :1;
// }TYPE_TMY_STATE_BIT;

// typedef union
// {
    // TYPE_TMY_STATE_BIT bit;
    // uint8_t all;
// }TYPE_TMY_STATE;

// //工作状态
// typedef enum
// {
	// STA_SLEEP=0,    //休眠
	// STA_STANDBY,    //待机
	// STA_MOVE,       //电机运行
	// STA_HX_DOWN,    //回吸下行
	// STA_HX_UP,      //回吸上行
	
	// STA_FIX,        //检修
	// STA_RESET,      //复位
	// STA_IDLE,       //空闲
	// STA_WORK        //工作
// }ENUM_STA;

//顶杆状态
MAIN_EXT uint8_t dg_sta;
typedef enum 
{
    DG_STOP = 0,   //停止
    DG_INIT,        //初始化
    DG_INIT_UP,    //初始化上升
    DG_INIT_DOWN,  //初始化下降
    
    DG_STOP_TOP,    //停在顶端
    DG_STOP_BOT,    //停在底端
    DG_UP,          //上升
    DG_DOWN,        //下降
    
    DG_ZDPK_UP,     //自动排空
    DG_ECPK_UP,     //二次排空
    
    DG_HX_UP,       //回吸上升
    DG_HX_DOWN,     //回吸下降
    DG_RESET_UP,    //复位上升
    DG_RESET_DOWN,  //复位下降
    DG_FIX_UP,      //检修上升
    DG_FIX_DOWN,    //检修下降
    
    DG_IDLE //
    
}TYPE_DING_GAN;

//设备工作状态
MAIN_EXT uint8_t dev_sta;
typedef enum 
{
    DEV_INIT = 0,   //开机初始化
    DEV_FIX,        //检修
    DEV_ERR,        //故障
    DEV_RUN,        //运行
    
    DEV_STANDBY      //待机
    
}TYPE_STA;

typedef struct
{
    uint8_t InitPara    :1; //参数初始化
    uint8_t init        :1;
    uint8_t poweron     :1;
    uint8_t powerlow    :1;
    uint8_t move        :1;
    uint8_t stop        :1;
	//uint8_t cruise  	:1; //定速巡航
    uint8_t zdgy  	    :1; //自动给药
    uint8_t             :1;
}TYPE_STATE_BIT;

typedef union
{
    TYPE_STATE_BIT bit;
    uint8_t all;
}TYPE_STATE;

//传感器状态
#define SYRINGE_1ML8    0
#define SYRINGE_2ML2    1
#define SYRINGE_OFF     0
#define SYRINGE_ON      1

typedef struct
{
    uint8_t bottom      :1;//底部
    uint8_t top         :1;//顶部
    uint8_t SyringeType :1;//注射器型号 medicine bottle 药瓶
    uint8_t SyringeState:1;//注射器插入状态 Syringe 注射器
    uint8_t OverPressure:1;//压力传感器过压
    uint8_t             :3;
}TYPE_SENSOR_BIT;

typedef union
{
    TYPE_SENSOR_BIT bit;
    uint8_t all;
}TYPE_SENSOR;


typedef struct //info
{
    uint8_t state;
    uint8_t dir;
}TYPE_MOTOR_STRUCT;

typedef struct //info
{
    //uint8_t motor_state;
    TYPE_MOTOR_STRUCT Motor;
    uint8_t work_state;
    uint8_t gear;//速度档位
    uint8_t speed;//电机运行速度
    uint8_t pressureLevel;//压力等级
    //TYPE_TMY_STATE state;//电机运行状态
    TYPE_TMY_KEY_BIT Key;
    
    TYPE_STATE State;
    TYPE_SENSOR Sensor;
    
    uint8_t power_percent;
    uint16_t power_v;
    uint32_t pressure; //压力
//    uint32_t counter_step; //电机步数
//    uint32_t max_step; //电机最大步数
//    uint32_t counter_hx_step; //回吸步数
//    uint32_t max_hx_step; //回吸最大步数
    
    uint32_t timer;//电机运行时间
    
}TYPE_STRUCT_TMY;

MAIN_EXT TYPE_STRUCT_TMY Tmy;

#endif
