/*
*********************************************************************************************************
*
*	ģ������ : ������ģ�顣
*	�ļ����� : main.c
*	��    �� : V1.0
*	˵    �� : 
*                 1. �����������ĸ�����
*                    vTaskTaskUserIF ����: �ӿ���Ϣ����
*                    vTaskLCD        ����: LCDͨ��
*                    vTaskMotorCtrl  ����: �������
*                    vTaskStart      ����: ��������Ҳ����������ȼ�������������LED��˸
*              
*	�޸ļ�¼ :
*		�汾��    ����         ����            ˵��
*       V1.0    2016-03-15   Eric2013    1. ST�̼��⵽V3.6.1�汾
*                                        2. BSP������V1.2
*                                        3. FreeRTOS�汾V8.2.3
*
*	Copyright (C), 2016-2020, ���������� www.armfly.com
*
*********************************************************************************************************
*/
#define MAIN_GLOBAL

#include "includes.h"
#include "bsp.h"

#define HW_VER "V1.0.0" //����汾��

//uint8_t LCD_TEM[20];

/*
**********************************************************************************************************
											��������
**********************************************************************************************************
*/
//static void vTaskTaskUserIF(void *pvParameters);
static void vTaskLCD(void *pvParameters);
static void vTaskMotorCtrl(void *pvParameters);
//static void vTaskStart(void *pvParameters);
static void AppTaskCreate (void);

/*
**********************************************************************************************************
											��������
**********************************************************************************************************
*/

static TaskHandle_t xHandleTaskLCD = NULL;
static TaskHandle_t xHandleTaskMotorCtrl = NULL;
static TaskHandle_t xHandleTaskSensor = NULL;
static TaskHandle_t xHandleTaskAdc = NULL;
static TaskHandle_t xHandleTaskBle = NULL;

#define PRIO_TaskSensor             1
#define PRIO_TaskLCD                4
#define PRIO_TaskMotorCtrl          3
#define PRIO_TaskAdc                2
#define PRIO_TaskBle                5

#define STKSIZE_TaskSensor          128
#define STKSIZE_TaskLCD             512
#define STKSIZE_TaskMotorCtrl       512
#define STKSIZE_TaskAdc             128
#define STKSIZE_TaskBle             128

static void vTaskSensor(void *pvParameters);
static void vTaskAdc(void *pvParameters);
static void vTaskBle(void *pvParameters);

uint16_t P_GetMsgFromUart(COM_PORT_E _ucPort, uint8_t *msg)
{
	uint16_t i=0;
    uint8_t tem8=0;
    uint32_t tem32 = 0;
    
    if( comGetLen(_ucPort) > 3 ) //�ս�������
    {
        while( comGetLen(_ucPort) )
        {
            comGetChar(_ucPort, &tem8);
            if( i == 0 )
            {
                if( tem8 == 0xEE ) 
                {
                    *( msg + i++ ) = tem8;
                }
            }else
            {
                *( msg + i++ ) = tem8;
                
                tem32 = ((tem32<<8)|tem8);                           //ƴ�����4���ֽڣ����һ��32λ����
                if( tem32 == 0xFFFCFFFF )
                {
                    return i;
                }
//                if( i > 13 )
//                {
//                    if( tem8 == 0xFF ) 
//                    {
//                        return i;
//                    }else
//                    {
//                        return 0;
//                    }
//                }
            }
            vTaskDelay(pdMS_TO_TICKS(1));
        }
    }
    return 0;
}

//Ĭ�ϲ���
void TMY_ParaInit( void )
{
	//���������ʼ��
    //����
	Motor.state.bit.dir = UP;
    //�ٶ�
	Motor.speed = SLOW;
	Motor.gear = GEAR_SLOW_NORMAL_FAST; //���п��٣�Ĭ����Ч
    //SetTextValue(0, 16,"ֹͣ\x00");
    
    //Ƶ��
    //SetTextValue(0, 17,"0");
    
	Motor.max_step = MAX_STEP_18ML; //�����
	Motor.counter_step = 0; //����
	
	//������ʼ��
	Tmy.pressure = 0; //ѹ��
	Tmy.power_percent = 100; //����
	
    Tmy.Key.zdpk = 1; //�Զ��ſգ�Ĭ����Ч
    Tmy.Key.hx = 1; //������Ĭ����Ч
    Tmy.Key.zh = 0; //1.8ml��Ĭ����Ч
}


void start_set( void )
{
    // if(RCC_GetFlagStatus(RCC_FLAG_PORRST))
    // {
    //     debug_usr("power reset:");//�ϵ�����
    // }else
    // {
    //     debug_usr("other reset:");//������
    // }
    
    //debug_usr("%d %d\r\n", data_saved.data.n_reset, data_saved.data.n_error);//
    
    RCC->CSR|=0X1000000    ; //����жϱ�ʾ
    
    debug_usr("************************************\r\n");
    debug_usr("          ������ IV��\r\n\r\n");
    debug_usr("Version : %s\r\n", HW_VER);
    printf("************************************\r\n");
}

/*
*********************************************************************************************************
*	�� �� ��: main
*	����˵��: ��׼c������ڡ�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
int main(void)
{
	/* 
	  ����������ǰ��Ϊ�˷�ֹ��ʼ��STM32����ʱ���жϷ������ִ�У������ֹȫ���ж�(����NMI��HardFault)��
	  �������ĺô��ǣ�
	  1. ��ִֹ�е��жϷ����������FreeRTOS��API������
	  2. ��֤ϵͳ�������������ܱ���ж�Ӱ�졣
	  3. �����Ƿ�ر�ȫ���жϣ���Ҹ����Լ���ʵ��������ü��ɡ�
	  ����ֲ�ļ�port.c�еĺ���prvStartFirstTask�л����¿���ȫ���жϡ�ͨ��ָ��cpsie i������__set_PRIMASK(1)
	  ��cpsie i�ǵ�Ч�ġ�
     */
	__set_PRIMASK(1);  
	
	/* Ӳ����ʼ�� */
	bsp_Init(); 
	debug_en(1);

	ReadFromFlash();
	
    TMY_ParaInit();
	start_set();
	
	/* �������� */
	AppTaskCreate();
	
    /* �������ȣ���ʼִ������ */
    vTaskStartScheduler();

	/* 
	  ���ϵͳ���������ǲ������е�����ģ����е����Ｋ�п��������ڶ�ʱ��������߿��������
	  heap�ռ䲻����ɴ���ʧ�ܣ���Ҫ�Ӵ�FreeRTOSConfig.h�ļ��ж����heap��С��
	  #define configTOTAL_HEAP_SIZE	      ( ( size_t ) ( 17 * 1024 ) )
	*/
	while(1);
}

