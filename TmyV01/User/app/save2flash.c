
/*
*********************************************************************************************************
*
*	模块名称 : IAP驱动模块
*	文件名称 : save2flash.c
*	版    本 : V1.0
*   M  C  U  : STM32F103RBT6
*   编 译 器 : IAR-Arm 8.22.2
*
*	说    明 : IAP做EEPROM
*              
*	修改记录 :
*		版本号  日期        作者     说明
*		V1.0    2018-05-01 robin   正式发布
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
* 函数名称: MemReadByte()
* 函数功能: 从存储器中读出num字节数据
* 入口参数: *dat:读出数据的保存地址
*                        num        :读出的字节数
* 出口参数: 0：操作失败；1：操作成功
* 使用说明: 无
* 创建日期: 2010年12月15日
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
* 函数名称: MemWriteByte()
* 函数功能: 向存储器中写入num字节数据
* 入口参数: *dat:待写入数据
*                        num        :写入的字节数
* 出口参数: 0：操作失败；1：操作成功
* 使用说明: 无
* 创建日期: 2010年12月15日
***************************************************************************************************/
uint8_t MemWriteByte(uint16_t *data,uint16_t num)                               
{
        FLASH_Status temp_stat;
        uint32_t temp_addr = PAGE_ADDR;
               
        FLASH_Unlock();     // Flash解锁，允许操作相关的寄存器
        temp_stat = FLASH_ErasePage(PAGE_ADDR); // 擦出制定的页
       
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
    
    //if( data_saved.data.lg.buf & SAVE_LG_MASK ) //初始化默认参数
    if( data_saved.data.lg.buf ) //初始化默认参数
    {
        //sprintf( data_saved.data.PWD,"%s","111111" ); //密码 
        sprintf( (char*)data_saved.data.PWD,"%s","000000" ); //密码
        sprintf( (char*)data_saved.data.TOKEN,"%s","0000" ); //令牌
        sprintf( (char*)data_saved.data.KEY,"%s","8888888888888888" ); //秘钥
        sprintf( (char*)data_saved.data.NAME,"%s","JC-G6201" ); //sprintf( data_saved.data.NAME,"%s","JC-G6201" ); //广播名称 
         
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
