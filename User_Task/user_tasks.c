/*-----------------------------------------------------------
 *����5������
 *1. LED3��˸����prvLED3BlinkTask
 *2. �����������prvButtonCheckTask
 *5. ���� IO �ڷ�ת�ٶȼ��
 *-----------------------------------------------------------*/

/* ��׼ͷ�ļ� */	
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stm32f30x_conf.h"
#include "user_tasks.h"	


static xSemaphoreHandle xButtonSpeedUpSemaphore;	// �����ź�

//=========================================================================================================
/**
 * @brief  LED3��˸����ÿ��500ms�л�LED3����ʾ״̬.
 * @param  pvParameters:����Ĭ�ϲ���.
 * @retval ��
 */
 //=========================================================================================================
void prvLED_BlinkTask(void *pvParameters)
{
        portTickType xNextWakeTime;
        const portTickType xFrequency = 100;
        xNextWakeTime = xTaskGetTickCount();
        for(;;)
        {				
                GPIOE->ODR ^= (GPIO_Pin_9 | GPIO_Pin_12 | GPIO_Pin_15);
                vTaskDelayUntil(&xNextWakeTime, xFrequency);
        }
}


//=========================================================================================================
/**
 * @brief  ���� IO �ڷ�ת�ٶ��� CPU ����Ƶ��
 * @param  pvParameters:����Ĭ�ϲ���.
 * @retval ��
 */
//=========================================================================================================
void prvPort_ToggleTask(void *pvParameters)
{
	for (;;) {
		GPIOE->ODR ^= GPIO_Pin_8;
		vTaskDelay(80);
	}
}	
	
	
//=========================================================================================================
/**
 * @brief  �����������ÿ��20ms��ⰴ��״̬������а����¼����ͷ��ź�.
 * @param  pvParameters:����Ĭ�ϲ���.
 * @retval ��
 */
//=========================================================================================================
void prvButtonCheckTask(void *pvParameters)
{	
        static uint8_t bounce_count;
        portTickType xNextWakeTime;
        const portTickType xFrequency = 10;
        xNextWakeTime = xTaskGetTickCount();

        /* �����ź� */
        vSemaphoreCreateBinary(xButtonSpeedUpSemaphore);	

        /* ��������źųɹ������źų�ʼ��Ϊ0 */	
        if (xButtonSpeedUpSemaphore != NULL) {
                xSemaphoreTake(xButtonSpeedUpSemaphore, (portTickType)0);
        }	

        for(;;) {
                /* ��ȡ����״̬ */
                if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == pdTRUE)//�����⵽��������
                {
                        if (bounce_count++ == 5)//��������
                        {	
				// ���������Ļ�, �͵üӵ����Ȼ�������¼ӵ�����, ������ǻ�ܳٶ�
				bounce_count = 0;	
				GPIOE->ODR ^= (GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_13 | GPIO_Pin_14);
                                xSemaphoreGive(xButtonSpeedUpSemaphore);//�ͷŰ����ź� 
                        }
                }
                /* ÿ��Nms���һ�� */ 
                vTaskDelayUntil(&xNextWakeTime,xFrequency);
        }
}

