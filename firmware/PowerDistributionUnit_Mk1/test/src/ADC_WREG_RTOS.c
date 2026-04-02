#include "stm32xx_hal.h"
// stm32xx_hal.h contains includes for RTOS stuff.
#include "printf.h"
#include "PDU_Mk1_Pins.h"
#include "PDU_Mk1_SPI.h"
#include "ADS131M08-Q1.h"
#include<stdio.h>
#include<string.h>

extern SPI_HandleTypeDef hspi2;

ADS131M08Q1_HandleTypeDef adc;
float adc_results[8];

SemaphoreHandle_t spi2_mutex;           // Mutex to prevent simultaneous SPI access
StaticSemaphore_t spi2_mutex_buffer;    // Static buffer for mutex allocation

SemaphoreHandle_t spi2_done_sem;        // Semaphore to signal SPI DMA/IT completion
StaticSemaphore_t spi2_done_sem_buffer; // Static buffer for completion semaphore

StaticTask_t initTaskBuffer;
StackType_t initTaskStack[configMINIMAL_STACK_SIZE];
StaticTask_t blinkTaskBuffer;
StackType_t blinkTaskStack[configMINIMAL_STACK_SIZE];
StaticTask_t adcWREGTaskBuffer;
StackType_t adcWREGTaskStack[configMINIMAL_STACK_SIZE];

// Initialize clock for heartbeat LED port
void Heartbeat_Clock_Init() {
    switch ((uint32_t)LED_PORT) {
        case (uint32_t)GPIOA:
            __HAL_RCC_GPIOA_CLK_ENABLE();
            break;
        case (uint32_t)GPIOB:
            __HAL_RCC_GPIOB_CLK_ENABLE();
            break;
        case (uint32_t)GPIOC:
            __HAL_RCC_GPIOC_CLK_ENABLE();
            break;
    }
}

// Initialize GPIO and UART
void Init_Task(void *argument)
{
    printf("Starting Initialization...\n");

    GPIO_InitTypeDef led_config = {
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pull = GPIO_NOPULL,
        .Pin = LED_PIN
    };

    GPIO_InitTypeDef adc_sns1_cs = {
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pull = GPIO_NOPULL,
        .Pin = ADC_SNS1_CS_PIN
    };

    Heartbeat_Clock_Init();
    HAL_GPIO_Init(LED_PORT, &led_config); // initialize GPIOA with led_config
    HAL_GPIO_WritePin(LED_PORT, LED_PIN, 0);

    __HAL_RCC_GPIOC_CLK_ENABLE();
    HAL_GPIO_Init(ADC_SNS1_CS_PORT, &adc_sns1_cs);
    HAL_GPIO_WritePin(ADC_SNS1_CS_PORT, ADC_SNS1_CS_PIN, 1);

    // Init UART printf
    husart3->Init.BaudRate = 115200;
    husart3->Init.WordLength = UART_WORDLENGTH_8B;
    husart3->Init.StopBits = UART_STOPBITS_1;
    husart3->Init.Parity = UART_PARITY_NONE;
    husart3->Init.Mode = UART_MODE_TX_RX;
    husart3->Init.HwFlowCtl = UART_HWCONTROL_NONE;
    husart3->Init.OverSampling = UART_OVERSAMPLING_16;

    printf_init(husart3);

    SPI2_ADC_Init();

    // create mutex (prevent simultaneous access to SPI2)
    spi2_mutex = xSemaphoreCreateMutexStatic(&spi2_mutex_buffer);
    //creates semaphore (tells when SPI2 hardware has finished transmission)
    spi2_done_sem = xSemaphoreCreateBinaryStatic(&spi2_done_sem_buffer);

    if(spi2_mutex == NULL || spi2_done_sem == NULL)
    {
        printf("FAIL:MUTEX_SEMAPHORE_NULL\n");
        while(1)
        {
            HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
            HAL_Delay(50);
        }
    }

    if(xSemaphoreTake(spi2_mutex, pdMS_TO_TICKS(100)) != pdTRUE)
    {
        printf("FAIL:SEMAPHORE_TAKE_INIT_SPI_DUMMY_SEND\n");
        while(1)
        {
            HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
            HAL_Delay(50);
        }
    }

    // SPI dummy send because CLK pin initializes high even when configured low for some reason
    uint8_t dummy_spi_send = 0;
    if(HAL_SPI_Transmit_IT(&hspi2, &dummy_spi_send, 1) != HAL_OK)
    {
        printf("FAIL:SPI_TRANSMIT_INIT_SPI_DUMMY_SEND\n");
        while(1)
        {
            HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
            HAL_Delay(50);
        }
    }

    if(xSemaphoreTake(spi2_done_sem, pdMS_TO_TICKS(100)) != pdTRUE)
    {
        printf("FAIL:SEMAPHORE_TAKE_INIT_SPI_DUMMY_SEND\n");
        while(1)
        {
            HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
            HAL_Delay(50);
        }
    }

    xSemaphoreGive(spi2_mutex);

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

    // Task kills itself
    vTaskDelete(NULL);
}

