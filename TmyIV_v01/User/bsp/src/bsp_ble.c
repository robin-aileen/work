
/*
*********************************************************************************************************
*
*	模块名称 : BLE模块驱动程序
*	文件名称 : BLE.c
*	版    本 : V0.1
*   M  C  U  : STM32F103RBT6
*   编 译 器 : IAR-Arm 8.22.2
*
*	说    明 : 模块软件版本RD1006@180511-CAUT03
*
*	修改记录 :
*		版本号  日期        作者     说明
*		V0.1    2018-05-15 robin    测试版
*       V0.1    2018-09-12 robin    增加了 xSemUsrSend 互斥保护发送缓存
*	Copyright (C), 2018-2028
*
*********************************************************************************************************
*/

#define BLE_GLOBAL

#include "includes.h"//#include "bsp.h"

#include <stdlib.h>

static uint8_t ble_cmd(char *cmd, char *ack, uint16_t t);
static uint8_t ble_wait_ack( char *ack, uint16_t t);

void BLE_CheckState( void )
{
    //uint16_t tem16;
    
    if( GPIO_ReadInputData( GPIO_PORT_BLE_LINK ) & GPIO_PIN_BLE_LINK )
    {
        Ble.state.bit.link = 1;
    }else
    {
        Ble.state.bit.link = 0;
    }

}
void ble_get_token( uint8_t *number )
{
    uint8_t i;
    //if( strlen(number) != 5 ) return; //内存溢出检测
    uint16_t tem16;
    tem16 = FilterTem.Buf[ 1 ]; //以ADC缓存中的数值为种子
    
    for(i=0; i<4; i++)
    {
        
        srand( tem16 );//srand(g_usAdcValue);//
        number[i] = ( rand() % 10 ) + 0x30; //生成0—10内的随机整数, 并转成asc码
    }
    number[4] = 0;
    debug_usr("token:%s\r\n", number);
}


uint8_t hexncmp( uint8_t *s1, uint8_t *s2, uint8_t n )
{
    uint8_t i;
    
    for(i=0; i<n; i++)
    {
        if( *(s1+i) != *(s2+i) ) return 1;
    }
    
    return 0;
}
void BLE_IoInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_ALL_BLE, ENABLE);
	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //推挽输出模式
    
    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_BLE_WAKE;
    GPIO_Init(GPIO_PORT_BLE_WAKE, &GPIO_InitStructure);
      
    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_BLE_RESET;
    GPIO_Init(GPIO_PORT_BLE_RESET, &GPIO_InitStructure);
    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;// 输入模式
    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_BLE_LINK;
    GPIO_Init(GPIO_PORT_BLE_LINK, &GPIO_InitStructure);
    
    //拉高reset和wake
    GPIO_SetBits(GPIO_PORT_BLE_WAKE, GPIO_PIN_BLE_WAKE);
    GPIO_SetBits(GPIO_PORT_BLE_RESET, GPIO_PIN_BLE_RESET);
    
}


/*
*********************************************************************************************************
*	函 数 名: BLE_hw_reset
*	功能说明: 模块硬件复位=Pwrkey管脚上电序列，开机执行
*	形    参:
*	返 回 值: 无
*********************************************************************************************************
*/
void BLE_HwReset(void)
{
    debug_ble("Ble reset!\r\n");
    
    GPIO_PORT_BLE_RESET->BRR = GPIO_PIN_BLE_RESET;//拉低2S
    vTaskDelay(pdMS_TO_TICKS(100));
    GPIO_PORT_BLE_RESET->BSRR = GPIO_PIN_BLE_RESET;//再拉高
    vTaskDelay(pdMS_TO_TICKS(100));
}

void BLE_WakeUp(void)
{
    debug_ble("Ble WakeUp!\r\n");
    
    GPIO_PORT_BLE_RESET->BRR = GPIO_PIN_BLE_WAKE;//拉低电平>1s 有效，
    vTaskDelay(pdMS_TO_TICKS(1500));
    GPIO_PORT_BLE_RESET->BSRR = GPIO_PIN_BLE_WAKE;//再拉高
    vTaskDelay(pdMS_TO_TICKS(100));
    ble_wait_ack( "Wake UP\r\n", 2000 );
    
}

