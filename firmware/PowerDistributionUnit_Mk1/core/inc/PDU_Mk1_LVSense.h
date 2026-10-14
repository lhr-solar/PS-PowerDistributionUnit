// PDU_Mk1_LVSense.h
// ----------------------------------------------------------------------------
// BBPDU Mk1 ADC configuration (LV Voltage Sensing)

#pragma once

#include "stm32xx_hal.h"
#include "ADC.h"

#include "PDU_Mk1_Pins.h"

#define QUEUE_LENGTH 1
#define ITEM_SIZE sizeof(uint32_t)

#define ADC_INT_PRI_LVSENSE 5

#define LVSENSE_ADC_CONVERSION_FACTOR 0.0008058608      // 3.3/4095
#define LVSENSE_RDIV_CONVERSION_FACTOR 11               // (1k+10k)/1k

bool PDU_Mk1_ADC_LVSense_Init(void);

bool PDU_Mk1_LVSense_CollectMeasurement();

float PDU_Mk1_LVSense_ConvertReading(uint32_t adc_reading);
