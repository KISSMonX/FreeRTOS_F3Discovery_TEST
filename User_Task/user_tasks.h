#ifndef __USER_TASKS_H
#define __USER_TASKS_H

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"


void prvLED_BlinkTask(void *pvParameters);//LED3��˸����
void prvButtonCheckTask(void *pvParameters);//�����������
void prvPort_ToggleTask(void *pvParameters);


#endif
