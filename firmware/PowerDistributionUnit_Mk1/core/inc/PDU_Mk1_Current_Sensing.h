// PDU_Mk1_Current_Sensing.h
// ----------------------------------------------------------------------------
// Stuff for output current sensing on BBPDU. 

#pragma once

// INCLUDES -------------------------------------------------------------------

#include "stm32xx_hal.h"

// BBPDU peripherals
#include "PDU_Mk1.h"
#include "PDU_Mk1_Pins.h"

// drivers
#include "ADS131M08-Q1.h"
#include "ACS3704x-010B3.h"

// VARIABLE DECLARATIONS ------------------------------------------------------

extern SPI_HandleTypeDef hspi2;
extern SemaphoreHandle_t spi2_mutex;
extern StaticSemaphore_t spi2_mutex_buffer;
extern SemaphoreHandle_t spi2_done_sem;

extern SPI_HandleTypeDef hspi3;
extern SemaphoreHandle_t spi3_mutex;
extern StaticSemaphore_t spi3_mutex_buffer;
extern SemaphoreHandle_t spi3_done_sem;

extern float currents[PDU_MK1_NUM_CHANNELS];
extern float current_offset[PDU_MK1_NUM_CHANNELS];

// FUNCTIONS ------------------------------------------------------------------

/**
	* @brief Initializes current sensing functions (ADCs, initial calibration-TODO). 
	* Must be called while all outputs are disabled. 
	* @param None
	* @retval bool: true if successful, false otherwise
	*/
bool PDU_Mk1_CurrentSensing_Init();

/**
	* @brief Initializes ADC_SNS1 for current sensing use. 
	* @param None
	* @retval ADS131M08Q1 Status (ADS131M08Q1_🙂 if successful)
	*/
ADS131M08Q1_Status_t PDU_Mk1_Init_ADC_SNS0();

/**
	* @brief Initializes ADC_SNS1 for current sensing use. 
	* @param None
	* @retval ADS131M08Q1 Status (ADS131M08Q1_🙂 if successful)
	*/
ADS131M08Q1_Status_t PDU_Mk1_Init_ADC_SNS1();

/**
	* @brief Initializes ADC_SNS1 for current sensing use. 
	* @param None
	* @retval ADS131M08Q1 Status (ADS131M08Q1_🙂 if successful)
	*/
bool PDU_Mk1_CurrentSensing_CollectOffsets();

/**
	* @brief Reads sample from current sensing ADCs and calculates current conversion.
	* @param None
	* @retval bool: true if successful, false otherwise
	*/
bool PDU_Mk1_CurrentSensing_ReadCurrents();

/**
	* @brief Returns pointer to array where last currents sample is stored. 
	* @param None
	* @retval float pointer to currents array
	*/
float* PDU_Mk1_CurrentSensing_GetCurrentsPtr();
