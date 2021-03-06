// make clean; make; make merge; make mob-app

//wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww

// Pin description
// P0.02   - ADC AN3 - Battery Level
// P0.08   - SPI CS - LSM6DSL - Accel & Gyro
// P0.09   - INTERRUPT - LSM6DSL - Gyro
// P0.10   - SPI MOSI
// P0.11   - SPI MISO
// P0.12   - SPI SCK
// P0.15   - SPI CS - MLX90393 - Mag
// P0.16   - INTERRUPT - LSM6DSL - Accel
// P0.21   - UART Tx Debug @115200 bauds
// P0.22   - UART Rx DEbug
// P0.22   - ENABLE TEST PIN
// P0.24   - INTERRUPT - ADXL362
// P0.25   - SPI CS - ADXL362
// P0.29   - INTERRUPT - MLX90393 - Mag


///wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
/// BLE Physics primary service 0x2000 (short UUID)
/// BLE Physics characteristics 0x2E00 (short UUID)
/// BLE DFU primary service 	0x1530 (short UUID)
///wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
/// Memory organization
///---------------------
/// SOFTDEVICE [0x00000000] - [0x00017FFF]
/// MAIN PROG  [0x00018000] - [0x00039FFF]
/// DFU  PROG  [0x0003A000] - [0x0003FC00]
/// 	- DFU alocated space 0x00005C00
//		- DFU utilized space 0x00005B82
///wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
// Compilation process is comprised of following commands
/// make clean				-> cleans all files
/// make 					-> creates main program *.hex and *.bin files
/// make merge				-> merges softdevice.hex, main.hex & dfu.hex
///							   into one file ble.hex
///---------------------------------------------------------------------
// if You want to generate new file that you want to upload using the
// dfu process then type as well
/// make mob-app			-> generates CRC CCITT-16, creates main.dat
///							   manifest.json and zip them into one file
///							   called main.zip, this is the file that
///							   has to be sent during the dfu process.
///							   main.zip is in the same folder as makefile
///	if process fails please install zip by typing
/// sudo apt-get intall zip
///wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
/// Example of write to characteristics to enable notifications
///---------------------------------------------------------------------
/// gatttool -b EB:26:04:97:75:32 -t random -I
/// [   ][EB:26:04:97:75:32][LE]> connect
/// [CON][EB:26:04:97:75:32][LE]> primary
/// [CON][EB:26:04:97:75:32][LE]>
/// attr handle: 0x0001, end grp handle: 0x0007 uuid: 00001800-0000-1000-8000-00805f9b34fb
/// attr handle: 0x0008, end grp handle: 0x000b uuid: 00001801-0000-1000-8000-00805f9b34fb
/// attr handle: 0x000c, end grp handle: 0x0010 uuid: 00002000-0000-1000-8000-00805f9b34fb
/// attr handle: 0x0011, end grp handle: 0xffff uuid: 00001530-1212-efde-1523-785feabcd123
/// [CON][EB:26:04:97:75:32][LE]> char-write-cmd 0x0010 05				-> start process of sending the data
/// [CON][EB:26:04:97:75:32][LE]> char-write-req 0x0010 05 -listen
/// [CON][EB:26:04:97:75:32][LE]> char-write-cmd 0x0010 00				-> stop process of sending the data
///---------------------------------------------------------------------
// Heap size reduce from 2048 to 1024 in the gcc_startup_nrf51.s file
// in device_manager_cnfg.h change DM_GATT_CCCD_COUNT from 2 to 4


#include "gpio.h"
#include "adc.h"
#include "delay.h"
#include "wdt.h"

#include "nrf_drv_gpiote.h"
#include "ble_hci.h"
#include "ble_advdata.h"
#include "ble_phy.h"
#include "ble_conn_params.h"
#include "app_scheduler.h"
#include "softdevice_handler.h"
#include "app_timer_appsh.h"

//#include "nrf51_bitfields.h"

//#include <stdint.h>
//#include <string.h>
//#include "nordic_common.h"
//#include "nrf.h"
//#include "app_error.h"
//#include "nrf_gpio.h"
//#include "ble.h"
//#include "ble_srv_common.h"
//#include "ble_advertising.h"
//#include "ble_advdata.h"
//#include "ble_error_log.h"


