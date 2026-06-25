// Task_HSSControl.c

#include "Task_HSSControl.h"

void Task_HSSControl()
{
    for(;;)
    {
        if(PDU_Mk1_HSSControl_AllOn() != HSSCONTROL_🙂)
        {
            printf("FAIL:HSSCONTROL_ALLON");
            Error_Handler();
        }

        vTaskDelay(pdMS_TO_TICKS(TASK_HSSCONTROL_INTERVAL_MS));
    }
}