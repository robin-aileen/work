
/*
*********************************************************************************************************
*
*	ģ������ : BLEģ����������
*	�ļ����� : BLE.c
*	��    �� : V0.1
*   M  C  U  : STM32F103RBT6
*   �� �� �� : IAR-Arm 8.22.2
*
*	˵    �� : ģ������汾RD1006@180511-CAUT03
*
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*		V0.1    2018-05-15 robin    ���԰�
*       V0.1    2018-09-12 robin    ������ xSemUsrSend ���Ᵽ�����ͻ���
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
    //if( strlen(number) != 5 ) return; //�ڴ�������
    uint16_t tem16;
    tem16 = FilterTem.Buf[ 1 ]; //��ADC�����е���ֵΪ����
    
    for(i=0; i<4; i++)
    {
        
        srand( tem16 );//srand(g_usAdcValue);//
        number[i] = ( rand() % 10 ) + 0x30; //����0��10�ڵ��������, ��ת��asc��
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
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //�������ģʽ
    
    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_BLE_WAKE;
    GPIO_Init(GPIO_PORT_BLE_WAKE, &GPIO_InitStructure);
      
    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_BLE_RESET;
    GPIO_Init(GPIO_PORT_BLE_RESET, &GPIO_InitStructure);
    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;// ����ģʽ
    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_BLE_LINK;
    GPIO_Init(GPIO_PORT_BLE_LINK, &GPIO_InitStructure);
    
    //����reset��wake
    GPIO_SetBits(GPIO_PORT_BLE_WAKE, GPIO_PIN_BLE_WAKE);
    GPIO_SetBits(GPIO_PORT_BLE_RESET, GPIO_PIN_BLE_RESET);
    
}


/*
*********************************************************************************************************
*	�� �� ��: BLE_hw_reset
*	����˵��: ģ��Ӳ����λ=Pwrkey�ܽ��ϵ����У�����ִ��
*	��    ��:
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void BLE_HwReset(void)
{
    debug_ble("Ble reset!\r\n");
    
    GPIO_PORT_BLE_RESET->BRR = GPIO_PIN_BLE_RESET;//����2S
    vTaskDelay(pdMS_TO_TICKS(100));
    GPIO_PORT_BLE_RESET->BSRR = GPIO_PIN_BLE_RESET;//������
    vTaskDelay(pdMS_TO_TICKS(100));
}

void BLE_WakeUp(void)
{
    debug_ble("Ble WakeUp!\r\n");
    
    GPIO_PORT_BLE_RESET->BRR = GPIO_PIN_BLE_WAKE;//���͵�ƽ>1s ��Ч��
    vTaskDelay(pdMS_TO_TICKS(1500));
    GPIO_PORT_BLE_RESET->BSRR = GPIO_PIN_BLE_WAKE;//������
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
            case 2://�͹���ģʽ���� 
                
                //err = ble_cmd("AT+AUTOSLEEP=ON,4\r\n","AUTO:ON\r\n", 1000 ); //�Զ�˯��ģʽ 2.5mA
                //err = ble_cmd("AT+DEEPSLEEP\r\n","+DEEPSLEEP:OK\r\n", 1000 ); //���˯��ģʽ 0.39mA
                err = ble_cmd("AT+HIBERNATE\r\n","OK\r\n", 1000 ); //����ģʽ <60uA
                if( err )
                {
                    return 1;
                }
                
                break; 
             case 3 ://
                if( !Ble.state.bit.link ) //�������û�м���
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
        if( err ) //�ɹ�����
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
        if( err ) //�ɹ�����
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
        if( err ) //�ɹ�����
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
             case 2 ://��������
                sprintf( TEM, "AT+NAME=%s\r\n", Ble.Name );
                err = ble_cmd( TEM, Ble.Name, 1000 ); //err = ble_cmd( TEM, "+NAME:JC-G6201", 1000 ); //
                if( err )
                {
                    i++;
                }
                break;
             case 3 ://����UUID
                err = ble_cmd("AT+UUID=AA0000fee700001000800000805f9b34fb\r\n","DATA:0000FEE7", 1000 ); //0000fee7-0000-1000-8000-00805f9b34fb
                if( err )
                {
                    i++;
                }
                break;
             case 4 ://����UUID r
                err = ble_cmd("AT+UUID=BB000036f600001000800000805f9b34fb\r\n","DATA:000036F6", 1000 ); //000036f6-0000-1000-8000-00805f9b34fb
                
                if( err )
                {
                    i++;
                }
                break;
             case 5 ://����UUID w
                err = ble_cmd("AT+UUID=CC000036f500001000800000805f9b34fb\r\n","DATA:000036F5", 1000 ); //000036f5-0000-1000-8000-00805f9b34fb
                if( err )
                {
                    i++;
                }
                break;
                
             case 6 ://��ѯMAC��ַ +MAC:9CA52512DD8E
                ble_get_mac( Ble.Mac, 3000 );//err = ble_cmd("AT+MAC?\r\n","+MAC:", 1000 ); //
                if( err )
                {
                    i++;
                }
                break;
//             case 7://��Ϊ�ӻ�ģʽ ������ӻ�ģ��Ḵλ���˳�����ģʽ
//                err = ble_cmd("AT+MODE=S\r\n","+MODE:Slave", 1000 ); //
//                if( err )
//                {
//                    i++;
//                }
//                break; AT+HIBERNATE
            case 7://�͹���ģʽ���� 
                
                err = ble_cmd("AT+AUTOSLEEP=OFF,4\r\n","AUTO:OFF\r\n", 1000 ); //ȡ���Զ�˯��ģʽ
                //err = ble_cmd("AT+AUTOSLEEP=ON,5\r\n","AUTO:ON\r\n", 1000 ); //�Զ�˯��ģʽ 2.5mA
                //err = ble_cmd("AT+DEEPSLEEP\r\n","+DEEPSLEEP:OK\r\n", 1000 ); //���˯��ģʽ 0.39mA
                //err = ble_cmd("AT+HIBERNATE\r\n","OK\r\n", 1000 ); //����ģʽ <60uA
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
*	�� �� ��: BLE_init
*	����˵��: mc20 Ӳ����ʼ��
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void BLE_Init(void)
{
    BLE_IoInit();
    BLE_VarInit();
}

/*
*********************************************************************************************************
*	�� �� ��: BLE_GetMsg
*	����˵��: ����16���ֽڵ�����, ��ʱ����
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
//uint8_t BLE_GetMsg( uint8_t *msg, uint16_t len )
//{
//    uint8_t i=0, err=0, err_n=0, tem8=0;
//    uint8_t TEM[20];
//    int *tem16;
//    
//    if( comGetLen( BLE_COM ) > 15 ) //�ս�������
//    {
//        while( ( err_n<2 ) && ( i<len ) )
//        {
//            err = comGetChar(BLE_COM, &tem8);
//            if( err ) //������
//            {
//                *( TEM + i++ ) = tem8;
//            }else //�����ݵȴ�500ms
//            {
//                err_n++;
//                vTaskDelay(pdMS_TO_TICKS(500));
//            }
//        }
//    }
//    
//    
//    //����
//    if( i )
//    {
//        //int AESEncode (BYTE* srcString, int srcLen, BYTE* dstString, int* dstLen, BYTE *UserKey) 
//        //err = AESDecode( TEM, i, msg, tem16, data_saved.data.KEY );
//        if( err ) i = 0;//���ܴ���
//        else i = (uint8_t) tem16;
//    }
//    
//    return i;
//}

uint8_t BLE_GetMsg( uint8_t *msg, uint16_t len )
{
    uint8_t i=0, err=0, err_n=0, tem8=0;
    
    if( comGetLen( BLE_COM ) > 5 ) //�ս�������
    {
        while( ( err_n<2 ) && ( i<len ) )
        {
            err = comGetChar(BLE_COM, &tem8);
            if( err ) //������
            {
                *( msg + i++ ) = tem8;
                Ble.timer_ble_nodata = BLE_TIME_NODATA; 
            }else //�����ݵȴ�500ms
            {
                err_n++;
                vTaskDelay(pdMS_TO_TICKS(100));
            }
        }
    }
    
    //����
    
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
//    if( len-n < 8 ) return 0; //ʣ�³��Ȳ���һ������(8�ֽ�),��û���ҵ�����
//    
//    if( n ) //������ݲ��Ǵ�ͷ��ʼ��ƽ�Ƶ���ͷ
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
    
     
    if( len-i < 8 ) return 0; //ʣ�³��Ȳ���һ������(8�ֽ�),��û���ҵ�����
    
    if( i ) //������ݲ��Ǵ�ͷ��ʼ��ƽ�Ƶ���ͷ
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
        case BLE_GET_TOKEN : //��ȡ����
            
            break;
        case BLE_SET_NAME : //�޸�����
            break;
        default : break;    
    }
    return 0;
}


uint8_t BLE_Send( uint8_t *msg, uint8_t len )
{
    uint8_t err;
    
    err = 0;
    
    //����
    
    comSendBuf( BLE_COM, msg, len);
    
    return err;
}

/*
*********************************************************************************************************
*	�� �� ��: BLE_Pares
*	����˵��: 
*	��    ��:  ��
*	�� �� ֵ: ��
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
//    if( ble_find_one_meg( msg, BLE_GET_TOKEN, len ) )//��ȡ����
//    {
//        debug_usr("BLE_GET_TOKEN\r\n");
//        
//        ble_get_token( Ble.Token );//�����ȡ����
//        
//        //���ɻ�Ӧ֡
//        *( msg + i++) = (uint8_t)( BLE_ACK_TOKEN >> 8 ); //������
//        *( msg + i++) = (uint8_t)( BLE_ACK_TOKEN ); //������
//        *( msg + i++) = 0x04; //LEN ������Ч�ֽ���
//        for( n=0; n<4; n++ ) //token
//        {
//            *( msg + i++ ) = Ble.Token[ n ];
//        }
//        *( msg + i++) = 0x01; //оƬ����
//        *( msg + i++) = 0x01; //�汾��
//        *( msg + i++) = 0x00; //�汾��
//        *( msg + i++) = 0x00; //IDL
//        *( msg + i++) = 0x00; //IDH
//        *( msg + i++) = !Lock.state.bit.lock; //STA �����Ŀ���״̬
//        *( msg + i++) = 0x00; //CNT �ܵĿ�������
//        *( msg + i++) = 0x00; //CNT
//        *( msg + i++) = 0x00; //
//        
//    }else
//    if( ble_find_one_meg( msg, BLE_SET_NAME, len ) ) //�޸Ĺ㲥����
//    {
//        debug_usr("BLE_SET_NAME\r\n");
//        
//        Ble.lg.bit.reset = 1; //��Ӧ��ģ�鸴λ
//        if( ble_check_token( msg+11, Ble.Token ) == 0 ) //У������
//        {
//            return 0; //У��ʧ��
//        }
//        
//        if( *( msg + 2 )>8 ) *( msg + 2 ) = 8; //���Ȳ��ܳ���8
//            
//        for( n=0; n<*( msg + 2 ); n++ ) //�õ��㲥����
//        {
//            Ble.Name[ n ] = *( msg + 3 + n );
//        }
//        
//        for( ;n<9; n++ ) //��������
//        {
//            Ble.Name[ n ] = 0x00;
//        }
//        
//        for( n=0; n<9; n++ ) //�õ��㲥����
//        {
//            data_saved.data.NAME[n] = Ble.Name[ n ];
//        }
//        
//        //save
//        SaveToFlash();
//        
//        //���ɻ�Ӧ֡
//        *( msg + i++) = (uint8_t)( BLE_ACK_NAME >> 8 ); //������
//        *( msg + i++) = (uint8_t)( BLE_ACK_NAME ); //������
//        *( msg + i++) = 0x01; //LEN ������Ч�ֽ���
//        *( msg + i++) = err; //�޸Ľ�� 00 �ɹ� 01 ʧ��
//        
//        for( ; i<16; ) //��Ч����
//        {
//            *( msg + i++ ) = 0;
//        }
//    }else
//    if( ble_find_one_meg( msg, BLE_GET_POWER, len ) ) //��ȡ����
//    {
//        debug_usr("BLE_GET_POWER\r\n");
//        
//        if( ble_check_token( msg+4, Ble.Token ) == 0 ) //У������
//        {
//            return 0; //У��ʧ��
//        }
//        
//        //���ɻ�Ӧ֡
//        *( msg + i++) = (uint8_t)( BLE_ACK_POWER >> 8 ); //������
//        *( msg + i++) = (uint8_t)( BLE_ACK_POWER ); //������
//        *( msg + i++) = 0x01; //LEN ������Ч�ֽ���
//        *( msg + i++) = Lock.power_percent; //��ص����ٷֱ� 0~100
//        
//        for( ; i<16; ) //��Ч����
//        {
//            *( msg + i++ ) = 0;
//        }
//    }else
//    if( ble_find_one_meg( msg, BLE_UNLOCK, len ) ) //����
//    {
//        debug_usr("BLE_UNLOCK\r\n");
//        
//        if( ble_check_token( msg+9, Ble.Token ) == 0 ) //У������
//        {
//            return 0; //У��ʧ��
//        }
//        
//        if( ble_check_pwd( msg+3, data_saved.data.PWD ) == 0 ) //У������
//        {
//            return 0; //У��ʧ��
//        }
//        
//        //���ɻ�Ӧ֡
//        *( msg + i++) = (uint8_t)( BLE_ACK_UNLOCK >> 8 ); //������
//        *( msg + i++) = (uint8_t)( BLE_ACK_UNLOCK ); //������
//        *( msg + i++) = 0x01; //LEN ������Ч�ֽ���
//        
//        //����
//        err = lock_unlock( );
//        
//        *( msg + i++) = !err; //״̬���� 00 ��ʾ�����ɹ� //01 ��ʾ����ʧ�� �Ϳ�������ʵ�ʷ���ֵ�෴ //Lock.state.bit.lock; //
//        
//        for( ; i<16; ) //��Ч����
//        {
//            *( msg + i++ ) = 0;
//        }
//    }else
//    if( ble_find_one_meg( msg, BLE_LOCK, len ) ) //����
//    {
//        debug_usr("BLE_LOCK\r\n");
//        
//        if( ble_check_token( msg+4, Ble.Token ) == 0 ) //У������
//        {
//            return 0; //У��ʧ��
//        }
//        
//        //���ɻ�Ӧ֡
//        *( msg + i++) = (uint8_t)( BLE_ACK_LOCK >> 8 ); //������
//        *( msg + i++) = (uint8_t)( BLE_ACK_LOCK ); //������
//        *( msg + i++) = 0x01; //LEN ������Ч�ֽ���
//        
//        //����
//        err = lock_lock( );
//        
//        *( msg + i++) = !err; //״̬���� 00 ��ʾ�����ɹ� //01 ��ʾ����ʧ�� �͹�������ʵ�ʷ���ֵ�෴ //!Lock.state.bit.lock; //
//        
//        for( ; i<16; ) //��Ч����
//        {
//            *( msg + i++ ) = 0;
//        }
//    }else
//    if( ble_find_one_meg( msg, BLE_GET_STA, len ) ) //��ѯ��״̬
//    {
//        debug_usr("BLE_GET_STA\r\n");
//        
//        if( ble_check_token( msg+4, Ble.Token ) == 0 ) //У������
//        {
//            return 0; //У��ʧ��
//        }
//        
//        //���ɻ�Ӧ֡
//        *( msg + i++) = (uint8_t)( BLE_ACK_STA >> 8 ); //������
//        *( msg + i++) = (uint8_t)( BLE_ACK_STA ); //������
//        *( msg + i++) = 0x01; //LEN ������Ч�ֽ���
//        *( msg + i++) = !Lock.state.bit.lock; //STA Ϊ��״̬�� 00 ��ʾ����״̬�� 01 ��ʾ�ر�״̬�� ����������Э���෴
//        
//        for( ; i<16; ) //��Ч����
//        {
//            *( msg + i++ ) = 0;
//        }
//    }else
//    if( ble_find_one_meg( msg, BLE_OLD_PWD, len ) ) //�޸����룬������
//    {
//        debug_usr("BLE_OLD_PWD\r\n");
//        
//        if( !Ble.lg.bit.pwd ) //��δ���չ�������
//        {
//            if( ble_check_token( msg+9, Ble.Token ) == 0 ) //У������
//            {
//                return 0; //У��ʧ��
//            }
//            for( n=0; n<6; n++ ) //�õ�������
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
//        }else //�Ѿ����չ������룬�ظ����մ�������
//        {
//            Ble.lg.bit.pwd = 0;
//        }
//        
//        
//        //Ӧ���볬ʱ����
//        //...
//        
//        return 0;//���������践��
//    }else
//    if( ble_find_one_meg( msg, BLE_NEW_PWD, len ) ) //�޸����룬������
//    {
//        debug_usr("BLE_NEW_PWD\r\n");
//        
//        if( Ble.lg.bit.pwd ) //������У��ɹ�
//        {
//            if( ble_check_token( msg+9, Ble.Token ) == 0 ) //У������
//            {
//                return 0; //У��ʧ��
//            }
//            
//            for( n=0; n<6; n++ ) //�õ�������
//            {
//                TEM[n] = *( msg + 3 + n );
//            }
//            
//            Ble.lg.bit.pwd = 0;
//            for( n=0; n<6; n++ ) //�õ�������
//            {
//                data_saved.data.PWD[ n ] = TEM[n];
//            }
//            
//            //save
//            SaveToFlash();
//            
//            err = 0;//�ɹ�
//            debug_usr("new pwd is setted.\r\n");
//        }else //������У��ʧ��
//        {
//            err = 1;
//            debug_usr("new pwd is error.\r\n");
//        }
//        
//        //���ɻ�Ӧ֡
//        *( msg + i++) = (uint8_t)( BLE_ACK_PWD >> 8 ); //������
//        *( msg + i++) = (uint8_t)( BLE_ACK_PWD ); //������
//        *( msg + i++) = 0x01; //LEN ������Ч�ֽ���
//        *( msg + i++) = err; //�޸Ľ�� 00 �ɹ� 01 ʧ��
//        
//        for( ; i<16; ) //��Ч����
//        {
//            *( msg + i++ ) = 0;
//        }
//    }else
//    if( ble_find_one_meg( msg, BLE_SET_KEYL, len ) ) //�޸���Կ������Կǰ 8 ���ֽ�
//    {
//        if( !Ble.lg.bit.key ) //��δ���չ�����Կǰ8���ֽ�
//        {
//            if( ble_check_token( msg+11, Ble.Token ) == 0 ) //У������
//            {
//                return 0; //У��ʧ��
//            }
//            for( n=0; n<8; n++ ) //�õ�����Կ
//            {
//                KEY_TEM[ n ]  = *( msg + 3 + n );
//            }
//            
//            Ble.lg.bit.key = 1;
//        
//            //Ӧ���볬ʱ����
//            //...
//        }else //���չ�����Կǰ8���ֽڣ��ظ����մ�������
//        {
//            Ble.lg.bit.key = 0;
//        }
//        return 0;//���������践��
//    }else
//    if( ble_find_one_meg( msg, BLE_SET_KEYH, len ) ) //�޸���Կ������Կ�� 8 ���ֽ�
//    {
//        if( Ble.lg.bit.key ) //�õ�����Կǰ8���ֽ�
//        {
//            Ble.lg.bit.key = 0;
//            
//            if( ble_check_token( msg+11, Ble.Token ) == 0 ) //У������
//            {
//                return 0; //У��ʧ��
//            }
//            
//            for( n=0; n<8; n++ ) //�õ�����Կ�� 8 ���ֽ�
//            {
//                KEY_TEM[ 8+n ]  = *( msg + 3 + n );
//            }
//            
//            for( n=0; n<16; n++ ) //�õ�����Կ�� 8 ���ֽ�
//            {
//                data_saved.data.KEY[ n ]  = KEY_TEM[n];
//            }
//            //save
//            SaveToFlash();
//            err = 0;
//            debug_usr("new key is setted.\r\n");
//            
//        }else //������У��ʧ��
//        {
//            err = 1;
//            debug_usr("new key is error.\r\n");
//        }
//        
//        //���ɻ�Ӧ֡
//        *( msg + i++) = (uint8_t)( BLE_ACK_KEY >> 8 ); //������
//        *( msg + i++) = (uint8_t)( BLE_ACK_KEY ); //������
//        *( msg + i++) = 0x01; //LEN ������Ч�ֽ���
//        *( msg + i++) = err; //�޸Ľ�� 00 �ɹ� 01 ʧ��
//        
//        for( ; i<16; ) //��Ч����
//        {
//            *( msg + i++ ) = 0;
//        }
//    }
////    else
////    if( ble_find_one_meg( msg, BLE_REPORT_STA, len ) )//�ϱ�״̬ 
////    {
////        ;
////    }
//    
//    if( i ) //���ͻ�Ӧ֡
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
//    if( Ble.lg.bit.reset ) //���Ĺ㲥���������Ӧ��ģ�鸴λ
//    {
//        Ble.lg.bit.reset = 0;
//        vTaskDelay( pdMS_TO_TICKS(2000) );
//        NVIC_SystemReset();//ϵͳ��λ
//    }
    
    return 1;//return i;
}

uint8_t BLE_ReportState( void )
{
    uint8_t i;
    
    i=0;
    
    //���ɻ�Ӧ֡
    BLE_TxBuf[ i++ ] = (uint8_t)( BLE_REPORT_STA >> 8 ); //������
    BLE_TxBuf[ i++ ] = (uint8_t)( BLE_REPORT_STA ) + 1; //������
    BLE_TxBuf[ i++ ] = 0x01; //LEN ������Ч�ֽ���
    BLE_TxBuf[ i++ ] = 0;//!Lock.state.bit.lock; //STA Ϊ��״̬�� 00 ��ʾ����״̬�� 01 ��ʾ�ر�״̬��
    
    for( ; i<16; i++ ) //��Ч����
    {
        BLE_TxBuf[ i++ ] = 0;
    }
    
    BLE_Send( BLE_TxBuf, i);
        
    return i;
}

/******************************************************* END OF FILE ************************************************************/

