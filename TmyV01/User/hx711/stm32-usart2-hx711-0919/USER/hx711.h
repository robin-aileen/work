#ifndef __HX711_H
#define __HX711_H

#include "stm32f10x.h"
#include "usart2.h"

void GPIO_INIT(void);

unsigned long ReadCount(void);
unsigned long FilterCount(void);
void delay_10ms(unsigned char timer);

void delay_us(u32 i);
void delay_ms(u32 i);

#endif

