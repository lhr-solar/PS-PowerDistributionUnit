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

StaticTask_t initTaskBuffer;
StackType_t initTaskStack[configMINIMAL_STACK_SIZE];
StaticTask_t blinkTaskBuffer;
StackType_t blinkTaskStack[configMINIMAL_STACK_SIZE];
StaticTask_t adcRREGTaskBuffer;
StackType_t adcRREGTaskStack[configMINIMAL_STACK_SIZE];

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
    uint8_t dummy_spi_send = 0;
    HAL_SPI_Transmit(&hspi2, &dummy_spi_send, 1, HAL_MAX_DELAY);
    vTaskDelay(1);

    // printf("Starting SPI intialization...\n");

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

    if(ADS131M08Q1_Reset(&adc) != ADS131M08Q1_🙂)
    {
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
        printf("Hello, I'm running1...\n");
        HAL_GPIO_TogglePin(LED_PORT, LED_PIN);

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void ADC_RREG_Task(void *argument)
{
    vTaskDelay(10);
    for(;;)
    {
        printf("Hello, I'm running2...\n");

        uint16_t rreg_results[14] = {0};
        if(ADS131M08Q1_ReadRegs(&adc, ADS131M08Q1_REG_ID, rreg_results, 14) != ADS131M08Q1_🙂)
        {
            printf("Fail1\n");
            while(1)
            {
                HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
                HAL_Delay(50);
            }
        }

        for(uint8_t i = 0; i < 14; i++)
        {
            printf("Register %u: %x \n", i, rreg_results[i]);
        }

        uint16_t just_id = 0;
        if(ADS131M08Q1_ReadRegs(&adc, ADS131M08Q1_REG_ID, &just_id, 1) != ADS131M08Q1_🙂)
        {
            printf("Fail3");
            while(1)
            {
                HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
                HAL_Delay(50);
            }
        }

        if((just_id >> 8) != 0x28)
        {
            printf("Fail4");
            while(1)
            {
                HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
                HAL_Delay(50);
            }
        }

        // if(ADS131M08Q1_ReadConversionResults(&adc, adc_results) != ADS131M08Q1_🙂)
        // {
        //     while(1)
        //     {
        //         HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
        //         HAL_Delay(50);
        //     }
        // }

        // printf("ADC Conv Results\n----------\nCH0: %.6f V\nCH1: %.6f V\nCH2: %.6f V\nCH3: %.6f V\nCH4: %.6f V\nCH5: %.6f V\nCH6: %.6f V\nCH7: %.6f V\n", adc_results[0], adc_results[1], adc_results[2], adc_results[3], adc_results[4], adc_results[5], adc_results[6], adc_results[7]);
        // printf("ADC Conv Results\n----------\nCH0: %.4f V\nCH1: %.4f V\nCH2: %.4f V\nCH3: %.4f V\nCH4: %.4f V\nCH5: %.4f V\n", adc_results[0], adc_results[1], adc_results[2], adc_results[3], adc_results[4], adc_results[5]);
        // vTaskDelay(pdMS_TO_TICKS(500));
        // printf("CH6: %.4f V\nCH7: %.4f V\n", adc_results[6], adc_results[7]);

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

    xTaskCreateStatic(ADC_RREG_Task,
                    "ADC RREG Task",
                    configMINIMAL_STACK_SIZE,
                    NULL,
                    tskIDLE_PRIORITY + 1,
                    adcRREGTaskStack,
                    &adcRREGTaskBuffer
                );

    vTaskStartScheduler();

    while(1) {}
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
