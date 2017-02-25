/*
 * ble_service.h
 *
 *  Created on: 18 wrz 2016
 *      Author: jakub
 */

#ifndef BLE_SERVICE_H_
#define BLE_SERVICE_H_

/* Copyright (c) 2012 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 */


#include <ble.h>
#include <ble_gatts.h>
#include <ble_srv_common.h>
#include <stdbool.h>

#define LOCATION_TAG_UUID_BASE 		{0xb0, 0xa7, 0x48, 0xc7, 0xbd, 0x86, 0x01, 0xaa, 0xd0, 0x9e, 0x99, 0x88, 0x00, 0x00, 0x16, 0x1a}
#define SERVICE_UUID 				(0x0001)
#define ADVDATA_CHAR_UUID			(0x0002)



/**@brief Service event type. */
typedef enum
{
    BLE_SERVICE_EVT_INDICATION_ENABLED,                                         /**< Service value indication enabled event. */
    BLE_SERVICE_EVT_INDICATION_DISABLED,                                        /**< Service value indication disabled event. */
    BLE_SERVICE_EVT_INDICATION_CONFIRMED,                                        /**< Confirmation of a Service indication has been received. */
	BLE_SERVICE_EVT_ADV_DATA_RECEIVED
} ble_service_evt_type_t;

/**@brief Service event. */
typedef struct
{
    ble_service_evt_type_t evt_type;                                            /**< Type of event. */
} ble_service_evt_t;

// Forward declaration of the ble_service_t type.
typedef struct ble_service_s ble_service_t;

/**@brief Service data typedef. This is a Service
 *        data packet. */
typedef uint8_t* ble_service_data_t;

/**@brief Service event handler type. */
typedef void (*ble_service_evt_handler_t) (ble_service_t * p_dt, ble_service_evt_t * p_evt, ble_service_data_t p_data, uint8_t data_size);


/**@brief Service init structure. This contains all options and data
 *        needed for initialization of the service. */
typedef struct
{
    ble_service_evt_handler_t    evt_handler;                              /**< Event handler to be called for handling events in the Service. */
    ble_srv_cccd_security_mode_t meas_attr_md;                          /**< Initial security level for Service measurement attribute */
    ble_srv_security_mode_t      feature_attr_md;                       /**< Initial security level for Service feature attribute */
    uint16_t                     feature;                                  /**< Initial value for Service feature */
} ble_service_init_t;

/**@brief Service structure. This contains various status information for
 *        the service. */
struct ble_service_s
{
    ble_service_evt_handler_t    evt_handler;               /**< Event handler to be called for handling events in the Service. */
    uint16_t                     service_handle;            /**< Handle of Service (as provided by the BLE stack). */
    ble_gatts_char_handles_t     advdata_handles;           /**< Handles related to the Advertising Data characteristic. */
    uint8_t 					 uuid_type;
	uint16_t                     conn_handle;               /**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection). */
};


/**@brief Function for initializing the Service.
 *
 * @param[out]  p_service       Service structure. This structure will have to
 *                          be supplied by the application. It will be initialized by this function,
 *                          and will later be used to identify this particular service instance.
 * @param[in]   p_service_init  Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on successful initialization of service, otherwise an error code.
 */
uint32_t ble_service_init(ble_service_t * p_service , const ble_service_init_t * p_service_init);

/**@brief Function for handling the Application's BLE Stack events.
 *
 * @details Handles all events from the BLE stack of interest to the Service.
 *
 * @param[in]   p_service       Service structure.
 * @param[in]   p_ble_evt  Event received from the BLE stack.
 */
void ble_service_on_ble_evt(ble_service_t * p_service , ble_evt_t * p_ble_evt);



/** @} */


#endif /* BLE_SERVICE_H_ */