/*
*********************************************************************************************************
*	�� �� ��: vTaskTaskUserIF
*	����˵��: �ӿ���Ϣ��������������LED��˸	
*	��    ��: pvParameters ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
*   �� �� ��: 1  (��ֵԽС���ȼ�Խ�ͣ������uCOS�෴)
*********************************************************************************************************
*/
static void vTaskAdc(void *pvParameters)
{
    HX711_Init();
    while(1)
    {
        vTaskDelay(1000);
        Tmy.pressure = HX711_Read();
        Tmy.pressureLevel = PRE_GetLevel(Tmy.pressure);
	}
}

void BLE_Test(void)
{
    uint8_t err, bleChar, localChar;
//    uint8_t StrTem[20];
    
//    err = BLE_GetMsg( StrTem, 20);
//    StrTem[err] = 0;
    
//    if(err)
//    {
//        SetTextValue(0,26, StrTem);
//    }
    
    err = comGetChar(BLE_COM, &bleChar);
    if (err)
    {
        comSendChar(LOCAL_COM, bleChar);
        //SetTextValue(0,26,"�Զ���ҩȡ��");
    }

    err = comGetChar(LOCAL_COM, &localChar);
    if(err)
    {
        comSendChar(BLE_COM, localChar);
    }
}

static void vTaskBle(void *pvParameters)
{
    uint8_t err, bleChar;
    //GPIO_InitTypeDef GPIO_InitStructure;
    
    BLE_Init();
//    BLE_HwReset( );
//    BLE_PapaInit( );
    BLE_HwReset( );
    vTaskDelay( 2000 );
    
    
    // //�������򣬴������óɸ�������
    // RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    // GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
    // GPIO_Init(GPIOC, &GPIO_InitStructure);


    printf( "vTaskBle\r\n" );
    
//    while (1)
//    {
//        vTaskDelay(5);
//        BLE_Test();
//    }

    while (1)
    {
        vTaskDelay(20);
        
        err = comGetChar(BLE_COM, &bleChar);
        if (err)
        {
            if (bleChar == 0xAA) //��̧̤��
            {
                //Ble.state.bit.link = 0;
                bsp_PutKey(PEDAL_UP);
            }else
            if (bleChar == 0x55) //��̤����
            {
                //Ble.state.bit.link = 1
                bsp_PutKey(PEDAL_DOWN);
            }
            
            //SetTextValue(0,26,"�Զ���ҩȡ��");
        }

	}
}

/*
************************************************************************************************************
*	�� �� ��: vTaskSensor
*	����˵��: �������ɼ�����
*	��    ��: pvParameters ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
*   �� �� ��: ��
************************************************************************************************************
*/
static void vTaskSensor(void *pvParameters)
{
	uint8_t tem8 = 0;
	
//    SoftWdtInit( vTaskSensor_ID, 10 );
    
    //timer_sensor = 4;
    while( 1 )
    {
//        SoftWdtFed( vTaskSensor_ID );
        
        //vTaskDelay( pdMS_TO_TICKS(1000) );//
        vTaskDelay( 20 );
        
        ADC1_GetV();
        //Tmy.pressure = HX711_Read();
        
        //bsp_KeyScan();
        if( tem8 != Tmy.Sensor.bit.bottom )
        {
            tem8 = Tmy.Sensor.bit.bottom;
            
            if( Tmy.Sensor.bit.bottom ) //���׼��
            {
                SetTextValue(0,29,"�Ѵ���"); //�Ѵ���
            }else
            {
                SetTextValue(0,29,"δ����"); //δ����
            }
        }
        
		if( bsp_GetKeyState( KID_K1 ) ) //�����ƶ���ť
        {
            Tmy.Key.jjzd = 0; //
        }else //����
        {
            Tmy.Key.jjzd = 1; //
        }
		
        if( bsp_GetKeyState( KID_ZERO ) ) //���׼��
        {
            Tmy.Sensor.bit.bottom = 0; //δ����
        }else
        {
            Tmy.Sensor.bit.bottom = 1; //�Ѵ���
        }
        
        //if( Motor.counter_step > Motor.counter_hx_step-1 ) //������� //if( bsp_GetKeyState( KID_ZERO ) ) 
        if( Motor.counter_step >= Motor.max_step )//�������
        {
            Tmy.Sensor.bit.top = 1; //�Ѵ���
        }else
        {
            Tmy.Sensor.bit.top = 0; //δ����
        }
        
//        if( Tmy.State.bit.init ) //��ʼ�����
//        {
//            if( Motor.counter_step == 0 )//���͵׼��
//            {
//                Tmy.Sensor.bit.bottom = 1; //�Ѵ���
//            }else
//            {
//                Tmy.Sensor.bit.bottom = 0; //δ����
//            }
//        }
        
        if( bsp_GetKeyState( KID_SYRINGE ) ) //ע��������
        {
            Tmy.Sensor.bit.SyringeType = SYRINGE_2ML2; //2.2ml
        }else
        {
            Tmy.Sensor.bit.SyringeType = SYRINGE_1ML8; //1.8ml
        }
        
        if( bsp_GetKeyState( KID_SYRINGE ) ) //ע��������״̬ XXX
        {
            Tmy.Sensor.bit.SyringeState = SYRINGE_ON; //�Ѳ���
        }else
        {
            Tmy.Sensor.bit.SyringeState = SYRINGE_OFF; //δ����
        }
        
        if( Tmy.pressure > MAX_PRESSURE ) //��ѹ
        {
            Tmy.Sensor.bit.OverPressure = 1; //�Ѳ���
        }else
        {
            Tmy.Sensor.bit.OverPressure = 0; //δ����
        }

        // if (Ble.state.bit.link) //����BLE����ͼ��״̬
        // {

        // }else
        // {
            
        // }
        
    }
}

//void LCD_ParaInit( void )
//{
//    //����
//    if( Motor.state.bit.dir == UP )
//    {
//        SetTextValue(0,18,"����");
//    }else
//    {
//        SetTextValue(0,18,"�½�");
//    }
//    
//    //�ٶ�
//    SetTextValue(0, 16,"ֹͣ\x00");
//    
//    //Ƶ��
//    SetTextValue(0, 17,"0");
//    
//    //�����
//    if( Motor.max_step == MAX_STEP_18ML )                                                            //���Ҳ��ؼ�
//    {
//        SetTextInt32(0, 20, Motor.max_step, 0, 1); //��ʾ�����
//    }else if( Motor.max_step == MAX_STEP_22ML )                                                            //���Ҳ��ؼ�
//    {
//        SetTextInt32(0, 20, Motor.max_step, 0, 1); //��ʾ�����
//    }
//    
//    //����
//    SetTextInt32(0, 19, Motor.counter_step, 0, 1);
//}

