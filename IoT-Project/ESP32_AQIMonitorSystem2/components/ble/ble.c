/*******************************************************************************
 * File:        ble_service.c
 *
 * Description: This file implements BLE services and event handling for the ESP32.
 *              It initializes BLE stack, sets up advertising parameters, and manages
 *              GATT and GAP event callbacks. BLE is used to receive configuration
 *              data such as Wi-Fi credentials, AWS IoT endpoint, private key, and
 *              certificate, which are parsed and stored in EEPROM or flash memory.
 *
 *              Key Functionalities:
 *              - BLE initialization and advertising start.
 *              - GAP event handling (advertising start/stop, config complete).
 *              - GATT server integration for custom characteristic handling.
 *              - Parsing and storing received BLE data (Wi-Fi, AWS credentials).
 *              - Event synchronization with FreeRTOS for connection control.
 *
 * Author:      FidilitySolutions
 *
 * Reference:   ESP-IDF BLE GATT Server Example  
 *              https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/bluetooth/esp_gattc.html
 *
 *******************************************************************************/


/* Header files */
#include "ble.h"
#include <inttypes.h>
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"
#include "eeprom.h"
#include "event_manager.h"
//#define BLE_CONNECTION_TIMEOUT_MS 30000 // 30 seconds timeout


static uint8_t au8ServiceUuid[16] = {
	/* LSB <--------------------------------------------------------------------------------> MSB */
	0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF, 0x12, 0x34, 0x56, 0x78, 0x00, 0x00, 0x00, 0x00,
};

static uint8_t au8AdvServiceUuid[16] = {
	/* LSB <--------------------------------------------------------------------------------> MSB */
	0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF, 0x12, 0x34, 0x56, 0x78, 0x01, 0x00, 0x00, 0x00,
};

static uint8_t au8WifiCharUuid[16] = {
	/* LSB <--------------------------------------------------------------------------------> MSB */
	0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF, 0x12, 0x34, 0x56, 0x78, 0x02, 0x00, 0x00, 0x00,
};

static uint8_t au8LocCharUuid[16] = {
	/* LSB <--------------------------------------------------------------------------------> MSB */
	0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF, 0x12, 0x34, 0x56, 0x78, 0x03, 0x00, 0x00, 0x00,
};

static uint8_t au8HVersionCharUuid[16] = {
	/* LSB <--------------------------------------------------------------------------------> MSB */
	0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF, 0x12, 0x34, 0x56, 0x78, 0x04, 0x00, 0x00, 0x00,
};

static uint8_t au8SVersionCharUuid[16] = {
	/* LSB <--------------------------------------------------------------------------------> MSB */
	0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF, 0x12, 0x34, 0x56, 0x78, 0x05, 0x00, 0x00, 0x00,
};



static uint8_t au8AWSCertCharUuid[16] = {
	/* LSB <--------------------------------------------------------------------------------> MSB */
	0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF, 0x12, 0x34, 0x56, 0x78, 0x06, 0x00, 0x00, 0x00,
};

static uint8_t au8AWSPrivateKeyCharUuid[16] = {
	/* LSB <--------------------------------------------------------------------------------> MSB */
	0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF, 0x12, 0x34, 0x56, 0x78, 0x07, 0x00, 0x00, 0x00,
};

static uint8_t au8AWSEndpointCharUuid[16] = {
	/* LSB <--------------------------------------------------------------------------------> MSB */
	0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF, 0x12, 0x34, 0x56, 0x78, 0x08, 0x00, 0x00, 0x00,
};

esp_ble_adv_data_t adv_data = {
	.set_scan_rsp = false,
	.include_name = true,
	.include_txpower = false,
	.min_interval = 0x20,
	.max_interval = 0x40,
	.appearance = 0x00,
	.manufacturer_len = 0,
	.p_manufacturer_data = NULL,
	.service_data_len = 0,
	.p_service_data = NULL,
	.service_uuid_len = 16,
	.p_service_uuid = au8ServiceUuid,
	.flag = ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT,
};


