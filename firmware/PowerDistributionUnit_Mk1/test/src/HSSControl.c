// HSSControl.c
// ----------------------------------------------------------------------------
// Tests HSS control via shift register for output enable and fault latching. 

// INCLUDES -------------------------------------------------------------------

#include "stm32xx_hal.h"
// stm32xx_hal.h contains includes for RTOS stuff.
#include<stdio.h>
#include<string.h>

// BBPDU Peripherals
#include "PDU_Mk1_Pins.h"
#include "PDU_Mk1_SPI.h"
#include "PDU_Mk1_GPIO.h"
#include "PDU_Mk1_UART.h"

// drivers
#include "ShiftRegister_SPI.h"
#include "PDU_Mk1_HSSControl.h"

// DEFINES --------------------------------------------------------------------

#define TASKPRIORITY_INIT tskIDLE_PRIORITY + 2
#define TASKSTACKSIZE_INIT configMINIMAL_STACK_SIZE+1500

#define TASKPRIORITY_BLINK tskIDLE_PRIORITY + 2
#define TASKSTACKSIZE_BLINK configMINIMAL_STACK_SIZE

#define TASKPRIORITY_SR_WRITE tskIDLE_PRIORITY + 2
#define TASKSTACKSIZE_SR_WRITE configMINIMAL_STACK_SIZE+1000

#define INTERVAL_BLINK_MS 500
#define INTERVAL_BLINK_ERROR_MS 50
#define INTERVAL_SR_WRITE_MS 60000

// DECLARATIONS ---------------------------------------------------------------

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;
SPI_HandleTypeDef hspi3;

SemaphoreHandle_t spi1_mutex;           // Mutex to prevent simultaneous SPI access
StaticSemaphore_t spi1_mutex_buffer;    // Static buffer for mutex allocation

SemaphoreHandle_t spi1_done_sem;        // Semaphore to signal SPI IT completion
StaticSemaphore_t spi1_done_sem_buffer; // Static buffer for completion semaphore

SemaphoreHandle_t spi2_mutex;           // Mutex to prevent simultaneous SPI access
StaticSemaphore_t spi2_mutex_buffer;    // Static buffer for mutex allocation

SemaphoreHandle_t spi2_done_sem;        // Semaphore to signal SPI IT completion
StaticSemaphore_t spi2_done_sem_buffer; // Static buffer for completion semaphore

SemaphoreHandle_t spi3_mutex;           // Mutex to prevent simultaneous SPI access
StaticSemaphore_t spi3_mutex_buffer;    // Static buffer for mutex allocation

SemaphoreHandle_t spi3_done_sem;        // Semaphore to signal SPI IT completion
StaticSemaphore_t spi3_done_sem_buffer; // Static buffer for completion semaphore

TaskHandle_t init_task;
StaticTask_t init_task_buffer;
StackType_t init_task_stack[TASKSTACKSIZE_INIT];

TaskHandle_t blink_task;
StaticTask_t blink_task_buffer;
StackType_t blink_task_stack[TASKSTACKSIZE_BLINK];

TaskHandle_t sr_write_task;
StaticTask_t sr_write_task_buffer;
StackType_t sr_write_task_stack[TASKSTACKSIZE_SR_WRITE];

extern HSSControl_FaultState_t HSS_fault_state[PDU_MK1_NUM_CHANNELS];

// TASKS ----------------------------------------------------------------------

// initialize stuff
void Init_Task(void *argument)
{
	printf("Starting Initialization...\n");

	PDU_Mk1_GPIO_Init();
	
	if(PDU_Mk1_SPI2_HSS_SR_Init() != true)
	{
		printf("FAIL:SPI_INIT\n");
		Error_Handler();
	}
	
	if(PDU_Mk1_UART_Printf_Init() != true)
	{
		printf("FAIL:UART_PRINTF_INIT\n");
		Error_Handler();
	}

	if(SPI_RTOS_Mutex_Semaphore_Setup(&spi2_mutex, &spi2_mutex_buffer, &spi2_done_sem, &spi2_done_sem_buffer) != true)
	{
		printf("FAIL:MUTEX_SEMAPHORE_INIT\n");
		Error_Handler();
	}

	// SPI dummy send because CLK pin initializes high even when configured low for some reason
	if(SPI_Init_Dummy_Send(&hspi2, spi2_mutex, spi2_done_sem) != true)
	{
		printf("FAIL:SPI_INIT_DUMMY_SEND\n");
		Error_Handler();
	}

	PDU_Mk1_HSSControl_Init();

	printf("Initialization complete.\n");

	// get this party started
	vTaskResume(blink_task);
	vTaskResume(sr_write_task);
	// task kills itself
	vTaskDelete(NULL);
}

