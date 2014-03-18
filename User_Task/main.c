//=========================================================================================================
// 
//=========================================================================================================

#include <string.h>
#include "STM32F30x.h"
#include "user_tasks.h"	


unsigned long ulRunTimeStatsClock = 0;
	

//=========================================================================================================
static void prvSetupHardware(void);	// Ó²¼ş³õÊ¼»¯º¯Êı

//=========================================================================================================
// Ö÷º¯Êı
// Ó²¼ş³õÊ¼»¯, ÈÎÎñ´´½¨, ¿ªÆôµ÷¶ÈÆ÷
//=========================================================================================================
int main(void)
{
	// ³õÊ¼»¯STM32F0XX Discovery ËùĞèÒªµÄÓ²¼şÏµÍ³
	prvSetupHardware();		

	// ´´½¨²¢ÔËĞĞÓÃ»§ÈÎÎñ 
	        /* ´´½¨ÈÎÎñ */
        xTaskCreate(prvLED_BlinkTask,			//ÈÎÎñº¯Êı
                        ( char *) "LED3 BLINK",   	//ÈÎÎñÃû³Æ
                        configMINIMAL_STACK_SIZE, 	//ÈÎÎñ¶ÑÕ»
                        NULL, 				//ÈÎÎñ²ÎÊı
                        1,	//ÈÎÎñÓÅÏÈ¼¶ 
                        NULL);				//ÈÎÎñ¾ä±ú
			
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
			
        /* Æô¶¯ÈÎÎñµ÷¶ÈÆ÷ */
        vTaskStartScheduler();

        /* ÔÚµ÷¶ÈÆ÷Õı³£ÔËĞĞÊ±£¬³ÌĞò²»»áÖ´ĞĞµ½´Ë´¦¡£Èç¹û³ÌĞòÔËĞĞµ½ÕâÀï£¬ÔòËµÃ÷
           FreeRTOS·ÖÅä¸ø¿ÕÏĞÈÎÎñºÍ¶¨Ê±Æ÷ÈÎÎñµÄ¶Ñ¿Õ¼ä²»×ã */
        for( ;; );		
}

//=========================================================================================================
// Ó²¼ş³õÊ¼»¯
//=========================================================================================================
static void prvSetupHardware( void )
{
	// ÄÚ²¿ÍâÉè½á¹¹Ìå±äÁ¿
	GPIO_InitTypeDef  	GPIO_InitStructure;	
	NVIC_InitTypeDef	NVIC_InitStructure;
	
	
	//ÍâÉèÊ±ÖÓÅäÖÃ---ENABLE
	RCC_AHBPeriphClockCmd(	RCC_AHBPeriph_GPIOA |		// GPIOAÄ£¿éÊ±ÖÓ
				RCC_AHBPeriph_GPIOE, ENABLE);	// GPIOEÄ£¿éÊ±ÖÓ											

	/*
	 * LED GPIO³õÊ¼»¯
	 * GPIOE ¿ØÖÆÒ»È¦ LED
	 */	 
	GPIO_InitStructure.GPIO_Pin   = (GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_12 | GPIO_Pin_15);	 
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;		// Êä³öÄ£Ê½
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		// ÍÆÍìÊä³ö
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;	// ½ûÖ¹ÉÏÏÂÀ­
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	// IO ¿ÚËÙ¶È
	GPIO_Init(GPIOE, &GPIO_InitStructure);			// Ğ´ÈëÅäÖÃ
	 
	 
	/* 
	 * °´¼ü³õÊ¼»¯
	 * GPIOA£ºPA0ÓÃÓÚ¿ØÖÆ°´¼ü
	 */
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	// ÏµÍ³ÖĞ¶Ï³õÊ¼»¯ -> ÏµÍ³ËùÓÃÖĞ¶Ï
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
	 ÄÚ´æ·ÖÅäÊ§°Ü¹³×Óº¯Êı
	 FreeRTOSÄÚºËÔÚ´´½¨ÈÎÎñ¡¢¶ÓÁĞ¡¢¶¨Ê±Æ÷»òÕßĞÅºÅÊ±»áµ÷ÓÃpvPortMalloc() º¯Êı¡£
	 Ò»µ©pvPortMalloc() º¯Êıµ÷ÓÃÊ§°Ü£¬²¢ÇÒÔÚÎÄ¼şFreeRTOSConfig.hÖĞ£¬
	 configUSE_MALLOC_FAILED_HOOKÉèÖÃÎª1Ê±»áµ÷ÓÃ¸Ãº¯Êı¡£
	 ¸Ãº¯ÊıÒ²¿ÉÒÔÓÉÓ¦ÓÃ³ÌĞòµ÷ÓÃ¡£
	 Èç¹ûÓ¦ÓÃ³ÌĞòÖĞ°üº¬ÁËÎÄ¼şheap_1.c »òÕß heap_2.c£¬pvPortMalloc() ¿ÉÊ¹ÓÃµÄ¶Ñ´óĞ¡
	 ÓÉÎÄ¼şFreeRTOSConfig.hÖĞµÄconfigTOTAL_HEAP_SIZE¶¨Òå¡£
	 ÀûÓÃAPIº¯ÊıxPortGetFreeHeapSize()¿ÉÒÔ·µ»Ø¿ÕÏĞµÄ¶Ñ¿Õ¼ä´óĞ¡¡£
	*/
	taskDISABLE_INTERRUPTS();//½ûÓÃËùÓĞ¿ÉÆÁ±ÎµÄÖĞ¶Ï
	for( ;; );
}

