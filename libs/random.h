/*
 * random.h
 *
 *  Created on: 11 wrz 2016
 *      Author: jakub
 */

#ifndef LIBS_RANDOM_H_
#define LIBS_RANDOM_H_
#include "nrf51.h"

/**
 * @brief Return random bytes from PRNG
 * @param buffer output buffer
 * @param size count of random bytes to get
 * @return error code ( 0 if success )
 */
uint32_t get_random_bytes(uint8_t* buffer, uint8_t size);

#endif /* LIBS_RANDOM_H_ */
