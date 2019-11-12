
/*
*********************************************************************************************************
*
*	ģ������ : IAP����ģ��
*	�ļ����� : save2flash.c
*	��    �� : V1.0
*   M  C  U  : STM32F103RBT6
*   �� �� �� : IAR-Arm 8.22.2
*
*	˵    �� : IAP��EEPROM
*              
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*		V1.0    2018-05-01 robin   ��ʽ����
*
*	Copyright (C), 2018-2028
*
*********************************************************************************************************
*/

#define SAVE2FLASH_GLOBAL 

#include "includes.h"//#include "bsp.h"
//----------------------------------------------------------------------------------------------------------------------
//
//STM32F051K8:64K FLASH | 8K RAM
#define EEPROM_SIZE     1   //K
#define MCU_FLASH_SIZE  64  //K

#define PAGE_ADDR   (0x08000000 + (MCU_FLASH_SIZE-EEPROM_SIZE) * 1024)


/***************************************************************************************************
* ��������: MemReadByte()
* ��������: �Ӵ洢���ж���num�ֽ�����
* ��ڲ���: *dat:�������ݵı����ַ
*                        num        :�������ֽ���
* ���ڲ���: 0������ʧ�ܣ�1�������ɹ�
* ʹ��˵��: ��
* ��������: 2010��12��15��
***************************************************************************************************/
uint8_t MemReadByte(uint16_t *data,uint16_t num)                               
{
        uint16_t *temp_addr = (uint16_t *)PAGE_ADDR;
       
        while(num --)
        {
                *data ++ = *temp_addr ++;
        }
                                                                                                       
        return 1;                                                                                                       
}

/***************************************************************************************************
* ��������: MemWriteByte()
* ��������: ��洢����д��num�ֽ�����
* ��ڲ���: *dat:��д������
*                        num        :д����ֽ���
* ���ڲ���: 0������ʧ�ܣ�1�������ɹ�
* ʹ��˵��: ��
* ��������: 2010��12��15��
***************************************************************************************************/
uint8_t MemWriteByte(uint16_t *data,uint16_t num)                               
{
        FLASH_Status temp_stat;
        uint32_t temp_addr = PAGE_ADDR;
               
        FLASH_Unlock();     // Flash���������������صļĴ���
        temp_stat = FLASH_ErasePage(PAGE_ADDR); // �����ƶ���ҳ
       
        if(temp_stat != FLASH_COMPLETE)
        {
                FLASH_Lock();
                return 0;
        }
       
        while(num --)
        {
                temp_stat = FLASH_ProgramHalfWord(temp_addr,*data);
                if(temp_stat != FLASH_COMPLETE)
                {
                        FLASH_Lock();
                        return 0;
                }
       
                temp_addr += 2;
                data++;
        }
       
        FLASH_Lock();       
        return 1;
}

void SaveToFlash( void )
{
    //DISABLE_INT();
			
    MemWriteByte( data_saved.buf, FLASH_DATA_SIZE );
    
    //delay_ms(10);
    
    //ENABLE_INT();
}


uint8_t ReadFromFlash( void )
{
//	uint16_t i, n;
	
    MemReadByte( data_saved.buf, FLASH_DATA_SIZE );
    
    //if( data_saved.data.lg.buf & SAVE_LG_MASK ) //��ʼ��Ĭ�ϲ���
    if( data_saved.data.lg.buf ) //��ʼ��Ĭ�ϲ���
    {
        //sprintf( data_saved.data.PWD,"%s","111111" ); //���� 
        sprintf( (char*)data_saved.data.PWD,"%s","000000" ); //����
        sprintf( (char*)data_saved.data.TOKEN,"%s","0000" ); //����
        sprintf( (char*)data_saved.data.KEY,"%s","8888888888888888" ); //��Կ
        sprintf( (char*)data_saved.data.NAME,"%s","JC-G6201" ); //sprintf( data_saved.data.NAME,"%s","JC-G6201" ); //�㲥���� 
         
        if( data_saved.data.lg.bit.ip == 1 )
        { //"180.101.147.115", "5683"
            
            //sprintf( Nb.IP,"%s","180.101.147.115" );
            //sprintf( Nb.PORT,"%s", "5683" ); 
            
            sprintf( data_saved.data.IP,"%s","180.101.147.115" );
            sprintf( data_saved.data.PORT,"%s", "5683" ); 
            
            data_saved.data.lg.bit.ip = 0;
        }
        
        if( data_saved.data.lg.bit.EnKey == 1 )
        {
            sprintf( (char*)data_saved.data.EncKey,"%s","JCKJ" );
            data_saved.data.lg.bit.EnKey = 0;
        }
        
        if( data_saved.data.lg.bit.time_wake == 1 )
        {
            data_saved.data.time_wake = TIME_LOCK_WAKE;
            data_saved.data.lg.bit.time_wake = 0;
        }
        
        if( data_saved.data.lg.bit.time_sleep == 1 )
        {
            data_saved.data.time_sleep = TIME_LOCK_SLEEP;
            data_saved.data.lg.bit.time_sleep = 0;
        }
        
        if( data_saved.data.lg.bit.threshold_lowpower == 1 )
        {
            data_saved.data.threshold_lowpower = THR_LOWPOWER;
            data_saved.data.lg.bit.threshold_lowpower = 0;
        }
        
        data_saved.data.lg.buf = 0;
        SaveToFlash( );
    }
//    sprintf( Nb.IP,"%s", data_saved.data.IP );
//    sprintf( Nb.PORT,"%s", data_saved.data.PORT );
    
    sprintf( Ble.Name,"%s", data_saved.data.NAME ); //
    sprintf( (char*)Ble.Token,"%s", data_saved.data.TOKEN ); //
    
    data_saved.data.time_sleep = 12;
    //data_saved.data.time_heart = 10;
    
//    timer_wake = data_saved.data.time_wake;
//    timer_sleep = data_saved.data.time_sleep;
    //timer_heart = data_saved.data.time_heart;
    //nb_usr_set_sleep_time( TIME_NB_SLEEP );
        
           
    return 1;
}


/******************************************************* END OF FILE ************************************************************/
