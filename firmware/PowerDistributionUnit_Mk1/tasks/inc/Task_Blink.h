// Task_Blink.h
// ----------------------------------------------------------------------------
// Task blinks LED at fixed interval.

#pragma once

// INCLUDES -------------------------------------------------------------------

#include "stm32xx_hal.h"

#include "PDU_Mk1_Pins.h"

// DEFINES --------------------------------------------------------------------

#define TASK_BLINK_STACK_SIZE configMINIMAL_STACK_SIZE
#define TASK_BLINK_PRIORITY tskIDLE_PRIORITY + 2

#define TASK_BLINK_INTERVAL_MS 500

// VARIABLE DECLARATIONS ------------------------------------------------------

// FUNCTIONS ------------------------------------------------------------------

void Task_Blink(void *argument);
