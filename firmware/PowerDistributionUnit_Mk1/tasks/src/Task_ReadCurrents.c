// Task_ReadCurrents.c

#include "Task_ReadCurrents.h"

void Task_ReadCurrents()
{
    // magic delay required to make things work for some reason
    // TODO: see if still required? add delay to init function instead?
    vTaskDelay(pdMS_TO_TICKS(1));
    
    for(;;)
    {
        if(PDU_Mk1_CurrentSensing_ReadCurrents() != true)
        {
            printf("FAIL:READ_CURRENTS\n");
            Error_Handler();
        }

#if (PDU_MK1_PRINT_STATUS_VIA_UART)
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
#endif
        
        vTaskDelay(pdMS_TO_TICKS(TASK_READCURRENTS_INTERVAL_MS));
    }
}
