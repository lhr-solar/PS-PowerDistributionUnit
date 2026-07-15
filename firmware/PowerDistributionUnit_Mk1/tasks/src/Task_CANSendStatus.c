// Task_CanSendStatus.c

#include "Task_CanSendStatus.h"

static uint16_t can_send_failures = 0;

void Task_CanSendStatus(void *argument)
{
	// CAN peripheral must be ready before running task. 

	FDCAN_TxHeaderTypeDef tx_header = {0};   
	tx_header.Identifier = PDU_MK1_CAN_ID_PDUSTATUS;
	tx_header.IdType = FDCAN_STANDARD_ID;
	tx_header.TxFrameType = FDCAN_DATA_FRAME;
	tx_header.DataLength = PDU_MK1_CAN_MSGLEN_PDUSTATUS;
	tx_header.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
	tx_header.BitRateSwitch = FDCAN_BRS_OFF;
	tx_header.FDFormat = FDCAN_CLASSIC_CAN;
	tx_header.TxEventFifoControl = FDCAN_STORE_TX_EVENTS;
	tx_header.MessageMarker = 0;

	// single channel
	uint8_t ch_to_send = 0;
	uint8_t ch_data[PDU_MK1_CAN_MSGLEN_PDUSTATUS] = {0};

	// spam all channels
	// uint8_t data[PDU_MK1_NUM_CHANNELS][PDU_MK1_CAN_MSGLEN_PDUSTATUS] = {0};

	for(;;)
	{
		// SINGLE CHANNEL
		for(uint8_t i = 0; i < PDU_MK1_CAN_MSGLEN_PDUSTATUS; i++)
		{
			ch_data[i] = 0;
		}

		ch_data[0] |= ch_to_send;

		if(PDU_Mk1_HSSControl_GetFaultState_Ch(ch_to_send) != HSSCONTROL_NOFAULT)
		{
			ch_data[0] |= (1 << PDU_MK1_CAN_PDUSTATUS_FAULT_LSHIFT);		// FAULT
		}

		if(PDU_Mk1_HSSControl_GetENState_Ch(ch_to_send) == HSSCONTROL_EN_ON)
		{
			ch_data[0] |= (1 << PDU_MK1_CAN_PDUSTATUS_ENABLED_LSHIFT); 		// ENABLED
		}

		// *((uint16_t*) (ch_data+1)) = ((uint16_t) ((24+ch_to_send*0.1)*1000));     // dummy voltage data
		// *((uint16_t*) (ch_data+3)) = ((uint16_t) ((ch_to_send*0.67)*1000));       // dummy current data
		*((uint16_t*) (ch_data+PDU_MK1_CAN_PDUSTATUS_VOLTAGE_INDEX)) = ((uint16_t) (0));     // no voltage data yet
		*((uint16_t*) (ch_data+PDU_MK1_CAN_PDUSTATUS_CURRENT_INDEX)) = ((uint16_t) (PDU_Mk1_CurrentSensing_GetCurrentsPtr()[ch_to_send]*PDU_MK1_CAN_PDUSTATUS_CURRENT_INVFACTOR));

		if (can_fd_send(hfdcan3, &tx_header, ch_data, pdMS_TO_TICKS(PDU_MK1_CAN_SEND_TIMEOUT_MS)) != CAN_OK){
			can_send_failures++;
			printf("ERROR:CAN_SEND_PDUSTATUS_%d\n", can_send_failures);

#ifdef PDU_MK1_SDLOG_ERRORS
        PDU_Mk1_SDCard_LogError(PDU_MK1_SDLOG_ERRORID_CAN_SEND_STATUS, can_send_failures);
#endif
		}
		else
		{
			ch_to_send = (ch_to_send + 1) % PDU_MK1_NUM_CHANNELS;
		}

		vTaskDelay(pdMS_TO_TICKS(TASK_CANSENDSTATUS_CH_INTERVAL_MS));



		// // SPAM ALL CHANNELS (doesn't work)
		// for(uint8_t ch = 0; ch < PDU_MK1_NUM_CHANNELS; ch++)
		// {
		//     for(uint8_t i = 0; i < PDU_MK1_CAN_MSGLEN_PDUSTATUS; i++)
		//     {
		//         data[ch][i] = 0;
		//     }

		//     data[ch][0] |= ch;
		//     data[ch][0] |= (0 << PDU_MK1_CAN_PDUSTATUS_FAULT_LSHIFT); // FAULT: TBD
		//     data[ch][0] |= (1 << PDU_MK1_CAN_PDUSTATUS_ENABLED_LSHIFT); // ENABLED: TBD

		//     *((uint16_t*) data[ch]+1) = ((uint16_t) ((24+ch*0.1)*1000));     // dummy voltage data
		//     *((uint16_t*) data[ch]+3) = ((uint16_t) ((ch*0.67)*1000));       // dummy current data
		//     *((uint16_t*) data[ch]+3) = ((uint16_t) (PDU_Mk1_CurrentSensing_GetCurrentsPtr()[ch]*1000));
		// }

		// for(uint8_t ch = 0; ch < 4; ch++)
		// {
		//     printf("try send %d\n", ch);
		//     if (can_fd_send(hfdcan3, &tx_header, data[ch], portMAX_DELAY) != CAN_OK){
		//         while(1)
		//         {
		//             printf("ERROR:CAN_SEND_PDUSTATUS\n");
		//             Error_Handler();
		//         }
		//     }
		//     printf("did %d\n", ch);
		// }

		// vTaskDelay(pdMS_TO_TICKS(TASK_CANSENDSTATUS_INTERVAL_MS));
	}
}
