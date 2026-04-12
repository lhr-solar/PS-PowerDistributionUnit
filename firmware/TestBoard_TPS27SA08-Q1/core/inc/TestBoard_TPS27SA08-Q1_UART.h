#include "stm32xx_hal.h"
#include "TestBoard_TPS27SA08-Q1_Pins.h"

extern UART_HandleTypeDef huart2;

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
void MX_USART2_UART_Init(void);

/**
  * @brief UART MSP Initialization
  * This function configures the hardware resources used in this example
  * @param huart: UART handle pointer
  * @retval None
  */
void HAL_UART_MspGPIOInit(UART_HandleTypeDef* huart);

/**
  * @brief UART MSP De-Initialization
  * This function freeze the hardware resources used in this example
  * @param huart: UART handle pointer
  * @retval None
  */
void HAL_UART_MspGPIODeInit(UART_HandleTypeDef* huart);
