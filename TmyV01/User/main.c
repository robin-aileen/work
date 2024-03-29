/*
*********************************************************************************************************
*
*	模块名称 : 主程序模块。
*	文件名称 : main.c
*	版    本 : V1.0
*	说    明 : 
*                 1. 创建了如下四个任务：
*                    vTaskTaskUserIF 任务: 接口消息处理
*                    vTaskLCD        任务: LCD通信
*                    vTaskMotorCtrl  任务: 电机控制
*                    vTaskStart      任务: 启动任务，也就是最高优先级任务，这里用作LED闪烁
*              
*	修改记录 :
*		版本号    日期         作者            说明
*       V1.0    2016-03-15   Eric2013    1. ST固件库到V3.6.1版本
*                                        2. BSP驱动包V1.2
*                                        3. FreeRTOS版本V8.2.3
*
*	Copyright (C), 2016-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#define MAIN_GLOBAL

#include "includes.h"
#include "bsp.h"

#define HW_VER "V1.0.0" //程序版本号

//uint8_t LCD_TEM[20];

/*
**********************************************************************************************************
											函数声明
**********************************************************************************************************
*/
//static void vTaskTaskUserIF(void *pvParameters);
static void vTaskLCD(void *pvParameters);
static void vTaskMotorCtrl(void *pvParameters);
//static void vTaskStart(void *pvParameters);
static void AppTaskCreate (void);

/*
**********************************************************************************************************
											变量声明
**********************************************************************************************************
*/
//static TaskHandle_t xHandleTaskUserIF = NULL;
static TaskHandle_t xHandleTaskLCD = NULL;
static TaskHandle_t xHandleTaskMotorCtrl = NULL;
static TaskHandle_t xHandleTaskSensor = NULL;

#define PRIO_TaskSensor             1
#define PRIO_TaskLCD                4
#define PRIO_TaskMotorCtrl          3

#define STKSIZE_TaskSensor          128
#define STKSIZE_TaskLCD             512
#define STKSIZE_TaskMotorCtrl       512

static void vTaskSensor(void *pvParameters);

uint16_t P_GetMsgFromUart(COM_PORT_E _ucPort, uint8_t *msg)
{
	uint16_t i=0;
    uint8_t tem8=0;
    uint32_t tem32 = 0;
    
    if( comGetLen(_ucPort) > 3 ) //收接收数据
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
                
                tem32 = ((tem32<<8)|tem8);                           //拼接最后4个字节，组成一个32位整数
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

//默认参数
void TMY_ParaInit( void )
{
	//步进电机初始化
    //方向
	Motor.state.bit.dir = UP;
    //速度
	Motor.speed = SLOW;
	Motor.gear = GEAR_SLOW_NORMAL_FAST; //慢中快速，默认有效
    //SetTextValue(0, 16,"停止\x00");
    
    //频率
    //SetTextValue(0, 17,"0");
    
	Motor.max_step = MAX_STEP_18ML; //最大步数
	Motor.counter_step = 0; //步数
	
	//其它初始化
	Tmy.pressure = 0; //压力
	Tmy.power_percent = 100; //电量
	
    Tmy.Key.zdpk = 1; //自动排空，默认有效
    Tmy.Key.hx = 1; //回吸，默认有效
    Tmy.Key.zh = 0; //1.8ml，默认有效
}


void start_set( void )
{
    if(RCC_GetFlagStatus(RCC_FLAG_PORRST))
    {
        debug_usr("power reset:");//上电重启
    }else
    {
        debug_usr("other reset:");//软重启
    }
    
    //debug_usr("%d %d\r\n", data_saved.data.n_reset, data_saved.data.n_error);//
    
    RCC->CSR|=0X1000000    ; //清除中断标示
    
    //debug_usr("Train num : %02x%02x max_room_n:%d\r\n", Train.TrainNum[0], Train.TrainNum[1], Train.max_room_n );
    //printf("Train num : %02x%02x max_room_n:%d\r\n", Train.TrainNum[0], Train.TrainNum[1], Train.max_room_n );
    
    debug_usr("************************************\r\n");
    debug_usr("          推麻仪 IV型\r\n\r\n");
    debug_usr("Version : %s\r\n", HW_VER);
    //printf("ID : %c%d - %02d\r\n", Train.TrainType, ( ( ( (uint16_t)Train.TrainNum[0]) << 8 ) + Train.TrainNum[1]), Train.max_room_n );
    //printf("ID : %02x%02x - %d\r\n", Train.TrainNum[0], Train.TrainNum[1], Train.max_room_n );
    
    printf("************************************\r\n");
}

/*
*********************************************************************************************************
*	函 数 名: main
*	功能说明: 标准c程序入口。
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
int main(void)
{
	/* 
	  在启动调度前，为了防止初始化STM32外设时有中断服务程序执行，这里禁止全局中断(除了NMI和HardFault)。
	  这样做的好处是：
	  1. 防止执行的中断服务程序中有FreeRTOS的API函数。
	  2. 保证系统正常启动，不受别的中断影响。
	  3. 关于是否关闭全局中断，大家根据自己的实际情况设置即可。
	  在移植文件port.c中的函数prvStartFirstTask中会重新开启全局中断。通过指令cpsie i开启，__set_PRIMASK(1)
	  和cpsie i是等效的。
     */
	__set_PRIMASK(1);  
	
	/* 硬件初始化 */
	bsp_Init(); 
	debug_en( 1 );
	
	TMY_ParaInit();
	start_set();
	
	/* 创建任务 */
	AppTaskCreate();
	
    /* 启动调度，开始执行任务 */
    vTaskStartScheduler();

	/* 
	  如果系统正常启动是不会运行到这里的，运行到这里极有可能是用于定时器任务或者空闲任务的
	  heap空间不足造成创建失败，此要加大FreeRTOSConfig.h文件中定义的heap大小：
	  #define configTOTAL_HEAP_SIZE	      ( ( size_t ) ( 17 * 1024 ) )
	*/
	while(1);
}

