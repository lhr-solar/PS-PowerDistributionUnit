// Task_LVSense.c

#include "Task_LVSense.h"

void Task_LVSense(void *argument)
{   
    for(;;)
    {
        if(PDU_Mk1_LVSense_CollectMeasurement() != true)
        {
            printf("FAIL:LVSENSE_READ");
        }

        lv_voltage = PDU_Mk1_LVSense_ConvertReading(lvsense_reading);

#if (PDU_MK1_PRINT_STATUS_VIA_UART)
        printf("LV Voltage Reading (V): %.3f V\n", lv_voltage);
#endif

        vTaskDelay(pdMS_TO_TICKS(TASK_LVSENSE_INTERVAL_MS));
    }
}
