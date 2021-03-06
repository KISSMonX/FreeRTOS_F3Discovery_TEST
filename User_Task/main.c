//=========================================================================================================
// 
//=========================================================================================================
#include "STM32F30x.h"
#include "user_tasks.h"	
#include "usart.h"
#include "sensors.h"


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
// 时钟配置, 包括系统时钟, 外设时钟
// 使用内部的 8MHz 时钟作为 PLL 时钟源, 配置成 72MHz
//=========================================================================================================
static void RCC_Config(void)
{
	/* Start with the clocks in their expected state. */
	RCC_DeInit();

	// Disable HSE (high speed external clock).
	RCC_HSEConfig( RCC_HSE_OFF );
	
	// Enables the Internal High Speed oscillator (HSI).
	RCC_HSICmd(ENABLE);

	/* Wait till HSI is ready. */
	while( RCC_GetFlagStatus( RCC_FLAG_HSIRDY ) == RESET )
	{
	}

//	/* 2 wait states required on the flash. */
//	*( ( unsigned long * ) 0x40022000 ) = 0x02;

	/* HCLK = SYSCLK */
	RCC_HCLKConfig( RCC_SYSCLK_Div1 );

	/* PCLK2 = HCLK */
	RCC_PCLK2Config( RCC_HCLK_Div1 );

	/* PCLK1 = HCLK/2 */
	RCC_PCLK1Config( RCC_HCLK_Div2 );
	
	/* 2 wait states required on the flash. */
	FLASH_SetLatency(FLASH_Latency_2);
	
	/* PLLCLK = 4MHz * 15 = 60 MHz. */
	RCC_PLLConfig( RCC_PLLSource_HSI_Div2, RCC_PLLMul_15 );

	/* Enable PLL. */
	RCC_PLLCmd( ENABLE );

	/* Wait till PLL is ready. */
	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
	{
	}

	/* Select PLL as system clock source. */
	RCC_SYSCLKConfig( RCC_SYSCLKSource_PLLCLK );

	/* Wait till PLL is used as system clock source. */
	while( RCC_GetSYSCLKSource() != 0x08 )
	{
	}

	/* Enable GPIOA, GPIOB, GPIOC, GPIOD, GPIOE and AFIO clocks */
	// STM32F3 和 STM32F0 中的 GPIO 连接总线 AHB2, 而不是 F1 系列的 APB2 
	// 具体参考 Block diagram 和 Migrating from STM32F1 to STM32F3 microcontrollers
	RCC_AHBPeriphClockCmd(	RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOE, ENABLE );
	
	RCC_MCOConfig(RCC_MCOSource_PLLCLK_Div2); 	// 引脚输出系统时钟/2

	/* Configure HCLK clock as SysTick clock source. */
	SysTick_CLKSourceConfig( SysTick_CLKSource_HCLK );

}

//=========================================================================================================
// 硬件初始化
//=========================================================================================================
static void prvSetupHardware( void )
{
	// 内部外设结构体变量
	GPIO_InitTypeDef  	GPIO_InitStructure;	
	
	// 设置 MCO 引脚输出到 PA8
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF ;  // OUT 模式再试试, 据说只能复用模式才能输出
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	// 推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/*
	 * LED GPIO初始化
	 * GPIOE 控制一圈 LED
	 */	 
	GPIO_InitStructure.GPIO_Pin   = (GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 \
					| GPIO_Pin_12 | GPIO_Pin_13| GPIO_Pin_14 | GPIO_Pin_15);	 
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
	
	//=================================================================================================										
	RCC_Config();			// 时钟配置	
	USART1_Init(115200, ENABLE);	// 波特率和中断使能
	printf("\fUSART1 initialized!\n\r");
	
	/* Initialize compass */
        Compass_Config();
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




