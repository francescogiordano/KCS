#ifndef _BLE_PHY_H_
#define _BLE_PHY_H_

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "led.h"
#include "sensors.h"

#include "dfu_app_handler.h"
#include "bootloader_util.h"
#include "nrf.h"
#include "nrf_sdm.h"
#include "ble_gatt.h"
#include "ble_gatts.h"
#include "dfu_ble_svc.h"
#include "device_manager.h"


#define BLE_APPEARANCE_KLUTCH_CURLING_SENSOR			2017

// Phy sensor structures

typedef enum
{
    BLE_PSS_EVT_NOTIFICATION_ENABLED,                            		// notification enabled event
    BLE_PSS_EVT_NOTIFICATION_DISABLED                            		// notification disabled event
} ble_pss_evt_type_t;
typedef struct
{
    ble_pss_evt_type_t evt_type;                                  		// type of event
} ble_pss_evt_t;

// Forward declaration of the ble_pss_t type.
typedef struct ble_pss_s ble_pss_t;
typedef void (*ble_pss_evt_handler_t) (ble_pss_t* p_pss, ble_pss_evt_t* p_evt);

typedef struct
{
    ble_pss_evt_handler_t         evt_handler;                    		// event handler
    bool                          support_notification;          		// true if notification is supported
    ble_srv_report_ref_t *        p_report_ref;                   		// if not NULL, a Report Reference descriptor with the specified value will be added tocharacteristic
    ble_srv_cccd_security_mode_t  phy_sens_char_attr_md;     			// Initial security level for physim characteristics attribute
    ble_gap_conn_sec_mode_t       phy_sens_report_read_perm; 			// Initial security level for physim report read attribute
} ble_pss_init_t;
typedef struct ble_pss_s												// various status information for the service
{
    ble_pss_evt_handler_t         evt_handler;                    		// event handler
    uint16_t                      service_handle;               		// handle of Service

    ble_gatts_char_handles_t      char_sensor_data_handle;          	// handles related to the phy sensor characteristic
    ble_gatts_char_handles_t      char_sensor_write_handle;
	ble_gatts_char_handles_t      char_diag_info_handle;
	ble_gatts_char_handles_t      char_firm_ver_handle;
	ble_gatts_char_handles_t      char_rock_num_handle;

    uint16_t                      report_ref_handle;              		// handle of the Report Reference descriptor.
    uint16_t                      conn_handle;                    		// handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection).
    bool                          is_notification_supported;      		// true if notification is supported.
    uint8_t                    	  uuid_type;
} ble_pss_t;
typedef enum
{

    BLE_LAUNCH_MODE = 0,
    //BLE_STICK_MODE=1,
    BLE_FREE_MODE=2,
    BLE_OTHER_MODE=3,
    BLE_SETTINGS_MODE=4,
    BLE_CALIB_AXIS_MODE=5,
} ble_mode_t;


#define BLE_UUID_VENDOR {0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}

#define BLE_UUID_SERVICE_SENSORS		    0x2000

#define BLE_UUID_CHAR_SENSORS_DATA		 	0x2E00
#define BLE_UUID_CHAR_SENSORS_DATA_WRITE	0x2E01
#define CHAR_SENSORS_DATA_VER_LEN			20

#define BLE_UUID_SERVICE_DIAG				0x2F00
#define BLE_UUID_CHAR_DIAG_INFO				0x2F01
#define BLE_UUID_CHAR_DIAG_WRITE	 		0x2F02
#define CHAR_DIAG_INFO_LEN					6

#define BLE_UUID_SERVICE_VERSION			0x3000
#define BLE_UUID_CHAR_FIRM_VER				0x3001
#define CHAR_FIRM_VER_LEN					3

#define BLE_UUID_SERVICE_ROCK_NUMBER		0x3100
#define BLE_UUID_CHAR_ROCK_NUMBER			0x3101
#define CHAR_ROCK_NUM_LEN					4
#define CHAR_ROCK_NUM_WRITE_BYTE			0x99


#define SETTINGS_MODE                   0x01
#define SETTINGS_READ                   0x02
#define SETTINGS_NEW                    0x03
#define DATA                            0x04
#define SHOT_SEQUENCE                   0x04
#define DATA_READY                      0x05
#define SHOT_MODE                       0x05
#define LAUNCH_MODE                     0x05
#define DATA_END                        0x06
#define LAUNCH_READY                    0x06
#define DATA_START                      0x08
#define DATA_DRAFT                      0x0A
#define STICK_START                     0x0B
#define STICK_MODE                      0x0B
#define STICK_MOMENT                    0x0C
#define FREE_MODE                       0x0D
#define CALIB_OUTPUT                    0x0E
#define CALIB_AXIS                      0x0F
#define POWER_DOWN_CORRECT              0x11

// Other configs:
#define HOGLINE_FINISH                  0x10;
#define MOVEMENT_FINISH                 0x11;
#define PLAYER_ID                       0x01;
#define MAGNETO_DATA                    0x02;


uint32_t initServiceSensors(ble_pss_t * p_pss, const ble_pss_init_t * p_pss_init);


static uint32_t addCharSensorsData(ble_pss_t * p_pss, const ble_pss_init_t * p_pss_init);
static uint32_t addCharSensorsWrite(ble_pss_t * p_pss, const ble_pss_init_t * p_pss_init);
static uint32_t addCharDiagInfo(ble_pss_t * p_pss, const ble_pss_init_t * p_pss_init);
static uint32_t addCharFirmVer(ble_pss_t * p_pss, const ble_pss_init_t * p_pss_init);
static uint32_t addCharRockNum(ble_pss_t * p_pss, const ble_pss_init_t * p_pss_init);

void onBleEvenPHYSEN(ble_pss_t * p_pss, ble_evt_t * p_ble_evt);


static void onEvtConn(ble_pss_t* , ble_evt_t* );
static void onEvtDisc(ble_pss_t* , ble_evt_t* );
static void onEvtWrite(ble_pss_t* , ble_evt_t* );
static void onEvtRW(ble_pss_t* , ble_evt_t* );


uint32_t sendDataPHYSENS(ble_pss_t * p_pss);
uint32_t sendDiagInfo(ble_pss_t* );

uint32_t charDiagInfoReadAuthReply(ble_pss_t*);
uint32_t charFirmwVerReadAuthReply(ble_pss_t* );
uint32_t charRockNumReadAuthReply(ble_pss_t*);
uint32_t charRockNumWriteAuthReply(ble_pss_t* , uint8_t* );

extern volatile uint8_t CharDiagInfoData[6];
extern volatile uint8_t CharFirmVerData[3];
extern volatile uint8_t CharRockNumData[4];

extern volatile uint8_t g_ble_conn;
extern volatile ble_mode_t ble_mode;

extern volatile uint8_t g_power_down;

#endif

