// LSOM_Blinky.c
// ----------------------------------------------------------------------------
// copied from Embedded-Sharepoint

#include "stm32xx_hal.h"
#include "PDU_Mk1_Pins.h"
#include "PDU_Mk1_SPI.h"
#include "ADS131M08-Q1.h"

uint8_t num = 0;

int main(){
    HAL_Init();

    GPIO_InitTypeDef led_config = {
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pull = GPIO_NOPULL,
        .Pin = LED_PIN
    };
    
    __HAL_RCC_GPIOC_CLK_ENABLE();
    HAL_GPIO_Init(LED_PORT, &led_config); // initialize GPIOA with led_config
    HAL_GPIO_WritePin(LED_PORT, LED_PIN, 0);

    SPI2_HSS_SR_Init();

    while(1){
        HAL_SPI_Transmit(&hspi2, &num, 1, HAL_MAX_DELAY);
        // HAL_SPI_Transmit(&hspi3, &num, 1, HAL_MAX_DELAY);
        num = ADS131M08Q1_REG_CHx_OCAL_LSB(9999);

        HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
        HAL_Delay(500);
    }

    return 0;
}
