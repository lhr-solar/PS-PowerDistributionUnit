// PDU_Mk1_LVSense.c

#include "PDU_Mk1_LVSense.h"

uint8_t readings_queuestorage[QUEUE_LENGTH * ITEM_SIZE];
static StaticQueue_t readings_staticqueue;

QueueHandle_t readings_queue;

ADC_ChannelConfTypeDef lvsense_adc_ch_config = {
    .Channel = ADC_CHANNEL_11,
    .SamplingTime = ADC_SAMPLETIME_24CYCLES_5
};

uint32_t lvsense_reading = 0;
float lv_voltage = 0.0f;

bool PDU_Mk1_ADC_LVSense_Init(void)
{
	readings_queue = xQueueCreateStatic(QUEUE_LENGTH, ITEM_SIZE, readings_queuestorage, &readings_staticqueue);

    GPIO_InitTypeDef lvsense = {
        .Pin = LVSENSE_ADC_PIN,
        .Mode = GPIO_MODE_ANALOG,
        .Pull = GPIO_NOPULL,
    };

    HAL_GPIO_Init(LVSENSE_ADC_PORT, &lvsense);

    HAL_ADC_MspInit(hadc1);

    ADC_InitTypeDef adc1_init = {0};

    adc1_init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
    adc1_init.Resolution = ADC_RESOLUTION_12B;
    adc1_init.DataAlign = ADC_DATAALIGN_RIGHT;
    adc1_init.ScanConvMode = ADC_SCAN_DISABLE;
    adc1_init.EOCSelection = ADC_EOC_SINGLE_CONV;
    adc1_init.LowPowerAutoWait = DISABLE;
    adc1_init.ContinuousConvMode = DISABLE;
    adc1_init.NbrOfConversion = 1;
    adc1_init.DiscontinuousConvMode = DISABLE;
    adc1_init.ExternalTrigConv = ADC_SOFTWARE_START;
    adc1_init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    adc1_init.DMAContinuousRequests = DISABLE;
    adc1_init.Overrun = ADC_OVR_DATA_PRESERVED;
    adc1_init.OversamplingMode = DISABLE;

    if(adc_init(&adc1_init, hadc1) != ADC_OK)
    {
        return false;
    }

    ADC_MultiModeTypeDef multimode = {0};
    multimode.Mode = ADC_MODE_INDEPENDENT;
    
    if (HAL_ADCEx_MultiModeConfigChannel(hadc1, &multimode) != HAL_OK)
    {
        return false;
    }

    return true;
}

void HAL_ADC_MspInit(ADC_HandleTypeDef *h) {
    __HAL_RCC_SYSCFG_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();
    
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    if (h->Instance == ADC1 || h->Instance == ADC2) { 
        PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC12;
        PeriphClkInit.Adc12ClockSelection = RCC_ADC12CLKSOURCE_SYSCLK;
        __HAL_RCC_ADC12_CLK_ENABLE(); 
        if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) Error_Handler();
    }   
    else if (h->Instance == ADC3 || h->Instance == ADC4 || h->Instance == ADC5) {
        PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC345;
        PeriphClkInit.Adc345ClockSelection = RCC_ADC345CLKSOURCE_SYSCLK;
        __HAL_RCC_ADC345_CLK_ENABLE(); 
        if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) Error_Handler();
    }

    // ADC 1 Interrupt
    HAL_NVIC_SetPriority(ADC1_2_IRQn, ADC_INT_PRI_LVSENSE, 0);
    HAL_NVIC_EnableIRQ(ADC1_2_IRQn);
}

bool PDU_Mk1_LVSense_CollectMeasurement()
{
    if(adc_read(hadc1, &lvsense_adc_ch_config, readings_queue) != ADC_OK)
    {
        return false;
    }

    xQueueReceive(readings_queue, &lvsense_reading, 0);

    return true;
}

float PDU_Mk1_LVSense_ConvertReading(uint32_t adc_reading)
{
    return (adc_reading * LVSENSE_ADC_CONVERSION_FACTOR * LVSENSE_RDIV_CONVERSION_FACTOR);
}
