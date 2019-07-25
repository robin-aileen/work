// =====================================================================================================================
// | File Name		: MOTOR.h
// | Summary 		: 
// | VersMOTORn 	: 1.00 
// | CPU	 		: STM32F103
// | Compile 		��keil
// | Author  		��Robin
// | Start Date 	��2019-03-16
// | Finish Date 	��
// =====================================================================================================================

#ifndef MOTOR_H
#define MOTOR_H 

#ifndef MOTOR_GLOBAL 
#define MOTOR_EXT	extern 
#else 
#define MOTOR_EXT 
#endif 

#define MOTOR_SIMULATION    1 //ģ�����б�־��ģ������ʱ�򣬰�PWM�ܽ�ӳ�䵽û���õĹܽţ������ȿ��Բ��õ���˶��ֲ�Ӱ�����

#define MAX_STEP_18ML 244094 //����� 244094.08
#define MAX_STEP_22ML 307086 //����� 307086.08

//MOTOR_EXT uint16_t BEEP_TIMER[5];
//MOTOR_EXT uint8_t MOTOR_SPEED_DISPLAY[5][7];
//MOTOR_EXT uint16_t MOTOR_SPEED_CYCLE[5];

MOTOR_EXT uint16_t BEEP_TIMER[7];
MOTOR_EXT uint8_t MOTOR_SPEED_DISPLAY[7][9];
MOTOR_EXT uint16_t MOTOR_SPEED_CYCLE[7];
                                
MOTOR_EXT uint16_t MOTOR_SPEED_INDEX[4][3];

//1.8ml�г�46.5mm 2.2ml�г�
//�ٶ�        ת��(ת/��)     ����Ƶ��(��/��)
//������      3.325           0.5
//����        6.65            1
//����        15.5839         2
//����        37.7296         3
//�����      150.9           6

//����        70              5
//�Զ��ſ�    70              5
//���˼��    246.0629

//�Զ���ҩ �����١����� 5�����ʾ���Զ���ҩ������2�����
//�Զ��ſ� 5.7414ת��ֹͣ
//�����ſ� 2.05ת��ֹͣ
                  
#define CYCLE_SLOWEST   2819//2819
#define CYCLE_SLOW      1409
#define CYCLE_NORMAL    601
#define CYCLE_FAST      248
#define CYCLE_FASTEST   62

#define CYCLE_ZDPK      134 //׼�� �Զ��ſ�
#define CYCLE_HX        134 //����
#define CYCLE_DGJC      38 //���˼�� ������ CYCLE_KSJ

//#define CYCLE_ZB 134 //׼�� �Զ��ſ�
//#define CYCLE_HX 134 //����
//#define CYCLE_KSJ 38 //������

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

//MOTOR_EXT uint16_t MOTOR_SPEED_18[5]; //1.8mlҩƿ�ٶ�
//MOTOR_EXT uint16_t MOTOR_SPEED_22[5]; //2.2mlҩƿ�ٶ�

#define MOTOR_MAX_STEP 65536 //�����

//����ٶȵ�λ
//typedef enum
//{
//    //STOP=0,      //ֹͣ
//	SLOWEST=0,      //������
//	SLOW,           //������
//	NORMAL,         //����
//	FAST,           //����
//	FASTEST         //�����
//}ENUM_SPEED;
typedef enum
{
    //STOP=0,       //ֹͣ
	SLOWEST=0,      //������
	SLOW,           //������
	NORMAL,         //����
	FAST,           //����
	FASTEST,        //�����
	ZDPK,           //�Զ��ſ�
	HX,             //����
	DGJC            //���˼��
}ENUM_SPEED;

//����ٶ���ϵ�λ
typedef enum
{
    //GEAR_STANDBY = 0,       //����
	GEAR_SLOWEST=0,           //������
	GEAR_SLOW,              //������
	GEAR_SLOW_NORMAL_FAST,  //���п���
	GEAR_SLOW_FAST_FASTEST, //���������
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
    uint8_t speed;//��������ٶ�
    uint8_t gear;//�ٶȵ�λ
    TYPE_MOTOR_STATE state;//�������״̬
    uint32_t counter_step; //�������
    uint32_t max_step; //��������
    uint32_t counter_hx_step; //��������
    uint32_t max_hx_step; //���������
    uint32_t timer;//�������ʱ��
    uint32_t timer_t_pulse; // t���˲���ʱ
    
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