static esp_ble_adv_params_t adv_params = {

	.adv_int_min = 0x20,
	.adv_int_max = 0x40,
	.adv_type = ADV_TYPE_IND,
	.own_addr_type = BLE_ADDR_TYPE_PUBLIC,
	.channel_map = ADV_CHNL_ALL,
	.adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

/**
 * @brief GATT Server Profile Structure
 */
typedef struct 
{
	esp_gatts_cb_t gatts_cb;                      /* GATT event callback function */
	uint16_t u16GattsIf;                          /* GATT interface ID assigned by the stack */
	uint16_t u16AppId;                            /* Application ID for the profile */
	uint16_t u16ConnId;                           /* Connection ID for the connected client */
	uint16_t u16ServiceHandle;                    /* Service handle returned by ESP stack */
	esp_gatt_srvc_id_t service_id;                /* GATT service ID structure */

	/* Characteristic handles */
	uint16_t u16WifiCharHandle;                  /* Handle for Wi-Fi SSID characteristic */
	uint16_t u16LocationCharHandle;              /* Handle for location characteristic */  
	uint16_t u16HwVersionCharHandle;             /* Handle for hardware version characteristic */
	uint16_t u16SwVersionCharHandle;             /* Handle for software version characteristic */
	uint16_t u16AwsCertCharHandle;               /* Handle for AWS certificate characteristic */
	uint16_t u16AwsPrivateKeyCharHandle;         /* Handle for AWS private key characteristic */
	uint16_t u16AwsEndpointCharHandle;           /* Handle for AWS endpoint characteristic */
} STR_GATTS_PROFILE_INST;

struct gatts_profile_inst 
{
	esp_gatts_cb_t gatts_cb;
	uint16_t u16GattsIf;
	uint16_t u16AppId;
	uint16_t u16ConnId;
	uint16_t u16ServiceHandle;
	esp_gatt_srvc_id_t service_id;
	uint16_t u16WifiCharHandle;
	uint16_t u16LocationCharHandle;
	uint16_t u16HwVersionCharHandle;
	uint16_t u16SwVersionCharHandle;
	uint16_t u16AwsCertCharHandle;
	uint16_t u16AwsPrivateKeyCharHandle;
	uint16_t u16AwsEndpointCharHandle;
};

typedef struct 
{
	char ssid[32];
	char password[64];
} STR_WIFI_CREDENTIALS;

typedef struct 
{
	char device_id[32];
} STR_LOCATION;

typedef struct 
{
	char hw_version[32];
	char sw_version[32];
} STR_VERSION_INFO;

/* Add AWS certificate structs */
typedef struct 
{
	char certificate[1700];  
} STR_AWS_CERTIFICATE;


static STR_WIFI_CREDENTIALS strWifiCredentials;
static STR_LOCATION strLocation;
static STR_VERSION_INFO strVersionInfo;
STR_AWS_CERTIFICATE strAwsCertificate;

bool wifi_configured = false;
static struct gatts_profile_inst gl_profile_tab[1];

void read_and_print_eeprom_section(const char* label, uint32_t address, size_t length);


/*******************************************************************************
 * Function      : parse_aws_certificate
 *
 * Description   : Parses the received AWS certificate, stores it into the
 *                 application data structure, and writes it to EEPROM/Flash.
 *
 * Parameters    :
 *      - ps8Data : Pointer to the received certificate buffer.
 *      - u64Len  : Length of the certificate string.
 *      - pcert   : Pointer to the structure holding the certificate.
 *
 * Returns       : void
 *
 *******************************************************************************/


static void parse_aws_certificate(const char* ps8Data, uint64_t u64Len, STR_AWS_CERTIFICATE* pstrcert, uint32_t eepromAddress, bool isPrivateKey);

//static void parse_aws_certificate(const char* ps8Data, uint64_t u64Len, STR_AWS_CERTIFICATE* pstrcert, uint32_t eepromAddress);
#define EEPROM_SIZE 2048  // Define the total EEPROM size
// Add a new variable to track which data type we are processing (certificate, private key, or endpoint)
#define EEPROM_SIZE 2048  // Define the total EEPROM size
// Add a new variable to track which data type we are processing (certificate, private key, or endpoint)
static void parse_aws_certificate(const char* ps8Data, uint64_t u64Len, STR_AWS_CERTIFICATE* pstrcert, uint32_t eepromAddress, bool isPrivateKey) {
    ESP_LOGI(BLE_TAG, "Processing certificate data chunk");

    // Skip status byte
    const char* data = ps8Data + 1;
    u64Len -= 1;

    // Static variables for tracking progress
    static size_t chunkCounter = 0;
    static size_t totalWritten = 0;

    // Reset buffers for new transmission
    if (chunkCounter == 0) {
        memset(pstrcert->certificate, 0, sizeof(pstrcert->certificate));
        totalWritten = 0;
    }

    // Verify no overflow (account for newline addition)
    if (totalWritten + u64Len + 1 > sizeof(pstrcert->certificate)) {
        ESP_LOGE(BLE_TAG, "Buffer overflow detected! Certificate exceeds allocated size.");
        return;
    }

    // Copy valid data to certificate buffer
    memcpy(pstrcert->certificate + totalWritten, data, u64Len);
    totalWritten += u64Len;

    // Add a newline character after the chunk
    pstrcert->certificate[totalWritten++] = '\n';

    chunkCounter++;

    ESP_LOGI(BLE_TAG, "Chunk %zu processed. Total written: %zu bytes", chunkCounter, totalWritten);

    // Write to EEPROM in chunks or after final chunk
    if (chunkCounter == (isPrivateKey ? 4 : 3)) {
        pstrcert->certificate[totalWritten] = '\0';  // Null-terminate
        ESP_LOGI(BLE_TAG, "Final AWS Certificate Content:\n%s", pstrcert->certificate);

        eeprom_write(eepromAddress, (uint8_t *)pstrcert->certificate, totalWritten);

        ESP_LOGI(BLE_TAG, "AWS certificate stored in EEPROM successfully at address 0x%x", (unsigned int)eepromAddress);

        // Reset static variables
        chunkCounter = 0;
        totalWritten = 0;
    }
}

void read_and_print_eeprom_section(const char* label, uint32_t address, size_t length) {
    uint8_t buffer[length];  // Buffer to hold the read data

    // Read data from EEPROM
    if (eeprom_read(address, buffer, length) == 0) {  // Assuming eeprom_read returns 0 on success
        ESP_LOGI(BLE_TAG, "%s Data at Address 0x%x:", label, (unsigned int)address);

        // For binary data, log in hexadecimal format
        ESP_LOGI(BLE_TAG, "Content (Hex):");
        for (size_t i = 0; i < length; i++) {
            printf("%02x ", buffer[i]);
        }
        printf("\n");

        // If data is a string, log it safely
        buffer[length - 1] = '\0';  // Null-terminate for safety
        ESP_LOGI(BLE_TAG, "Content (String): %s", buffer);
    } else {
        ESP_LOGE(BLE_TAG, "Failed to read EEPROM at Address 0x%x", (unsigned int)address);
    }
}

/*******************************************************************************
 * Function      : parse_wifi_credentials
 *
 * Description   : Parses Wi-Fi credentials received over BLE in the format
 *                 "SSID,PASSWORD", stores them into the provided structure,
 *                 and writes them to EEPROM for persistent storage.
 *
 * Parameters    :
 *      - ps8Data      : Pointer to received data string containing SSID and password.
 *      - u64Len       : Length of the received data.
 *      - pstrpCrntls  : Pointer to structure to store parsed Wi-Fi credentials.
 *
 *******************************************************************************/
static void parse_wifi_credentials(const char* ps8Data, uint64_t u64Len, STR_WIFI_CREDENTIALS* pstrpCrntls) 
{
	char as8Buffer[100] = {0};
	if(u64Len >= sizeof(as8Buffer)) 
	{
		ESP_LOGE(BLE_TAG, "Received data too large");
		return;
	}
	ESP_LOGI(BLE_TAG, "Raw Received WiFi Data: '%s' (Length: %llu)", ps8Data, u64Len);

	/* Copy and ensure null termination */
	memcpy(as8Buffer, ps8Data, u64Len);
	as8Buffer[u64Len] = '\0';

	/* Find delimiter */
	char *ps8Delimiter = strchr(as8Buffer, ',');
	if(!ps8Delimiter) 
	{
		ESP_LOGE(BLE_TAG, "Invalid format: missing delimiter");
		return;
	}

	/* Split into SSID and password */
	*ps8Delimiter = '\0';
	char *ps8Ssid = as8Buffer;
	char *ps8Password = ps8Delimiter + 1;

	/* Copy to wifi_credentials struct */
	memset(pstrpCrntls, 0, sizeof(strWifiCredentials));

	strncpy(pstrpCrntls->ssid, ps8Ssid, sizeof(pstrpCrntls->ssid) - 1);
	strncpy(pstrpCrntls->password, ps8Password, sizeof(pstrpCrntls->password) - 1);

	ESP_LOGI(BLE_TAG, "Parsed WiFi credentials - SSID: %s, PASS: %s", pstrpCrntls->ssid, pstrpCrntls->password);

	ESP_LOGI(BLE_TAG, "Password: ****** (hidden for security)");

	eeprom_erase(EEPROM_WIFI_SSID_ADDR);
	eeprom_erase(EEPROM_WIFI_PASS_ADDR);

	eeprom_write(EEPROM_WIFI_SSID_ADDR, (uint8_t *)ps8Ssid, strlen(ps8Ssid) + 1);

	eeprom_write(EEPROM_WIFI_PASS_ADDR, (uint8_t *)pstrpCrntls->password, strlen(ps8Password) + 1);

	printf("Written Wifi Credential to EEPROM\n");

	/* Global variable for wifi credential settings */
	wifi_configured = true;
	xEventGroupSetBits(xEventGroup, BLE_WIFI_CONNECT_BIT);

	/* Manual configure */
	/* wifi_init(credentials->ssid, credentials->password); */
}

/*******************************************************************************
 * Function      : parse_location
 *
 * Description   : Parses latitude and longitude values from a comma-separated
 *                 string and stores them in the provided location structure.
 *
 * Parameters    :
 *      - ps8Data : Pointer to the received data buffer (format: "lat,lng").
 *      - u64Len  : Length of the input data.
 *      - pstrPLoc : Pointer to the location structure to store the result.
 *
 * Returns       : void
 *
 *******************************************************************************/
static void parse_location(const char* ps8Data, uint64_t u64Len, STR_LOCATION* pstrLoc) 
{
	char s8Buffer[100] = {0};

	if (u64Len >= sizeof(s8Buffer))
	{
		ESP_LOGE(BLE_TAG, "Received data too large");
		return;
	}

	/* Copy and ensure null termination */
	memcpy(s8Buffer, ps8Data, u64Len);
	s8Buffer[u64Len] = '\0';

	/* Validate data length for device_id */
	if (strlen(s8Buffer) >= sizeof(pstrLoc->device_id))
	{
		ESP_LOGE(BLE_TAG, "Device ID exceeds maximum length");
		return;
	}

	/* Copy to device_id field */
	strncpy(pstrLoc->device_id, s8Buffer, sizeof(pstrLoc->device_id) - 1);
	pstrLoc->device_id[sizeof(pstrLoc->device_id) - 1] = '\0'; // Ensure null termination

	ESP_LOGI(BLE_TAG, "Parsed Device ID: %s", pstrLoc->device_id);

	eeprom_erase(EEPROM_LOCATION_ADDR);

	eeprom_write(EEPROM_LOCATION_ADDR, (uint8_t *)pstrLoc->device_id, strlen(pstrLoc->device_id) + 1);

}

static void apply_hardware_version(const char *hw_version)
{
	ESP_LOGI(BLE_TAG, "Applying Hardware Version information:");
	ESP_LOGI(BLE_TAG, "Hardware Version: %s", hw_version);

	eeprom_erase(HW_VERSION_ADDR);

	eeprom_write(HW_VERSION_ADDR, (uint8_t *)hw_version, strlen(hw_version) + 1);

	/*char hwn_version[32]; // Adjust the size to match your expected hardware version length
	  memset(hwn_version, 0, sizeof(hwn_version)); // Initialize the buffer to avoid garbage data

	  ESP_LOGI(BLE_TAG, "Reading Hardware Version information:");

	// Read the hardware version from EEPROM
	eeprom_read(HW_VERSION_ADDR, (uint8_t *)hwn_version, sizeof(hwn_version) - 1);

	ESP_LOGI(BLE_TAG, "Hardware Version: %s", hwn_version);*/

}

static void apply_software_version(const char *sw_version)
{
	ESP_LOGI(BLE_TAG, "Applying Software Version information:");
	ESP_LOGI(BLE_TAG, "Software Version: %s", sw_version);

	eeprom_erase(SW_VERSION_ADDR);

	eeprom_write(SW_VERSION_ADDR, (uint8_t *)sw_version, strlen(sw_version) + 1);

}


/*******************************************************************************
 * Function      : gap_event_handler
 *
 * Description   : GAP callback handler for BLE events such as advertising
 *                 start/stop and advertisement data configuration completion.
 *
 * Parameters    :
 *      - event : GAP BLE callback event.
 *      - param : Parameters associated with the event.
 *
 *******************************************************************************/
static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
	printf("This is event handler: %d\n", event);
	switch(event)
	{
		case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
			printf("ble adv\n");
			esp_ble_gap_start_advertising(&adv_params);
			break;
		case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
			if(param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS)
			{
				ESP_LOGE(BLE_TAG, "Advertising start failed");
			}
			else
			{
				ESP_LOGI(BLE_TAG, "Advertising started");
			}
			break;
		case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
			if(param->adv_stop_cmpl.status != ESP_BT_STATUS_SUCCESS)
			{
				ESP_LOGE(BLE_TAG, "Advertising stop failed");
			}
			else
			{
				ESP_LOGI(BLE_TAG, "Advertising stopped");
			}
			break;
		default:
			break;
	}
}