//=========================================================================================================
//=========================================================================================================
void vApplicationIdleHook(void)
{
	/* 
	 ¿ÕÏĞÈÎÎñ¹³×Óº¯Êı
	 µ±ÎÄ¼şFreeRTOSConfig.hÖĞ configUSE_IDLE_HOOK ÉèÖÃÎª1Ê±£¬ÏµÍ³¿ÕÏĞÈÎÎñµÄ
	 Ã¿´ÎÑ­»·ÖĞ»áµ÷ÓÃ¸Ãº¯Êı¡£¸Ãº¯ÊıÖĞ²»ÄÜÓĞ×èÈûÓ¦ÓÃ³ÌĞòÖ´ĞĞµÄ´úÂë¡£
	 Èç¹ûÓ¦ÓÃ³ÌĞòµ÷ÓÃvTaskDelete() APIº¯ÊıÊ±£¬¸Ã¹³×Óº¯ÊıÓ¦ÄÜ¹»·µ»Øµ÷ÓÃº¯Êı¡£
	 ÕâÊ±ÒòÎª¿ÕÏĞÈÎÎñÒ»°ãÓÃÀ´ÇåÀíÄÚºË·ÖÅä¸øÒÑÉ¾³ıÈÎÎñµÄ´æ´¢¿Õ¼ä¡£
	*/
}

//=========================================================================================================
//=========================================================================================================
void vApplicationStackOverflowHook(xTaskHandle pxTask, signed char *pcTaskName)
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* 
	 Ó¦ÓÃ³ÌĞò¶ÑÕ»Òç³ö¹³×Óº¯Êı
	 µ±ÎÄ¼şFreeRTOSConfig.hÖĞ configCHECK_FOR_STACK_OVERFLOWÉèÖÃÎª1»ò2Ê±½«Ö´ĞĞ
	 ÔËĞĞÊ±¶ÑÕ»Òç³ö¼ì²é¡£µ±¼ì²âµ½¶ÑÕ»Òç³öÊ±»áµ÷ÓÃ¸Ã¹³×Óº¯ÊıR 
	*/ 
	taskDISABLE_INTERRUPTS();//½ûÓÃËùÓĞ¿ÉÆÁ±ÎÖĞ¶Ï
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
	 Ó¦ÓÃ³ÌĞòtick¹³×Óº¯Êıµ±ÎÄ¼şFreeRTOSConfig.hÖĞ configUSE_TICK_HOOKÉèÖÃÎª1Ê±£¬¸Ãº¯Êı½
	ÔÚtickÖĞ¶ÏÖĞ±»µ÷ÓÃ¡£¸Ã¹³×Óº¯ÊıÖĞ¿ÉÒÔÌí¼ÓÓÃ»§³ÌĞò£¬µ«ÊÇÓÉÓÚ¸Ãº¯ÊıÔÚÖĞ¶Ï·şÎñ³ÌĞòÖĞµ÷ÓÃ£
	ÓÃ»§´úÂë²»ÄÜ×èÈû³ÌĞòÖ´ĞĞ£¬¶øÇÒÖ»ÄÜÊ¹ÓÃÖĞ¶Ï°²È«µÄAPIº¯Êı¡£
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




