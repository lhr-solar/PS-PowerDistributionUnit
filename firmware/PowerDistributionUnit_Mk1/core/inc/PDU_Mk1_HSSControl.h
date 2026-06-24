// PDU_Mk1_HSSControl.h
// ----------------------------------------------------------------------------
// Stuff for controlling the high-side switches on BBPDU. 
// WARNING: this is NOT thread-safe. This is not designed to be used in 
// multiple threads.

#pragma once

// INCLUDES -------------------------------------------------------------------

#include "stm32xx_hal.h"

// BBPDU peripherals
#include "PDU_Mk1.h"
#include "PDU_Mk1_Pins.h"
#include "PDU_Mk1_OutputConfig.h"
#include "PDU_Mk1_SPI.h"

// drivers
#include "ShiftRegister_SPI.h"

// DEFINES AND ENUMS ----------------------------------------------------------

typedef enum {
	HSSCONTROL_😢,				// HSS control sad
	HSSCONTROL_🙂,				// HSS control happy
} HSSControl_Status_t;

typedef enum {
	PDU_OUTPUT_0,
	PDU_OUTPUT_1,
	PDU_OUTPUT_2,
	PDU_OUTPUT_3,
	PDU_OUTPUT_4,
	PDU_OUTPUT_5,
	PDU_OUTPUT_6,
	PDU_OUTPUT_7,
	PDU_OUTPUT_8,
	PDU_OUTPUT_9,
	PDU_OUTPUT_10,
	PDU_OUTPUT_11,
	PDU_OUTPUT_12,
	PDU_OUTPUT_13,
	PDU_OUTPUT_14,
	PDU_OUTPUT_15,
	PDU_OUTPUT_INVALID,
} HSSControl_Channel_t;

typedef enum {								// Desired action to output channel EN state...
	HSSCONTROL_EN_NOCHANGE,					// performs no change
	HSSCONTROL_EN_OFF,						// turns off the output channel
	HSSCONTROL_EN_ON,						// turns on the output channel
	HSSCONTROL_EN_TOGGLE,					// toggles the output channel
	HSSCONTROL_EN_INVALID,					// (used for errors when trying to read EN state)
} HSSControl_EnState_t;

typedef enum {								// Desired action to output channel LATCH state...
	HSSCONTROL_LATCH_NOCHANGE,				// performs no change
	HSSCONTROL_UNLATCHFAULT_AUTORETRY,		// enables HSS auto-retry (LATCH = 0)
	HSSCONTROL_LATCHFAULT_STAYOFF,			// disables HSS auto-retry, faults are latched (LATCH = 1)
} HSSControl_LatchState_t;

typedef enum {                              // Describes fault state of each output channel...
	HSSCONTROL_NOFAULT,						// no fault, everything's dandy

	HSSCONTROL_FAULT_HSS_TRIP,				// HSS tripped (detected via ST pin through GPIO expander)
											// TPS27SA08-Q1 current limit is fixed at 20 A

	HSSCONTROL_FAULT_SOFTWARE_TRIP,			// Current sensing detected current over limit
											// Current limits to be set in PDU_Mk1_OutputConfig.h (TODO)

	HSSCONTROL_FAULT_INVALID,				// (used for errors when trying to read fault state)
} HSSControl_FaultState_t;

#define HSSCONTROL_STATE_NUM_BYTES 4