static void gatts_profile_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, 
		esp_ble_gatts_cb_param_t *param) 
{
	struct gatts_profile_inst *profile = &gl_profile_tab[CONFIG_APP_ID];

	switch (event) 
	{
		case ESP_GATTS_REG_EVT:
			ESP_LOGI(BLE_TAG, "REGISTER_APP_EVT, status %d, u16AppId %d", param->reg.status, param->reg.app_id);
			profile->service_id.is_primary = true;
			profile->service_id.id.inst_id = 0x00;
			profile->service_id.id.uuid.len = ESP_UUID_LEN_128;
			memcpy(profile->service_id.id.uuid.uuid.uuid128, au8ServiceUuid, ESP_UUID_LEN_128);

			esp_ble_gap_set_device_name(DEVICE_NAME);

			esp_err_t eErrStat = esp_ble_gap_config_adv_data(&adv_data);
			if (eErrStat) 
			{
				ESP_LOGE(BLE_TAG, "config adv data failed: %s", esp_err_to_name(eErrStat));
				return;
			}

			esp_ble_gatts_create_service(gatts_if, &profile->service_id, 20);
			break;
		case ESP_GATTS_CREATE_EVT:
			ESP_LOGI(BLE_TAG, "CREATE_SERVICE_EVT, status %d,  service_handle %d", 
					param->create.status, param->create.service_handle);
			profile->u16ServiceHandle = param->create.service_handle;

			/* Create location characteristic */
			esp_bt_uuid_t location_uuid;
			location_uuid.len = ESP_UUID_LEN_128;
			memcpy(location_uuid.uuid.uuid128, au8LocCharUuid, ESP_UUID_LEN_128);

			esp_ble_gatts_add_char(profile->u16ServiceHandle, &location_uuid,
					ESP_GATT_PERM_WRITE,
					ESP_GATT_CHAR_PROP_BIT_WRITE,
					NULL, NULL);


			/* Create HW version characteristic */
			esp_bt_uuid_t hw_version_uuid;
			hw_version_uuid.len = ESP_UUID_LEN_128;
			memcpy(hw_version_uuid.uuid.uuid128, au8HVersionCharUuid, ESP_UUID_LEN_128);

			esp_ble_gatts_add_char(profile->u16ServiceHandle, &hw_version_uuid,
					ESP_GATT_PERM_WRITE,
					ESP_GATT_CHAR_PROP_BIT_WRITE,
					NULL, NULL);

			/* Create SW version characteristic */
			esp_bt_uuid_t sw_version_uuid;
			sw_version_uuid.len = ESP_UUID_LEN_128;
			memcpy(sw_version_uuid.uuid.uuid128, au8SVersionCharUuid, ESP_UUID_LEN_128);

			esp_ble_gatts_add_char(profile->u16ServiceHandle, &sw_version_uuid,
					ESP_GATT_PERM_WRITE,
					ESP_GATT_CHAR_PROP_BIT_WRITE,
					NULL, NULL);

			/* Create AWS certificate characteristic */
			esp_bt_uuid_t aws_cert_uuid;
			aws_cert_uuid.len = ESP_UUID_LEN_128;
			memcpy(aws_cert_uuid.uuid.uuid128, au8AWSCertCharUuid, ESP_UUID_LEN_128);

			esp_ble_gatts_add_char(profile->u16ServiceHandle, &aws_cert_uuid,
					ESP_GATT_PERM_WRITE,
					ESP_GATT_CHAR_PROP_BIT_WRITE,
					NULL, NULL);

			/* Create AWS private key characteristic */
			esp_bt_uuid_t aws_private_key_uuid;
			aws_private_key_uuid.len = ESP_UUID_LEN_128;
			memcpy(aws_private_key_uuid.uuid.uuid128, au8AWSPrivateKeyCharUuid, ESP_UUID_LEN_128);

			esp_ble_gatts_add_char(profile->u16ServiceHandle, &aws_private_key_uuid,
					ESP_GATT_PERM_WRITE,
					ESP_GATT_CHAR_PROP_BIT_WRITE,
					NULL, NULL);

			/* Create AWS endpoint characteristic */
			esp_bt_uuid_t aws_endpoint_uuid;
			aws_endpoint_uuid.len = ESP_UUID_LEN_128;
			memcpy(aws_endpoint_uuid.uuid.uuid128, au8AWSEndpointCharUuid, ESP_UUID_LEN_128);

			esp_ble_gatts_add_char(profile->u16ServiceHandle, &aws_endpoint_uuid,
					ESP_GATT_PERM_WRITE,
					ESP_GATT_CHAR_PROP_BIT_WRITE,
					NULL, NULL);	

			esp_bt_uuid_t wifi_uuid;
			wifi_uuid.len = ESP_UUID_LEN_128;
			memcpy(wifi_uuid.uuid.uuid128, au8WifiCharUuid, ESP_UUID_LEN_128);

			esp_ble_gatts_add_char(profile->u16ServiceHandle, &wifi_uuid,
					ESP_GATT_PERM_WRITE,
					ESP_GATT_CHAR_PROP_BIT_WRITE,
					NULL, NULL);

			esp_ble_gatts_start_service(profile->u16ServiceHandle);
			break;

		case ESP_GATTS_ADD_CHAR_EVT:
			ESP_LOGI(BLE_TAG, "ADD_CHAR_EVT, status %d, attr_handle %d, u16ServiceHandle %d",
					param->add_char.status, param->add_char.attr_handle, 
					param->add_char.service_handle);

			/* Assign handles to respective characteristics */
			if(memcmp(param->add_char.char_uuid.uuid.uuid128, au8WifiCharUuid, 16) == 0) 
			{
				profile->u16WifiCharHandle = param->add_char.attr_handle;
				ESP_LOGI(BLE_TAG, "WiFi characteristic added");
			} 
			else if(memcmp(param->add_char.char_uuid.uuid.uuid128, au8LocCharUuid, 16) == 0) 
			{
				profile->u16LocationCharHandle = param->add_char.attr_handle;
				ESP_LOGI(BLE_TAG, "Location characteristic added");
			}
			else if (memcmp(param->add_char.char_uuid.uuid.uuid128, au8HVersionCharUuid, 16) == 0) 
			{
				profile->u16HwVersionCharHandle = param->add_char.attr_handle;
				ESP_LOGI(BLE_TAG, "HW Version characteristic added");
			} 
			else if (memcmp(param->add_char.char_uuid.uuid.uuid128, au8SVersionCharUuid, 16) == 0) 
			{
				profile->u16SwVersionCharHandle = param->add_char.attr_handle;
				ESP_LOGI(BLE_TAG, "SW Version characteristic added");
			}
			else if(memcmp(param->add_char.char_uuid.uuid.uuid128, au8AWSCertCharUuid, 16) == 0) 
			{
				profile->u16AwsCertCharHandle = param->add_char.attr_handle;
				ESP_LOGI(BLE_TAG, "AWS certificate characteristic added");
			} 
			else if(memcmp(param->add_char.char_uuid.uuid.uuid128, au8AWSPrivateKeyCharUuid, 16) == 0) 
			{
				profile->u16AwsPrivateKeyCharHandle = param->add_char.attr_handle;
				ESP_LOGI(BLE_TAG, "AWS private key characteristic added");
			} 
			else if(memcmp(param->add_char.char_uuid.uuid.uuid128, au8AWSEndpointCharUuid, 16) == 0) 
			{
				profile->u16AwsEndpointCharHandle = param->add_char.attr_handle;
				ESP_LOGI(BLE_TAG, "AWS endpoint characteristic added");
			}
			break;
		case ESP_GATTS_WRITE_EVT:
			ESP_LOGI(BLE_TAG, "WRITE_EVT, handle %d, value_len %d", param->write.handle, param->write.len);

			if(param->write.handle == profile->u16LocationCharHandle) 
			{
				if(param->write.len <= sizeof(strLocation)) 
				{
					memcpy(&strLocation, param->write.value, param->write.len);
					ESP_LOGI(BLE_TAG, "Received location data");
					parse_location((char*)param->write.value, param->write.len, &strLocation);
				}
			} 
			else if (param->write.handle == profile->u16HwVersionCharHandle)
			{
				if (param->write.len < sizeof(strVersionInfo.hw_version))
				{
					memcpy(strVersionInfo.hw_version, param->write.value, param->write.len);
					strVersionInfo.hw_version[param->write.len] = '\0';

					/* Call the hardware version application function */
					apply_hardware_version(strVersionInfo.hw_version);
				}
			}
			else if (param->write.handle == profile->u16SwVersionCharHandle)
			{
				if (param->write.len < sizeof(strVersionInfo.sw_version))
				{
					memcpy(strVersionInfo.sw_version, param->write.value, param->write.len);
					strVersionInfo.sw_version[param->write.len] = '\0';

					/* Call the software version application function */
					apply_software_version(strVersionInfo.sw_version);
				}
			}
			else if(param->write.handle == profile->u16AwsCertCharHandle) 
			{
				ESP_LOGI(BLE_TAG, "Received AWS certificate data");
				parse_aws_certificate((char*)param->write.value, param->write.len, &strAwsCertificate,EEPROM_AWS_CERT_ADDR, false);
				read_and_print_eeprom_section("AWS Certificate", EEPROM_AWS_CERT_ADDR, 1700);
			} 
			else if(param->write.handle == profile->u16AwsPrivateKeyCharHandle) 
			{
				ESP_LOGI(BLE_TAG, "Received AWS private key data");
				parse_aws_certificate((char*)param->write.value, param->write.len, &strAwsCertificate,EEPROM_AWS_KEY_ADDR, true);
				  //  read_and_print_eeprom_section("AWS Private Key", EEPROM_AWS_KEY_ADDR, 170);
			} 
			else if(param->write.handle == profile->u16AwsEndpointCharHandle) 
			{
				ESP_LOGI(BLE_TAG, "Received AWS endpoint data");
				parse_aws_certificate((char*)param->write.value, param->write.len, &strAwsCertificate,EEPROM_AWS_ENDPOINT_ADDR, false);
				   // read_and_print_eeprom_section("AWS Endpoint", EEPROM_AWS_ENDPOINT_ADDR, 170);
			}
			else if(param->write.handle == profile->u16WifiCharHandle)
			{
				if(param->write.len <= sizeof(strWifiCredentials))
				{
					memcpy(&strWifiCredentials, param->write.value, param->write.len);
					ESP_LOGI(BLE_TAG, "Received WiFi credentials");
					parse_wifi_credentials((char*)param->write.value, param->write.len,
							&strWifiCredentials);
				}
			}

			esp_gatt_rsp_t rsp = {0};
			rsp.attr_value.handle = param->write.handle;
			rsp.attr_value.len = 1;
			rsp.attr_value.value[0] = 0x01;  // Simple ACK

			// Send response with minimal delay
			esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, ESP_GATT_OK, &rsp);
			ESP_LOGI(BLE_TAG, "Sent write response");

			break;

		case ESP_GATTS_CONNECT_EVT:
			ESP_LOGI(BLE_TAG, "Connection established, u16ConnId %d", param->connect.conn_id);
			profile->u16ConnId = param->connect.conn_id;

			break;
		case ESP_GATTS_DISCONNECT_EVT:
			ESP_LOGI(BLE_TAG, "Disconnected, start advertising again");
			esp_ble_gap_start_advertising(&adv_params);
			break;
		case ESP_GATTS_MTU_EVT:
			ESP_LOGI("BLE", "Negotiated MTU: %d", param->mtu.mtu);
			break;
		default:
			break;
	}
}


