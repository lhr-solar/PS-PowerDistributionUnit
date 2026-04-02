// ADS131M08-Q1.h
// ----------------------------------------------------------------------------
// Driver for ADS131M08-Q1 24-bit simultaneously-sampling sigma-delta ADC
// Datasheet: datasheets/ADS131M08-Q1.pdf (from repo root)

# pragma once

#include "stm32xx_hal.h"

#define ADS131M08Q1_SPI_MUTEX_DELAY portMAX_DELAY
#define ADS131M08Q1_SPI_TRANSMISSION_DELAY pdMS_TO_TICKS(100)

#define ADS131M08Q1_NUM_CHANNELS 8
#define ADS131M08Q1_NUM_BITS 24
#define ADS131M08Q1_NUM_STEPS 8388608   // 2^23 (24 bits - 1 sign bit)
#define ADS131M08Q1_CH_VALID_CHECK(ch) (ch < ADS131M08Q1_NUM_CHANNELS && ch >= 0)
#define ADS131M08Q1_CH_INVALID_CHECK(ch) (ch >= ADS131M08Q1_NUM_CHANNELS || ch < 0)

#define ADS131M08Q1_WORD_SIZE 24
#define ADS131M08Q1_FRAME_LEN_24 10
#define ADS131M08Q1_FRAME_LEN_8 30 // (ADS131M08Q1_WORD_SIZE*ADS131M08Q1_FRAME_LEN_24)/8
#define ADS131M08Q1_WORD_LEN_8 3

#define ADS131M08Q1_OPCODE_NULL_MSB 0x00
#define ADS131M08Q1_OPCODE_NULL_LSB 0x00
#define ADS131M08Q1_OPCODE_RESET_MSB 0x00
#define ADS131M08Q1_OPCODE_RESET_LSB 0x11
#define ADS131M08Q1_OPCODE_STANDBY_MSB 0x00
#define ADS131M08Q1_OPCODE_STANDBY_LSB 0x22
#define ADS131M08Q1_OPCODE_WAKEUP_MSB 0x00
#define ADS131M08Q1_OPCODE_WAKEUP_LSB 0x33
#define ADS131M08Q1_OPCODE_LOCK_MSB 0x05
#define ADS131M08Q1_OPCODE_LOCK_LSB 0x55
#define ADS131M08Q1_OPCODE_UNLOCK_MSB 0x06
#define ADS131M08Q1_OPCODE_UNLOCK_LSB 0x55
#define ADS131M08Q1_OPCODE_RREG_MSB 0xA0
#define ADS131M08Q1_OPCODE_WREG_MSB 0x60

#define ADS131M08Q1_RESPONSE_RESET_MSB 0xFF
#define ADS131M08Q1_RESPONSE_RESET_LSB 0x28
#define ADS131M08Q1_RESPONSE_STANDBY_MSB 0x00
#define ADS131M08Q1_RESPONSE_STANDBY_LSB 0x22
#define ADS131M08Q1_RESPONSE_WAKEUP_MSB 0x00
#define ADS131M08Q1_RESPONSE_WAKEUP_LSB 0x33
#define ADS131M08Q1_RESPONSE_LOCK_MSB 0x05
#define ADS131M08Q1_RESPONSE_LOCK_LSB 0x55
#define ADS131M08Q1_RESPONSE_UNLOCK_MSB 0x06
#define ADS131M08Q1_RESPONSE_UNLOCK_LSB 0x55
#define ADS131M08Q1_RESPONSE_RREG_MSB 0xE0
#define ADS131M08Q1_RESPONSE_WREG_MSB 0x40

typedef enum {
	ADS131M08Q1_😢,				// ADS131M08Q1 sad
	ADS131M08Q1_🙂,				// ADS131M08Q1 happy
} ADS131M08Q1_Status;

// REGISTERS -----------------------------------------------------------

