// MCP23S17_in_test.c
// Tests input capability of MCP23S17 GPIO expander on TestBoard TPS27SA08-Q1
// ----------------------------------------------------------------------------
// Polls MCP23S17 GPIO Expander state every 100 ms.
// Prints out the GPIO state to serial monitor with baud rate 115200 bits/s. 
// Format: [A7] 10100000 [A0]    [B7] 00000011 [B0]
// Note that if LEDs are soldered on, can affect readings (specifically when 
// testing internal pull-up resistors).

#include "stm32xx_hal.h"
// stm32xx_hal.h contains includes for RTOS stuff.
// #include "printf.h"
#include "TestBoard_TPS27SA08-Q1_Pins.h"
#include "TestBoard_TPS27SA08-Q1_SPI.h"
#include "TestBoard_TPS27SA08-Q1_UART.h"
#include "Test_Utilities.h"
#include "MCP23S17.h"
#include<stdio.h>
#include<string.h>

// DECLARATIONS ---------------------------------------------------------------

// MCU Peripheral Handles
SPI_HandleTypeDef hspi1;
UART_HandleTypeDef huart2;

// Tasks
TaskHandle_t blink_task_handle;
TaskHandle_t gpioexp_int_task_handle;

StaticTask_t initTaskBuffer;
StackType_t initTaskStack[configMINIMAL_STACK_SIZE];
StaticTask_t blinkTaskBuffer;
StackType_t blinkTaskStack[configMINIMAL_STACK_SIZE];
StaticTask_t gpioexpIntTaskBuffer;
StackType_t gpioexpIntTaskStack[configMINIMAL_STACK_SIZE];

// RTOS crap
SemaphoreHandle_t spi1_mutex;           // Mutex to prevent simultaneous SPI access
StaticSemaphore_t spi1_mutex_buffer;    // Static buffer for mutex allocation

SemaphoreHandle_t spi1_done_sem;        // Semaphore to signal SPI DMA/IT completion
StaticSemaphore_t spi1_done_sem_buffer; // Static buffer for completion semaphore

// Driver Handles
MCP23S17_HandleTypeDef gpioexp;

// Variables 
uint8_t int_state[2] = {0, 0};
uint8_t gpio_state[2] = {0, 0};
uint8_t cap_state[2] = {0, 0};

char dashed_line[] = "----------------\n";
char newline[] = "\n";
char interrupt_msg[] = 		"Interrupt happened!\n";
char int_state_msg[] =  	"INT State:  [A7] XXXXXXXX [A0]   [B7] XXXXXXXX [B0]\n";
char gpio_state_msg[] = 	"GPIO State: [A7] XXXXXXXX [A0]   [B7] XXXXXXXX [B0]\n";
char cap_state_msg[] =  	"CAP State:  [A7] XXXXXXXX [A0]   [B7] XXXXXXXX [B0]\n";
char interest_indicator[] = "                   ^\n";

// TASKS ----------------------------------------------------------------------

