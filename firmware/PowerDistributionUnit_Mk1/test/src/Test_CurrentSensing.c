// Test_CurrentSensing.c
// ----------------------------------------------------------------------------
// Tests current sensing thorugh ADCs and hall-effect current sensors. Prints
// to UART. (Make sure PDU_MK1_PRINT_STATUS_VIA_UART is set to true in 
// PDU_Mk1.h) 
// Prerequisites: ACS_Current_Sensing

// INCLUDES -------------------------------------------------------------------

#include "stm32xx_hal.h"
// stm32xx_hal.h contains includes for RTOS stuff.
#include<stdio.h>
#include<string.h>

// Tasks
#include "Task_Init.h"
#include "Task_Blink.h"
#include "Task_ReadCurrents.h"

// DEFINES --------------------------------------------------------------------

#define INTERVAL_BLINK_ERROR_MS 50

// DECLARATIONS ---------------------------------------------------------------

// SPI1
SPI_HandleTypeDef hspi1;                // Handle
SemaphoreHandle_t spi1_mutex;           // Mutex to prevent simultaneous SPI access
StaticSemaphore_t spi1_mutex_buffer;    // SPI mutex buffer
SemaphoreHandle_t spi1_done_sem;        // Semaphore to signal SPI IT completion
StaticSemaphore_t spi1_done_sem_buffer; // SPI completion sempahore buffer

// SPI2
SPI_HandleTypeDef hspi2;
SemaphoreHandle_t spi2_mutex;
StaticSemaphore_t spi2_mutex_buffer;
SemaphoreHandle_t spi2_done_sem;
StaticSemaphore_t spi2_done_sem_buffer;

// SPI3
SPI_HandleTypeDef hspi3;
SemaphoreHandle_t spi3_mutex;
StaticSemaphore_t spi3_mutex_buffer;
SemaphoreHandle_t spi3_done_sem;
StaticSemaphore_t spi3_done_sem_buffer;

// Task: Init
TaskHandle_t init_task;
StaticTask_t init_task_buffer;                      // Task buffer
StackType_t init_task_stack[TASK_INIT_STACK_SIZE];    // Task stack

// Task: Blink
TaskHandle_t task_blink;
StaticTask_t task_blink_buffer;
StackType_t task_blink_stack[TASK_BLINK_STACK_SIZE];

// Task: Read Currents
TaskHandle_t task_readcurrents;
StaticTask_t task_readcurrents_buffer;
StackType_t task_readcurrents_stack[TASK_READCURRENTS_STACK_SIZE];

// MAIN -----------------------------------------------------------------------

int main()
{
    HAL_Init();
    SystemClock_Config();

    init_task = xTaskCreateStatic(Task_Init,
                    "Init Task",
                    TASK_INIT_STACK_SIZE,
                    NULL,
                    TASK_INIT_PRIORITY,
                    init_task_stack,
                    &init_task_buffer
                );

    task_blink = xTaskCreateStatic(Task_Blink,
                    "Blink Task",
                    TASK_BLINK_STACK_SIZE,
                    NULL,
                    TASK_BLINK_PRIORITY,
                    task_blink_stack,
                    &task_blink_buffer
                );

    task_readcurrents = xTaskCreateStatic(Task_ReadCurrents,
                    "Current Sense Task",
                    TASK_READCURRENTS_STACK_SIZE,
                    NULL,
                    TASK_READCURRENTS_PRIORITY,
                    task_readcurrents_stack,
                    &task_readcurrents_buffer
                );

    vTaskSuspend(task_blink);
    vTaskSuspend(task_readcurrents);
    vTaskStartScheduler();

    while(1) {}
}

void PDU_Mk1_StartTasks()
{
    vTaskResume(task_blink);
    vTaskResume(task_readcurrents);
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
