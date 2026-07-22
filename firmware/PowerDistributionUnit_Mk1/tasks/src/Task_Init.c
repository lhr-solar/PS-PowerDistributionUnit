// Task_Init.c

#include "Task_Init.h"

// Task: Blink
StaticTask_t task_blink_buffer;
StackType_t task_blink_stack[TASK_BLINK_STACK_SIZE];

// Task: Read Currents
StaticTask_t task_readcurrents_buffer;
StackType_t task_readcurrents_stack[TASK_READCURRENTS_STACK_SIZE];

// Task: HSS Output Control
StaticTask_t task_hsscontrol_buffer;
StackType_t task_hsscontrol_stack[TASK_HSSCONTROL_STACK_SIZE];

// Task: Send CAN Status
StaticTask_t task_cansendstatus_buffer;
StackType_t task_cansendstatus_stack[TASK_CANSENDSTATUS_STACK_SIZE];

// Task: SD Card Logging
StaticTask_t task_sdcard_buffer;
StackType_t task_sdcard_stack[TASK_SDCARD_STACK_SIZE];

void Task_Init(void *argument)
{
    PDU_Mk1_GPIO_Init();

    if(PDU_Mk1_UART_Printf_Init() != true)
    {
        printf("FAIL:UART_PRINTF_INIT\n");
        Error_Handler();
    }

    if(PDU_Mk1_CAN_Init() != CAN_OK)
    {
        printf("FAIL: CAN\n");
        Error_Handler();
    }

    if(PDU_Mk1_SPI1_SDCard_Init() != true)
    {
        printf("FAIL:SPI1_INIT\n");
        Error_Handler();
    }

    if(PDU_Mk1_SPI2_ADC_Init() != true)
    {
        printf("FAIL:SPI2_INIT\n");
        Error_Handler();
    }

#ifndef PDU_MK1_REV_A            // PDU_Mk1_REV_A has SPI pinout issue
    if(PDU_Mk1_SPI3_ADC_Init() != true)
    {
        printf("FAIL:SPI3_INIT\n");
        Error_Handler();
    }
#endif
    
    if(SPI_RTOS_Mutex_Semaphore_Setup(&spi2_mutex, &spi2_mutex_buffer, &spi2_done_sem, &spi2_done_sem_buffer) != true)
    {
        printf("FAIL:MUTEX_SEMAPHORE_INIT\n");
        Error_Handler();
    }

    // SPI dummy send because CLK pin initializes high even when configured low for some reason
    if(SPI_Init_Dummy_Send(&hspi2, spi2_mutex, spi2_done_sem) != true)
    {
        printf("FAIL:SPI2_INIT_DUMMY_SEND\n");
        Error_Handler();
    }

#ifndef PDU_MK1_REV_A            // PDU_Mk1_REV_A has SPI pinout issue
    if(SPI_RTOS_Mutex_Semaphore_Setup(&spi3_mutex, &spi3_mutex_buffer, &spi3_done_sem, &spi3_done_sem_buffer) != true)
    {
        printf("FAIL:MUTEX_SEMAPHORE_INIT\n");
        Error_Handler();
    }

    // SPI dummy send because CLK pin initializes high even when configured low for some reason
    if(SPI_Init_Dummy_Send(&hspi3, spi3_mutex, spi3_done_sem) != true)
    {
        printf("FAIL:SPI3_INIT_DUMMY_SEND\n");
        Error_Handler();
    }
#endif

    if(PDU_Mk1_CurrentSensing_Init() != true)
    {
        printf("FAIL:ISENSE_INIT\n");
        Error_Handler();
    }

    if(PDU_Mk1_SDCard_Init() != true)
    {
        printf("FAIL:SDCARD_INIT\n");
        Error_Handler();
    }

    if(PDU_Mk1_HSSControl_Init() != HSSCONTROL_🙂)
    {
        printf("FAIL:HSSCONTROL_INIT\n");
        Error_Handler();
    }

    printf("Initialization complete.\n");

    // get this party started
    xTaskCreateStatic(Task_Blink,
                    "Blink Task",
                    TASK_BLINK_STACK_SIZE,
                    NULL,
                    TASK_BLINK_PRIORITY,
                    task_blink_stack,
                    &task_blink_buffer
                );

    xTaskCreateStatic(Task_ReadCurrents,
                    "Current Sense Task",
                    TASK_READCURRENTS_STACK_SIZE,
                    NULL,
                    TASK_READCURRENTS_PRIORITY,
                    task_readcurrents_stack,
                    &task_readcurrents_buffer
                );

    xTaskCreateStatic(Task_HSSControl,
                    "HSS Control Task",
                    TASK_HSSCONTROL_STACK_SIZE,
                    NULL,
                    TASK_HSSCONTROL_PRIORITY,
                    task_hsscontrol_stack,
                    &task_hsscontrol_buffer
                );

    xTaskCreateStatic(Task_CanSendStatus,
                    "CAN Send Status task",
                    TASK_CANSENDSTATUS_STACK_SIZE,
                    NULL,
                    TASK_CANSENDSTATUS_PRIORITY,
                    task_cansendstatus_stack,
                    &task_cansendstatus_buffer
                );

    xTaskCreateStatic(Task_SDCard,
                    "SD Card Logging Task",
                    TASK_SDCARD_STACK_SIZE,
                    NULL,
                    TASK_SDCARD_PRIORITY,
                    task_sdcard_stack,
                    &task_sdcard_buffer
                );

    // task kills itself
    vTaskDelete(NULL);
}
