// Task_SDCard.h
// ----------------------------------------------------------------------------
// Task logs data to SD card.

#pragma once

// INCLUDES -------------------------------------------------------------------

#include<stdio.h>

#include "stm32xx_hal.h"
#include "printf.h"

// BBPDU peripherals
#include "PDU_Mk1.h"
#include "PDU_Mk1_SDCard.h"
#include "PDU_Mk1_CurrentSensing.h"
#include "PDU_Mk1_HSSControl.h"

// drivers
#include "sdcard.h"

// DEFINES --------------------------------------------------------------------

#define TASK_SDCARD_STACK_SIZE configMINIMAL_STACK_SIZE+200
#define TASK_SDCARD_PRIORITY tskIDLE_PRIORITY + 2

// sets how often the SD card is written to
#define TASK_SDCARD_INTERVAL_MS 100

// VARIABLE DECLARATIONS ------------------------------------------------------

extern sd_handle_t sd;

extern uint16_t sdcard_write_failures;

// FUNCTIONS ------------------------------------------------------------------

void Task_SDCard(void *argument);
