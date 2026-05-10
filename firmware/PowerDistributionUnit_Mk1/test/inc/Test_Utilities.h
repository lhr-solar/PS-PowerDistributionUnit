#include "stm32xx_hal.h"

/**
 * @brief	Converts uint8_t bits into ASCII characters for printouts.
 * @param	val Value for bits to be converted to ASCII.
 * @param	str Pointer to starting location for ASCII bits (can be in the middle of a char array) with MSB first. 
 *  Warning: ensure this does not overflow the array length.
 */
static void uint8_to_binary_str(uint8_t val, char* str)
{
	str += 7;

	for(uint8_t i = 0; i < 8; i++)
	{
		*str = '0' + (val & 0x01);
		val = val >> 1;
		str--;
	}
}
