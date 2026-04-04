#include "MCP23S17.h"

// FUNCTION DEFINITIONS -------------------------------------------------------

MCP23S17_Status_t MCP23S17_WriteRegs(MCP23S17_HandleTypeDef* device, uint8_t reg_addr, uint8_t* data, uint16_t num_regs)
{
    if(MCP23S17_REG_INVALID_CHECK || num_regs == 0){return MCP23S17_😢;}

    // get SPI mutex / wait for SPI mutex to free (to prevent simulatenous SPI access)
    if(xSemaphoreTake(device->spi_mutex, MCP23S17_SPI_MUTEX_DELAY) != pdTRUE)
    {
        return MCP23S17_🕷️;
    }

    // bring CS pin low
    HAL_GPIO_WritePin(device->cs_port, device->cs_pin, 0);
    vTaskDelay(1);

    // command/device address word + starting register address word + num_regs
    uint8_t msg[2+num_regs];
    memset(msg, 0, 2+num_regs);
    
    msg[0] = MCP23S17_OPCODE_WRITE | (device->addr);        // command/device address
    msg[1] = reg_addr;                                      // starting register address
    memcpy(msg+2, data, num_regs);                          // register data to write

    // send SPI transmission
    if(HAL_SPI_Transmit_IT(device->spi, msg, 2+num_regs) != HAL_OK){return MCP23S17_😢;}

    // wait for SPI completion
    if(xSemaphoreTake(device->spi_done_sem, MCP23S17_SPI_TRANSMISSION_DELAY) != pdTRUE)
    {
        HAL_SPI_Abort(device->spi);

        return MCP23S17_🕸️;
    }

    // bring CS pin high again
    HAL_GPIO_WritePin(device->cs_port, device->cs_pin, 1);

    // release SPI mutex
    xSemaphoreGive(device->spi_mutex);

    return MCP23S17_🙂;
}

MCP23S17_Status_t MCP23S17_ReadRegs(MCP23S17_HandleTypeDef* device, uint8_t reg_addr, uint8_t* data, uint16_t num_regs)
{
    if(MCP23S17_REG_INVALID_CHECK || num_regs == 0){return MCP23S17_😢;}

    // get SPI mutex / wait for SPI mutex to free (to prevent simulatenous SPI access)
    if(xSemaphoreTake(device->spi_mutex, MCP23S17_SPI_MUTEX_DELAY) != pdTRUE)
    {
        return MCP23S17_🕷️;
    }

    // bring CS pin low
    HAL_GPIO_WritePin(device->cs_port, device->cs_pin, 0);
    vTaskDelay(1);

    // command/device address word + starting register address word + num_regs
    // HAL_SPI_Receive_x will transmit prexisting data in buffer, which takes care
    // of command / address words (while the rest are filled with data)
    uint8_t msg[2+num_regs];
    memset(msg, 0, 2+num_regs);
    
    msg[0] = MCP23S17_OPCODE_READ | (device->addr);         // command/device address
    msg[1] = reg_addr;                                      // starting register address

    // SPI transmission
    if(HAL_SPI_Receive_IT(device->spi, msg, 2+num_regs) != HAL_OK){return MCP23S17_😢;}

    // wait for SPI completion
    if(xSemaphoreTake(device->spi_done_sem, MCP23S17_SPI_TRANSMISSION_DELAY) != pdTRUE)
    {
        HAL_SPI_Abort(device->spi);

        return MCP23S17_🕸️;
    }

    // bring CS pin high again
    HAL_GPIO_WritePin(device->cs_port, device->cs_pin, 1);

    // release SPI mutex
    xSemaphoreGive(device->spi_mutex);

    // copy register data from msg buffer
    memcpy(data, msg+2, num_regs);

    return MCP23S17_🙂;
}

/**
 * @brief	Writes to a specific register bit (for a specific GPIO pin) on MCP23S17 in FRIENDLY manner (performs register read before write).
 * @param	device MCP23S17 Device Handle
 * @param	reg_addr Register Address
 * @param	pin Device GPIO Pin
 * @param	val Value to write in
 * @returns MCP23S17 Status (MCP23S17_🙂 if successful, MCP23S17_😢 otherwise)
 */