#define ADS131M08Q1_REG_ID 0x00
#define ADS131M08Q1_REG_STATUS 0x01
#define ADS131M08Q1_REG_MODE 0x02
#define ADS131M08Q1_REG_CLOCK 0x03
#define ADS131M08Q1_REG_GAIN1 0x04
#define ADS131M08Q1_REG_GAIN2 0x05
#define ADS131M08Q1_REG_CFG 0x06
#define ADS131M08Q1_REG_THRSHLD_MSB 0x07
#define ADS131M08Q1_REG_THRSHLD_LSB 0x08
#define ADS131M08Q1_REG_REGMAP_CRC 0x3E
#define ADS131M08Q1_REG_CHx_CFG(x) (ADS131M08Q1_CH_VALID_CHECK(x) ? 0x09 + 5*x : 0x09)
#define ADS131M08Q1_REG_CHx_OCAL_MSB(x) (ADS131M08Q1_CH_VALID_CHECK(x) ? 0x0A + 5*x : 0x0A)
#define ADS131M08Q1_REG_CHx_OCAL_LSB(x) (ADS131M08Q1_CH_VALID_CHECK(x) ? 0x0B + 5*x : 0x0B)
#define ADS131M08Q1_REG_CHx_GCAL_MSB(x) (ADS131M08Q1_CH_VALID_CHECK(x) ? 0x0C + 5*x : 0x0C)
#define ADS131M08Q1_REG_CHx_GCAL_LSB(x) (ADS131M08Q1_CH_VALID_CHECK(x) ? 0x0D + 5*x : 0x0D)

#define ADS131M08Q1_REG_INVALID 0x3F
#define ADS131M08Q1_REG_INVALID_CHECK(reg_addr, num_regs) (reg_addr+num_regs-1) >= ADS131M08Q1_REG_INVALID || reg_addr < 0

// CONFIGTEMPLATEs are register default values, with fields to be set by Init function zeroed out
#define ADS131M08Q1_CONFIGTEMPLATE_MODE 0x0110      // MODE also has the "reset ocurred" bit cleared
#define ADS131M08Q1_CONFIGTEMPLATE_CLOCK 0x000C
#define ADS131M08Q1_CONFIGTEMPLATE_GAIN 0x0000

#define ADS131M08Q1_MODECONFIG_LSHIFT_DRDY_SOURCE 2
#define ADS131M08Q1_MODECONFIG_LSHIFT_DRDY_IDLEPINSTATE 1
#define ADS131M08Q1_MODECONFIG_LSHIFT_DRDY_FORMAT 0
#define ADS131M08Q1_CLOCKCONFIG_LSHIFT_CHx_ENABLE(x) (8+x)
#define ADS131M08Q1_CLOCKCONFIG_LSHIFT_REFERENCE_SOURCE 6
#define ADS131M08Q1_CLOCKCONFIG_LSHIFT_POWERMODE 0
#define ADS131M08Q1_GAINCONFIG_LSHIFT_CH0_CH4_GAIN 0
#define ADS131M08Q1_GAINCONFIG_LSHIFT_CH1_CH5_GAIN 4
#define ADS131M08Q1_GAINCONFIG_LSHIFT_CH2_CH6_GAIN 8
#define ADS131M08Q1_GAINCONFIG_LSHIFT_CH3_CH7_GAIN 12

// CONFIGURATION --------------------------------------------------------------

typedef enum {
    ADS131M08Q1_CH_DISABLE,
    ADS131M08Q1_CH_ENABLE,
} ADS131M08Q1_Ch_Enable;
#define ADS131M08Q1_CH_ENABLE_DEFAULT 0b1
// TODO: change default defines to use enum

typedef enum {
    ADS131M08Q1_CH_GAIN_1,
    ADS131M08Q1_CH_GAIN_2,
    ADS131M08Q1_CH_GAIN_4,
    ADS131M08Q1_CH_GAIN_8,
    ADS131M08Q1_CH_GAIN_16,
    ADS131M08Q1_CH_GAIN_32,
    ADS131M08Q1_CH_GAIN_64,
    ADS131M08Q1_CH_GAIN_128,
} ADS131M08Q1_Ch_Gain;
#define ADS131M08Q1_CH_GAIN_DEFAULT 0b000

#define ADS131M08Q1_CH_PHASE_DELAY_DEFAULT 0
#define ADS131M08Q1_CH_OFFSET_CAL_DEFAULT 0
#define ADS131M08Q1_CH_GAIN_CAL_DEFAULT 0

