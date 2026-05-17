// ACS3704x-010B3.h
// ----------------------------------------------------------------------------
// Driver for ACS3704x-010B3 current sensor (+/- 10A range, 3.3V supply).
// Datasheet: datasheets/ACS3704x.pdf (from BBPDU repo root)

#pragma once

// DEVICE PARAMETERS ----------------------------------------------------------

#define ACS3704x_010B3_V_QVO 1.65f				// V, quiescent output voltage (no current)
#define ACS3704x_010B3_SENSITIVITY 0.132f		// V/A, output voltage increase per amp

// USER FUNCTIONS -----------------------------------------------------------

/**
 * @brief	Converts current sensor output voltage to current passing through the device.
 * @param	v float: current sensor output voltage (relative to GND)
 * @returns float: current conversion
 */
float ACS3704x_010B3_Current_Conversion(float v);

/**
 * @brief	Converts current sensor output voltage (already centered on V_QVO = 1.65 V) 
 * to current passing through the device.
 * @param	v float: current sensor output voltage (relative to V_QVO = 1.65 V)
 * @returns float: current conversion
 */
float ACS3704x_010B3_Current_Conversion_QVOCentered(float v);
