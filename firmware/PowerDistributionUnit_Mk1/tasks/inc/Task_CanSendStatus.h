// Task_ReadCurrents.h
// ----------------------------------------------------------------------------
// Task sends PDU status on CAN. 

#pragma once

// INCLUDES -------------------------------------------------------------------

#include "stm32xx_hal.h"
#include "printf.h"

#include "PDU_Mk1.h"
#include "PDU_Mk1_CAN.h"
#include "PDU_Mk1_CurrentSensing.h"
#include "PDU_Mk1_HSSControl.h"

// DEFINES --------------------------------------------------------------------

#define TASK_CANSENDSTATUS_STACK_SIZE configMINIMAL_STACK_SIZE+3000
#define TASK_CANSENDSTATUS_PRIORITY tskIDLE_PRIORITY + 2

// sets how often the CAN status is sent
#define TASK_CANSENDSTATUS_CH_INTERVAL_MS 100		// single channel mode
// #define TASK_CANSENDSTATUS_INTERVAL_MS 1000		// spam all channels (doesn't work)

// VARIABLE DECLARATIONS ------------------------------------------------------

// FUNCTIONS ------------------------------------------------------------------

void Task_CanSendStatus();