/*
*********************************************************************************************************
*	�� �� ��: vTaskLCD
*	����˵��: LED��˸	
*	��    ��: pvParameters ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
*   �� �� ��: 2  
*********************************************************************************************************
*/
static void vTaskLCD(void *pvParameters)
{      
//    uint8_t err, tem8;
    
    ULCD_Init();
    printf( "vTaskLCD\r\n" );
    bsp_LedOff(1);
    
    //vTaskDelay(pdMS_TO_TICKS(10000));
    
    //LCD_ParaInit( );
    
    while(1)
    {
        vTaskDelay(1);
        ULCD_Task();
        
//        if( lg_lcd_init == 1 )
//        {
//            //SetBuzzer(100);
//            lg_lcd_init = 2;
//            LCD_ParaInit();
//            Tmy.State.bit.InitPara = 1;
//        }
        
    }
}

/*
*********************************************************************************************************
*	�� �� ��: vTaskMotorCtrl
*	����˵��: ��Ϣ����������������LED��˸	
*	��    ��: pvParameters ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
*   �� �� ��: 3  
*********************************************************************************************************
*/
//void SetControlVisiable(uint16 screen_id,uint16 control_id,uint8 visible)
static void vTaskMotorCtrl(void *pvParameters)
{
    uint8_t key_v, tem8;
    //uint8_t  key_state, key_state_c;
    uint8_t count_init_down;
    
    uint8_t dg_sta_c;
    uint8_t lg_zdgy = 0;//�Զ���ҩ̧���־��������
	
    MOTOR_Init( 1000 );
    key_v = 0;
    MOTOR_SetDir( UP );
    
    Motor.max_hx_step = 31495;
    
    count_init_down = 0;
    dev_sta = DEV_INIT;
    dg_sta = DG_INIT;
    //dg_sta = DG_STOP; // ���� �����ж��˹��㶯��
    
    vTaskDelay(10000);
    
    while(1)
    {
        vTaskDelay(2);
		
		if(lg_lcd_init == 0) continue; //�ȴ�LCD��ʼ�����
		    
		key_v = bsp_GetKey();
        //if( LCD_GetScreenId() == 0 ) //������һ
        {
            switch( key_v ) //
    		{
    		    case PEDAL_DOWN : //���½�̤���������
    			
    				timer_pedal_up = 0;
    		        
    				if( dg_sta == DG_UP ) //���������ڼ�
    		        {   
    					//�Զ���ҩ����£����½�̤�������κζ������ȵ�̧����ֹͣ
    					if( Tmy.State.bit.zdgy )
    					{
    					    timer_pedal_down = 1;
    					    Tmy.State.bit.zdgy = 0; //ȡ���Զ���ҩ
    					    //lg_zdgy = 0;//�Զ���ҩ̧���־��ȡ��
    					    //��������:�Զ���ҩȡ����
    					    SetTextValue(0,26,"�Զ���ҩȡ��");
    					    
    					}else //���Զ���ҩ
    					{
        					timer_pedal_down = 1;
        					//Tmy.State.bit.zdgy = 0;//ȡ���Զ���ҩ
        					lg_zdgy = 0;//�Զ���ҩ̧���־��ȡ��
        					
            	            Motor.speed++;
            	            if( Motor.speed > 3 ) 
            	            {
            	                Motor.speed = 1;
            	            }
            	            
            	            tem8 = MOTOR_SPEED_INDEX[Motor.gear][Motor.speed-1];
            	            Tmy.speed = tem8;
            	             
            	            MOTOR_SetSpeed( MOTOR_SPEED_CYCLE[tem8] ); //���õ���ٶ�
            	            SetTextValue(0, 16, &MOTOR_SPEED_DISPLAY[tem8][0]); 
            	            //SetTextInt32(0, 17, MOTOR_SPEED_CYCLE[tem8], 0, 1);
            	            //Tmy.work_state = STA_MOVE;
    					}
    		        }
    		        
    		        break;
    		    case PEDAL_UP : //̧���̤
    			
    				timer_pedal_down = 0;
    				
    		        if( dg_sta == DG_UP ) //���������ڼ�
    		        {
    					if( Tmy.State.bit.zdgy ) //�Զ���ҩ
        		        {
    						;//�Զ���ҩ
    					}else //���Զ���ҩ
    					{
    						//Motor.speed = 0;
    						MOTOR_Stop(); //���ֹͣ
    						timer_pedal_up = 1;//������̧̤���ʱ
    						
    						SetTextValue(0, 16,"ֹͣ\x00");
    						SetTextInt32(0, 17, 0, 0, 1);
    						//Tmy.work_state = STA_IDLE;
    					}
        	        }
    	            break;
    		    default : break;
    		}
            
    		if( dg_sta == DG_UP ) //���������ڼ�
    		{
    			// if( timer_pedal_up > DLY_PEDAL_UP_MOTOR_STOP ) //̧���̤ʱ����� DLY_PEDAL_UP_MOTOR_STOP ��
    			// {
    				// timer_pedal_up = 0;
    				// Motor.speed = 0; //�ָ��ٶ�
    				// if( Tmy.Key.hx )//&& (!Motor.state.bit.dir) )//����ģʽ��Ч Motor.state.bit.dir
    				// {
    					// if(!lg_zdgy) //���Զ���ҩ
    					// {
    						// lg_zdgy = 0;//�Զ���ҩ̧���־��ȡ��
    						// if( ( Tmy.speed == SLOWEST )   //������
    						 // || ( Tmy.speed == SLOW    ) ) //������
    						// {
    							// Motor.counter_hx_step = Motor.counter_step - Motor.max_hx_step;
    							// dg_sta = DG_HX_DOWN;//
    						// }
    					// }
    				// }
    			// }
    			//�����ж�
    			//if( ( Tmy.speed == SLOWEST )   //������
    			// || ( Tmy.speed == SLOW    ) ) //������
    			{
    				if( timer_pedal_up > DLY_PEDAL_UP_MOTOR_STOP ) //̧���̤ʱ����� DLY_PEDAL_UP_MOTOR_STOP ��
    				{
    					timer_pedal_up = 0;
    					Motor.speed = 0; //�ָ��ٶ�
    					if( ( Tmy.Key.hx ) //����ģʽ��Ч
    					  &&( ( Tmy.speed == SLOWEST ) || ( Tmy.speed == SLOW ) ) )//������ ������
    					{
    						if(!lg_zdgy) //���Զ���ҩ
    						{
    							lg_zdgy = 0;//�Զ���ҩ̧���־��ȡ��
    							Motor.counter_hx_step_up = Motor.counter_step; //�洢����λ��
    							//�жϻ������룬�˴����жϴ���STEP_HX_ENABLE����ſ��Ի�����STEP_HX_ENABLEĬ��0
    							if( Motor.counter_step > ( Motor.max_hx_step + STEP_HX_ENABLE ) ) //��Ч����
    							{
    								Motor.counter_hx_step = Motor.counter_step - Motor.max_hx_step; 
    								
    							}else //�жϻ������벻�����������ʹ���
    							{
    								Motor.counter_hx_step = 0;
    							}
    							dg_sta = DG_HX_DOWN;//�������
    						}
    					}
    				}
    			}
    			
    			//�Զ���ҩ�ж�
    			if( ( Tmy.speed == SLOWEST )   //������
    			 || ( Tmy.speed == SLOW    ) ) //������
    			{
    				if( ( !Tmy.State.bit.zdgy ) && ( timer_pedal_down > TIME_ZDGY_ENABLE ) ) //���½�̤ʱ�����TIME_CRUISE_START��
    				{
    					Tmy.State.bit.zdgy = 1; //�����Զ���ҩ
    					SetTextValue(0,26,"�Զ���ҩ...");
    					PlayMusic("a:/sounds/zdgy.mp3"); //��������:�Զ���ҩ
    					//���2���ڣ�̧�ţ��Զ���ҩ
    					//���2���ڣ�û��̧�ţ������Զ���ҩ
    					lg_zdgy = 1;//�Զ���ҩ̧���־��������
    				}
    				
    				if( ( Tmy.State.bit.zdgy ) && ( timer_pedal_down > TIME_ZDGY_CANCLE ) ) //���½�̤ʱ�����5��
    				{
    					Tmy.State.bit.zdgy = 0;//ȡ���Զ���ҩ
    					timer_pedal_down = 0;
    					SetTextValue(0,26,"�Զ���ҩȡ��");
    					//��������:�Զ���ҩȡ��xxx
    					//���2���ڣ�̧�ţ��Զ���ҩ
    					//���2���ڣ�û��̧�ţ������Զ���ҩ
    					lg_zdgy = 0;//�Զ���ҩ̧���־��ȡ��
    				}
    			}
    		}
		
		//�����ƶ������д�����
//		if( Tmy.Key.jjzd ) //�����ƶ�
//		{
//			MOTOR_Stop();//�������ֹͣ
//			Tmy.Motor.state = MOTOR_STOP;//���Ĳ������״̬
//			dg_sta = DG_STOP;
//		}
		
            switch( dg_sta )
            {
                case DG_INIT : //�ϵ磬������ʼ��
                    if( dg_sta_c != dg_sta )
                    {
                        dg_sta_c = dg_sta;
                        //SetTextValue(0,26,"��ʼ��...");
                    }
                    
                    if( !Tmy.State.bit.InitPara ) //������δ��ʼ��
                    {
                        if( lg_lcd_init == 1 )
                        {
                            //lg_lcd_init = 2;
                            LCD_ParaInit();
                            Tmy.State.bit.InitPara = 1;
                        }
                        //Tmy.State.bit.InitPara = 1;
                    }else //������ʼ�����
                    {
                        SetTextValue(0,26,"LCD��ʼ����ɡ�");
                        dg_sta = DG_INIT_DOWN;//���˹���
                    }
                    
                    break;
                
                case DG_INIT_UP : //�ϵ��ʼ�����̣����˲�����λ���ȹ��㣬Ȼ����������һС���ٹ���
                    
                    if( dg_sta_c != dg_sta )
                    {
                        dg_sta_c = dg_sta;
                        SetTextValue(0,26,"�����㴫����,����...");
                    }
                    
                    if( !Tmy.Sensor.bit.bottom )//����뿪�ײ�
                    {
                        dg_sta = DG_INIT_DOWN;//�ٴθ�λ
                    }
                    
                    if( Tmy.Motor.state != MOTOR_UP ) //�����������������
                    {
                        MOTOR_Stop();//�������ֹͣ
                        MOTOR_SetDir( UP );//��������
                        MOTOR_SetSpeed( CYCLE_HX ); //���õ���ٶ� XXX
                        Tmy.Motor.state = MOTOR_UP;//���Ĳ������״̬
                    }else //�������������
                    {
                        //����5�뻹δ��⵽�뿪�ײ�
                        if (!timer_dg_init)
                        {
                            dev_sta = DEV_ERR;

                            MOTOR_Stop();//�������ֹͣ
                            MOTOR_SetDir( UP );//��������
                            Tmy.Motor.state = MOTOR_STOP;//���Ĳ������״̬
                            dg_sta = DG_STOP;//
                        }
    //                    if( key_v == PEDAL_UP )//��⵽��̧̤��
    //                    {
    //                        dg_sta = DG_STOP;//ֹͣ
    //                    }
                    }
                    
                    break;
                case DG_INIT_DOWN : //�ϵ��ʼ�����̣����˲�����λ���ȹ��㣬Ȼ����������һС���ٹ���
                    
                    if( dg_sta_c != dg_sta )
                    {
                        dg_sta_c = dg_sta;
                        SetTextValue(0,26,"���˹���...");
                    }
                    
                    if( key_v == ZERO_DETECT )//��⵽����
                    {
                        Tmy.Sensor.bit.bottom = 1;
        		        MOTOR_Stop();
        		        Motor.counter_step = 0;
        		        SetTextValue(0,18,"�Ѵ���");
                    }
                    
                    if( Tmy.Sensor.bit.bottom )//����ײ�
                    {
                        if( !count_init_down ) //��һ�δ���
                        {
                            SetTextValue(0,26,"�Ѵ���");
                            count_init_down++;
                            dg_sta = DG_INIT_UP;//����һ��
                            timer_dg_init = 5; //5���ʼ������
                        }else //�ڶ��δ���
                        {
                            SetBuzzer(100);
                            SetTextValue(0,26,"���˹������");
                            Tmy.State.bit.init = 1; //��ʼ�����
                            dg_sta = DG_STOP_BOT;//ͣ�ڵײ�
                            //dev_sta = DEV_STANDBY;//�豸����  
                        }
                        
                    }else //δ����ײ�
                    {
                        if( Tmy.Motor.state != MOTOR_DOWN ) //��������������½�
                        {
                            MOTOR_Stop();//�������ֹͣ
                            MOTOR_SetDir( DOWN );//�����½�
                            MOTOR_SetSpeed( CYCLE_HX ); //���õ���ٶ� XXX
                            Tmy.Motor.state = MOTOR_DOWN;//���Ĳ������״̬
                        }else //����������½�
                        {
        //                    if( key_v == PEDAL_UP )//��⵽��̧̤��
        //                    {
        //                        dg_sta = DG_STOP;//ֹͣ
        //                    }
                        }
                    }
                    break;
                
                case DG_STOP_BOT : //ͣ���ڵײ�
                    //���ײ�������
                    MOTOR_ClrStep( );
                    
                    if( dg_sta_c != dg_sta )
                    {
                        dg_sta_c = dg_sta;
                        SetTextValue(0,26,"���״���");
                    }
                    
                    if( Tmy.Motor.state != MOTOR_STOP ) //�������������
                    {
                        MOTOR_Stop();//�������ֹͣ
                        MOTOR_SetDir( UP );//��������
                        Tmy.Motor.state = MOTOR_STOP;//���Ĳ������״̬
                        MOTOR_ClrStep( ); //��������
                    }
                    
                    if( key_v == PEDAL_DOWN ) //��⵽�Ƚ�̤
                    {
                        if( Tmy.Key.zdpk ) //�Զ��ſ�
                        {
                           // SetTextValue(0,26,"�Զ��ſ�...");
                            dg_sta = DG_ZDPK_UP;//��������
                        }else
                        if( Tmy.Key.ecpk ) //�����ſ�
                        {
                           // SetTextValue(0,26,"�����ſ�...");
                            dg_sta = DG_ECPK_UP;//��������
                        }else
                        //if( dev_sta == DEV_RUN )//�豸����״̬
                        {
                            //SetTextValue(0,26,"��������...");
                            dg_sta = DG_UP;//��������
                        }
                    }
    //                if(  )//���˼�鰴������
    //                {
    //                    if( dev_sta == DEV_FIX )//���˼��
    //                    {
    //                        dg_sta = DG_FIX_UP;//���˼������
    //                    }
    //                }
                    
    //                if( key_v == PEDAL_DOWN )//��⵽��̤����
    //                {
    //                    if( dev_sta == DEV_RUN )//�豸����״̬
    //                    {
    //                        dg_sta = DG_UP;//��������
    //                    }else
    //                    if( dev_sta == DEV_FIX )//���˼��
    //                    {
    //                        dg_sta = DG_FIX_UP;//���˼������
    //                    }
    //                }

                    break;
                        
                case DG_STOP : //ֹͣ
                    
                    if( dg_sta_c != dg_sta )
                    {
                        dg_sta_c = dg_sta;
                        SetTextValue(0,26,"����");

                        if (dev_sta == DEV_ERR) //�豸����
                        {
                            SetTextValue(0,26,"��λ����������");
                        }
                    }
                    
                    if( Tmy.Motor.state != MOTOR_STOP ) //�������������
                    {
                        MOTOR_Stop();//�������ֹͣ
                        //MOTOR_SetDir( UP );//��������
                        Tmy.Motor.state = MOTOR_STOP;//���Ĳ������״̬
                        //MOTOR_ClrStep( ); //��������
                    }
                    
                    if (dev_sta == DEV_ERR) //�豸����
                    {
                        //����
                        break;
                    }else
                    if( key_v == PEDAL_DOWN )//��⵽��̤����
                    {
                        //if( dev_sta == DEV_RUN )//�豸����״̬
                        {
                            timer_pedal_down = 1; //��ʼ��̤���¼�ʱ
                            dg_sta = DG_UP;//��������
                        }
                    }//else
    //                if( key_v == PEDAL_DOWN )//��⸴λ������
    //                {
    //                    if( )//�豸����״̬
    //                    {
    //                        dg_sta = DG_RESET_DOWN;//��λ�½�
    //                    }
    //                    ;
    //                }
                    break;
                    
                case DG_ZDPK_UP : //�Զ��ſ�
                    
                    if( dg_sta_c != dg_sta )
                    {
                        dg_sta_c = dg_sta;
                        SetTextValue(0,26,"�Զ��ſ�...");
                    }
                    
                    if( Motor.counter_step > STEP_MAX_ZDPK ) //�����Զ��ſղ���
                    {
                        dg_sta = DG_STOP;//���ֹͣ
                    }
                    
                    if( Tmy.Motor.state != MOTOR_UP ) //�����������������
                    {
                        MOTOR_Stop();//�������ֹͣ
                        MOTOR_SetDir( UP );//��������
                        MOTOR_SetSpeed( CYCLE_ZDPK ); //���õ����
                        Tmy.Motor.state = MOTOR_UP;//���Ĳ������״̬
                    }else //�������������
                    {
                        ;
                    }
                    
                    break;
                
                case DG_ECPK_UP : //�����ſ�
                     if( dg_sta_c != dg_sta )
                    {
                        dg_sta_c = dg_sta;
                        SetTextValue(0,26,"�����ſ�...");
                    }
                    
                    if( Motor.counter_step > STEP_MAX_ECPK ) //�����Զ��ſղ���
                    {
                        dg_sta = DG_STOP;//���ֹͣ
                    }
                    
                    if( Tmy.Motor.state != MOTOR_UP ) //�����������������
                    {
                        MOTOR_Stop();//�������ֹͣ
                        MOTOR_SetDir( UP );//��������
                        MOTOR_SetSpeed( CYCLE_ZDPK ); //���õ���ٶ� XXX
                        Tmy.Motor.state = MOTOR_UP;//���Ĳ������״̬
                    }else //�������������
                    {
                        ;
                    }
                    
                    break;
                    
                case DG_UP : //��������
                    
                    if( dg_sta_c != dg_sta )
                    {
                        dg_sta_c = dg_sta;
                        SetTextValue(0,26,"��������...");
                    }
                    
                    if( ( Motor.counter_step >= Motor.max_step ) //���ﶥ��
                     || ( Tmy.Sensor.bit.top ) )
                    {
                        MOTOR_Stop();//����������������һ�α���
                        dg_sta = DG_STOP_TOP;//ͣ���ڶ���
                    }else
                    {
                        if( Tmy.Motor.state != MOTOR_UP ) //�������δ����
                        {
                            Motor.speed = 1;
                            tem8 = MOTOR_SPEED_INDEX[Motor.gear][Motor.speed-1];
            	            Tmy.speed = tem8;
            	             
            	            MOTOR_SetSpeed( MOTOR_SPEED_CYCLE[tem8] ); //���õ���ٶ�
            	            SetTextValue(0, 16, &MOTOR_SPEED_DISPLAY[tem8][0]); 
        	            
                            MOTOR_Stop(); //�����ͣ
                            MOTOR_SetDir( UP ); //���õ������
                            MOTOR_SetSpeed( MOTOR_SPEED_CYCLE[tem8] ); //���õ���ٶȣ�����
                            
                            //SetTextValue(0,18,"���˼������"); //������ʾ
                        
                            Tmy.Motor.state = MOTOR_UP;//
                        }
                    }
                    
    //                if( Tmy.Motor.state != MOTOR_UP ) //�����������������
    //                {
    //                    MOTOR_Stop();//�������ֹͣ
    //                    MOTOR_SetDir( UP );//��������
    //                    MOTOR_SetSpeed( CYCLE_KSJ ); //���õ���ٶ� XXX
    //                    Tmy.Motor.state = MOTOR_UP;//���Ĳ������״̬
    //                }else //�������������
    //                {
    //                    if( key_v == PEDAL_UP )//��⵽��̧̤��
    //                    {
    //                        dg_sta = DG_STOP;//ֹͣ
    //                    }
    //                }
                    
                    break;
                
                case DG_STOP_TOP : //ͣ���ڶ���
                    
                    if( dg_sta_c != dg_sta )
                    {
                        dg_sta_c = dg_sta;
                        SetTextValue(0,26,"����ͣ������");
                    }
                    
                    if( Tmy.Motor.state != MOTOR_STOP ) //�������������
                    {
                        MOTOR_Stop();//�����ͣ
                        MOTOR_SetDir( DOWN ); //�����½�
                        Tmy.Motor.state = MOTOR_STOP;//���ĵ��״̬
                    }
                    
                    if( key_v == PEDAL_DOWN ) //��⵽�Ƚ�̤ // || ( key_v == PEDAL_DOWN ) ) //�� ����λ��
                    {
                        //if( dev_sta == DEV_RUN )//�豸����״̬
                        {
                            SetTextValue(0,26,"���˹���...");
                            dg_sta = DG_FIX_DOWN;//���˹���
                        }
                    }
                    
    				//if( 1 )//��xxx�ٶ��£��������Զ�����
    				if( ( Tmy.speed == NORMAL )   //����
    			     || ( Tmy.speed == FAST    ) ) //FAST
    				{
    					SetTextValue(0,26,"���˹���...");
                        dg_sta = DG_FIX_DOWN;//���˹���
    				}
    				
                    break;
                    
                case DG_DOWN : //�����½� ����û�д�״̬������
                    
                    if( dg_sta_c != dg_sta )
                    {
                        dg_sta_c = dg_sta;
                        SetTextValue(0,26,"�����½�...");
                    }
                    
                    if(Tmy.Sensor.bit.bottom) //����ײ�
                    {
                        dg_sta = DG_STOP_TOP;//ͣ���ڵײ�
                    }
                    
                    if( Tmy.Motor.state != MOTOR_DOWN ) //��������������½�
                    {
                        MOTOR_Stop();//�������ֹͣ
                        MOTOR_SetDir( DOWN );//��������
                        MOTOR_SetSpeed( CYCLE_DGJC ); //���õ���ٶ� XXX
                        Tmy.Motor.state = MOTOR_DOWN;//���Ĳ������״̬
                    }else //����������½�
                    {
                        if( key_v == PEDAL_UP )//��⵽��̧̤��
                        {
                            dg_sta = DG_STOP;//ֹͣ
                        }
                    }
                    break;
                    
                case DG_HX_DOWN : //�����½�
    			
                    if( dg_sta_c != dg_sta )
                    {
                        dg_sta_c = dg_sta;
                        SetTextValue(0,26,"�����½�...");
                    }
                    
                    if( ( Motor.counter_step <= Motor.counter_hx_step )//��������ײ�
    								  ||( Motor.counter_step == 0 ) )//����,�˴��Դ��������������
                    {
                        MOTOR_Stop();//���ֹͣ
                        Motor.counter_hx_step = Motor.counter_hx_step_up;//Motor.counter_step + Motor.max_hx_step;
                        
                        if( Tmy.speed == SLOW )   //������
                        {
                            vTaskDelay(pdMS_TO_TICKS(1000));//��ʱ1��
                        }
                        dg_sta = DG_HX_UP;//
                    }else
                    {
                        if( Tmy.Motor.state != MOTOR_DOWN ) //�������δ�½�
                        {
                            MOTOR_Stop(); //�����ͣ
                            MOTOR_SetDir( DOWN ); //���õ������
                            MOTOR_SetSpeed( CYCLE_HX ); //���õ���ٶȣ�����
                            SetTextValue(0,18,"�����½�"); //������ʾ
                            Tmy.Motor.state = MOTOR_DOWN;//
                        }
                    }
                    
    //	            if( Motor.counter_step > Motor.counter_hx_step ) //
    //	            {
    //	                ;//�н���
    //	            }else //��������ײ�
    //	            {
    //	                MOTOR_Stop();
    //	                dg_sta = DG_HX_UP;//
    //                    Motor.counter_hx_step = Motor.counter_step + Motor.max_hx_step;
    //                    MOTOR_SetDir( UP );
    //	                MOTOR_SetSpeed( CYCLE_HX ); //���õ���ٶ�
    //                    SetTextValue(0,18,"��������");
    //                    if( Tmy.speed == SLOW )   //������
    //                    {
    //                        vTaskDelay(pdMS_TO_TICKS(1000));//��ʱ1��
    //                    }
    //	            }
    	            
    	            break;
    	            
    	        case DG_HX_UP : //��������
    	            
    	            if( dg_sta_c != dg_sta )
                    {
                        dg_sta_c = dg_sta;
                        SetTextValue(0,26,"��������...");
                    }
                    
                    if( Motor.counter_step > Motor.counter_hx_step-1 )//�����������
                    {
                        MOTOR_Stop(); //���ֹͣ
                        //Tmy.state.bit.hx = 0;
                        dg_sta = DG_STOP; //
                    }else
                    {
                        if( Tmy.Motor.state != MOTOR_UP ) //�������δ�½�
                        {
                            MOTOR_Stop(); //�����ͣ
                            MOTOR_SetDir( UP ); //���õ������
                            MOTOR_SetSpeed( CYCLE_HX ); //���õ���ٶȣ�����
                            SetTextValue(0,18,"��������"); //������ʾ
                            Tmy.Motor.state = MOTOR_UP;//
                        }
                    }
                    
    //	            if( Motor.counter_step < Motor.counter_hx_step ) 
    //	            {
    //	                ;//�н���
    //	            }else //�����������
    //	            {
    //	                MOTOR_Stop();
    //	                Tmy.state.bit.hx = 0;
    //	                Tmy.work_state = STA_STANDBY;
    //	            }
    	            
    	            break;
    	            
                case DG_FIX_UP : //���˼������
                    
                    if( ( Motor.counter_step >= Motor.max_step ) //���ﶥ��
                     || ( Tmy.Sensor.bit.top ) )
                    {
                        MOTOR_Stop();//����������������һ�α���
                        dg_sta = DG_STOP_TOP;//ͣ���ڶ���
                    }else
                    {
                        if( Tmy.Motor.state != MOTOR_UP ) //�������δ����
                        {
                            MOTOR_Stop(); //�����ͣ
                            MOTOR_SetDir( UP ); //���õ������
                            MOTOR_SetSpeed( CYCLE_DGJC ); //���õ���ٶȣ�����
                            SetTextValue(2,26,"���˼������"); //������ʾ
                        
                            Tmy.Motor.state = MOTOR_UP;//
                        }
                    }
                    
                    break;
                    
                case DG_FIX_DOWN : //���˼���½�
                    
                    if( Tmy.Sensor.bit.bottom )//��⵽��
                    {
                        MOTOR_Stop();//����������������һ�α���
                        dg_sta = DG_STOP_BOT;//
                    }else
                    {
                        if( Tmy.Motor.state != MOTOR_DOWN ) //�������δ�½�
                        {
                            MOTOR_Stop(); //�����ͣ
                            MOTOR_SetDir( DOWN ); //���õ������
                            MOTOR_SetSpeed( CYCLE_DGJC ); //���õ���ٶȣ�����
                            SetTextValue(2,26,"���˼���½�"); //������ʾ
                        
                            Tmy.Motor.state = MOTOR_DOWN;//
                        }
                    }
                    
                    break;
                
                default : 
                    break;
            }
        }
    }
}



		
//        switch( dev_sta )
//        {
//            case DEV_INIT :
//                //���ó�ʼ����
//                //DEV_SetInitPara( );
//                if( !Tmy.State.bit.init ) //δ��ʼ
//                {
//                    dg_sta = DG_RESET_DOWN;
//                }else//��ʼ�����
//                {
//                    dev_sta = DEV_STANDBY;//�豸����
//                }
//                break;
//            case DEV_RUN : //����
//                break;
//            case DEV_FIX :
//                break;
//            case DEV_STANDBY : //����
//                if( Tmy.State.bit.init ) //��ʼ�����
//                {
//                    ;
//                }
//                break;
//            default : break;
//        }

