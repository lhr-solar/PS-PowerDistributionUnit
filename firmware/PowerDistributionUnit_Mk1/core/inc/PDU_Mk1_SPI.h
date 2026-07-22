// PDU_Mk1_SPI.h
// ----------------------------------------------------------------------------
// BBPDU Mk1 SPI configuration

#pragma once

#include "stm32xx_hal.h"
#include "sdcard.h"

#define SPI_INIT_DUMMY_SEND_TIMEOUT pdMS_TO_TICKS(100)

#define SPI1_INTERRUPT_PRIORITY configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY+1
#define SPI2_INTERRUPT_PRIORITY configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY+1
#define SPI3_INTERRUPT_PRIORITY configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY+1

extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;
extern SPI_HandleTypeDef hspi3;

extern SemaphoreHandle_t spi1_done_sem;
extern SemaphoreHandle_t spi2_done_sem;
extern SemaphoreHandle_t spi3_done_sem;

/**
	* @brief SPI RTOS mutex and completion semaphore intialization.
	* @param SPIx_mutex pointer to mutex handle for SPI
    * @param SPIx_mutex_buffer pointer to mutex buffer for SPI
    * @param SPIx_done_sem pointer to semaphore handle for SPI completion
    * @param SPIx_done_sem_buffer pointer to semaphore buffer for SPI completion
	* @retval bool: true if successful, false otherwise
*/
bool SPI_RTOS_Mutex_Semaphore_Setup(SemaphoreHandle_t* SPIx_mutex, 
                                    StaticSemaphore_t* SPIx_mutex_buffer, 
                                    SemaphoreHandle_t* SPIx_done_sem, 
                                    StaticSemaphore_t* SPIx_done_sem_buffer);

/**
	* @brief SPI1 Initialization Function for SD card use
	* @param None
	* @retval bool: true if successful, false otherwise
*/
bool PDU_Mk1_SPI1_SDCard_Init(void);

/**
	* @brief SPI2 Initialization Function for ADS131M08-Q1 Use
	* @param None
	* @retval bool: true if successful, false otherwise
	*/
bool PDU_Mk1_SPI2_ADC_Init(void);

/**
	* @brief SPI2 Initialization Function for HSS Control Shift Registers Use
	* @param None
	* @retval bool: true if successful, false otherwise
*/
bool PDU_Mk1_SPI2_HSS_SR_Init(void);

/**
	* @brief SPI3 Initialization Function for ADS131M08-Q1 ADC Use
	* @param None
	* @retval bool: true if successful, false otherwise
	*/
bool PDU_Mk1_SPI3_ADC_Init(void);

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

/**
	* @brief SPI init dummy send to bring CLK to idle low (initializes high even if 
    * set to idle low in configuration).
	* @param spi SPI handle pointer
    * @param SPIx_mutex pointer to SPI mutex
    * @param SPIx_done_sem pointer to SPI completion semaphore
	* @retval bool: true if successful, false otherwise
	*/
bool SPI_Init_Dummy_Send(SPI_HandleTypeDef* spi, SemaphoreHandle_t SPIx_mutex, SemaphoreHandle_t SPIx_done_sem);
