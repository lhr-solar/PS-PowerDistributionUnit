// ADS131M08Q1_LockUnlock_Status.c
// ----------------------------------------------------------------------------
// Locks and unlocks the ADC SPI interface for ADC_SNS1 on BBPDU Mk1. using 
// their respective commands. Checks the LOCK bit in the status register to 
// verify (un)locking is successful. 

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
#include "PDU_Mk1_Current_Sensing.h"

// drivers
#include "ADS131M08-Q1.h"

// DEFINES --------------------------------------------------------------------

#define TASKPRIORITY_INIT tskIDLE_PRIORITY + 2
#define TASKSTACKSIZE_INIT configMINIMAL_STACK_SIZE+1500

#define TASKPRIORITY_BLINK tskIDLE_PRIORITY + 2
#define TASKSTACKSIZE_BLINK configMINIMAL_STACK_SIZE

#define TASKPRIORITY_ADC_LOCKUNLOCK_STATUS tskIDLE_PRIORITY + 2
#define TASKSTACKSIZE_ADC_LOCKUNLOCK_STATUS configMINIMAL_STACK_SIZE+200

#define INTERVAL_BLINK_MS 500
#define INTERVAL_BLINK_ERROR_MS 50
#define INTERVAL_ADC_LOCKUNLOCK_STATUS 500

#define ADS131M08Q1_STATUS_LOCK_MASK 0x8000

// DECLARATIONS ---------------------------------------------------------------

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;
SPI_HandleTypeDef hspi3;

extern ADS131M08Q1_HandleTypeDef adc_sns1;

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

TaskHandle_t adc_lock_unlock_status_task;
StaticTask_t adc_lock_unlock_status_task_buffer;
StackType_t adc_lock_unlock_status_task_stack[TASKSTACKSIZE_ADC_LOCKUNLOCK_STATUS];

// TASKS ----------------------------------------------------------------------

// initialize stuff
void Init_Task(void *argument)
{
    printf("Starting Initialization...\n");

    PDU_Mk1_GPIO_Init();
    
    if(PDU_Mk1_SPI2_ADC_Init() != true)
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

    if(PDU_Mk1_Current_Sensing_Init() != true)
    {
        printf("FAIL:ISENSE_INIT\n");
        Error_Handler();
    }

    printf("Initialization complete.\n");

    // get this party started
    vTaskResume(blink_task);
    vTaskResume(adc_lock_unlock_status_task);
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

// lock and unlock ADC SPI interface, checking if successful by bit on STATUS register
void ADC_LockUnlock_Task(void *argument)
{
    uint16_t status = 0;
    
    for(;;)
    {
        if(ADS131M08Q1_Lock(&adc_sns1) == ADS131M08Q1_😢)
        {
            printf("FAIL:LOCK_CMD\n");
            Error_Handler();
        }

        ADS131M08Q1_ReadStatus(&adc_sns1, &status);
        // validate SPI interface locked using STATUS bit
        if(!(status & ADS131M08Q1_STATUS_LOCK_MASK))
        {
            printf("FAIL:LOCK_STATUS\n");
            Error_Handler();
        }
        printf("Lock successful.\n");

        vTaskDelay(pdMS_TO_TICKS(INTERVAL_ADC_LOCKUNLOCK_STATUS));

        if(ADS131M08Q1_Unlock(&adc_sns1) == ADS131M08Q1_😢)
        {
            printf("FAIL:UNLOCK_CMD\n");
            Error_Handler();
        }

        ADS131M08Q1_ReadStatus(&adc_sns1, &status);
        // validate SPI interface unlocked using STATUS bit
        if(status & ADS131M08Q1_STATUS_LOCK_MASK)
        {
            printf("FAIL:UNLOCK_STATUS\n");
            Error_Handler();
        }
        printf("Unlock successful.\n");

        vTaskDelay(pdMS_TO_TICKS(INTERVAL_ADC_LOCKUNLOCK_STATUS));
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

    adc_lock_unlock_status_task = xTaskCreateStatic(ADC_LockUnlock_Task,
                    "ADC Standby Wakeup Task",
                    TASKSTACKSIZE_ADC_LOCKUNLOCK_STATUS,
                    NULL,
                    TASKPRIORITY_ADC_LOCKUNLOCK_STATUS,
                    adc_lock_unlock_status_task_stack,
                    &adc_lock_unlock_status_task_buffer
                );

    vTaskSuspend(blink_task);
    vTaskSuspend(adc_lock_unlock_status_task);
    vTaskStartScheduler();

    while(1) {}
}

// ERROR HANDLER --------------------------------------------------------------

void Error_Handler(void)
{
    __disable_irq();
    while(1)
    {
        HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
        vTaskDelay(pdMS_TO_TICKS(INTERVAL_BLINK_ERROR_MS));
    }
}
