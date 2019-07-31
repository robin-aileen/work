
/*
************************************************************************************************************
*
*	ģ������ : BLE����ģ��
*	�ļ����� : BLE.h
*	��    �� : V1.0
*   M  C  U  : STM32F103RBT6
*   �� �� �� : IAR-Arm 8.22.2
*
*	˵    �� : BLEM����ģ��,GPRS+GNSS,��Ҫ��һЩIO���ƣ�GNSSЭ�������AT���̷ŵ�����������
*
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*		V1.0    2018-05-15 robin   ��ʽ����
*
*	Copyright (C), 2018-2028
*
************************************************************************************************************
*/


#ifndef _BLE_H
#define _BLE_H

#ifndef BLE_GLOBAL

#define BLE_EXT extern
#else
#define BLE_EXT
#endif

#define BLE_COM COM4

#define BLE_TX_BUF_SIZE 100
#define BLE_RX_BUF_SIZE 100

static uint8_t BLE_TxBuf[ BLE_TX_BUF_SIZE ];		//���ͻ�����
static uint8_t BLE_RxBuf[ BLE_RX_BUF_SIZE ];		//���ջ�����

#define RCC_ALL_BLE 	(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB)

#define GPIO_PORT_BLE_WAKE      GPIOB
#define GPIO_PIN_BLE_WAKE       GPIO_Pin_7

#define GPIO_PORT_BLE_RESET     GPIOB
#define GPIO_PIN_BLE_RESET      GPIO_Pin_6

#define GPIO_PORT_BLE_LINK     GPIOA
#define GPIO_PIN_BLE_LINK      GPIO_Pin_15

#define BLE_PEDAL_UP      0xAA
#define BLE_PEDAL_DOWN    0x55


// #define BLE_P_HEAD 0x06

// #define BLE_GET_TOKEN   0x0601 //��ȡ����
// #define BLE_ACK_TOKEN   (BLE_GET_TOKEN+1) //��ȡ���ƣ���Ӧ

// #define BLE_SET_NAME    0x0401 //�޸Ĺ㲥����
// #define BLE_ACK_NAME   (BLE_SET_NAME+1) //��ȡ��������Ӧ

// #define BLE_GET_POWER   0x0201 //��ȡ����
// #define BLE_ACK_POWER   (BLE_GET_POWER+1) //��ȡ��������Ӧ

// #define BLE_UNLOCK      0x0501 //����
// #define BLE_ACK_UNLOCK  (BLE_UNLOCK+1) //��������Ӧ

// #define BLE_LOCK        0x050C //����
// #define BLE_ACK_LOCK    (BLE_LOCK+1) //��������Ӧ

// #define BLE_GET_STA     0x050E //��ѯ��״̬
// #define BLE_ACK_STA    (BLE_GET_STA+1) //��ѯ��״̬����Ӧ

// #define BLE_REPORT_STA  0x0508 //�ϱ�״̬

// #define BLE_OLD_PWD     0x0503 //�޸����룬������
// #define BLE_NEW_PWD     0x0504 //�޸����룬������
// #define BLE_ACK_PWD     (BLE_NEW_PWD+1) //�޸����룬��Ӧ

// #define BLE_SET_KEYL    0x0701 //�޸���Կ������Կǰ 8 ���ֽ�
// #define BLE_SET_KEYH    0x0702 //�޸���Կ������Կ�� 8 ���ֽ�
// #define BLE_ACK_KEY     0x0703 //�޸���Կ����Ӧ

// #define BLE_MODE_DATA   0 //͸��ģʽ
// #define BLE_MODE_AT     1 //ָ��ģʽ
// //Ble.state.bit.mode = BLE_MODE_AT;

#define BLE_UUID_SERVER 
enum BLE_ATTYPE{
    AT_MODE, DATA_MODE
};

typedef struct
{
    uint8_t link:1;
    uint8_t mode:1; //0:͸��ģʽ 1:����ģʽ
    uint8_t sleep:1; //����
    uint8_t wake:1; //����
    uint8_t go_to_sleep:1; //��Ҫ����
    uint8_t go_to_wake:1; //��Ҫ����
	uint8_t :2;

}TYPE_BLE_STATE_BIT;

typedef union
{
    TYPE_BLE_STATE_BIT bit;
    uint8_t all;
}TYPE_BLE_STATE;


typedef struct
{
    uint8_t pwd     :1; //�޸�����
    uint8_t key     :1; //�޸���Կ
    uint8_t reset   :1; //��λ��־    
	uint8_t         :5;

}TYPE_LG_BIT;

typedef union
{
    TYPE_LG_BIT bit;
    uint8_t all;
}TYPE_LG;

#define BLE_TIME_NODATA 30 //BLEû�����ݼ�ʱ

typedef struct
{
    TYPE_BLE_STATE state;
    TYPE_LG lg;
    uint8_t Token[5];
    uint8_t Mac[13]; //12λ
    char Name[9]; //NAME Ϊ�㲥���ƣ����֧�� 8 �� ASCII �ַ�
    uint8_t timer_ble_nodata;
}TYPE_BLE;

BLE_EXT TYPE_BLE Ble;


//-----------------------------------------ģ������-------------------------------------------------


//enum BLE_ATTYPE{
//    POWON,
//    AT, CGSN,/* BLEAND, */NUESTATS, CFUN0, NCSEARFCN, NCDP, CFUN,  NCCID, CGDCONT, CPSMS, CGATT, NNMI,
//    NSMI, CSQ_INIT, CCLK, CGPADDR, CSQ, NQMGR, NMGR, CHKSEND, NMGS,
//    BLETXDTASK_STANDBY, GPRS_OFFLINE, QICLOSE, BLE_IDLE,
//    HWRESET, SHUTDOWN
//};
//

/* ���ⲿ���õĺ������� */
BLE_EXT void BLE_Init( void );
BLE_EXT void BLE_HwReset(void);
BLE_EXT void BLE_WakeUp(void);
BLE_EXT uint8_t BLE_Sleep(void);

BLE_EXT uint8_t BLE_GetMsg( void );//uint8_t BLE_GetMsg( uint8_t *msg, uint16_t len );
BLE_EXT uint8_t BLE_Pares( uint8_t *msg, uint8_t len );
BLE_EXT uint8_t BLE_ReportState( void );
BLE_EXT uint8_t BLE_PapaInit( void );
BLE_EXT void BLE_CheckState( void );

#endif

/******************************************************* END OF FILE ************************************************************/
