// PDU_Mk1_HSSControl.c

#include "PDU_Mk1_HSSControl.h"

ShiftRegister_SPI_HandleTypeDef hsscontrol_sr;

uint32_t HSS_state = HSSCONTROL_MASK_LATCH;	// initialize with all HSS off, all HSS fault latching on
HSSControl_FaultState_t HSS_fault_state[PDU_MK1_NUM_CHANNELS] = {HSSCONTROL_NOFAULT};
// int8_t HSS_fault_retry_counter[PDU_MK1_NUM_CHANNELS] = {-1};

const uint32_t HSSCONTROL_EN_MASKS[PDU_MK1_NUM_CHANNELS] = {
    HSSCONTROL_MASK_0EN,
    HSSCONTROL_MASK_1EN,
    HSSCONTROL_MASK_2EN,
    HSSCONTROL_MASK_3EN,
    HSSCONTROL_MASK_4EN,
    HSSCONTROL_MASK_5EN,
    HSSCONTROL_MASK_6EN,
    HSSCONTROL_MASK_7EN,
    HSSCONTROL_MASK_8EN,
    HSSCONTROL_MASK_9EN,
    HSSCONTROL_MASK_10EN,
    HSSCONTROL_MASK_11EN,
    HSSCONTROL_MASK_12EN,
    HSSCONTROL_MASK_13EN,
    HSSCONTROL_MASK_14EN,
    HSSCONTROL_MASK_15EN
};

const uint32_t HSSCONTROL_LATCH_MASKS[PDU_MK1_NUM_CHANNELS] = {
    HSSCONTROL_MASK_0LA,
    HSSCONTROL_MASK_1LA,
    HSSCONTROL_MASK_2LA,
    HSSCONTROL_MASK_3LA,
    HSSCONTROL_MASK_4LA,
    HSSCONTROL_MASK_5LA,
    HSSCONTROL_MASK_6LA,
    HSSCONTROL_MASK_7LA,
    HSSCONTROL_MASK_8LA,
    HSSCONTROL_MASK_9LA,
    HSSCONTROL_MASK_10LA,
    HSSCONTROL_MASK_11LA,
    HSSCONTROL_MASK_12LA,
    HSSCONTROL_MASK_13LA,
    HSSCONTROL_MASK_14LA,
    HSSCONTROL_MASK_15LA
};

HSSControl_Status_t PDU_Mk1_HSSControl_Init()
{
	hsscontrol_sr.spi = &hspi2;
    hsscontrol_sr.en_port = HSS_SR_EN_PORT;
    hsscontrol_sr.en_pin = HSS_SR_EN_PIN;

    hsscontrol_sr.out_clk_port = HSS_SR_OUT_CLK_PORT;
    hsscontrol_sr.out_clk_pin = HSS_SR_OUT_CLK_PIN;

    hsscontrol_sr.all_off_port = HSS_SR_ALL_OFF_PORT;
    hsscontrol_sr.all_off_pin = HSS_SR_ALL_OFF_PIN;

    hsscontrol_sr.spi_mutex = spi2_mutex;
    hsscontrol_sr.spi_done_sem = spi2_done_sem;

    hsscontrol_sr.data = (uint8_t*) &HSS_state;
    hsscontrol_sr.num_bytes = HSSCONTROL_STATE_NUM_BYTES;

    return (SR_SPI_Init(&hsscontrol_sr) == SR_SPI_🙂);
}

inline HSSControl_Status_t PDU_Mk1_HSSControl_UpdateHSSShiftRegs()
{
	return (SR_SPI_SetRegs(&hsscontrol_sr) == SR_SPI_🙂);	
}

HSSControl_Status_t PDU_Mk1_HSSControl_WriteHSSEnField(HSSControl_Channel_t ch, HSSControl_EnState_t action)
{
	if(ch >= PDU_OUTPUT_INVALID)
	{
		return HSSCONTROL_😢;
	}

	switch(action)
	{	
		case HSSCONTROL_EN_NOCHANGE:
			break;
		case HSSCONTROL_EN_OFF:
			HSS_state &= ~(HSSCONTROL_EN_MASKS[ch]);
			break;
		case HSSCONTROL_EN_ON:
			HSS_state |= HSSCONTROL_EN_MASKS[ch];
			break;
		case HSSCONTROL_EN_TOGGLE:
			HSS_state ^= HSSCONTROL_EN_MASKS[ch];
			break;
		default:
			// invalid action
			return HSSCONTROL_😢;
	}

	return HSSCONTROL_🙂;
}

HSSControl_Status_t PDU_Mk1_HSSControl_WriteOutputEN_Ch(HSSControl_Channel_t ch, HSSControl_EnState_t action)
{
	if(PDU_Mk1_HSSControl_WriteHSSEnField(ch, action) != HSSCONTROL_🙂)
	{
		return HSSCONTROL_😢;
	}
	
	return PDU_Mk1_HSSControl_UpdateHSSShiftRegs();
}

HSSControl_Status_t PDU_Mk1_HSSControl_WriteOutputEN_All(HSSControl_EnState_t actions[])
{
	uint32_t original_HSS_state = HSS_state;

	for(uint8_t ch = 0; ch < PDU_MK1_NUM_CHANNELS; ch++)
	{
		if(PDU_Mk1_HSSControl_WriteHSSEnField(ch, actions[ch]) != HSSCONTROL_🙂)
		{
			HSS_state = original_HSS_state;

			return HSSCONTROL_😢;
		}
	}
	
	return PDU_Mk1_HSSControl_UpdateHSSShiftRegs();
}

