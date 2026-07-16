// PDU_Mk1_SDCard.c

#include "PDU_Mk1_SDCard.h"

sd_handle_t sd;

uint16_t sdcard_write_failures = 0;

bool PDU_Mk1_SDCard_Init(void)
{
	sd.hspi = &hspi1;
	sd.cs_port = SDCARD_CS_PORT;
	sd.cs_pin  = SDCARD_CS_PIN;
	
	HAL_GPIO_WritePin(SDCARD_CS_PORT, SDCARD_CS_PIN, 1); 

	if (USER_SD_Card_Init(&sd, tskIDLE_PRIORITY + 3) != SD_OK) {
		return false;
	}

	return true;
}

#ifdef PDU_MK1_SDLOG_ERRORS
bool PDU_Mk1_SDCard_LogError(uint8_t error_id, uint8_t error_count)
{
	char sdlog_str_buffer[PDU_MK1_SDLOG_ERROR_STRBUFFERSIZE];

	snprintf(sdlog_str_buffer, PDU_MK1_SDLOG_ERROR_STRBUFFERSIZE, "F%02d_%05d\n", error_id, error_count);

	if(USER_SD_Card_Write_Async(&sd, PDU_MK1_SDLOG_FILENAME, sdlog_str_buffer, pdMS_TO_TICKS(PDU_MK1_SDLOG_TIMEOUT_MS)) != SD_OK)
	{
		sdcard_write_failures++;
		printf("FAIL:SDCARDWRITE_%d\n", sdcard_write_failures);

		return false;
	}

	return true;
}
#endif
