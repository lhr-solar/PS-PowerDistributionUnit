// Task_Init.c

#include "Task_Init.h"

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

    if(PDU_Mk1_SPI2_ADC_Init() != true)
    {
        printf("FAIL:SPI2_INIT\n");
        Error_Handler();
    }

#if (PDU_MK1_REV_A == false)    // PDU_Mk1_REV_A has SPI pinout issue
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

#if (PDU_MK1_REV_A == false)    // PDU_Mk1_REV_A has SPI pinout issue
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

    if(PDU_Mk1_HSSControl_Init() != HSSCONTROL_🙂)
    {
        printf("FAIL:HSSCONTROL_INIT\n");
        Error_Handler();
    }

    printf("Initialization complete.\n");

    // get this party started
    PDU_Mk1_StartTasks();
    // task kills itself
    vTaskDelete(NULL);
}