static inline MCP23S17_Status_t MCP23S17_WriteBitFriendly(MCP23S17_HandleTypeDef* device, uint8_t reg, MCP23S17_Pin_t pin, bool val)
{
    uint8_t reg_state = 0;
    if(MCP23S17_ReadRegs(device, reg, &reg_state, 1) != MCP23S17_🙂){return MCP23S17_😢;}

    if(val)
    {
        reg_state |= (0x01 << pin);
    }
    else
    {
        reg_state &= ~(0x01 << pin);
    }

    if(MCP23S17_WriteRegs(device, reg, &reg_state, 1) != MCP23S17_🙂){return MCP23S17_😢;}

   return MCP23S17_🙂;
}

/**
 * @brief	Reads a specific register bit (for a specific GPIO pin) on MCP23S17.
 * @param	device MCP23S17 Device Handle
 * @param	reg_addr Register Address
 * @param	pin Device GPIO Pin
 * @param   bool Pointer (bool) to store bit read
 * @returns MCP23S17 Status (MCP23S17_🙂 if successful, MCP23S17_😢 otherwise)
 */
static inline MCP23S17_Status_t MCP23S17_ReadBit(MCP23S17_HandleTypeDef* device, uint8_t reg, MCP23S17_Pin_t pin, bool* state)
{
    uint8_t reg_state = 0;
    if(MCP23S17_ReadRegs(device, reg, &reg_state, 1) != MCP23S17_🙂){return MCP23S17_😢;}
    *state = (reg_state >> pin) & 0x01;

    return MCP23S17_🙂;
}

MCP23S17_Status_t MCP23S17_Init(MCP23S17_HandleTypeDef* device)
{
    device->addr = device->address_en == MCP23S17_CONFIG_ADDRESSING_ENABLED ? device->addr << 1 : 0;

    // validate SPI configured correctly
    if(device->spi == NULL)
    {
        return MCP23S17_😢;
    }

    // validate SPI mutex and semaphore configured correctly
    if(device->spi_mutex == NULL || device->spi_done_sem == NULL)
    {
        return MCP23S17_😢;
    }

    // set up configuration
    uint8_t configuration = MCP23S17_IOCON_TEMPLATE
                                // IOCON.BANK = 0 | REGISTER ADDRESSING (This driver is designed ONLY for IOCON.BANK=0 register addresses.)
                                | (device->int_mirror << MCP23S17_IOCON_MIRROR_LSHIFT)  // IOCON.MIRROR | INT PIN MIRRORING
                                // IOCON.SEQOP = 0 | SEQUENTIAL OPERATION MODE (This driver is designed to utilize sequential operations.)
                                // IOCON.DISSLW = 0 | SDA SLEW RATE CONTROL (I don't know why this is here or what it's for.)
                                | (device->address_en << MCP23S17_IOCON_HAEN_LSHIFT)    // IOCON.HAEN | HARDWARE ADDRESS ENABLE
                                | (device->int_drive << MCP23S17_IOCON_ODR_LSHIFT)      // IOCON.ODR | INT PIN OPEN-DRAIN
                                | (device->int_pol << MCP23S17_IOCON_INTPOL_LSHIFT);    // IOCON.INTPOL | INT PIN POLARITY

    // write configuration
    if(MCP23S17_WriteRegs(device, MCP23S17_REG_IOCON, &configuration, 1) != MCP23S17_🙂){return MCP23S17_😢;}

    // verify configuration via readback
    uint8_t configuration_readback = 0;
    if(MCP23S17_ReadRegs(device, MCP23S17_REG_IOCON, &configuration_readback, 1) != MCP23S17_🙂){return MCP23S17_😢;}

    if(configuration == configuration_readback)
    {
       return MCP23S17_🙂;
    }
    else
    {
        return MCP23S17_😢;
    }
}

MCP23S17_Status_t MCP23S17_SetDirection_Pin(MCP23S17_HandleTypeDef* device, MCP23S17_Port_t port, MCP23S17_Pin_t pin, MCP23S17_Dir_t dir)
{
    if(MCP23S17_PORT_PIN_INVALID_CHECK){return MCP23S17_😢;}

    if(MCP23S17_WriteBitFriendly(device, (MCP23S17_REG_IODIRA+port), pin, dir) != MCP23S17_🙂){return MCP23S17_😢;}

    return MCP23S17_🙂;
}