#define HSSCONTROL_MASK_EN 0xAAAAAAAA
#define HSSCONTROL_MASK_LATCH 0x55555555
// control group 3
#define HSSCONTROL_LSHIFT_12EN 7
#define HSSCONTROL_MASK_12EN (1<<HSSCONTROL_LSHIFT_12EN)
#define HSSCONTROL_LSHIFT_12LA 6
#define HSSCONTROL_MASK_12LA (1<<HSSCONTROL_LSHIFT_12LA)
#define HSSCONTROL_LSHIFT_13EN 5
#define HSSCONTROL_MASK_13EN (1<<HSSCONTROL_LSHIFT_13EN)
#define HSSCONTROL_LSHIFT_13LA 4
#define HSSCONTROL_MASK_13LA (1<<HSSCONTROL_LSHIFT_13LA)
#define HSSCONTROL_LSHIFT_14EN 3
#define HSSCONTROL_MASK_14EN (1<<HSSCONTROL_LSHIFT_14EN)
#define HSSCONTROL_LSHIFT_14LA 2
#define HSSCONTROL_MASK_14LA (1<<HSSCONTROL_LSHIFT_14LA)
#define HSSCONTROL_LSHIFT_15EN 1
#define HSSCONTROL_MASK_15EN (1<<HSSCONTROL_LSHIFT_15EN)
#define HSSCONTROL_LSHIFT_15LA 0
#define HSSCONTROL_MASK_15LA (1<<HSSCONTROL_LSHIFT_15LA)
// control group 2
#define HSSCONTROL_LSHIFT_8EN 15
#define HSSCONTROL_MASK_8EN (1<<HSSCONTROL_LSHIFT_8EN)
#define HSSCONTROL_LSHIFT_8LA 14
#define HSSCONTROL_MASK_8LA (1<<HSSCONTROL_LSHIFT_8LA)
#define HSSCONTROL_LSHIFT_9EN 13
#define HSSCONTROL_MASK_9EN (1<<HSSCONTROL_LSHIFT_9EN)
#define HSSCONTROL_LSHIFT_9LA 12
#define HSSCONTROL_MASK_9LA (1<<HSSCONTROL_LSHIFT_9LA)
#define HSSCONTROL_LSHIFT_10EN 11
#define HSSCONTROL_MASK_10EN (1<<HSSCONTROL_LSHIFT_10EN)
#define HSSCONTROL_LSHIFT_10LA 10
#define HSSCONTROL_MASK_10LA (1<<HSSCONTROL_LSHIFT_10LA)
#define HSSCONTROL_LSHIFT_11EN 9
#define HSSCONTROL_MASK_11EN (1<<HSSCONTROL_LSHIFT_11EN)
#define HSSCONTROL_LSHIFT_11LA 8
#define HSSCONTROL_MASK_11LA (1<<HSSCONTROL_LSHIFT_11LA)
// control group 1
#define HSSCONTROL_LSHIFT_4EN 23
#define HSSCONTROL_MASK_4EN (1<<HSSCONTROL_LSHIFT_4EN)
#define HSSCONTROL_LSHIFT_4LA 22
#define HSSCONTROL_MASK_4LA (1<<HSSCONTROL_LSHIFT_4LA)
#define HSSCONTROL_LSHIFT_5EN 21
#define HSSCONTROL_MASK_5EN (1<<HSSCONTROL_LSHIFT_5EN)
#define HSSCONTROL_LSHIFT_5LA 20
#define HSSCONTROL_MASK_5LA (1<<HSSCONTROL_LSHIFT_5LA)
#define HSSCONTROL_LSHIFT_6EN 19
#define HSSCONTROL_MASK_6EN (1<<HSSCONTROL_LSHIFT_6EN)
#define HSSCONTROL_LSHIFT_6LA 18
#define HSSCONTROL_MASK_6LA (1<<HSSCONTROL_LSHIFT_6LA)
#define HSSCONTROL_LSHIFT_7EN 17
#define HSSCONTROL_MASK_7EN (1<<HSSCONTROL_LSHIFT_7EN)
#define HSSCONTROL_LSHIFT_7LA 16
#define HSSCONTROL_MASK_7LA (1<<HSSCONTROL_LSHIFT_7LA)
// control group 0
#define HSSCONTROL_LSHIFT_0EN 31
#define HSSCONTROL_MASK_0EN (1<<HSSCONTROL_LSHIFT_0EN)
#define HSSCONTROL_LSHIFT_0LA 30
#define HSSCONTROL_MASK_0LA (1<<HSSCONTROL_LSHIFT_0LA)
#define HSSCONTROL_LSHIFT_1EN 29
#define HSSCONTROL_MASK_1EN (1<<HSSCONTROL_LSHIFT_1EN)
#define HSSCONTROL_LSHIFT_1LA 28
#define HSSCONTROL_MASK_1LA (1<<HSSCONTROL_LSHIFT_1LA)
#define HSSCONTROL_LSHIFT_2EN 27
#define HSSCONTROL_MASK_2EN (1<<HSSCONTROL_LSHIFT_2EN)
#define HSSCONTROL_LSHIFT_2LA 26
#define HSSCONTROL_MASK_2LA (1<<HSSCONTROL_LSHIFT_2LA)
#define HSSCONTROL_LSHIFT_3EN 25
#define HSSCONTROL_MASK_3EN (1<<HSSCONTROL_LSHIFT_3EN)
#define HSSCONTROL_LSHIFT_3LA 24
#define HSSCONTROL_MASK_3LA (1<<HSSCONTROL_LSHIFT_3LA)

