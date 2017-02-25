/*
 * persistence.c
 *
 *  Created on: 18 wrz 2016
 *      Author: jakub
 */

#include <nrf_error.h>
#include <nrf_soc.h>
#include <persistence.h>


#define PERSISTENCE_OPERATION_SUCCESS 		2
#define PERSISTENCE_OPERATION_FAILED 		1
#define PERSISTENCE_OPERATION_IN_PROGRESS	0

#define PERSISTENCE_FLASH_ERROR				0x21

uint8_t operation_completed_flag = 0;

uint32_t persistence_op_callback(uint32_t evt) {
	switch(evt)
	{
		case NRF_EVT_FLASH_OPERATION_SUCCESS:
		{
			operation_completed_flag = PERSISTENCE_OPERATION_SUCCESS;
			return PERSISTENCE_OPERATION_SUCCESS;
		}
		case NRF_EVT_FLASH_OPERATION_ERROR:
		{
			operation_completed_flag = PERSISTENCE_OPERATION_FAILED;
			return PERSISTENCE_OPERATION_FAILED;
		}
	}

	return PERSISTENCE_OPERATION_IN_PROGRESS;
}

uint32_t persistence_store_data(uint32_t* addr, uint8_t* data, uint32_t size) {


	operation_completed_flag = PERSISTENCE_OPERATION_IN_PROGRESS;
	sd_flash_write(addr, data, size/4);

	while(operation_completed_flag == PERSISTENCE_OPERATION_IN_PROGRESS) {
		__WFI();
	}

	if(operation_completed_flag != PERSISTENCE_OPERATION_SUCCESS)
		return PERSISTENCE_FLASH_ERROR;

	else return NRF_SUCCESS;
}

uint32_t persistence_store_data_async(uint32_t* addr, uint8_t* data, uint32_t size) {


	operation_completed_flag = PERSISTENCE_OPERATION_IN_PROGRESS;
	sd_flash_write(addr, data, size/4);

	return NRF_SUCCESS;
}
