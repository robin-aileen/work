//��ȡָ����ַ��1��(32λ����)
//faddr:����ַ(�˵�ַ����Ϊ4�ı���!!)
//����ֵ:��Ӧ����.
//�洢ԭ�� �ߵ�ַ�洢��λ
#include "flash.h"
u32 FlashReadWord(u32 faddr)
{
	return ( *(vu32*)faddr);
}
/*************  �� FLASH д������ ****************/
void SaveFlashParameter(void)
{	
	FLASH_Unlock();//����
	FLASH_ErasePage( FLASH_SAVE_ADDR );//����
/************************ ��ָ���ĵ�ַ����ʼ��ַ��,**** д��ָ�������� ***********/
 	FLASH_ProgramWord( FLASH_SAVE_ADDR + FLASH_HX711_ADDR,Correction);
  FLASH_Lock();//����
}
/*********************** ��ȡ FLASH ָ����ַ������***********************/
void GetFlashParameter(void)
{
	Correction = FlashReadWord( FLASH_SAVE_ADDR + FLASH_HX711_ADDR );
}
