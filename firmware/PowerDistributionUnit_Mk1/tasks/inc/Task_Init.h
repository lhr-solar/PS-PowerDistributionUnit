// Task_Init.h
// ----------------------------------------------------------------------------
// Task inits everything.

#pragma once

// INCLUDES -------------------------------------------------------------------

#include "stm32xx_hal.h"

#include "PDU_Mk1_GPIO.h"
#include "PDU_Mk1_SPI.h"
#include "PDU_Mk1_UART.h"
#include "PDU_Mk1_CAN.h"

#include "PDU_Mk1_CurrentSensing.h"
#include "PDU_Mk1_HSSControl.h"

// DEFINES --------------------------------------------------------------------

#define TASK_INIT_STACK_SIZE configMINIMAL_STACK_SIZE+1000
#define TASK_INIT_PRIORITY tskIDLE_PRIORITY + 3

// VARIABLE DECLARATIONS ------------------------------------------------------

extern TaskHandle_t task_blink;
extern TaskHandle_t task_readcurrents;
extern TaskHandle_t task_cansendstatus;

extern SemaphoreHandle_t spi1_mutex;           // Mutex to prevent simultaneous SPI access
extern StaticSemaphore_t spi1_mutex_buffer;    // Static buffer for mutex allocation
extern SemaphoreHandle_t spi1_done_sem;        // Semaphore to signal SPI IT completion
extern StaticSemaphore_t spi1_done_sem_buffer; // Static buffer for completion semaphore

extern SemaphoreHandle_t spi2_mutex;           // Mutex to prevent simultaneous SPI access
extern StaticSemaphore_t spi2_mutex_buffer;    // Static buffer for mutex allocation
extern SemaphoreHandle_t spi2_done_sem;        // Semaphore to signal SPI IT completion
extern StaticSemaphore_t spi2_done_sem_buffer; // Static buffer for completion semaphore

extern SemaphoreHandle_t spi3_mutex;           // Mutex to prevent simultaneous SPI access
extern StaticSemaphore_t spi3_mutex_buffer;    // Static buffer for mutex allocation
extern SemaphoreHandle_t spi3_done_sem;        // Semaphore to signal SPI IT completion
extern StaticSemaphore_t spi3_done_sem_buffer; // Static buffer for completion semaphore

// FUNCTIONS ------------------------------------------------------------------

void Task_Init(void *argument);
