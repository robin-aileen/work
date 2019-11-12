#ifndef __MAIN_H
#define __MAIN_H
/*     ��������    */
#include "stm32f10x.h"
#include "usart2.h"
#include "hx711.h"
#include "flash.h"
/***** ����ֵ *****  2mV *��2^24�� * 128 = 4294967296 *******/
/*************  1mV *��2^24�� * 128 = 2147483648 *******/
#define SensorAccuracy2mV  429496.73
#define SensorAccuracy1mV  214748.36
/***** ���ڽ���ָ�� *****/
u8  function = 0; 
/***** ��λ *****/
u8 unit = 0;
/**** ������� ****/
float weight;
/************* ʵ������ ****** ë�� ******* ������ ****/
unsigned long actual_weight,rough_weight, first_weight;
 /*�������еĴ���������ô׼�����Բ����Ƕ�ֵ����ҪУ׼*/
float Correction ;
/*******************************/
void  Weighing(void);
void  GetRoughWeight(void);
void  WeighingCorrection(void);
#endif
