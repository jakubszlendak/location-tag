/* Copyright (c) 2012 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

/* Attention!
*  To maintain compliance with Nordic Semiconductor ASA�s Bluetooth profile
*  qualification listings, this section of source code must not be modified.
*/

#include <ble_service.h>
#include <ble_gap.h>
#include <ble_gatt.h>
#include <ble_types.h>
#include <nordic_common.h>
#include <nrf_error.h>
#include <nrf_soc.h>
#include <string.h>
#include <sys/_stdint.h>







/**@brief Function for handling the Connect event.
 *
 * @param[in]   p_service       Blood Pressure Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_connect(ble_service_t * p_service, ble_evt_t * p_ble_evt)
{
    p_service->conn_handle = p_ble_evt->evt.gatts_evt.conn_handle;
}


/**@brief Function for handling the Disconnect event.
 *
 * @param[in]   p_service       Blood Pressure Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_disconnect(ble_service_t * p_service, ble_evt_t * p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    p_service->conn_handle = BLE_CONN_HANDLE_INVALID;
}


/**@brief Function for handling the CCCD write events to ADVDATA characteristic.
 *
 * @param[in]   p_service         BLE Service structure.
 * @param[in]   p_evt_write   Write event received from the BLE stack.
 */
static void on_cccd_write(ble_service_t * p_service, ble_gatts_evt_write_t * p_evt_write)
{
    if (p_evt_write->len == 2)
    {
        // CCCD written, update indication state
        if (p_service->evt_handler != NULL)
        {
            ble_service_evt_t evt;

            if (ble_srv_is_indication_enabled(p_evt_write->data))
            {
                evt.evt_type = BLE_SERVICE_EVT_INDICATION_ENABLED;
            }
            else
            {
                evt.evt_type = BLE_SERVICE_EVT_INDICATION_DISABLED;
            }

            p_service->evt_handler(p_service, &evt, p_evt_write->data, 0);
        }
    }
}

/**@brief Function for handling the value write events to ADVDATA characteristic.
 *
 * @param[in]   p_service         BLE Service structure.
 * @param[in]   p_evt_write   Write event received from the BLE stack.
 */
static void on_advdata_write(ble_service_t * p_service, ble_gatts_evt_write_t * p_evt_write)
{
    if (p_evt_write->len)
    {
        // CCCD written, update indication state
        if (p_service->evt_handler != NULL)
        {
            ble_service_evt_t evt;
			evt.evt_type = BLE_SERVICE_EVT_ADV_DATA_RECEIVED;
            p_service->evt_handler(p_service, &evt, p_evt_write->data, p_evt_write->len);
        }
    }
}


/**@brief Function for handling the Write event
 *
 * @param[in]   p_service       Blood Pressure Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_write(ble_service_t * p_service, ble_evt_t * p_ble_evt)
{
    ble_gatts_evt_write_t * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

	if (p_evt_write->handle == p_service->advdata_handles.value_handle) //Handle VALUE write to RX characteristic
	{
			on_advdata_write(p_service, p_evt_write);
	}
}


void ble_service_on_ble_evt(ble_service_t * p_service, ble_evt_t * p_ble_evt)
{
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            on_connect(p_service, p_ble_evt);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            on_disconnect(p_service, p_ble_evt);
            break;

        case BLE_GATTS_EVT_WRITE:
            on_write(p_service, p_ble_evt);
            break;

        default:
            // No implementation needed.
            break;
    }
}


/**@brief Function for adding ADVDATA characteristics.
 *
 * @param[in]   p_service        BLE Service structure.
 * @param[in]   p_service_init   Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t advdata_char_add(ble_service_t * p_service, const ble_service_init_t * p_service_init)
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;

    // Clear char_md structure
    memset(&char_md, 0, sizeof(char_md));

		//	Set write avaliability
    char_md.char_props.write  = 1;

		//	Setup descriptor
	char_md.p_char_user_desc = NULL;
	char_md.p_char_pf        = NULL;
    char_md.p_user_desc_md   = NULL;
    char_md.p_cccd_md        = NULL;
    char_md.p_sccd_md        = NULL;

	//	Inform the characteristic that it will have a custom vendor UUID
	ble_uuid.type = p_service->uuid_type;
	//	Set the 16-bit UUID for the characteristic
	ble_uuid.uuid = ADVDATA_CHAR_UUID;

    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
    attr_md.vloc       = BLE_GATTS_VLOC_USER;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = 1;
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = 20;
    attr_char_value.p_value      = 0x00;

    return sd_ble_gatts_characteristic_add(p_service->service_handle,
                                           &char_md,
                                           &attr_char_value,
                                           &p_service->advdata_handles);
}



uint32_t ble_service_init(ble_service_t * p_service, const ble_service_init_t * p_service_init)
{
    uint32_t   err_code;
	ble_uuid128_t base_uuid = LOCATION_TAG_UUID_BASE;

    // Initialize service structure
    p_service->evt_handler = p_service_init->evt_handler;
    p_service->conn_handle = BLE_CONN_HANDLE_INVALID;

	// Assign base UUID
	err_code = sd_ble_uuid_vs_add(&base_uuid, &(p_service->uuid_type));
	if(err_code != NRF_SUCCESS)
	{
			return err_code;
	}


	ble_uuid_t ble_uuid;
	//	Set the vendor specific UUID to the newly created service (?)
	ble_uuid.type = p_service->uuid_type;
	//  Set the 16-bit UUID
	ble_uuid.uuid = SERVICE_UUID;
	//	Add the service and create a service_handle to it
	err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &(p_service->service_handle));
	if(err_code != NRF_SUCCESS)
	{
			return err_code;
	}


    // Add characteristic
    err_code = advdata_char_add(p_service, p_service_init);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }


    return NRF_SUCCESS;
}