// ADC Channel Configuration Struct
typedef struct {
    ADS131M08Q1_Ch_Enable enable;
	ADS131M08Q1_Ch_Gain gain;
    int16_t phase_delay;            // in modulator clock cycles
    float offset_cal;               // range: -FSR to FSR (converted to same format as ADC data)
    float gain_cal;                 // range: 0 - 2
    // DC block filter and input mux settings not implemented.

} ADS131M08Q1_ChannelConfig;

typedef enum {
    ADS131M08Q1_MOST_LAGGING_ENABLED_CHANNEL,
    ADS131M08Q1_LOGIC_OR_ALL_CHANNELS,
    ADS131M08Q1_MOST_LEADING_ENABLED_CHANNEL,
    ADS131M08Q1_MOST_LEADING_ENABLED_CHANNEL_2_ELECTRIC_BOOGALOO,
} ADS131M08Q1_Config_DRDY_Source;
#define ADS131M08Q1_CONFIG_DRDY_SOURCE_DEFAULT 0b00

typedef enum {
    ADS131M08Q1_DRDY_LOGIC_HIGH,
    ADS131M08Q1_DRDY_OPEN_DRAIN,
} ADS131M08Q1_Config_DRDY_IdlePinState;
#define ADS131M08Q1_CONFIG_DRDY_IDLEPINSTATE_DEFAULT 0b0

typedef enum {
    ADS131M08Q1_DRDY_LOGIC_LOW,
    ADS131M08Q1_DRDY_LOW_PULSE_FIXED_DURATION,
} ADS131M08Q1_Config_DRDY_Format;
#define ADS131M08Q1_CONFIG_DRDY_FORMAT_DEFAULT 0b0

typedef enum {
    ADS131M08Q1_REFERENCE_SOURCE_INTERNAL,
    ADS131M08Q1_REFERENCE_SOURCE_EXTERNAL,
} ADS131M08Q1_Config_ReferenceSource;
#define ADS131M08Q1_CONFIG_REFERENCE_SOURCE_DEFAULT 0b0
#define ADS131M08Q1_CONFIG_FSR_DEFAULT 1.2

typedef enum {
    ADS131M08Q1_VERY_LOW_POWER,
    ADS131M08Q1_LOW_POWER,
    ADS131M08Q1_HIGH_RESOLUTION,
    ADS131M08Q1_HIGH_RESOLUTION_2_THE_SEQUEL,
} ADS131M08Q1_Config_PowerMode;
#define ADS131M08Q1_CONFIG_POWERMODE_DEFAULT 0b10

// FOR FUTURE IMPLEMENTATION
// typedef enum {
//     ADS131M08Q1_MODE_CONTINUOUS_CONVERSION,
//     ADS131M08Q1_MODE_GLOBAL_CHOP,
//     ADS131M08Q1_MODE_STANDBY,
//     ADS131M08Q1_MODE_CURRENT_DETECT,
// } ADS131M08Q1_Config_OperatingMode;

// FOR FUTURE IMPLEMENTATION
// typedef enum {
//     ADS131M08Q1_GLOBALCHOP_DELAY_2,
//     ADS131M08Q1_GLOBALCHOP_DELAY_4,
//     ADS131M08Q1_GLOBALCHOP_DELAY_8,
//     ADS131M08Q1_GLOBALCHOP_DELAY_16,
//     ADS131M08Q1_GLOBALCHOP_DELAY_32,
//     ADS131M08Q1_GLOBALCHOP_DELAY_64,
//     ADS131M08Q1_GLOBALCHOP_DELAY_128,
//     ADS131M08Q1_GLOBALCHOP_DELAY_256,
//     ADS131M08Q1_GLOBALCHOP_DELAY_512,
//     ADS131M08Q1_GLOBALCHOP_DELAY_1024,
//     ADS131M08Q1_GLOBALCHOP_DELAY_2048,
//     ADS131M08Q1_GLOBALCHOP_DELAY_4096,
//     ADS131M08Q1_GLOBALCHOP_DELAY_8192,
//     ADS131M08Q1_GLOBALCHOP_DELAY_16384,
//     ADS131M08Q1_GLOBALCHOP_DELAY_32768,
//     ADS131M08Q1_GLOBALCHOP_DELAY_65536,
// }
// #define ADS131M08Q1_GLOBALCHOP_DELAY_DEFAULT 0x03