// blink LED
void Blink_Task(void *argument)
{
	for(;;)
	{
		HAL_GPIO_TogglePin(LED_PORT, LED_PIN);

		vTaskDelay(pdMS_TO_TICKS(INTERVAL_BLINK_MS));
	}
}

// read conversion from ADC and print to serial
void SR_Write_Task(void *argument)
{
	HSS_fault_state[7] = HSSCONTROL_FAULT_HSS_TRIP;

	for(;;)
	{
		//					[0]            [15]
		// expected EN: 	0010 0011 0100 0010 (note: depends on PDU_Mk1_OutputConfig.h)
		PDU_Mk1_HSSControl_CritOnly();

		// expected LATCH: 	1111 1111 1111 1111

		vTaskDelay(pdMS_TO_TICKS(INTERVAL_SR_WRITE_MS));

		// expected EN: 	1111 1111 1111 1111
		PDU_Mk1_HSSControl_AllOn();

		// expected LATCH: 	1111 0111 1011 1111
		PDU_Mk1_HSSControl_WriteLatch_Ch(PDU_OUTPUT_2, HSSCONTROL_LATCH_NOCHANGE);
		PDU_Mk1_HSSControl_WriteLatch_Ch(PDU_OUTPUT_4, HSSCONTROL_UNLATCHFAULT_AUTORETRY);
		PDU_Mk1_HSSControl_WriteLatch_Ch(PDU_OUTPUT_6, HSSCONTROL_LATCHFAULT_STAYOFF);
		PDU_Mk1_HSSControl_WriteLatch_Ch(PDU_OUTPUT_9, HSSCONTROL_UNLATCHFAULT_AUTORETRY);

		vTaskDelay(pdMS_TO_TICKS(INTERVAL_SR_WRITE_MS));

		// expected EN: 	0000 0000 0000 0000
		PDU_Mk1_HSSControl_AllOff();

		// expected LATCH: 	1111 1101 1011 1111
		PDU_Mk1_HSSControl_WriteLatch_Ch(PDU_OUTPUT_2, HSSCONTROL_LATCH_NOCHANGE);
		PDU_Mk1_HSSControl_WriteLatch_Ch(PDU_OUTPUT_4, HSSCONTROL_LATCHFAULT_STAYOFF);
		PDU_Mk1_HSSControl_WriteLatch_Ch(PDU_OUTPUT_6, HSSCONTROL_UNLATCHFAULT_AUTORETRY);
		PDU_Mk1_HSSControl_WriteLatch_Ch(PDU_OUTPUT_9, HSSCONTROL_LATCH_NOCHANGE);

		vTaskDelay(pdMS_TO_TICKS(INTERVAL_SR_WRITE_MS));

		// expected EN: 	0100 0000 1010 0001
		PDU_Mk1_HSSControl_WriteOutputEN_Ch(PDU_OUTPUT_1, HSSCONTROL_EN_ON);
		PDU_Mk1_HSSControl_WriteOutputEN_Ch(PDU_OUTPUT_8, HSSCONTROL_EN_ON);
		PDU_Mk1_HSSControl_WriteOutputEN_Ch(PDU_OUTPUT_10, HSSCONTROL_EN_TOGGLE);
		PDU_Mk1_HSSControl_WriteOutputEN_Ch(PDU_OUTPUT_11, HSSCONTROL_EN_OFF);
		PDU_Mk1_HSSControl_WriteOutputEN_Ch(PDU_OUTPUT_15, HSSCONTROL_EN_ON);

		// expected LATCH: 	1111 1100 1010 1111
		PDU_Mk1_HSSControl_OutputFaultRetry_Ch(PDU_OUTPUT_11);
		PDU_Mk1_HSSControl_OutputFaultRelatch_Ch(PDU_OUTPUT_12);
		PDU_Mk1_HSSControl_OutputFaultRetry_AllFaulted();

		vTaskDelay(pdMS_TO_TICKS(INTERVAL_SR_WRITE_MS));

		// expected EN: 	0000 0000 0111 0001
		PDU_Mk1_HSSControl_WriteOutputEN_Ch(PDU_OUTPUT_1, HSSCONTROL_EN_OFF);
		PDU_Mk1_HSSControl_WriteOutputEN_Ch(PDU_OUTPUT_8, HSSCONTROL_EN_TOGGLE);
		PDU_Mk1_HSSControl_WriteOutputEN_Ch(PDU_OUTPUT_9, HSSCONTROL_EN_TOGGLE);
		PDU_Mk1_HSSControl_WriteOutputEN_Ch(PDU_OUTPUT_11, HSSCONTROL_EN_ON);
		PDU_Mk1_HSSControl_WriteOutputEN_Ch(PDU_OUTPUT_15, HSSCONTROL_EN_NOCHANGE);

		// expected LATCH: 	1111 1101 1011 0111
		PDU_Mk1_HSSControl_OutputFaultRelatch_Ch(PDU_OUTPUT_11);
		PDU_Mk1_HSSControl_OutputFaultRetry_Ch(PDU_OUTPUT_12);
		PDU_Mk1_HSSControl_OutputFaultRelatch_AllFaulted();

		vTaskDelay(pdMS_TO_TICKS(INTERVAL_SR_WRITE_MS));

		// expected EN: 	1010 0101 1010 1100
		HSSControl_EnState_t en_actions[PDU_MK1_NUM_CHANNELS] = {
			HSSCONTROL_EN_ON,
			HSSCONTROL_EN_NOCHANGE,
			HSSCONTROL_EN_TOGGLE,
			HSSCONTROL_EN_NOCHANGE,
			HSSCONTROL_EN_OFF,
			HSSCONTROL_EN_ON,
			HSSCONTROL_EN_OFF,
			HSSCONTROL_EN_TOGGLE,
			HSSCONTROL_EN_TOGGLE,
			HSSCONTROL_EN_OFF,
			HSSCONTROL_EN_ON,
			HSSCONTROL_EN_TOGGLE,
			HSSCONTROL_EN_ON,
			HSSCONTROL_EN_ON,
			HSSCONTROL_EN_OFF,
			HSSCONTROL_EN_OFF
		};
		PDU_Mk1_HSSControl_WriteOutputEN_All(en_actions);

		// expected LATCH: 	1000 1101 1101 0001
		HSSControl_LatchState_t latch_states[PDU_MK1_NUM_CHANNELS] = {
			HSSCONTROL_LATCH_NOCHANGE,
			HSSCONTROL_UNLATCHFAULT_AUTORETRY,
			HSSCONTROL_UNLATCHFAULT_AUTORETRY,
			HSSCONTROL_UNLATCHFAULT_AUTORETRY,
			HSSCONTROL_LATCH_NOCHANGE,
			HSSCONTROL_LATCH_NOCHANGE,
			HSSCONTROL_UNLATCHFAULT_AUTORETRY,
			HSSCONTROL_LATCHFAULT_STAYOFF,
			HSSCONTROL_LATCHFAULT_STAYOFF,
			HSSCONTROL_LATCHFAULT_STAYOFF,
			HSSCONTROL_UNLATCHFAULT_AUTORETRY,
			HSSCONTROL_LATCHFAULT_STAYOFF,
			HSSCONTROL_UNLATCHFAULT_AUTORETRY,
			HSSCONTROL_UNLATCHFAULT_AUTORETRY,
			HSSCONTROL_UNLATCHFAULT_AUTORETRY,
			HSSCONTROL_LATCHFAULT_STAYOFF
		};
		PDU_Mk1_HSSControl_WriteLatch_All(latch_states);

		vTaskDelay(pdMS_TO_TICKS(INTERVAL_SR_WRITE_MS));
	}
}

// MAIN -----------------------------------------------------------------------

int main()
{
	HAL_Init();
	SystemClock_Config();

	init_task = xTaskCreateStatic(Init_Task,
					"Init Task",
					TASKSTACKSIZE_INIT,
					NULL,
					TASKPRIORITY_INIT,
					init_task_stack,
					&init_task_buffer
				);

	blink_task = xTaskCreateStatic(Blink_Task,
					"Blink Task",
					TASKSTACKSIZE_BLINK,
					NULL,
					TASKPRIORITY_BLINK,
					blink_task_stack,
					&blink_task_buffer
				);

	sr_write_task = xTaskCreateStatic(SR_Write_Task,
					"SR Write Task",
					TASKSTACKSIZE_SR_WRITE,
					NULL,
					TASKPRIORITY_SR_WRITE,
					sr_write_task_stack,
					&sr_write_task_buffer
				);

	vTaskSuspend(blink_task);
	vTaskSuspend(sr_write_task);
	vTaskStartScheduler();

	while(1) {}
}

// ERROR HANDLER --------------------------------------------------------------

void Error_Handler(void)
{
	while(1)
	{
		HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
		vTaskDelay(pdMS_TO_TICKS(INTERVAL_BLINK_ERROR_MS));
	}
}