//#include "ble_bondmngr.h"
#include "ble_debug_assert_handler.h"
#include "pstorage.h"
#include "usart.h"
#include "sensors.h"
#include "delay.h"
#include "flashmemory.h"

// DFU BLE service support
#include "ble_dfu.h"
#include "dfu_app_handler.h"


#define IS_SRVC_CHANGED_CHARACT_PRESENT		1
#define SCHED_MAX_EVENT_DATA_SIZE			sizeof(app_timer_event_t)                   /**< Maximum size of scheduler events. Note that scheduler BLE stack events do not contain any data, as the events are being pulled from the stack in the event handler. */
#define SCHED_QUEUE_SIZE					20                                          /**< Maximum number of events in the scheduler queue. */

//#define DEVICE_NAME                         "PaulTheBest"
//#define DEVICE_NAME                         "Francesco"
#define DEVICE_NAME							"Klutch Curling Sensor"						/**< Name of device. Will be included in the advertising data. */
#define DEVICE_FIRMWARE_VERSION_MAJOR		0x01
#define DEVICE_FIRMWARE_VERSION_MINOR		0x03
#define DEVICE_FIRMWARE_VERSION_REVISION	0x03
#define MANUFACTURER_NAME                   "nRF51822"                     				/**< Manufacturer. Will be passed to Device Information Service. */
#define APP_ADV_INTERVAL                    32											/**< The advertising interval (in units of 0.625 ms. This value corresponds to 25 ms). */
#define APP_ADV_TIMEOUT_180_SECONDS         180											/**< The advertising timeout in units of seconds. */
#define APP_ADV_TIMEOUT_10_SECONDS	        10											/**< The advertising timeout in units of seconds. */

#define APP_TIMER_PRESCALER                 0											/**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_MAX_TIMERS                1											/**< Maximum number of simultaneously created timers. */
#define APP_TIMER_OP_QUEUE_SIZE             1											/**< Size of timer operation queues. */

#define MIN_CONN_INTERVAL                   MSEC_TO_UNITS(7.5, UNIT_1_25_MS)			/**< Minimum acceptable connection interval (0.5 seconds). */
#define MAX_CONN_INTERVAL                   MSEC_TO_UNITS(10, UNIT_1_25_MS)				/**< Maximum acceptable connection interval (1 second). */

#define SLAVE_LATENCY                       0											/**< Slave latency. */
#define CONN_SUP_TIMEOUT                    MSEC_TO_UNITS(300, UNIT_10_MS)				/**< Connection supervisory timeout (4 seconds). */

#define FIRST_CONN_PARAMS_UPDATE_DELAY      APP_TIMER_TICKS(500, APP_TIMER_PRESCALER)	/**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY       APP_TIMER_TICKS(500, APP_TIMER_PRESCALER)	/**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT        3											/**< Number of attempts before giving up the connection parameter negotiation. */

#define SEC_PARAM_TIMEOUT                   30											/**< Timeout for Pairing Request or Security Request (in seconds). */
#define SEC_PARAM_BOND                      1											/**< Perform bonding. */
#define SEC_PARAM_MITM                      0											/**< Man In The Middle protection not required. */
#define SEC_PARAM_IO_CAPABILITIES           BLE_GAP_IO_CAPS_NONE						/**< No I/O capabilities. */
#define SEC_PARAM_OOB                       0											/**< Out Of Band data not available. */
#define SEC_PARAM_MIN_KEY_SIZE              7											/**< Minimum encryption key size. */
#define SEC_PARAM_MAX_KEY_SIZE              16											/**< Maximum encryption key size. */

#define DEAD_BEEF                           0xDEADBEEF									/**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

#define DFU_REV_MAJOR						0x00										/** DFU Major revision number to be exposed. */
#define DFU_REV_MINOR						0x01										/** DFU Minor revision number to be exposed. */
#define DFU_REVISION						((DFU_REV_MAJOR << 8) | DFU_REV_MINOR)		/** DFU Revision number to be exposed. Combined of major and minor versions. */
#define APP_SERVICE_HANDLE_START			0x000C										/**< Handle of first application specific service when when service changed characteristic is present. */
#define BLE_HANDLE_MAX						0xFFFF										/**< Max handle value in BLE. */