MCP23S17_Status_t MCP23S17_SetPullup_Pin(MCP23S17_HandleTypeDef* device, MCP23S17_Port_t port, MCP23S17_Pin_t pin, MCP23S17_Pullup_t pu)
{
    if(MCP23S17_PORT_PIN_INVALID_CHECK){return MCP23S17_😢;}

    if(MCP23S17_WriteBitFriendly(device, (MCP23S17_REG_GPPUA+port), pin, pu) != MCP23S17_🙂){return MCP23S17_😢;}

    return MCP23S17_🙂;
}

MCP23S17_Status_t MCP23S17_SetInputPolarity_Pin(MCP23S17_HandleTypeDef* device, MCP23S17_Port_t port, MCP23S17_Pin_t pin, MCP23S17_InputPolarity_t pol)
{
    if(MCP23S17_PORT_PIN_INVALID_CHECK){return MCP23S17_😢;}

    if(MCP23S17_WriteBitFriendly(device, (MCP23S17_REG_IPOLA+port), pin, pol) != MCP23S17_🙂){return MCP23S17_😢;}

    return MCP23S17_🙂;
}

MCP23S17_Status_t MCP23S17_WriteGPIO_Pin(MCP23S17_HandleTypeDef* device, MCP23S17_Port_t port, MCP23S17_Pin_t pin, bool state)
{
    if(MCP23S17_PORT_PIN_INVALID_CHECK){return MCP23S17_😢;}

    if(MCP23S17_WriteBitFriendly(device, (MCP23S17_REG_GPIOA+port), pin, state) != MCP23S17_🙂){return MCP23S17_😢;}

    return MCP23S17_🙂;
}

MCP23S17_Status_t MCP23S17_WriteGPIO_All(MCP23S17_HandleTypeDef* device, uint8_t* state)
{
    if(MCP23S17_WriteRegs(device, MCP23S17_REG_GPIOA, state, 2) != MCP23S17_🙂){return MCP23S17_😢;}

    return MCP23S17_🙂;
}

MCP23S17_Status_t MCP23S17_ReadGPIO_Pin(MCP23S17_HandleTypeDef* device, MCP23S17_Port_t port, MCP23S17_Pin_t pin, bool* state)
{
    if(MCP23S17_PORT_PIN_INVALID_CHECK){return MCP23S17_😢;}

    return MCP23S17_ReadBit(device, (MCP23S17_REG_GPIOA+port), pin, state);
}

MCP23S17_Status_t MCP23S17_ReadGPIO_All(MCP23S17_HandleTypeDef* device, uint8_t* state)
{
    return MCP23S17_ReadRegs(device, MCP23S17_REG_GPIOA, state, 2);
}

MCP23S17_Status_t MCP23S17_SetInterruptEnable_Pin(MCP23S17_HandleTypeDef* device, MCP23S17_Port_t port, MCP23S17_Pin_t pin, MCP23S17_InterruptEnable_t inten)
{
    if(MCP23S17_PORT_PIN_INVALID_CHECK){return MCP23S17_😢;}

    return MCP23S17_WriteBitFriendly(device, (MCP23S17_REG_GPINTENA+port), pin, inten);
}

MCP23S17_Status_t MCP23S17_SetInterruptMode_Pin(MCP23S17_HandleTypeDef* device, MCP23S17_Port_t port, MCP23S17_Pin_t pin, MCP23S17_InterruptMode_t intmode)
{
    if(MCP23S17_PORT_PIN_INVALID_CHECK){return MCP23S17_😢;}

    return MCP23S17_WriteBitFriendly(device, (MCP23S17_REG_INTCONA+port), pin, intmode);
}

MCP23S17_Status_t MCP23S17_SetInterruptDefaultValue_Pin(MCP23S17_HandleTypeDef* device, MCP23S17_Port_t port, MCP23S17_Pin_t pin, bool defval)
{
    if(MCP23S17_PORT_PIN_INVALID_CHECK){return MCP23S17_😢;}

    return MCP23S17_WriteBitFriendly(device, (MCP23S17_REG_DEFVALA+port), pin, defval);
}

MCP23S17_Status_t MCP23S17_SetInterruptDefaultValue_All(MCP23S17_HandleTypeDef* device, uint8_t* defval)
{
    return MCP23S17_WriteRegs(device, MCP23S17_REG_DEFVALA, defval, 2);
}

MCP23S17_Status_t MCP23S17_ReadInterruptStatus_Port(MCP23S17_HandleTypeDef* device, MCP23S17_Port_t port, uint8_t* state)
{
    return MCP23S17_ReadRegs(device, (MCP23S17_REG_INTFA+port), state, 1);
}

