#include "ble.h"

static const char *TAG = "BLE_CONFIG";

static uint8_t service_uuid[16] = {
	/* LSB <--------------------------------------------------------------------------------> MSB */
	0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
};

static uint8_t wifi_char_uuid[16] = {
	/* LSB <--------------------------------------------------------------------------------> MSB */
	0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x01, 0xFF, 0x00, 0x00,
};

static uint8_t location_char_uuid[16] = {
	/* LSB <--------------------------------------------------------------------------------> MSB */
	0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x02, 0xFF, 0x00, 0x00,
};

static uint8_t hw_version_char_uuid[16] = {
	/* LSB <--------------------------------------------------------------------------------> MSB */
	0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x03, 0xFF, 0x00, 0x00,
};

static uint8_t sw_version_char_uuid[16] = {
	/* LSB <--------------------------------------------------------------------------------> MSB */
	0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x04, 0xFF, 0x00, 0x00,
};

static uint8_t adv_service_uuid128[32] = {
	/* LSB <--------------------------------------------------------------------------------> MSB */
	0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
};

static uint8_t aws_cert_char_uuid[16] = {
	/* LSB <--------------------------------------------------------------------------------> MSB */
	0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x05, 0xFF, 0x00, 0x00,
};

static uint8_t aws_private_key_char_uuid[16] = {
	/* LSB <--------------------------------------------------------------------------------> MSB */
	0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x06, 0xFF, 0x00, 0x00,
};

static uint8_t aws_endpoint_char_uuid[16] = {
	/* LSB <--------------------------------------------------------------------------------> MSB */
	0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x07, 0xFF, 0x00, 0x00,
};



static esp_ble_adv_data_t adv_data = {
	.set_scan_rsp = false,
	.include_name = true,
	.include_txpower = true,
	.min_interval = 0x20,
	.max_interval = 0x40,
	.appearance = 0x00,
	.manufacturer_len = 0,
	.p_manufacturer_data = NULL,
	.service_data_len = 0,
	.p_service_data = NULL,
	.service_uuid_len = sizeof(adv_service_uuid128),
	.p_service_uuid = adv_service_uuid128,
	.flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};

