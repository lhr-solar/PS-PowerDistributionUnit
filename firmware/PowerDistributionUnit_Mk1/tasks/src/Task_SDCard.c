// Task_SDCard.c

#include "Task_SDCard.h"

void Task_SDCard(void *argument)
{
    char log_buffer[PDU_MK1_SDLOG_NUMLOGITEMS][PDU_MK1_SDLOG_STRBUFFERSIZE] = {0};

    for(;;)
    {
        sprintf(log_buffer[0], "0-8|%2.3f,%2.3f,%2.3f,%2.3f,%2.3f,%2.3f,%2.3f,%2.3f[A]\n",
            PDU_Mk1_CurrentSensing_GetCurrentsPtr()[0],
            PDU_Mk1_CurrentSensing_GetCurrentsPtr()[1],
            PDU_Mk1_CurrentSensing_GetCurrentsPtr()[2],
            PDU_Mk1_CurrentSensing_GetCurrentsPtr()[3],
            PDU_Mk1_CurrentSensing_GetCurrentsPtr()[4],
            PDU_Mk1_CurrentSensing_GetCurrentsPtr()[5],
            PDU_Mk1_CurrentSensing_GetCurrentsPtr()[6],
            PDU_Mk1_CurrentSensing_GetCurrentsPtr()[7]
        );

        sprintf(log_buffer[1], "8-F|%2.3f,%2.3f,%2.3f,%2.3f,%2.3f,%2.3f,%2.3f,%2.3f[A]\n",
            PDU_Mk1_CurrentSensing_GetCurrentsPtr()[8],
            PDU_Mk1_CurrentSensing_GetCurrentsPtr()[9],
            PDU_Mk1_CurrentSensing_GetCurrentsPtr()[10],
            PDU_Mk1_CurrentSensing_GetCurrentsPtr()[11],
            PDU_Mk1_CurrentSensing_GetCurrentsPtr()[12],
            PDU_Mk1_CurrentSensing_GetCurrentsPtr()[13],
            PDU_Mk1_CurrentSensing_GetCurrentsPtr()[14],
            PDU_Mk1_CurrentSensing_GetCurrentsPtr()[15]
        );

        // would be more efficient to have hex instead of bits, but bits easier to code for now
        sprintf(log_buffer[2], "V|%2.3f[V]|EN|%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d|FLT|%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d\n", 
            0.0,
            (PDU_Mk1_HSSControl_GetENState_Ch(0) == HSSCONTROL_EN_ON),
            (PDU_Mk1_HSSControl_GetENState_Ch(1) == HSSCONTROL_EN_ON),
            (PDU_Mk1_HSSControl_GetENState_Ch(2) == HSSCONTROL_EN_ON),
            (PDU_Mk1_HSSControl_GetENState_Ch(3) == HSSCONTROL_EN_ON),
            (PDU_Mk1_HSSControl_GetENState_Ch(4) == HSSCONTROL_EN_ON),
            (PDU_Mk1_HSSControl_GetENState_Ch(5) == HSSCONTROL_EN_ON),
            (PDU_Mk1_HSSControl_GetENState_Ch(6) == HSSCONTROL_EN_ON),
            (PDU_Mk1_HSSControl_GetENState_Ch(7) == HSSCONTROL_EN_ON),
            (PDU_Mk1_HSSControl_GetENState_Ch(8) == HSSCONTROL_EN_ON),
            (PDU_Mk1_HSSControl_GetENState_Ch(9) == HSSCONTROL_EN_ON),
            (PDU_Mk1_HSSControl_GetENState_Ch(10) == HSSCONTROL_EN_ON),
            (PDU_Mk1_HSSControl_GetENState_Ch(11) == HSSCONTROL_EN_ON),
            (PDU_Mk1_HSSControl_GetENState_Ch(12) == HSSCONTROL_EN_ON),
            (PDU_Mk1_HSSControl_GetENState_Ch(13) == HSSCONTROL_EN_ON),
            (PDU_Mk1_HSSControl_GetENState_Ch(14) == HSSCONTROL_EN_ON),
            (PDU_Mk1_HSSControl_GetENState_Ch(15) == HSSCONTROL_EN_ON),
            (PDU_Mk1_HSSControl_GetFaultState_Ch(0) != HSSCONTROL_NOFAULT),
            (PDU_Mk1_HSSControl_GetFaultState_Ch(1) != HSSCONTROL_NOFAULT),
            (PDU_Mk1_HSSControl_GetFaultState_Ch(2) != HSSCONTROL_NOFAULT),
            (PDU_Mk1_HSSControl_GetFaultState_Ch(3) != HSSCONTROL_NOFAULT),
            (PDU_Mk1_HSSControl_GetFaultState_Ch(4) != HSSCONTROL_NOFAULT),
            (PDU_Mk1_HSSControl_GetFaultState_Ch(5) != HSSCONTROL_NOFAULT),
            (PDU_Mk1_HSSControl_GetFaultState_Ch(6) != HSSCONTROL_NOFAULT),
            (PDU_Mk1_HSSControl_GetFaultState_Ch(7) != HSSCONTROL_NOFAULT),
            (PDU_Mk1_HSSControl_GetFaultState_Ch(8) != HSSCONTROL_NOFAULT),
            (PDU_Mk1_HSSControl_GetFaultState_Ch(9) != HSSCONTROL_NOFAULT),
            (PDU_Mk1_HSSControl_GetFaultState_Ch(10) != HSSCONTROL_NOFAULT),
            (PDU_Mk1_HSSControl_GetFaultState_Ch(11) != HSSCONTROL_NOFAULT),
            (PDU_Mk1_HSSControl_GetFaultState_Ch(12) != HSSCONTROL_NOFAULT),
            (PDU_Mk1_HSSControl_GetFaultState_Ch(13) != HSSCONTROL_NOFAULT),
            (PDU_Mk1_HSSControl_GetFaultState_Ch(14) != HSSCONTROL_NOFAULT),
            (PDU_Mk1_HSSControl_GetFaultState_Ch(15) != HSSCONTROL_NOFAULT)
        );

        for(uint8_t i=0; i < PDU_MK1_SDLOG_NUMLOGITEMS; i++)
        {
            if(USER_SD_Card_Write_Async(&sd, PDU_MK1_SDLOG_FILENAME, log_buffer[i], pdMS_TO_TICKS(PDU_MK1_SDLOG_TIMEOUT_MS)) != SD_OK)
            {
                sdcard_write_failures++;
                printf("FAIL:SDCARDWRITE_%d\n", sdcard_write_failures);
            }
        }
        
        vTaskDelay(pdMS_TO_TICKS(TASK_SDCARD_INTERVAL_MS));
    }
}
