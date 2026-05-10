// ADS131M08-Q1.c

#include "ADS131M08-Q1.h"

inline ADS131M08Q1_Status_t ADS131M08Q1_Frame_Transmit(ADS131M08Q1_HandleTypeDef* device, uint8_t* out_data)
{
    return ADS131M08Q1_Frame_TransmitReceive(device, out_data, NULL);
}

inline ADS131M08Q1_Status_t ADS131M08Q1_Frame_Receive(ADS131M08Q1_HandleTypeDef* device, uint8_t* in_data)
{
    return ADS131M08Q1_Frame_TransmitReceive(device, NULL, in_data);
}

inline ADS131M08Q1_Status_t ADS131M08Q1_Frame_TransmitReceive(ADS131M08Q1_HandleTypeDef* device, uint8_t* out_data, uint8_t* in_data)
{
    return ADS131M08Q1_FrameVar_TransmitReceive(device, out_data, in_data, ADS131M08Q1_FRAME_LEN_24);
}

inline ADS131M08Q1_Status_t ADS131M08Q1_FrameVar_Transmit(ADS131M08Q1_HandleTypeDef* device, uint8_t* out_data, uint8_t num_words)
{
    return ADS131M08Q1_FrameVar_TransmitReceive(device, out_data, NULL, num_words);
}

inline ADS131M08Q1_Status_t ADS131M08Q1_FrameVar_Receive(ADS131M08Q1_HandleTypeDef* device, uint8_t* in_data, uint8_t num_words)
{
    return ADS131M08Q1_FrameVar_TransmitReceive(device, NULL, in_data, num_words);
}

ADS131M08Q1_Status_t ADS131M08Q1_FrameVar_TransmitReceive(ADS131M08Q1_HandleTypeDef* device, uint8_t* out_data, uint8_t* in_data, uint8_t num_words)
{
    // validate SPI configured correctly
    // SPI CLKPhase must be set to SPI_PHASE_2EDGE (data valid on clock TRAILING EDGE)!!!
    if(device->spi == NULL || device->spi->Init.CLKPhase != SPI_PHASE_2EDGE)
    {
        return ADS131M08Q1_😢;
    }
    // validate SPI RTOS stuff initialized correctly
    if(device->spi_mutex == NULL || device->spi_done_sem == NULL)
    {
        return ADS131M08Q1_😢;
    }

    HAL_GPIO_WritePin(device->cs_port, device->cs_pin, 0);
    vTaskDelay(ADS131M08Q1_CS_DELAY_TICKS);

    if(in_data == NULL)
    {
        if(out_data == NULL)
        {
            HAL_GPIO_WritePin(device->cs_port, device->cs_pin, 1);

            return ADS131M08Q1_😢;
        }

        // Transmit only
        if(HAL_SPI_Transmit_IT(device->spi, out_data, ADS131M08Q1_WORD_LEN_8*num_words) != HAL_OK)
        {
            HAL_GPIO_WritePin(device->cs_port, device->cs_pin, 1);

            return ADS131M08Q1_😢;
        }
    }
    else if(out_data == NULL)
    {
        if(in_data == NULL)
        {
            HAL_GPIO_WritePin(device->cs_port, device->cs_pin, 1);

            return ADS131M08Q1_😢;
        }

        // Clear data buffer so HAL_SPI_Receive only sends out zeros
        memset(in_data, 0, ADS131M08Q1_WORD_LEN_8*num_words);

        // Receive only
        if(HAL_SPI_Receive_IT(device->spi, in_data, ADS131M08Q1_WORD_LEN_8*num_words) != HAL_OK)
        {
            HAL_GPIO_WritePin(device->cs_port, device->cs_pin, 1);

            return ADS131M08Q1_😢;
        }
    }
    else if(out_data != NULL && in_data != NULL)
    {
        // Transmit and receive simultaneously
        if(HAL_SPI_TransmitReceive_IT(device->spi, out_data, in_data, ADS131M08Q1_WORD_LEN_8*num_words) != HAL_OK)
        {
            HAL_GPIO_WritePin(device->cs_port, device->cs_pin, 1);

            return ADS131M08Q1_😢;
        }
    }
    else
    {
        HAL_GPIO_WritePin(device->cs_port, device->cs_pin, 1);

        return ADS131M08Q1_😢;
    }

    // take spi completion semaphore
    if(xSemaphoreTake(device->spi_done_sem, ADS131M08Q1_SPI_TRANSMISSION_DELAY_TICKS) != pdTRUE)
    {
        HAL_SPI_Abort(device->spi);

        HAL_GPIO_WritePin(device->cs_port, device->cs_pin, 1);

        return ADS131M08Q1_🕸️;
    }

    HAL_GPIO_WritePin(device->cs_port, device->cs_pin, 1);

    return ADS131M08Q1_🙂;
}