static uint16_t                             m_conn_handle = BLE_CONN_HANDLE_INVALID;	/**< Handle of the current connection. */
static ble_gap_sec_params_t                 m_sec_params;								/**< Security requirements for this application. */
//static ble_gap_adv_params_t                 m_adv_params;                              /**< Parameters to be passed to the stack when starting advertising. */
static ble_pss_t                            m_pss;

static ble_dfu_t							m_dfus;										/**< Structure used to identify the DFU service. */
static dm_application_instance_t			m_app_handle;								/**< Application identifier allocated by device manager */

volatile uint8_t CharDiagInfoData[6];
volatile uint8_t CharFirmVerData[3] = { DEVICE_FIRMWARE_VERSION_MAJOR, DEVICE_FIRMWARE_VERSION_MINOR, DEVICE_FIRMWARE_VERSION_REVISION };
volatile uint8_t CharRockNumData[4];
volatile uint8_t TxPowerLevelData[1] = { 0 };

//Function Prototypes
void mcuSystemOff(void);

static void advertising_init(void)  {	// init advertising data with type of services that can be accessed
	uint32_t      err_code;
	ble_advdata_t advdata;
	ble_advdata_t scanrsp;

	ble_uuid_t adv_uuids[] = { { BLE_UUID_SERVICE_SENSORS, BLE_UUID_TYPE_BLE } };
	
	/*
	ble_advdata_manuf_data_t manuf_data; // Variable to hold manufacturer specific data
	uint8_t data[] = "SomeData!"; // Our data to adverise
	manuf_data.company_identifier = 0x0059; // Nordics company ID
	manuf_data.data.p_data = data;
	manuf_data.data.size = sizeof(data);
	*/

	UpdateCharDiagInfoDataBatTemp();

	uint32_t* flashMemory;
	flashMemory = GetFlashMemoryStartAddress();

	CharRockNumData[0] = flashMemory[0] >> 24;
	CharRockNumData[1] = flashMemory[0] >> 16;
	CharRockNumData[2] = flashMemory[0] >> 8;
	CharRockNumData[3] = flashMemory[0];

	uint8_t data[10];
	
	data[0] = CharRockNumData[0];
	data[1] = CharRockNumData[1];
	data[2] = CharRockNumData[2];
	data[3] = CharRockNumData[3];

	data[4] = CharDiagInfoData[0];
	data[5] = CharDiagInfoData[1];
	data[6] = CharDiagInfoData[2];

	data[7] = CharDiagInfoData[3];
	data[8] = CharDiagInfoData[4];
	data[9] = CharDiagInfoData[5];

	ble_advdata_service_data_t service_data;
	service_data.service_uuid = BLE_UUID_CHAR_ROCK_NUMBER;
	service_data.data.p_data = data;
	service_data.data.size = sizeof(data);

	// Build and set advertising data
	memset(&advdata, 0, sizeof(advdata));

	advdata.name_type = BLE_ADVDATA_SHORT_NAME;
	advdata.short_name_len = 6;
	advdata.include_appearance = true;
	advdata.flags = BLE_GAP_ADV_FLAGS_LE_ONLY_LIMITED_DISC_MODE;
	advdata.p_service_data_array = &service_data;
	advdata.service_data_count = 1;
	//advdata.p_manuf_specific_data = &manuf_data;
	
	memset(&scanrsp, 0, sizeof(scanrsp));
	scanrsp.uuids_complete.uuid_cnt = sizeof(adv_uuids) / sizeof(adv_uuids[0]);
	scanrsp.uuids_complete.p_uuids = adv_uuids;

	err_code = ble_advdata_set(&advdata, &scanrsp);
	APP_ERROR_CHECK(err_code);
}
static void advertising_start(void) {
	uint32_t err_code;
	ble_gap_adv_params_t adv_params;

	// Start advertising
	memset(&adv_params, 0, sizeof(adv_params));

	adv_params.type = BLE_GAP_ADV_TYPE_ADV_IND;
	adv_params.p_peer_addr = NULL;
	adv_params.fp = BLE_GAP_ADV_FP_ANY;
	adv_params.interval = APP_ADV_INTERVAL;
	adv_params.timeout = APP_ADV_TIMEOUT_180_SECONDS;

	err_code = sd_ble_gap_adv_start(&adv_params);
	APP_ERROR_CHECK(err_code);

	printUSART0("Advertise Start\n", 0);
}
static void advertising_stop(void) {
	uint32_t err_code;

	err_code = sd_ble_gap_adv_stop();
	APP_ERROR_CHECK(err_code);

	printUSART0("Advertise Stop\n", 0);
}