/*
*********************************************************************************************************
*	�� �� ��: vTaskStart
*	����˵��: ��������Ҳ����������ȼ�������������LED��˸
*	��    ��: pvParameters ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
*   �� �� ��: 4  
*********************************************************************************************************
*/
//static void vTaskStart(void *pvParameters)
//{
//    while(1)
//    {
//		/* LED��˸ */
//		bsp_LedToggle(4);
//        vTaskDelay(400);
//    }
//}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskCreate
*	����˵��: ����Ӧ������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void AppTaskCreate (void)
{
	xTaskCreate( vTaskLCD,    		/* ������  */
                 "vTaskLCD",  		/* ������    */
                 STKSIZE_TaskLCD,         		/* ����ջ��С����λword��Ҳ����4�ֽ� */
                 NULL,        		/* �������  */
                 PRIO_TaskLCD,      /* �������ȼ�*/
                 &xHandleTaskLCD ); /* ������  */
	
	xTaskCreate( vTaskMotorCtrl,     		/* ������  */
                 "vTaskMotorCtrl",   		/* ������    */
                 STKSIZE_TaskMotorCtrl,     /* ����ջ��С����λword��Ҳ����4�ֽ� */
                 NULL,           		    /* �������  */
                 PRIO_TaskMotorCtrl,        /* �������ȼ�*/
                 &xHandleTaskMotorCtrl );  /* ������  */
	
    xTaskCreate( vTaskSensor,     		    /* ������  */
                 "vTaskSensor",   		    /* ������    */
                 STKSIZE_TaskSensor,        /* ����ջ��С����λword��Ҳ����4�ֽ� */
                 NULL,           		    /* �������  */
                 PRIO_TaskSensor,           /* �������ȼ�*/
                 &xHandleTaskSensor );      /* ������  */
    
    xTaskCreate( vTaskAdc,   	/* ������  */
                 "vTaskAdc",     	/* ������    */
                 STKSIZE_TaskAdc,               	/* ����ջ��С����λword��Ҳ����4�ֽ� */
                 NULL,              	/* �������  */
                 PRIO_TaskAdc,                 	/* �������ȼ�*/
                 &xHandleTaskAdc );  /* ������  */
                 
    xTaskCreate( vTaskBle,   	/* ������  */
                 "vTaskAdc",     	/* ������    */
                 STKSIZE_TaskBle,               	/* ����ջ��С����λword��Ҳ����4�ֽ� */
                 NULL,              	/* �������  */
                 PRIO_TaskBle,                 	/* �������ȼ�*/
                 &xHandleTaskBle );  /* ������  */
}

