// ADS131M08Q1_Convert.c
// ----------------------------------------------------------------------------
// Reads ADC conversions in continuous-conversion mode every 500 ms. Prints
// out results to serial monitor. 

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

#define TASKPRIORITY_INIT tskIDLE_PRIORITY + 3
#define TASKSTACKSIZE_INIT configMINIMAL_STACK_SIZE+1800

#define TASKPRIORITY_BLINK tskIDLE_PRIORITY + 2
#define TASKSTACKSIZE_BLINK configMINIMAL_STACK_SIZE

#define TASKPRIORITY_ADC_READ tskIDLE_PRIORITY + 2
#define TASKSTACKSIZE_ADC_READ configMINIMAL_STACK_SIZE+1200

#define INTERVAL_BLINK_MS 500
#define INTERVAL_BLINK_ERROR_MS 50
#define INTERVAL_ADC_READ_MS 500

// DECLARATIONS ---------------------------------------------------------------

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;
SPI_HandleTypeDef hspi3;

extern ADS131M08Q1_HandleTypeDef adc_sns1;
float adc_results[8];

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

TaskHandle_t adc_read_task;
StaticTask_t adc_read_task_buffer;
StackType_t adc_read_task_stack[TASKSTACKSIZE_ADC_READ];

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
    vTaskResume(adc_read_task);
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
void ADC_Read_Task(void *argument)
{
    for(;;)
    {
        if(ADS131M08Q1_ReadConversionResults(&adc_sns1, adc_results) != ADS131M08Q1_🙂)
        {
            printf("FAIL:CONV_RESULTS\n");
            Error_Handler();
        }

        printf("\nADC Conv Results\n----------\nCH0: %.4f V\nCH1: %.4f V\nCH2: %.4f V\nCH3: %.4f V\nCH4: %.4f V\nCH5: %.4f V\nCH6: %.4f V\nCH7: %.4f V\n", adc_results[0], adc_results[1], adc_results[2], adc_results[3], adc_results[4], adc_results[5], adc_results[6], adc_results[7]);
        vTaskDelay(pdMS_TO_TICKS(INTERVAL_ADC_READ_MS));
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

    adc_read_task = xTaskCreateStatic(ADC_Read_Task,
                    "ADC Read Task",
                    TASKSTACKSIZE_ADC_READ,
                    NULL,
                    TASKPRIORITY_ADC_READ,
                    adc_read_task_stack,
                    &adc_read_task_buffer
                );

    vTaskSuspend(blink_task);
    vTaskSuspend(adc_read_task);
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
