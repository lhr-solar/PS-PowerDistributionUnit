// PDU_Mk1_GPIO.c

#include "PDU_Mk1_GPIO.h"

void PDU_Mk1_GPIO_Init(void)
{
    GPIO_InitTypeDef led_config = {
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pull = GPIO_NOPULL,
        .Pin = LED_PIN
    };

    LED_CLOCK_INIT();
    HAL_GPIO_Init(LED_PORT, &led_config); // initialize GPIOA with led_config
    HAL_GPIO_WritePin(LED_PORT, LED_PIN, 0);

    GPIO_InitTypeDef adc_sns1_cs = {
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pull = GPIO_NOPULL,
        .Pin = ADC_SNS1_CS_PIN
    };

    __HAL_RCC_GPIOC_CLK_ENABLE();
    HAL_GPIO_Init(ADC_SNS1_CS_PORT, &adc_sns1_cs);
    HAL_GPIO_WritePin(ADC_SNS1_CS_PORT, ADC_SNS1_CS_PIN, 1);
}