static void app_context_load(dm_handle_t const * p_handle) {
	// DFU - function for loading application-specific context after establishing a secure connection.
	uint32_t                 err_code;
	static uint32_t          context_data;
	dm_application_context_t context;

	context.len = sizeof(context_data);
	context.p_data = (uint8_t *)&context_data;

	err_code = dm_application_context_get(p_handle, &context);
	if (err_code == NRF_SUCCESS) {
		// Send Service Changed Indication if ATT table has changed.
		if ((context_data & (DFU_APP_ATT_TABLE_CHANGED << DFU_APP_ATT_TABLE_POS)) != 0)
		{
			err_code = sd_ble_gatts_service_changed(m_conn_handle, APP_SERVICE_HANDLE_START, BLE_HANDLE_MAX);
			if ((err_code != NRF_SUCCESS) &&
				(err_code != BLE_ERROR_INVALID_CONN_HANDLE) &&
				(err_code != NRF_ERROR_INVALID_STATE) &&
				(err_code != BLE_ERROR_NO_TX_BUFFERS) &&
				(err_code != NRF_ERROR_BUSY) &&
				(err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING))
			{
				APP_ERROR_HANDLER(err_code);
			}
		}

		err_code = dm_application_context_delete(p_handle);
		APP_ERROR_CHECK(err_code);
	}
	else if (err_code == DM_NO_APP_CONTEXT) {
		// No context available. Ignore.
	}
	else {
		APP_ERROR_HANDLER(err_code);
	}
}
static void reset_prepare(void) {
	// DFU - prepare system for safe system reset
	uint32_t err_code;

	if (m_conn_handle != BLE_CONN_HANDLE_INVALID) {
		// Disconnect from peer.
		err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
		APP_ERROR_CHECK(err_code);
	}
	else {		// If not connected, the device will be advertising. Hence stop the advertising.
		advertising_stop();
	}

	err_code = ble_conn_params_stop();
	APP_ERROR_CHECK(err_code);

	delay_ms(500);
}
static void power_manage(void)	{
	// power manager
	//APP_ERROR_CHECK(sd_power_mode_set(NRF_POWER_MODE_LOWPWR));
	uint32_t err_code = sd_app_evt_wait();
	APP_ERROR_CHECK(err_code);
}

static void on_ble_evt(ble_evt_t * p_ble_evt) {		// handle application's BLE stack events
	uint32_t err_code;
	uint32_t printValue;

	static ble_gap_sec_keyset_t keys_exchanged;

	switch (p_ble_evt->header.evt_id) {
		case BLE_EVT_TX_COMPLETE:
			break;

		case BLE_GAP_EVT_CONNECTED:
			printUSART0("-> BLE: New device connected [%d]\n", &printValue);
			g_ble_conn = 1;

			//execute appropriate handle function for a given ble service!!!
			m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
			break;

		case BLE_GAP_EVT_DISCONNECTED:
			printUSART0("-> BLE: Device disconnected [%d]\n", &printValue);
			g_ble_conn = 0;
			m_conn_handle = BLE_CONN_HANDLE_INVALID;
			break;

		case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
			printUSART0("-> BLE: Param request received\n", 0);
			err_code = sd_ble_gap_sec_params_reply(m_conn_handle, BLE_GAP_SEC_STATUS_SUCCESS, &m_sec_params, &keys_exchanged);
			APP_ERROR_CHECK(err_code);
			break;

		case BLE_GAP_EVT_TIMEOUT:
			printUSART0("-> BLE: GAP timeout - nRF51 sleeping...\n", 0);
			if (p_ble_evt->evt.gap_evt.params.timeout.src == BLE_GAP_TIMEOUT_SRC_ADVERTISING) {
				mcuSystemOff();
			}
			break;

		default:
			break;
	}
}
static void ble_evt_dispatch(ble_evt_t * p_ble_evt) {	// dispatch BLE stack event

	dm_ble_evt_handler(p_ble_evt);
	ble_conn_params_on_ble_evt(p_ble_evt);
	ble_dfu_on_ble_evt(&m_dfus, p_ble_evt);
	ble_advertising_on_ble_evt(p_ble_evt);

	on_ble_evt(p_ble_evt);
	onBleEvtKCS(&m_pss, p_ble_evt);
}
static void sys_evt_dispatch(uint32_t sys_evt) {	// dispatch system event to designated module (called from System event interrupt handler)
	pstorage_sys_event_handler(sys_evt);

	if (sys_evt == NRF_EVT_FLASH_OPERATION_SUCCESS || sys_evt == NRF_EVT_FLASH_OPERATION_ERROR) {
		SetFlashMemoryOpDoneFlag(1);
		printUSART0("NRF_EVT_FLASH Called\n", 0);
	}
}

