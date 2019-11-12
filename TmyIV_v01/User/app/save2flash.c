
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

#define	ONE_DATA_SIZE	    10

//STM32F051K8:64K FLASH | 8K RAM
//STM32F072CBT6:128 FLASH | 16K RAM

#define	BYTE_COMP ( (STM32_SECTOR_SIZE%ONE_DATA_SIZE)/2 ) //Byte complement 需补齐的字节（必须为2的整数倍）,转换成偏移地址除以2

uint8_t Msg_Write( uint32_t WriteAddr, uint16_t * pBuffer, uint16_t NumToWrite );
void Msg_Read( uint32_t ReadAddr, uint8_t * pBuffer, uint16_t NumToRead );
uint8_t Msg_WriteNoCheck ( uint32_t WriteAddr, uint16_t * pBuffer, uint16_t NumToWrite );

uint16_t VirtAddVarTab[NB_OF_VAR] = {0, 1};
//uint16_t VarDataTab[NB_OF_VAR] = {0, 0, 0};

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
        uint16_t *temp_addr = (uint16_t *)EEPROM1_START_ADDR;
       
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
        uint32_t temp_addr = EEPROM1_START_ADDR;
               
        FLASH_Unlock();     // Flash解锁，允许操作相关的寄存器
        temp_stat = FLASH_ErasePage(EEPROM1_START_ADDR); // 擦出制定的页
       
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
    
    //ENABLE_INT();
}

uint8_t ReadFromFlash( void )
{
//	uint8_t i;
	
    MemReadByte( data_saved.buf, FLASH_DATA_SIZE );
    
//    if( data_saved.data.lg.bit.v_init == 1 )
//    {
//        data_saved.data.lg.bit.v_init = 0;
//        data_saved.data.max_room_n = MAX_ROOM_N;
//        data_saved.data.TrainNum[0] = TRAIN_NUM_H;
//        data_saved.data.TrainNum[1] = TRAIN_NUM_L;
//        data_saved.data.dly_move = DLY_MOVE;
//        data_saved.data.dly_stop = DLY_STOP;
//        data_saved.data.TrainType = TRAIN_TPYE;
//        SaveToFlash( );
//    }
//    
//    Train.TrainType = data_saved.data.TrainType;
//    
//    for( i=0; i<2; i++ )
//    {
//        Train.TrainNum[i] = data_saved.data.TrainNum[i];
//    }
//    
//    Train.max_room_n = data_saved.data.max_room_n;
//    
//    if( Train.max_room_n > MAX_ROOM_N )
//    {
//        Train.max_room_n = MAX_ROOM_N;
//    }
//    
    return 1;
}

uint8_t SaveMsgToFlash( uint8_t *data, uint8_t len )
{
	uint8_t i;
	uint8_t msgbuf[16];
	uint8_t tmp_state;
	uint32_t temp_addr = EEPROM3_START_ADDR + MsgSaveAddr.write_addr_offset;
	
	if( MsgSaveAddr.write_addr_offset >= EEPROM3_MAX_ADDR_OFFSET )
	{
		temp_addr = EEPROM3_START_ADDR;
		MsgSaveAddr.write_addr_offset = 0;
		Bkd.write_addr_offset = MsgSaveAddr.write_addr_offset;
		bkd_save();
	}
	
	for(i=0; i<len; i++)
	{
	    msgbuf[i] = *(data+i);
	}
	
	__disable_irq();
	
	tmp_state = Msg_Write( temp_addr, (uint16_t *)msgbuf, len/2);
	
	__enable_irq();
	
	if( tmp_state == 0 )
	{
		debug_usr("[ Generate Msg Error ]\r\n");
	}

		MsgSaveAddr.write_addr_offset += len;
		Bkd.write_addr_offset = MsgSaveAddr.write_addr_offset;
		bkd_save();
		
	return 1;
}


