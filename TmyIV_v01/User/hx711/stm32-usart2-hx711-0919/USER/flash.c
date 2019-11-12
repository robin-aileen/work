//读取指定地址的1字(32位数据)
//faddr:读地址(此地址必须为4的倍数!!)
//返回值:对应数据.
//存储原则 高地址存储高位
#include "flash.h"
u32 FlashReadWord(u32 faddr)
{
	return ( *(vu32*)faddr);
}
/*************  向 FLASH 写入数据 ****************/
void SaveFlashParameter(void)
{	
	FLASH_Unlock();//解锁
	FLASH_ErasePage( FLASH_SAVE_ADDR );//擦除
/************************ 向指定的地址（起始地址）,**** 写入指定的数据 ***********/
 	FLASH_ProgramWord( FLASH_SAVE_ADDR + FLASH_HX711_ADDR,Correction);
  FLASH_Lock();//上锁
}
/*********************** 获取 FLASH 指定地址的数据***********************/
void GetFlashParameter(void)
{
	Correction = FlashReadWord( FLASH_SAVE_ADDR + FLASH_HX711_ADDR );
}
