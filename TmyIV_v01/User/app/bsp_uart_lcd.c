// =====================================================================================================================
// | File Name		: bsp_uart_lcd.c
// | Summary 	    : 
// | Version 	    : 1.00 
// | CPU	 	    : STM32103F
// | Compile 	    ：keil
// | Author  	    ：Robin
// | Start Date 	：2019-03-10
// | Finish Date 	：
// =====================================================================================================================

#define ULCD_GLOBAL


//#include "includes.h"
#include "bsp.h"
#include "bsp_motor.h"

#include "hmi_driver.h"
#include "hmi_user_uart.h"
#include "cmd_queue.h"
#include "cmd_process.h"
#include "stdio.h"
#include "hw_config.h"
#include "ulitity.h"
#include "string.h"



#define TIME_100MS 10                                                                //100毫秒(10个单位)

volatile uint32  timer_tick_count = 0;                                               //定时器节拍

uint8  cmd_buffer[CMD_MAX_SIZE];                                                     //指令缓存
static uint16 current_screen_id = 0;                                                 //当前画面ID
//static int32 progress_value = 0;                                                     //进度条测试值
//static int32 test_value = 0;                                                         //测试值
//static uint8 update_en = 0;                                                          //更新标记
//static int32 meter_flag = 0;                                                         //仪表指针往返标志位
//static int32 num = 0;                                                                //曲线采样点计数
//static int sec = 1;                                                                  //时间秒
//static int32 curves_type = 0;                                                        //曲线标志位  0为正弦波，1为锯齿波                  
//static int32 icon_flag = 0;                                                          //图标标志位
//static int years,months,weeks,days,hours,minutes;                                    //年月日时分秒星期
//static uint8 Select_H ;                                                              //滑动选择小时
//static uint8 Select_M ;                                                              //滑动选择分钟 
//static uint8 Last_H ;                                                                //上一个选择小时
//static uint8 Last_M;                                                                 //上一个选择分钟 
//static int32 second_flag=0;                                                          //时间标志位 
static uint8 menu_visable=1; //菜单按键显示状态 

void UpdateUI(void);                                                                 //更新UI数据

uint16_t LCD_GetScreenId( void )
{
    return current_screen_id;
}