inline ADS131M08Q1_Status_t ADS131M08Q1_SendCommand(ADS131M08Q1_HandleTypeDef* device, uint8_t cmd_MSB, uint8_t cmd_LSB, uint8_t response_MSB, uint8_t response_LSB, uint8_t response_delay_ms)
{
    uint8_t cmd_frame[ADS131M08Q1_FRAME_LEN_8] = {0};
    cmd_frame[0] = cmd_MSB;
    cmd_frame[1] = cmd_LSB;

    uint8_t frame_response[ADS131M08Q1_FRAME_LEN_8] = {0};

    if(xSemaphoreTake(device->spi_mutex, ADS131M08Q1_SPI_MUTEX_DELAY_TICKS) != pdTRUE)
    {
        return ADS131M08Q1_🕷️;
    }

    // send ADS131M08-Q1 frame
    if(ADS131M08Q1_Frame_Transmit(device, cmd_frame) != ADS131M08Q1_🙂)
    {
        // release SPI mutex
        xSemaphoreGive(device->spi_mutex);

        return ADS131M08Q1_😢;
    }

    vTaskDelay(pdMS_TO_TICKS(response_delay_ms));

    if(ADS131M08Q1_Frame_Receive(device, frame_response) != ADS131M08Q1_🙂)
    {
        // release SPI mutex
        xSemaphoreGive(device->spi_mutex);

        return ADS131M08Q1_😢;
    }
    
    // release SPI mutex
    xSemaphoreGive(device->spi_mutex);

    // check for correct response
    if(frame_response[0] == response_MSB && frame_response[1] == response_LSB)
    {
        return ADS131M08Q1_🙂;
    }
    else
    {
        return ADS131M08Q1_😢;
    }
}