uint8_t BLE_Sleep( void )
{
    uint8_t i, err;
    //char TEM[ 10 ];
    i=0;
    
    while( 1 )
    {
        vTaskDelay( 1 );
//        SoftWdtFed( vTaskOtg_ID );
        
        switch( i )
        {
            case 0 ://
                err = ble_cmd("+++a","a+ok", 500 );
                if( err )
                {
                    i = 2;
                }else
                {
                    i = 1;
                }
                break;
             case 1 ://
                err = ble_cmd("AT+ENTM\r\n","+ENTM:OK", 500 );//
                
                if( err )
                {
                    i = 0;
                }else
                {
                    i = 3;
                }
                break;
            case 2://低功耗模式设置 
                
                //err = ble_cmd("AT+AUTOSLEEP=ON,4\r\n","AUTO:ON\r\n", 1000 ); //自动睡眠模式 2.5mA
                //err = ble_cmd("AT+DEEPSLEEP\r\n","+DEEPSLEEP:OK\r\n", 1000 ); //深度睡眠模式 0.39mA
                err = ble_cmd("AT+HIBERNATE\r\n","OK\r\n", 1000 ); //休眠模式 <60uA
                if( err )
                {
                    return 1;
                }
                
                break; 
             case 3 ://
                if( !Ble.state.bit.link ) //如果蓝牙没有激活
                {
                    Ble.state.bit.go_to_wake = 0;
                    Ble.state.bit.sleep = 0;
                    BLE_WakeUp( );
                    Ble.state.bit.wake = 1;
                }else
                {
                    BLE_HwReset( );
                }
                i = 0;
                break;
             default :
                vTaskDelay( 1000 ); 
                i = 0;
                break;
        }
        
        if( !err ) vTaskDelay( 3000 ); 
    }
    
}


void BLE_VarInit(void)
{
    Ble.lg.all = 0;
}

static uint8_t ble_wait_ack( char *ack, uint16_t t)
{
    uint8_t err, qn=1;
    char  *add;
    
    qn=t/100;
    
    while( qn )
    {
        err = comGetALine( BLE_COM, BLE_RxBuf );
        if( err ) //成功接收
    	{
    	    debug_ble("%s",BLE_RxBuf);
    	    
    	    add = strstr( (char*)BLE_RxBuf, ack );
            if( add )
            {
                comClearRxFifo( BLE_COM );
                return 1;
            }else
            {
               qn--;
               vTaskDelay(pdMS_TO_TICKS(100));
            }
    	}
    	else
    	{
    	    qn=0;
    	    debug_ble("[Timeout.Try again.]\r\n");
    	}
    }

    return 0;
}

static uint8_t ble_cmd(char *cmd, char *ack, uint16_t t)
{
    uint8_t err, qn=1;
    char *add;

    comSendBuf( BLE_COM, (uint8_t *)cmd, strlen(cmd) );
    
    debug_ble("%s\r\n",cmd);
    
    qn=t/100;
    
    while( qn )
    {
        vTaskDelay( 10 );
        err = comGetALine( BLE_COM, BLE_RxBuf );
        if( err ) //成功接收
    	{
    	    debug_ble("%s",BLE_RxBuf);
    	    add = strstr( (char *)BLE_RxBuf, ack );
            if( add )
            {
                comClearRxFifo( BLE_COM );
                return 1;
            }else
            {
               qn--;
               vTaskDelay(pdMS_TO_TICKS(100));
            }
    	}
    	else
    	{
    	    qn=0;
    	    debug_ble("[Timeout.Try again.]\r\n");
    	}
    }

    return 0;
}

uint8_t ble_get_mac( uint8_t *mac, uint16_t t)
{
    uint8_t err, qn=1, i;
    char *add;
    
    //BaseType_t xResult;
    //err = ble_cmd("AT+MAC?\r\n","+MAC:", 1000 ); // +MAC:9CA52512DD8E
    comSendBuf( BLE_COM, "AT+MAC?\r\n", strlen("AT+MAC?\r\n") );
    
    debug_ble("%s\r\n","AT+MAC?\r\n");
    
    qn=t/100;
    
    while( qn )
    {
        vTaskDelay( 10 );
        err = comGetALine( BLE_COM, BLE_RxBuf );
        if( err ) //成功接收
    	{
    	    debug_ble("%s",BLE_RxBuf);
    	    add = strstr( (char *)BLE_RxBuf, "+MAC:" );
            if( add )
            {
                for( i=0; i<12; i++ )
                {
                    *(mac+i) = *(add+5+i);
                }
                comClearRxFifo( BLE_COM );
                return 1;
            }else
            {
               qn--;
               vTaskDelay(pdMS_TO_TICKS(100));
            }
    	}
    	else
    	{
    	    qn=0;
    	    debug_ble("[Timeout.Try again.]\r\n");
    	}
    }

    return 0;
}