/*
*********************************************************************************************************
*	函 数 名: vTaskTaskUserIF
*	功能说明: 接口消息处理，这里用作LED闪烁	
*	形    参: pvParameters 是在创建该任务时传递的形参
*	返 回 值: 无
*   优 先 级: 1  (数值越小优先级越低，这个跟uCOS相反)
*********************************************************************************************************
*/
//static void vTaskTaskUserIF(void *pvParameters)
//{
//    while(1)
//    {
//        vTaskDelay(100);
//        
//		bsp_LedToggle(LED_STA);
//		
//	}
//}

/*
************************************************************************************************************
*	函 数 名: vTaskSensor
*	功能说明: 传感器采集任务
*	形    参: pvParameters 是在创建该任务时传递的形参
*	返 回 值: 无
*   优 先 级: 无
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
        vTaskDelay( 2 );
        
        ADC1_GetV();
        //Tmy.pressure = HX711_Read();
        
        //bsp_KeyScan();
        if( tem8 != Tmy.Sensor.bit.bottom )
        {
            tem8 = Tmy.Sensor.bit.bottom;
            
            if( Tmy.Sensor.bit.bottom ) //触底检测
            {
                SetTextValue(0,29,"已触底"); //已触底
            }else
            {
                SetTextValue(0,29,"未触底"); //未触底
            }
        }
        
        if( bsp_GetKeyState( KID_ZERO ) ) //触底检测
        {
            Tmy.Sensor.bit.bottom = 0; //未触底
        }else
        {
            Tmy.Sensor.bit.bottom = 1; //已触底
        }
        
        //if( Motor.counter_step > Motor.counter_hx_step-1 ) //触顶检测 //if( bsp_GetKeyState( KID_ZERO ) ) 
        if( Motor.counter_step >= Motor.max_step )//触顶检测
        {
            Tmy.Sensor.bit.top = 1; //已触顶
        }else
        {
            Tmy.Sensor.bit.top = 0; //未触顶
        }
        
//        if( Tmy.State.bit.init ) //初始化完成
//        {
//            if( Motor.counter_step == 0 )//触低底检测
//            {
//                Tmy.Sensor.bit.bottom = 1; //已触底
//            }else
//            {
//                Tmy.Sensor.bit.bottom = 0; //未触底
//            }
//        }
        
        if( bsp_GetKeyState( KID_SYRINGE ) ) //注射器类型
        {
            Tmy.Sensor.bit.SyringeType = SYRINGE_2ML2; //2.2ml
        }else
        {
            Tmy.Sensor.bit.SyringeType = SYRINGE_1ML8; //1.8ml
        }
        
        if( bsp_GetKeyState( KID_SYRINGE ) ) //注射器插入状态 XXX
        {
            Tmy.Sensor.bit.SyringeState = SYRINGE_ON; //已插入
        }else
        {
            Tmy.Sensor.bit.SyringeState = SYRINGE_OFF; //未插入
        }
        
        if( Tmy.pressure > MAX_PRESSURE ) //过压
        {
            Tmy.Sensor.bit.OverPressure = 1; //已插入
        }else
        {
            Tmy.Sensor.bit.OverPressure = 0; //未插入
        }
    }
}

//void LCD_ParaInit( void )
//{
//    //方向
//    if( Motor.state.bit.dir == UP )
//    {
//        SetTextValue(0,18,"上升");
//    }else
//    {
//        SetTextValue(0,18,"下降");
//    }
//    
//    //速度
//    SetTextValue(0, 16,"停止\x00");
//    
//    //频率
//    SetTextValue(0, 17,"0");
//    
//    //最大步数
//    if( Motor.max_step == MAX_STEP_18ML )                                                            //正弦波控件
//    {
//        SetTextInt32(0, 20, Motor.max_step, 0, 1); //显示最大步数
//    }else if( Motor.max_step == MAX_STEP_22ML )                                                            //正弦波控件
//    {
//        SetTextInt32(0, 20, Motor.max_step, 0, 1); //显示最大步数
//    }
//    
//    //步数
//    SetTextInt32(0, 19, Motor.counter_step, 0, 1);
//}

/*
*********************************************************************************************************
*	函 数 名: vTaskLCD
*	功能说明: LED闪烁	
*	形    参: pvParameters 是在创建该任务时传递的形参
*	返 回 值: 无
*   优 先 级: 2  
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
*	函 数 名: vTaskMotorCtrl
*	功能说明: 信息处理，这里是用作LED闪烁	
*	形    参: pvParameters 是在创建该任务时传递的形参
*	返 回 值: 无
*   优 先 级: 3  
*********************************************************************************************************
*/

