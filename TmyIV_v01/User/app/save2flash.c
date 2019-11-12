
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

#define	ONE_DATA_SIZE	    10

//STM32F051K8:64K FLASH | 8K RAM
//STM32F072CBT6:128 FLASH | 16K RAM

#define	BYTE_COMP ( (STM32_SECTOR_SIZE%ONE_DATA_SIZE)/2 ) //Byte complement �貹����ֽڣ�����Ϊ2����������,ת����ƫ�Ƶ�ַ����2

uint8_t Msg_Write( uint32_t WriteAddr, uint16_t * pBuffer, uint16_t NumToWrite );
void Msg_Read( uint32_t ReadAddr, uint8_t * pBuffer, uint16_t NumToRead );
uint8_t Msg_WriteNoCheck ( uint32_t WriteAddr, uint16_t * pBuffer, uint16_t NumToWrite );

uint16_t VirtAddVarTab[NB_OF_VAR] = {0, 1};
//uint16_t VarDataTab[NB_OF_VAR] = {0, 0, 0};

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
        uint16_t *temp_addr = (uint16_t *)EEPROM1_START_ADDR;
       
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
        uint32_t temp_addr = EEPROM1_START_ADDR;
               
        FLASH_Unlock();     // Flash���������������صļĴ���
        temp_stat = FLASH_ErasePage(EEPROM1_START_ADDR); // �����ƶ���ҳ
       
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
	
	//20181219���룬���㷨��ֻҪ����д���ָ�룬�Ͷ����㣬��ֹ��ָ��ָ��Ƿ�����
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
	
	Msg_Read( EEPROM3_START_ADDR + MsgSaveAddr.read_addr_offset, data, len);//������������������
	
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
* @Description : ��ָ����ַ��ʼд��ָ�����ȵ�����
* @Argument(s) : WriteAddr				�ڲ�flash��ַ
*								 pBuffer					Ҫд�������ݴ��buf
*								 NumToWrite				д�����ݵĳ���
* @Return(s)   : NONE
* @Caller(s)   : Application.
* @Note(s)     : none.
**********************************************************************************************************/
uint8_t Msg_Write( uint32_t WriteAddr, uint16_t * pBuffer, uint16_t NumToWrite )
{
	uint16_t secoff;	   					//������ƫ�Ƶ�ַ(16λ�ּ���)
 	//uint16_t i;    
	uint32_t secpos;	   					//������ַ
	uint32_t offaddr;   					//ȥ��0X08000000��ĵ�ַ
	uint8_t temp_stat;
	
	if( (WriteAddr < FLASH_BASE) || ( WriteAddr >= ( FLASH_BASE + 1024 * STM32_FLASH_SIZE ) ) )
		return 0;											//�Ƿ���ַ
	
	FLASH_Unlock();
	
	offaddr = WriteAddr - FLASH_BASE;						    //ʵ��ƫ�Ƶ�ַ.
	secpos = offaddr / STM32_SECTOR_SIZE;					    //������ַ  0~127 for STM32F103RBT6
	secoff = ( offaddr % STM32_SECTOR_SIZE ) / 2;				//�������ڵ�ƫ��(2���ֽ�Ϊ������λ.)
	
	//debug_usr("[ 1 ]\r\n");
	//Msg_Read( secpos * STM32_SECTOR_SIZE + FLASH_BASE, STMFLASH_BUF, STM32_SECTOR_SIZE / 2);//������������������
	
	if(!secoff)
	{
		temp_stat = FLASH_ErasePage(secpos * STM32_SECTOR_SIZE + FLASH_BASE);
	
		if(temp_stat != FLASH_COMPLETE)
		{
				FLASH_Lock();
				return 0;
		}
	}

	/*for(i=0;i < NumToWrite;i++)//����
	{
		STMFLASH_BUF[i+secoff]=pBuffer[i];	  
	}*/
	/*
	while( !Msg_WriteNoCheck(WriteAddr, pBuffer, NumToWrite) )
	{
		uint8 i = 10;
		Msg_Read( secpos * STM32_SECTOR_SIZE + FLASH_BASE, STMFLASH_BUF, STM32_SECTOR_SIZE / 2);
		while( (FLASH_ErasePage(secpos * STM32_SECTOR_SIZE + FLASH_BASE) != FLASH_COMPLETE) && (i--) );
		for(i=0;i < NumToWrite;i++)//����
		{
			STMFLASH_BUF[i+secoff]=pBuffer[i];	  
		}
		
	}
	*/
	//temp_stat = Msg_WriteNoCheck(WriteAddr, pBuffer, NumToWrite);
	temp_stat = Msg_WriteNoCheck(WriteAddr+BYTE_COMP*(secpos+1), pBuffer, NumToWrite); //ÿ��д��ƫ��n�ֽ���ҳ����
	//temp_stat = Msg_WriteNoCheck(secpos * STM32_SECTOR_SIZE + FLASH_BASE, STMFLASH_BUF, (secoff+NumToWrite));//д�Ѿ������˵�,ֱ��д������ʣ������. 				   

	if(temp_stat != 1)	//дʧ�ܣ��˵�ַ�Ѿ�������
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
	
    //uint16_t secoff;	   					//������ƫ�Ƶ�ַ(16λ�ּ���)
	uint32_t secpos;	   					//������ַ
	uint32_t offaddr;   					//ȥ��0X08000000��ĵ�ַ
	//uint8_t temp_stat;
	
//	if( (ReadAddr < FLASH_BASE) || ( ReadAddr >= ( FLASH_BASE + 1024 * STM32_FLASH_SIZE ) ) )
//		return 0;											//�Ƿ���ַ
	
	FLASH_Unlock();
	
	offaddr = ReadAddr - FLASH_BASE;						    //ʵ��ƫ�Ƶ�ַ.
	secpos = offaddr / STM32_SECTOR_SIZE;					    //������ַ  0~127 for STM32F103RBT6
	//secoff = ( offaddr % STM32_SECTOR_SIZE ) / 2;				//�������ڵ�ƫ��(2���ֽ�Ϊ������λ.)
	
	
	temAdd = ReadAddr + BYTE_COMP*(secpos+1);
	
	
	for(i=0;i<NumToRead;)
	{
		
		tem16 = *(__IO uint16_t*)temAdd;//��ȡ2���ֽ�.
		temAdd+=2;							//ƫ��2���ֽ�.	
		
		pBuffer[i++] = (uint8_t)tem16;
		pBuffer[i++] = (uint8_t)(tem16>>8);
	}
}


	
/**********************************************************************************************************
*                                      BSP_STMFLASH_WriteNoCheck()
*
* @Description : ������д��
* @Argument(s) : WriteAddr				�ڲ�flash��ַ
*								 pBuffer					�������ݴ����buf
*								 NumToWrite				�������ݵĳ���
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
