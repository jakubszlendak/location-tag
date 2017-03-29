/*
 * ble.c
 *
 *  Created on: 11 wrz 2016
 *      Author: jakub
 */

#include "ble_wrapper.h"
#include "advertising.h"
#include "ble_service.h"
#include "persistence.h"
#include "config.h"

#include <ble_hci.h>
#include <softdevice_handler.h>
#include <string.h>
#include <ble.h>


uint16_t 			m_conn_handle = BLE_CONN_HANDLE_INVALID;
ble_service_t		m_service;

void app_error_handler(uint32_t error_code, uint32_t line_num, const uint8_t * p_file_name) {

	while(1) {
		__disable_irq();
	}
}


static void on_service_evt(ble_service_t * p_service, ble_service_evt_t * p_evt, ble_service_data_t p_data, uint8_t data_size) {
	persistence_store_data_async(PERSISTENCE_DATA_ADDRESS, p_data, 4);
}

/**@brief Function for handling the Application's BLE Stack events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 */
static void on_ble_evt(ble_evt_t * p_ble_evt) {
    uint32_t                         err_code;
    static ble_gap_evt_auth_status_t m_auth_status;
    ble_gap_enc_info_t *             p_enc_info;

    switch (p_ble_evt->header.evt_id) {
        case BLE_GAP_EVT_CONNECTED:
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            m_conn_handle = BLE_CONN_HANDLE_INVALID; 	/*< Invalidate handle */

            advertising_start();						/*< Restart advertising */
            break;

        case BLE_GATTC_EVT_TIMEOUT:
		case BLE_GATTS_EVT_TIMEOUT:
            if (p_ble_evt->evt.gatts_evt.params.timeout.src == BLE_GATT_TIMEOUT_SRC_PROTOCOL) {
                err_code = sd_ble_gap_disconnect(m_conn_handle,
                                                 BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            }
            break;

        case BLE_GAP_EVT_TIMEOUT:
            if(p_ble_evt->evt.gap_evt.params.timeout.src == BLE_GAP_TIMEOUT_SRC_CONN) {
                err_code = sd_ble_gap_disconnect(m_conn_handle,
                                                 BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            }
			if (p_ble_evt->evt.gap_evt.params.timeout.src == BLE_GAP_TIMEOUT_SRC_ADVERTISING) {
				advertising_start();
			}
            break;

        default:
            // No implementation needed.
            break;
    }
}



/**@brief Function for dispatching a BLE stack event to all modules with a BLE stack event handler.
 *
 * @details This function is called from the scheduler in the main loop after a BLE stack
 *          event has been received.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 */
static void ble_evt_dispatch(ble_evt_t * p_ble_evt) {
	//ble_advertising_on_ble_evt(p_ble_evt);
	on_ble_evt(p_ble_evt);
	ble_service_on_ble_evt(&m_service, p_ble_evt);
}


/**@brief Function for dispatching a system event to interested modules.
 *
 * @details This function is called from the System event interrupt handler after a system
 *          event has been received.
 *
 * @param[in]   sys_evt   System stack event.
 */
static void sys_evt_dispatch(uint32_t sys_evt) {
	uint32_t persistence_cb_result = persistence_op_callback(sys_evt);

	if(persistence_cb_result) {
		// reconfigure advertising
		advertising_init();
		if(m_conn_handle == BLE_CONN_HANDLE_INVALID) {
			advertising_start();
		}
	}
}


/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init() {
    uint32_t err_code;

    // Initialize the SoftDevice handler module.
    SOFTDEVICE_HANDLER_INIT(NRF_CLOCK_LFCLKSRC_XTAL_20_PPM, false);

    // Enable BLE stack
    ble_enable_params_t ble_enable_params;
    memset(&ble_enable_params, 0, sizeof(ble_enable_params));
    ble_enable_params.gatts_enable_params.service_changed = 0;

    err_code = sd_ble_enable(&ble_enable_params);
    APP_ERROR_CHECK(err_code);

    // Register with the SoftDevice handler module for BLE events.
    err_code = softdevice_ble_evt_handler_set(ble_evt_dispatch);
    APP_ERROR_CHECK(err_code);

    // Register with the SoftDevice handler module for system events.
    err_code = softdevice_sys_evt_handler_set(sys_evt_dispatch);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for the GAP initialization.
 *
 * @details This function sets up all the necessary GAP (Generic Access Profile) parameters of the
 *          device including the device name, appearance, and the preferred connection parameters.
 */
static void gap_params_init() {
    uint32_t                err_code;
//    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *)ADVERTISING_NAME,
                                          strlen(ADVERTISING_NAME));
    APP_ERROR_CHECK(err_code);

}


/**@brief Function for initializing services that will be used by the application.
 */
static void services_init() {

    uint32_t 						err_code;
	ble_service_init_t				service_init;

	//handler assignement
	service_init.evt_handler = on_service_evt;//TODO

	err_code = ble_service_init(&m_service, &service_init);
	APP_ERROR_CHECK(err_code);
}



void BLE_init() {
	ble_stack_init();
	gap_params_init();
	services_init();
}

