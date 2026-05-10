// PDU_Mk1_Current_Sensing.c

#include "PDU_Mk1_Current_Sensing.h"

ADS131M08Q1_HandleTypeDef adc_sns1;
ADS131M08Q1_HandleTypeDef adc_sns0;

float currents[PDU_MK1_NUM_CHANNELS] = {0};
float current_offset[PDU_MK1_NUM_CHANNELS] = {0};

bool PDU_Mk1_Current_Sensing_Init()
{
    if(PDU_Mk1_Init_ADC_SNS0() != ADS131M08Q1_🙂)
    {
        return false;
    }

    if(PDU_Mk1_Init_ADC_SNS1() != ADS131M08Q1_🙂)
    {
        return false;
    }

    return true;
}

ADS131M08Q1_Status_t PDU_Mk1_Init_ADC_SNS0()
{
    adc_sns0.spi = &hspi3;
    adc_sns0.cs_port = ADC_SNS0_CS_PORT;
    adc_sns0.cs_pin = ADC_SNS0_CS_PIN;

    adc_sns0.spi_mutex = spi3_mutex;
    adc_sns0.spi_done_sem = spi3_done_sem;
    
    for(uint8_t ch = 0; ch < ADS131M08Q1_NUM_CHANNELS; ch++)
    {
        adc_sns0.config.ch_configs[ch].enable = ADS131M08Q1_CH_ENABLE;
        adc_sns0.config.ch_configs[ch].gain = ADS131M08Q1_CH_GAIN_1;
        adc_sns0.config.ch_configs[ch].phase = ADS131M08Q1_CH_PHASE_DEFAULT;
        adc_sns0.config.ch_configs[ch].offset_cal = ADS131M08Q1_CH_OFFSET_CAL_DEFAULT;
        adc_sns0.config.ch_configs[ch].gain_cal = ADS131M08Q1_CH_GAIN_CAL_DEFAULT;
    }
    adc_sns0.config.drdy_format = ADS131M08Q1_CONFIG_DRDY_FORMAT_DEFAULT;
    adc_sns0.config.drdy_idlepinstate = ADS131M08Q1_CONFIG_DRDY_IDLEPINSTATE_DEFAULT;
    adc_sns0.config.drdy_source = ADS131M08Q1_CONFIG_DRDY_SOURCE_DEFAULT;
    adc_sns0.config.reference_source = ADS131M08Q1_CONFIG_REFERENCE_SOURCE_DEFAULT;
    adc_sns0.config.fsr = ADS131M08Q1_INTERNAL_REFERENCE_V;
    adc_sns0.config.powermode = ADS131M08Q1_CONFIG_POWERMODE_DEFAULT;

    return ADS131M08Q1_Init(&adc_sns0);
}

ADS131M08Q1_Status_t PDU_Mk1_Init_ADC_SNS1()
{
    adc_sns1.spi = &hspi2;
    adc_sns1.cs_port = ADC_SNS1_CS_PORT;
    adc_sns1.cs_pin = ADC_SNS1_CS_PIN;

    adc_sns1.spi_mutex = spi2_mutex;
    adc_sns1.spi_done_sem = spi2_done_sem;
    
    for(uint8_t ch = 0; ch < ADS131M08Q1_NUM_CHANNELS; ch++)
    {
        adc_sns1.config.ch_configs[ch].enable = ADS131M08Q1_CH_ENABLE;
        adc_sns1.config.ch_configs[ch].gain = ADS131M08Q1_CH_GAIN_1;
        adc_sns1.config.ch_configs[ch].phase = ADS131M08Q1_CH_PHASE_DEFAULT;
        adc_sns1.config.ch_configs[ch].offset_cal = ADS131M08Q1_CH_OFFSET_CAL_DEFAULT;
        adc_sns1.config.ch_configs[ch].gain_cal = ADS131M08Q1_CH_GAIN_CAL_DEFAULT;
    }
    adc_sns1.config.drdy_format = ADS131M08Q1_CONFIG_DRDY_FORMAT_DEFAULT;
    adc_sns1.config.drdy_idlepinstate = ADS131M08Q1_CONFIG_DRDY_IDLEPINSTATE_DEFAULT;
    adc_sns1.config.drdy_source = ADS131M08Q1_CONFIG_DRDY_SOURCE_DEFAULT;
    adc_sns1.config.reference_source = ADS131M08Q1_CONFIG_REFERENCE_SOURCE_DEFAULT;
    adc_sns1.config.fsr = ADS131M08Q1_INTERNAL_REFERENCE_V;
    adc_sns1.config.powermode = ADS131M08Q1_CONFIG_POWERMODE_DEFAULT;

    // channel 0 reads about 0.24 V high for some reason (rev A only)
    // adc_sns1.config.ch_configs[0].offset_cal = ADS131M08Q1_CalcOffsetCalRegValue(&adc_sns1, -0.24);

    return ADS131M08Q1_Init(&adc_sns1);
}