static esp_ble_adv_params_t adv_params = {
	.adv_int_min = 0x40,
	.adv_int_max = 0x40,
	.adv_type = ADV_TYPE_IND,
	.own_addr_type = BLE_ADDR_TYPE_PUBLIC,
	.channel_map = ADV_CHNL_ALL,
	.adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

struct gatts_profile_inst {
	esp_gatts_cb_t gatts_cb;
	uint16_t gatts_if;
	uint16_t app_id;
	uint16_t conn_id;
	uint16_t service_handle;
	esp_gatt_srvc_id_t service_id;
	uint16_t wifi_char_handle;
	uint16_t location_char_handle;
	uint16_t hw_version_char_handle;
	uint16_t sw_version_char_handle;
	uint16_t aws_cert_char_handle;
	uint16_t aws_private_key_char_handle;
	uint16_t aws_endpoint_char_handle;
};

typedef struct {
	char ssid[32];
	char password[64];
} wifi_credentials_t;

typedef struct {
	double latitude;
	double longitude;
} location_t;

typedef struct {
	char hw_version[32];
	char sw_version[32];
} version_info_t;

// Add AWS certificate structs
typedef struct {
	char certificate[2048];  // Adjust size as needed
} aws_certificate_t;

typedef struct {
	char private_key[2048];  // Adjust size as needed
} aws_private_key_t;

typedef struct {
	char endpoint[128];  // AWS IoT endpoint
} aws_endpoint_t;

static wifi_credentials_t wifi_credentials;
static location_t location;
static version_info_t version_info;
static aws_certificate_t aws_certificate;
static aws_private_key_t aws_private_key;
static aws_endpoint_t aws_endpoint;

bool wifi_configured = false;
static struct gatts_profile_inst gl_profile_tab[1];

/* Add parsing functions for AWS certificates */
static void parse_aws_certificate(const char* data, size_t len, aws_certificate_t* cert) 
{
	if(len >= sizeof(cert->certificate)) 
	{
		ESP_LOGE(TAG, "Certificate too large");
		return;
	}
	
	memcpy(cert->certificate, data, len);
	cert->certificate[len] = '\0';
	
	ESP_LOGI(TAG, "Parsed AWS certificate (length: %d)", len);
	
	// Save to EEPROM or flash
	eeprom_erase(EEPROM_AWS_CERT_ADDR);
	eeprom_write(EEPROM_AWS_CERT_ADDR, (uint8_t *)cert->certificate, len + 1);
	
	ESP_LOGI(TAG, "AWS certificate stored");
}

static void parse_aws_private_key(const char* data, size_t len, aws_private_key_t* key) 
{
	if(len >= sizeof(key->private_key)) 
	{
		ESP_LOGE(TAG, "Private key too large");
		return;
	}
	
	memcpy(key->private_key, data, len);
	key->private_key[len] = '\0';
	
	ESP_LOGI(TAG, "Parsed AWS private key (length: %d)", len);
	
	// Save to EEPROM or flash
	eeprom_erase(EEPROM_AWS_KEY_ADDR);
	eeprom_write(EEPROM_AWS_KEY_ADDR, (uint8_t *)key->private_key, len + 1);
	
	ESP_LOGI(TAG, "AWS private key stored");
}

static void parse_aws_endpoint(const char* data, size_t len, aws_endpoint_t* endpoint) 
{
	if(len >= sizeof(endpoint->endpoint)) 
	{
		ESP_LOGE(TAG, "Endpoint too large");
		return;
	}
	
	memcpy(endpoint->endpoint, data, len);
	endpoint->endpoint[len] = '\0';
	
	ESP_LOGI(TAG, "Parsed AWS endpoint: %s", endpoint->endpoint);
	
	// Save to EEPROM or flash
	eeprom_erase(EEPROM_AWS_ENDPOINT_ADDR);
	eeprom_write(EEPROM_AWS_ENDPOINT_ADDR, (uint8_t *)endpoint->endpoint, len + 1);
	
	ESP_LOGI(TAG, "AWS endpoint stored");
}
/* Add these parsing functions to your code */
static void parse_wifi_credentials(const char* data, size_t len, wifi_credentials_t* credentials) 
{
	char buffer[100] = {0};
	if(len >= sizeof(buffer)) 
	{
		ESP_LOGE(TAG, "Received data too large");
		return;
	}
	ESP_LOGI(TAG, "Raw Received WiFi Data: '%s' (Length: %d)", data, len);

	/* Copy and ensure null termination */
	memcpy(buffer, data, len);
	buffer[len] = '\0';

	/* Find delimiter */
	char *delimiter = strchr(buffer, ',');
	if(!delimiter) 
	{
		ESP_LOGE(TAG, "Invalid format: missing delimiter");
		return;
	}

	/* Split into SSID and password */
	*delimiter = '\0';
	char *ssid = buffer;
	char *password = delimiter + 1;

	/* Copy to wifi_credentials struct */
	memset(credentials, 0, sizeof(wifi_credentials_t));
	strncpy(credentials->ssid, ssid, sizeof(credentials->ssid) - 1);
	strncpy(credentials->password, password, sizeof(credentials->password) - 1);

	ESP_LOGI(TAG, "Parsed WiFi credentials - SSID: %s, PASS: %s", credentials->ssid, credentials->password);
	
	ESP_LOGI(TAG, "Password: ****** (hidden for security)");
	
	eeprom_erase(EEPROM_WIFI_SSID_ADDR);
	eeprom_erase(EEPROM_WIFI_PASS_ADDR);
	
	eeprom_write(EEPROM_WIFI_SSID_ADDR, (uint8_t *)ssid, strlen(ssid) + 1);
	
	eeprom_write(EEPROM_WIFI_PASS_ADDR, (uint8_t *)credentials->password, strlen(password) + 1);
	
	printf("Written Wifi Credential to EEPROM\n");
	
	/* Global variable for wifi credential settings */
	wifi_configured = true;
	xEventGroupSetBits(xEventGroup, BLE_WIFI_CONNECT_BIT);

//	wifi_init(credentials->ssid, credentials->password);
}
static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) 
{
	switch(event) 
	{
		case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
			esp_ble_gap_start_advertising(&adv_params);
			break;
		case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
			if(param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS) 
			{
				ESP_LOGE(TAG, "Advertising start failed");
			} 
			else 
			{
				ESP_LOGI(TAG, "Advertising started");
			}
			break;
		case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
			if(param->adv_stop_cmpl.status != ESP_BT_STATUS_SUCCESS) 
			{
				ESP_LOGE(TAG, "Advertising stop failed");
			} 
			else 
			{
				ESP_LOGI(TAG, "Advertising stopped");
			}
			break;
		default:
			break;
	}
}

