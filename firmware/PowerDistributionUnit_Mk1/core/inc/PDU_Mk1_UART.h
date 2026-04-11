// PDU_Mk1_SPI.h
#pragma once

#include "stm32xx_hal.h"
#include "printf.h"

extern UART_HandleTypeDef* husart3;

bool PDU_Mk1_UART_Printf_Init(void);
