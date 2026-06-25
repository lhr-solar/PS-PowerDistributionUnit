// Task_PrintStatus.c

#include "Task_PrintStatus.h"

void Task_PrintStatus()
{
    for(;;)
    {
        float* currents = PDU_Mk1_CurrentSensing_GetCurrentsPtr();

        printf("\nCurrents\n----------\n"
                "CH0: %.4f A\n"
                "CH1: %.4f A\n"
                "CH2: %.4f A\n"
                "CH3: %.4f A\n"
                "CH4: %.4f A\n"
                "CH5: %.4f A\n"
                "CH6: %.4f A\n"
                "CH7: %.4f A\n" 
                "CH8: %.4f A\n"
                "CH9: %.4f A\n"
                "CH10: %.4f A\n"
                "CH11: %.4f A\n"
                "CH12: %.4f A\n"
                "CH13: %.4f A\n"
                "CH14: %.4f A\n"
                "CH15: %.4f A\n", 
                currents[0], 
                currents[1], 
                currents[2], 
                currents[3], 
                currents[4], 
                currents[5], 
                currents[6], 
                currents[7], 
                currents[8], 
                currents[9], 
                currents[10], 
                currents[11], 
                currents[12], 
                currents[13], 
                currents[14],
                currents[15]);

        vTaskDelay(pdMS_TO_TICKS(TASK_PRINTSTATUS_INTERVAL_MS));
    }
}
