#ifndef __FLASH_H
#define __FLASH_H
#include "stm32f10x.h"
#include "stm32f10x_flash.h"
extern float Correction ;
//FLASH��ʼ��ַ
#define  STM32_FLASH_BASE  0x08000000 	//STM32 FLASH����ʼ��ַ
//FLASH  �������濪ʼ��ַ
#define  FLASH_SAVE_ADDR   0x08000000 + 1024 * 32  //����FLASH �����ַ(����Ϊż��)

#define  FLASH_HX711_ADDR	 8 * 0 
void SaveFlashParameter(void);
void GetFlashParameter(void);
u32 FlashReadWord(u32 faddr);
#endif
