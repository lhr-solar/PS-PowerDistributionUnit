// PDU_Mk1_GPIO.c

#include "PDU_Mk1_GPIO.h"

void PDU_Mk1_GPIO_Init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

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
    HAL_GPIO_Init(ADC_SNS1_CS_PORT, &adc_sns1_cs);
    HAL_GPIO_WritePin(ADC_SNS1_CS_PORT, ADC_SNS1_CS_PIN, 1);

    GPIO_InitTypeDef hss_sr_en = {
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pull = GPIO_NOPULL,
        .Pin = HSS_SR_EN_PIN
    };
    HAL_GPIO_Init(HSS_SR_EN_PORT, &hss_sr_en);
    HAL_GPIO_WritePin(HSS_SR_EN_PORT, HSS_SR_EN_PIN, 0);

    GPIO_InitTypeDef hss_sr_out_clk = {
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pull = GPIO_NOPULL,
        .Pin = HSS_SR_OUT_CLK_PIN
    };
    HAL_GPIO_Init(HSS_SR_OUT_CLK_PORT, &hss_sr_out_clk);
    HAL_GPIO_WritePin(HSS_SR_OUT_CLK_PORT, HSS_SR_OUT_CLK_PIN, 0);

    GPIO_InitTypeDef hss_sr_all_off = {
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pull = GPIO_NOPULL,
        .Pin = HSS_SR_ALL_OFF_PIN
    };
    HAL_GPIO_Init(HSS_SR_ALL_OFF_PORT, &hss_sr_all_off);
    HAL_GPIO_WritePin(HSS_SR_ALL_OFF_PORT, HSS_SR_ALL_OFF_PIN, 0);

    GPIO_InitTypeDef adc_sns0_cs = {
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pull = GPIO_NOPULL,
        .Pin = ADC_SNS0_CS_PIN
    };
    HAL_GPIO_Init(ADC_SNS0_CS_PORT, &adc_sns0_cs);
    HAL_GPIO_WritePin(ADC_SNS0_CS_PORT, ADC_SNS0_CS_PIN, 1);
}