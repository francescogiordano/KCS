#include "ble_phy.h"
#include <string.h>
#include "nordic_common.h"
#include "ble_srv_common.h"
#include "app_util.h"
#include "nrf_gpio.h"
#include "delay.h"
#include "adc.h"
#include "usart.h"
#include "sensors.h"

#include "mlx90393.h"

volatile uint8_t g_ble_conn = 0;

extern int m_send_packet;

uint32_t initServiceSensors(ble_pss_t * p_pss, const ble_pss_init_t * p_pss_init) {	// init BLE Sensors Service
    uint32_t   err_code;
    ble_uuid_t ble_uuid;

	// initialize service structure
	p_pss->evt_handler = p_pss_init->evt_handler;
	p_pss->conn_handle = BLE_CONN_HANDLE_INVALID;
	p_pss->is_notification_supported = p_pss_init->support_notification;

	//Add Vendor UUID
	ble_uuid128_t base_uuid = { BLE_UUID_VENDOR };
	err_code = sd_ble_uuid_vs_add(&base_uuid, &p_pss->uuid_type);
	if (err_code != NRF_SUCCESS) {
		return err_code;
	}

	//Add Service UUID

	//ble_uuid.type = p_pss->uuid_type;
	ble_uuid.uuid = BLE_UUID_SERVICE_SENSORS;
	ble_uuid.type = BLE_UUID_TYPE_BLE;

	err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_pss->service_handle);
    if (err_code != NRF_SUCCESS) {
        return err_code;
    }

    // Add  Service Sensors Characteristics
	err_code = addCharSensorsData(p_pss, p_pss_init);
	err_code = addCharSensorsDataNotify(p_pss, p_pss_init);
	err_code = addCharDiagInfo(p_pss, p_pss_init);
	err_code = addCharFirmVer(p_pss, p_pss_init);
	err_code = addCharRockNum(p_pss, p_pss_init);
	err_code = addCharTxPowerLevel(p_pss, p_pss_init);

	return err_code;
}

