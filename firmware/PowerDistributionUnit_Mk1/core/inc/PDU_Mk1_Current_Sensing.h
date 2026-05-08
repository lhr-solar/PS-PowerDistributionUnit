// Current_Sensing.h
// ----------------------------------------------------------------------------
// Stuff for output current sensing on BBPDU. 

#pragma once

#include "stm32xx_hal.h"

#include "ADS131M08-Q1.h"

#include "PDU_Mk1.h"
#include "PDU_Mk1_Pins.h"

extern SPI_HandleTypeDef hspi2;
extern SemaphoreHandle_t spi2_mutex;
extern StaticSemaphore_t spi2_mutex_buffer;
extern SemaphoreHandle_t spi2_done_sem;

extern float currents[PDU_MK1_NUM_CHANNELS];
extern float current_offset[PDU_MK1_NUM_CHANNELS];

bool PDU_Mk1_Current_Sensing_Init();

ADS131M08Q1_Status_t PDU_Mk1_Init_ADC_SNS1();

// TODO
// ADS131M08Q1_Status_t PDU_Mk1_Init_ADC_SNS0();
// bool PDU_Mk1_Current_Sensing_SetOffsets();
// bool PDU_Mk1_Current_Sensing_ReadCurrents();