uint8_t BLE_PapaInit( void )
{
    uint8_t i, err;
    char TEM[ 10 ];
    i=0;
    //WH-BLE103 V1.0.7
    while( 1 )
    {
        vTaskDelay( 1 );
//        SoftWdtFed( vTaskOtg_ID );
        
        switch( i )
        {
            case 0 ://
                err = ble_cmd("+++a","a+ok", 500 );
                if( err )
                {
                    i = 2;
                }else
                {
                    i = 1;
                }
                break;
             case 1 ://
                err = ble_cmd("AT+ENTM\r\n","+ENTM:OK", 500 );//err = ble_cmd("AT+PASS?\r\n","+PASS:", 500 ); //JC-G6201
//                if( err )
//                {
//                    i=0;
//                }
                i=0;
                break;
             case 2 ://设置名称
                sprintf( TEM, "AT+NAME=%s\r\n", Ble.Name );
                err = ble_cmd( TEM, Ble.Name, 1000 ); //err = ble_cmd( TEM, "+NAME:JC-G6201", 1000 ); //
                if( err )
                {
                    i++;
                }
                break;
             case 3 ://设置UUID
                err = ble_cmd("AT+UUID=AA0000fee700001000800000805f9b34fb\r\n","DATA:0000FEE7", 1000 ); //0000fee7-0000-1000-8000-00805f9b34fb
                if( err )
                {
                    i++;
                }
                break;
             case 4 ://设置UUID r
                err = ble_cmd("AT+UUID=BB000036f600001000800000805f9b34fb\r\n","DATA:000036F6", 1000 ); //000036f6-0000-1000-8000-00805f9b34fb
                
                if( err )
                {
                    i++;
                }
                break;
             case 5 ://设置UUID w
                err = ble_cmd("AT+UUID=CC000036f500001000800000805f9b34fb\r\n","DATA:000036F5", 1000 ); //000036f5-0000-1000-8000-00805f9b34fb
                if( err )
                {
                    i++;
                }
                break;
                
             case 6 ://查询MAC地址 +MAC:9CA52512DD8E
                ble_get_mac( Ble.Mac, 3000 );//err = ble_cmd("AT+MAC?\r\n","+MAC:", 1000 ); //
                if( err )
                {
                    i++;
                }
                break;
//             case 7://设为从机模式 设置完从机模块会复位，退出命令模式
//                err = ble_cmd("AT+MODE=S\r\n","+MODE:Slave", 1000 ); //
//                if( err )
//                {
//                    i++;
//                }
//                break; AT+HIBERNATE
            case 7://低功耗模式设置 
                
                err = ble_cmd("AT+AUTOSLEEP=OFF,4\r\n","AUTO:OFF\r\n", 1000 ); //取消自动睡眠模式
                //err = ble_cmd("AT+AUTOSLEEP=ON,5\r\n","AUTO:ON\r\n", 1000 ); //自动睡眠模式 2.5mA
                //err = ble_cmd("AT+DEEPSLEEP\r\n","+DEEPSLEEP:OK\r\n", 1000 ); //深度睡眠模式 0.39mA
                //err = ble_cmd("AT+HIBERNATE\r\n","OK\r\n", 1000 ); //休眠模式 <60uA
                if( err )
                {
                    i++;
                    return 1;
                }
                
                break; 
            case 8 ://
                err = ble_cmd("AT+ENTM\r\n","+ENTM:OK", 500 );
                if( err )
                {
                    return 1;
                }
                break;
             
             default :
                vTaskDelay( 5000 ); 
                i = 0;
                break;
        }
        
        if( !err ) vTaskDelay( 3000 ); 
    }
    
}

