// PDU_Mk1_CAN.h
// ----------------------------------------------------------------------------
// BBPDU Mk1 CAN stuff

#pragma once

#include "stm32xx_hal.h"

#include "PDU_Mk1.h"

// drivers
#include "CAN_FD.h"

// DEFINES --------------------------------------------------------------------

#define PDU_MK1_CAN_SEND_TIMEOUT_MS 100						// ticks

#define PDU_MK1_CAN_ID_PDUSTATUS 0x350
#define PDU_MK1_CAN_MSGLEN_PDUSTATUS 5
#define PDU_MK1_CAN_PDUSTATUS_FAULT_LSHIFT 4
#define PDU_MK1_CAN_PDUSTATUS_ENABLED_LSHIFT 5
#define PDU_MK1_CAN_PDUSTATUS_VOLTAGE_INDEX 1
#define PDU_MK1_CAN_PDUSTATUS_VOLTAGE_INVFACTOR 1000		// 1/0.001 = 1000
#define PDU_MK1_CAN_PDUSTATUS_CURRENT_INDEX 3
#define PDU_MK1_CAN_PDUSTATUS_CURRENT_INVFACTOR 1000		// 1/0.001 = 1000

// VARIABLE DECLARATIONS ------------------------------------------------------

// FUNCTIONS ------------------------------------------------------------------

/**
	* @brief Initializes CAN stuff.
	* @param None
	* @retval bool: true if successful, false otherwise
	*/
can_status_t PDU_Mk1_CAN_Init();
