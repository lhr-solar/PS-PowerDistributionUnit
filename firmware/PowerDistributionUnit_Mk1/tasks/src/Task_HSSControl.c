// Task_HSSControl.c

#include "Task_HSSControl.h"

static uint16_t hsscontrol_send_failures = 0;

void Task_HSSControl(void *argument)
{
    for(;;)
    {
        if(PDU_Mk1_HSSControl_AllOn() != HSSCONTROL_🙂)
        {
            hsscontrol_send_failures++;
            printf("FAIL:HSSCONTROL_ALLON_%d", hsscontrol_send_failures);
        }

        vTaskDelay(pdMS_TO_TICKS(TASK_HSSCONTROL_INTERVAL_MS));
    }
}