static void on_conn_params_evt(ble_conn_params_evt_t * p_evt)
{/// handle connection parameters module (CMP) (called for all events in CMP)
	uint32_t err_code;

	if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED) {
		err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
		APP_ERROR_CHECK(err_code);
	}
}
static void bond_manager_error_handler(uint32_t nrf_error)
{/// bond manager error handling
	APP_ERROR_HANDLER(nrf_error);
}
static void conn_params_error_handler(uint32_t nrf_error)
{/// error handler for connection parameters
	APP_ERROR_HANDLER(nrf_error);
}

static uint32_t device_manager_evt_handler(dm_handle_t const * p_handle, dm_event_t const  * p_event, ret_code_t event_result)
{
	APP_ERROR_CHECK(event_result);

	if (p_event->event_id == DM_EVT_LINK_SECURED) {
		app_context_load(p_handle);
	}

	return NRF_SUCCESS;
}

static void timers_init(void)
{/// init timer handle functions
    APP_TIMER_APPSH_INIT(APP_TIMER_PRESCALER, APP_TIMER_MAX_TIMERS, APP_TIMER_OP_QUEUE_SIZE, true);
}
static void dfu_service_init(void) {
	// init dfu service
	uint32_t       err_code;

	// DFU BLE Service initialization
	ble_dfu_init_t   dfus_init;
	memset(&dfus_init, 0, sizeof(dfus_init));
	dfus_init.evt_handler = dfu_app_on_dfu_evt;
	dfus_init.error_handler = NULL;
	dfus_init.evt_handler = dfu_app_on_dfu_evt;
	dfus_init.revision = DFU_REVISION;

	err_code = ble_dfu_init(&m_dfus, &dfus_init);
	APP_ERROR_CHECK(err_code);

	dfu_app_reset_prepare_set(reset_prepare);
	dfu_app_dm_appl_instance_set(m_app_handle);
}
static void primary_service_init(void) {
	// init primary service
	uint32_t       err_code;
	ble_pss_init_t pss_init;

	memset(&pss_init, 0, sizeof(pss_init));
	pss_init.evt_handler = NULL;				//call back function
	pss_init.support_notification = true;
	pss_init.p_report_ref = NULL;

	err_code = initServiceSensors(&m_pss, &pss_init);
	APP_ERROR_CHECK(err_code);
}