static uint32_t addCharSensorsData(ble_pss_t * p_pss, const ble_pss_init_t * p_pss_init) {	// add Sensors characteristics
	ble_uuid_t          ble_uuid;
	ble_gatts_char_md_t char_md;
	ble_gatts_attr_md_t attr_md;
	//ble_gatts_attr_md_t p_user_desc_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;


    char user_desc[] = "Sensor Data";

	memset(&cccd_md, 0, sizeof(cccd_md));
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
	cccd_md.vloc = BLE_GATTS_VLOC_STACK;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read			= 1;
    char_md.char_props.notify		= 1;
	char_md.p_char_user_desc		= (uint8_t *)user_desc;
	char_md.char_user_desc_size		= strlen(user_desc);
	char_md.char_user_desc_max_size = strlen(user_desc);
    char_md.p_char_pf				= NULL;
    char_md.p_user_desc_md			= NULL;
    char_md.p_cccd_md				= &cccd_md;
    char_md.p_sccd_md				= NULL;


    ble_uuid.type = p_pss->uuid_type;
    ble_uuid.uuid = BLE_UUID_CHAR_SENSORS_DATA;

    memset(&attr_md, 0, sizeof(attr_md));
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&attr_md.write_perm);
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

	uint8_t value[CHAR_SENSORS_DATA_VER_LEN];
    memset(&attr_char_value, 0, sizeof(attr_char_value));
    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = (CHAR_SENSORS_DATA_VER_LEN);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = (CHAR_SENSORS_DATA_VER_LEN);
    attr_char_value.p_value      = value;

    return sd_ble_gatts_characteristic_add(p_pss->service_handle, &char_md, &attr_char_value, &p_pss->char_sensor_data_handle);
}
static uint32_t addCharSensorsDataNotify(ble_pss_t * p_pss, const ble_pss_init_t * p_pss_init) {	// add Sensors Write characteristics

	ble_uuid_t          ble_uuid;
	ble_gatts_char_md_t char_md;
	ble_gatts_attr_md_t attr_md;
	//ble_gatts_attr_md_t p_user_desc_md;
	ble_gatts_attr_md_t cccd_md;
	ble_gatts_attr_t    attr_char_value;

	char user_desc[] = "Sensor Data Notify";

	memset(&cccd_md, 0, sizeof(cccd_md));
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
	cccd_md.vloc = BLE_GATTS_VLOC_STACK;

	memset(&char_md, 0, sizeof(char_md));

	char_md.char_props.read			= 1;
	char_md.char_props.write		= 1;
	char_md.p_char_user_desc		= (uint8_t *)user_desc;
	char_md.char_user_desc_size		= strlen(user_desc);
	char_md.char_user_desc_max_size = strlen(user_desc);
	char_md.p_char_pf				= NULL;
	char_md.p_user_desc_md			= NULL;
	char_md.p_cccd_md				= &cccd_md;
	char_md.p_sccd_md				= NULL;


	ble_uuid.type = p_pss->uuid_type;
	ble_uuid.uuid = BLE_UUID_CHAR_SENSORS_DATA_NOTIFY;

	memset(&attr_md, 0, sizeof(attr_md));
	BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&attr_md.read_perm);
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
	attr_md.vloc	= BLE_GATTS_VLOC_STACK;
	attr_md.rd_auth = 0;
	attr_md.wr_auth = 0;
	attr_md.vlen	= 0;

	memset(&attr_char_value, 0, sizeof(attr_char_value));
	attr_char_value.p_uuid		= &ble_uuid;
	attr_char_value.p_attr_md	= &attr_md;
	attr_char_value.init_len	= (CHAR_SENSORS_DATA_VER_LEN);
	attr_char_value.init_offs	= 0;
	attr_char_value.max_len		= (CHAR_SENSORS_DATA_VER_LEN);
	attr_char_value.p_value		= NULL;

	return sd_ble_gatts_characteristic_add(p_pss->service_handle, &char_md, &attr_char_value, &p_pss->char_sensor_write_handle);
}
static uint32_t addCharDiagInfo(ble_pss_t * p_pss, const ble_pss_init_t * p_pss_init) {
	
	// add Diag characteristics
	ble_uuid_t          ble_uuid;
	ble_gatts_char_md_t char_md;
	ble_gatts_attr_md_t attr_md;
	//ble_gatts_attr_md_t p_user_desc_md;
	ble_gatts_attr_md_t cccd_md;
	ble_gatts_attr_t    attr_char_value;

	char user_desc[] = "Diagnostic Information";

	memset(&cccd_md, 0, sizeof(cccd_md));
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
	cccd_md.vloc = BLE_GATTS_VLOC_STACK;

	memset(&char_md, 0, sizeof(char_md));
	char_md.char_props.read = 1;
	//char_md.char_props.notify = 1;
	char_md.p_char_user_desc = (uint8_t *)user_desc;
	char_md.char_user_desc_size = strlen(user_desc);
	char_md.char_user_desc_max_size = strlen(user_desc);
	char_md.p_char_pf = NULL;
	char_md.p_user_desc_md = NULL;
	char_md.p_cccd_md = &cccd_md;
	char_md.p_sccd_md = NULL;

	ble_uuid.type = p_pss->uuid_type;
	ble_uuid.uuid = BLE_UUID_CHAR_DIAG_INFO;

	memset(&attr_md, 0, sizeof(attr_md));
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
	BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&attr_md.write_perm);
	attr_md.vloc = BLE_GATTS_VLOC_STACK;
	attr_md.rd_auth = 1;
	attr_md.wr_auth = 0;
	attr_md.vlen = 0;

	uint8_t value[CHAR_DIAG_INFO_LEN];
	memset(&attr_char_value, 0, sizeof(attr_char_value));
	attr_char_value.p_uuid			= &ble_uuid;
	attr_char_value.p_attr_md		= &attr_md;
	attr_char_value.init_len		= (CHAR_DIAG_INFO_LEN);
	attr_char_value.init_offs		= 0;
	attr_char_value.max_len			= (CHAR_DIAG_INFO_LEN);
	attr_char_value.p_value			= value;

	return sd_ble_gatts_characteristic_add(p_pss->service_handle, &char_md, &attr_char_value, &p_pss->char_diag_info_handle);
}
static uint32_t addCharFirmVer(ble_pss_t * p_pss, const ble_pss_init_t * p_pss_init) {	// add Diag characteristics
	ble_uuid_t          ble_uuid;
	ble_gatts_char_md_t char_md;
	ble_gatts_attr_md_t attr_md;
	//ble_gatts_attr_md_t p_user_desc_md;
	ble_gatts_attr_md_t cccd_md;
	ble_gatts_attr_t    attr_char_value;

	char user_desc[] = "Firmware Version";

	memset(&cccd_md, 0, sizeof(cccd_md));
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
	cccd_md.vloc = BLE_GATTS_VLOC_STACK;

	memset(&char_md, 0, sizeof(char_md));
	char_md.char_props.read = 1;
	//char_md.char_props.notify = 1;
	char_md.p_char_user_desc = (uint8_t *)user_desc;
	char_md.char_user_desc_size = strlen(user_desc);
	char_md.char_user_desc_max_size = strlen(user_desc);
	char_md.p_char_pf = NULL;
	char_md.p_user_desc_md = NULL;
	char_md.p_cccd_md = &cccd_md;
	char_md.p_sccd_md = NULL;

	ble_uuid.type = p_pss->uuid_type;
	ble_uuid.uuid = BLE_UUID_CHAR_FIRM_VER;

	memset(&attr_md, 0, sizeof(attr_md));
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
	BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&attr_md.write_perm);
	attr_md.vloc = BLE_GATTS_VLOC_STACK;
	attr_md.rd_auth = 1;
	attr_md.wr_auth = 0;
	attr_md.vlen = 0;

	uint8_t value[CHAR_FIRM_VER_LEN];
	memset(&attr_char_value, 0, sizeof(attr_char_value));
	attr_char_value.p_uuid = &ble_uuid;
	attr_char_value.p_attr_md = &attr_md;
	attr_char_value.init_len = (CHAR_FIRM_VER_LEN);
	attr_char_value.init_offs = 0;
	attr_char_value.max_len = (CHAR_FIRM_VER_LEN);
	attr_char_value.p_value = value;

	return sd_ble_gatts_characteristic_add(p_pss->service_handle, &char_md, &attr_char_value, &p_pss->char_firm_ver_handle);
}
static uint32_t addCharRockNum(ble_pss_t * p_pss, const ble_pss_init_t * p_pss_init) {
	
	ble_uuid_t          ble_uuid;
	ble_gatts_char_md_t char_md;
	ble_gatts_attr_md_t attr_md;
	//ble_gatts_attr_md_t p_user_desc_md;
	ble_gatts_attr_md_t cccd_md;
	ble_gatts_attr_t    attr_char_value;

	char user_desc[] = "Rock Number";

	memset(&cccd_md, 0, sizeof(cccd_md));
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
	cccd_md.vloc = BLE_GATTS_VLOC_STACK;

	memset(&char_md, 0, sizeof(char_md));
	char_md.char_props.read = 1;
	char_md.char_props.write = 1;
	char_md.p_char_user_desc = (uint8_t *)user_desc;
	char_md.char_user_desc_size = strlen(user_desc);
	char_md.char_user_desc_max_size = strlen(user_desc);
	char_md.p_char_pf = NULL;
	char_md.p_user_desc_md = NULL;
	char_md.p_cccd_md = &cccd_md;
	char_md.p_sccd_md = NULL;

	ble_uuid.type = p_pss->uuid_type;
	ble_uuid.uuid = BLE_UUID_CHAR_ROCK_NUMBER;

	memset(&attr_md, 0, sizeof(attr_md));
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
	attr_md.vloc = BLE_GATTS_VLOC_STACK;
	attr_md.rd_auth = 1;
	attr_md.wr_auth = 1;
	attr_md.vlen = 0;

	uint8_t value[CHAR_ROCK_NUM_LEN];
	memset(&attr_char_value, 0, sizeof(attr_char_value));
	attr_char_value.p_uuid = &ble_uuid;
	attr_char_value.p_attr_md = &attr_md;
	attr_char_value.init_len = (CHAR_ROCK_NUM_LEN);
	attr_char_value.init_offs = 0;
	attr_char_value.max_len = (CHAR_ROCK_NUM_LEN);
	attr_char_value.p_value = value;

	return sd_ble_gatts_characteristic_add(p_pss->service_handle, &char_md, &attr_char_value, &p_pss->char_rock_num_handle);
}
static uint32_t addCharTxPowerLevel(ble_pss_t * p_pss, const ble_pss_init_t * p_pss_init) {

	ble_uuid_t          ble_uuid;
	ble_gatts_char_md_t char_md;
	ble_gatts_attr_md_t attr_md;
	//ble_gatts_attr_md_t p_user_desc_md;
	ble_gatts_attr_md_t cccd_md;
	ble_gatts_attr_t    attr_char_value;

	char user_desc[] = "Tx Power Level";

	memset(&cccd_md, 0, sizeof(cccd_md));
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
	cccd_md.vloc = BLE_GATTS_VLOC_STACK;

	memset(&char_md, 0, sizeof(char_md));
	char_md.char_props.read = 1;
	char_md.char_props.write = 1;
	char_md.p_char_user_desc = (uint8_t *)user_desc;
	char_md.char_user_desc_size = strlen(user_desc);
	char_md.char_user_desc_max_size = strlen(user_desc);
	char_md.p_char_pf = NULL;
	char_md.p_user_desc_md = NULL;
	char_md.p_cccd_md = &cccd_md;
	char_md.p_sccd_md = NULL;

	ble_uuid.type = p_pss->uuid_type;
	ble_uuid.uuid = BLE_UUID_CHAR_TX_POWER_LEVEL;

	memset(&attr_md, 0, sizeof(attr_md));
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
	attr_md.vloc = BLE_GATTS_VLOC_STACK;
	attr_md.rd_auth = 1;
	attr_md.wr_auth = 1;
	attr_md.vlen = 0;

	uint8_t value[CHAR_TX_POWER_LEVEL_LEN];
	memset(&attr_char_value, 0, sizeof(attr_char_value));
	attr_char_value.p_uuid = &ble_uuid;
	attr_char_value.p_attr_md = &attr_md;
	attr_char_value.init_len = (CHAR_TX_POWER_LEVEL_LEN);
	attr_char_value.init_offs = 0;
	attr_char_value.max_len = (CHAR_TX_POWER_LEVEL_LEN);
	attr_char_value.p_value = value;

	return sd_ble_gatts_characteristic_add(p_pss->service_handle, &char_md, &attr_char_value, &p_pss->char_tx_power_level_handle);
}


