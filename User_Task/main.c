//=========================================================================================================
// 
//=========================================================================================================
#include "STM32F30x.h"
#include "user_tasks.h"	


unsigned long ulRunTimeStatsClock = 0;
	

//=========================================================================================================
static void prvSetupHardware(void);	// Ӳ����ʼ������

//=========================================================================================================
// ������
// Ӳ����ʼ��, ���񴴽�, ����������
//=========================================================================================================
int main(void)
{
	// ��ʼ��STM32F0XX Discovery ����Ҫ��Ӳ��ϵͳ
	prvSetupHardware();		

	// �����������û����� 
	        /* �������� */
        xTaskCreate(prvLED_BlinkTask,			//������
                        ( char *) "LED3 BLINK",   	//��������
                        configMINIMAL_STACK_SIZE, 	//�����ջ
                        NULL, 				//�������
                        1,	//�������ȼ� 
                        NULL);				//������
			
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
			
        /* ������������� */
        vTaskStartScheduler();

        /* �ڵ�������������ʱ�����򲻻�ִ�е��˴�������������е������˵��
           FreeRTOS�������������Ͷ�ʱ������Ķѿռ䲻�� */
        for( ;; );		
}


//=========================================================================================================
// ʱ������, ����ϵͳʱ��, ����ʱ��
// ʹ���ڲ��� 8MHz ʱ����Ϊ PLL ʱ��Դ, ���ó� 72MHz
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
	
	/* PLLCLK = 4MHz * 10 = 40 MHz. */
	RCC_PLLConfig( RCC_PLLSource_HSI_Div2, RCC_PLLMul_10 );

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
	// STM32F3 �� STM32F0 �е� GPIO �������� AHB2, ������ F1 ϵ�е� APB2 
	// ����ο� Block diagram �� Migrating from STM32F1 to STM32F3 microcontrollers
	RCC_AHBPeriphClockCmd(	RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOE, ENABLE );
	
	RCC_MCOConfig(RCC_MCOSource_PLLCLK_Div2); 	// �������ϵͳʱ��/2
	
//	/* Set the Vector Table base address at 0x08000000 */
//	NVIC_SetVectorTable( NVIC_VectTab_FLASH, 0x0 );

//	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );

	/* Configure HCLK clock as SysTick clock source. */
	SysTick_CLKSourceConfig( SysTick_CLKSource_HCLK );

}

//=========================================================================================================
// Ӳ����ʼ��
//=========================================================================================================
static void prvSetupHardware( void )
{
	// �ڲ�����ṹ�����
	GPIO_InitTypeDef  	GPIO_InitStructure;	
//	NVIC_InitTypeDef	NVIC_InitStructure;
	
	
	//ʱ������											
	RCC_Config();
	
	
	// ���� MCO ��������� PA8
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF ;  // OUT ģʽ������, ��˵ֻ�ܸ���ģʽ�������
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	// �������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/*
	 * LED GPIO��ʼ��
	 * GPIOE ����һȦ LED
	 */	 
	GPIO_InitStructure.GPIO_Pin   = (GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 \
					| GPIO_Pin_12 | GPIO_Pin_13| GPIO_Pin_14 | GPIO_Pin_15);	 
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;		// ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		// �������
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;	// ��ֹ������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	// IO ���ٶ�
	GPIO_Init(GPIOE, &GPIO_InitStructure);			// д������
	 
	 
	/* 
	 * ������ʼ��
	 * GPIOA��PA0���ڿ��ư���
	 */
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	// ϵͳ�жϳ�ʼ�� -> ϵͳ�����ж�
//	NVIC_InitStructure.NVIC_IRQChannel = SVCall_IRQn;	
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);	
}


//=========================================================================================================
// 
//=========================================================================================================
void vApplicationMallocFailedHook(void)
{
	/* 
	 �ڴ����ʧ�ܹ��Ӻ���
	 FreeRTOS�ں��ڴ������񡢶��С���ʱ�������ź�ʱ�����pvPortMalloc() ������
	 һ��pvPortMalloc() ��������ʧ�ܣ��������ļ�FreeRTOSConfig.h�У�
	 configUSE_MALLOC_FAILED_HOOK����Ϊ1ʱ����øú�����
	 �ú���Ҳ������Ӧ�ó�����á�
	 ���Ӧ�ó����а������ļ�heap_1.c ���� heap_2.c��pvPortMalloc() ��ʹ�õĶѴ�С
	 ���ļ�FreeRTOSConfig.h�е�configTOTAL_HEAP_SIZE���塣
	 ����API����xPortGetFreeHeapSize()���Է��ؿ��еĶѿռ��С��
	*/
	taskDISABLE_INTERRUPTS();//�������п����ε��ж�
	for( ;; );
}

//=========================================================================================================
//=========================================================================================================
void vApplicationIdleHook(void)
{
	/* 
	 ���������Ӻ���
	 ���ļ�FreeRTOSConfig.h�� configUSE_IDLE_HOOK ����Ϊ1ʱ��ϵͳ���������
	 ÿ��ѭ���л���øú������ú����в���������Ӧ�ó���ִ�еĴ��롣
	 ���Ӧ�ó������vTaskDelete() API����ʱ���ù��Ӻ���Ӧ�ܹ����ص��ú�����
	 ��ʱ��Ϊ��������һ�����������ں˷������ɾ������Ĵ洢�ռ䡣
	*/
}

//=========================================================================================================
//=========================================================================================================
void vApplicationStackOverflowHook(xTaskHandle pxTask, signed char *pcTaskName)
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* 
	 Ӧ�ó����ջ������Ӻ���
	 ���ļ�FreeRTOSConfig.h�� configCHECK_FOR_STACK_OVERFLOW����Ϊ1��2ʱ��ִ��
	 ����ʱ��ջ�����顣����⵽��ջ���ʱ����øù��Ӻ���R 
	*/ 
	taskDISABLE_INTERRUPTS();//�������п������ж�
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
	 Ӧ�ó���tick���Ӻ������ļ�FreeRTOSConfig.h�� configUSE_TICK_HOOK����Ϊ1ʱ���ú����
	��tick�ж��б����á��ù��Ӻ����п�������û����򣬵������ڸú������жϷ�������е��ã
	�û����벻����������ִ�У�����ֻ��ʹ���жϰ�ȫ��API������
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




