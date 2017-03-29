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

typedef struct {
	uint8_t available;
	uint32_t data[8];
	uint8_t size;
	uint32_t* addr;
} data_to_persist_t;

data_to_persist_t 	data_to_persist;

uint8_t operation_completed_flag = 0;
uint8_t write_op_pending_flag = 0;

uint32_t data_to_write[16];
uint8_t data_to_write_size = 0;

static uint32_t perform_pending_write_op() {
	operation_completed_flag = PERSISTENCE_OPERATION_IN_PROGRESS;
	uint32_t err_code = sd_flash_write(data_to_persist.addr, data_to_persist.data, data_to_persist.size/4);
	return err_code;
}



uint32_t persistence_op_callback(uint32_t evt) {
	switch(evt)
	{
		case NRF_EVT_FLASH_OPERATION_SUCCESS:
		{
			operation_completed_flag = PERSISTENCE_OPERATION_SUCCESS;

			if(write_op_pending_flag) {
				perform_pending_write_op();
				write_op_pending_flag = 0;
			}

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


	persistence_erase_page_async(addr);

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

	write_op_pending_flag = 1;

	data_to_persist.addr = addr;
	data_to_persist.size = 4;
	memcpy(data_to_persist.data, data, size);
	data_to_persist.available = 1;
	persistence_erase_page_async(addr);



	return NRF_SUCCESS;
}

uint32_t persistence_erase_page_async(uint32_t* addr) {
	operation_completed_flag = PERSISTENCE_OPERATION_IN_PROGRESS;
	sd_flash_page_erase((uint32_t) addr/1024);

	return NRF_SUCCESS;
}