ADS131M08Q1_Status_t ADS131M08Q1_ReadRegs(ADS131M08Q1_HandleTypeDef* device, uint8_t reg_addr, uint16_t* data, uint8_t num_regs)
{
    if(ADS131M08Q1_REG_INVALID_CHECK(reg_addr, num_regs)){return ADS131M08Q1_😢;}

    uint8_t rreg_cmd[ADS131M08Q1_FRAME_LEN_8] = {0};

    // RREG command format: 101a aaaa annn nnnn
    // aaaaaa = starting address, nnnnnnn = number of regs - 1
    rreg_cmd[0] = ADS131M08Q1_OPCODE_RREG_MSB | (reg_addr >> 1);
    rreg_cmd[1] = ((reg_addr & 0x01) << 7) | (num_regs-1);

    if(xSemaphoreTake(device->spi_mutex, ADS131M08Q1_SPI_MUTEX_DELAY_TICKS) != pdTRUE)
    {
        return ADS131M08Q1_🕷️;
    }
    
    if(ADS131M08Q1_Frame_Transmit(device, rreg_cmd) != ADS131M08Q1_🙂)
    {
        // release SPI mutex
        xSemaphoreGive(device->spi_mutex);

        return ADS131M08Q1_😢;
    }

    if(num_regs == 1)
    {
        // when reading one reg, the reg is read out on the response (first word) of the next frame
        // no separate response is given

        uint8_t frame_response[ADS131M08Q1_FRAME_LEN_8] = {0};

        if(ADS131M08Q1_Frame_Receive(device, frame_response) != ADS131M08Q1_🙂)
        {
            // release SPI mutex
            xSemaphoreGive(device->spi_mutex);

            return ADS131M08Q1_😢;
        }
        
        // release SPI mutex
        xSemaphoreGive(device->spi_mutex);

        *data = __builtin_bswap16(*((uint16_t*) frame_response));
    }
    else
    {
        // when reading more than one reg, the regs are read out after a response 
        // (same as command, but starts with 111...)

        // +2 words for RREG response word and for CRC word (unused but sent by IC)
        uint8_t rreg_response[ADS131M08Q1_WORD_LEN_8*(num_regs+2)];
        for(uint8_t i = 0; i < ADS131M08Q1_WORD_LEN_8*(num_regs+2); i++)
        {
            rreg_response[i] = 0;
        }

        if(ADS131M08Q1_FrameVar_Receive(device, rreg_response, num_regs+2) != ADS131M08Q1_🙂)
        {
            // release SPI mutex
            xSemaphoreGive(device->spi_mutex);

            return ADS131M08Q1_😢;
        }
        
        // release SPI mutex
        xSemaphoreGive(device->spi_mutex);

        // check RREG response
        if(__builtin_bswap16(*((uint16_t*) rreg_response)) != ((ADS131M08Q1_RESPONSE_RREG_MSB << 8) | (reg_addr << 7) | (num_regs-1)))
        {
            return ADS131M08Q1_😢;
        }

        for(uint8_t i = 0; i < num_regs; i++)
        {
            data[i] = __builtin_bswap16(*((uint16_t*) (rreg_response+3*(i+1))));
        }

        // potentially more optimal way is to clock one frame, check reponse, then clock the register readouts
        // (storing directly into data buffer passed in), then clock one word for crc to avoid extra frame_response
        // buffer - don't want to deal with the extra complexity of a word function though right now
    }

    return ADS131M08Q1_🙂;
}

ADS131M08Q1_Status_t ADS131M08Q1_WriteRegs(ADS131M08Q1_HandleTypeDef* device, uint8_t reg_addr, uint16_t* data, uint8_t num_regs)
{
    if(ADS131M08Q1_REG_INVALID_CHECK(reg_addr, num_regs)){return ADS131M08Q1_😢;}

    uint8_t frame_len_24 = 0;
    // check if enough words to fill up an SPI frame
    if(num_regs < (ADS131M08Q1_FRAME_LEN_24-1))
    {
        // need to clock at least ten frames (to maintain synchronization with ADC data readout)
        frame_len_24 = ADS131M08Q1_FRAME_LEN_24;
    }
    else
    {
        // num_regs + 1 (command word)
        frame_len_24 = num_regs+1;
    }

    uint8_t wreg_cmd[frame_len_24*ADS131M08Q1_WORD_LEN_8];
    for(uint8_t i = 0; i < frame_len_24*ADS131M08Q1_WORD_LEN_8; i++)
    {
        wreg_cmd[i] = 0;
    }

    // WREG command format: 011a aaaa annn nnnn
    // aaaaaa = starting address, nnnnnnn = number of regs - 1
    wreg_cmd[0] = ADS131M08Q1_OPCODE_WREG_MSB | (reg_addr >> 1);
    wreg_cmd[1] = ((reg_addr & 0x01) << 7) | (num_regs-1);

    for(uint8_t i = 0; i < num_regs; i++)
    {
        *((uint16_t*) (wreg_cmd+(3*(i+1)))) = __builtin_bswap16(data[i]);
    }

    if(xSemaphoreTake(device->spi_mutex, ADS131M08Q1_SPI_MUTEX_DELAY_TICKS) != pdTRUE)
    {
        return ADS131M08Q1_🕷️;
    }

    if(ADS131M08Q1_FrameVar_Transmit(device, wreg_cmd, frame_len_24) != ADS131M08Q1_🙂)
    {
        // release SPI mutex
        xSemaphoreGive(device->spi_mutex);
        
        return ADS131M08Q1_😢;
    }

    uint8_t wreg_response[ADS131M08Q1_FRAME_LEN_8] = {0};   // TODO: move these to top

    // send another frame to get WREG response
    if(ADS131M08Q1_Frame_Receive(device, wreg_response) != ADS131M08Q1_🙂)    // TODO: change this to just read
    {
        // release SPI mutex
        xSemaphoreGive(device->spi_mutex);
        
        return ADS131M08Q1_😢;
    }
    
    // release SPI mutex
    xSemaphoreGive(device->spi_mutex);

    // check WREG response (same as WREG command, but starts with 010...)
    if(__builtin_bswap16(*((uint16_t*) wreg_response)) != ((ADS131M08Q1_RESPONSE_WREG_MSB << 8) | (reg_addr << 7) | (num_regs-1)))
    {
        return ADS131M08Q1_😢;
    }

    return ADS131M08Q1_🙂;
}

