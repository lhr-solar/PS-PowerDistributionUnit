// Task_LVSense.h
// ----------------------------------------------------------------------------
// Senses LV input voltage.

#pragma once

// INCLUDES -------------------------------------------------------------------

#include "stm32xx_hal.h"
#include "printf.h"

#include "PDU_Mk1_LVSense.h"

// DEFINES --------------------------------------------------------------------

#define TASK_LVSENSE_STACK_SIZE configMINIMAL_STACK_SIZE
#define TASK_LVSENSE_PRIORITY tskIDLE_PRIORITY + 3

#define TASK_LVSENSE_INTERVAL_MS 500

// VARIABLE DECLARATIONS ------------------------------------------------------

extern QueueHandle_t readings_queue;

extern uint32_t lvsense_reading;
extern float lv_voltage;

// FUNCTIONS ------------------------------------------------------------------

void Task_LVSense(void *argument);
