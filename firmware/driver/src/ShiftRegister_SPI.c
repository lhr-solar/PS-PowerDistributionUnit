// ShiftRegister_SPI.c

#include "ShiftRegister_SPI.h"

// FUNCTION DEFINITIONS -------------------------------------------------------

ShiftRegister_SPI_Status_t SR_SPI_Init(ShiftRegister_SPI_HandleTypeDef* sr)
{
    // validate SPI configured correctly
    if(sr->spi == NULL)
    {
        return SR_SPI_😢;
    }
    // validate SPI RTOS stuff initialized correctly
    if(sr->spi_mutex == NULL || sr->spi_done_sem == NULL)
    {
        return SR_SPI_😢;
    }
    // validate data is pointing somewhere
    if(sr->data == NULL || sr->num_bytes == 0)
    {
        return SR_SPI_😢;
    }

    SR_SPI_Assert_AllOff(sr);
    vTaskDelay(SR_SPI_INITIAL_ALL_OFF_DELAY_TICKS);
    SR_SPI_Deassert_AllOff(sr);

    return SR_SPI_SetRegs(sr);
}

ShiftRegister_SPI_Status_t SR_SPI_SetRegs(ShiftRegister_SPI_HandleTypeDef* sr)
{
    // take SPI mutex
    if(xSemaphoreTake(sr->spi_mutex, SR_SPI_MUTEX_DELAY_TICKS) != pdTRUE)
    {
        return SR_SPI_🕷️;
    }

    // enable SPI CLK to reach SR
    HAL_GPIO_WritePin(sr->en_port, sr->en_pin, 1);
    vTaskDelay(pdMS_TO_TICKS(1));

    // transmit data
    if(HAL_SPI_Transmit_IT(sr->spi, sr->data, sr->num_bytes) != HAL_OK)
    {
        HAL_GPIO_WritePin(sr->en_port, sr->en_pin, 0);
        xSemaphoreGive(sr->spi_mutex);

        return SR_SPI_😢;
    }

    // take spi completion semaphore
    if(xSemaphoreTake(sr->spi_done_sem, SR_SPI_TRANSMISSION_DELAY_TICKS) != pdTRUE)
    {
        HAL_SPI_Abort(sr->spi);

        HAL_GPIO_WritePin(sr->en_port, sr->en_pin, 0);
        xSemaphoreGive(sr->spi_mutex);

        return SR_SPI_🕸️;
    }

    // done with SPI
    HAL_GPIO_WritePin(sr->en_port, sr->en_pin, 0);
    xSemaphoreGive(sr->spi_mutex);

    // write output register with new values
    HAL_GPIO_WritePin(sr->out_clk_port, sr->out_clk_pin, 1);
    vTaskDelay(SR_SPI_EN_DELAY_TICKS);
    HAL_GPIO_WritePin(sr->out_clk_port, sr->out_clk_pin, 0);

    return SR_SPI_🙂;
}

inline void SR_SPI_Assert_AllOff(ShiftRegister_SPI_HandleTypeDef* sr)
{
    HAL_GPIO_WritePin(sr->all_off_port, sr->all_off_pin, 0);
}

inline void SR_SPI_Deassert_AllOff(ShiftRegister_SPI_HandleTypeDef* sr)
{
    HAL_GPIO_WritePin(sr->all_off_port, sr->all_off_pin, 1);
}
