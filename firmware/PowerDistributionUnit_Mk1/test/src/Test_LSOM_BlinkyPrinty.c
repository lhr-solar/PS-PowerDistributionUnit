// LSOM_Blinky_Printy.c
// ----------------------------------------------------------------------------
// Makes the LSOM blink. Makes the LSOM print.

// INCLUDES -------------------------------------------------------------------

#include "stm32xx_hal.h"
// stm32xx_hal.h contains includes for RTOS stuff.
#include "printf.h"
#include "PDU_Mk1_Pins.h"

// DECLARATIONS ---------------------------------------------------------------

// for compatibility with BBPDU peripheral headers
SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;
SPI_HandleTypeDef hspi3;

StaticTask_t initTaskBuffer;
StackType_t initTaskStack[configMINIMAL_STACK_SIZE];
StaticTask_t blinkTaskBuffer;
StackType_t blinkTaskStack[configMINIMAL_STACK_SIZE];
StaticTask_t printTaskBuffer;
StackType_t printTaskStack[configMINIMAL_STACK_SIZE];

// TASKS ----------------------------------------------------------------------

// initialize GPIO and UART
void Init_Task(void *argument)
{
    GPIO_InitTypeDef led_config = {
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pull = GPIO_NOPULL,
        .Pin = LED_PIN
    };
    
    LED_CLOCK_INIT();
    HAL_GPIO_Init(LED_PORT, &led_config); // initialize GPIOA with led_config
    HAL_GPIO_WritePin(LED_PORT, LED_PIN, 0);

    // Init UART printf
    husart3->Init.BaudRate = 115200;
    husart3->Init.WordLength = UART_WORDLENGTH_8B;
    husart3->Init.StopBits = UART_STOPBITS_1;
    husart3->Init.Parity = UART_PARITY_NONE;
    husart3->Init.Mode = UART_MODE_TX_RX;
    husart3->Init.HwFlowCtl = UART_HWCONTROL_NONE;
    husart3->Init.OverSampling = UART_OVERSAMPLING_16;

    printf_init(husart3);

    // Task kills itself
    vTaskDelete(NULL);
}

// blinks...
void Blink_Task(void *argument)
{
    for(;;)
    {
        HAL_GPIO_TogglePin(LED_PORT, LED_PIN);

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

// prints...
void Print_Task(void *argument)
{
    for(;;)
    {
        printf("Hello, I'm running...\n");

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

// MAIN -----------------------------------------------------------------------

int main()
{
    HAL_Init();
    SystemClock_Config();

    xTaskCreateStatic(Init_Task,
                    "Init Task",
                    configMINIMAL_STACK_SIZE,
                    NULL,
                    tskIDLE_PRIORITY + 2,
                    initTaskStack,
                    &initTaskBuffer
                );

    xTaskCreateStatic(Blink_Task,
                    "Blink Task",
                    configMINIMAL_STACK_SIZE,
                    NULL,
                    tskIDLE_PRIORITY + 1,
                    blinkTaskStack,
                    &blinkTaskBuffer
                );

    xTaskCreateStatic(Print_Task,
                    "Print Task",
                    configMINIMAL_STACK_SIZE,
                    NULL,
                    tskIDLE_PRIORITY + 2,
                    printTaskStack,
                    &printTaskBuffer
                );

    vTaskStartScheduler();

    while(1) {}
}
