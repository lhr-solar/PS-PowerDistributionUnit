// Task_HSSControl.h
// ----------------------------------------------------------------------------
// Task controls PDU output channels. 

#pragma once

// INCLUDES -------------------------------------------------------------------

#include "stm32xx_hal.h"
#include "printf.h"

// BBPDU peripherals
#include "PDU_Mk1_HSSControl.h"
#ifdef PDU_MK1_SDLOG_ERRORS
 #include "PDU_Mk1_SDCard.h"
#endif

// DEFINES --------------------------------------------------------------------

#define TASK_HSSCONTROL_STACK_SIZE configMINIMAL_STACK_SIZE+200
#define TASK_HSSCONTROL_PRIORITY tskIDLE_PRIORITY + 2

#define TASK_HSSCONTROL_INTERVAL_MS 100

// VARIABLE DECLARATIONS ------------------------------------------------------

// FUNCTIONS ------------------------------------------------------------------

void Task_HSSControl(void *argument);