/*******************************************************************************
 * Function      : gatts_event_handler
 *
 * Description   : This is the main GATT server event handler. It handles BLE
 *                 GATT registration events and delegates all other events to
 *                 the appropriate profile's callback function from the profile
 *                 table.
 *
 * Parameters    :
 *      - event      : GATT server callback event type.
 *      - gatts_if   : GATT server interface, provided by the stack.
 *      - param      : Pointer to the GATT server callback parameters.
 *
 * Returns       : void
 *
 *******************************************************************************/
static void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, 
		esp_ble_gatts_cb_param_t *param) 
{
	if (event == ESP_GATTS_REG_EVT) 
	{
		if (param->reg.status == ESP_GATT_OK) 
		{
			/* Store the GATT interface for the registered application */
			gl_profile_tab[param->reg.app_id].u16GattsIf = gatts_if;
		} 
		else 
		{
			ESP_LOGI(BLE_TAG, "Registration failed, u16AppId %04x, status %d", param->reg.app_id, 
					param->reg.status);
			return;
		}
	}

	/* Currently handling only one profile (index 0) */
	uint8_t u8Index;
	for (u8Index = 0; u8Index < 1; u8Index++) 
	{
		if(gatts_if == ESP_GATT_IF_NONE || gatts_if == gl_profile_tab[u8Index].u16GattsIf) 
		{
			if (gl_profile_tab[u8Index].gatts_cb) 
			{
				gl_profile_tab[u8Index].gatts_cb(event, gatts_if, param);
			}
		}
	}
}

