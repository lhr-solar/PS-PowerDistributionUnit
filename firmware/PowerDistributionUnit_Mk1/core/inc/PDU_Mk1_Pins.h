// PDU_Mk1_Pins.h
// ----------------------------------------------------------------------------
// BBPDU pin assignments

#pragma once

#include "PDU_Mk1.h"

#if defined(LSOM_U)
 #include "LSOM_U_Pins.h"
#elif defined(LSOM_S)
 #include "LSOM_S_Pins.h"
#else
 #error "LSOM type not defined."
#endif

#define LED_PIN GPIO_PIN_3
#define LED_PORT GPIOC
#define LED_CLOCK_INIT() __HAL_RCC_GPIOC_CLK_ENABLE();

#define ADC_SNS1_CS_PIN GPIO_PIN_4
#define ADC_SNS1_CS_PORT GPIOC

#define ADC_SNS1_DRDY_PIN GPIO_PIN_5
#define ADC_SNS1_DRDY_PORT GPIOC

#define ADC_SNS1_SYNCRESET_PIN GPIO_PIN_12
#define ADC_SNS1_SYNCRESET_PORT GPIOC

#define HSS_SR_EN_PIN GPIO_PIN_2
#define HSS_SR_EN_PORT GPIOB

#define HSS_SR_OUT_CLK_PIN GPIO_PIN_8
#define HSS_SR_OUT_CLK_PORT GPIOC

#define HSS_SR_ALL_OFF_PORT LSOM_13_PORT
#define HSS_SR_ALL_OFF_PIN LSOM_13_PIN

#define ADC_SNS0_CS_PORT LSOM_44_PORT
#define ADC_SNS0_CS_PIN LSOM_44_PIN

#define ADC_SNS0_DRDY_PORT LSOM_36_PORT
#define ADC_SNS0_DRDY_PIN LSOM_36_PIN

#define ADC_SNS0_SYNCRESET_PORT LSOM_40_PORT
#define ADC_SNS0_SYNCRESET_PIN LSOM_40_PIN

#define SDCARD_CS_PORT       GPIOA
#define SDCARD_CS_PIN        GPIO_PIN_4

#define SDCARD_SCK_PORT      GPIOA
#define SDCARD_SCK_PIN       GPIO_PIN_5

#define SDCARD_MISO_PORT     GPIOA
#define SDCARD_MISO_PIN      GPIO_PIN_6

#define SDCARD_MOSI_PORT     GPIOA
#define SDCARD_MOSI_PIN      GPIO_PIN_7