#define HSSCONTROL_CRITICAL_MASK (HSSCONTROL_OUT12_PRIORITY<<HSSCONTROL_LSHIFT_12EN) | \
								(HSSCONTROL_OUT13_PRIORITY<<HSSCONTROL_LSHIFT_13EN) | \
								(HSSCONTROL_OUT14_PRIORITY<<HSSCONTROL_LSHIFT_14EN) | \
								(HSSCONTROL_OUT15_PRIORITY<<HSSCONTROL_LSHIFT_15EN) | \
								(HSSCONTROL_OUT8_PRIORITY<<HSSCONTROL_LSHIFT_8EN) | \
								(HSSCONTROL_OUT9_PRIORITY<<HSSCONTROL_LSHIFT_9EN) | \
								(HSSCONTROL_OUT10_PRIORITY<<HSSCONTROL_LSHIFT_10EN) | \
								(HSSCONTROL_OUT11_PRIORITY<<HSSCONTROL_LSHIFT_11EN) | \
								(HSSCONTROL_OUT4_PRIORITY<<HSSCONTROL_LSHIFT_4EN) | \
								(HSSCONTROL_OUT5_PRIORITY<<HSSCONTROL_LSHIFT_5EN) | \
								(HSSCONTROL_OUT6_PRIORITY<<HSSCONTROL_LSHIFT_6EN) | \
								(HSSCONTROL_OUT7_PRIORITY<<HSSCONTROL_LSHIFT_7EN) | \
								(HSSCONTROL_OUT0_PRIORITY<<HSSCONTROL_LSHIFT_0EN) | \
								(HSSCONTROL_OUT1_PRIORITY<<HSSCONTROL_LSHIFT_1EN) | \
								(HSSCONTROL_OUT2_PRIORITY<<HSSCONTROL_LSHIFT_2EN) | \
								(HSSCONTROL_OUT3_PRIORITY<<HSSCONTROL_LSHIFT_3EN)

extern const uint32_t HSSCONTROL_EN_MASKS[PDU_MK1_NUM_CHANNELS];
extern const uint32_t HSSCONTROL_LATCH_MASKS[PDU_MK1_NUM_CHANNELS];

// VARIABLE DECLARATIONS ------------------------------------------------------

extern uint32_t HSS_state;
extern HSSControl_FaultState_t HSS_fault_state[PDU_MK1_NUM_CHANNELS];

extern SPI_HandleTypeDef hspi2;
extern SemaphoreHandle_t spi2_mutex;
extern SemaphoreHandle_t spi2_done_sem;

// GENERAL FUNCTIONS ----------------------------------------------------------

/**
 * @brief 	Initializes HSS control (including initializing ShiftRegister_SPI 
 * 			driver).
 * @param 	None
 * @returns HSSControl Status (HSSCONTROL_🙂 if successful)
 */
HSSControl_Status_t PDU_Mk1_HSSControl_Init();

/**
 * @brief 	Shifts the HSS_state into the hardware shift registers. 
 * @param 	None
 * @returns HSSControl Status (HSSCONTROL_🙂 if successful)
 */
HSSControl_Status_t PDU_Mk1_HSSControl_UpdateHSSShiftRegs();

// EN FUNCTIONS ---------------------------------------------------------------

