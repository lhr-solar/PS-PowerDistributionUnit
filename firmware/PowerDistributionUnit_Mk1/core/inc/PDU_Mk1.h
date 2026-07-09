// PDU_Mk1.h
// ----------------------------------------------------------------------------
// BBPDU Mk1 constants and stuff

#pragma once

#define PDU_MK1_NUM_CHANNELS 16

#define PDU_MK1_REV_A

#define PDU_MK1_PRINT_STATUS_VIA_UART true

// enables external loopback mode for solo CAN development (bypass ACK requirement)
// should not be defined for final in-car use
#define PDU_MK1_CAN_DEV_EXTLOOPBACK
