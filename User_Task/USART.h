#include "stm32f30x_usart.h"
#include <stdio.h>

#ifndef USART_H_
#define USART_H_

#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

#define RX_BUFFER_LENGTH        (64)

extern PUTCHAR_PROTOTYPE;
extern void USART1_Init(uint32_t speed, uint8_t inten);
extern void USART1_IRQHandler(void);
extern uint8_t USART1_ReadChar(void);

#endif /* USART_H_ */