/**
 * @brief 	Updates HSS_states EN state for a particular channel in software 
 * 			without updating hardware shift register.
 * @param 	ch (HSSControl_Channel_t) PDU output channel
 * @param 	action (HSSControl_EnState_t) action to take, including 
 * 				HSSCONTROL_EN_NOCHANGE (no change is taken), 
 * 				HSSCONTROL_EN_OFF (turns off output), 
 * 				HSSCONTROL_EN_ON (turns on output), and 
 * 				HSSCONTROL_EN_TOGGLE (toggles output)
 * @retval 	HSSControl Status (HSSCONTROL_🙂 if successful)
 */
HSSControl_Status_t PDU_Mk1_HSSControl_WriteHSSEnField(HSSControl_Channel_t ch, HSSControl_EnState_t action);

/**
 * @brief 	Turns on/off or toggles a particular output channel
 * @param 	ch (HSSControl_Channel_t) PDU output channel
 * @param 	action (HSSControl_EnState_t) action to take, including 
 * 				HSSCONTROL_EN_NOCHANGE (no change is taken), 
 * 				HSSCONTROL_EN_OFF (turns off output), 
 * 				HSSCONTROL_EN_ON (turns on output), and 
 * 				HSSCONTROL_EN_TOGGLE (toggles output)
 * @retval 	HSSControl Status (HSSCONTROL_🙂 if successful)
 */
HSSControl_Status_t PDU_Mk1_HSSControl_WriteOutputEN_Ch(HSSControl_Channel_t ch, HSSControl_EnState_t action);

/**
 * @brief	Performs specified turn-on/-off/ toggle actions for each output channel.
 * @param 	actions (HSSControl_EnState_t) array of actions to take, starting 
 * 			with output 0 and incrementing. Possibible values include 
 * 				HSSCONTROL_EN_NOCHANGE (no change is taken), 
 * 				HSSCONTROL_EN_OFF (turns off output), 
 * 				HSSCONTROL_EN_ON (turns on output), and 
 *				HSSCONTROL_EN_TOGGLE (toggles output)
 * @retval 	HSSControl Status (HSSCONTROL_🙂 if successful)
 */
HSSControl_Status_t PDU_Mk1_HSSControl_WriteOutputEN_All(HSSControl_EnState_t actions[]);

/**
 * @brief 	Turns on all outputs. 
 * @param 	None
 * @retval 	HSSControl Status (HSSCONTROL_🙂 if successful)
 */
HSSControl_Status_t PDU_Mk1_HSSControl_AllOn();

/**
 * @brief 	Turns off all outputs. 
 * @param 	None
 * @retval 	HSSControl Status (HSSCONTROL_🙂 if successful)
 */
HSSControl_Status_t PDU_Mk1_HSSControl_AllOff();

/**
 * @brief 	Turns on outputs marked as critical in PDU_Mk1_OutputConfigs.h, 
 * 			and turns off all other outputs.
 * @param 	None
 * @retval 	HSSControl Status (HSSCONTROL_🙂 if successful)
 */
HSSControl_Status_t PDU_Mk1_HSSControl_CritOnly();

/**
 * @brief 	Gets EN state of an output channel.
 * @param 	ch (HSSControl_Channel_t) PDU output channel
 * @retval 	Output channel EN state
 * 			(HSSControl_EnState_t, either HSSCONTROL_EN_OFF or HSSCONTROL_EN_ON -
 * 			or HSSCONTROL_EN_INVALID if invalid channel)
 */
HSSControl_EnState_t PDU_Mk1_HSSControl_GetENState_Ch(HSSControl_Channel_t ch);

// LATCH FUNCTIONS ------------------------------------------------------------

/**
 * @brief 	Updates HSS_states LATCH state for a particular channel in software 
 * 			without updating hardware shift register.
 * @param 	ch (HSSControl_Channel_t) PDU output channel
 * @param 	latch (HSSControl_LatchState_t) latch state to set, including 
 * 				HSSCONTROL_LATCH_NOCHANGE (no change is taken), 
 * 				HSSCONTROL_UNLATCHFAULT_AUTORETRY (sets LATCH=0 for auto-retry), 
 * 				HSSCONTROL_LATCHFAULT_STAYOFF (sets LATCH=1 to disable 
 * 				auto-retry and latch fault - stays off)
 * @retval 	HSSControl Status (HSSCONTROL_🙂 if successful)
 */