uint8_t ReadMsgToRam( uint8_t *data, uint8_t len )
{
	__disable_irq();
	
	//20181219加入，简化算法，只要大于写入的指针，就都清零，防止读指针指向非法数据
	if( MsgSaveAddr.read_addr_offset >= MsgSaveAddr.write_addr_offset )
	{
	    MsgSaveAddr.read_addr_offset = 0;
	    MsgSaveAddr.write_addr_offset = 0;
	    
	    Bkd.read_addr_offset = 0;
	    Bkd.write_addr_offset = 0;
	    bkd_save();
	    
	    return 0;
	}
	
	if( MsgSaveAddr.read_addr_offset >= EEPROM3_MAX_ADDR_OFFSET )
	{
		MsgSaveAddr.read_addr_offset = 0;
		//data_saved.data.write_addr_offset = MsgSaveAddr.write_addr_offset;
		//data_saved.data.read_addr_offset 
		Bkd.read_addr_offset = MsgSaveAddr.read_addr_offset;
		bkd_save();
		
		return 0;
	}
	
	FLASH_Unlock();
	
	Msg_Read( EEPROM3_START_ADDR + MsgSaveAddr.read_addr_offset, data, len);//读出整个扇区的内容
	
	FLASH_Lock();
	MsgSaveAddr.read_addr_offset += len;
	//data_saved.data.write_addr_offset = MsgSaveAddr.write_addr_offset;
	//data_saved.data.read_addr_offset 
	Bkd.read_addr_offset = MsgSaveAddr.read_addr_offset;
	bkd_save();
	
	__enable_irq();
	
	return len;
}

/**********************************************************************************************************
*                                      BSP_STMFLASH_Write()
*
* @Description : 从指定地址开始写入指定长度的数据
* @Argument(s) : WriteAddr				内部flash地址
*								 pBuffer					要写入数据暂存的buf
*								 NumToWrite				写入数据的长度
* @Return(s)   : NONE
* @Caller(s)   : Application.
* @Note(s)     : none.
**********************************************************************************************************/
uint8_t Msg_Write( uint32_t WriteAddr, uint16_t * pBuffer, uint16_t NumToWrite )
{
	uint16_t secoff;	   					//扇区内偏移地址(16位字计算)
 	//uint16_t i;    
	uint32_t secpos;	   					//扇区地址
	uint32_t offaddr;   					//去掉0X08000000后的地址
	uint8_t temp_stat;
	
	if( (WriteAddr < FLASH_BASE) || ( WriteAddr >= ( FLASH_BASE + 1024 * STM32_FLASH_SIZE ) ) )
		return 0;											//非法地址
	
	FLASH_Unlock();
	
	offaddr = WriteAddr - FLASH_BASE;						    //实际偏移地址.
	secpos = offaddr / STM32_SECTOR_SIZE;					    //扇区地址  0~127 for STM32F103RBT6
	secoff = ( offaddr % STM32_SECTOR_SIZE ) / 2;				//在扇区内的偏移(2个字节为基本单位.)
	
	//debug_usr("[ 1 ]\r\n");
	//Msg_Read( secpos * STM32_SECTOR_SIZE + FLASH_BASE, STMFLASH_BUF, STM32_SECTOR_SIZE / 2);//读出整个扇区的内容
	
	if(!secoff)
	{
		temp_stat = FLASH_ErasePage(secpos * STM32_SECTOR_SIZE + FLASH_BASE);
	
		if(temp_stat != FLASH_COMPLETE)
		{
				FLASH_Lock();
				return 0;
		}
	}

	/*for(i=0;i < NumToWrite;i++)//复制
	{
		STMFLASH_BUF[i+secoff]=pBuffer[i];	  
	}*/
	/*
	while( !Msg_WriteNoCheck(WriteAddr, pBuffer, NumToWrite) )
	{
		uint8 i = 10;
		Msg_Read( secpos * STM32_SECTOR_SIZE + FLASH_BASE, STMFLASH_BUF, STM32_SECTOR_SIZE / 2);
		while( (FLASH_ErasePage(secpos * STM32_SECTOR_SIZE + FLASH_BASE) != FLASH_COMPLETE) && (i--) );
		for(i=0;i < NumToWrite;i++)//复制
		{
			STMFLASH_BUF[i+secoff]=pBuffer[i];	  
		}
		
	}
	*/
	//temp_stat = Msg_WriteNoCheck(WriteAddr, pBuffer, NumToWrite);
	temp_stat = Msg_WriteNoCheck(WriteAddr+BYTE_COMP*(secpos+1), pBuffer, NumToWrite); //每次写入偏移n字节以页对齐
	//temp_stat = Msg_WriteNoCheck(secpos * STM32_SECTOR_SIZE + FLASH_BASE, STMFLASH_BUF, (secoff+NumToWrite));//写已经擦除了的,直接写入扇区剩余区间. 				   

	if(temp_stat != 1)	//写失败，此地址已经有数据
	{
		FLASH_Lock();
		return 0;
	}

	FLASH_Lock();

	return 1;
}