static void sec_params_init(void)
{/// init security parameters
	m_sec_params.bond = SEC_PARAM_BOND;
	m_sec_params.mitm = SEC_PARAM_MITM;
	m_sec_params.io_caps = SEC_PARAM_IO_CAPABILITIES;
	m_sec_params.oob = SEC_PARAM_OOB;
	m_sec_params.min_key_size = SEC_PARAM_MIN_KEY_SIZE;
	m_sec_params.max_key_size = SEC_PARAM_MAX_KEY_SIZE;
}
static void ble_stack_init(void)
{/// init BLE stack (softdevice) & interrupts
	uint32_t err_code;

	// Initialize the SoftDevice handler module.
	//SOFTDEVICE_HANDLER_INIT(NRF_CLOCK_LFCLKSRC_XTAL_500_PPM, false);
	
	SOFTDEVICE_HANDLER_INIT(NRF_CLOCK_LFCLKSRC_RC_250_PPM_250MS_CALIBRATION, false);

	// Enable BLE stack
	ble_enable_params_t ble_enable_params;
	memset(&ble_enable_params, 0, sizeof(ble_enable_params));
	ble_enable_params.gatts_enable_params.service_changed = IS_SRVC_CHANGED_CHARACT_PRESENT;
	err_code = sd_ble_enable(&ble_enable_params);
	APP_ERROR_CHECK(err_code);

	/*ble_gap_addr_t addr;
	err_code = sd_ble_gap_address_get(&addr);
	APP_ERROR_CHECK(err_code);
	sd_ble_gap_address_set(BLE_GAP_ADDR_CYCLE_MODE_NONE, &addr);
	APP_ERROR_CHECK(err_code);*/

	// Subscribe for BLE events.
	err_code = softdevice_ble_evt_handler_set(ble_evt_dispatch);
	APP_ERROR_CHECK(err_code);

	// Register with the SoftDevice handler module for BLE events.
	err_code = softdevice_sys_evt_handler_set(sys_evt_dispatch);
	APP_ERROR_CHECK(err_code);

}
static void scheduler_init(void)
{
	APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);
}
static void gap_params_init(void)
{/// GAP (Generic Access Profile) parameters ->  device name, appearance, and the preferred connection parameters
    uint32_t                err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode, (const uint8_t *)DEVICE_NAME, strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    err_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_KLUTCH_CURLING_SENSOR);
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}
static void conn_params_init(void)
{/// init connection parameters
	uint32_t               err_code;
	ble_conn_params_init_t cp_init;

	memset(&cp_init, 0, sizeof(cp_init));

	cp_init.p_conn_params = NULL;
	cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
	cp_init.next_conn_params_update_delay = NEXT_CONN_PARAMS_UPDATE_DELAY;
	cp_init.max_conn_params_update_count = MAX_CONN_PARAMS_UPDATE_COUNT;
	//cp_init.start_on_notify_cccd_handle    = m_hrs.hrm_handles.cccd_handle;
	cp_init.disconnect_on_fail = false;
	cp_init.evt_handler = on_conn_params_evt;
	cp_init.error_handler = conn_params_error_handler;

	err_code = ble_conn_params_init(&cp_init);
	APP_ERROR_CHECK(err_code);
}
static void device_manager_init(bool erase_bonds)
{
	uint32_t               err_code;
	dm_init_param_t        init_param = { .clear_persistent_data = erase_bonds };
	dm_application_param_t register_param;

	// Initialize persistent storage module.
	err_code = pstorage_init();
	APP_ERROR_CHECK(err_code);

	err_code = dm_init(&init_param);
	APP_ERROR_CHECK(err_code);

	memset(&register_param.sec_param, 0, sizeof(ble_gap_sec_params_t));

	register_param.sec_param.bond = SEC_PARAM_BOND;
	register_param.sec_param.mitm = SEC_PARAM_MITM;
	register_param.sec_param.io_caps = SEC_PARAM_IO_CAPABILITIES;
	register_param.sec_param.oob = SEC_PARAM_OOB;
	register_param.sec_param.min_key_size = SEC_PARAM_MIN_KEY_SIZE;
	register_param.sec_param.max_key_size = SEC_PARAM_MAX_KEY_SIZE;
	register_param.evt_handler = device_manager_evt_handler;
	register_param.service_type = DM_PROTOCOL_CNTXT_GATT_SRVR_ID;

	err_code = dm_register(&m_app_handle, &register_param);
	APP_ERROR_CHECK(err_code);
}

void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name) {
	// callback function in case of error SoftDevice.
	app_error_handler(DEAD_BEEF, line_num, p_file_name);
}
void in_pin_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{

}
void app_error_handler(uint32_t error_code, uint32_t line_num, const uint8_t* p_file_name) {	// in case system hangs -> self reboot
	printUSART0("-> ERROR: Code[%h]\n", &error_code);
	printUSART0("-> ERROR: Line[%h]\n", &line_num);
	//printUSART0("-> ERROR: File[%s]\n", p_file_name);
	NVIC_SystemReset();
}

