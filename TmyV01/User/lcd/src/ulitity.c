/************************************版权申明********************************************
**                             广州大彩光电科技有限公司
**                             http://www.gz-dc.com
**-----------------------------------文件信息--------------------------------------------
** 文件名称:   ulitity.c
** 修改时间:   2018-05-18
** 文件说明:   用户MCU串口驱动函数库
** 技术支持：  Tel: 020-82186683  Email: hmi@gz-dc.com Web:www.gz-dc.com
--------------------------------------------------------------------------------------*/
#include "ulitity.h"

extern volatile  uint32 timer_tick_count;



/*!
*   \brief  延时n毫秒
*   \param  n-延时时间
*/
void delay_ms(uint32 delay)
{
    //uint32 tick = timer_tick_count;
    ;//vTaskDelay(delay);
    
//    while(1)
//    {
//        if(timer_tick_count-tick>delay/10)
//            break;
//    }
}
/*!
*   \brief  节拍定时器 10ms
*/
void systicket_init(void)
{
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
    SysTick_Config(48000000/100);                              //一秒钟进入100次中断
}