void ULCD_InitIO(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

	/* 打开GPIO时钟 */
	RCC_APB2PeriphClockCmd(RCC_ULCD, ENABLE);
	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //推挽输出模式
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_ULCD_POWEN;
	GPIO_Init(GPIO_PORT_ULCD, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_ULCD_BUSY;
	GPIO_Init(GPIO_PORT_ULCD, &GPIO_InitStructure);
    
    ULCD_POW_ON;
}

/*!                                                                                 
*  \brief  程序入口                                                                 
*/                                                                                  
//int task_main()                                                                          
//{                                                                                   
//    static int32 test_value = 0;                                                     //测试值
//    uint32 timer_tick_last_update = 0;                                               //上一次更新的时间
//    qsize  size = 0;                                                                

////    //配置时钟                                                                    
////    Set_System();                                                                   
////    //配置串口中断                                                                
////    Interrupts_Config();                                                            
////    //配置时钟节拍                                                                
////    systicket_init();                                                               
//    //串口初始化                                              
//    UartInit(115200);//UartInit(19200);                                                               

//    //清空串口接收缓冲区                                                          
//    queue_reset();                                                                  

//    //延时等待串口屏初始化完毕,必须等待300ms                                      
//    delay_ms(300);                                                                  

//    while(1)                                                                        
//    {                                                                               
//        size = queue_find_cmd(cmd_buffer,CMD_MAX_SIZE);                               //从缓冲区中获取一条指令         

//        if(size>0)                                                              
//        {                                                                           
//            ProcessMessage((PCTRL_MSG)cmd_buffer, size);                             //指令处理  
//        } 
//        //    特别注意
//        //    MCU不要频繁向串口屏发送数据，否则串口屏的内部缓存区会满，从而导致数据丢失(缓冲区大小：标准型8K，基本型4.7K)
//        //    1) 一般情况下，控制MCU向串口屏发送数据的周期大于100ms，就可以避免数据丢失的问题；
//        //    2) 如果仍然有数据丢失的问题，请判断串口屏的BUSY引脚，为高时不能发送数据给串口屏。

//        //    TODO: 添加用户代码
//        //    数据有更新时标志位发生改变，定时100毫秒刷新屏幕
//        if(update_en&&timer_tick_count-timer_tick_last_update>=TIME_100MS)
//        {
//            update_en = 0;
//            timer_tick_last_update = timer_tick_count;   

//            UpdateUI();
//        }
//  
//        //这里为了演示方便，每20毫秒自动更新一次
//        //进一次中断10ms，timer_tick_count值+1,100*timer_tick_count = 1s
//        if(timer_tick_count%2==0)
//        {   
//            UpdateUI(); 
//        }
//    }  
//}


void ULCD_Init(void)
{
    ULCD_InitIO();
   // UartInit(115200);//UartInit(19200);                                                               

    //清空串口接收缓冲区                                                          
    //queue_reset();                                                                  
    lcd_prosser = 0; 
    timer_beep = 0;
    lg_lcd_init = 0;
    dg_sta_befor = 0;
    //延时等待串口屏初始化完毕,必须等待300ms                                      
    vTaskDelay(pdMS_TO_TICKS(300));//bsp_DelayMS(300);
	
}

void ULCD_Task( void )                                                                          
{       
//    uint8_t i; 
//    uint16_t n = 0;   
//    static int32 test_value = 0;                                                     //测试值
//    uint32 timer_tick_last_update = 0;                                               //上一次更新的时间
    qsize  size = 0;                                                                

    //while(1)                                                                        
    {                                                                               
        size = queue_find_cmd(cmd_buffer,CMD_MAX_SIZE);                               //从缓冲区中获取一条指令         

        if(size>0)                                                              
        {   
//            for( i=0; i<size; i++ )
//            {
//                printf("%02X ",cmd_buffer[i]);
//            }
            //comSendBuf(COM1, cmd_buffer, size );                                                                       
            ProcessMessage((PCTRL_MSG)cmd_buffer, size);                             //指令处理  
        } 
        //    特别注意
        //    MCU不要频繁向串口屏发送数据，否则串口屏的内部缓存区会满，从而导致数据丢失(缓冲区大小：标准型8K，基本型4.7K)
        //    1) 一般情况下，控制MCU向串口屏发送数据的周期大于100ms，就可以避免数据丢失的问题；
        //    2) 如果仍然有数据丢失的问题，请判断串口屏的BUSY引脚，为高时不能发送数据给串口屏。

        //    TODO: 添加用户代码
        //    数据有更新时标志位发生改变，定时100毫秒刷新屏幕
//        if(update_en&&timer_tick_count-timer_tick_last_update>=TIME_100MS)
//        {
//            update_en = 0;
//            timer_tick_last_update = timer_tick_count;   
//
//            UpdateUI();
//        }
  

/*
EE B1 10 00 2B 00 0C 00 00 00 32 FF FC FF FF 
EE B1 10 00 2B 00 0C 00 00 00 01 FF FC FF FF 
EE B1 10 00 2B 00 10 00 00 00 01 FF FC FF FF 
*/
        //这里为了演示方便，每20毫秒自动更新一次
        //进一次中断10ms，timer_tick_count值+1,100*timer_tick_count = 1s
        if( !timer_lcd_update )//if(timer_tick_count%2==0)
        {
            timer_lcd_update = 100; //1s
            
             UpdateUI();
             
//             if( i>6 ) i = 0;
//             AnimationPlayFrame(0,4,i++);//显示wifi图标
             
//            //if( Motor.state.bit.move )
//            {
//                lcd_prosser = 100*Motor.counter_step/Motor.max_step;
//                SetProgressValue(0,12,lcd_prosser);
//                SetTextInt32(0, 19, Motor.counter_step, 0, 1);
//            }
//            SetProgressValue(0,15,Tmy.power_percent);
//            SetTextInt32(0, 27, Tmy.pressure, 0, 1); //测试显示压力
        }
        
        if( Motor.state.bit.move )
        {
            if( !timer_beep )
            {
//                if( dg_sta == DG_DGJC ) //顶杆检查，无声
//                {
//                    
//                }else
                if( dg_sta == DG_UP ) //工作，上升
                {
                    timer_beep = BEEP_TIMER[Motor.speed-1];
                    SetBuzzer( 10 ); //蜂鸣器
                }else
                if( ( dg_sta == DG_HX_UP ) || ( dg_sta == DG_HX_DOWN ) ) //回吸
                {
                    timer_beep = BEEP_TIMER[HX];
                    SetBuzzer( 10 ); //蜂鸣器
                }else
                if( dg_sta == DG_ZDPK_UP ) //自动排空
                {
                    timer_beep = BEEP_TIMER[ZDPK];
                    SetBuzzer( 10 ); //蜂鸣器
                }
//                if( Motor.speed ) //
//                {
//                    timer_beep = BEEP_TIMER[Motor.speed-1];
//                    SetBuzzer( 10 ); //蜂鸣器
//                }
            }
        }
    }  
}

/*! 
*  \brief  消息处理流程
*  \param msg 待处理消息
*  \param size 消息长度
*/
void ProcessMessage( PCTRL_MSG msg, uint16 size )
{
    uint8 cmd_type = msg->cmd_type;                                                  //指令类型
    uint8 ctrl_msg = msg->ctrl_msg;                                                  //消息的类型
    uint8 control_type = msg->control_type;                                          //控件类型
    uint16 screen_id = PTR2U16(&msg->screen_id);                                     //画面ID
    uint16 control_id = PTR2U16(&msg->control_id);                                   //控件ID
    uint32 value = PTR2U32(msg->param);                                              //数值


    switch(cmd_type)
    {  
    case NOTIFY_TOUCH_PRESS:                                                        //触摸屏按下
    case NOTIFY_TOUCH_RELEASE:                                                      //触摸屏松开
        NotifyTouchXY(cmd_buffer[1],PTR2U16(cmd_buffer+2),PTR2U16(cmd_buffer+4)); 
        break;                                                                    
    case NOTIFY_WRITE_FLASH_OK:                                                     //写FLASH成功
        NotifyWriteFlash(1);                                                      
        break;                                                                    
    case NOTIFY_WRITE_FLASH_FAILD:                                                  //写FLASH失败
        NotifyWriteFlash(0);                                                      
        break;                                                                    
    case NOTIFY_READ_FLASH_OK:                                                      //读取FLASH成功
        NotifyReadFlash(1,cmd_buffer+2,size-6);                                     //去除帧头帧尾
        break;                                                                    
    case NOTIFY_READ_FLASH_FAILD:                                                   //读取FLASH失败
        NotifyReadFlash(0,0,0);                                                   
        break;                                                                    
    case NOTIFY_READ_RTC:                                                           //读取RTC时间
        NotifyReadRTC(cmd_buffer[2],cmd_buffer[3],cmd_buffer[4],cmd_buffer[5],cmd_buffer[6],cmd_buffer[7],cmd_buffer[8]);
        break;
    case NOTIFY_CONTROL:
        {
            if(ctrl_msg==MSG_GET_CURRENT_SCREEN)                                    //画面ID变化通知
            {
                NotifyScreen(screen_id);                                            //画面切换调动的函数
            }
            else
            {
                switch(control_type)
                {
                case kCtrlButton:                                                   //按钮控件
                    NotifyButton(screen_id,control_id,msg->param[1]);                  
                    break;                                                             
                case kCtrlText:                                                     //文本控件
                    NotifyText(screen_id,control_id,msg->param);                       
                    break;                                                             
                case kCtrlProgress:                                                 //进度条控件
                    NotifyProgress(screen_id,control_id,value);                        
                    break;                                                             
                case kCtrlSlider:                                                   //滑动条控件
                    NotifySlider(screen_id,control_id,value);                          
                    break;                                                             
                case kCtrlMeter:                                                    //仪表控件
                    NotifyMeter(screen_id,control_id,value);                           
                    break;                                                             
                case kCtrlMenu:                                                     //菜单控件
                    NotifyMenu(screen_id,control_id,msg->param[0],msg->param[1]);      
                    break;                                                              
                case kCtrlSelector:                                                 //选择控件
                    NotifySelector(screen_id,control_id,msg->param[0]);                
                    break;                                                              
                case kCtrlRTC:                                                      //倒计时控件
                    NotifyTimer(screen_id,control_id);
                    break;
                default:
                    break;
                }
            } 
            break;  
        } 
    case NOTIFY_HandShake:                                                          //握手通知                                                     
        NOTIFYHandShake();
        break;
    default:
        break;
    }
}

/*! 
*  \brief  握手通知
*/
void NOTIFYHandShake()
{
   SetButtonValue(3,2,1);
}

void LCD_ParaInit( void )
{
    //方向
    if( Motor.state.bit.dir == UP )
    {
        SetTextValue(0,18,"上升");
    }else
    {
        SetTextValue(0,18,"下降");
    }
    
    //速度
    SetTextValue(0, 16,"停止\x00");
    
    //频率
    SetTextValue(0, 17,"0");
    
    //最大步数
    if( Motor.max_step == MAX_STEP_18ML )                                                            //正弦波控件
    {
        SetTextInt32(0, 20, Motor.max_step, 0, 1); //显示最大步数
    }else if( Motor.max_step == MAX_STEP_22ML )                                                            //正弦波控件
    {
        SetTextInt32(0, 20, Motor.max_step, 0, 1); //显示最大步数
    }
    
    //步数
    SetTextInt32(0, 19, Motor.counter_step, 0, 1);
    
    //默认参数
    Tmy.Key.zdpk = 1;
    Tmy.Key.hx = 1;
    Tmy.Key.zh = 0;
    Motor.gear = GEAR_SLOW_NORMAL_FAST; //慢中快速
    SetButtonValue(0, 1, 1);//自动排空，默认有效
    SetButtonValue(0, 3, 1);//回吸，默认有效
    SetButtonValue(0, 21, 1);//1.8ml，默认有效
    SetButtonValue(0, 5, 0);//
    SetButtonValue(0, 6, 0);//
    SetButtonValue(0, 7, 1);//慢中快速，默认有效
    SetButtonValue(0, 8, 0);//
    
    AnimationPlayFrame(0,32,0);//显示wifi图标

    // SetControlVisiable(0, 10, 0);
    // SetControlVisiable(0, 11, 0);
    // SetControlVisiable(0, 33, 0);
    // SetControlVisiable(0, 34, 0);
}


/*! 
*  \brief  画面切换通知
*  \details  当前画面改变时(或调用GetScreen)，执行此函数
*  \param screen_id 当前画面ID
*/
void NotifyScreen(uint16 screen_id)
{
    uint8_t i;

    //TODO: 添加用户代码
    current_screen_id = screen_id;                                                   //在工程配置中开启画面切换通知，记录当前画面ID

    //工作页面
    if(screen_id == 0)
    {
        //SetBuzzer(100);               
        lg_lcd_init = 1;
//        //if( lg_lcd_init < 2 )
//        {
//            SetBuzzer(100);
//            lg_lcd_init = 2;
//            LCD_ParaInit();
//        }
    }
    //设置参数
   if(screen_id == 1)
   {
        delay_ms(100); 
        //压力值校准 0 3 5 7 9 11 11.5 Kg
        for ( i=0; i<7; i++) //0~11.5 Kg
        {
            SetTextInt32(1, 11+i, data_saved.data.Pressure[i], 0, 1); //显示压力值
            delay_ms(100);                                              
        }
   }
}

/*! 
*  \brief  触摸坐标事件响应
*  \param press 1按下触摸屏，3松开触摸屏
*  \param x x坐标
*  \param y y坐标
*/
void NotifyTouchXY(uint8 press,uint16 x,uint16 y)
{ 
    //TODO: 添加用户代码
}


/*! 
*  \brief  更新数据
*/ 
void UpdateUI()
{
    //EE B1 26 00 00 00 0D 00 00 FF FC FF FF 
    if(current_screen_id == 0)//主页面一
    {
        if (ble_state_tem.bit.link != Ble.state.bit.link)
        {
            ble_state_tem.bit.link = Ble.state.bit.link;
            if (ble_state_tem.bit.link) //更新BLE联网图标状态
            {
                AnimationPlayFrame(0,32,0);//联网
            }else
            {
                AnimationPlayFrame(0,32,1);//掉线
            }
        }
        
        //if( Motor.state.bit.move )
        {
            lcd_prosser = 100*Motor.counter_step/Motor.max_step;
            SetProgressValue(0,12,lcd_prosser);
            SetTextInt32(0, 19, Motor.counter_step, 0, 1);
        }
        SetProgressValue(0,15,Tmy.power_percent);
        SetTextInt32(0, 27, Tmy.pressure, 0, 1); //测试显示压力 
        //SetTextInt32(0, 26, Tmy.pressureLevel, 0, 1); //测试显示压力 
//        SetTextInt32(0, 29, PressureTable[ 0 ], 0, 1); //
//        SetTextInt32(0, 24, PressureTable[ 1 ], 0, 1); //
        
        AnimationPlayFrame(0, 4, Tmy.pressureLevel);//显示压力图标

        if ((dg_sta != DG_STOP_BOT) && (menu_visable)) 
        {
            menu_visable = 0;

            //显示控件
            SetControlVisiable(0, 13, 1);

            //隐藏控件
            //SetControlVisiable(0, 10, 0);
            //SetControlVisiable(0, 11, 0);
            SetControlVisiable(0, 33, 0);
            SetControlVisiable(0, 34, 0);
            SetTextValue(0, 10, "  ");
		    SetTextValue(0, 11, "  ");
            //SetTextValue(0, 11, "\xCD\xA3\xD6\xB9\x00");
            //SetTextValue(0, 11, "复位");

            //禁止控件
            SetControlEnable(0, 5, 0);
            SetControlEnable(0, 6, 0);
            SetControlEnable(0, 7, 0);
            SetControlEnable(0, 8, 0);
            SetControlEnable(0, 21, 0);
            SetControlEnable(0, 22, 0);

        }else
        if ((dg_sta == DG_STOP_BOT) && (!menu_visable)) 
        {
            menu_visable = 1;

            //隐藏控件
            SetControlVisiable(0, 13, 0);

            //显示控件
            // SetControlVisiable(0, 10, 1);
            // SetControlVisiable(0, 11, 1);
            SetControlVisiable(0, 33, 1);
            SetControlVisiable(0, 34, 1);
            SetTextValue(0, 10, "配置1");
            SetTextValue(0, 11, "配置2");

            //使能控件
            SetControlEnable(0, 5, 1);
            SetControlEnable(0, 6, 1);
            SetControlEnable(0, 7, 1);
            SetControlEnable(0, 8, 1);
            SetControlEnable(0, 21, 1);
            SetControlEnable(0, 22, 1);
        }
    }
    
    if(current_screen_id == 1)//主页面二
    {
        SetTextInt32(1, 11, Tmy.pressure, 0, 1); //显示压力
    }
    
    //AnimationPlayFrame(0,4,0);//显示wifi图标
    if(current_screen_id == 2)//主页面三
    {
        //if( Motor.state.bit.move )
        {
            lcd_prosser = 100*Motor.counter_step/Motor.max_step;
            SetProgressValue(2,12,lcd_prosser);
            //SetTextInt32(2, 19, Motor.counter_step, 0, 1);
        }
    }

}


/*! 
*  \brief  按钮控件通知
*  \details  当按钮状态改变(或调用GetControlValue)时，执行此函数
*  \param screen_id 画面ID
*  \param control_id 控件ID
*  \param state 按钮状态：0弹起，1按下
*/
void NotifyButton(uint16 screen_id, uint16 control_id, uint8  state)
{ 
    //SetBuzzer( 10 ); //蜂鸣器
    
    
    //主页面一
    if(screen_id == 0)
    {
        if(control_id==1) //自动排空
        {
            if( state ) //按下
            {
                //AnimationPlayFrame(0,4,2);//显示图标
                Tmy.Key.zdpk = 1;
            }else
            {
                //AnimationPlayFrame(0,4,0);//显示图标
                Tmy.Key.zdpk = 0;
            }                                                        
        }else
        if(control_id==2) //二次排空 
        {
            if( state ) //按下
            {
                Tmy.Key.ecpk = 1;
            }else
            {
                Tmy.Key.ecpk = 0;
            }                                                        
        }else
        if(control_id==3) //回吸
        {
            if( state ) //按下
            {
                Tmy.Key.hx = 1;
            }else
            {
                Tmy.Key.hx = 0;
            }                                                        
        }else
        if(control_id==4) //方向
        {
            if( state ) //按下
            {
                MOTOR_SetDir( UP );//
                //SetTextValue(0,18,"上升");
            }else
            {
                MOTOR_SetDir( DOWN );
                //SetTextValue(0,18,"下降");
            }                                                        
        }else
        if(control_id==6)
        {
            Motor.gear = GEAR_SLOWEST; //慢慢速          
        }
        else if(control_id==5)
        {
            Motor.gear = GEAR_SLOW; //常慢速
        }else if(control_id==7)
        {
            Motor.gear = GEAR_SLOW_NORMAL_FAST; //慢中快速
        }  else if(control_id==8) 
        {
            Motor.gear = GEAR_SLOW_FAST_FASTEST; //慢快最快速
        }else if(control_id==21) //注射器容量1.8ml
        {
            Motor.max_step = MAX_STEP_18ML;
            SetTextInt32(0, 20, Motor.max_step, 0, 1); //显示最大步数                                                      
        }else if(control_id==22) //注射器容量2.2ml
        {
            Motor.max_step = MAX_STEP_22ML;
            SetTextInt32(0, 20, Motor.max_step, 0, 1); //显示最大步数                                                      
        }else if(control_id==28) //培训
        {
            if( state ) //按下
            {
                Tmy.Key.px = 1;

                
                //显示控件
                SetControlVisiable(0, 16, 1);
                SetControlVisiable(0, 17, 1);
                SetControlVisiable(0, 18, 1);
                SetControlVisiable(0, 19, 1);
                SetControlVisiable(0, 20, 1);

                SetControlVisiable(0, 25, 1);
                SetControlVisiable(0, 26, 1);
                SetControlVisiable(0, 29, 1);
            }else
            {
                Tmy.Key.px = 0;

                //隐藏控件
                SetControlVisiable(0, 16, 0);
                SetControlVisiable(0, 17, 0);
                SetControlVisiable(0, 18, 0);
                SetControlVisiable(0, 19, 0);
                SetControlVisiable(0, 20, 0);

                SetControlVisiable(0, 25, 0);
                SetControlVisiable(0, 26, 0);
                SetControlVisiable(0, 29, 0);
            }                                                      
        }
        // else if(control_id==9) //顶杆检查
        // {
        //     if(1)//不在工作状态
        //     {
        //         if( dg_sta == DG_STOP_BOT ) //顶杆在底部
        //         {    
        //             PlayMusic("a:/sounds/dgjc.mp3");
        //             SetTextValue(0,26,"检修上升...");
        //             dg_sta = DG_FIX_UP;//检修上升
        //             dg_sta_befor = DG_FIX_UP;
                
        //         }else
        //         if( dg_sta == DG_STOP_TOP ) //顶杆在底部
        //         {
        //             PlayMusic("a:/sounds/dggl.mp3");
        //             SetTextValue(0,26,"顶杆归零...");
        //             dg_sta = DG_FIX_DOWN;//顶杆归零
        //             dg_sta_befor = DG_FIX_DOWN;
        //         }else    
        //         {
        //             if( dg_sta == DG_STOP ) //运行中停止，停止后运行
        //             {
        //                 dg_sta = dg_sta_befor;//
        //                 if( dg_sta == DG_FIX_UP ) //上升
        //                 {
        //                     PlayMusic("a:/sounds/dgjc.mp3");
        //                     SetTextValue(0,26,"检修上升...");
        //                 }else
        //                 if( dg_sta == DG_FIX_DOWN ) //下降
        //                 {
        //                     PlayMusic("a:/sounds/dggl.mp3");
        //                     SetTextValue(0,26,"顶杆归零...");
        //                 }else
        //                 {
        //                     dg_sta = DG_STOP;
		// 												SetTextValue(0,26,"故障，顶杆停止");
        //                 }
                        
        //             }
        //             else
        //             {
        //                 PlayMusic("a:/sounds/tz.mp3");
        //                 dg_sta = DG_STOP;//顶杆停止
        //                 SetTextValue(0,26,"顶杆停止");
        //             }
        //         }
        //     }
        // }
    }
    
    //参数设置
    if(screen_id == 1)
    {
        //压力值校准 0 3 5 7 9 11 11.5 Kg
        if( (control_id > 0 ) && (control_id < 8 ) ) //0~11.5 Kg
        {
            if( state ) //按下
            {
                SetTextInt32(1, 11+control_id, Tmy.pressure, 0, 1); //显示压力值
                PRE_Adjust(control_id-1, Tmy.pressure); //存储压力值
            }else
            {
               SetTextInt32(1, 11+control_id, 0, 0, 1); //显示压力值
                PRE_Adjust(control_id-1, 0); //存储压力值
            }                                                        
        } 
        // else if(control_id==8) //顶杆检查
        // {
	    //     if( state ) //按下 //顶杆检查
        //     {
        //         Tmy.Key.fix = 1;
        //         //Tmy.work_state = STA_FIX;
                
        //         if( dg_sta == DG_STOP_BOT ) //顶杆在底部
        //         {
        //             SetTextValue(0,26,"检修上升...");
        //             dg_sta = DG_FIX_UP;//检修上升
        //         }else
        //         {
        //             SetTextValue(0,26,"顶杆未在底部，操作错误");
        //         }
        //     }
        // }
        // else if(control_id==9) //顶杆停止
        // {
	    //     if( state ) //按下 //顶杆停止
        //     {
        //         dg_sta = DG_STOP;//顶杆停止
        //         SetTextValue(0,26,"顶杆停止");
        //     }
        // }
        // else if(control_id==10) //顶杆归零
        // {
	    //     if( state ) //按下 //顶杆归零
        //     {
        //         if( dg_sta == DG_STOP_TOP ) //顶杆在顶部
        //         {
        //             SetTextValue(0,26,"顶杆归零...");
        //             dg_sta = DG_FIX_DOWN;//顶杆归零
        //         }else
        //         {
        //             SetTextValue(0,26,"顶杆未在底部，操作错误");
        //         }
        //     }
        // }
    }

    //页面三 参数设置2
    if(screen_id == 2)
    {
        if(control_id==9) //顶杆检查
        {
	        //if( state ) //按下 //顶杆上升
            {
                Tmy.Key.fix = 1;
                
                if ((dg_sta == DG_STOP_BOT) //顶杆在底部
                    || (dg_sta == DG_STOP))
                {
                    SetTextValue(2,26,"顶杆上升...");
                    dg_sta = DG_FIX_UP;//顶杆上升
                }else
                {
                    SetTextValue(0,26,"顶杆未在底部，操作错误");
                }
            }
        }
        else if(control_id==10) //顶杆停止
        {
	        //if( state ) //按下 //顶杆停止
            {
                dg_sta = DG_STOP;//顶杆停止
                SetTextValue(2,26,"顶杆停止");
            }
        }
        else if(control_id==11) //顶杆归零
        {
	        //if( state ) //按下 //顶杆归零
            {
                if ((dg_sta == DG_STOP_TOP) //顶杆在顶部
                    || (dg_sta == DG_STOP))
                {
                    SetTextValue(2,26,"顶杆归零...");
                    dg_sta = DG_FIX_DOWN;//顶杆归零
                }else
                {
                    SetTextValue(2,26,"顶杆未在底部，操作错误");
                }
            }
        }
    }
}

/*! 
*  \brief  文本控件通知
*  \details  当文本通过键盘更新(或调用GetControlValue)时，执行此函数
*  \details  文本控件的内容以字符串形式下发到MCU，如果文本控件内容是浮点值，
*  \details  则需要在此函数中将下发字符串重新转回浮点值。
*  \param screen_id 画面ID
*  \param control_id 控件ID
*  \param str 文本控件内容
*/
void NotifyText(uint16 screen_id, uint16 control_id, uint8 *str)
{
    if(screen_id==4)                                                                //画面ID2：文本设置和显示
    {                                                                            
        int32 value=0;                                                            
        sscanf((char*)str,"%ld",&value);//sscanf(str,"%ld",&value);                 //把字符串转换为整数 
        if(control_id==2)                                                           //最高电压
        {                                                                         
            //限定数值范围（也可以在文本控件属性中设置）                             
            if(value<0)                                                              
            {                                                                        
                value = 0;                                                            
            }                                                                        
            else if(value>380)                                                       
            {                                                                        
                value = 380;                                                           
            }                                                                        
            SetTextInt32(4,2,value,0,1);                                             //更新最高电压
            SetTextInt32(4,5,value/2,1,1);                                           //更新最高电压/2
        }                                                                         
    }                                                                            
}                                                                                

/*!                                                                              
*  \brief  进度条控件通知                                                       
*  \details  调用GetControlValue时，执行此函数                                  
*  \param screen_id 画面ID                                                      
*  \param control_id 控件ID                                                     
*  \param value 值                                                              
*/                                                                              
void NotifyProgress(uint16 screen_id, uint16 control_id, uint32 value)           
{  
    //TODO: 添加用户代码   
}                                                                                

/*!                                                                              
*  \brief  滑动条控件通知                                                       
*  \details  当滑动条改变(或调用GetControlValue)时，执行此函数                  
*  \param screen_id 画面ID                                                      
*  \param control_id 控件ID                                                     
*  \param value 值                                                              
*/                                                                              
void NotifySlider(uint16 screen_id, uint16 control_id, uint32 value)             
{                                                             
    if(screen_id==7&&control_id==5)                                                  //滑块控制
    {                                                                              
        if(value<100||value>0)                                                       
        {                                                                            
            SetProgressValue(7,4,value);                                             //更新进度条数值
            SetTextInt32(7,6,value,0,1);                                             
        }
    }
}

/*! 
*  \brief  仪表控件通知
*  \details  调用GetControlValue时，执行此函数
*  \param screen_id 画面ID
*  \param control_id 控件ID
*  \param value 值
*/
void NotifyMeter(uint16 screen_id, uint16 control_id, uint32 value)
{
    //TODO: 添加用户代码
}

/*! 
*  \brief  菜单控件通知
*  \details  当菜单项按下或松开时，执行此函数
*  \param screen_id 画面ID
*  \param control_id 控件ID
*  \param item 菜单项索引
*  \param state 按钮状态：0松开，1按下
*/
void NotifyMenu(uint16 screen_id, uint16 control_id, uint8 item, uint8 state)
{
    //TODO: 添加用户代码
}

/*! 
*  \brief  选择控件通知
*  \details  当选择控件变化时，执行此函数
*  \param screen_id 画面ID
*  \param control_id 控件ID
*  \param item 当前选项
*/
void NotifySelector(uint16 screen_id, uint16 control_id, uint8  item)
{
	;
//    if(screen_id == 13&&control_id == 1)                                               //获取当前选择控件的值
//    {
//        Select_H =  item;
//    } 
//    if(screen_id == 13&&control_id == 2)                                               //获取当前选择控件的值
//    {
//        Select_M =  item;
//    } 

}

/*! 
*  \brief  定时器超时通知处理
*  \param screen_id 画面ID
*  \param control_id 控件ID
*/
void NotifyTimer(uint16 screen_id, uint16 control_id)
{
    // if(screen_id==8&&control_id == 7)
    // {
    //     SetBuzzer(100);
    // } 
}

/*! 
*  \brief  读取用户FLASH状态返回
*  \param status 0失败，1成功
*  \param _data 返回数据
*  \param length 数据长度
*/
void NotifyReadFlash(uint8 status,uint8 *_data,uint16 length)
{
    //TODO: 添加用户代码
}

/*! 
*  \brief  写用户FLASH状态返回
*  \param status 0失败，1成功
*/
void NotifyWriteFlash(uint8 status)
{
    //TODO: 添加用户代码
}

/*! 
*  \brief  读取RTC时间，注意返回的是BCD码
*  \param year 年（BCD）
*  \param month 月（BCD）
*  \param week 星期（BCD）
*  \param day 日（BCD）
*  \param hour 时（BCD）
*  \param minute 分（BCD）
*  \param second 秒（BCD）
*/
void NotifyReadRTC(uint8 year,uint8 month,uint8 week,uint8 day,uint8 hour,uint8 minute,uint8 second)
{ 
;	
//    sec    =(0xff & (second>>4))*10 +(0xf & second);                                    //BCD码转十进制
//    years   =(0xff & (year>>4))*10 +(0xf & year);                                      
//    months  =(0xff & (month>>4))*10 +(0xf & month);                                     
//    weeks   =(0xff & (week>>4))*10 +(0xf & week);                                      
//    days    =(0xff & (day>>4))*10 +(0xf & day);                                      
//    hours   =(0xff & (hour>>4))*10 +(0xf & hour);                                       
//    minutes =(0xff & (minute>>4))*10 +(0xf & minute);                                                                                                                  
}