void onBleEvtKCS(ble_pss_t* p_pss, ble_evt_t* p_ble_evt) {	// call functions for appropriate ble events
    switch (p_ble_evt->header.evt_id) {
        case BLE_GAP_EVT_CONNECTED:
			onEvtConn(p_pss, p_ble_evt);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
			onEvtDisc(p_pss, p_ble_evt);
            break;

        case BLE_GATTS_EVT_WRITE:
			onEvtWrite(p_pss, p_ble_evt);
            break;

		case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
			onEvtRW(p_pss, p_ble_evt);
			break;

        default:
            break;
    }
	updateCharSensorsData(p_pss);
}

static void onEvtConn(ble_pss_t* p_pss, ble_evt_t* p_ble_evt) {	// handle connect event
    p_pss->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
}
static void onEvtDisc(ble_pss_t* p_pss, ble_evt_t* p_ble_evt) {	// handle disconnect event
    UNUSED_PARAMETER(p_ble_evt);
    p_pss->conn_handle = BLE_CONN_HANDLE_INVALID;
}
static void onEvtWrite(ble_pss_t* p_pss, ble_evt_t* p_ble_evt) {		// handle write event
    ble_gatts_evt_write_t * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

    if (p_evt_write->handle == p_pss->char_sensor_write_handle.value_handle) {
        /*
		switch(p_evt_write->data[0]) {

            case FREE_MODE:
                ble_mode = BLE_FREE_MODE;
                initTIMER2();
                break;
        }
		*/
    }

    if (p_pss->is_notification_supported) {
        if ((p_evt_write->handle == p_pss->char_sensor_write_handle.cccd_handle)) {
            // CCCD written, call application event handler
            if (p_pss->evt_handler != NULL) {
                ble_pss_evt_t evt;
                evt.evt_type = BLE_PSS_EVT_NOTIFICATION_ENABLED;
                p_pss->evt_handler(p_pss, &evt);
            }
        }
    }
}
static void onEvtRW(ble_pss_t* p_pss, ble_evt_t* p_ble_evt) {		// handle write event

	ble_gatts_evt_rw_authorize_request_t *p_evt_rw_auth_req = &p_ble_evt->evt.gatts_evt.params.authorize_request;
	if (p_evt_rw_auth_req->type == BLE_GATTS_AUTHORIZE_TYPE_READ) {

		//Reading CharDiagInfo
		if (p_evt_rw_auth_req->request.read.handle == p_pss->char_diag_info_handle.value_handle) {
			charDiagInfoReadAuthReply(p_pss);
		}		
		//Reading CharFirmVer
		else if (p_evt_rw_auth_req->request.read.handle == p_pss->char_firm_ver_handle.value_handle) {
			charFirmwVerReadAuthReply(p_pss);
		}
		//Reading CharRockNum
		else if (p_evt_rw_auth_req->request.read.handle == p_pss->char_rock_num_handle.value_handle) {
			charRockNumReadAuthReply(p_pss);
		}
		//Reading CharTxPowerLevel
		else if (p_evt_rw_auth_req->request.read.handle == p_pss->char_tx_power_level_handle.value_handle) {
			charTxPowerLevelReadAuthReply(p_pss);
		}
	}
	
	if (p_evt_rw_auth_req->type == BLE_GATTS_AUTHORIZE_TYPE_WRITE) {
		//Writing CharRockNum
		if (p_evt_rw_auth_req->request.read.handle == p_pss->char_rock_num_handle.value_handle) {
			charRockNumWriteAuthReply(p_pss, p_evt_rw_auth_req->request.write.data);
		}
		//Writing CharTxPowerLevel
		else if (p_evt_rw_auth_req->request.read.handle == p_pss->char_tx_power_level_handle.value_handle) {
			charTxPowerLevelWriteAuthReply(p_pss, p_evt_rw_auth_req->request.write.data);
		}
	}

	ble_gatts_evt_read_t * p_evt_read = &p_ble_evt->evt.gatts_evt.params.authorize_request.request.read;
	if (p_pss->is_notification_supported) {
		if ((p_evt_read->handle == p_pss->char_diag_info_handle.cccd_handle)) {
			// CCCD written, call application event handler
			if (p_pss->evt_handler != NULL) {
				ble_pss_evt_t evt;
				evt.evt_type = BLE_PSS_EVT_NOTIFICATION_ENABLED;
				p_pss->evt_handler(p_pss, &evt);
			}
		}
	}
}

