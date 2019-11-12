// =====================================================================================================================
// | File Name		: main.h
// | Summary 		: 
// | VersMOTORn 	: 1.00 
// | CPU	 		: STM32F103
// | Compile 		��keil
// | Author  		��Robin
// | Start Date 	��2019-03-16
// | Finish Date 	��
// =====================================================================================================================

#ifndef MAIN_H
#define MAIN_H 

#ifndef MAIN_GLOBAL 
#define MAIN_EXT	extern 
#else 
#define MAIN_EXT 
#endif 

#define ONE_CYCLE_STEP	6400 //һת�Ĳ���

#define STEP_HX_ENABLE	0 //������Ч����
#define STEP_MAX_ZDPK   36745 //( ONE_CYCLE_STEP*3 ) //�Զ��ſղ��� 5.7414ת=36,744.96��~=36745��
#define STEP_MAX_ECPK   13120 //�����ſղ��� 2.05ת 2.5mm =13120?��
#define MAX_PRESSURE    1000 //ѹ����������ѹֵ
//#define TIME_CRUISE_ENABLE	5 //���½�̤��Ѳ������ʱ��
//#define TIME_CRUISE_CANCLE	7 //���½�̤��Ѳ��ȡ��ʱ��
#define TIME_ZDGY_ENABLE	5 //���½�̤���Զ���ҩ����ʱ��
#define TIME_ZDGY_CANCLE	8 //���½�̤���Զ���ҩȡ��ʱ��
#define DLY_PEDAL_UP_MOTOR_STOP	2 //̧���̤�󣬵��ֹͣʱ�� timer_pedal_up

typedef enum 
{
    MOTOR_STOP = 0,
    MOTOR_UP,
    MOTOR_DOWN
}TYPE_MOTOR_STA;

typedef struct
{
    uint8_t hx          :1; //����
    uint8_t zdpk        :1; //�Զ��ſ�
    uint8_t ecpk        :1; //�����ſ�
    uint8_t zh          :1; //1.8ml 2.2mlת��
    uint8_t fix         :1; //����
    uint8_t px          :1; //��ѵ
    uint8_t jjzd        :1; //�����ƶ� emergency stop
	uint8_t             :1; //
}TYPE_TMY_KEY_BIT;


// typedef struct
// {
    // uint8_t dir         :1;
    // uint8_t alarm       :1;
    // uint8_t zdpk        :1;//���Զ��ſ� //uint8_t powerlow    :1;
    // uint8_t move        :1;
    // uint8_t stop        :1;
    // uint8_t hx          :1;
	// uint8_t cruise  	:1; //����Ѳ��
    // uint8_t             :1;
// }TYPE_TMY_STATE_BIT;

// typedef union
// {
    // TYPE_TMY_STATE_BIT bit;
    // uint8_t all;
// }TYPE_TMY_STATE;

// //����״̬
// typedef enum
// {
	// STA_SLEEP=0,    //����
	// STA_STANDBY,    //����
	// STA_MOVE,       //�������
	// STA_HX_DOWN,    //��������
	// STA_HX_UP,      //��������
	
	// STA_FIX,        //����
	// STA_RESET,      //��λ
	// STA_IDLE,       //����
	// STA_WORK        //����
// }ENUM_STA;

//����״̬
MAIN_EXT uint8_t dg_sta;
typedef enum 
{
    DG_STOP = 0,   //ֹͣ
    DG_INIT,        //��ʼ��
    DG_INIT_UP,    //��ʼ������
    DG_INIT_DOWN,  //��ʼ���½�
    
    DG_STOP_TOP,    //ͣ�ڶ���
    DG_STOP_BOT,    //ͣ�ڵ׶�
    DG_UP,          //����
    DG_DOWN,        //�½�
    
    DG_ZDPK_UP,     //�Զ��ſ�
    DG_ECPK_UP,     //�����ſ�
    
    DG_HX_UP,       //��������
    DG_HX_DOWN,     //�����½�
    DG_RESET_UP,    //��λ����
    DG_RESET_DOWN,  //��λ�½�
    DG_FIX_UP,      //��������
    DG_FIX_DOWN,    //�����½�
    
    DG_IDLE //
    
}TYPE_DING_GAN;

//�豸����״̬
MAIN_EXT uint8_t dev_sta;
typedef enum 
{
    DEV_INIT = 0,   //������ʼ��
    DEV_FIX,        //����
    DEV_ERR,        //����
    DEV_RUN,        //����
    
    DEV_STANDBY      //����
    
}TYPE_STA;

typedef struct
{
    uint8_t InitPara    :1; //������ʼ��
    uint8_t init        :1;
    uint8_t poweron     :1;
    uint8_t powerlow    :1;
    uint8_t move        :1;
    uint8_t stop        :1;
	//uint8_t cruise  	:1; //����Ѳ��
    uint8_t zdgy  	    :1; //�Զ���ҩ
    uint8_t             :1;
}TYPE_STATE_BIT;

typedef union
{
    TYPE_STATE_BIT bit;
    uint8_t all;
}TYPE_STATE;

//������״̬
#define SYRINGE_1ML8    0
#define SYRINGE_2ML2    1
#define SYRINGE_OFF     0
#define SYRINGE_ON      1

typedef struct
{
    uint8_t bottom      :1;//�ײ�
    uint8_t top         :1;//����
    uint8_t SyringeType :1;//ע�����ͺ� medicine bottle ҩƿ
    uint8_t SyringeState:1;//ע��������״̬ Syringe ע����
    uint8_t OverPressure:1;//ѹ����������ѹ
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
    uint8_t gear;//�ٶȵ�λ
    uint8_t speed;//��������ٶ�
    uint8_t pressureLevel;//ѹ���ȼ�
    //TYPE_TMY_STATE state;//�������״̬
    TYPE_TMY_KEY_BIT Key;
    
    TYPE_STATE State;
    TYPE_SENSOR Sensor;
    
    uint8_t power_percent;
    uint16_t power_v;
    uint32_t pressure; //ѹ��
//    uint32_t counter_step; //�������
//    uint32_t max_step; //��������
//    uint32_t counter_hx_step; //��������
//    uint32_t max_hx_step; //���������
    
    uint32_t timer;//�������ʱ��
    
}TYPE_STRUCT_TMY;

MAIN_EXT TYPE_STRUCT_TMY Tmy;

#endif
