// LSOM_Blinky.c
// ----------------------------------------------------------------------------
// copied from Embedded-Sharepoint

#include "stm32xx_hal.h"
#include "PDU_Mk1_Pins.h"
#include "PDU_Mk1_SPI.h"
#include "ADS131M08-Q1.h"

extern SPI_HandleTypeDef hspi2;

ADS131M08Q1_HandleTypeDef adc;

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

int main(){
    HAL_Init();

    __HAL_RCC_GPIOC_CLK_ENABLE();
    HAL_GPIO_Init(LED_PORT, &led_config);
    HAL_GPIO_WritePin(LED_PORT, LED_PIN, 0);

    __HAL_RCC_GPIOC_CLK_ENABLE();
    HAL_GPIO_Init(ADC_SNS1_CS_PORT, &adc_sns1_cs);
    HAL_GPIO_WritePin(ADC_SNS1_CS_PORT, ADC_SNS1_CS_PIN, 1);

    SPI2_ADC_Init();

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
    adc.config.powermode = ADS131M08Q1_CONFIG_POWERMODE_DEFAULT;

    adc.spi = &hspi2;
    adc.cs_port = ADC_SNS1_CS_PORT;
    adc.cs_pin = ADC_SNS1_CS_PIN;

    uint8_t dummy_spi_send = 0;
    HAL_SPI_Transmit(&hspi2, &dummy_spi_send, 1, HAL_MAX_DELAY);

    HAL_Delay(2000);

    ADS131M08Q1_Reset(&adc);

    HAL_Delay(2000);

    
    while(1){
        if(ADS131M08Q1_Standby(&adc) == ADS131M08Q1_😢)
        {
            while(1)
            {
                HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
                HAL_Delay(50);
            }
        }

        HAL_GPIO_WritePin(LED_PORT, LED_PIN, 1);
        HAL_Delay(1000);

        if(ADS131M08Q1_Wakeup(&adc) == ADS131M08Q1_😢)
        {
            while(1)
            {
                HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
                HAL_Delay(50);
            }
        }

        HAL_GPIO_WritePin(LED_PORT, LED_PIN, 0);
        HAL_Delay(1000);
    }

    return 0;
}