void Msg_Read( uint32_t ReadAddr, uint8_t * pBuffer, uint16_t NumToRead )
{
	uint16_t i;
	uint16_t tem16;
	uint32_t temAdd;
	
    //uint16_t secoff;	   					//扇区内偏移地址(16位字计算)
	uint32_t secpos;	   					//扇区地址
	uint32_t offaddr;   					//去掉0X08000000后的地址
	//uint8_t temp_stat;
	
//	if( (ReadAddr < FLASH_BASE) || ( ReadAddr >= ( FLASH_BASE + 1024 * STM32_FLASH_SIZE ) ) )
//		return 0;											//非法地址
	
	FLASH_Unlock();
	
	offaddr = ReadAddr - FLASH_BASE;						    //实际偏移地址.
	secpos = offaddr / STM32_SECTOR_SIZE;					    //扇区地址  0~127 for STM32F103RBT6
	//secoff = ( offaddr % STM32_SECTOR_SIZE ) / 2;				//在扇区内的偏移(2个字节为基本单位.)
	
	
	temAdd = ReadAddr + BYTE_COMP*(secpos+1);
	
	
	for(i=0;i<NumToRead;)
	{
		
		tem16 = *(__IO uint16_t*)temAdd;//读取2个字节.
		temAdd+=2;							//偏移2个字节.	
		
		pBuffer[i++] = (uint8_t)tem16;
		pBuffer[i++] = (uint8_t)(tem16>>8);
	}
}


	
/**********************************************************************************************************
*                                      BSP_STMFLASH_WriteNoCheck()
*
* @Description : 不检查的写入
* @Argument(s) : WriteAddr				内部flash地址
*								 pBuffer					读出数据存入的buf
*								 NumToWrite				读出数据的长度
* @Return(s)   : NONE
* @Caller(s)   : Application.
* @Note(s)     : none.
**********************************************************************************************************/
uint8_t Msg_WriteNoCheck ( uint32_t WriteAddr, uint16_t * pBuffer, uint16_t NumToWrite )   
{
	uint16_t i;
	FLASH_Status status;
	
	for(i=0;i<NumToWrite;i++)
	{
		status = FLASH_ProgramHalfWord( WriteAddr, pBuffer[i]);
		if(status != FLASH_COMPLETE)
		{
						debug_usr("[ SAVE ERROR ]\r\n");
						return 0;
		}
	  WriteAddr+=2;
	}
	return 1;
} 


uint16_t msg_get_num( void )
{
	uint16_t tem16;
	
	tem16 = MsgSaveAddr.write_addr_offset - MsgSaveAddr.read_addr_offset;
	
	return tem16;
}

uint8_t bkd_save( void )
{
    //Bkd.write_addr_offset = 0x55;
    //Bkd.read_addr_offset  = 0xAA;
    
    EE_WriteVariable( EEPROM2_W_OFFSET, Bkd.write_addr_offset );
    EE_WriteVariable( EEPROM2_R_OFFSET, Bkd.read_addr_offset );
    
    return 1;
}

uint8_t bkd_read( void )
{
    EE_ReadVariable( EEPROM2_W_OFFSET, &Bkd.write_addr_offset );
    EE_ReadVariable( EEPROM2_R_OFFSET, &Bkd.read_addr_offset );
    
    printf("offset: %02X %02X\r\n", Bkd.write_addr_offset, Bkd.read_addr_offset);
    
    return 1;
}


/******************************************************* END OF FILE ************************************************************/