ADS131M08Q1_Status_t ADS131M08Q1_Init(ADS131M08Q1_HandleTypeDef* device)
{
    // validate SPI configured correctly
    // SPI CLKPhase must be set to SPI_PHASE_2EDGE (data valid on clock TRAILING EDGE)!!!
    if(device->spi == NULL || device->spi->Init.CLKPhase != SPI_PHASE_2EDGE)
    {
        return ADS131M08Q1_😢;
    }
    // validate SPI RTOS stuff initialized correctly
    if(device->spi_mutex == NULL || device->spi_done_sem == NULL)
    {
        return ADS131M08Q1_😢;
    }

    if(device->config.reference_source == ADS131M08Q1_REFERENCE_SOURCE_INTERNAL)
    {
        device->config.fsr = ADS131M08Q1_INTERNAL_REFERENCE_V;
    }

    // RESET DEVICE (ensure device regs in known state)
    // --------------------------------
    if(ADS131M08Q1_Reset(device) != ADS131M08Q1_🙂)
    {
        return ADS131M08Q1_😢;
    }

    // ID CHECK
    // --------------------------------
    uint16_t id_check = 0;
    if(ADS131M08Q1_ReadRegs(device, ADS131M08Q1_REG_ID, &id_check, 1) != ADS131M08Q1_🙂)
    {
        return ADS131M08Q1_😢;
    }
    if((id_check >> 8) != ADS131M08Q1_REG_ID_VAL)
    {
        return ADS131M08Q1_😢;
    }

    // WRITE CONFIGURATION
    // --------------------------------
    uint16_t global_configuration[4] = {0};

    // MODE fields
    global_configuration[0] = ADS131M08Q1_CONFIGTEMPLATE_MODE 
                                | (device->config.drdy_source << ADS131M08Q1_MODECONFIG_LSHIFT_DRDY_SOURCE)
                                | (device->config.drdy_idlepinstate << ADS131M08Q1_MODECONFIG_LSHIFT_DRDY_IDLEPINSTATE)
                                | (device->config.drdy_format << ADS131M08Q1_MODECONFIG_LSHIFT_DRDY_FORMAT);
    
    // CLOCK fields
    global_configuration[1] = ADS131M08Q1_CONFIGTEMPLATE_CLOCK
                                | (device->config.reference_source << ADS131M08Q1_CLOCKCONFIG_LSHIFT_REFERENCE_SOURCE)
                                | (device->config.powermode << ADS131M08Q1_CLOCKCONFIG_LSHIFT_POWERMODE);
    for(uint8_t i = 0; i < ADS131M08Q1_NUM_CHANNELS; i++)
    {
        global_configuration[1] |= (device->config.ch_configs[i].enable << ADS131M08Q1_CLOCKCONFIG_LSHIFT_CHx_ENABLE(i));
    }
    
    // GAIN1 and GAIN2 fields
    global_configuration[2] = ADS131M08Q1_CONFIGTEMPLATE_GAIN
                                | (device->config.ch_configs[0].gain << ADS131M08Q1_GAINCONFIG_LSHIFT_CH0_CH4_GAIN)
                                | (device->config.ch_configs[1].gain << ADS131M08Q1_GAINCONFIG_LSHIFT_CH1_CH5_GAIN)
                                | (device->config.ch_configs[2].gain << ADS131M08Q1_GAINCONFIG_LSHIFT_CH2_CH6_GAIN)
                                | (device->config.ch_configs[3].gain << ADS131M08Q1_GAINCONFIG_LSHIFT_CH3_CH7_GAIN);
    global_configuration[3] = ADS131M08Q1_CONFIGTEMPLATE_GAIN
                                | (device->config.ch_configs[4].gain << ADS131M08Q1_GAINCONFIG_LSHIFT_CH0_CH4_GAIN)
                                | (device->config.ch_configs[5].gain << ADS131M08Q1_GAINCONFIG_LSHIFT_CH1_CH5_GAIN)
                                | (device->config.ch_configs[6].gain << ADS131M08Q1_GAINCONFIG_LSHIFT_CH2_CH6_GAIN)
                                | (device->config.ch_configs[7].gain << ADS131M08Q1_GAINCONFIG_LSHIFT_CH3_CH7_GAIN);
    
    if(ADS131M08Q1_WriteRegs(device, ADS131M08Q1_REG_MODE, global_configuration, 4) != ADS131M08Q1_🙂)
    {
        return ADS131M08Q1_😢;
    }
    
    uint16_t global_configuration_readback[4] = {0};
    if(ADS131M08Q1_ReadRegs(device, ADS131M08Q1_REG_MODE, global_configuration_readback, 4) != ADS131M08Q1_🙂)
    {
        return ADS131M08Q1_😢;
    }

    for(uint8_t i = 0; i < 4; i++)
    {
        if(global_configuration_readback[i] != global_configuration[i])
        {
            return ADS131M08Q1_😢;
        }
    }

    // WRITE CHANNEL CONFIGURATION
    // --------------------------------
    uint8_t ch_config_len = ADS131M08Q1_NUM_CHANNELS*5;
    uint16_t channel_configuration[ch_config_len];
    memset(channel_configuration, 0, sizeof(channel_configuration));

    for(uint8_t ch = 0; ch < ADS131M08Q1_NUM_CHANNELS; ch++)
    {
        // CHx_CFG (phase)
        channel_configuration[ch*5+0] = ADS131M08Q1_CONFIGTEMPLATE_CHx_CFG
                                        | (device->config.ch_configs[ch].phase << ADS131M08Q1_CHCONFIG_CFG_LSHIFT_PHASE);
        // CHx_OCAL (offset calibration)
        channel_configuration[ch*5+1] = ((device->config.ch_configs[ch].offset_cal >> ADS131M08Q1_CHCONFIG_CAL_MSB_RSHIFT) & ADS131M08Q1_CHCONFIG_CAL_MSB_KEEPMASK);
        channel_configuration[ch*5+2] = (device->config.ch_configs[ch].offset_cal << ADS131M08Q1_CHCONFIG_CAL_LSB_LSHIFT);
        // CHx_GCAL (gain calibration)
        channel_configuration[ch*5+3] = ((device->config.ch_configs[ch].gain_cal >> ADS131M08Q1_CHCONFIG_CAL_MSB_RSHIFT) & ADS131M08Q1_CHCONFIG_CAL_MSB_KEEPMASK);
        channel_configuration[ch*5+4] = (device->config.ch_configs[ch].gain_cal << ADS131M08Q1_CHCONFIG_CAL_LSB_LSHIFT);
    }

    if(ADS131M08Q1_WriteRegs(device, ADS131M08Q1_REG_CHx_CFG(0), channel_configuration, ch_config_len) != ADS131M08Q1_🙂)
    {
        return ADS131M08Q1_😢;
    }
    
    uint16_t channel_configuration_readback[ch_config_len];
    memset(channel_configuration_readback, 0, sizeof(channel_configuration_readback));
    if(ADS131M08Q1_ReadRegs(device, ADS131M08Q1_REG_CHx_CFG(0), channel_configuration_readback, ch_config_len) != ADS131M08Q1_🙂)
    {
        return ADS131M08Q1_😢;
    }

    for(uint8_t i = 0; i < ch_config_len; i++)
    {
        if(channel_configuration_readback[i] != channel_configuration[i])
        {
            return ADS131M08Q1_😢;
        }
    }

    // precompute conversion factor
    for(uint8_t ch = 0; ch < ADS131M08Q1_NUM_CHANNELS; ch++)
    {
        device->conversion_factor[ch] = (device->config.fsr/ADS131M08Q1_NUM_STEPS) / (1 << device->config.ch_configs[ch].gain);
    }

    return ADS131M08Q1_🙂;
}