void mcuSystemOff() {
	if (getSleepSensorErrorFlag() == 1) {
		NVIC_SystemReset();
	}
	else {
		stopTIMER2();

		sleepMlx90393();
		/*
		if (sleepLsm6dsl() == 1) {		//Accel Wake-Up
		NVIC_SystemReset();
		}
		*/
		/*
		if (sleepTiltLsm6dsl() == 1) {		//Tilt Wake-Up
		NVIC_SystemReset();
		}
		*/
		/*
		if (sleepDoubleTapLsm6dsl() == 1) {		//Double Tap Wake-Up
			NVIC_SystemReset();
		}
		*/
		if (sleepWristTiltLsm6dsl() == 1){		//Wrist Tilt Wake-Up
			NVIC_SystemReset();
		}
		//sleepSPIO();
		printUSART0("Power Down\n", 0);
		sleepUSART0();
		//sleepGPIO();
		//NRF_ADC->CONFIG = 0;		

		APP_ERROR_CHECK(sd_power_system_off());
	}
}

int main(void){

	uint32_t err_code;
	uint32_t printValue;
    uint32_t sleepCounter = 0;
	uint32_t maxCounter = 20;	//10 * 100ms = 1 second

	//Init & Start Watch Dog Timer
	initWDT();
	startWDT();
   
	// Init system modules
	initGPIO();

	InitAdc(ADC_PIN);
    initUSART0(UART_TX_PIN, UART_RX_PIN, USAR0_BAUDRATE_115200);
	initSPI0(SPI_DATARATE_4Mbps);

	printUSART0("\n\nStarting BLE KCS...\n", 0);
	printUSART0("Device Name: ", 0);
	printUSART0(DEVICE_NAME, 0);
	printUSART0("\n", 0);

	//ENABLE_TEST_PIN active high will enter "TEST MODE"
	if (nrf_gpio_pin_read(ENABLE_TEST_PIN)) {
		printUSART0("Entered Test Mode\n", 0);
		while (nrf_gpio_pin_read(ENABLE_TEST_PIN)) {
			if (readCharUSART0() == 0x61) {
				
				//Test Mag & Accel IC
				initSensors();

				//Test ADC
				uint32_t value = GetAdcValue();
				if (value >= 156 && value <= 163) {
					printUSART0("R7 ADC [OK]\n", 0);
				}
				else {
					//printUSART0("R7 ADC [FAIL][%d]\n", &value);
					printUSART0("R7 ADC [FAIL]\n", 0);
				}
			}
			tickleWDT();
		}
	}

	initSensors();
    timers_init();
    ble_stack_init();
    device_manager_init(0);
    scheduler_init();
    gap_params_init();

	err_code = sd_ble_gap_tx_power_set(NRF51822_TX_POWER_LEVEL_NEG_8dBm);
    if(err_code == (NRF_SUCCESS)) {
		printValue = NRF51822_TX_POWER_LEVEL_NEG_8dBm;
		printUSART0("Tx power changed to [%d]dBm\n",&printValue);
		TxPowerLevelData[0] = 4;
	}
	else {
		printUSART0("Error setting Tx power\n",0);
	}

	primary_service_init();
	dfu_service_init();
    advertising_init();
    conn_params_init();
    sec_params_init();

	advertising_start();

	initTIMER2();
	startTIMER2();

    while(1) {
        sleepCounter++;

		if (g_ble_conn) {
			sleepCounter = 0;
			if (CharRockNumData[3] == CHAR_ROCK_NUM_WRITE_BYTE) {
				uint32_t data[1];

				data[0] = 0x00;
				data[0] += ((uint32_t)CharRockNumData[0]) << 24;
				data[0] += ((uint32_t)CharRockNumData[1]) << 16;
				data[0] += ((uint32_t)CharRockNumData[2]) << 8;
				CharRockNumData[3] = 0x00;

				APP_ERROR_CHECK(FlashMemoryPageErase(0, 1));
				APP_ERROR_CHECK(FlashMemoryPageWrite(0, data, 1));		

				mcuSystemOff();
			}
		}
		else if (sleepCounter >= maxCounter) {
			sleepCounter = 0;
			mcuSystemOff();
		}
		tickleWDT();
		power_manage();
    }
}