// ADC Device Init Config Struct
typedef struct {
	ADS131M08Q1_ChannelConfig ch_configs[8];
    ADS131M08Q1_Config_DRDY_Source drdy_source;
    ADS131M08Q1_Config_DRDY_IdlePinState drdy_idlepinstate;
    ADS131M08Q1_Config_DRDY_Format drdy_format;
    ADS131M08Q1_Config_ReferenceSource reference_source;
    float fsr;
    ADS131M08Q1_Config_PowerMode powermode;

    // FOR FUTURE IMPLEMENTATION
    // ADS131M08Q1_Config_OperatingMode mode;
    // ADS131M08Q1_GlobalChop_Delay gc_delay;
    // ADS131M08Q1_CurrentDetect_Delay cd_delay;
    // ADS131M08Q1_CurrentDetect_Threshold cd_threshold;
    // ADS131M08Q1_CurrentDetect_Chs cd_channels;
    // uint32_t current_detect_threshold;
} ADS131M08Q1_DeviceConfig;

// DEVICE ---------------------------------------------------------------------

typedef struct {
	SPI_HandleTypeDef* spi;     // STM32 HAL SPI handle
	GPIO_TypeDef* cs_port;
	uint16_t cs_pin;

    SemaphoreHandle_t spi_mutex;        // Mutex to prevent simultaenous SPI access
    SemaphoreHandle_t spi_done_sem;     // Semaphore to signal SPI transmission complete

    ADS131M08Q1_DeviceConfig config;
} ADS131M08Q1_HandleTypeDef;


// DEVICE FUNCTIONS -----------------------------------------------------------
// meant to be used interally

ADS131M08Q1_Status ADS131M08Q1_Frame(ADS131M08Q1_HandleTypeDef* device, uint8_t* out_data, uint8_t* in_data);

ADS131M08Q1_Status ADS131M08Q1_FrameVar(ADS131M08Q1_HandleTypeDef* device, uint8_t* out_data, uint8_t* in_data, uint8_t num_words);

ADS131M08Q1_Status ADS131M08Q1_SendCommand(ADS131M08Q1_HandleTypeDef* device, uint8_t cmd_MSB, uint8_t cmd_LSB, uint8_t response_MSB, uint8_t response_LSB);
/**
 * @brief	blah
 * @param	blah blah
 * @returns blah
 */
ADS131M08Q1_Status ADS131M08Q1_ReadRegs(ADS131M08Q1_HandleTypeDef* device, uint8_t reg_addr, uint16_t* data, uint8_t num_regs);

ADS131M08Q1_Status ADS131M08Q1_WriteRegs(ADS131M08Q1_HandleTypeDef* device, uint8_t reg_addr, uint16_t* data, uint8_t num_regs);

// USER FUNCTIONS -----------------------------------------------------------

ADS131M08Q1_Status ADS131M08Q1_Init(ADS131M08Q1_HandleTypeDef* device, SPI_HandleTypeDef* spi, GPIO_TypeDef* cs_port, uint16_t cs_pin);

ADS131M08Q1_Status ADS131M08Q1_ReadConversionResults(ADS131M08Q1_HandleTypeDef* device, float* results);

ADS131M08Q1_Status ADS131M08Q1_ReadStatus(ADS131M08Q1_HandleTypeDef* device, uint16_t* status);
ADS131M08Q1_Status ADS131M08Q1_ReadConversionStatus(ADS131M08Q1_HandleTypeDef* device, uint8_t* status);

ADS131M08Q1_Status ADS131M08Q1_Reset(ADS131M08Q1_HandleTypeDef* device);
ADS131M08Q1_Status ADS131M08Q1_Standby(ADS131M08Q1_HandleTypeDef* device);
ADS131M08Q1_Status ADS131M08Q1_Wakeup(ADS131M08Q1_HandleTypeDef* device);
ADS131M08Q1_Status ADS131M08Q1_Lock(ADS131M08Q1_HandleTypeDef* device);
ADS131M08Q1_Status ADS131M08Q1_Unlock(ADS131M08Q1_HandleTypeDef* device);

// TODO
// ADS131M08Q1_Status ADDS131M08Q1_SetPowerMode(ADS131M08Q1_Config_PowerMode powermode);
// self calibration function
// global chop? current detect?
// global chop might not be usable due to lower negative voltage limits
// resetHappened
// SPI locked? Standby? 
