/*
 * persistence.h
 *
 *  Created on: 18 wrz 2016
 *      Author: jakub
 */

#ifndef PERSISTENCE_H_
#define PERSISTENCE_H_

#include <sys/_stdint.h>

/**
 * @brief SoftDevice callback, to be called from system event handler.
 * @return 0 if no persistence operation or operation in progress, otherwise 1 or 2
 */
uint32_t persistence_op_callback(uint32_t evt);

/**
 * @brief Stores value in non-volatile memory
 * @param addr address in memory
 * @param data pointer to data buffer
 * @param size buffer size, must be multiply of 4
 * @return NRF_SUCCESS or error code
 */
uint32_t persistence_store_data(uint32_t* addr, uint8_t* data, uint32_t size);

/**
 * @brief Stores value in non-volatile memory in async way
 * @param addr address in memory
 * @param data pointer to data buffer
 * @param size buffer size, must be multiply of 4
 * @return NRF_SUCCESS
 */
uint32_t persistence_store_data_async(uint32_t* addr, uint8_t* data, uint32_t size);

uint32_t persistence_erase_page_async(uint32_t* addr);

#endif /* PERSISTENCE_H_ */
