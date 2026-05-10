// PDU_Mk1_SPI.c

#include "PDU_Mk1_SPI.h"

bool SPI_RTOS_Mutex_Semaphore_Setup(SemaphoreHandle_t* SPIx_mutex, 
                                    StaticSemaphore_t* SPIx_mutex_buffer, 
                                    SemaphoreHandle_t* SPIx_done_sem, 
                                    StaticSemaphore_t* SPIx_done_sem_buffer)
{
    // create mutex (prevent simultaneous access to SPIx)
    *SPIx_mutex = xSemaphoreCreateMutexStatic(SPIx_mutex_buffer);
    //creates semaphore (tells when SPIx hardware has finished)
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
                                    

bool PDU_Mk1_SPI2_ADC_Init(void)
{
	/* SPI2 parameter configuration*/
	hspi2.Instance = SPI2;
	hspi2.Init.Mode = SPI_MODE_MASTER;
	hspi2.Init.Direction = SPI_DIRECTION_2LINES;
	hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi2.Init.CLKPhase = SPI_PHASE_2EDGE;
	hspi2.Init.NSS = SPI_NSS_SOFT;
	hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
	hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi2.Init.CRCPolynomial = 7;
	hspi2.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
	hspi2.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
	
    if(HAL_SPI_Init(&hspi2) != HAL_OK)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool PDU_Mk1_SPI2_HSS_SR_Init(void)
{
    /* SPI2 parameter configuration*/
    hspi2.Instance = SPI2;
    hspi2.Init.Mode = SPI_MODE_MASTER;
    hspi2.Init.Direction = SPI_DIRECTION_2LINES;
    hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi2.Init.NSS = SPI_NSS_SOFT;
    hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
    hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi2.Init.CRCPolynomial = 7;
    hspi2.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
    hspi2.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  
    if(HAL_SPI_Init(&hspi2) != HAL_OK)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool PDU_Mk1_SPI3_ADC_Init(void)
{
	/* SPI2 parameter configuration*/
	hspi3.Instance = SPI3;
	hspi3.Init.Mode = SPI_MODE_MASTER;
	hspi3.Init.Direction = SPI_DIRECTION_2LINES;
	hspi3.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi3.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi3.Init.CLKPhase = SPI_PHASE_2EDGE;
	hspi3.Init.NSS = SPI_NSS_SOFT;
	hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
	hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi3.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi3.Init.CRCPolynomial = 7;
	hspi3.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
	hspi3.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;

    if(HAL_SPI_Init(&hspi3) != HAL_OK)
    {
        return false;
    }
    else
    {
        return true;
    }
}

void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if(hspi->Instance==SPI1)
	{
        /* Peripheral clock enable */
        __HAL_RCC_SPI1_CLK_ENABLE();

        __HAL_RCC_GPIOA_CLK_ENABLE();
        /**SPI1 GPIO Configuration
        PA5     ------> SPI1_SCK
        PA6     ------> SPI1_MISO
        PA7     ------> SPI1_MOSI
        */
        GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* SPI1 interrupt Init */
        HAL_NVIC_SetPriority(SPI1_IRQn, SPI1_INTERRUPT_PRIORITY, 0);
        HAL_NVIC_EnableIRQ(SPI1_IRQn);
	}
	else if(hspi->Instance==SPI2)
	{
        /* Peripheral clock enable */
        __HAL_RCC_SPI2_CLK_ENABLE();

        __HAL_RCC_GPIOB_CLK_ENABLE();
        /**SPI2 GPIO Configuration
        PB13     ------> SPI2_SCK
        PB14     ------> SPI2_MISO
        PB15     ------> SPI2_MOSI
        */
        GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        /* SPI2 interrupt Init */
        HAL_NVIC_SetPriority(SPI2_IRQn, SPI2_INTERRUPT_PRIORITY, 0);
        HAL_NVIC_EnableIRQ(SPI2_IRQn);
	}
	else if(hspi->Instance==SPI3)
	{
        /* Peripheral clock enable */
        __HAL_RCC_SPI3_CLK_ENABLE();

        __HAL_RCC_GPIOB_CLK_ENABLE();
        /**SPI3 GPIO Configuration
        PB3     ------> SPI3_SCK
        PB4     ------> SPI3_MISO
        PB5     ------> SPI3_MOSI
        */
        GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        /* SPI3 interrupt Init */
        HAL_NVIC_SetPriority(SPI3_IRQn, SPI3_INTERRUPT_PRIORITY, 0);
        HAL_NVIC_EnableIRQ(SPI3_IRQn);
	}
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi)
{
	if(hspi->Instance==SPI1)
	{
	/* Peripheral clock disable */
	__HAL_RCC_SPI1_CLK_DISABLE();

	/**SPI1 GPIO Configuration
	PA5     ------> SPI1_SCK
	PA6     ------> SPI1_MISO
	PA7     ------> SPI1_MOSI
	*/
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);
	}
	else if(hspi->Instance==SPI2)
	{
	/* Peripheral clock disable */
	__HAL_RCC_SPI2_CLK_DISABLE();

	/**SPI2 GPIO Configuration
	PB13     ------> SPI2_SCK
	PB14     ------> SPI2_MISO
	PB15     ------> SPI2_MOSI
	*/
	HAL_GPIO_DeInit(GPIOB, GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15);
	}
	else if(hspi->Instance==SPI3)
	{
	/* Peripheral clock disable */
	__HAL_RCC_SPI3_CLK_DISABLE();

	/**SPI3 GPIO Configuration
	PB3     ------> SPI3_SCK
	PB4     ------> SPI3_MISO
	PB5     ------> SPI3_MOSI
	*/
	HAL_GPIO_DeInit(GPIOB, GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5);
	}
}

bool SPI_Init_Dummy_Send(SPI_HandleTypeDef* spi, SemaphoreHandle_t SPIx_mutex, SemaphoreHandle_t SPIx_done_sem)
{
    // take SPI mutex
	if(xSemaphoreTake(SPIx_mutex, pdMS_TO_TICKS(100)) != pdTRUE)
    {
        return false;
    }

    // dummy SPI send
    uint8_t dummy_spi_send = 0;
    if(HAL_SPI_Transmit_IT(spi, &dummy_spi_send, 1) != HAL_OK)
    {
        // release SPI mutex
        xSemaphoreGive(SPIx_mutex);

        return false;
    }

    // wait for SPI completion
    if(xSemaphoreTake(SPIx_done_sem, SPI_INIT_DUMMY_SEND_TIMEOUT) != pdTRUE)
    {
        // abort SPI transmission
        HAL_SPI_Abort(spi);
        // release SPI mutex
        xSemaphoreGive(SPIx_mutex);
        
        return false;
    }

    // release SPI mutex
    xSemaphoreGive(SPIx_mutex);

	return true;
}

// INTERRUPT STUFF ------------------------------------------------------------

/**
  * @brief SPI transmit completion callback.
  */
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef* hspi)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if(hspi == &hspi1)
    {
        if(spi1_done_sem != NULL)
        {
            xSemaphoreGiveFromISR(spi1_done_sem, &xHigherPriorityTaskWoken);
        }
    }
    else if(hspi == &hspi2)
    {
        if(spi2_done_sem != NULL)
        {
            xSemaphoreGiveFromISR(spi2_done_sem, &xHigherPriorityTaskWoken);
        }
    }
    else if(hspi == &hspi3)
    {
        if(spi3_done_sem != NULL)
        {
            xSemaphoreGiveFromISR(spi3_done_sem, &xHigherPriorityTaskWoken);
        }
    }
    
    // Context switch if a higher priority task was woken up
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/**
  * @brief SPI receive completion callback.
  */
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef* hspi)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if(hspi == &hspi1)
    {
        if(spi1_done_sem != NULL)
        {
            xSemaphoreGiveFromISR(spi1_done_sem, &xHigherPriorityTaskWoken);
        }
    }
    else if(hspi == &hspi2)
    {
        if(spi2_done_sem != NULL)
        {
            xSemaphoreGiveFromISR(spi2_done_sem, &xHigherPriorityTaskWoken);
        }
    }
    else if(hspi == &hspi3)
    {
        if(spi3_done_sem != NULL)
        {
            xSemaphoreGiveFromISR(spi3_done_sem, &xHigherPriorityTaskWoken);
        }
    }
    
    // Context switch if a higher priority task was woken up
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/**
  * @brief SPI transmit-receive completion callback.
  */
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef* hspi)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if(hspi == &hspi1)
    {
        if(spi1_done_sem != NULL)
        {
            xSemaphoreGiveFromISR(spi1_done_sem, &xHigherPriorityTaskWoken);
        }
    }
    else if(hspi == &hspi2)
    {
        if(spi2_done_sem != NULL)
        {
            xSemaphoreGiveFromISR(spi2_done_sem, &xHigherPriorityTaskWoken);
        }
    }
    else if(hspi == &hspi3)
    {
        if(spi3_done_sem != NULL)
        {
            xSemaphoreGiveFromISR(spi3_done_sem, &xHigherPriorityTaskWoken);
        }
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

/**
  * @brief This function handles SPI2 global interrupt.
  */
void SPI2_IRQHandler(void)
{
    HAL_SPI_IRQHandler(&hspi2);
}

/**
  * @brief This function handles SPI3 global interrupt.
  */
void SPI3_IRQHandler(void)
{
    HAL_SPI_IRQHandler(&hspi3);
}