ADS131M08Q1_Status_t ADS131M08Q1_ReadConversionResults(ADS131M08Q1_HandleTypeDef* device, float* results)
{
    uint8_t frame_response[ADS131M08Q1_FRAME_LEN_8] = {0};

    if(xSemaphoreTake(device->spi_mutex, ADS131M08Q1_SPI_MUTEX_DELAY_TICKS) != pdTRUE)
    {
        return ADS131M08Q1_🕷️;
    }
    
    if(ADS131M08Q1_Frame_Receive(device, frame_response) != ADS131M08Q1_🙂)
    {
        // release SPI mutex
        xSemaphoreGive(device->spi_mutex);

        return ADS131M08Q1_😢;
    }
    
    // release SPI mutex
    xSemaphoreGive(device->spi_mutex);

    // compute results
    for(uint8_t ch = 0; ch < ADS131M08Q1_NUM_CHANNELS; ch++)
    {
        uint32_t conversion = (__builtin_bswap32(*((uint32_t*) (frame_response+3*(ch+1)))) >> 8);
        results[ch] = (float) conversion * (device->config.fsr/ADS131M08Q1_NUM_STEPS) / (1 << device->config.ch_configs[ch].gain);

        if(conversion & 0x800000)
        {
            results[ch] -= 2*device->config.fsr;
        }
    }

    return ADS131M08Q1_🙂;
}

