#include "stm32xx_hal.h"
// stm32xx_hal.h contains includes for RTOS stuff.
#include<stdio.h>
#include<string.h>

#include "PDU_Mk1_Pins.h"
#include "PDU_Mk1_SPI.h"
#include "PDU_Mk1_GPIO.h"
#include "PDU_Mk1_UART.h"

#include "ADS131M08-Q1.h"

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;
SPI_HandleTypeDef hspi3;

ADS131M08Q1_HandleTypeDef adc;
float adc_results[8];

SemaphoreHandle_t spi1_mutex;           // Mutex to prevent simultaneous SPI access
StaticSemaphore_t spi1_mutex_buffer;    // Static buffer for mutex allocation

SemaphoreHandle_t spi1_done_sem;        // Semaphore to signal SPI DMA/IT completion
StaticSemaphore_t spi1_done_sem_buffer; // Static buffer for completion semaphore

SemaphoreHandle_t spi2_mutex;           // Mutex to prevent simultaneous SPI access
StaticSemaphore_t spi2_mutex_buffer;    // Static buffer for mutex allocation

SemaphoreHandle_t spi2_done_sem;        // Semaphore to signal SPI DMA/IT completion
StaticSemaphore_t spi2_done_sem_buffer; // Static buffer for completion semaphore

SemaphoreHandle_t spi3_mutex;           // Mutex to prevent simultaneous SPI access
StaticSemaphore_t spi3_mutex_buffer;    // Static buffer for mutex allocation

SemaphoreHandle_t spi3_done_sem;        // Semaphore to signal SPI DMA/IT completion
StaticSemaphore_t spi3_done_sem_buffer; // Static buffer for completion semaphore

TaskHandle_t init_task;
StaticTask_t init_task_buffer;
StackType_t init_task_stack[configMINIMAL_STACK_SIZE];

TaskHandle_t blink_task;
StaticTask_t blink_task_buffer;
StackType_t blink_task_stack[configMINIMAL_STACK_SIZE];

TaskHandle_t adc_read_task;
StaticTask_t adc_read_task_buffer;
StackType_t adc_read_task_stack[configMINIMAL_STACK_SIZE];


// Initialize GPIO and UART
void Init_Task(void *argument)
{
    printf("Starting Initialization...\n");

    PDU_Mk1_GPIO_Init();
    
    if(PDU_Mk1_SPI2_ADC_Init() != true)
    {
        printf("FAIL:SPI_INIT\n");
        while(1)
        {
            HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
            HAL_Delay(50);
        }
    }
    
    if(PDU_Mk1_UART_Printf_Init() != true)
    {
        printf("FAIL:UART_PRINTF_INIT\n");
        while(1)
        {
            HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
            HAL_Delay(50);
        }
    }

    if(SPI_RTOS_Mutex_Semaphore_Setup(&spi2_mutex, &spi2_mutex_buffer, &spi2_done_sem, &spi2_done_sem_buffer) != true)
    {
        printf("FAIL:MUTEX_SEMAPHORE_INIT\n");
        while(1)
        {
            HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
            HAL_Delay(50);
        }
    }

    // SPI dummy send because CLK pin initializes high even when configured low for some reason
    if(SPI_Init_Dummy_Send(&hspi2, spi2_mutex, spi2_done_sem) != true)
    {
        printf("FAIL:SPI_INIT_DUMMY_SEND\n");
        while(1)
        {
            HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
            HAL_Delay(50);
        }
    }

    for(uint8_t ch = 0; ch < ADS131M08Q1_NUM_CHANNELS; ch++)
    {
        adc.config.ch_configs[ch].enable = ADS131M08Q1_CH_ENABLE;
        adc.config.ch_configs[ch].gain = ADS131M08Q1_CH_GAIN_1;
        adc.config.ch_configs[ch].phase_delay = ADS131M08Q1_CH_PHASE_DELAY_DEFAULT;
        adc.config.ch_configs[ch].offset_cal = ADS131M08Q1_CH_OFFSET_CAL_DEFAULT;
        adc.config.ch_configs[ch].gain_cal = ADS131M08Q1_CH_GAIN_CAL_DEFAULT;
    }
    adc.config.drdy_format = ADS131M08Q1_CONFIG_DRDY_FORMAT_DEFAULT;
    adc.config.drdy_idlepinstate = ADS131M08Q1_CONFIG_DRDY_IDLEPINSTATE_DEFAULT;
    adc.config.drdy_source = ADS131M08Q1_CONFIG_DRDY_SOURCE_DEFAULT;
    adc.config.reference_source = ADS131M08Q1_CONFIG_REFERENCE_SOURCE_DEFAULT;
    adc.config.fsr = 3.3;
    adc.config.powermode = ADS131M08Q1_CONFIG_POWERMODE_DEFAULT;

    adc.spi = &hspi2;
    adc.cs_port = ADC_SNS1_CS_PORT;
    adc.cs_pin = ADC_SNS1_CS_PIN;

    adc.spi_mutex = spi2_mutex;
    adc.spi_done_sem = spi2_done_sem;

    if(ADS131M08Q1_Reset(&adc) != ADS131M08Q1_🙂)
    {
        printf("FAIL:ADC_INIT_RESET\n");
        while(1)
        {
            HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
            HAL_Delay(50);
        }
    }

    printf("Initialization complete.\n");

    // get this party started
    vTaskResume(blink_task);
    vTaskResume(adc_read_task);
    // task kills itself
    vTaskDelete(NULL);
}

void Blink_Task(void *argument)
{
    for(;;)
    {
        HAL_GPIO_TogglePin(LED_PORT, LED_PIN);

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void ADC_Read_Task(void *argument)
{
    vTaskDelay(10);

    for(;;)
    {
        if(ADS131M08Q1_ReadConversionResults(&adc, adc_results) != ADS131M08Q1_🙂)
        {
            while(1)
            {
                HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
                HAL_Delay(50);
            }
        }

        printf("\nADC Conv Results\n----------\nCH0: %.4f V\nCH1: %.4f V\nCH2: %.4f V\nCH3: %.4f V\nCH4: %.4f V\nCH5: %.4f V\n", adc_results[0], adc_results[1], adc_results[2], adc_results[3], adc_results[4], adc_results[5]);

        vTaskDelay(pdMS_TO_TICKS(500));

    }
}

int main()
{
    HAL_Init();
    SystemClock_Config();

    xTaskCreateStatic(Init_Task,
                    "Init Task",
                    configMINIMAL_STACK_SIZE,
                    NULL,
                    tskIDLE_PRIORITY + 2,
                    init_task_stack,
                    &init_task_buffer
                );

    blink_task = xTaskCreateStatic(Blink_Task,
                    "Blink Task",
                    configMINIMAL_STACK_SIZE,
                    NULL,
                    tskIDLE_PRIORITY + 1,
                    blink_task_stack,
                    &blink_task_buffer
                );

    adc_read_task = xTaskCreateStatic(ADC_Read_Task,
                    "ADC Read Task",
                    configMINIMAL_STACK_SIZE,
                    NULL,
                    tskIDLE_PRIORITY + 1,
                    adc_read_task_stack,
                    &adc_read_task_buffer
                );

    vTaskSuspend(blink_task);
    vTaskSuspend(adc_read_task);
    vTaskStartScheduler();

    while(1) {}
}