HSSControl_Status_t PDU_Mk1_HSSControl_WriteHSSLatchField(HSSControl_Channel_t ch, HSSControl_LatchState_t latch);

/**
 * @brief 	Sets latch state for a particular output channel
 * @param 	ch (HSSControl_Channel_t) PDU output channel
 * @param 	action (HSSControl_EnState_t) latch state to set, including 
 * 				HSSCONTROL_LATCH_NOCHANGE (no change is taken), 
 * 				HSSCONTROL_UNLATCHFAULT_AUTORETRY (sets LATCH=0 for auto-retry), 
 * 				HSSCONTROL_LATCHFAULT_STAYOFF (sets LATCH=1 to disable 
 * 				auto-retry and latch fault - stays off)
 * @retval 	HSSControl Status (HSSCONTROL_🙂 if successful)
 */
HSSControl_Status_t PDU_Mk1_HSSControl_WriteLatch_Ch(HSSControl_Channel_t ch, HSSControl_LatchState_t latch);

/**
 * @brief	Sets specified latch state for each output channel.
 * @param 	actions (HSSControl_EnState_t) array of latch states to set, 
 * 			starting with output 0 and incrementing. Possible values include
 *				HSSCONTROL_LATCH_NOCHANGE (no change is taken), 
 * 				HSSCONTROL_UNLATCHFAULT_AUTORETRY (sets LATCH=0 for auto-retry), 
 * 				HSSCONTROL_LATCHFAULT_STAYOFF (sets LATCH=1 to disable 
 * 				auto-retry and latch fault - stays off)
 * @retval 	HSSControl Status (HSSCONTROL_🙂 if successful)
 */
HSSControl_Status_t PDU_Mk1_HSSControl_WriteLatch_All(HSSControl_LatchState_t states[]);

/**
 * @brief 	Unlatches (sets latch bit to 0) a particular channel to enable 
 * 			auto-retry on that high side switch.
 * @param 	ch (HSSControl_Channel_t) channel to retry
 * @retval 	HSSControl Status (HSSCONTROL_🙂 if successful)
 */
HSSControl_Status_t PDU_Mk1_HSSControl_OutputFaultRetry_Ch(HSSControl_Channel_t ch);

/**
 * @brief 	Unlatches (sets latch bit to 0) all faulted channels to enable 
 * 			auto-retry on those high side switches.
 * @param 	None
 * @retval 	HSSControl Status (HSSCONTROL_🙂 if successful)
 */
HSSControl_Status_t PDU_Mk1_HSSControl_OutputFaultRetry_AllFaulted();

/**
 * @brief 	Relatches (sets latch bit to 1) a particular channel to disable 
 * 			auto-retry on that high side switch. 
 * @param 	ch (HSSControl_Channel_t) channel to retry
 * @retval 	HSSControl Status (HSSCONTROL_🙂 if successful)
 */
HSSControl_Status_t PDU_Mk1_HSSControl_OutputFaultRelatch_Ch(HSSControl_Channel_t ch);

/**
 * @brief 	Relatches (sets latch bit to 1) all faulted channels to disable 
			auto-retry on those high side switches. 
 * @param 	None
 * @retval 	HSSControl Status (HSSCONTROL_🙂 if successful)
 */
HSSControl_Status_t PDU_Mk1_HSSControl_OutputFaultRelatch_AllFaulted();

// FAULT FUNCTIONS ------------------------------------------------------------

/**
 * @brief 	Gets fault state of an output channel.
 * @param 	ch (HSSControl_Channel_t) PDU output channel
 * @retval 	Output channel fault state
 * 			(HSSControl_FaultState_t, incl. HSSCONTROL_EN_NOCHANGE if invalid
 * 			channel)
 */
HSSControl_FaultState_t PDU_Mk1_HSSControl_GetFaultState_Ch(HSSControl_Channel_t ch);