ADS131M08Q1_Status_t ADS131M08Q1_ReadStatus(ADS131M08Q1_HandleTypeDef* device, uint16_t* status)
{
    uint8_t null_frame[ADS131M08Q1_FRAME_LEN_8] = {0};

    uint8_t frame_response[ADS131M08Q1_FRAME_LEN_8] = {0};

    if(xSemaphoreTake(device->spi_mutex, ADS131M08Q1_SPI_MUTEX_DELAY_TICKS) != pdTRUE)
    {
        return ADS131M08Q1_🕷️;
    }

    if(ADS131M08Q1_Frame_Transmit(device, null_frame) != ADS131M08Q1_🙂)
    {
        // release SPI mutex
        xSemaphoreGive(device->spi_mutex);

        return ADS131M08Q1_😢;
    }

    if(ADS131M08Q1_Frame_Receive(device, frame_response) != ADS131M08Q1_🙂)
    {
        // release SPI mutex
        xSemaphoreGive(device->spi_mutex);

        return ADS131M08Q1_😢;
    }
    
    // release SPI mutex
    xSemaphoreGive(device->spi_mutex);

    *status = __builtin_bswap16(*((uint16_t*) frame_response));

    return ADS131M08Q1_🙂;
}

ADS131M08Q1_Status_t ADS131M08Q1_ReadConversionStatus(ADS131M08Q1_HandleTypeDef* device, uint8_t* status)
{
    uint16_t temp_status = 0;

    if(ADS131M08Q1_ReadStatus(device, &temp_status) != ADS131M08Q1_🙂)
    {
        return ADS131M08Q1_😢;
    }

    *status = temp_status & 0x00FF;

    return ADS131M08Q1_🙂;
}

