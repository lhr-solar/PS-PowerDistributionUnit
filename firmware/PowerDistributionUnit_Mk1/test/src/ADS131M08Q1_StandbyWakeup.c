// ADS131M08Q1_StandbyWakeup.c
// ----------------------------------------------------------------------------
// Puts the ADC in and out of standby. Verify this is successful by probing
// the ~DRDY pin using a logic analyzer. ~DRDY should stay high when in
// standby, and spend most of the time low otherwise. 

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
#include "ADS131M08-Q1.h"

// DEFINES --------------------------------------------------------------------

#define TASKPRIORITY_INIT tskIDLE_PRIORITY + 2
#define TASKSTACKSIZE_INIT configMINIMAL_STACK_SIZE+1500

#define TASKPRIORITY_BLINK tskIDLE_PRIORITY + 2
#define TASKSTACKSIZE_BLINK configMINIMAL_STACK_SIZE

#define TASKPRIORITY_ADC_STANDBYWAKEUP tskIDLE_PRIORITY + 2
#define TASKSTACKSIZE_ADC_STANDBYWAKEUP configMINIMAL_STACK_SIZE

#define INTERVAL_BLINK_MS 500
#define INTERVAL_BLINK_ERROR_MS 50
#define INTERVAL_ADC_STANDBYWAKEUP 500

// DECLARATIONS ---------------------------------------------------------------

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;
SPI_HandleTypeDef hspi3;

ADS131M08Q1_HandleTypeDef adc;
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

TaskHandle_t adc_standby_wakeup_task;
StaticTask_t adc_standby_wakeup_task_buffer;
StackType_t adc_standby_wakeup_task_stack[TASKSTACKSIZE_ADC_STANDBYWAKEUP];

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

    for(uint8_t ch = 0; ch < ADS131M08Q1_NUM_CHANNELS; ch++)
    {
        adc.config.ch_configs[ch].enable = ADS131M08Q1_CH_ENABLE;
        adc.config.ch_configs[ch].gain = ADS131M08Q1_CH_GAIN_1;
        adc.config.ch_configs[ch].phase = ADS131M08Q1_CH_PHASE_DEFAULT;
        adc.config.ch_configs[ch].offset_cal = ADS131M08Q1_CH_OFFSET_CAL_DEFAULT;
        adc.config.ch_configs[ch].gain_cal = ADS131M08Q1_CH_GAIN_CAL_DEFAULT;
    }
    adc.config.drdy_format = ADS131M08Q1_CONFIG_DRDY_FORMAT_DEFAULT;
    adc.config.drdy_idlepinstate = ADS131M08Q1_CONFIG_DRDY_IDLEPINSTATE_DEFAULT;
    adc.config.drdy_source = ADS131M08Q1_CONFIG_DRDY_SOURCE_DEFAULT;
    adc.config.reference_source = ADS131M08Q1_CONFIG_REFERENCE_SOURCE_DEFAULT;
    adc.config.fsr = ADS131M08Q1_INTERNAL_REFERENCE_V;
    adc.config.powermode = ADS131M08Q1_CONFIG_POWERMODE_DEFAULT;

    adc.spi = &hspi2;
    adc.cs_port = ADC_SNS1_CS_PORT;
    adc.cs_pin = ADC_SNS1_CS_PIN;

    adc.spi_mutex = spi2_mutex;
    adc.spi_done_sem = spi2_done_sem;

    // channel 0 reads about 0.24 V high for some reason
    adc.config.ch_configs[0].offset_cal = ADS131M08Q1_CalcOffsetCalRegValue(&adc, -0.24);

    if(ADS131M08Q1_Init(&adc) != ADS131M08Q1_🙂)
    {
        printf("FAIL:ADC_INIT\n");
        Error_Handler();
    }

    printf("Initialization complete.\n");

    // get this party started
    vTaskResume(blink_task);
    vTaskResume(adc_standby_wakeup_task);
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

// put ADC in and out of standby
void ADC_StandbyWakeup_Task(void *argument)
{
    for(;;)
    {
        if(ADS131M08Q1_Standby(&adc) == ADS131M08Q1_😢)
        {
            printf("FAIL:STANDBY_CMD\n");
            Error_Handler();
        }
        printf("ADC in standby.\n");

        vTaskDelay(pdMS_TO_TICKS(INTERVAL_ADC_STANDBYWAKEUP));

        if(ADS131M08Q1_Wakeup(&adc) == ADS131M08Q1_😢)
        {
            printf("FAIL:WAKEUP_CMD\n");
            Error_Handler();
        }
        printf("ADC out of standby.\n");

        vTaskDelay(pdMS_TO_TICKS(INTERVAL_ADC_STANDBYWAKEUP));
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

    adc_standby_wakeup_task = xTaskCreateStatic(ADC_StandbyWakeup_Task,
                    "ADC Standby Wakeup Task",
                    TASKSTACKSIZE_ADC_STANDBYWAKEUP,
                    NULL,
                    TASKPRIORITY_ADC_STANDBYWAKEUP,
                    adc_standby_wakeup_task_stack,
                    &adc_standby_wakeup_task_buffer
                );

    vTaskSuspend(blink_task);
    vTaskSuspend(adc_standby_wakeup_task);
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