/*
*********************************************************************************************************
*	函 数 名: BLE_init
*	功能说明: mc20 硬件初始化
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void BLE_Init(void)
{
    BLE_IoInit();
    BLE_VarInit();
}

/*
*********************************************************************************************************
*	函 数 名: BLE_GetMsg
*	功能说明: 接收16个字节的数据, 超时结束
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
//uint8_t BLE_GetMsg( uint8_t *msg, uint16_t len )
//{
//    uint8_t i=0, err=0, err_n=0, tem8=0;
//    uint8_t TEM[20];
//    int *tem16;
//    
//    if( comGetLen( BLE_COM ) > 15 ) //收接收数据
//    {
//        while( ( err_n<2 ) && ( i<len ) )
//        {
//            err = comGetChar(BLE_COM, &tem8);
//            if( err ) //有数据
//            {
//                *( TEM + i++ ) = tem8;
//            }else //无数据等待500ms
//            {
//                err_n++;
//                vTaskDelay(pdMS_TO_TICKS(500));
//            }
//        }
//    }
//    
//    
//    //解密
//    if( i )
//    {
//        //int AESEncode (BYTE* srcString, int srcLen, BYTE* dstString, int* dstLen, BYTE *UserKey) 
//        //err = AESDecode( TEM, i, msg, tem16, data_saved.data.KEY );
//        if( err ) i = 0;//解密错误
//        else i = (uint8_t) tem16;
//    }
//    
//    return i;
//}

uint8_t BLE_GetMsg( uint8_t *msg, uint16_t len )
{
    uint8_t i=0, err=0, err_n=0, tem8=0;
    
    if( comGetLen( BLE_COM ) > 5 ) //收接收数据
    {
        while( ( err_n<2 ) && ( i<len ) )
        {
            err = comGetChar(BLE_COM, &tem8);
            if( err ) //有数据
            {
                *( msg + i++ ) = tem8;
                Ble.timer_ble_nodata = BLE_TIME_NODATA; 
            }else //无数据等待500ms
            {
                err_n++;
                vTaskDelay(pdMS_TO_TICKS(100));
            }
        }
    }
    
    //解密
    
    return i;
}

//uint8_t ble_find_one_meg( uint8_t *hex1, uint8_t *hex2, uint8_t len )
//{
//    uint8_t i=0, n=0;
//    
//    for( i=0; i<len; )
//    {
//        if( ( *( hex1 + i ) == *hex2 ) && ( *( hex1 + i + 1 ) == *( hex2 + 1 ) ) )
//        {
//            n = i;
//            break;
//        }else
//        {
//            i++;
//        }
//    }
//    
//     
//    if( len-n < 8 ) return 0; //剩下长度不够一条数据(8字节),或没有找到数据
//    
//    if( n ) //如果数据不是从头开始，平移到开头
//    {
//        for( i=0; i<8; i++ )
//        {
//            *( hex1 + i ) = *( hex1 + n + i );
//        }
//    }    
//    
//    return i;
//}

//uint8_t FindOneMeg( uint8_t *buf, uint16_t hex16, uint8_t len )
uint8_t ble_find_one_meg( uint8_t *buf, uint16_t hex16, uint8_t len )
{
    uint8_t i=0, k=0, n=0;
    
    uint16_t tem16;
    
    for( i=0; i<len; )
    {
        tem16 = ( ( (uint16_t)(*( buf + i ) ) << 8 ) )+ *( buf + i + 1 );
        if( tem16 == hex16 )
        {
            n = 1;
            break;
        }else
        {
            i++;
        }
    }
    
     
    if( len-i < 8 ) return 0; //剩下长度不够一条数据(8字节),或没有找到数据
    
    if( i ) //如果数据不是从头开始，平移到开头
    {
        for( k=0; k<8; k++ )
        {
            *( buf + i ) = *( buf + i + k);
        }
    }    
    
    return n;
}

//uint8_t CheckToken( uint8_t *buf, uint16_t hex16, uint8_t len )
uint8_t ble_check_token( uint8_t *buf, uint8_t *token )
{
    uint8_t i;
    
    for( i=0; i<4; i++ )
    {
        if( *(buf+i) != *(token+i) ) return 0;
    }
    
    return 1;
}

uint8_t ble_check_pwd( uint8_t *buf, uint8_t *pwd )
{
    uint8_t i;
    
    for( i=0; i<6; i++ )
    {
        if( *(buf+i) != *(pwd+i) ) return 0;
    }
    
    return 1;
}

uint8_t BLE_GetSendMsg( uint8_t *msg, uint16_t cmd )
{
    //uint8_t i=0, err=0, err_n=0, tem8=0;
    switch( cmd )
    {
        case BLE_GET_TOKEN : //获取令牌
            
            break;
        case BLE_SET_NAME : //修改名称
            break;
        default : break;    
    }
    return 0;
}


uint8_t BLE_Send( uint8_t *msg, uint8_t len )
{
    uint8_t err;
    
    err = 0;
    
    //加密
    
    comSendBuf( BLE_COM, msg, len);
    
    return err;
}

/*
*********************************************************************************************************
*	函 数 名: BLE_Pares
*	功能说明: 
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
uint8_t BLE_Pares( uint8_t *msg, uint8_t len )
{
//    uint8_t i, n, err, TEM[10], KEY_TEM[16];
//    
//    i=0;
//    n=0;
//    err = 0;
//    
//    if( ble_find_one_meg( msg, BLE_GET_TOKEN, len ) )//获取令牌
//    {
//        debug_usr("BLE_GET_TOKEN\r\n");
//        
//        ble_get_token( Ble.Token );//随机获取令牌
//        
//        //生成回应帧
//        *( msg + i++) = (uint8_t)( BLE_ACK_TOKEN >> 8 ); //命令编号
//        *( msg + i++) = (uint8_t)( BLE_ACK_TOKEN ); //命令编号
//        *( msg + i++) = 0x04; //LEN 后续有效字节数
//        for( n=0; n<4; n++ ) //token
//        {
//            *( msg + i++ ) = Ble.Token[ n ];
//        }
//        *( msg + i++) = 0x01; //芯片类型
//        *( msg + i++) = 0x01; //版本号
//        *( msg + i++) = 0x00; //版本号
//        *( msg + i++) = 0x00; //IDL
//        *( msg + i++) = 0x00; //IDH
//        *( msg + i++) = !Lock.state.bit.lock; //STA 是锁的开关状态
//        *( msg + i++) = 0x00; //CNT 总的开锁次数
//        *( msg + i++) = 0x00; //CNT
//        *( msg + i++) = 0x00; //
//        
//    }else
//    if( ble_find_one_meg( msg, BLE_SET_NAME, len ) ) //修改广播名称
//    {
//        debug_usr("BLE_SET_NAME\r\n");
//        
//        Ble.lg.bit.reset = 1; //回应后，模块复位
//        if( ble_check_token( msg+11, Ble.Token ) == 0 ) //校验令牌
//        {
//            return 0; //校验失败
//        }
//        
//        if( *( msg + 2 )>8 ) *( msg + 2 ) = 8; //长度不能超过8
//            
//        for( n=0; n<*( msg + 2 ); n++ ) //得到广播名称
//        {
//            Ble.Name[ n ] = *( msg + 3 + n );
//        }
//        
//        for( ;n<9; n++ ) //后面清零
//        {
//            Ble.Name[ n ] = 0x00;
//        }
//        
//        for( n=0; n<9; n++ ) //得到广播名称
//        {
//            data_saved.data.NAME[n] = Ble.Name[ n ];
//        }
//        
//        //save
//        SaveToFlash();
//        
//        //生成回应帧
//        *( msg + i++) = (uint8_t)( BLE_ACK_NAME >> 8 ); //命令编号
//        *( msg + i++) = (uint8_t)( BLE_ACK_NAME ); //命令编号
//        *( msg + i++) = 0x01; //LEN 后续有效字节数
//        *( msg + i++) = err; //修改结果 00 成功 01 失败
//        
//        for( ; i<16; ) //无效数据
//        {
//            *( msg + i++ ) = 0;
//        }
//    }else
//    if( ble_find_one_meg( msg, BLE_GET_POWER, len ) ) //获取电量
//    {
//        debug_usr("BLE_GET_POWER\r\n");
//        
//        if( ble_check_token( msg+4, Ble.Token ) == 0 ) //校验令牌
//        {
//            return 0; //校验失败
//        }
//        
//        //生成回应帧
//        *( msg + i++) = (uint8_t)( BLE_ACK_POWER >> 8 ); //命令编号
//        *( msg + i++) = (uint8_t)( BLE_ACK_POWER ); //命令编号
//        *( msg + i++) = 0x01; //LEN 后续有效字节数
//        *( msg + i++) = Lock.power_percent; //电池电量百分比 0~100
//        
//        for( ; i<16; ) //无效数据
//        {
//            *( msg + i++ ) = 0;
//        }
//    }else
//    if( ble_find_one_meg( msg, BLE_UNLOCK, len ) ) //开锁
//    {
//        debug_usr("BLE_UNLOCK\r\n");
//        
//        if( ble_check_token( msg+9, Ble.Token ) == 0 ) //校验令牌
//        {
//            return 0; //校验失败
//        }
//        
//        if( ble_check_pwd( msg+3, data_saved.data.PWD ) == 0 ) //校验密码
//        {
//            return 0; //校验失败
//        }
//        
//        //生成回应帧
//        *( msg + i++) = (uint8_t)( BLE_ACK_UNLOCK >> 8 ); //命令编号
//        *( msg + i++) = (uint8_t)( BLE_ACK_UNLOCK ); //命令编号
//        *( msg + i++) = 0x01; //LEN 后续有效字节数
//        
//        //开锁
//        err = lock_unlock( );
//        
//        *( msg + i++) = !err; //状态返回 00 表示开锁成功 //01 表示开锁失败 和开锁函数实际返回值相反 //Lock.state.bit.lock; //
//        
//        for( ; i<16; ) //无效数据
//        {
//            *( msg + i++ ) = 0;
//        }
//    }else
//    if( ble_find_one_meg( msg, BLE_LOCK, len ) ) //关锁
//    {
//        debug_usr("BLE_LOCK\r\n");
//        
//        if( ble_check_token( msg+4, Ble.Token ) == 0 ) //校验令牌
//        {
//            return 0; //校验失败
//        }
//        
//        //生成回应帧
//        *( msg + i++) = (uint8_t)( BLE_ACK_LOCK >> 8 ); //命令编号
//        *( msg + i++) = (uint8_t)( BLE_ACK_LOCK ); //命令编号
//        *( msg + i++) = 0x01; //LEN 后续有效字节数
//        
//        //关锁
//        err = lock_lock( );
//        
//        *( msg + i++) = !err; //状态返回 00 表示开锁成功 //01 表示开锁失败 和关锁函数实际返回值相反 //!Lock.state.bit.lock; //
//        
//        for( ; i<16; ) //无效数据
//        {
//            *( msg + i++ ) = 0;
//        }
//    }else
//    if( ble_find_one_meg( msg, BLE_GET_STA, len ) ) //查询锁状态
//    {
//        debug_usr("BLE_GET_STA\r\n");
//        
//        if( ble_check_token( msg+4, Ble.Token ) == 0 ) //校验令牌
//        {
//            return 0; //校验失败
//        }
//        
//        //生成回应帧
//        *( msg + i++) = (uint8_t)( BLE_ACK_STA >> 8 ); //命令编号
//        *( msg + i++) = (uint8_t)( BLE_ACK_STA ); //命令编号
//        *( msg + i++) = 0x01; //LEN 后续有效字节数
//        *( msg + i++) = !Lock.state.bit.lock; //STA 为锁状态， 00 表示开启状态， 01 表示关闭状态。 这里与锁的协议相反
//        
//        for( ; i<16; ) //无效数据
//        {
//            *( msg + i++ ) = 0;
//        }
//    }else
//    if( ble_find_one_meg( msg, BLE_OLD_PWD, len ) ) //修改密码，旧密码
//    {
//        debug_usr("BLE_OLD_PWD\r\n");
//        
//        if( !Ble.lg.bit.pwd ) //从未接收过旧密码
//        {
//            if( ble_check_token( msg+9, Ble.Token ) == 0 ) //校验令牌
//            {
//                return 0; //校验失败
//            }
//            for( n=0; n<6; n++ ) //得到旧密码
//            {
//                TEM[n] = *( msg + 3 + i );
//            }
//            
//            err = hexncmp( data_saved.data.PWD, TEM, 6 );
//            if( !err )
//            {
//                Ble.lg.bit.pwd = 1;
//                debug_usr("old pwd is correct.\r\n");
//            }else
//            {
//                debug_usr("old pwd is incorrect.\r\n");
//            }
//        }else //已经接收过旧密码，重复接收错误，舍弃
//        {
//            Ble.lg.bit.pwd = 0;
//        }
//        
//        
//        //应加入超时处理
//        //...
//        
//        return 0;//此命令无需返回
//    }else
//    if( ble_find_one_meg( msg, BLE_NEW_PWD, len ) ) //修改密码，新密码
//    {
//        debug_usr("BLE_NEW_PWD\r\n");
//        
//        if( Ble.lg.bit.pwd ) //旧密码校验成功
//        {
//            if( ble_check_token( msg+9, Ble.Token ) == 0 ) //校验令牌
//            {
//                return 0; //校验失败
//            }
//            
//            for( n=0; n<6; n++ ) //得到新密码
//            {
//                TEM[n] = *( msg + 3 + n );
//            }
//            
//            Ble.lg.bit.pwd = 0;
//            for( n=0; n<6; n++ ) //得到旧密码
//            {
//                data_saved.data.PWD[ n ] = TEM[n];
//            }
//            
//            //save
//            SaveToFlash();
//            
//            err = 0;//成功
//            debug_usr("new pwd is setted.\r\n");
//        }else //旧密码校验失败
//        {
//            err = 1;
//            debug_usr("new pwd is error.\r\n");
//        }
//        
//        //生成回应帧
//        *( msg + i++) = (uint8_t)( BLE_ACK_PWD >> 8 ); //命令编号
//        *( msg + i++) = (uint8_t)( BLE_ACK_PWD ); //命令编号
//        *( msg + i++) = 0x01; //LEN 后续有效字节数
//        *( msg + i++) = err; //修改结果 00 成功 01 失败
//        
//        for( ; i<16; ) //无效数据
//        {
//            *( msg + i++ ) = 0;
//        }
//    }else
//    if( ble_find_one_meg( msg, BLE_SET_KEYL, len ) ) //修改秘钥，新密钥前 8 个字节
//    {
//        if( !Ble.lg.bit.key ) //从未接收过新秘钥前8个字节
//        {
//            if( ble_check_token( msg+11, Ble.Token ) == 0 ) //校验令牌
//            {
//                return 0; //校验失败
//            }
//            for( n=0; n<8; n++ ) //得到新秘钥
//            {
//                KEY_TEM[ n ]  = *( msg + 3 + n );
//            }
//            
//            Ble.lg.bit.key = 1;
//        
//            //应加入超时处理
//            //...
//        }else //接收过新秘钥前8个字节，重复接收错误，舍弃
//        {
//            Ble.lg.bit.key = 0;
//        }
//        return 0;//此命令无需返回
//    }else
//    if( ble_find_one_meg( msg, BLE_SET_KEYH, len ) ) //修改秘钥，新密钥后 8 个字节
//    {
//        if( Ble.lg.bit.key ) //得到新秘钥前8个字节
//        {
//            Ble.lg.bit.key = 0;
//            
//            if( ble_check_token( msg+11, Ble.Token ) == 0 ) //校验令牌
//            {
//                return 0; //校验失败
//            }
//            
//            for( n=0; n<8; n++ ) //得到新秘钥后 8 个字节
//            {
//                KEY_TEM[ 8+n ]  = *( msg + 3 + n );
//            }
//            
//            for( n=0; n<16; n++ ) //得到新秘钥后 8 个字节
//            {
//                data_saved.data.KEY[ n ]  = KEY_TEM[n];
//            }
//            //save
//            SaveToFlash();
//            err = 0;
//            debug_usr("new key is setted.\r\n");
//            
//        }else //旧密码校验失败
//        {
//            err = 1;
//            debug_usr("new key is error.\r\n");
//        }
//        
//        //生成回应帧
//        *( msg + i++) = (uint8_t)( BLE_ACK_KEY >> 8 ); //命令编号
//        *( msg + i++) = (uint8_t)( BLE_ACK_KEY ); //命令编号
//        *( msg + i++) = 0x01; //LEN 后续有效字节数
//        *( msg + i++) = err; //修改结果 00 成功 01 失败
//        
//        for( ; i<16; ) //无效数据
//        {
//            *( msg + i++ ) = 0;
//        }
//    }
////    else
////    if( ble_find_one_meg( msg, BLE_REPORT_STA, len ) )//上报状态 
////    {
////        ;
////    }
//    
//    if( i ) //发送回应帧
//    {
//        BLE_Send( msg, i);
//        
//        for(n=0;n<i;n++) 
//        {
//            comSendCharHex( LOCAL_COM, msg[n] );
//            comSendChar( LOCAL_COM, ' ' );
//        }
//        comSendBuf( LOCAL_COM, "\r\n", 2 );
//    }
//    
//    if( Ble.lg.bit.reset ) //更改广播名等命令，回应后，模块复位
//    {
//        Ble.lg.bit.reset = 0;
//        vTaskDelay( pdMS_TO_TICKS(2000) );
//        NVIC_SystemReset();//系统复位
//    }
    
    return 1;//return i;
}

uint8_t BLE_ReportState( void )
{
    uint8_t i;
    
    i=0;
    
    //生成回应帧
    BLE_TxBuf[ i++ ] = (uint8_t)( BLE_REPORT_STA >> 8 ); //命令编号
    BLE_TxBuf[ i++ ] = (uint8_t)( BLE_REPORT_STA ) + 1; //命令编号
    BLE_TxBuf[ i++ ] = 0x01; //LEN 后续有效字节数
    BLE_TxBuf[ i++ ] = 0;//!Lock.state.bit.lock; //STA 为锁状态， 00 表示开启状态， 01 表示关闭状态。
    
    for( ; i<16; i++ ) //无效数据
    {
        BLE_TxBuf[ i++ ] = 0;
    }
    
    BLE_Send( BLE_TxBuf, i);
        
    return i;
}

/******************************************************* END OF FILE ************************************************************/

