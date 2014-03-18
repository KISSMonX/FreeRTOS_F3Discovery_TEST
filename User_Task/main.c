//=========================================================================================================
// 
//=========================================================================================================

#include <string.h>
#include "STM32F30x.h"
#include "user_tasks.h"	


unsigned long ulRunTimeStatsClock = 0;
	

//=========================================================================================================
static void prvSetupHardware(void);	// 硬件初始化函数

//=========================================================================================================
// 主函数
// 硬件初始化, 任务创建, 开启调度器
//=========================================================================================================
int main(void)
{
	// 初始化STM32F0XX Discovery 所需要的硬件系统
	prvSetupHardware();		

	// 创建并运行用户任务 
	        /* 创建任务 */
        xTaskCreate(prvLED_BlinkTask,			//任务函数
                        ( char *) "LED3 BLINK",   	//任务名称
                        configMINIMAL_STACK_SIZE, 	//任务堆栈
                        NULL, 				//任务参数
                        1,	//任务优先级 
                        NULL);				//任务句柄
			
        xTaskCreate(prvButtonCheckTask,
                        ( char *) "BTN CHECK",
                        configMINIMAL_STACK_SIZE,
                        NULL,
                        2,	
                        NULL);	

	xTaskCreate(prvPort_ToggleTask,
                        ( char *) "Port Test",
                        configMINIMAL_STACK_SIZE,
                        NULL,
                        3,	
                        NULL);
			
        /* 启动任务调度器 */
        vTaskStartScheduler();

        /* 在调度器正常运行时，程序不会执行到此处。如果程序运行到这里，则说明
           FreeRTOS分配给空闲任务和定时器任务的堆空间不足 */
        for( ;; );		
}

//=========================================================================================================
// 硬件初始化
//=========================================================================================================
static void prvSetupHardware( void )
{
	// 内部外设结构体变量
	GPIO_InitTypeDef  	GPIO_InitStructure;	
	NVIC_InitTypeDef	NVIC_InitStructure;
	
	
	//外设时钟配置---ENABLE
	RCC_AHBPeriphClockCmd(	RCC_AHBPeriph_GPIOA |		// GPIOA模块时钟
				RCC_AHBPeriph_GPIOE, ENABLE);	// GPIOE模块时钟											

	/*
	 * LED GPIO初始化
	 * GPIOE 控制一圈 LED
	 */	 
	GPIO_InitStructure.GPIO_Pin   = (GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_12 | GPIO_Pin_15);	 
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;		// 输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		// 推挽输出
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;	// 禁止上下拉
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	// IO 口速度
	GPIO_Init(GPIOE, &GPIO_InitStructure);			// 写入配置
	 
	 
	/* 
	 * 按键初始化
	 * GPIOA：PA0用于控制按键
	 */
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	// 系统中断初始化 -> 系统所用中断
	NVIC_InitStructure.NVIC_IRQChannel = SVCall_IRQn;	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
}


//=========================================================================================================
// 
//=========================================================================================================
void vApplicationMallocFailedHook(void)
{
	/* 
	 内存分配失败钩子函数
	 FreeRTOS内核在创建任务、队列、定时器或者信号时会调用pvPortMalloc() 函数。
	 一旦pvPortMalloc() 函数调用失败，并且在文件FreeRTOSConfig.h中，
	 configUSE_MALLOC_FAILED_HOOK设置为1时会调用该函数。
	 该函数也可以由应用程序调用。
	 如果应用程序中包含了文件heap_1.c 或者 heap_2.c，pvPortMalloc() 可使用的堆大小
	 由文件FreeRTOSConfig.h中的configTOTAL_HEAP_SIZE定义。
	 利用API函数xPortGetFreeHeapSize()可以返回空闲的堆空间大小。
	*/
	taskDISABLE_INTERRUPTS();//禁用所有可屏蔽的中断
	for( ;; );
}

//=========================================================================================================
//=========================================================================================================
void vApplicationIdleHook(void)
{
	/* 
	 空闲任务钩子函数
	 当文件FreeRTOSConfig.h中 configUSE_IDLE_HOOK 设置为1时，系统空闲任务的
	 每次循环中会调用该函数。该函数中不能有阻塞应用程序执行的代码。
	 如果应用程序调用vTaskDelete() API函数时，该钩子函数应能够返回调用函数。
	 这时因为空闲任务一般用来清理内核分配给已删除任务的存储空间。
	*/
}

//=========================================================================================================
//=========================================================================================================
void vApplicationStackOverflowHook(xTaskHandle pxTask, signed char *pcTaskName)
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* 
	 应用程序堆栈溢出钩子函数
	 当文件FreeRTOSConfig.h中 configCHECK_FOR_STACK_OVERFLOW设置为1或2时将执行
	 运行时堆栈溢出检查。当检测到堆栈溢出时会调用该钩子函数R 
	*/ 
	taskDISABLE_INTERRUPTS();//禁用所有可屏蔽中断
	for( ;; );
}


//=========================================================================================================
// This function will be called by each tick interrupt if
// configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h.  User code can be
// added here, but the tick hook is called from an interrupt context, so
// code must not attempt to block, and only the interrupt safe FreeRTOS API
// functions can be used (those that end in FromISR()).
//=========================================================================================================
void vApplicationTickHook(void)
{
	/* 
	 应用程序tick钩子函数当文件FreeRTOSConfig.h中 configUSE_TICK_HOOK设置为1时，该函数�
	在tick中断中被调用。该钩子函数中可以添加用户程序，但是由于该函数在中断服务程序中调用�
	用户代码不能阻塞程序执行，而且只能使用中断安全的API函数。
	*/
}


//=========================================================================================================
//=========================================================================================================
#ifdef JUST_AN_EXAMPLE_ISR

void Dummy_IRQHandler(void)
{
	long lHigherPriorityTaskWoken = pdFALSE;

	/* Clear the interrupt if necessary. */
	Dummy_ClearITPendingBit();

	/* This interrupt does nothing more than demonstrate how to synchronise a
	task with an interrupt.  A semaphore is used for this purpose.  Note
	lHigherPriorityTaskWoken is initialised to zero. Only FreeRTOS API functions
	that end in "FromISR" can be called from an ISR. */
	xSemaphoreGiveFromISR( xTestSemaphore, &lHigherPriorityTaskWoken );

	/* If there was a task that was blocked on the semaphore, and giving the
	semaphore caused the task to unblock, and the unblocked task has a priority
	higher than the current Running state task (the task that this interrupt
	interrupted), then lHigherPriorityTaskWoken will have been set to pdTRUE
	internally within xSemaphoreGiveFromISR().  Passing pdTRUE into the
	portEND_SWITCHING_ISR() macro will result in a context switch being pended to
	ensure this interrupt returns directly to the unblocked, higher priority,
	task.  Passing pdFALSE into portEND_SWITCHING_ISR() has no effect. */
	portEND_SWITCHING_ISR( lHigherPriorityTaskWoken );
}

#endif /* JUST_AN_EXAMPLE_ISR */




