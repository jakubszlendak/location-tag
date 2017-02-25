#include <ble_gap.h>
#include <config.h>
#include <string.h>
#include <sys/_stdint.h>
#include <nrf_error.h>
#include <app_error.h>
#include "advertising.h"
#include "random.h"

/**
 * Structure of advertising data type declaration
 */
typedef struct {
	uint8_t len;
	uint8_t type;
} adv_data_type_t;

/**
 * LocationTAG specific payload
 */
typedef struct {
	uint32_t group_id;
	uint8_t beacon_id[6];
} location_tag_payload_t;


/**
 * Advertising packet structure
 */
typedef struct {
	adv_data_type_t flags_type;
	uint8_t flags;
	adv_data_type_t name_type;
	uint8_t name[11];
	adv_data_type_t payload_type;
	uint16_t manufacturer_id;
	location_tag_payload_t payload;
} advertising_packet_t;



const uint8_t ADV_FLAGS_LENGTH = 2;
const uint8_t ADV_NAME_LENGTH = 12;
const uint8_t ADV_PAYLOAD_LENGTH = 14;
const uint16_t ADV_MANUFACTURER_ID = 0xFFFF;

uint8_t m_adv_buffer[31];
uint8_t m_adv_name[11] = ADVERTISING_NAME;

advertising_packet_t m_advertising_packet;
ble_gap_adv_params_t m_advertising_params;

/**
 * @brief Initializes advertising structure
 */
void advertising_init(void) {
	uint32_t err_code = 0;
	// Advertising content
	m_advertising_packet.flags_type.len = ADV_FLAGS_LENGTH;
	m_advertising_packet.flags_type.type = BLE_GAP_AD_TYPE_FLAGS;
	m_advertising_packet.flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;

	m_advertising_packet.name_type.len = ADV_NAME_LENGTH;
	m_advertising_packet.name_type.type = BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME;
	memcpy(&(m_advertising_packet.name), m_adv_name, sizeof(m_adv_name));

	m_advertising_packet.payload_type.len = ADV_PAYLOAD_LENGTH;
	m_advertising_packet.payload_type.type = BLE_GAP_AD_TYPE_MANUFACTURER_SPECIFIC_DATA;
	m_advertising_packet.manufacturer_id = ADV_MANUFACTURER_ID;

	// Default GROUP ID to 0xFF
	uint32_t *gid = PERSISTENCE_DATA_ADDRESS;
	m_advertising_packet.payload.group_id = *gid;
	// Default BEACON_ID to beacon MAC address
	ble_gap_addr_t mac_addr;
	err_code = sd_ble_gap_address_get(&mac_addr);
	if (err_code != NRF_SUCCESS) {
		APP_ERROR_CHECK(err_code);
	}
	memcpy(&(m_advertising_packet.payload.beacon_id), mac_addr.addr, sizeof(mac_addr.addr));


	err_code = sd_ble_gap_adv_data_set((uint8_t*)&m_advertising_packet, 31, NULL, 0);
	if (err_code != NRF_SUCCESS) {
		APP_ERROR_CHECK(err_code);
	}

	/// Advertising params
	memset(&m_advertising_params, 0, sizeof(m_advertising_params));
	m_advertising_params.type = 		BLE_GAP_ADV_TYPE_ADV_IND;
	m_advertising_params.p_peer_addr = 	NULL;
	m_advertising_params.fp = 			BLE_GAP_ADV_FP_ANY;
	m_advertising_params.p_whitelist = 	NULL;
	m_advertising_params.interval = 	ADVERTISING_INTERVAL;
	m_advertising_params.timeout = 		0;

}

/**
 * @brief Starts advertising
 */
void advertising_start(void) {
	uint32_t err_code = sd_ble_gap_adv_data_set((uint8_t*)&m_advertising_packet, 31, NULL, 0);
	if (err_code != NRF_SUCCESS)	{
		APP_ERROR_CHECK(err_code);
	}

	err_code = sd_ble_gap_adv_start(&m_advertising_params);
	if (err_code != NRF_SUCCESS)	{
		APP_ERROR_CHECK(err_code);
	}

}

