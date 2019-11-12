#ifndef __FLASH_H
#define __FLASH_H
#include "stm32f10x.h"
#include "stm32f10x_flash.h"
extern float Correction ;
//FLASH起始地址
#define  STM32_FLASH_BASE  0x08000000 	//STM32 FLASH的起始地址
//FLASH  参数保存开始地址
#define  FLASH_SAVE_ADDR   0x08000000 + 1024 * 32  //设置FLASH 保存地址(必须为偶数)

#define  FLASH_HX711_ADDR	 8 * 0 
void SaveFlashParameter(void);
void GetFlashParameter(void);
u32 FlashReadWord(u32 faddr);
#endif