static void vTaskMotorCtrl(void *pvParameters)
{
    uint8_t key_v, tem8;
    //uint8_t  key_state, key_state_c;
    uint8_t count_init_down;
    
    uint8_t dg_sta_c;
    
    MOTOR_Init( 1000 );
    key_v = 0;
    MOTOR_SetDir( UP );
    
    Motor.max_hx_step = 31495;
    
    count_init_down = 0;
    dev_sta = DEV_INIT;
    dg_sta = DG_INIT;
    
    while(1)
    {
        vTaskDelay(2);
		
		//key_state = bsp_GetKeyState( KID_SYRINGE );
		//key_state_c = key_state;
			
		key_v = bsp_GetKey();
        
        switch( key_v ) //
		{
		    case PEDAL_DOWN : //按下脚踏，电机运行
			
				timer_pedal_up = 0;
		        
				if( dg_sta == DG_UP ) //顶杆上升期间
		        {   
					//自动给药情况下，按下脚踏，停机?
					if( Tmy.State.bit.zdgy )
					{
					    timer_pedal_down = 1;
					    Tmy.State.bit.zdgy = 0; //取消自动给药
					    //播放语音:自动给药取消？
					    SetTextValue(0,26,"自动给药取消");
					    
					}else //非自动给药
					{
    					timer_pedal_down = 1;
    					//Tmy.State.bit.zdgy = 0;//取消自动给药
    					
        	            Motor.speed++;
        	            if( Motor.speed > 3 ) 
        	            {
        	                Motor.speed = 1;
        	            }
        	            
        	            tem8 = MOTOR_SPEED_INDEX[Motor.gear][Motor.speed-1];
        	            Tmy.speed = tem8;
        	             
        	            MOTOR_SetSpeed( MOTOR_SPEED_CYCLE[tem8] ); //设置电机速度
        	            SetTextValue(0, 16, &MOTOR_SPEED_DISPLAY[tem8][0]); 
        	            //SetTextInt32(0, 17, MOTOR_SPEED_CYCLE[tem8], 0, 1);
        	            //Tmy.work_state = STA_MOVE;
					}
		        }
		        
		        break;
		    case PEDAL_UP : //抬起脚踏
			
				timer_pedal_down = 0;
				
		        if( dg_sta == DG_UP ) //顶杆上升期间
		        {
					if( Tmy.State.bit.zdgy ) //自动给药
    		        {
						;//自动给药
					}else //非自动给药
					{
						//Motor.speed = 0;
						MOTOR_Stop(); //电机停止
						timer_pedal_up = 1;//启动脚踏抬起计时
						
						SetTextValue(0, 16,"停止\x00");
						SetTextInt32(0, 17, 0, 0, 1);
						//Tmy.work_state = STA_IDLE;
					}
    	        }
	            break;
//		    case ZERO_UNDETECT : //
//		        Motor.state.bit.zero = 0;
//		        SetTextValue(0,18,"未触底");
//		        break;
//	        case ZERO_DETECT : //
//	            Motor.state.bit.zero = 1;
//		        MOTOR_Stop();
//		        Motor.counter_step = 0;
//		        SetTextValue(0,18,"已触底");
//		    case SYRINGE_UNDETECT : //
////		        Motor.state.bit.zero = 0;
//		        //SetTextValue(0,18,"1.8ml");
//		        break;
//	        case SYRINGE_DETECT : //
////	            Motor.state.bit.zero = 1;
////		        MOTOR_Stop();
////		        Motor.counter_step = 0;
//		        //SetTextValue(0,18,"2.2ml");
//		        break;
//	             
		    default : break;
		}
        
		if( dg_sta == DG_UP ) //顶杆上升期间
		{
			if( timer_pedal_up > DLY_PEDAL_UP_MOTOR_STOP ) //抬起脚踏时间大于 DLY_PEDAL_UP_MOTOR_STOP 秒
			{
				timer_pedal_up = 0;
				Motor.speed = 0; //恢复速度
				   if( Tmy.Key.hx )//&& (!Motor.state.bit.dir) )//回吸模式有效 Motor.state.bit.dir
				   {
				        if(!Tmy.State.bit.zdgy) //非自动给药
    				    {
//    						if( ( Tmy.speed == SLOW )   //常慢速
//    						 || ( Tmy.speed == NORMAL ) //中速
//    						 || ( Tmy.speed == FAST ) ) //快速 
                            if( ( Tmy.speed == SLOWEST )   //慢慢速
			                 || ( Tmy.speed == SLOW    ) ) //常慢速
    						{
    							Motor.counter_hx_step = Motor.counter_step - Motor.max_hx_step;
    							//Tmy.state.bit.hx = 1;
    							dg_sta = DG_HX_DOWN;//Tmy.work_state = STA_HX_DOWN;
    							//MOTOR_SetDir( DOWN );
    							//MOTOR_SetSpeed( CYCLE_HX ); //设置电机速度
    							//SetTextValue(0,18,"回吸下降");
    						}
    				    }
				   }
			}
			
			//自动给药判断
			if( ( Tmy.speed == SLOWEST )   //慢慢速
			 || ( Tmy.speed == SLOW    ) ) //常慢速
			{
				if( ( !Tmy.State.bit.zdgy ) && ( timer_pedal_down > TIME_ZDGY_ENABLE ) ) //按下脚踏时间大于TIME_CRUISE_START秒
				{
					Tmy.State.bit.zdgy = 1; //进入自动给药
					SetTextValue(0,26,"自动给药...");
					//play_sound('a:/sounds/zdgy.mp3');
					PlayMusic("a:/sounds/zdgy.mp3"); //EE 94 41 3A 2F 53 6F 75 6E 64 73 2F 7A 64 67 79 2E 6D 70 33 FF FC FF FF 
					//播放语音:即将进入巡航模式
					//如果2秒内，抬脚，进入巡航模式
					//如果2秒内，没有抬脚，不进入巡航模式
				}
				
				if( ( Tmy.State.bit.zdgy ) && ( timer_pedal_down > TIME_ZDGY_CANCLE ) ) //按下脚踏时间大于5秒
				{
					Tmy.State.bit.zdgy = 0;//取消自动给药
					timer_pedal_down = 0;
					//播放语音:巡航模式取消？
					SetTextValue(0,26,"自动给药取消");
					//如果2秒内，抬脚，进入巡航模式
					//如果2秒内，没有抬脚，不进入巡航模式
				}
			}
		}

        switch( dg_sta )
        {
            case DG_INIT : //上电，参数初始化
                if( dg_sta_c != dg_sta )
                {
                    dg_sta_c = dg_sta;
                    //SetTextValue(0,26,"初始化...");
                }
                
                if( !Tmy.State.bit.InitPara ) //参数尚未初始化
                {
                    if( lg_lcd_init == 1 )
                    {
                        //lg_lcd_init = 2;
                        LCD_ParaInit();
                        Tmy.State.bit.InitPara = 1;
                    }
                    //Tmy.State.bit.InitPara = 1;
                }else //参数初始化完成
                {
                    SetTextValue(0,26,"LCD初始化完成。");
                    dg_sta = DG_INIT_DOWN;//顶杆归零
                }
                
                break;
            
            case DG_INIT_UP : //上电初始化过程，顶杆不在零位，先归零，然后再先上升一小段再归零
                
                if( dg_sta_c != dg_sta )
                {
                    dg_sta_c = dg_sta;
                    SetTextValue(0,26,"检测归零传感器,上升...");
                }
                
                if( !Tmy.Sensor.bit.bottom )//检测离开底部
                {
                    dg_sta = DG_INIT_DOWN;//再次复位
                }
                
                if( Tmy.Motor.state != MOTOR_UP ) //步进电机不是在上升
                {
                    MOTOR_Stop();//步进电机停止
                    MOTOR_SetDir( UP );//方向上升
                    MOTOR_SetSpeed( CYCLE_HX ); //设置电机速度 XXX
                    Tmy.Motor.state = MOTOR_UP;//更改步进电机状态
                }else //步进电机在上升
                {
//                    if( key_v == PEDAL_UP )//检测到脚踏抬起
//                    {
//                        dg_sta = DG_STOP;//停止
//                    }
                }
                
                break;
            case DG_INIT_DOWN : //上电初始化过程，顶杆不在零位，先归零，然后再先上升一小段再归零
                
                if( dg_sta_c != dg_sta )
                {
                    dg_sta_c = dg_sta;
                    SetTextValue(0,26,"顶杆归零...");
                }
                
                if( key_v == ZERO_DETECT )//检测到触底
                {
                    Tmy.Sensor.bit.bottom = 1;
    		        MOTOR_Stop();
    		        Motor.counter_step = 0;
    		        SetTextValue(0,18,"已触底");
                }
                
                if( Tmy.Sensor.bit.bottom )//到达底部
                {
                    if( !count_init_down ) //第一次触底
                    {
                        SetTextValue(0,26,"已触底");
                        count_init_down++;
                        dg_sta = DG_INIT_UP;//上升一段
                    }else //第二次触底
                    {
                        SetBuzzer(100);
                        SetTextValue(0,26,"顶杆归零完成");
                        Tmy.State.bit.init = 1; //初始化完成
                        dg_sta = DG_STOP_BOT;//停在底部
                        //dev_sta = DEV_STANDBY;//设备待机  
                    }
                    
                }else //未到达底部
                {
                    if( Tmy.Motor.state != MOTOR_DOWN ) //步进电机不是在下降
                    {
                        MOTOR_Stop();//步进电机停止
                        MOTOR_SetDir( DOWN );//方向下降
                        MOTOR_SetSpeed( CYCLE_HX ); //设置电机速度 XXX
                        Tmy.Motor.state = MOTOR_DOWN;//更改步进电机状态
                    }else //步进电机在下降
                    {
    //                    if( key_v == PEDAL_UP )//检测到脚踏抬起
    //                    {
    //                        dg_sta = DG_STOP;//停止
    //                    }
                    }
                }
                break;
            
            case DG_STOP_BOT : //停留在底部
                //触底步数归零
                MOTOR_ClrStep( );
                
                if( dg_sta_c != dg_sta )
                {
                    dg_sta_c = dg_sta;
                    SetTextValue(0,26,"触底待机");
                }
                
                if( Tmy.Motor.state != MOTOR_STOP ) //步进电机在运行
                {
                    MOTOR_Stop();//步进电机停止
                    MOTOR_SetDir( UP );//方向上升
                    Tmy.Motor.state = MOTOR_STOP;//更改步进电机状态
                    MOTOR_ClrStep( ); //步数清零
                }
                
                if( key_v == PEDAL_DOWN ) //检测到踩脚踏
                {
                    if( Tmy.Key.zdpk ) //自动排空
                    {
                       // SetTextValue(0,26,"自动排空...");
                        dg_sta = DG_ZDPK_UP;//顶杆上升
                    }else
                    //if( dev_sta == DEV_RUN )//设备运行状态
                    {
                        //SetTextValue(0,26,"顶杆上升...");
                        dg_sta = DG_UP;//顶杆上升
                    }
                }
//                if(  )//顶杆检查按键按下
//                {
//                    if( dev_sta == DEV_FIX )//顶杆检查
//                    {
//                        dg_sta = DG_FIX_UP;//顶杆检查上升
//                    }
//                }
                
//                if( key_v == PEDAL_DOWN )//检测到脚踏按下
//                {
//                    if( dev_sta == DEV_RUN )//设备运行状态
//                    {
//                        dg_sta = DG_UP;//顶杆上升
//                    }else
//                    if( dev_sta == DEV_FIX )//顶杆检查
//                    {
//                        dg_sta = DG_FIX_UP;//顶杆检查上升
//                    }
//                }

                break;
                    
            case DG_STOP : //停止
                
                if( dg_sta_c != dg_sta )
                {
                    dg_sta_c = dg_sta;
                    SetTextValue(0,26,"待机");
                }
                
                if( Tmy.Motor.state != MOTOR_STOP ) //步进电机在运行
                {
                    MOTOR_Stop();//步进电机停止
                    //MOTOR_SetDir( UP );//方向上升
                    Tmy.Motor.state = MOTOR_STOP;//更改步进电机状态
                    //MOTOR_ClrStep( ); //步数清零
                }
                
                if( key_v == PEDAL_DOWN )//检测到脚踏按下
                {
                    //if( dev_sta == DEV_RUN )//设备运行状态
                    {
                        timer_pedal_down = 1; //开始脚踏按下计时
                        dg_sta = DG_UP;//顶杆上升
                    }
                }//else
//                if( key_v == PEDAL_DOWN )//检测复位键按下
//                {
//                    if( )//设备运行状态
//                    {
//                        dg_sta = DG_RESET_DOWN;//复位下降
//                    }
//                    ;
//                }
                break;
                
            case DG_ZDPK_UP : //自动排空
                
                if( dg_sta_c != dg_sta )
                {
                    dg_sta_c = dg_sta;
                    SetTextValue(0,26,"自动排空...");
                }
                
                if( Motor.counter_step > STEP_MAX_ZDPK ) //到达自动排空步数
                {
                    dg_sta = DG_STOP;//电机停止
                }
                
                if( Tmy.Motor.state != MOTOR_UP ) //步进电机不是在上升
                {
                    MOTOR_Stop();//步进电机停止
                    MOTOR_SetDir( UP );//方向上升
                    MOTOR_SetSpeed( CYCLE_ZDPK ); //设置电机速
                    Tmy.Motor.state = MOTOR_UP;//更改步进电机状态
                }else //步进电机在上升
                {
                    ;
                }
                
                break;
            
            case DG_ECPK_UP : //二次排空
                
                if( Motor.counter_step > STEP_MAX_ECPK ) //到达自动排空步数
                {
                    dg_sta = DG_STOP;//电机停止
                }
                
                if( Tmy.Motor.state != MOTOR_UP ) //步进电机不是在上升
                {
                    MOTOR_Stop();//步进电机停止
                    MOTOR_SetDir( UP );//方向上升
                    MOTOR_SetSpeed( CYCLE_ZDPK ); //设置电机速度 XXX
                    Tmy.Motor.state = MOTOR_UP;//更改步进电机状态
                }else //步进电机在上升
                {
                    ;
                }
                
                break;
                
            case DG_UP : //顶杆上升
                
                if( dg_sta_c != dg_sta )
                {
                    dg_sta_c = dg_sta;
                    SetTextValue(0,26,"顶杆上升...");
                }
                
                if( ( Motor.counter_step >= Motor.max_step ) //到达顶部
                 || ( Tmy.Sensor.bit.top ) )
                {
                    MOTOR_Stop();//这个是冗余操作，多一次保护
                    dg_sta = DG_STOP_TOP;//停留在顶部
                }else
                {
                    if( Tmy.Motor.state != MOTOR_UP ) //步进电机未上升
                    {
                        Motor.speed = 1;
                        tem8 = MOTOR_SPEED_INDEX[Motor.gear][Motor.speed-1];
        	            Tmy.speed = tem8;
        	             
        	            MOTOR_SetSpeed( MOTOR_SPEED_CYCLE[tem8] ); //设置电机速度
        	            SetTextValue(0, 16, &MOTOR_SPEED_DISPLAY[tem8][0]); 
    	            
                        MOTOR_Stop(); //电机暂停
                        MOTOR_SetDir( UP ); //设置电机方向
                        MOTOR_SetSpeed( MOTOR_SPEED_CYCLE[tem8] ); //设置电机速度，运行
                        
                        //SetTextValue(0,18,"顶杆检查上升"); //更新提示
                    
                        Tmy.Motor.state = MOTOR_UP;//
                    }
                }
                
//                if( Tmy.Motor.state != MOTOR_UP ) //步进电机不是在上升
//                {
//                    MOTOR_Stop();//步进电机停止
//                    MOTOR_SetDir( UP );//方向上升
//                    MOTOR_SetSpeed( CYCLE_KSJ ); //设置电机速度 XXX
//                    Tmy.Motor.state = MOTOR_UP;//更改步进电机状态
//                }else //步进电机在上升
//                {
//                    if( key_v == PEDAL_UP )//检测到脚踏抬起
//                    {
//                        dg_sta = DG_STOP;//停止
//                    }
//                }
                
                break;
            
            case DG_STOP_TOP : //停留在顶部
                
                if( dg_sta_c != dg_sta )
                {
                    dg_sta_c = dg_sta;
                    SetTextValue(0,26,"到顶停机待机");
                }
                
                if( Tmy.Motor.state != MOTOR_STOP ) //步进电机在运行
                {
                    MOTOR_Stop();//电机暂停
                    MOTOR_SetDir( DOWN ); //方向下降
                    Tmy.Motor.state = MOTOR_STOP;//更改电机状态
                }
                
                if( key_v == PEDAL_DOWN ) //检测到踩脚踏 // || ( key_v == PEDAL_DOWN ) ) //或 按复位键
                {
                    //if( dev_sta == DEV_RUN )//设备运行状态
                    {
                        SetTextValue(0,26,"顶杆归零...");
                        dg_sta = DG_FIX_DOWN;//顶杆归零
                    }
                }
                
				if( 1 )//在xxx速度下，到顶后自动归零
				{
					;
				}
				
                break;
                
            case DG_DOWN : //顶杆下降 可能没有此状态！！！
                
                if( dg_sta_c != dg_sta )
                {
                    dg_sta_c = dg_sta;
                    SetTextValue(0,26,"顶杆下降...");
                }
                
                if(Tmy.Sensor.bit.bottom) //到达底部
                {
                    dg_sta = DG_STOP_TOP;//停留在底部
                }
                
                if( Tmy.Motor.state != MOTOR_DOWN ) //步进电机不是在下降
                {
                    MOTOR_Stop();//步进电机停止
                    MOTOR_SetDir( DOWN );//方向上升
                    MOTOR_SetSpeed( CYCLE_DGJC ); //设置电机速度 XXX
                    Tmy.Motor.state = MOTOR_DOWN;//更改步进电机状态
                }else //步进电机在下降
                {
                    if( key_v == PEDAL_UP )//检测到脚踏抬起
                    {
                        dg_sta = DG_STOP;//停止
                    }
                }
                break;
                
            case DG_HX_DOWN : //回吸下降
			
                if( dg_sta_c != dg_sta )
                {
                    dg_sta_c = dg_sta;
                    SetTextValue(0,26,"回吸下降...");
                }
                
                if( Motor.counter_step < Motor.counter_hx_step-1 )//到达回吸底部
                {
                    MOTOR_Stop();//电机停止
                    Motor.counter_hx_step = Motor.counter_step + Motor.max_hx_step;
                    
                    if( Tmy.speed == SLOW )   //常慢速
                    {
                        vTaskDelay(pdMS_TO_TICKS(1000));//延时1秒
                    }
                    dg_sta = DG_HX_UP;//
                }else
                {
                    if( Tmy.Motor.state != MOTOR_DOWN ) //步进电机未下降
                    {
                        MOTOR_Stop(); //电机暂停
                        MOTOR_SetDir( DOWN ); //设置电机方向
                        MOTOR_SetSpeed( CYCLE_HX ); //设置电机速度，运行
                        SetTextValue(0,18,"回吸下降"); //更新提示
                        Tmy.Motor.state = MOTOR_DOWN;//
                    }
                }
                
//	            if( Motor.counter_step > Motor.counter_hx_step ) //
//	            {
//	                ;//行进中
//	            }else //到达回吸底部
//	            {
//	                MOTOR_Stop();
//	                dg_sta = DG_HX_UP;//
//                    Motor.counter_hx_step = Motor.counter_step + Motor.max_hx_step;
//                    MOTOR_SetDir( UP );
//	                MOTOR_SetSpeed( CYCLE_HX ); //设置电机速度
//                    SetTextValue(0,18,"回吸上升");
//                    if( Tmy.speed == SLOW )   //常慢速
//                    {
//                        vTaskDelay(pdMS_TO_TICKS(1000));//延时1秒
//                    }
//	            }
	            
	            break;
	            
	        case DG_HX_UP : //回吸上升
	            
	            if( dg_sta_c != dg_sta )
                {
                    dg_sta_c = dg_sta;
                    SetTextValue(0,26,"回吸上升...");
                }
                
                if( Motor.counter_step > Motor.counter_hx_step-1 )//到达回吸顶部
                {
                    MOTOR_Stop(); //电机停止
                    //Tmy.state.bit.hx = 0;
                    dg_sta = DG_STOP; //
                }else
                {
                    if( Tmy.Motor.state != MOTOR_UP ) //步进电机未下降
                    {
                        MOTOR_Stop(); //电机暂停
                        MOTOR_SetDir( UP ); //设置电机方向
                        MOTOR_SetSpeed( CYCLE_HX ); //设置电机速度，运行
                        SetTextValue(0,18,"回吸上升"); //更新提示
                        Tmy.Motor.state = MOTOR_UP;//
                    }
                }
                
//	            if( Motor.counter_step < Motor.counter_hx_step ) 
//	            {
//	                ;//行进中
//	            }else //到达回吸顶部
//	            {
//	                MOTOR_Stop();
//	                Tmy.state.bit.hx = 0;
//	                Tmy.work_state = STA_STANDBY;
//	            }
	            
	            break;
	            
            case DG_FIX_UP : //顶杆检查上升
                
                if( ( Motor.counter_step >= Motor.max_step ) //到达顶部
                 || ( Tmy.Sensor.bit.top ) )
                {
                    MOTOR_Stop();//这个是冗余操作，多一次保护
                    dg_sta = DG_STOP_TOP;//停留在顶部
                }else
                {
                    if( Tmy.Motor.state != MOTOR_UP ) //步进电机未上升
                    {
                        MOTOR_Stop(); //电机暂停
                        MOTOR_SetDir( UP ); //设置电机方向
                        MOTOR_SetSpeed( CYCLE_DGJC ); //设置电机速度，运行
                        //SetTextValue(0,18,"顶杆检查上升"); //更新提示
                    
                        Tmy.Motor.state = MOTOR_UP;//
                    }
                }
                
                break;
                
            case DG_FIX_DOWN : //顶杆检查下降
                
                if( Tmy.Sensor.bit.bottom )//检测到底
                {
                    MOTOR_Stop();//这个是冗余操作，多一次保护
                    dg_sta = DG_STOP_BOT;//
                }else
                {
                    if( Tmy.Motor.state != MOTOR_DOWN ) //步进电机未下降
                    {
                        MOTOR_Stop(); //电机暂停
                        MOTOR_SetDir( DOWN ); //设置电机方向
                        MOTOR_SetSpeed( CYCLE_DGJC ); //设置电机速度，运行
                        //SetTextValue(0,18,"顶杆检查下降"); //更新提示
                    
                        Tmy.Motor.state = MOTOR_DOWN;//
                    }
                }
                
                break;
            
            default : 
                break;
        }
        
    }
}



		
//        switch( dev_sta )
//        {
//            case DEV_INIT :
//                //设置初始参数
//                //DEV_SetInitPara( );
//                if( !Tmy.State.bit.init ) //未初始
//                {
//                    dg_sta = DG_RESET_DOWN;
//                }else//初始化完成
//                {
//                    dev_sta = DEV_STANDBY;//设备待机
//                }
//                break;
//            case DEV_RUN : //运行
//                break;
//            case DEV_FIX :
//                break;
//            case DEV_STANDBY : //待机
//                if( Tmy.State.bit.init ) //初始化完成
//                {
//                    ;
//                }
//                break;
//            default : break;
//        }