ADS131M08Q1_Status_t ADS131M08Q1_Reset(ADS131M08Q1_HandleTypeDef* device)
{
    return ADS131M08Q1_SendCommand(device, ADS131M08Q1_OPCODE_RESET_MSB, ADS131M08Q1_OPCODE_RESET_LSB, ADS131M08Q1_RESPONSE_RESET_MSB, ADS131M08Q1_RESPONSE_RESET_LSB, 1);
}

ADS131M08Q1_Status_t ADS131M08Q1_Standby(ADS131M08Q1_HandleTypeDef* device)
{
    return ADS131M08Q1_SendCommand(device, ADS131M08Q1_OPCODE_STANDBY_MSB, ADS131M08Q1_OPCODE_STANDBY_LSB, ADS131M08Q1_RESPONSE_STANDBY_MSB, ADS131M08Q1_RESPONSE_STANDBY_LSB, 0);
}

ADS131M08Q1_Status_t ADS131M08Q1_Wakeup(ADS131M08Q1_HandleTypeDef* device)
{
    return ADS131M08Q1_SendCommand(device, ADS131M08Q1_OPCODE_WAKEUP_MSB, ADS131M08Q1_OPCODE_WAKEUP_LSB, ADS131M08Q1_RESPONSE_WAKEUP_MSB, ADS131M08Q1_RESPONSE_WAKEUP_LSB, 0);
}

ADS131M08Q1_Status_t ADS131M08Q1_Lock(ADS131M08Q1_HandleTypeDef* device)
{
    return ADS131M08Q1_SendCommand(device, ADS131M08Q1_OPCODE_LOCK_MSB, ADS131M08Q1_OPCODE_LOCK_LSB, ADS131M08Q1_RESPONSE_LOCK_MSB, ADS131M08Q1_RESPONSE_LOCK_LSB, 0);
}

ADS131M08Q1_Status_t ADS131M08Q1_Unlock(ADS131M08Q1_HandleTypeDef* device)
{
    return ADS131M08Q1_SendCommand(device, ADS131M08Q1_OPCODE_UNLOCK_MSB, ADS131M08Q1_OPCODE_UNLOCK_LSB, ADS131M08Q1_RESPONSE_UNLOCK_MSB, ADS131M08Q1_RESPONSE_UNLOCK_LSB, 0);
}

inline int32_t ADS131M08Q1_CalcOffsetCalRegValue(ADS131M08Q1_HandleTypeDef* device, float offset)
{
    if(offset > device->config.fsr){offset = device->config.fsr;}
    if(offset < -1.0*device->config.fsr){offset = -1.0*device->config.fsr;}

    return roundf(-1*offset / (device->config.fsr / ADS131M08Q1_NUM_STEPS));
}

inline uint32_t ADS131M08Q1_CalcGainCalRegValue(ADS131M08Q1_HandleTypeDef* device, float gain)
{
    if(gain < ADS131M08Q1_GAIN_CAL_MIN){gain = ADS131M08Q1_GAIN_CAL_MIN;}
    if(gain > ADS131M08Q1_GAIN_CAL_MAX){gain = ADS131M08Q1_GAIN_CAL_MAX;}

    return roundf(gain / ADS131M08Q1_GAIN_CAL_STEP);
}