MCP23S17_Status_t MCP23S17_ReadInterruptStatus_All(MCP23S17_HandleTypeDef* device, uint8_t* state)
{
    return MCP23S17_ReadRegs(device, MCP23S17_REG_INTFA, state, 2);
}

MCP23S17_Status_t MCP23S17_ReadInterruptGPIOState_Port(MCP23S17_HandleTypeDef* device, MCP23S17_Port_t port, uint8_t* state)
{
    return MCP23S17_ReadRegs(device, (MCP23S17_REG_INTCAPA+port), state, 1);
}

MCP23S17_Status_t MCP23S17_ReadInterruptGPIOState_All(MCP23S17_HandleTypeDef* device, uint8_t* state)
{
    return MCP23S17_ReadRegs(device, MCP23S17_REG_INTCAPA, state, 2);
}

// set up struct for pin information instead?
MCP23S17_Status_t MCP23S17_TheOneStopShopForAllYourOutputGPIOInitNeedsOfOneSpecificPin_DoneInOneLineOrYourMoneyBack(MCP23S17_HandleTypeDef* device, MCP23S17_PinConfigOutput_t pin_config)
{
    // pin setup
    if(MCP23S17_SetDirection_Pin(device, pin_config.port, pin_config.pin, MCP23S17_DIR_OUTPUT) != MCP23S17_🙂){return MCP23S17_😢;}
    if(MCP23S17_SetPullup_Pin(device, pin_config.port, pin_config.pin, MCP23S17_PULLUP_DISABLED) != MCP23S17_🙂){return MCP23S17_😢;}
    if(MCP23S17_WriteGPIO_Pin(device, pin_config.port, pin_config.pin, pin_config.initial_state) != MCP23S17_🙂){return MCP23S17_😢;}

    return MCP23S17_🙂;
}

MCP23S17_Status_t MCP23S17_GetAllOfYourSingleInputGPIOInitSetUpWithThisOneFunctionCallThatDoesEverythingForYourInstantly(MCP23S17_HandleTypeDef* device, MCP23S17_PinConfigInput_t pin_config)
{
    // pin setup
    if(MCP23S17_SetDirection_Pin(device, pin_config.port, pin_config.pin, MCP23S17_DIR_INPUT) != MCP23S17_🙂){return MCP23S17_😢;}
    if(MCP23S17_SetPullup_Pin(device, pin_config.port, pin_config.pin, pin_config.pullup) != MCP23S17_🙂){return MCP23S17_😢;}
    if(MCP23S17_SetInputPolarity_Pin(device, pin_config.port, pin_config.pin, pin_config.inpol) != MCP23S17_🙂){return MCP23S17_😢;}

    // interrupt setup
    if(pin_config.inten == MCP23S17_INTEN_ENABLED)
    {
        if(MCP23S17_SetInterruptEnable_Pin(device, pin_config.port, pin_config.pin, pin_config.inten) != MCP23S17_🙂){return MCP23S17_😢;}
        if(MCP23S17_SetInterruptMode_Pin(device, pin_config.port, pin_config.pin, pin_config.intmode) != MCP23S17_🙂){return MCP23S17_😢;}
        if(MCP23S17_SetInterruptDefaultValue_Pin(device, pin_config.port, pin_config.pin, pin_config.default_value) != MCP23S17_🙂){return MCP23S17_😢;}
    }

    return MCP23S17_🙂;
}

MCP23S17_Status_t MCP23S17_TheBestGPIOInterruptSetupThatYoullEverSeeAnywhereInTheSolarSystem_CallNowToSeeItHappen(MCP23S17_HandleTypeDef* device, MCP23S17_Port_t port, MCP23S17_Pin_t pin, MCP23S17_InterruptEnable_t inten, MCP23S17_InterruptMode_t intmode, bool defval)
{
    if(MCP23S17_SetInterruptEnable_Pin(device, port, pin, inten) != MCP23S17_🙂){return MCP23S17_😢;}
    if(MCP23S17_SetInterruptMode_Pin(device, port, pin, intmode) != MCP23S17_🙂){return MCP23S17_😢;}
    if(MCP23S17_SetInterruptDefaultValue_Pin(device, port, pin, defval) != MCP23S17_🙂){return MCP23S17_😢;}

    return MCP23S17_🙂;
}
