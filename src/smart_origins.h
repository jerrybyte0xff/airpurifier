/*****************************************************************************
 * @file    smart_origins.h
 * @brief   smart_origins Prototype
 * @author  jerry
 * @date    2016.7.01.
*****************************************************************************/
 /****************************************************************************
 *	 @(#)Copyright (C) Kywoo Beijing Co., Ltd.
 *****************************************************************************/
#ifndef _SMART_ORIGINS_H_
#define _SMART_ORIGINS_H_

/*****************************************************************************
 *        Include
 *****************************************************************************/
#include <hap.h>
#include <mdev_uart.h>
#include <services/mext_serv_fw_upgrade.h>
#include <push_button.h>
#include <services/mext_provisioning.h>
#include <mfg_psm.h>

#include <mdev_i2c.h>
#include <wmstdio.h>
#include <wm_os.h>
#include <pwrmgr.h>
#include <wmtime.h>
#include <mdev_pinmux.h>
#include <cli.h>


#ifdef APPCONFIG_DEBUG_ENABLE
#include <wmsysinfo.h>
#endif /* APPCONFIG_DEBUG_ENABLE */
/*****************************************************************************
 *        MacroDefine
 *****************************************************************************/
#define DEBUG_MODE 1

 

/**************** Initialization Value Of Characteristics*********************/
/* characteristic initialization value for air_particulate_density*/
#define HAP_CHAR_INIT_AIR_PARTICULATE_DENSITY		0

/* characteristic initialization value for current temperature*/
#define HAP_CHAR_INIT_CURRENT_TEMPERATURE			0

/* characteristic initialization value for current huminity*/
#define HAP_CHAR_INIT_CURRENT_HUMIDITY				0

/* characteristic initialization value for  battery level characteristic.*/
#define HAP_CHAR_INIT_BATTERY_LEVEL					100
#define HAP_CHAR_INIT_BATTERY_CHARGING_STATE		CHARGING_STATE_NO_CHARGING
#define HAP_CHAR_INIT_BATTERY_LOW_BATTERY_STATUS	BATTERY_STATUS_NORMAL


/* Pin used for non-ios provisioning service */
//#define PROV_PIN "12345678"
//#define HAP_CONFIG_VERSION	1
//#define APP_FW_VERSION	"1.0"
//#define APP_SW_VERSION	"1.0"

/** The Max serial-number Length of this accessory. */
#define HAP_ACCESSORY_CFG_SERIAL_NO_MAX						20

/** Maximum number of accessories. */
#define HAP_ACCESSORY_NO_MAX							1
/** Maximum number of total services. */
#define HAP_SERVICES_NO_MAX								7 //6
 /** Maximum number of characteristics. */
#define HAP_CHARACTERISTICS_NO_MAX						32 // 22


/* Pin used for non-ios provisioning service */
#define PROV_PIN "12345678"
#define HAP_CONFIG_VERSION	1
// #define APP_FW_VERSION	"1.0"
// #define APP_SW_VERSION	"1.0"

/* Accesory Configuration*/
/** The name of this accessory. */
#define HAP_ACCESSORY_CFG_NAME							"AirPurifier"
/** The name of the manufacturer. */
#define HAP_ACCESSORY_CFG_MANUFACTURE					"Smart Origins"
/** The model of this accessory. */
#define HAP_ACCESSORY_CFG_MODEL							"AirPurifier"
/** The serial-number of this accessory. */
#define HAP_ACCESSORY_CFG_SERIAL_NO						"123456789"
#define HAP_ACCESSORY_CFG_SERIAL_NO_MAX					20

#define HAP_CHAR_TYPE_ACTIVE			"000000B0-0000-1000-8000-0026BB765291"

/***********************************************************************************/
/*  smart origin crit_err  */
typedef enum _SMART_ORIGIN_CRIT_ERR 
{
	CRIT_ERR_APP = HAP_CRIT_ERR_LAST + 1,
}SMART_ORIGIN_CRIT_ERR;

/*  air quality level  */
typedef enum _AIR_QUALITY_LEVEL
{
	AIR_QUALITY_LEVEL_UNKNOWN = 0,
	AIR_QUALITY_LEVEL_EXCELLENT,
	AIR_QUALITY_LEVEL_GOOD,
	AIR_QUALITY_LEVEL_FAIR,
	AIR_QUALITY_LEVEL_INFERIOR,
	AIR_QUALITY_LEVEL_POOR

}AIR_QUALITY_LEVEL;

/*  air particulate size */
typedef enum _AIR_PARTICULATE_SIZE
{
	AIR_PARTICULATE_SIZE_2_5 = 0,
	AIR_PARTICULATE_SIZE_10,
	AIR_QUALITY_LEVEL_RESERVE
}AIR_PARTICULATE_SIZE;

/*  Battery  Charging state */
typedef enum _BATTERY_CHARGING_STATE
{
	CHARGING_STATE_NO_CHARGING = 0,
	CHARGING_STATE_CHARGING
}BATTERY_CHARGING_STATE;

/*  Battery  Status*/
typedef enum _BATTERY_STATUS
{
	BATTERY_STATUS_NORMAL = 0,
	BATTERY_STATUS_LOW
}BATTERY_STATUS;

/*  Air Data Struct*/
typedef  struct _HAP_AIRPURIFIER_T
{
	float  rotation_speed;
	uint8_t air_status_active;
	uint8_t current_air_purifier_state;
	uint8_t target_air_purifier_state;
	uint8_t lock_physical_controls;

	uint8_t filter_change_indication;
	float filter_life_level;
	uint8_t rest_filter_indication;
	uint8_t wifi_status;

} HAP_AIRPURIFIER_T;

typedef enum _COMMAND_TYPE
{
	ROTATION_SPEED_CHANGE=0,
	WIFI_STATUS_INFO,
	RESEVERD,
}COMMAND_TYPE;

/*  Battery Data Struct*/
typedef struct _HAP_BAT_T 
{
	uint8_t battery_level;
	uint8_t status_low_battery;
	uint8_t charging_state;
}HAP_BAT_T;

/*  Temperature && Humidity Data Struct*/
//typedef struct _HAP_HT_T
//{
//	float temperature;
//	float humidity;
//}HAP_HT_T;

/* No used */
// typedef struct _DEV_STATUS_DATA
// {
// 	char * str_quality_name;
// 	float air_particulate_density;
// 	uint8_t air_particulate_size;
// 	uint8_t air_quality;
// 	uint8_t battery_level;
// 	uint8_t status_low_battery;
// 	uint8_t charging_state;

// }DEV_STATUS_DATA;

/*****************************************************************************
 *        Function Extern
 *****************************************************************************/
extern hap_serv_t *AirPurifierServiceNew();
extern hap_serv_t *FilterMaintenanceServiceNew();
extern void reset_to_factory(int pin, void *data);
//extern  int get_dev_info_data(DEV_STATUS_DATA * dev_status);
extern int hap_air_update(HAP_AIRPURIFIER_T* p_AirSensorData);


extern int hap_inintialization_process();
void 	  hap_set_wlan_country_code(char *code);
/*****************************************************************************
 *        Global Variables Extern 
 *****************************************************************************/
extern hap_serv_t *hs_upgrade;
extern HAP_AIRPURIFIER_T  t_AirSensorData;
/***********************************************************************************/

#endif