static void parse_location(const char* data, size_t len, location_t* loc) 
{
	char buffer[100] = {0};

	if(len >= sizeof(buffer)) 
	{
		ESP_LOGE(TAG, "Received data too large");
		return;
	}

	/* Copy and ensure null termination */
	memcpy(buffer, data, len);
	buffer[len] = '\0';

	/* Find delimiter */
	char *delimiter = strchr(buffer, ',');
	if(!delimiter) 
	{
		ESP_LOGE(TAG, "Invalid format: missing delimiter");
		return;
	}

	/* Split into latitude and longitude */
	*delimiter = '\0';
	char *lat_str = buffer;
	char *lng_str = delimiter + 1;

	/* Convert to double */
	loc->latitude = atof(lat_str);
	loc->longitude = atof(lng_str);

	ESP_LOGI(TAG, "Parsed location - Lat: %f, Lng: %f", loc->latitude, loc->longitude);
}


static void apply_version_config(version_info_t *ver) 
{
	ESP_LOGI(TAG, "Applying Version information:");
	ESP_LOGI(TAG, "Hardware Version: %s", ver->hw_version);
	ESP_LOGI(TAG, "Software Version: %s", ver->sw_version);
}

static void gatts_profile_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, 
		esp_ble_gatts_cb_param_t *param) 
{
	struct gatts_profile_inst *profile = &gl_profile_tab[CONFIG_APP_ID];

	switch (event) 
	{
		case ESP_GATTS_REG_EVT:
			ESP_LOGI(TAG, "REGISTER_APP_EVT, status %d, app_id %d", param->reg.status, param->reg.app_id);
			profile->service_id.is_primary = true;
			profile->service_id.id.inst_id = 0x00;
			profile->service_id.id.uuid.len = ESP_UUID_LEN_128;
			memcpy(profile->service_id.id.uuid.uuid.uuid128, service_uuid, ESP_UUID_LEN_128);

			esp_ble_gap_set_device_name(DEVICE_NAME);
			esp_ble_gap_config_adv_data(&adv_data);

			esp_ble_gatts_create_service(gatts_if, &profile->service_id, 10);
			break;
		case ESP_GATTS_CREATE_EVT:
			ESP_LOGI(TAG, "CREATE_SERVICE_EVT, status %d,  service_handle %d", 
					param->create.status, param->create.service_handle);
			profile->service_handle = param->create.service_handle;

			/* Create WiFi credentials characteristic */
			esp_bt_uuid_t wifi_uuid;
			wifi_uuid.len = ESP_UUID_LEN_128;
			memcpy(wifi_uuid.uuid.uuid128, wifi_char_uuid, ESP_UUID_LEN_128);

			esp_ble_gatts_add_char(profile->service_handle, &wifi_uuid,
					ESP_GATT_PERM_WRITE,
					ESP_GATT_CHAR_PROP_BIT_WRITE,
					NULL, NULL);

			/* Create location characteristic */
			esp_bt_uuid_t location_uuid;
			location_uuid.len = ESP_UUID_LEN_128;
			memcpy(location_uuid.uuid.uuid128, location_char_uuid, ESP_UUID_LEN_128);

			esp_ble_gatts_add_char(profile->service_handle, &location_uuid,
					ESP_GATT_PERM_WRITE,
					ESP_GATT_CHAR_PROP_BIT_WRITE,
					NULL, NULL);

			/* Create HW version characteristic */
			esp_bt_uuid_t hw_version_uuid;
			hw_version_uuid.len = ESP_UUID_LEN_128;
			memcpy(hw_version_uuid.uuid.uuid128, hw_version_char_uuid, ESP_UUID_LEN_128);

			esp_ble_gatts_add_char(profile->service_handle, &hw_version_uuid,
					ESP_GATT_PERM_WRITE,
					ESP_GATT_CHAR_PROP_BIT_WRITE,
					NULL, NULL);

			/* Create SW version characteristic */
			esp_bt_uuid_t sw_version_uuid;
			sw_version_uuid.len = ESP_UUID_LEN_128;
			memcpy(sw_version_uuid.uuid.uuid128, sw_version_char_uuid, ESP_UUID_LEN_128);

			esp_ble_gatts_add_char(profile->service_handle, &sw_version_uuid,
					ESP_GATT_PERM_WRITE,
					ESP_GATT_CHAR_PROP_BIT_WRITE,
					NULL, NULL);
					
			/* Create AWS certificate characteristic */
			esp_bt_uuid_t aws_cert_uuid;
			aws_cert_uuid.len = ESP_UUID_LEN_128;
			memcpy(aws_cert_uuid.uuid.uuid128, aws_cert_char_uuid, ESP_UUID_LEN_128);

			esp_ble_gatts_add_char(profile->service_handle, &aws_cert_uuid,
					ESP_GATT_PERM_WRITE,
					ESP_GATT_CHAR_PROP_BIT_WRITE,
					NULL, NULL);

			/* Create AWS private key characteristic */
			esp_bt_uuid_t aws_private_key_uuid;
			aws_private_key_uuid.len = ESP_UUID_LEN_128;
			memcpy(aws_private_key_uuid.uuid.uuid128, aws_private_key_char_uuid, ESP_UUID_LEN_128);

			esp_ble_gatts_add_char(profile->service_handle, &aws_private_key_uuid,
					ESP_GATT_PERM_WRITE,
					ESP_GATT_CHAR_PROP_BIT_WRITE,
					NULL, NULL);

			/* Create AWS endpoint characteristic */
			esp_bt_uuid_t aws_endpoint_uuid;
			aws_endpoint_uuid.len = ESP_UUID_LEN_128;
			memcpy(aws_endpoint_uuid.uuid.uuid128, aws_endpoint_char_uuid, ESP_UUID_LEN_128);

			esp_ble_gatts_add_char(profile->service_handle, &aws_endpoint_uuid,
					ESP_GATT_PERM_WRITE,
					ESP_GATT_CHAR_PROP_BIT_WRITE,
					NULL, NULL);		
					
			

			esp_ble_gatts_start_service(profile->service_handle);
			break;
		case ESP_GATTS_ADD_CHAR_EVT:
			ESP_LOGI(TAG, "ADD_CHAR_EVT, status %d, attr_handle %d, service_handle %d",
					param->add_char.status, param->add_char.attr_handle, 
					param->add_char.service_handle);

			/* Assign handles to respective characteristics */
			if(memcmp(param->add_char.char_uuid.uuid.uuid128, wifi_char_uuid, 16) == 0) 
			{
				profile->wifi_char_handle = param->add_char.attr_handle;
				ESP_LOGI(TAG, "WiFi characteristic added");
			} 
			else if(memcmp(param->add_char.char_uuid.uuid.uuid128, location_char_uuid, 16) == 0) 
			{
				profile->location_char_handle = param->add_char.attr_handle;
				ESP_LOGI(TAG, "Location characteristic added");
			} 
			else if (memcmp(param->add_char.char_uuid.uuid.uuid128, hw_version_char_uuid, 16) == 0) 
			{
				profile->hw_version_char_handle = param->add_char.attr_handle;
				ESP_LOGI(TAG, "HW Version characteristic added");
			} 
			else if (memcmp(param->add_char.char_uuid.uuid.uuid128, sw_version_char_uuid, 16) == 0) 
			{
				profile->sw_version_char_handle = param->add_char.attr_handle;
				ESP_LOGI(TAG, "SW Version characteristic added");
			}
			else if(memcmp(param->add_char.char_uuid.uuid.uuid128, aws_cert_char_uuid, 16) == 0) 
			{
				profile->aws_cert_char_handle = param->add_char.attr_handle;
				ESP_LOGI(TAG, "AWS certificate characteristic added");
			} 
			else if(memcmp(param->add_char.char_uuid.uuid.uuid128, aws_private_key_char_uuid, 16) == 0) 
			{
				profile->aws_private_key_char_handle = param->add_char.attr_handle;
				ESP_LOGI(TAG, "AWS private key characteristic added");
			} 
			else if(memcmp(param->add_char.char_uuid.uuid.uuid128, aws_endpoint_char_uuid, 16) == 0) 
			{
				profile->aws_endpoint_char_handle = param->add_char.attr_handle;
				ESP_LOGI(TAG, "AWS endpoint characteristic added");
			}
			break;
		case ESP_GATTS_WRITE_EVT:
			ESP_LOGI(TAG, "WRITE_EVT, handle %d, value_len %d", param->write.handle, param->write.len);

			if(param->write.handle == profile->wifi_char_handle) 
			{
				if(param->write.len <= sizeof(wifi_credentials_t)) 
				{
					memcpy(&wifi_credentials, param->write.value, param->write.len);
					ESP_LOGI(TAG, "Received WiFi credentials");
					parse_wifi_credentials((char*)param->write.value, param->write.len, 
							&wifi_credentials);
				}
			} 
			else if(param->write.handle == profile->location_char_handle) 
			{
				if(param->write.len <= sizeof(location_t)) 
				{
					memcpy(&location, param->write.value, param->write.len);
					ESP_LOGI(TAG, "Received location data");
					parse_location((char*)param->write.value, param->write.len, &location);
				}
			} 
			else if (param->write.handle == profile->hw_version_char_handle) 
			{
				if(param->write.len < sizeof(version_info.hw_version)) 
				{
					memcpy(version_info.hw_version, param->write.value, param->write.len);
					version_info.hw_version[param->write.len] = '\0';
					ESP_LOGI(TAG, "Received HW version: %s", version_info.hw_version);
					apply_version_config(&version_info);
				}
			} 
			else if (param->write.handle == profile->sw_version_char_handle) 
			{
				if (param->write.len < sizeof(version_info.sw_version)) 
				{
					memcpy(version_info.sw_version, param->write.value, param->write.len);
					version_info.sw_version[param->write.len] = '\0';
					ESP_LOGI(TAG, "Received SW version: %s", version_info.sw_version);
					apply_version_config(&version_info);
				}
			}
			else if(param->write.handle == profile->aws_cert_char_handle) 
			{
				ESP_LOGI(TAG, "Received AWS certificate data");
				parse_aws_certificate((char*)param->write.value, param->write.len, &aws_certificate);
			} 
			else if(param->write.handle == profile->aws_private_key_char_handle) 
			{
				ESP_LOGI(TAG, "Received AWS private key data");
				parse_aws_private_key((char*)param->write.value, param->write.len, &aws_private_key);
			} 
			else if(param->write.handle == profile->aws_endpoint_char_handle) 
			{
				ESP_LOGI(TAG, "Received AWS endpoint data");
				parse_aws_endpoint((char*)param->write.value, param->write.len, &aws_endpoint);
			}
			
			
			/* Create a proper response to client */
			esp_gatt_rsp_t rsp;
			memset(&rsp, 0, sizeof(esp_gatt_rsp_t));
			rsp.attr_value.handle = param->write.handle;
			rsp.attr_value.len = param->write.len;
			esp_ble_gatts_send_response(gatts_if, param->write.conn_id, 
					param->write.trans_id, ESP_GATT_OK, &rsp);
			break;
		case ESP_GATTS_CONNECT_EVT:
			ESP_LOGI(TAG, "Connection established, conn_id %d", param->connect.conn_id);
			profile->conn_id = param->connect.conn_id;
			break;
		case ESP_GATTS_DISCONNECT_EVT:
			ESP_LOGI(TAG, "Disconnected, start advertising again");
			esp_ble_gap_start_advertising(&adv_params);
			break;
		case ESP_GATTS_MTU_EVT:
    			ESP_LOGI("BLE", "Negotiated MTU: %d", param->mtu.mtu);
    			break;
		default:
			break;
	}
}