/*

测试数据：

获取令牌：       
06 01 01 01 2D 1A 68 3D 48 27 1A 18 31 6E 47 1A
06 02 04 30 30 30 30 01 01 00 00 00 01 00 00 00 
修改广播名称: 04 01 N NAME[N] TOKEN[4] F[1]
              04 01 08 41 42 43 44 45 46 47 48 30 30 30 30 00
              04 01 08 4A 43 2D 47 36 32 30 31 30 30 30 30 00 //
获取电量:        
02 01 01 01 30 30 30 30 30 00 00 00 00 00 00 00
02 02 01 2C 00 00 00 00 00 00 00 00 00 00 00 00 
开锁: 05 01 06 PWD[6] TOKEN[4] FILL[3]
      05 01 06 30 30 30 30 30 30 30 30 30 30 00 00 00
      05 02 01 01 00 00 00 00 00 00 00 00 00 00 00 00
        
关锁：05 0C 01 01 TOKEN[4] FILL[8]
      05 0C 01 01 30 30 30 30 00 00 00 00 00 00 00 00       
      
查询锁状态：05 0E 01 01 TOKEN[4] FILL[12]
            05 0E 01 01 30 30 30 30 00 00 00 00 00 00 00 00
            05 0F 01 01 00 00 00 00 00 00 00 00 00 00 00 00
            
修改密码：主机连续发送 2 条通信帧，两条直接需要插入一定的延迟：
          05 03 06 OLDPWD[6] TOKEN[4] FILL[3]
          05 04 06 NEWPWD[6] TOKEN[4] FILL[3]
          05 03 06 30 30 30 30 30 30 30 30 30 30 00 00 00
          05 04 06 30 30 30 30 30 30 30 30 30 30 00 00 00
          05 05 01 00 00 00 00 00 00 00 00 00 00 00 00 00   //设置成功
          05 05 01 01 00 00 00 00 00 00 00 00 00 00 00 00   //设置失败
          
修改密钥：
          07 01 08 KEYL[8] TOKEN[4] F[1]
          07 02 08 KEYH[8] TOKEN[4] F[1]
          07 01 08 38 38 38 38 38 38 38 38 30 30 30 30 00
          07 02 08 38 38 38 38 38 38 38 38 30 30 30 30 00
          
          07 03 01 00 00 00 00 00 00 00 00 00 00 00 00 00   //设置成功
          07 03 01 01 00 00 00 00 00 00 00 00 00 00 00 00   //设置失败
          
869976031496988
*/
