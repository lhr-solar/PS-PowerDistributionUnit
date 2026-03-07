// MCP23S17_in_test.c
// Tests input capability of MCP23S17 GPIO expander

// INCLUDES -------------------------------------------------------------------
#include "stm32xx_hal.h"
#include<stdio.h>
#include<string.h>

#include "MCP23S17.h"

// DEFINES --------------------------------------------------------------------
#define LED_PIN GPIO_PIN_3
#define LED_PORT GPIOB

#define VCP_TX_PIN GPIO_PIN_2
#define VCP_TX_PORT GPIOA
#define VCP_RX_PIN GPIO_PIN_15
#define VCP_RX_PORT GPIOA

#define GPIOEXP_CS_PIN GPIO_PIN_3
#define GPIOEXP_CS_PORT GPIOA

// CONFIGURATION HANDLES/STRUCTS ----------------------------------------------
SPI_HandleTypeDef hspi1;
UART_HandleTypeDef huart2;

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

MCP23S17_PinConfigInput gpioexp_B0 = {
	.port = MCP23S17_GPIOB,
	.pin = MCP23S17_PIN0,
	.pullup = MCP23S17_PULLUP_DISABLED,
	.inpol = MCP23S17_POLARITY_SAME,
	.inten = MCP23S17_INT_DISABLED,
	.intmode = MCP23S17_INT_ON_CHANGE,
	.default_value = 0,
};

MCP23S17_PinConfigInput gpioexp_B1 = {
	.port = MCP23S17_GPIOB,
	.pin = MCP23S17_PIN1,
	.pullup = MCP23S17_PULLUP_ENABLED,
	.inpol = MCP23S17_POLARITY_SAME,
	.inten = MCP23S17_INT_DISABLED,
	.intmode = MCP23S17_INT_ON_CHANGE,
	.default_value = 1,
};

MCP23S17_PinConfigInput gpioexp_B2 = {
	.port = MCP23S17_GPIOB,
	.pin = MCP23S17_PIN2,
	.pullup = MCP23S17_PULLUP_ENABLED,
	.inpol = MCP23S17_POLARITY_INVERT,
	.inten = MCP23S17_INT_DISABLED,
	.intmode = MCP23S17_INT_ON_CHANGE,
	.default_value = 1,
};

// FUNCTION DECLARATIONS ------------------------------------------------------
void Error_Handler(void);

void SystemClock_Config(void);

void MX_SPI1_Init(void);
void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi);
void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi);

void MX_USART2_UART_Init(void);
void HAL_UART_MspGPIOInit(UART_HandleTypeDef* huart);
void HAL_UART_MspGPIODeInit(UART_HandleTypeDef* huart);

static void uint8_to_binary_str(uint8_t val, char str[]);

// GLOBAL VARIABLES -----------------------------------------------------------
uint8_t gpio_state[2] = {0, 0};

char dashed_line[] = "----------------\n";
char newline[] = "\n";
char gpio_state_msg[] = "GPIO State: [A7] XXXXXXXX [A0]    [B7] XXXXXXXX [B0]\n";


int main()
{
	HAL_Init();
	SystemClock_Config();
	MX_SPI1_Init();
    MX_USART2_UART_Init();

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
	MCP23S17_GetAllOfYourSingleInputGPIOInitSetUpWithThisOneFunctionCallThatDoesEverythingForYourInstantly(&gpioexp,  gpioexp_B0);
	MCP23S17_GetAllOfYourSingleInputGPIOInitSetUpWithThisOneFunctionCallThatDoesEverythingForYourInstantly(&gpioexp,  gpioexp_B1);
	MCP23S17_GetAllOfYourSingleInputGPIOInitSetUpWithThisOneFunctionCallThatDoesEverythingForYourInstantly(&gpioexp,  gpioexp_B2);

	HAL_Delay(1000);

	while(1)
	{
		MCP23S17_ReadGPIO_All(&gpioexp, gpio_state);	// get states and acknowledge interrupt

		HAL_UART_Transmit(&huart2, (uint8_t*) &dashed_line, strlen(dashed_line), HAL_MAX_DELAY);
		uint8_to_binary_str(gpio_state[0], gpio_state_msg+17);
		uint8_to_binary_str(gpio_state[1], gpio_state_msg+39);
		HAL_UART_Transmit(&huart2, (uint8_t*) &gpio_state_msg, strlen(gpio_state_msg), HAL_MAX_DELAY);
		HAL_UART_Transmit(&huart2, (uint8_t*) &dashed_line, strlen(dashed_line), HAL_MAX_DELAY);
		HAL_UART_Transmit(&huart2, (uint8_t*) &newline, strlen(newline), HAL_MAX_DELAY);

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

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
void MX_USART2_UART_Init(void)
{
	huart2.Instance = USART2;
	huart2.Init.BaudRate = 115200;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart2) != HAL_OK)
	{
		Error_Handler();
	}
}

/**
  * @brief UART MSP Initialization
  * This function configures the hardware resources used in this example
  * @param huart: UART handle pointer
  * @retval None
  */
void HAL_UART_MspGPIOInit(UART_HandleTypeDef* huart)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
	if(huart->Instance==USART2)
	{
		/** Initializes the peripherals clock */
		PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
		PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
		if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
		{
		Error_Handler();
		}

		/* Peripheral clock enable */
		__HAL_RCC_USART2_CLK_ENABLE();

		__HAL_RCC_GPIOA_CLK_ENABLE();
		/**USART2 GPIO Configuration
		PA2     ------> USART2_TX
		PA15 (JTDI)     ------> USART2_RX
		*/
		GPIO_InitStruct.Pin = VCP_TX_PIN;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
		HAL_GPIO_Init(VCP_TX_PORT, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = VCP_RX_PIN;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF3_USART2;
		HAL_GPIO_Init(VCP_RX_PORT, &GPIO_InitStruct);
	}
}

/**
  * @brief UART MSP De-Initialization
  * This function freeze the hardware resources used in this example
  * @param huart: UART handle pointer
  * @retval None
  */
void HAL_UART_MspGPIODeInit(UART_HandleTypeDef* huart)
{
	if(huart->Instance==USART2)
	{
		/* Peripheral clock disable */
		__HAL_RCC_USART2_CLK_DISABLE();

		/**USART2 GPIO Configuration
		PA2     ------> USART2_TX
		PA15 (JTDI)     ------> USART2_RX
		*/
		HAL_GPIO_DeInit(GPIOA, VCP_TX_PIN|VCP_RX_PIN);
	}
}

static void uint8_to_binary_str(uint8_t val, char str[])
{
	str += 7;

	for(uint8_t i = 0; i < 8; i++)
	{
		*str = 48+(val & 0x01);
		val = val >> 1;
		str--;
	}
}
