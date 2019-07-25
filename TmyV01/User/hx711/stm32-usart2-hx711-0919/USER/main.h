#ifndef __MAIN_H
#define __MAIN_H
/*     函数调用    */
#include "stm32f10x.h"
#include "usart2.h"
#include "hx711.h"
#include "flash.h"
/***** 修正值 *****  2mV *（2^24） * 128 = 4294967296 *******/
/*************  1mV *（2^24） * 128 = 2147483648 *******/
#define SensorAccuracy2mV  429496.73
#define SensorAccuracy1mV  214748.36
/***** 串口接收指令 *****/
u8  function = 0; 
/***** 单位 *****/
u8 unit = 0;
/**** 输出重量 ****/
float weight;
/************* 实际重量 ****** 毛重 ******* 总重量 ****/
unsigned long actual_weight,rough_weight, first_weight;
 /*不是所有的传感器都那么准，所以不能是定值，需要校准*/
float Correction ;
/*******************************/
void  Weighing(void);
void  GetRoughWeight(void);
void  WeighingCorrection(void);
#endif