/*
*********************************************************************************************************
*	函 数 名: vTaskStart
*	功能说明: 启动任务，也就是最高优先级任务，这里用作LED闪烁
*	形    参: pvParameters 是在创建该任务时传递的形参
*	返 回 值: 无
*   优 先 级: 4  
*********************************************************************************************************
*/
//static void vTaskStart(void *pvParameters)
//{
//    while(1)
//    {
//		/* LED闪烁 */
//		bsp_LedToggle(4);
//        vTaskDelay(400);
//    }
//}

/*
*********************************************************************************************************
*	函 数 名: AppTaskCreate
*	功能说明: 创建应用任务
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void AppTaskCreate (void)
{
//    xTaskCreate( vTaskTaskUserIF,   	/* 任务函数  */
//                 "vTaskUserIF",     	/* 任务名    */
//                 512,               	/* 任务栈大小，单位word，也就是4字节 */
//                 NULL,              	/* 任务参数  */
//                 1,                 	/* 任务优先级*/
//                 &xHandleTaskUserIF );  /* 任务句柄  */
//	
//	


	xTaskCreate( vTaskLCD,    		/* 任务函数  */
                 "vTaskLCD",  		/* 任务名    */
                 STKSIZE_TaskLCD,         		/* 任务栈大小，单位word，也就是4字节 */
                 NULL,        		/* 任务参数  */
                 PRIO_TaskLCD,      /* 任务优先级*/
                 &xHandleTaskLCD ); /* 任务句柄  */
	
	xTaskCreate( vTaskMotorCtrl,     		/* 任务函数  */
                 "vTaskMotorCtrl",   		/* 任务名    */
                 STKSIZE_TaskMotorCtrl,     /* 任务栈大小，单位word，也就是4字节 */
                 NULL,           		    /* 任务参数  */
                 PRIO_TaskMotorCtrl,        /* 任务优先级*/
                 &xHandleTaskMotorCtrl );  /* 任务句柄  */
	
    xTaskCreate( vTaskSensor,     		    /* 任务函数  */
                 "vTaskSensor",   		    /* 任务名    */
                 STKSIZE_TaskSensor,        /* 任务栈大小，单位word，也就是4字节 */
                 NULL,           		    /* 任务参数  */
                 PRIO_TaskSensor,           /* 任务优先级*/
                 &xHandleTaskSensor );      /* 任务句柄  */
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
		
		if( timer_pedal_up > 2 ) //抬起脚踏时间大于2秒
		{
		    timer_pedal_up = 0;
		    Motor.speed = 0; //恢复速度
            //if( Tmy.key.hx && Tmy.state.bit.dir )//回吸模式有效 Motor.state.bit.dir
       			if( Tmy.key.hx && (!Motor.state.bit.dir) )//回吸模式有效 Motor.state.bit.dir
            {
                if( ( Tmy.speed == SLOW )   //常慢速
                 || ( Tmy.speed == NORMAL ) //中速
                 || ( Tmy.speed == FAST ) ) //快速 
                 {
                    Tmy.state.bit.hx = 1;
                    Tmy.work_state = STA_HX_DOWN;
                    Motor.counter_hx_step = Motor.counter_step - Motor.max_hx_step;
                    MOTOR_SetDir( DOWN );
	                MOTOR_SetSpeed( CYCLE_HX ); //设置电机速度
                    SetTextValue(0,18,"回吸下降");
                 }
            }
		}
		
		switch( Tmy.work_state )
	    {
	        case STA_IDLE : //空闲
	            break;
	        case STA_MOVE : //电机运行
        		//if( Motor.state.bit.move )
        		{
        		
//        		    if( ( Motor.counter_step > Motor.max_step-1 ) //达到顶部
//            		 //|| ( Motor.counter_step == 0 )
//            		 || ( Motor.state.bit.zero ) ) //达到底部
            		 
            		if( Motor.state.bit.dir == UP )
            		{
            		    if( Motor.counter_step > Motor.max_step-1 ) //达到顶部
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
            		    if( Motor.state.bit.zero ) //达到底部
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
	        case STA_SLEEP : //休眠
	            break;
	        case STA_STANDBY : //待机
	            MOTOR_Stop();
	            MOTOR_SetDir( UP );
	            Tmy.work_state = STA_IDLE;
	            break;
	            
	        case STA_HX_DOWN : //回吸下行
	            if( Motor.counter_step > Motor.counter_hx_step ) //
	            {
	                ;//行进中
	            }else //到达回吸底部
	            {
	                MOTOR_Stop();
	                Tmy.work_state = STA_HX_UP;
                    Motor.counter_hx_step = Motor.counter_step + Motor.max_hx_step;
                    MOTOR_SetDir( UP );
	                MOTOR_SetSpeed( CYCLE_HX ); //设置电机速度
                    SetTextValue(0,18,"回吸上升");
                    if( Tmy.speed == SLOW )   //常慢速
                    {
                        vTaskDelay(pdMS_TO_TICKS(1000));//延时1秒
                    }
	            }
	            break;
	        case STA_HX_UP : //回吸上行
	            if( Motor.counter_step < Motor.counter_hx_step ) 
	            {
	                ;//行进中
	            }else //到达回吸顶部
	            {
	                MOTOR_Stop();
	                Tmy.state.bit.hx = 0;
	                Tmy.work_state = STA_STANDBY;
	            }
	            break;
	            
	        case STA_FIX : //顶杆检查
	            MOTOR_Stop();
                MOTOR_SetDir( UP );
                MOTOR_SetSpeed( CYCLE_KSJ ); //设置电机速度
                SetTextValue(0,18,"顶杆检查");
                Tmy.work_state = STA_MOVE;
	            break;
	        case STA_RESET : //复位
	            MOTOR_Stop();
                MOTOR_SetDir( DOWN );
                MOTOR_SetSpeed( CYCLE_KSJ ); //设置电机速度
                SetTextValue(0,18,"复位");
                Tmy.work_state = STA_MOVE;
	            break;
	        
	        case STA_WORK : //工作
	            break;
	           
	        default : break;
	    }
		switch( key_v ) //
		{
		    case PEDAL_DOWN : //按下脚踏，电机运行
		        
		        timer_pedal_up = 0;
		        
	            Motor.speed++;
	            if( Motor.speed > 3 ) 
	            {
	                Motor.speed = 1;
	            }
	            
	            tem8 = MOTOR_SPEED_INDEX[Motor.gear][Motor.speed-1];
	            Tmy.speed = tem8;
	             
	            MOTOR_SetSpeed( MOTOR_SPEED_CYCLE[tem8] ); //设置电机速度
	            SetTextValue(0, 16, &MOTOR_SPEED_DISPLAY[tem8][0]); 
	            //SetTextInt32(0, 17, MOTOR_SPEED_CYCLE[tem8], 0, 1);
	            Tmy.work_state = STA_MOVE;
		        break;
		    case PEDAL_UP : //抬起脚踏，电机停止
		        //Motor.speed = 0;
	            MOTOR_Stop();
	            timer_pedal_up = 1;//启动脚踏抬起计时
	            
	            SetTextValue(0, 16,"停止\x00");
	            SetTextInt32(0, 17, 0, 0, 1);
	            Tmy.work_state = STA_IDLE;
	            break;
		    case ZERO_UNDETECT : //
		        Motor.state.bit.zero = 0;
		        SetTextValue(0,18,"未触底");
		        break;
	        case ZERO_DETECT : //
	            Motor.state.bit.zero = 1;
		        MOTOR_Stop();
		        Motor.counter_step = 0;
		        SetTextValue(0,18,"已触底");
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