// Initialize MCU peripherals and external device drivers
void Init_Task(void *argument)
{
    GPIO_InitTypeDef led_config = {
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pull = GPIO_NOPULL,
        .Pin = LED_PIN
    };

	// initialize LED pin
    LED_CLOCK_INIT();
    HAL_GPIO_Init(LED_PORT, &led_config);
    HAL_GPIO_WritePin(LED_PORT, LED_PIN, 0);

    GPIO_InitTypeDef gpioexp_cs = {
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pull = GPIO_NOPULL,
        .Pin = GPIOEXP_CS_PIN
    };

	// initialize GPIO expander CS pin
    __HAL_RCC_GPIOA_CLK_ENABLE();
    HAL_GPIO_Init(GPIOEXP_CS_PORT, &gpioexp_cs);
    HAL_GPIO_WritePin(GPIOEXP_CS_PORT, GPIOEXP_CS_PIN, 1);

    GPIO_InitTypeDef gpioexp_intA = {
        .Mode = GPIO_MODE_INPUT,
        .Pull = GPIO_NOPULL,
        .Pin = GPIOEXP_INTA_PIN
    };

    // initialize GPIO expander CS pin
    __HAL_RCC_GPIOA_CLK_ENABLE();
    HAL_GPIO_Init(GPIOEXP_INTA_PORT, &gpioexp_intA);

	// UART2 init
    MX_USART2_UART_Init();

	// SPI1 init
	MX_SPI1_Init();

    // set up SPI mutex and done semaphore
    if(SPI_RTOS_Mutex_Semaphore_Setup(&spi1_mutex, &spi1_mutex_buffer, &spi1_done_sem, &spi1_done_sem_buffer) != true)
    {
        char fail_spi_mutex_semaphore_init_msg[] = "FAIL:SPI_MUTEX_SEMAPHORE_INIT\n";
    	HAL_UART_Transmit(&huart2, (uint8_t*) fail_spi_mutex_semaphore_init_msg, strlen(fail_spi_mutex_semaphore_init_msg), HAL_MAX_DELAY);
        while(1)
        {
            HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
            HAL_Delay(50);
        }
    }

	// SPI dummy send because MCU starts with CLK idling high (despite configuration
	// to idle low) until first transmission
    if(SPI_Init_Dummy_Send(&hspi1, spi1_mutex, spi1_done_sem) != true)
    {
		char fail_spi_init_dummy_send_msg[] = "FAIL:SPI_INIT_DUMMY_SEND\n";
    	HAL_UART_Transmit(&huart2, (uint8_t*) fail_spi_init_dummy_send_msg, strlen(fail_spi_init_dummy_send_msg), HAL_MAX_DELAY);
        while(1)
        {
            HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
            HAL_Delay(50);
        }
    }

	// GPIO expander configuration
    gpioexp = (MCP23S17_HandleTypeDef) {
        .spi = &hspi1,
        .cs_port = GPIOEXP_CS_PORT,
        .cs_pin = GPIOEXP_CS_PIN,
        .spi_mutex = spi1_mutex,
        .spi_done_sem = spi1_done_sem,

        .address_en = MCP23S17_CONFIG_INT_MIRRORED,
        .int_mirror = MCP23S17_CONFIG_ADDRESSING_DISABLED, 
        .int_drive = MCP23S17_CONFIG_INTDRIVE_PP,
        .int_pol = MCP23S17_CONFIG_INTPOL_ACTIVE_HIGH,
    };

    if(MCP23S17_Init(&gpioexp) != MCP23S17_🙂)
    {
		char fail_mcp23s17_init_msg[] = "FAIL:MCP23S17_INIT\n";
    	HAL_UART_Transmit(&huart2, (uint8_t*) fail_mcp23s17_init_msg, strlen(fail_mcp23s17_init_msg), HAL_MAX_DELAY);
        while(1)
        {
            HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
            HAL_Delay(50);
        }
    }

	// GPIO expander pin configuration
    MCP23S17_PinConfigInput_t hss_st = {
        .port = MCP23S17_GPIOA,
        .pin = MCP23S17_PIN5,
        .pullup = MCP23S17_PULLUP_DISABLED,
        .inpol = MCP23S17_INPUTPOLARITY_SAME,
        .inten = MCP23S17_INTEN_ENABLED,
        .intmode = MCP23S17_INTMODE_ON_CHANGE,
        .default_value = 1,
    };

    if(MCP23S17_GetAllOfYourSingleInputGPIOInitSetUpWithThisOneFunctionCallThatDoesEverythingForYourInstantly(&gpioexp,  hss_st) != MCP23S17_🙂)
    {
        char fail_mcp23s17_pin_init_msg[] = "FAIL:MCP23S17_PIN_INIT\n";
    	HAL_UART_Transmit(&huart2, (uint8_t*) fail_mcp23s17_pin_init_msg, strlen(fail_mcp23s17_pin_init_msg), HAL_MAX_DELAY);
        while(1)
        {
            HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
            HAL_Delay(50);
        }
    }

	// celebrate!
	char initialization_complete_msg[] = "Initialization complete.\n";
    HAL_UART_Transmit(&huart2, (uint8_t*) initialization_complete_msg, strlen(initialization_complete_msg), HAL_MAX_DELAY);
	// let's get this party started...
    vTaskResume(blink_task_handle);
    vTaskResume(gpioexp_int_task_handle);

    // Init task kills itself
    vTaskDelete(NULL);
}

