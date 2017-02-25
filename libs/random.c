/*
 * random.c
 *
 *  Created on: 11 wrz 2016
 *      Author: jakub
 */
#include "random.h"
#include <nrf_soc.h>

uint32_t get_random_bytes(uint8_t* buffer, uint8_t size)
{
	uint8_t left = size;
	uint8_t bytes_available = 0;
	uint32_t err_code = 0;
	while(left != 0) {
		while( bytes_available == 0 ) {
			sd_rand_application_bytes_available_get(&bytes_available);
		}
		if( left < bytes_available ) {
			bytes_available = left;
		}
		err_code = sd_rand_application_vector_get(buffer + size - left, bytes_available);
		if(err_code == NRF_SUCCESS) {
			left -= bytes_available;
		}
		bytes_available = 0;
	}
	return NRF_SUCCESS;
}
