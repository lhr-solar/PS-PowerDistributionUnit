// PDU_Mk1.h
// ----------------------------------------------------------------------------
// BBPDU Mk1 constants and stuff

#pragma once

// CONSTANTS ------------------------------------------------------------------
#define PDU_MK1_NUM_CHANNELS 16

// OPTIONS --------------------------------------------------------------------

/* PDU REVISION _______________________________________________________ */

// #define PDU_MK1_REV_A
#define PDU_MK1_REV_B

/* LSOM REVISION ______________________________________________________ */

// #define LSOM_S
#define LSOM_U

/* DEBUG ______________________________________________________________ */

// prints channel currents
#define PDU_MK1_PRINT_STATUS_VIA_UART true

// log failures to SD card
// if enabled for a test, make sure it actually initializes SD card stuff
#define PDU_MK1_SDLOG_ERRORS

// enables external loopback mode for solo CAN development (bypass ACK requirement)
// should not be defined for final in-car use
// #define PDU_MK1_CAN_DEV_EXTLOOPBACK