/*

�������ݣ�

��ȡ���ƣ�       
06 01 01 01 2D 1A 68 3D 48 27 1A 18 31 6E 47 1A
06 02 04 30 30 30 30 01 01 00 00 00 01 00 00 00 
�޸Ĺ㲥����: 04 01 N NAME[N] TOKEN[4] F[1]
              04 01 08 41 42 43 44 45 46 47 48 30 30 30 30 00
              04 01 08 4A 43 2D 47 36 32 30 31 30 30 30 30 00 //
��ȡ����:        
02 01 01 01 30 30 30 30 30 00 00 00 00 00 00 00
02 02 01 2C 00 00 00 00 00 00 00 00 00 00 00 00 
����: 05 01 06 PWD[6] TOKEN[4] FILL[3]
      05 01 06 30 30 30 30 30 30 30 30 30 30 00 00 00
      05 02 01 01 00 00 00 00 00 00 00 00 00 00 00 00
        
������05 0C 01 01 TOKEN[4] FILL[8]
      05 0C 01 01 30 30 30 30 00 00 00 00 00 00 00 00       
      
��ѯ��״̬��05 0E 01 01 TOKEN[4] FILL[12]
            05 0E 01 01 30 30 30 30 00 00 00 00 00 00 00 00
            05 0F 01 01 00 00 00 00 00 00 00 00 00 00 00 00
            
�޸����룺������������ 2 ��ͨ��֡������ֱ����Ҫ����һ�����ӳ٣�
          05 03 06 OLDPWD[6] TOKEN[4] FILL[3]
          05 04 06 NEWPWD[6] TOKEN[4] FILL[3]
          05 03 06 30 30 30 30 30 30 30 30 30 30 00 00 00
          05 04 06 30 30 30 30 30 30 30 30 30 30 00 00 00
          05 05 01 00 00 00 00 00 00 00 00 00 00 00 00 00   //���óɹ�
          05 05 01 01 00 00 00 00 00 00 00 00 00 00 00 00   //����ʧ��
          
�޸���Կ��
          07 01 08 KEYL[8] TOKEN[4] F[1]
          07 02 08 KEYH[8] TOKEN[4] F[1]
          07 01 08 38 38 38 38 38 38 38 38 30 30 30 30 00
          07 02 08 38 38 38 38 38 38 38 38 30 30 30 30 00
          
          07 03 01 00 00 00 00 00 00 00 00 00 00 00 00 00   //���óɹ�
          07 03 01 01 00 00 00 00 00 00 00 00 00 00 00 00   //����ʧ��
          
869976031496988
*/
