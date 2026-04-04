// MCP23S17_out_test.c
// Tests output capability of MCP23S17 GPIO expander on TestBoard_TPS27SA08-Q1
// ----------------------------------------------------------------------------
// THIS IS A BAD TEST AND IN IT'S CURRENT STATE SHOULD NOT BE REFERENCED FOR FUTURE TEST PRACTICES.
// Configures MCP23S17 GPIO A0, A1, and A2 as outputs. Initializes with all LEDs on, then cycling through 
// each LED, before returning to all LEDs on (and repeating) with state change every 100 ms. All SPI 
// communication with MCP23S17 is in a *blocking* manner. 

// INCLUDES -------------------------------------------------------------------
#include "stm32xx_hal.h"
#include "TestBoard_TPS27SA08-Q1_Pins.h"

#include "MCP23S17.h"

// DEFINES --------------------------------------------------------------------

// CONFIGURATION HANDLES/STRUCTS ----------------------------------------------
SPI_HandleTypeDef hspi1;

GPIO_InitTypeDef led_config = {
    .Mode = GPIO_MODE_OUTPUT_PP,
    .Pull = GPIO_NOPULL,
    .Pin = LED_PIN
};

GPIO_InitTypeDef gpioexp_cs_config = {
	.Mode = GPIO_MODE_OUTPUT_PP,
	.Pull = GPIO_NOPULL,
	.Pin = GPIOEXP_CS_PIN
};

MCP23S17_HandleTypeDef gpioexp;

MCP23S17_PinConfigOutput_t gpioexp_A0 = {
	.port = MCP23S17_GPIOA,
	.pin = MCP23S17_PIN0,
	.initial_state = 1,
};

MCP23S17_PinConfigOutput_t gpioexp_A1 = {
	.port = MCP23S17_GPIOA,
	.pin = MCP23S17_PIN1,
	.initial_state = 1,
};

MCP23S17_PinConfigOutput_t gpioexp_A2 = {
	.port = MCP23S17_GPIOA,
	.pin = MCP23S17_PIN2,
	.initial_state = 1,
};

// FUNCTION DECLARATIONS ------------------------------------------------------
void Error_Handler(void);

void SystemClock_Config(void);

void MX_SPI1_Init(void);
void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi);
void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi);

// GLOBAL VARIABLES -----------------------------------------------------------
const bool led_dance[4][3] = {{1, 1, 1}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
uint8_t led_dance_state = 0;

int main()
{
	HAL_Init();
	SystemClock_Config();
	MX_SPI1_Init();

	// initialize on-board LED
	__HAL_RCC_GPIOB_CLK_ENABLE();
	HAL_GPIO_Init(LED_PORT, &led_config);
	HAL_GPIO_WritePin(LED_PORT, LED_PIN, 0);

	// initialize GPIO expander CS pin
	__HAL_RCC_GPIOA_CLK_ENABLE();
	HAL_GPIO_Init(GPIOEXP_CS_PORT, &gpioexp_cs_config);
	HAL_GPIO_WritePin(GPIOEXP_CS_PORT, GPIOEXP_CS_PIN, 1);

	// dummy SPI send because for some reason SCK idles high until an initial transmit
	uint8_t dummy_send = 0x21;
	HAL_SPI_Transmit(&hspi1, &dummy_send, 1, HAL_MAX_DELAY);
	HAL_Delay(10);

	// initialize GPIO expander with config
	if(MCP23S17_Init(&gpioexp, &hspi1, GPIOEXP_CS_PORT, GPIOEXP_CS_PIN, 0x00, MCP23S17_CONFIG_INT_MIRRORED, MCP23S17_ADDRESSING_DISABLE, MCP23S17_CONFIG_INT_PP, MCP23S17_CONFIG_INT_ACTIVE_HIGH) != MCP23S17_🙂)
	{
		Error_Handler();
	}

	// initialize GPIO expander
	MCP23S17_TheOneStopShopForAllYourOutputGPIOInitNeedsOfOneSpecificPin_DoneInOneLineOrYourMoneyBack(&gpioexp,  gpioexp_A0);
	MCP23S17_TheOneStopShopForAllYourOutputGPIOInitNeedsOfOneSpecificPin_DoneInOneLineOrYourMoneyBack(&gpioexp,  gpioexp_A1);
	MCP23S17_TheOneStopShopForAllYourOutputGPIOInitNeedsOfOneSpecificPin_DoneInOneLineOrYourMoneyBack(&gpioexp,  gpioexp_A2);

	HAL_Delay(1000);

	while(1)
	{
		// LED shuffle (test outputs)
		MCP23S17_WriteGPIO_Pin(&gpioexp, MCP23S17_GPIOA, MCP23S17_PIN0, led_dance[led_dance_state][0]);
		MCP23S17_WriteGPIO_Pin(&gpioexp, MCP23S17_GPIOA, MCP23S17_PIN1, led_dance[led_dance_state][1]);
		MCP23S17_WriteGPIO_Pin(&gpioexp, MCP23S17_GPIOA, MCP23S17_PIN2, led_dance[led_dance_state][2]);

		led_dance_state++;
		if(led_dance_state > 3)
		{
			led_dance_state = 0;
		}

		HAL_Delay(100);
	}

	return 0;
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
	// __disable_irq();
	while (1)
	{
		HAL_GPIO_TogglePin(LED_PORT, LED_PIN);

		HAL_Delay(50);
	}
	/* USER CODE END Error_Handler_Debug */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 16;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable MSI Auto calibration
  */
  HAL_RCCEx_EnableMSIPLLMode();
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
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

/**
  * @brief SPI MSP Initialization
  * This function configures the hardware resources used in this example
  * @param hspi: SPI handle pointer
  * @retval None
  */
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
	}
}

/**
  * @brief SPI MSP De-Initialization
  * This function freeze the hardware resources used in this example
  * @param hspi: SPI handle pointer
  * @retval None
  */
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
