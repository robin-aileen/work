/************************************��Ȩ����********************************************
**                             ���ݴ�ʹ��Ƽ����޹�˾
**                             http://www.gz-dc.com
**-----------------------------------�ļ���Ϣ--------------------------------------------
** �ļ�����:   ulitity.c
** �޸�ʱ��:   2018-05-18
** �ļ�˵��:   �û�MCU��������������
** ����֧�֣�  Tel: 020-82186683  Email: hmi@gz-dc.com Web:www.gz-dc.com
--------------------------------------------------------------------------------------*/
#include "ulitity.h"

extern volatile  uint32 timer_tick_count;



/*!
*   \brief  ��ʱn����
*   \param  n-��ʱʱ��
*/
//void delay_ms(uint32 delay)
//{
//    //uint32 tick = timer_tick_count;
//    ;//vTaskDelay(delay);
//    
////    while(1)
////    {
////        if(timer_tick_count-tick>delay/10)
////            break;
////    }
//}
/*!
*   \brief  ���Ķ�ʱ�� 10ms
*/
void systicket_init(void)
{
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
    SysTick_Config(48000000/100);                              //һ���ӽ���100���ж�
}