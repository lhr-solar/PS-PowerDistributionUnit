#include "TestBoard_TPS27SA08-Q1_SPI.h"

void MX_SPI1_Init(void)
{
	/* SPI1 parameter configuration*/
	hspi1.Instance = SPI1;
	hspi1.Init.Mode = SPI_MODE_MASTER;
	hspi1.Init.Direction = SPI_DIRECTION_2LINES;
	hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi1.Init.NSS = SPI_NSS_SOFT;
	hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
	hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi1.Init.CRCPolynomial = 7;
	hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
	hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
	if (HAL_SPI_Init(&hspi1) != HAL_OK)
	{
		Error_Handler();
	}
}

void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if(hspi->Instance==SPI1)
	{
		/* USER CODE BEGIN SPI1_MspInit 0 */

		/* USER CODE END SPI1_MspInit 0 */
		/* Peripheral clock enable */
		__HAL_RCC_SPI1_CLK_ENABLE();

		__HAL_RCC_GPIOA_CLK_ENABLE();
		/**SPI1 GPIO Configuration
		PA1     ------> SPI1_SCK
		PA11     ------> SPI1_MISO
		PA12     ------> SPI1_MOSI
		*/
		GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_11|GPIO_PIN_12;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* SPI1 interrupt Init */
        HAL_NVIC_SetPriority(SPI1_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY+1, 0);
        HAL_NVIC_EnableIRQ(SPI1_IRQn);
	}
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi)
{
	if(hspi->Instance==SPI1)
	{
		/* Peripheral clock disable */
		__HAL_RCC_SPI1_CLK_DISABLE();

		/**SPI1 GPIO Configuration
		PA1     ------> SPI1_SCK
		PA11     ------> SPI1_MISO
		PA12     ------> SPI1_MOSI
		*/
		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_1|GPIO_PIN_11|GPIO_PIN_12);
	}
}

bool SPI_Init_Dummy_Send(SPI_HandleTypeDef* spi, SemaphoreHandle_t spi2_mutex, SemaphoreHandle_t spi2_done_sem)
{
	if(xSemaphoreTake(spi2_mutex, pdMS_TO_TICKS(100)) != pdTRUE)
    {
        return false;
    }

    // SPI dummy send because CLK pin initializes high even when configured low for some reason
    uint8_t dummy_spi_send = 0;
    if(HAL_SPI_Transmit_IT(spi, &dummy_spi_send, 1) != HAL_OK)
    {
        return false;
    }

    if(xSemaphoreTake(spi2_done_sem, pdMS_TO_TICKS(100)) != pdTRUE)
    {
        return false;
    }

    xSemaphoreGive(spi2_mutex);

	return true;
}

bool SPI_RTOS_Mutex_Semaphore_Setup(SemaphoreHandle_t* SPIx_mutex, StaticSemaphore_t* SPIx_mutex_buffer, SemaphoreHandle_t* SPIx_done_sem, StaticSemaphore_t* SPIx_done_sem_buffer)
{
	// create mutex (prevent simultaneous access to SPIx)
    *SPIx_mutex = xSemaphoreCreateMutexStatic(SPIx_mutex_buffer);
    //creates semaphore (tells when SPIx hardware has finished transmission)
    *SPIx_done_sem = xSemaphoreCreateBinaryStatic(SPIx_done_sem_buffer);

	// check mutex and semaphore creation
    if((*SPIx_mutex == NULL) || (*SPIx_done_sem == NULL))
    {
		return false;
    }
	else
	{
		return true;
	}
}

// INTERRUPT STUFF ------------------------------------------------------------

extern SemaphoreHandle_t spi1_done_sem;

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef* hspi)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if(spi1_done_sem != NULL)
    {
        xSemaphoreGiveFromISR(spi1_done_sem, &xHigherPriorityTaskWoken);
    }

    // Context switch if a higher priority task was woken up
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef* hspi)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if(spi1_done_sem != NULL)
    {
        xSemaphoreGiveFromISR(spi1_done_sem, &xHigherPriorityTaskWoken);
    }

    // Context switch if a higher priority task was woken up
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef* hspi)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if(spi1_done_sem != NULL)
    {
        xSemaphoreGiveFromISR(spi1_done_sem, &xHigherPriorityTaskWoken);
    }
    
    // Context switch if a higher priority task was woken up
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/**
  * @brief This function handles SPI1 global interrupt.
  */
void SPI1_IRQHandler(void)
{
    HAL_SPI_IRQHandler(&hspi1);
}
