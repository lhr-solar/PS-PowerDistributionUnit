// Task_Blink.c

#include "Task_Blink.h"

void Task_Blink(void *argument)
{
    for(;;)
    {
        HAL_GPIO_TogglePin(LED_PORT, LED_PIN);

        vTaskDelay(pdMS_TO_TICKS(TASK_BLINK_INTERVAL_MS));
    }
}


