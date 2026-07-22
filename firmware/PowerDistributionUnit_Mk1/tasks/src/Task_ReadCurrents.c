// Task_ReadCurrents.c

#include "Task_ReadCurrents.h"

static uint16_t current_read_failures = 0;

void Task_ReadCurrents(void* argument) {
    for (;;)
    {
        if (PDU_Mk1_CurrentSensing_ReadCurrents() != true)
        {
            current_read_failures++;
            printf("FAIL:READ_CURRENTS_%d\n", current_read_failures);
        }

#if (PDU_MK1_PRINT_STATUS_VIA_UART)
        float* currents = PDU_Mk1_CurrentSensing_GetCurrentsPtr();

        printf("\r\n--------- Current Measurements ---------\r\n");
        printf("----------------------------------------\r\n");

        for (uint8_t i = 0; i < 16; i++) {
            printf("CH%u: %.4f A\r\n", i, currents[i]);
        }
#endif

        vTaskDelay(pdMS_TO_TICKS(TASK_READCURRENTS_INTERVAL_MS));
    }
}
