// Current_Sensing.c
// ----------------------------------------------------------------------------
// Senses current through each channel on BBPDU Mk1 using the hall-effect  
// current sensor (not HSS one) and prints to the serial monitor. 

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
#include "PDU_Mk1_CurrentSensing.h"

// drivers
#include "ADS131M08-Q1.h"
#include "ACS3704x-010B3.h"

// DEFINES --------------------------------------------------------------------

#define TASKPRIORITY_INIT tskIDLE_PRIORITY + 3
#define TASKSTACKSIZE_INIT configMINIMAL_STACK_SIZE+5000

#define TASKPRIORITY_BLINK tskIDLE_PRIORITY + 2
#define TASKSTACKSIZE_BLINK configMINIMAL_STACK_SIZE

#define TASKPRIORITY_CURRENT_SENSE tskIDLE_PRIORITY + 2
#define TASKSTACKSIZE_CURRENT_SENSE configMINIMAL_STACK_SIZE+3000

#define INTERVAL_BLINK_MS 500
#define INTERVAL_BLINK_ERROR_MS 50
#define INTERVAL_CURRENT_SENSE_MS 500

// DECLARATIONS ---------------------------------------------------------------

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;
SPI_HandleTypeDef hspi3;

extern ADS131M08Q1_HandleTypeDef adc_sns0;
extern ADS131M08Q1_HandleTypeDef adc_sns1;
float adc_results_sns0[8];
float adc_results_sns1[8];

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

TaskHandle_t current_sense_task;
StaticTask_t current_sense_task_buffer;
StackType_t current_sense_task_stack[TASKSTACKSIZE_CURRENT_SENSE];

// TASKS ----------------------------------------------------------------------

// initialize stuff
void Init_Task(void *argument)
{
    PDU_Mk1_GPIO_Init();
    
    if(PDU_Mk1_SPI2_ADC_Init() != true)
    {
        printf("FAIL:SPI2_INIT\n");
        Error_Handler();
    }

#if (PDU_MK1_REV_A == false)    // PDU_Mk1_REV_A has SPI pinout issue
    if(PDU_Mk1_SPI3_ADC_Init() != true)
    {
        printf("FAIL:SPI3_INIT\n");
        Error_Handler();
    }
#endif
    
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
        printf("FAIL:SPI2_INIT_DUMMY_SEND\n");
        Error_Handler();
    }

#if (PDU_MK1_REV_A == false)    // PDU_Mk1_REV_A has SPI pinout issue
    if(SPI_RTOS_Mutex_Semaphore_Setup(&spi3_mutex, &spi3_mutex_buffer, &spi3_done_sem, &spi3_done_sem_buffer) != true)
    {
        printf("FAIL:MUTEX_SEMAPHORE_INIT\n");
        Error_Handler();
    }

    // SPI dummy send because CLK pin initializes high even when configured low for some reason
    if(SPI_Init_Dummy_Send(&hspi3, spi3_mutex, spi3_done_sem) != true)
    {
        printf("FAIL:SPI3_INIT_DUMMY_SEND\n");
        while(1)
        {
            HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
            vTaskDelay(pdMS_TO_TICKS(INTERVAL_BLINK_ERROR_MS));
        }
    }
#endif

    if(PDU_Mk1_CurrentSensing_Init() != true)
    {
        printf("FAIL:ISENSE_INIT\n");
        Error_Handler();
    }

    printf("Initialization complete.\n");

    // get this party started
    vTaskResume(blink_task);
    vTaskResume(current_sense_task);
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

// read currents and print to serial
void Current_Sense_Task(void *argument)
{
    // magic delay required to make things work for some reason
    vTaskDelay(pdMS_TO_TICKS(1));
    
    for(;;)
    {
        if(PDU_Mk1_CurrentSensing_ReadCurrents() != true)
        {
            printf("FAIL:CURRENT_SAMPLING\n");
            Error_Handler();
        }

        float* currents = PDU_Mk1_CurrentSensing_GetCurrentsPtr();

        printf("\nCurrents\n----------\n"
                "CH0: %.4f A\n"
                "CH1: %.4f A\n"
                "CH2: %.4f A\n"
                "CH3: %.4f A\n"
                "CH4: %.4f A\n"
                "CH5: %.4f A\n"
                "CH6: %.4f A\n"
                "CH7: %.4f A\n" 
                "CH8: %.4f A\n"
                "CH9: %.4f A\n"
                "CH10: %.4f A\n"
                "CH11: %.4f A\n"
                "CH12: %.4f A\n"
                "CH13: %.4f A\n"
                "CH14: %.4f A\n"
                "CH15: %.4f A\n", 
                currents[0], 
                currents[1], 
                currents[2], 
                currents[3], 
                currents[4], 
                currents[5], 
                currents[6], 
                currents[7], 
                currents[8], 
                currents[9], 
                currents[10], 
                currents[11], 
                currents[12], 
                currents[13], 
                currents[14],
                currents[15]);
        
        vTaskDelay(pdMS_TO_TICKS(INTERVAL_CURRENT_SENSE_MS));
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

    current_sense_task = xTaskCreateStatic(Current_Sense_Task,
                    "Current Sense Task",
                    TASKSTACKSIZE_CURRENT_SENSE,
                    NULL,
                    TASKPRIORITY_CURRENT_SENSE,
                    current_sense_task_stack,
                    &current_sense_task_buffer
                );

    vTaskSuspend(blink_task);
    vTaskSuspend(current_sense_task);
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
