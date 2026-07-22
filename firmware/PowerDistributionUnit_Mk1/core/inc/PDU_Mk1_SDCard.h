// PDU_Mk1_SDCard.h
// ----------------------------------------------------------------------------
// Stuff for SD card logging on BBPDU. 

#pragma once

// INCLUDES -------------------------------------------------------------------

#include <stdio.h>

#include "stm32xx_hal.h"

// BBPDU peripherals
#include "PDU_Mk1.h"
#include "PDU_Mk1_Pins.h"
#include "PDU_Mk1_SPI.h"

// drivers
#include "sdcard.h"

// DEFINES --------------------------------------------------------------------

#define PDU_MK1_SDLOG_FILENAME "PDU_LOG.TXT"

#define PDU_MK1_SDLOG_FAULTLOG_TIMEOUT_MS 1
#define PDU_MK1_SDLOG_TIMEOUT_MS 20

#define PDU_MK1_SDLOG_NUMLOGITEMS 3
#define PDU_MK1_SDLOG_STRBUFFERSIZE 64  // note that the SD card driver
                                        // internally has a 64 limit

#define PDU_MK1_SDLOG_INDEX_I_CH0_7 0
#define PDU_MK1_SDLOG_INDEX_I_CH8_15 1
#define PDU_MK1_SDLOG_INDEX_V_HSSCONTROL_STATE 2

#define PDU_MK1_SDLOG_MIN_WRITABLE_CURRENT 0.0001f	// minimum current limit to
													// "negative zero" that
													// risk str buffer overflow

// Error Logging
#define PDU_MK1_SDLOG_ERROR_STRBUFFERSIZE 16
#define SD_QUEUE_RESERVE_FAULT_LOG_RESERVE_SIZE 1 	// amount of space to leave
													// in queue in case a fault
													// log comes by
													// (data logging is skipped)

// Error Log IDs
#define PDU_MK1_SDLOG_ERRORID_READ_CURRENTS 1
#define PDU_MK1_SDLOG_ERRORID_HSSCONTROL 2
#define PDU_MK1_SDLOG_ERRORID_CAN_SEND_STATUS 3

// VARIABLE DECLARATIONS ------------------------------------------------------

extern SPI_HandleTypeDef hspi1;
extern SemaphoreHandle_t spi1_mutex;
extern StaticSemaphore_t spi1_mutex_buffer;
extern SemaphoreHandle_t spi1_done_sem;

// FUNCTIONS ------------------------------------------------------------------

/**
	* @brief Initializes SD card stuff.
	* @param None
	* @retval bool: true if successful, false otherwise
	*/
bool PDU_Mk1_SDCard_Init(void);

/**
	* @brief Logs lil error text to the SD card.
	* @param error_id Error ID
	* @param error_count Error count
	* @retval bool: true if successful, false otherwise
	*/
#ifdef PDU_MK1_SDLOG_ERRORS
bool PDU_Mk1_SDCard_LogError(uint8_t error_id, uint8_t error_count);
#endif
