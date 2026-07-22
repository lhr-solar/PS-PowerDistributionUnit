// Task_SDCard.c

#include "Task_SDCard.h"

void Task_SDCard(void *argument)
{
    char log_buffer[PDU_MK1_SDLOG_NUMLOGITEMS][PDU_MK1_SDLOG_STRBUFFERSIZE] = {0};

    float* currents = PDU_Mk1_CurrentSensing_GetCurrentsPtr();

    if(USER_SD_Card_Write_Async(&sd, PDU_MK1_SDLOG_FILENAME, "START\n", pdMS_TO_TICKS(PDU_MK1_SDLOG_TIMEOUT_MS)) != SD_OK)
    {
        sdcard_write_failures++;
        printf("FAIL:SDCARDWRITE_%d\n", sdcard_write_failures);
    }

    for(;;)
    {
        snprintf(log_buffer[PDU_MK1_SDLOG_INDEX_I_CH0_7], PDU_MK1_SDLOG_STRBUFFERSIZE,
            "0-8|%2.3f,%2.3f,%2.3f,%2.3f,%2.3f,%2.3f,%2.3f,%2.3f[A]\n",
            (currents[0] <= PDU_MK1_SDLOG_MIN_WRITABLE_CURRENT ? PDU_MK1_SDLOG_MIN_WRITABLE_CURRENT : currents[0]),
            (currents[1] <= PDU_MK1_SDLOG_MIN_WRITABLE_CURRENT ? PDU_MK1_SDLOG_MIN_WRITABLE_CURRENT : currents[1]),
            (currents[2] <= PDU_MK1_SDLOG_MIN_WRITABLE_CURRENT ? PDU_MK1_SDLOG_MIN_WRITABLE_CURRENT : currents[2]),
            (currents[3] <= PDU_MK1_SDLOG_MIN_WRITABLE_CURRENT ? PDU_MK1_SDLOG_MIN_WRITABLE_CURRENT : currents[3]),
            (currents[4] <= PDU_MK1_SDLOG_MIN_WRITABLE_CURRENT ? PDU_MK1_SDLOG_MIN_WRITABLE_CURRENT : currents[4]),
            (currents[5] <= PDU_MK1_SDLOG_MIN_WRITABLE_CURRENT ? PDU_MK1_SDLOG_MIN_WRITABLE_CURRENT : currents[5]),
            (currents[6] <= PDU_MK1_SDLOG_MIN_WRITABLE_CURRENT ? PDU_MK1_SDLOG_MIN_WRITABLE_CURRENT : currents[6]),
            (currents[7] <= PDU_MK1_SDLOG_MIN_WRITABLE_CURRENT ? PDU_MK1_SDLOG_MIN_WRITABLE_CURRENT : currents[7])
        );

        snprintf(log_buffer[PDU_MK1_SDLOG_INDEX_I_CH8_15], PDU_MK1_SDLOG_STRBUFFERSIZE,
            "8-F|%2.3f,%2.3f,%2.3f,%2.3f,%2.3f,%2.3f,%2.3f,%2.3f[A]\n",
            (currents[8] <= PDU_MK1_SDLOG_MIN_WRITABLE_CURRENT ? PDU_MK1_SDLOG_MIN_WRITABLE_CURRENT: currents[8]),
            (currents[9] <= PDU_MK1_SDLOG_MIN_WRITABLE_CURRENT ? PDU_MK1_SDLOG_MIN_WRITABLE_CURRENT : currents[9]),
            (currents[10] <= PDU_MK1_SDLOG_MIN_WRITABLE_CURRENT ? PDU_MK1_SDLOG_MIN_WRITABLE_CURRENT : currents[10]),
            (currents[11] <= PDU_MK1_SDLOG_MIN_WRITABLE_CURRENT ? PDU_MK1_SDLOG_MIN_WRITABLE_CURRENT : currents[11]),
            (currents[12] <= PDU_MK1_SDLOG_MIN_WRITABLE_CURRENT ? PDU_MK1_SDLOG_MIN_WRITABLE_CURRENT : currents[12]),
            (currents[13] <= PDU_MK1_SDLOG_MIN_WRITABLE_CURRENT ? PDU_MK1_SDLOG_MIN_WRITABLE_CURRENT : currents[13]),
            (currents[14] <= PDU_MK1_SDLOG_MIN_WRITABLE_CURRENT ? PDU_MK1_SDLOG_MIN_WRITABLE_CURRENT : currents[14]),
            (currents[15] <= PDU_MK1_SDLOG_MIN_WRITABLE_CURRENT ? PDU_MK1_SDLOG_MIN_WRITABLE_CURRENT : currents[15])
        );

        // would be more efficient to have hex instead of bits, but bits easier to code for now
        snprintf(log_buffer[PDU_MK1_SDLOG_INDEX_V_HSSCONTROL_STATE], PDU_MK1_SDLOG_STRBUFFERSIZE,
            "V|%2.3f[V]|EN|%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d|FLT|%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d\n",
            0.0f,
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

        for(uint8_t i = 0; i < PDU_MK1_SDLOG_NUMLOGITEMS; i++)
        {
            if(uxQueueMessagesWaiting(sd.job_queue) < SD_QUEUE_LENGTH - SD_QUEUE_RESERVE_FAULT_LOG_RESERVE_SIZE)
            {
                if(USER_SD_Card_Write_Async(&sd, PDU_MK1_SDLOG_FILENAME, log_buffer[i], pdMS_TO_TICKS(PDU_MK1_SDLOG_TIMEOUT_MS)) != SD_OK)
                {
                    sdcard_write_failures++;
                    printf("FAIL:SDCARDWRITE_%d\n", sdcard_write_failures);
                }
            }
        }
        
        vTaskDelay(pdMS_TO_TICKS(TASK_SDCARD_INTERVAL_MS));
    }
}
