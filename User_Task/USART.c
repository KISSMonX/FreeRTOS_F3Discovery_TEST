#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "stm32f30x_conf.h"
#include "usart.h"



uint8_t rx_buffer[RX_BUFFER_LENGTH];
uint8_t rx_counter = 0;

/**
 * @brief  This function initializes USART1 module
 * @param  speed: baudrate
 *          inten: enable interrupt
 * @retval None
 */
void USART1_Init(uint32_t speed, uint8_t INT_Enable)
{
        USART_InitTypeDef USART_InitStructure;
        GPIO_InitTypeDef GPIO_InitStructure;
        NVIC_InitTypeDef NVIC_InitStructure;

        /* Enable GPIO clock */
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

        /* Enable USART clock */
        RCC_APB1PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

        /* Connect PXx to USARTx_Tx */
        GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_7);

        /* Connect PXx to USARTx_Rx */
        GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_7);

        /* Configure USART Tx as alternate function push-pull */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_Init(GPIOA, &GPIO_InitStructure);

        /* Configure USART Rx as alternate function push-pull */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
        GPIO_Init(GPIOA, &GPIO_InitStructure);

        /* USART configuration */
        USART_InitStructure.USART_BaudRate = speed;
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;
        USART_InitStructure.USART_StopBits = USART_StopBits_1;
        USART_InitStructure.USART_Parity = USART_Parity_No;
        USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
        USART_Init(USART1, &USART_InitStructure);

        if(INT_Enable) {
                /* Enable the USART1 Receive interrupt: this interrupt is generated when the
                   USART1 receive data register is not empty */
                USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

                /* Enable the USART1 Interrupt */
                NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
                NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
                NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
                NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
                NVIC_Init(&NVIC_InitStructure);
        }

        /* Enable USART */
        USART_Cmd(USART1, ENABLE);
}

/**
 * @brief  Retargets the C library printf function to the USART.
 *                      Overwrite __io_putchar function used by printf
 * @param  None
 * @retval None
 */
PUTCHAR_PROTOTYPE
{
        /* Put character on the serial line */
        USART1->TDR = (ch & (uint16_t)0x01FF);

        /* Loop until transmit data register is empty */
        while ((USART1->ISR & USART_FLAG_TXE) == (uint16_t) RESET);

        return ch;
}

/**
 * @brief  This function handles USART1 global interrupt request.
 * @param  None
 * @retval None
 */
void USART1_IRQHandler(void)
{
        if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
                /* Read one byte from the receive data register */
                rx_buffer[rx_counter] = (USART_ReceiveData(USART1) & 0x7F);

                if(rx_counter + 1 == RX_BUFFER_LENGTH || rx_buffer[rx_counter] == '\n' 
			|| rx_buffer[rx_counter] == '\r') {
                        printf("%s\n\r", rx_buffer);
                        memset(rx_buffer, 0, RX_BUFFER_LENGTH);
                        rx_counter = 0;
                }
                else {
                        rx_counter++;
                }
        }
}


uint8_t USART1_ReadChar(void)
{
        while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
        return USART_ReceiveData(USART1);
}