// Heartbeat LED
void Blink_Task(void *argument)
{
    for(;;)
    {
        char blink_task_runs_msg[] = "Blink_Task runs...\n";
    	HAL_UART_Transmit(&huart2, (uint8_t*) blink_task_runs_msg, strlen(blink_task_runs_msg), HAL_MAX_DELAY);

        HAL_GPIO_TogglePin(LED_PORT, LED_PIN);

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

// Poll GPIO expander inputs
void GpioExp_Int_Task(void *argument)
{
    for(;;)
    {
		char gpioexp_int_task_runs_msg[] = "GpioExp_Int_Task runs...\n";
    	HAL_UART_Transmit(&huart2, (uint8_t*) gpioexp_int_task_runs_msg, strlen(gpioexp_int_task_runs_msg), HAL_MAX_DELAY);

		if(HAL_GPIO_ReadPin(GPIOEXP_INTA_PORT, GPIOEXP_INTA_PIN))
		{
			// get states and acknowledge interrupt
			MCP23S17_ReadInterruptStatus_All(&gpioexp, int_state);	
			MCP23S17_ReadGPIO_All(&gpioexp, gpio_state);	
			MCP23S17_ReadInterruptGPIOState_All(&gpioexp, cap_state);

            // print notification to serial monitor
			HAL_UART_Transmit(&huart2, (uint8_t*) &dashed_line, strlen(dashed_line), HAL_MAX_DELAY);			
			HAL_UART_Transmit(&huart2, (uint8_t*) &interrupt_msg, strlen(interrupt_msg), HAL_MAX_DELAY);

			uint8_to_binary_str(int_state[0], int_state_msg+17);
			uint8_to_binary_str(int_state[1], int_state_msg+38);
			HAL_UART_Transmit(&huart2, (uint8_t*) &int_state_msg, strlen(int_state_msg), HAL_MAX_DELAY);

			uint8_to_binary_str(gpio_state[0], gpio_state_msg+17);
			uint8_to_binary_str(gpio_state[1], gpio_state_msg+38);
			HAL_UART_Transmit(&huart2, (uint8_t*) &gpio_state_msg, strlen(gpio_state_msg), HAL_MAX_DELAY);

			uint8_to_binary_str(cap_state[0], cap_state_msg+17);
			uint8_to_binary_str(cap_state[1], cap_state_msg+38);
			HAL_UART_Transmit(&huart2, (uint8_t*) &cap_state_msg, strlen(cap_state_msg), HAL_MAX_DELAY);
			HAL_UART_Transmit(&huart2, (uint8_t*) &interest_indicator, strlen(interest_indicator), HAL_MAX_DELAY);
			HAL_UART_Transmit(&huart2, (uint8_t*) &dashed_line, strlen(dashed_line), HAL_MAX_DELAY);
			HAL_UART_Transmit(&huart2, (uint8_t*) &newline, strlen(newline), HAL_MAX_DELAY);
		}

        vTaskDelay(pdMS_TO_TICKS(100));
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

    blink_task_handle = xTaskCreateStatic(Blink_Task,
                                            "Blink Task",
                                            configMINIMAL_STACK_SIZE,
                                            NULL,
                                            tskIDLE_PRIORITY + 1,
                                            blinkTaskStack,
                                            &blinkTaskBuffer
                                        );

    gpioexp_int_task_handle = xTaskCreateStatic(GpioExp_Int_Task,
                                                "GPIO Exp Int Task",
                                                configMINIMAL_STACK_SIZE,
                                                NULL,
                                                tskIDLE_PRIORITY + 1,
                                                gpioexpIntTaskStack,
                                                &gpioexpIntTaskBuffer
                                            );

    vTaskSuspend(blink_task_handle);
    vTaskSuspend(gpioexp_int_task_handle);

    vTaskStartScheduler();

    while(1) {}
}