/** BLE Server Setup **/
/*******************************************************************************
 * Function      : ble_init
 *
 * Description   : This function initializes the BLE stack, configures the
 *                 Bluetooth controller, sets the MTU size, and registers GATT
 *                 and GAP event handlers. It also initializes BLE profiles and
 *                 application-specific global variables related to BLE operations.
 *
 * Parameters    : None
 *
 * Returns       : void
 *
 *******************************************************************************/
void ble_init(void) 
{

	ESP_LOGI(BLE_TAG, "Initializing BLE...");
	ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

	/* Check if BT controller is already enabled */
	if (esp_bt_controller_get_status() == ESP_BT_CONTROLLER_STATUS_ENABLED) 
	{
		ESP_LOGW(BLE_TAG, "Bluetooth controller is already enabled, skipping memory release.");
	} 
	else 
	{
		//ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));
	}

	/* Default Bluetooth controller configuration */
	esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();

	/* Initialize BT controller */
	esp_err_t eErrStat = esp_bt_controller_init(&bt_cfg);
	if (eErrStat) 
	{
		ESP_LOGE(BLE_TAG, "%s initialize controller failed: %s", __func__, esp_err_to_name(eErrStat));
		return;
	}

	/* Enable BLE mode */
	eErrStat = esp_bt_controller_enable(ESP_BT_MODE_BLE);
	if (eErrStat) 
	{
		ESP_LOGE(BLE_TAG, "%s enable controller failed: %s", __func__, esp_err_to_name(eErrStat));
		return;
	}

	/* Initialize Bluedroid stack */
	eErrStat = esp_bluedroid_init();
	if (eErrStat) 
	{
		ESP_LOGE(BLE_TAG, "%s init bluetooth failed: %s", __func__, esp_err_to_name(eErrStat));
		return;
	}

	/* Enable Bluedroid stack */
	eErrStat = esp_bluedroid_enable();
	if (eErrStat) 
	{
		ESP_LOGE(BLE_TAG, "%s enable bluetooth failed: %s", __func__, esp_err_to_name(eErrStat));
		return;
	}

	/* Set BLE MTU size */
	eErrStat = esp_ble_gatt_set_local_mtu(512);
	if(eErrStat != ESP_OK)
	{
		ESP_LOGE("BLE", "Failed to set MTU size");
	}

	/* Register GATT and GAP event callbacks */
	eErrStat = esp_ble_gatts_register_callback(gatts_event_handler);
	if (eErrStat) 
	{
		ESP_LOGE(BLE_TAG, "gatts register error, error code = %x", eErrStat);
		return;
	}

	eErrStat = esp_ble_gap_register_callback(gap_event_handler);
	if (eErrStat) 
	{
		ESP_LOGE(BLE_TAG, "gap register error, error code = %x", eErrStat);
		return;
	}

	/* Register the BLE application profile */
	eErrStat = esp_ble_gatts_app_register(CONFIG_APP_ID);
	if (eErrStat) 
	{
		ESP_LOGE(BLE_TAG, "gatts app register error, error code = %x", eErrStat);
		return;
	}
#if CONFIG_PM_ENABLE
	/* Configure power management settings if enabled */
	esp_pm_config_esp32_t pm_config = 
	{
		.max_freq_mhz = 240,
		.min_freq_mhz = 240,
		.light_sleep_enable = false
	};
	esp_pm_configure(&pm_config);
#endif

	/* Initialize BLE-related global structures */
	memset(&strWifiCredentials, 0, sizeof(STR_WIFI_CREDENTIALS));
	memset(&strLocation, 0, sizeof(STR_LOCATION));
	memset(&strVersionInfo, 0, sizeof(STR_VERSION_INFO));
	memset(&strAwsCertificate, 0, sizeof(STR_AWS_CERTIFICATE));

	/* Assign profile-specific callback handlers */
	gl_profile_tab[CONFIG_APP_ID].gatts_cb = gatts_profile_event_handler;
	gl_profile_tab[CONFIG_APP_ID].u16GattsIf = ESP_GATT_IF_NONE;

	ESP_LOGI(BLE_TAG, "BLE Configuration service initialized");
}