/*
double toDoubleFra(uint8_t high, uint8_t low) {
	uint16_t conversion_form;

	conversion_form = (high << 8 | low);
	short shortVal = (short)conversion_form;
	if (shortVal<0) {
		return (double)(shortVal*-1);
	}
	return (double)shortVal;
}
double pythagore3Fra(double a, double b, double c) {
	return sqrt((a*a) + (b*b) + (c*c));
}
*/

uint32_t updateCharSensorsData(ble_pss_t* p_pss)
{
    uint32_t err_code = NRF_SUCCESS;
	uint16_t len = (CHAR_SENSORS_DATA_VER_LEN);
    uint8_t data[CHAR_SENSORS_DATA_VER_LEN] = { 0 };

	if ((p_pss->conn_handle != BLE_CONN_HANDLE_INVALID) && p_pss->is_notification_supported) {
		ble_gatts_hvx_params_t hvx_params;

		memset(&hvx_params, 0, sizeof(hvx_params));
		hvx_params.handle   = p_pss->char_sensor_data_handle.value_handle;
		hvx_params.offset	= 0;
		hvx_params.type     = BLE_GATT_HVX_NOTIFICATION;
		hvx_params.p_len    = &len;

		getDataSENSOR(data);

		hvx_params.p_data = data;

		err_code = sd_ble_gatts_hvx(p_pss->conn_handle, &hvx_params);
    }
	else{
		err_code = NRF_ERROR_INVALID_STATE;
	}
    return err_code;
}
uint32_t updateCharDiagInfo(ble_pss_t* p_pss)
{
	uint32_t err_code = NRF_SUCCESS;
	uint16_t len = (CHAR_DIAG_INFO_LEN);

	uint32_t adcResult = 0;
	uint8_t	tempData[2] = { 0 };

	adcResult = GetAdcValue();		//Battery max level read 7.2V = 1.2 (referencec) * 3 (1/3 prescaling) * 2 (resistor divider)

	CharDiagInfoData[2] = ((adcResult & 0X00FF) * 0x64) / 0xFF;	//Battery Percentage Value
	CharDiagInfoData[3] = adcResult & 0x00FF;					//Battery Actual Value

	/*
	uint32_t text = 0;
	text = CharDiagInfoData[2];
	printUSART0("-> Battery Level Percent: [%h]\n", &text);
	text = CharDiagInfoData[3];
	printUSART0("-> ADC Value - Max 0xFF: [%h]\n", &text);
	*/

	getTempData(tempData);

	//0x00 = 25 deg C - 256bit/deg C
	CharDiagInfoData[4] = tempData[1];	//tempData[1] = TEMP_H
	CharDiagInfoData[5] = tempData[0];	//tempData[0] = TEMP_L

	//temp = CharDiagInfoData[4]*256 + CharDiagInfoData[5];
	//printUSART0("Temperature Level: [%h]\n", &temp);

	ble_gatts_hvx_params_t hvx_params;
	memset(&hvx_params, 0, sizeof(hvx_params));
	hvx_params.handle	= p_pss->char_diag_info_handle.value_handle;
	hvx_params.type		= BLE_GATT_HVX_NOTIFICATION;
	hvx_params.offset	= 0;
	hvx_params.p_len	= &len;
	hvx_params.p_data	= CharDiagInfoData;

	//if ((p_pss->conn_handle != BLE_CONN_HANDLE_INVALID) && p_pss->is_notification_supported)
	if ((p_pss->conn_handle != BLE_CONN_HANDLE_INVALID)) {
		err_code = sd_ble_gatts_hvx(p_pss->conn_handle, &hvx_params);
	}
	else {
		err_code = NRF_ERROR_INVALID_STATE;
	}
	return err_code;
}

