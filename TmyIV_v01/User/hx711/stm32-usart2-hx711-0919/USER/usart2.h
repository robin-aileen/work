#ifndef __USART2_H
#define	__USART2_H

#include "stm32f10x.h"
#include "stdarg.h"
#include "stdio.h"
#include "misc.h"
void USART2_Config(void);
int fputc(int ch, FILE *f);
void USART2_printf(USART_TypeDef* USARTx, uint8_t *Data,...);

#endif /* __USART2_H */
