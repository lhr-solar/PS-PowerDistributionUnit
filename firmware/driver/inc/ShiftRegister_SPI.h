// ShiftRegister_SPI.h
// ----------------------------------------------------------------------------
// Driver for shift registers with shfit and output registers that make use
// of shared active-low shift register reset and output register reset pins. 
// Ex: 74LVC594A
// Uses SPI for fast, non-blocking shifting (instead of bit-banging).

#pragma once

#include "stm32xx_hal.h"

#define SR_SPI_MUTEX_DELAY_TICKS portMAX_DELAY
#define SR_SPI_TRANSMISSION_DELAY_TICKS pdMS_TO_TICKS(100)
#define SR_SPI_EN_DELAY_TICKS pdMS_TO_TICKS(1)
#define SR_SPI_INITIAL_ALL_OFF_DELAY_TICKS pdMS_TO_TICKS(1)

typedef enum {
	SR_SPI_😢,				// ShiftRegister_SPI sad
	SR_SPI_🙂,				// ShiftRegister_SPI happy
	SR_SPI_🕷️,    		    // ShiftRegister_SPI SPI mutex timeout
	SR_SPI_🕸️,			    // ShiftRegister_SPI_ SPI done semaphore timeout
} ShiftRegister_SPI_Status_t;

typedef struct {
	SPI_HandleTypeDef* spi;             // STM32 HAL SPI handle
	GPIO_TypeDef* en_port;              // enables SPI CLK to reach SR
	uint16_t en_pin;                    // like CS, but active-high

    GPIO_TypeDef* out_clk_port;         // out clk moves SR contents to output reg
	uint16_t out_clk_pin;

    GPIO_TypeDef* all_off_port;         // turns off all SR outputs
	uint16_t all_off_pin;

	SemaphoreHandle_t spi_mutex;        // Mutex to prevent simultaenous SPI access
	SemaphoreHandle_t spi_done_sem;     // Semaphore to signal SPI transmission complete

    uint8_t* data;                      // pointer to SR values
    uint8_t num_bytes;
} ShiftRegister_SPI_HandleTypeDef;

ShiftRegister_SPI_Status_t SR_SPI_Init(ShiftRegister_SPI_HandleTypeDef* sr);

ShiftRegister_SPI_Status_t SR_SPI_SetRegs(ShiftRegister_SPI_HandleTypeDef* sr);

void SR_SPI_Assert_AllOff(ShiftRegister_SPI_HandleTypeDef* sr);

void SR_SPI_Deassert_AllOff(ShiftRegister_SPI_HandleTypeDef* sr);