static void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, 
		esp_ble_gatts_cb_param_t *param) 
{
	if (event == ESP_GATTS_REG_EVT) 
	{
		if (param->reg.status == ESP_GATT_OK) 
		{
			gl_profile_tab[param->reg.app_id].gatts_if = gatts_if;
		} 
		else 
		{
			ESP_LOGI(TAG, "Registration failed, app_id %04x, status %d", param->reg.app_id, 
					param->reg.status);
			return;
		}
	}

	int idx;
	for (idx = 0; idx < 1; idx++) 
	{
		if(gatts_if == ESP_GATT_IF_NONE || gatts_if == gl_profile_tab[idx].gatts_if) 
		{
			if (gl_profile_tab[idx].gatts_cb) 
			{
				gl_profile_tab[idx].gatts_cb(event, gatts_if, param);
			}
		}
	}
}
/** BLE Server Setup **/
void ble_init(void) 
{

	ESP_LOGI(BLE_TAG, "Initializing BLE...");

	/* Initialize the Bluetooth controller */
	if (esp_bt_controller_get_status() == ESP_BT_CONTROLLER_STATUS_ENABLED) 
	{
		ESP_LOGW(BLE_TAG, "Bluetooth controller is already enabled, skipping memory release.");
	} 
	else 
	{
		ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));
	}

	esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();


	esp_err_t ret = esp_bt_controller_init(&bt_cfg);
	if (ret) 
	{
		ESP_LOGE(TAG, "%s initialize controller failed: %s", __func__, esp_err_to_name(ret));
		return;
	}

	ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
	if (ret) 
	{
		ESP_LOGE(TAG, "%s enable controller failed: %s", __func__, esp_err_to_name(ret));
		return;
	}

	ret = esp_bluedroid_init();
	if (ret) 
	{
		ESP_LOGE(TAG, "%s init bluetooth failed: %s", __func__, esp_err_to_name(ret));
		return;
	}

	ret = esp_bluedroid_enable();
	if (ret) 
	{
		ESP_LOGE(TAG, "%s enable bluetooth failed: %s", __func__, esp_err_to_name(ret));
		return;
	}

	/*  Set MTU size */
        ret = esp_ble_gatt_set_local_mtu(512);
        if(ret != ESP_OK)
        {
                ESP_LOGE("BLE", "Failed to set MTU size");
        }

	/* Initialize BLE GATT server */
	ret = esp_ble_gatts_register_callback(gatts_event_handler);
	if (ret) 
	{
		ESP_LOGE(TAG, "gatts register error, error code = %x", ret);
		return;
	}

	ret = esp_ble_gap_register_callback(gap_event_handler);
	if (ret) 
	{
		ESP_LOGE(TAG, "gap register error, error code = %x", ret);
		return;
	}

	/* Register the profile */
	ret = esp_ble_gatts_app_register(CONFIG_APP_ID);
	if (ret) 
	{
		ESP_LOGE(TAG, "gatts app register error, error code = %x", ret);
		return;
	}

	/* Initialize global variables */
	memset(&wifi_credentials, 0, sizeof(wifi_credentials_t));
	memset(&location, 0, sizeof(location_t));
	memset(&version_info, 0, sizeof(version_info_t));
	memset(&aws_certificate, 0, sizeof(aws_certificate_t));
	memset(&aws_private_key, 0, sizeof(aws_private_key_t));
	memset(&aws_endpoint, 0, sizeof(aws_endpoint_t));

	/* Set the callback for the GATT profile */
	gl_profile_tab[CONFIG_APP_ID].gatts_cb = gatts_profile_event_handler;
	gl_profile_tab[CONFIG_APP_ID].gatts_if = ESP_GATT_IF_NONE;

	ESP_LOGI(TAG, "BLE Configuration service initialized");
}





