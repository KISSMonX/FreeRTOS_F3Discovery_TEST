/*-----------------------------------------------------------
 *创建5个任务：
 *1. LED3闪烁任务prvLED3BlinkTask
 *2. 按键检测任务prvButtonCheckTask
 *5. 增加 IO 口反转速度检测
 *-----------------------------------------------------------*/

/* 标准头文件 */	
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stm32f30x_conf.h"
#include "user_tasks.h"	


static xSemaphoreHandle xButtonPushSemaphore;	// 按键信号, 全局本地的......

//=========================================================================================================
/**
 * @brief  LED3闪烁任务：每隔500ms切换LED3的显示状态.
 * @param  pvParameters:任务默认参数.
 * @retval 无
 */
 //=========================================================================================================
void prvLED_BlinkTask(void *pvParameters)
{
        portTickType xNextWakeTime;
        const portTickType xFrequency = 500;
        xNextWakeTime = xTaskGetTickCount();
        for(;;)
        {				
                GPIOE->ODR ^= (GPIO_Pin_9 | GPIO_Pin_12 | GPIO_Pin_15);
                vTaskDelayUntil(&xNextWakeTime, xFrequency);
        }
}


//=========================================================================================================
/**
 * @brief  测试 IO 口翻转速度与 CPU 工作频率
 * @param  pvParameters:任务默认参数.
 * @retval 无
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
 * @brief  按键检测任务：每隔20ms检测按键状态。如果有按键事件则释放信号.
 * @param  pvParameters:任务默认参数.
 * @retval 无
 */
//=========================================================================================================
void prvButtonCheckTask(void *pvParameters)
{	
        static uint8_t bounce_count;
        portTickType xNextWakeTime;
        const portTickType xFrequency = 30;
        xNextWakeTime = xTaskGetTickCount();

        /* 创建信号 */
        vSemaphoreCreateBinary(xButtonPushSemaphore);	

        /* 如果创建信号成功，将信号初始化为0 */	
        if (xButtonPushSemaphore != NULL) {
                xSemaphoreTake(xButtonPushSemaphore, (portTickType)0);
        }	

        for(;;) {
                /* 读取按键状态 */
                if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == pdTRUE)//如果检测到按键按下
                {
                        if (bounce_count++ == 6)//按键防抖
                        {	
				// 如果不清零的话, 就得加到溢出然后再重新加到才行, 结果就是会很迟钝
				bounce_count = 0;	
                                xSemaphoreGive(xButtonPushSemaphore);//释放按键信号 
                        }
                }
                /* 每隔Nms检测一次 */ 
                vTaskDelayUntil(&xNextWakeTime,xFrequency);
        }
}