uint32_t charDiagInfoReadAuthReply(ble_pss_t* p_pss) {
	uint32_t err_code = NRF_SUCCESS;

	uint32_t adcResult = 0;
	uint8_t	tempData[2] = {0, 0};

	adcResult = GetAdcValue();		//Battery max level read 7.2V = 1.2 (referencec) * 3 (1/3 prescaling) * 2 (resistor divider)

	CharDiagInfoData[2] = ((adcResult & 0X00FF) * 0x64) / 0xFF;	//Battery Percentage Value
	CharDiagInfoData[3] = adcResult & 0x00FF;					//Battery Actual Value

	getTempData(tempData);

	//0x00 = 25 deg C - 256bit/deg C
	CharDiagInfoData[4] = tempData[1];	//tempData[1] = TEMP_H
	CharDiagInfoData[5] = tempData[0];	//tempData[0] = TEMP_L

	ble_gatts_rw_authorize_reply_params_t p_rw_authorize_reply_params;
	p_rw_authorize_reply_params.type = BLE_GATTS_AUTHORIZE_TYPE_READ;
	p_rw_authorize_reply_params.params.read.gatt_status = BLE_GATT_STATUS_SUCCESS;
	p_rw_authorize_reply_params.params.read.update = 1;
	p_rw_authorize_reply_params.params.read.len = CHAR_DIAG_INFO_LEN;
	p_rw_authorize_reply_params.params.read.p_data = CharDiagInfoData;
	p_rw_authorize_reply_params.params.read.offset = 0;

	if ((p_pss->conn_handle != BLE_CONN_HANDLE_INVALID)) {
		err_code = sd_ble_gatts_rw_authorize_reply(p_pss->conn_handle, &p_rw_authorize_reply_params);
	}
	else {
		err_code = NRF_ERROR_INVALID_STATE;
	}
	return err_code;
}
uint32_t charFirmwVerReadAuthReply(ble_pss_t* p_pss) {
	uint32_t err_code = NRF_SUCCESS;

	ble_gatts_rw_authorize_reply_params_t p_rw_authorize_reply_params;
	p_rw_authorize_reply_params.type = BLE_GATTS_AUTHORIZE_TYPE_READ;
	p_rw_authorize_reply_params.params.read.gatt_status = BLE_GATT_STATUS_SUCCESS;
	p_rw_authorize_reply_params.params.read.update = 1;
	p_rw_authorize_reply_params.params.read.len = CHAR_FIRM_VER_LEN;
	p_rw_authorize_reply_params.params.read.p_data = CharFirmVerData;
	p_rw_authorize_reply_params.params.read.offset = 0;
	
	if ((p_pss->conn_handle != BLE_CONN_HANDLE_INVALID)) {
		err_code = sd_ble_gatts_rw_authorize_reply(p_pss->conn_handle, &p_rw_authorize_reply_params);
	}
	else {
		err_code = NRF_ERROR_INVALID_STATE;
	}
	return err_code;
}
uint32_t charRockNumReadAuthReply(ble_pss_t* p_pss) {
	uint32_t err_code = NRF_SUCCESS;

	ble_gatts_rw_authorize_reply_params_t p_rw_authorize_reply_params;
	p_rw_authorize_reply_params.type = BLE_GATTS_AUTHORIZE_TYPE_READ;
	p_rw_authorize_reply_params.params.read.gatt_status = BLE_GATT_STATUS_SUCCESS;
	p_rw_authorize_reply_params.params.read.update = 1;
	p_rw_authorize_reply_params.params.read.len = CHAR_ROCK_NUM_LEN;
	p_rw_authorize_reply_params.params.read.p_data = CharRockNumData;
	p_rw_authorize_reply_params.params.read.offset = 0;

	if ((p_pss->conn_handle != BLE_CONN_HANDLE_INVALID)) {
		err_code = sd_ble_gatts_rw_authorize_reply(p_pss->conn_handle, &p_rw_authorize_reply_params);
	}
	else {
		err_code = NRF_ERROR_INVALID_STATE;
	}
	return err_code;
}
uint32_t charRockNumWriteAuthReply(ble_pss_t* p_pss, uint8_t* p_data) {
	uint32_t err_code = NRF_SUCCESS;

	uint32_t text;
	text = p_data[3];
	printUSART0("RockNum[3]: [%h]\n", &text);

	if (p_data[3] == CHAR_ROCK_NUM_WRITE_BYTE) {		
		/*
		temp = p_data[0];
		printUSART0("RockNum[0]: [%h]\n", &text);
		temp = p_data[1];
		printUSART0("RockNum[1]: [%h]\n", &text);
		temp = p_data[2];
		printUSART0("RockNum[2]: [%h]\n", &text);
		*/
		CharRockNumData[0] = p_data[0];
		CharRockNumData[1] = p_data[1];
		CharRockNumData[2] = p_data[2];
		CharRockNumData[3] = p_data[3];
	}

	ble_gatts_rw_authorize_reply_params_t p_rw_authorize_reply_params;
	p_rw_authorize_reply_params.type = BLE_GATTS_AUTHORIZE_TYPE_WRITE;
	p_rw_authorize_reply_params.params.write.gatt_status = BLE_GATT_STATUS_SUCCESS;

	if ((p_pss->conn_handle != BLE_CONN_HANDLE_INVALID)) {
		err_code = sd_ble_gatts_rw_authorize_reply(p_pss->conn_handle, &p_rw_authorize_reply_params);
	}
	else {
		err_code = NRF_ERROR_INVALID_STATE;
	}
	return err_code;
}