HSSControl_Status_t PDU_Mk1_HSSControl_AllOn()
{
	HSS_state |= HSSCONTROL_MASK_EN;

	return PDU_Mk1_HSSControl_UpdateHSSShiftRegs();
}

HSSControl_Status_t PDU_Mk1_HSSControl_AllOff()
{
	HSS_state &= (~HSSCONTROL_MASK_EN);
	
	return PDU_Mk1_HSSControl_UpdateHSSShiftRegs();
}

HSSControl_Status_t PDU_Mk1_HSSControl_CritOnly()
{
	HSS_state &= (~HSSCONTROL_MASK_EN);
	HSS_state |= HSSCONTROL_CRITICAL_MASK;
	
	return PDU_Mk1_HSSControl_UpdateHSSShiftRegs();
}

HSSControl_Status_t PDU_Mk1_HSSControl_WriteHSSLatchField(HSSControl_Channel_t ch, HSSControl_LatchState_t latch)
{
	if(ch >= PDU_OUTPUT_INVALID)
	{
		return HSSCONTROL_😢;
	}

	switch(latch)
	{	
		case HSSCONTROL_LATCH_NOCHANGE:
			break;
		case HSSCONTROL_UNLATCHFAULT_AUTORETRY:
			HSS_state &= ~(HSSCONTROL_LATCH_MASKS[ch]);
			break;
		case HSSCONTROL_LATCHFAULT_STAYOFF:
			HSS_state |= HSSCONTROL_LATCH_MASKS[ch];
			break;
		default:
			// invalid action
			return HSSCONTROL_😢;
	}

	return HSSCONTROL_🙂;
}

HSSControl_Status_t PDU_Mk1_HSSControl_WriteLatch_Ch(HSSControl_Channel_t ch, HSSControl_LatchState_t latch)
{
	if(PDU_Mk1_HSSControl_WriteHSSLatchField(ch, latch) != HSSCONTROL_🙂)
	{
		return HSSCONTROL_😢;
	}
	
	return PDU_Mk1_HSSControl_UpdateHSSShiftRegs();
}

HSSControl_Status_t PDU_Mk1_HSSControl_WriteLatch_All(HSSControl_LatchState_t latch[])
{
	uint32_t original_HSS_state = HSS_state;

	for(uint8_t ch = 0; ch < PDU_MK1_NUM_CHANNELS; ch++)
	{
		if(PDU_Mk1_HSSControl_WriteHSSLatchField(ch, latch[ch]) != HSSCONTROL_🙂)
		{
			HSS_state = original_HSS_state;

			return HSSCONTROL_😢;
		}
	}
	
	return PDU_Mk1_HSSControl_UpdateHSSShiftRegs();
}

inline HSSControl_Status_t PDU_Mk1_HSSControl_OutputFaultRetry_Ch(HSSControl_Channel_t ch)
{
	return PDU_Mk1_HSSControl_WriteLatch_Ch(ch, HSSCONTROL_UNLATCHFAULT_AUTORETRY);
}

HSSControl_Status_t PDU_Mk1_HSSControl_OutputFaultRetry_AllFaulted()
{
	uint32_t original_HSS_state = HSS_state;

	for(uint8_t ch = 0; ch < PDU_MK1_NUM_CHANNELS; ch++)
	{
		if(HSS_fault_state[ch] != HSSCONTROL_NOFAULT)
		{
			if(PDU_Mk1_HSSControl_WriteHSSLatchField(ch, HSSCONTROL_UNLATCHFAULT_AUTORETRY) != HSSCONTROL_🙂)
			{
				HSS_state = original_HSS_state;

				return HSSCONTROL_😢;
			}
		}
	}

	return PDU_Mk1_HSSControl_UpdateHSSShiftRegs();
}

inline HSSControl_Status_t PDU_Mk1_HSSControl_OutputFaultRelatch_Ch(HSSControl_Channel_t ch)
{
	return PDU_Mk1_HSSControl_WriteLatch_Ch(ch, HSSCONTROL_LATCHFAULT_STAYOFF);
}

HSSControl_Status_t PDU_Mk1_HSSControl_OutputFaultRelatch_AllFaulted()
{
	uint32_t original_HSS_state = HSS_state;

	for(uint8_t ch = 0; ch < PDU_MK1_NUM_CHANNELS; ch++)
	{
		if(HSS_fault_state[ch] != HSSCONTROL_NOFAULT)
		{
			if(PDU_Mk1_HSSControl_WriteHSSLatchField(ch, HSSCONTROL_LATCHFAULT_STAYOFF) != HSSCONTROL_🙂)
			{
				HSS_state = original_HSS_state;

				return HSSCONTROL_😢;
			}
		}
	}

	return PDU_Mk1_HSSControl_UpdateHSSShiftRegs();
}

bool SR_SPI_ReconfigSPI(ShiftRegister_SPI_HandleTypeDef* device)
{
	if(device == &hsscontrol_sr)
	{
		return PDU_Mk1_SPI2_HSS_SR_Init();
	}

    return false;
}
