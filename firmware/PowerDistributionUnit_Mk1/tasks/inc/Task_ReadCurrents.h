// Task_ReadCurrents.h
// ----------------------------------------------------------------------------
// Task reads ADC to get output currents at fixed interval. 

#pragma once

// INCLUDES -------------------------------------------------------------------

#include "stm32xx_hal.h"
#include "printf.h"

// BBPDU peripherals
#include "PDU_Mk1.h"
#include "PDU_Mk1_CurrentSensing.h"

// DEFINES --------------------------------------------------------------------

#define TASK_READCURRENTS_STACK_SIZE configMINIMAL_STACK_SIZE+3000
#define TASK_READCURRENTS_PRIORITY tskIDLE_PRIORITY + 2

// sets how often the current reading ADC is sampled
#define TASK_READCURRENTS_INTERVAL_MS 500

// VARIABLE DECLARATIONS ------------------------------------------------------

// FUNCTIONS ------------------------------------------------------------------

void Task_ReadCurrents();
