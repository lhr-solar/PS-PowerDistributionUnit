#include "stm32xx_hal.h"

#define SPI1_INTERRUPT_PRIORITY configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY+1

extern SPI_HandleTypeDef hspi1;

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
void MX_SPI1_Init(void);

/**
  * @brief SPI MSP Initialization
  * This function configures the hardware resources used in this example
  * @param hspi: SPI handle pointer
  * @retval None
  */
void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi);

/**
  * @brief SPI MSP De-Initialization
  * This function freeze the hardware resources used in this example
  * @param hspi: SPI handle pointer
  * @retval None
  */
void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi);

bool SPI_Init_Dummy_Send(SPI_HandleTypeDef* spi, SemaphoreHandle_t spi2_mutex, SemaphoreHandle_t spi2_done_sem);

bool SPI_RTOS_Mutex_Semaphore_Setup(SemaphoreHandle_t* SPIx_mutex, StaticSemaphore_t* SPIx_mutex_buffer, SemaphoreHandle_t* SPIx_done_sem, StaticSemaphore_t* SPIx_done_sem_buffer);