uint32_t charTxPowerLevelReadAuthReply(ble_pss_t* p_pss) {
	uint32_t err_code = NRF_SUCCESS;

	ble_gatts_rw_authorize_reply_params_t p_rw_authorize_reply_params;
	p_rw_authorize_reply_params.type = BLE_GATTS_AUTHORIZE_TYPE_READ;
	p_rw_authorize_reply_params.params.read.gatt_status = BLE_GATT_STATUS_SUCCESS;
	p_rw_authorize_reply_params.params.read.update = 1;
	p_rw_authorize_reply_params.params.read.len = CHAR_TX_POWER_LEVEL_LEN;
	p_rw_authorize_reply_params.params.read.p_data = TxPowerLevelData;
	p_rw_authorize_reply_params.params.read.offset = 0;

	if ((p_pss->conn_handle != BLE_CONN_HANDLE_INVALID)) {
		err_code = sd_ble_gatts_rw_authorize_reply(p_pss->conn_handle, &p_rw_authorize_reply_params);
	}
	else {
		err_code = NRF_ERROR_INVALID_STATE;
	}
	return err_code;
}
uint32_t charTxPowerLevelWriteAuthReply(ble_pss_t* p_pss, uint8_t* p_data) {
	uint32_t err_code = NRF_SUCCESS;
	uint32_t text;

	if (p_data[0] > 0 && p_data[0] <= 4) {
		if (p_data[0] == 1) {
			text = sd_ble_gap_tx_power_set(NRF51822_TX_POWER_LEVEL_4dBm);
			if (text == (NRF_SUCCESS)) {
				text = NRF51822_TX_POWER_LEVEL_4dBm;
				printUSART0("Tx power changed to [%d]dBm\n", &text);
				TxPowerLevelData[0] = 1;
			}
		}
		else if (p_data[0] == 2) {
			text = sd_ble_gap_tx_power_set(NRF51822_TX_POWER_LEVEL_0dBm);
			if (text == (NRF_SUCCESS)) {
				text = NRF51822_TX_POWER_LEVEL_0dBm;
				printUSART0("Tx power changed to [%d]dBm\n", &text);
				TxPowerLevelData[0] = 2;
			}
		}
		else if (p_data[0] == 3) {
			text = sd_ble_gap_tx_power_set(NRF51822_TX_POWER_LEVEL_NEG_4dBm);
			if (text == (NRF_SUCCESS)) {
				text = NRF51822_TX_POWER_LEVEL_NEG_4dBm;
				printUSART0("Tx power changed to [%d]dBm\n", &text);
				TxPowerLevelData[0] = 3;
			}
		}
		else if (p_data[0] == 4) {
			text = sd_ble_gap_tx_power_set(NRF51822_TX_POWER_LEVEL_NEG_8dBm);
			if (text == (NRF_SUCCESS)) {
				text = NRF51822_TX_POWER_LEVEL_NEG_8dBm;
				printUSART0("Tx power changed to [%d]dBm\n", &text);
				TxPowerLevelData[0] = 4;
			}
		}
	}

	ble_gatts_rw_authorize_reply_params_t p_rw_authorize_reply_params;
	p_rw_authorize_reply_params.type = BLE_GATTS_AUTHORIZE_TYPE_WRITE;
	p_rw_authorize_reply_params.params.write.gatt_status = BLE_GATT_STATUS_SUCCESS;

	if ((p_pss->conn_handle != BLE_CONN_HANDLE_INVALID)) {
		err_code = sd_ble_gatts_rw_authorize_reply(p_pss->conn_handle, &p_rw_authorize_reply_params);
	}
	else {
		err_code = NRF_ERROR_INVALID_STATE;
	}
	return err_code;
}
