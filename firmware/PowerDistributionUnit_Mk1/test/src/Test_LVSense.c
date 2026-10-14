// Test_HSSControl.c
// ----------------------------------------------------------------------------
// Runs HSSControl task (which just turns all outputs on for now).

// INCLUDES -------------------------------------------------------------------

#include "stm32xx_hal.h"
// stm32xx_hal.h contains includes for RTOS stuff.
#include<stdio.h>
#include<string.h>

// BBPDU peripherals
#include "PDU_Mk1_GPIO.h"
#include "PDU_Mk1_UART.h"
#include "PDU_Mk1_LVSense.h"

// Tasks
#include "Task_Blink.h"
#include "Task_LVSense.h"

// DEFINES --------------------------------------------------------------------

#define TASK_INIT_STACK_SIZE configMINIMAL_STACK_SIZE+1000
#define TASK_INIT_PRIORITY tskIDLE_PRIORITY + 3

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
StaticTask_t init_task_buffer;                          // Task buffer
StackType_t init_task_stack[TASK_INIT_STACK_SIZE];      // Task stack

// Task: Blink
StaticTask_t task_blink_buffer;
StackType_t task_blink_stack[TASK_BLINK_STACK_SIZE];

// Task: LVSense
StaticTask_t task_lvsense_buffer;
StackType_t task_lvsense_stack[TASK_LVSENSE_STACK_SIZE];

// Task: HSS Output Control
// StaticTask_t task_hsscontrol_buffer;
// StackType_t task_hsscontrol_stack[TASK_HSSCONTROL_STACK_SIZE];

// INIT TASK ------------------------------------------------------------------

void Task_Init(void *argument)
{
    PDU_Mk1_GPIO_Init();

    if(PDU_Mk1_UART_Printf_Init() != true)
    {
        printf("FAIL:UART_PRINTF_INIT\n");
        Error_Handler();
    }

    if(PDU_Mk1_ADC_LVSense_Init() != true)
    {
        printf("FAIL:ADC_LVSENSE_INIT\n");
        Error_Handler();
    }

    printf("Initialization complete.\n");

    // get this party started
    xTaskCreateStatic(Task_Blink,
                    "Blink Task",
                    TASK_BLINK_STACK_SIZE,
                    NULL,
                    TASK_BLINK_PRIORITY,
                    task_blink_stack,
                    &task_blink_buffer
                );

    xTaskCreateStatic(Task_LVSense,
                    "LV Voltage Sense Task",
                    TASK_LVSENSE_STACK_SIZE,
                    NULL,
                    TASK_LVSENSE_PRIORITY,
                    task_lvsense_stack,
                    &task_lvsense_buffer
                );

    // task kills itself
    vTaskDelete(NULL);
}

// MAIN -----------------------------------------------------------------------

int main()
{
    HAL_Init();
    SystemClock_Config();

    xTaskCreateStatic(Task_Init,
                    "Init Task",
                    TASK_INIT_STACK_SIZE,
                    NULL,
                    TASK_INIT_PRIORITY,
                    init_task_stack,
                    &init_task_buffer
                );

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