void Blink_Task(void *argument)
{
    for(;;)
    {
        printf("Blink_Task runs...\n");
        HAL_GPIO_TogglePin(LED_PORT, LED_PIN);

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void ADC_WREG_Task(void *argument)
{
    vTaskDelay(10);

    for(;;)
    {
        printf("ADC_WREG_Task runs...\n");

        uint16_t wreg_buf[12] = {0x0000, 0x1234, 0x5600, 0x789A, 0xBC00, 0x0000, 0x2345, 0x6700, 0x89AB, 0xCD00, 0x0000, 0x3456};
        uint16_t rreg_results[12] = {0};

        // TEST SHORT WRITE (should still fill up a frame)
        printf("\nTest short write...\n");
        printf("-----\n");
        if(ADS131M08Q1_WriteRegs(&adc, ADS131M08Q1_REG_CHx_CFG(0), wreg_buf, 5) != ADS131M08Q1_🙂)
        {
            printf("FAIL:SPI_REG_WRITE_5\n");
            while(1)
            {
                HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
                HAL_Delay(50);
            }
        }

        // check readback
        if(ADS131M08Q1_ReadRegs(&adc, ADS131M08Q1_REG_CHx_CFG(0), rreg_results, 5) != ADS131M08Q1_🙂)
        {
            printf("FAIL:SPI_REG_READBACK_5\n");
            while(1)
            {
                HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
                HAL_Delay(50);
            }
        }

        bool ok = true;
        for(uint8_t i = 0; i < 5; i++)
        {
            printf("Register %u: %x \n", i, rreg_results[i]);

            if(rreg_results[i] != wreg_buf[i])
            {
                ok = false;
            }
        }

        if(!ok)
        {
            printf("FAIL:REG_VALUE_MISMATCH_5\n");
            while(1)
            {
                HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
                HAL_Delay(50);
            }
        }

        // TEST LONG WRITE (should extend frame size)
        printf("\nTest long write...\n");
        printf("-----\n");
        if(ADS131M08Q1_WriteRegs(&adc, ADS131M08Q1_REG_CHx_CFG(0), wreg_buf, 12) != ADS131M08Q1_🙂)
        {
            printf("FAIL:SPI_REG_WRITE_12\n");
            while(1)
            {
                HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
                HAL_Delay(50);
            }
        }

        // check readback
        if(ADS131M08Q1_ReadRegs(&adc, ADS131M08Q1_REG_CHx_CFG(0), rreg_results, 12) != ADS131M08Q1_🙂)
        {
            printf("FAIL:SPI_REG_READBACK_12\n");
            while(1)
            {
                HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
                HAL_Delay(50);
            }
        }

        ok = true;
        for(uint8_t i = 0; i < 12; i++)
        {
            printf("Register %u: %x \n", i, rreg_results[i]);

            if(rreg_results[i] != wreg_buf[i])
            {
                ok = false;
            }
        }

        if(!ok)
        {
            printf("FAIL:REG_VALUE_MISMATCH_12\n");
            while(1)
            {
                HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
                HAL_Delay(50);
            }
        }

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
                    initTaskStack,
                    &initTaskBuffer
                );

    xTaskCreateStatic(Blink_Task,
                    "Blink Task",
                    configMINIMAL_STACK_SIZE,
                    NULL,
                    tskIDLE_PRIORITY + 1,
                    blinkTaskStack,
                    &blinkTaskBuffer
                );

    xTaskCreateStatic(ADC_WREG_Task,
                    "ADC WREG Task",
                    configMINIMAL_STACK_SIZE,
                    NULL,
                    tskIDLE_PRIORITY + 1,
                    adcWREGTaskStack,
                    &adcWREGTaskBuffer
                );

    vTaskStartScheduler();

    while(1) {}
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef* hspi)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    xSemaphoreGiveFromISR(spi2_done_sem, &xHigherPriorityTaskWoken);

    // Context switch if a higher priority task was woken up
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef* hspi)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    xSemaphoreGiveFromISR(spi2_done_sem, &xHigherPriorityTaskWoken);

    // Context switch if a higher priority task was woken up
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef* hspi)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    xSemaphoreGiveFromISR(spi2_done_sem, &xHigherPriorityTaskWoken);

    // Context switch if a higher priority task was woken up
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/**
  * @brief This function handles SPI2 global interrupt.
  */
void SPI2_IRQHandler(void)
{
    HAL_SPI_IRQHandler(&hspi2);
}

/**
  * @brief UART MSP Initialization
  * This function configures the hardware resources used in this example
  * @param huart: UART handle pointer
  * @retval None
  */
void HAL_UART_MspGPIOInit(UART_HandleTypeDef* huart)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  if(huart->Instance==USART3)
  {
  /** Initializes the peripherals clocks
  */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART3;
    PeriphClkInit.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }

    /* Peripheral clock enable */
    __HAL_RCC_USART3_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    /**USART3 GPIO Configuration
    PB10     ------> USART3_TX
    PC11     ------> USART3_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  }

}