/*
EE B1 11 00 2B 00 05 10 01 01 FF FC FF FF 
EE B1 11 00 2B 00 06 10 01 01 FF FC FF FF 
EE B1 11 00 2B 00 07 10 01 01 FF FC FF FF 
EE B1 11 00 2B 00 08 10 01 01 FF FC FF FF 

static void vTaskMotorCtrl2(void *pvParameters)
{
    uint8_t key_v, tem8;
    //uint8_t  key_state, key_state_c;
    
    MOTOR_Init( 1000 );
    key_v = 0;
    MOTOR_SetDir( UP );
    
    Motor.max_hx_step = 31495;
    Tmy.work_state = STA_STANDBY;
    
    while(1)
    {
		vTaskDelay(2);
		
        //bsp_KeyScan();
        
		key_v = bsp_GetKey();
		
		//key_state = bsp_GetKeyState( KID_SYRINGE );
//		if( key_state_c != key_state )
//		{
//		    key_state_c = key_state;
//		    if( key_state == KEY_UP )
//		    {
//		        SetTextValue(0,18,"[1.8ml]");
//		    }else
//		    {
//		        SetTextValue(0,18,"[2.2ml]");
//		    }
//		}
		
		if( timer_pedal_up > 2 ) //̧���̤ʱ�����2��
		{
		    timer_pedal_up = 0;
		    Motor.speed = 0; //�ָ��ٶ�
            //if( Tmy.key.hx && Tmy.state.bit.dir )//����ģʽ��Ч Motor.state.bit.dir
       			if( Tmy.key.hx && (!Motor.state.bit.dir) )//����ģʽ��Ч Motor.state.bit.dir
            {
                if( ( Tmy.speed == SLOW )   //������
                 || ( Tmy.speed == NORMAL ) //����
                 || ( Tmy.speed == FAST ) ) //���� 
                 {
                    Tmy.state.bit.hx = 1;
                    Tmy.work_state = STA_HX_DOWN;
                    Motor.counter_hx_step = Motor.counter_step - Motor.max_hx_step;
                    MOTOR_SetDir( DOWN );
	                MOTOR_SetSpeed( CYCLE_HX ); //���õ���ٶ�
                    SetTextValue(0,18,"�����½�");
                 }
            }
		}
		
		switch( Tmy.work_state )
	    {
	        case STA_IDLE : //����
	            break;
	        case STA_MOVE : //�������
        		//if( Motor.state.bit.move )
        		{
        		
//        		    if( ( Motor.counter_step > Motor.max_step-1 ) //�ﵽ����
//            		 //|| ( Motor.counter_step == 0 )
//            		 || ( Motor.state.bit.zero ) ) //�ﵽ�ײ�
            		 
            		if( Motor.state.bit.dir == UP )
            		{
            		    if( Motor.counter_step > Motor.max_step-1 ) //�ﵽ����
            		    {
                		    MOTOR_Stop();
                		    Motor.counter_step = Motor.max_step;
                		    Tmy.work_state = STA_IDLE;//
                		    
                		    SetProgressValue(0,12,100);
                            SetTextInt32(0, 19, Motor.max_step, 0, 1);
            		    }
            		}else
            		if( Motor.state.bit.dir == DOWN )
            		{
            		    if( Motor.state.bit.zero ) //�ﵽ�ײ�
            		    {
            		        MOTOR_Stop();
            		        //MOTOR_ClrStep();
            		        Motor.counter_step = 0;
            		        Tmy.work_state = STA_IDLE;//
            		        
            		        SetProgressValue(0,12,0);
                            SetTextInt32(0, 19, 0, 0, 1);
                
            		    }
            		}
        		}
	            break;
	        case STA_SLEEP : //����
	            break;
	        case STA_STANDBY : //����
	            MOTOR_Stop();
	            MOTOR_SetDir( UP );
	            Tmy.work_state = STA_IDLE;
	            break;
	            
	        case STA_HX_DOWN : //��������
	            if( Motor.counter_step > Motor.counter_hx_step ) //
	            {
	                ;//�н���
	            }else //��������ײ�
	            {
	                MOTOR_Stop();
	                Tmy.work_state = STA_HX_UP;
                    Motor.counter_hx_step = Motor.counter_step + Motor.max_hx_step;
                    MOTOR_SetDir( UP );
	                MOTOR_SetSpeed( CYCLE_HX ); //���õ���ٶ�
                    SetTextValue(0,18,"��������");
                    if( Tmy.speed == SLOW )   //������
                    {
                        vTaskDelay(pdMS_TO_TICKS(1000));//��ʱ1��
                    }
	            }
	            break;
	        case STA_HX_UP : //��������
	            if( Motor.counter_step < Motor.counter_hx_step ) 
	            {
	                ;//�н���
	            }else //�����������
	            {
	                MOTOR_Stop();
	                Tmy.state.bit.hx = 0;
	                Tmy.work_state = STA_STANDBY;
	            }
	            break;
	            
	        case STA_FIX : //���˼��
	            MOTOR_Stop();
                MOTOR_SetDir( UP );
                MOTOR_SetSpeed( CYCLE_KSJ ); //���õ���ٶ�
                SetTextValue(0,18,"���˼��");
                Tmy.work_state = STA_MOVE;
	            break;
	        case STA_RESET : //��λ
	            MOTOR_Stop();
                MOTOR_SetDir( DOWN );
                MOTOR_SetSpeed( CYCLE_KSJ ); //���õ���ٶ�
                SetTextValue(0,18,"��λ");
                Tmy.work_state = STA_MOVE;
	            break;
	        
	        case STA_WORK : //����
	            break;
	           
	        default : break;
	    }
		switch( key_v ) //
		{
		    case PEDAL_DOWN : //���½�̤���������
		        
		        timer_pedal_up = 0;
		        
	            Motor.speed++;
	            if( Motor.speed > 3 ) 
	            {
	                Motor.speed = 1;
	            }
	            
	            tem8 = MOTOR_SPEED_INDEX[Motor.gear][Motor.speed-1];
	            Tmy.speed = tem8;
	             
	            MOTOR_SetSpeed( MOTOR_SPEED_CYCLE[tem8] ); //���õ���ٶ�
	            SetTextValue(0, 16, &MOTOR_SPEED_DISPLAY[tem8][0]); 
	            //SetTextInt32(0, 17, MOTOR_SPEED_CYCLE[tem8], 0, 1);
	            Tmy.work_state = STA_MOVE;
		        break;
		    case PEDAL_UP : //̧���̤�����ֹͣ
		        //Motor.speed = 0;
	            MOTOR_Stop();
	            timer_pedal_up = 1;//������̧̤���ʱ
	            
	            SetTextValue(0, 16,"ֹͣ\x00");
	            SetTextInt32(0, 17, 0, 0, 1);
	            Tmy.work_state = STA_IDLE;
	            break;
		    case ZERO_UNDETECT : //
		        Motor.state.bit.zero = 0;
		        SetTextValue(0,18,"δ����");
		        break;
	        case ZERO_DETECT : //
	            Motor.state.bit.zero = 1;
		        MOTOR_Stop();
		        Motor.counter_step = 0;
		        SetTextValue(0,18,"�Ѵ���");
		    case SYRINGE_UNDETECT : //
//		        Motor.state.bit.zero = 0;
		        //SetTextValue(0,18,"1.8ml");
		        break;
	        case SYRINGE_DETECT : //
//	            Motor.state.bit.zero = 1;
//		        MOTOR_Stop();
//		        Motor.counter_step = 0;
		        //SetTextValue(0,18,"2.2ml");
		        break;
	             
		    default : break;
		}
	}
	
}



		
*/