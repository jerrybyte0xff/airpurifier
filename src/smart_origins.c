/*****************************************************************************
 * @file    Air_Purifier.c
 * @brief   Air_Purifier APIs.
 * @author  Jerry
 * @date    2017/06/08
*****************************************************************************/
 /*******************************************************************************
 *	 @(#)Copyright (C) Kywoo Beijing Co., Ltd.
  ******************************************************************************/
/*===============================================================================
 *	INCLUDE
 *===============================================================================*/
#include "smart_origins.h"

#include <wmsysinfo.h>
#include "event_handler.h"
#include <push_button.h>
#include <generic_io.h>
 #include <psm-v2.h>


#define UART_FROM_MCU_BUFFER_SIZE 50

/*===============================================================================
 *	Global Variables 
 *===============================================================================*/
/* Services of The Acceesory */
static hap_serv_t *hs_fliter_mantenance = NULL;
static hap_serv_t *hs_air_purifier = NULL;



hap_char_t *hc_status_active;
hap_char_t *hc_current_air_purifier_state;
hap_char_t *hc_target_air_purifier_state;
hap_char_t *hc_rotation_speed;
hap_char_t *hc_lock_physical_controls;
hap_char_t *hc_filter_change_indication;
hap_char_t *hc_filter_life_level;
hap_char_t *hc_rest_filter_indication;

///** The name of this accessory. */
//char accessory_name[] = HAP_ACCESSORY_CFG_NAME;
//
///** The name of the manufacturer. */
//char accessory_mfrr[128] = HAP_ACCESSORY_CFG_MANUFACTURE;
//
///** The model of this accessory. */
//char accessory_model[] = HAP_ACCESSORY_CFG_MODEL;
//
///** The serial-number of this accessory. */
//char accessory_default_serial_no[] = HAP_ACCESSORY_CFG_SERIAL_NO;

/** Manufacturer specific serial number */
char buffer[HAP_ACCESSORY_CFG_SERIAL_NO_MAX]={0};



hap_serv_t *hs_upgrade = NULL;

/* This is the configuration for the primary accessory and is a must for an HAP application. */
hap_acc_cfg_t accessory_cfg;



/* UART handle*/
mdev_t *uart_dev;

/* UART Scan Thread handle*/
 os_thread_t uart_scan_thread_handle;
 os_thread_t app_thread_read;

/* UART Scan Thread Stack*/
os_thread_stack_define(uart_scan_thread_stack, 1024);
static os_thread_stack_define(app_stack_read, 1024);
/*===============================================================================
 *	Internal Funvtion
 *===============================================================================*/
hap_serv_t *AirPurifierServiceNew();
hap_serv_t *FilterMaintenanceServiceNew();


/*===============================================================================
 *	PROCESS FUNCTION
 *===============================================================================*/
#define RESET_TO_FACTORY_TIMEOUT	5000

input_gpio_cfg_t ps_board_reset_to_factory_button = {
	.gpio = GPIO_22, .type = GPIO_ACTIVE_LOW };

	
static void air_purifier_reset_to_factory_cb(int pin, void *data)
{
	psm_erase_and_init();
	pm_reboot_soc();
}

void configure_reset_to_factory_button()
{
	push_button_set_cb(ps_board_reset_to_factory_button,
			air_purifier_reset_to_factory_cb,
			RESET_TO_FACTORY_TIMEOUT, 0, NULL);
}

int air_purifierwrite(hap_char_t *chars[], hap_val_t vals[],
		hap_status_t status_codes[], int count, void *priv)
{
	return 1;
}



void SendToPowerBoard(uint8_t * iic_From_MCU_Buffer)
{

}


/*  Brief: This is the Function of uart scan thread 
*         This Function Get The State Of Device From The Information 
		  Transformed From Uart. 
*  Paramater[in]:os_thread_arg_t
*  Return: void 
*  Version: @jerry 2015/08/02
****************************************************************************/
 void uart_scan_task(os_thread_arg_t data)
{
	int len = 0;
	uint8_t sum_check = 0;
	// char cReadByte[100];

	uint8_t  uiUart_From_MCU_Buffer_Temp[UART_FROM_MCU_BUFFER_SIZE];
	uint8_t CommandValue=0;
	hap_val_t val;
	while(1)
	{

		memset(uiUart_From_MCU_Buffer_Temp, 0, UART_FROM_MCU_BUFFER_SIZE);
		len = uart_drv_read(uart_dev, uiUart_From_MCU_Buffer_Temp, UART_FROM_MCU_BUFFER_SIZE);
		 
		hap_d("#####uart_scan_task receive  [%s]: \n", uiUart_From_MCU_Buffer_Temp);
		
		/*   Data Check  */

		 if (len != (uiUart_From_MCU_Buffer_Temp[1]+2))
		{		
		 	hap_d("#####len error [%d]: \n", uiUart_From_MCU_Buffer_Temp[1]);
		 	if (len > uiUart_From_MCU_Buffer_Temp[1]+2)
		 		len = uiUart_From_MCU_Buffer_Temp[1]+2;
		 	else
		 		// continue;
		}
		sum_check = 0;
		for (int i = 2; i < len-1; i++)
		{
			sum_check = sum_check + uiUart_From_MCU_Buffer_Temp[i];
		}
		sum_check = ~sum_check + 1

		if (uiUart_From_MCU_Buffer_Temp[0] != 0x55 || uiUart_From_MCU_Buffer_Temp[len-1] != sum_check)
		{
		 	hap_d("##### sum_check error [%d]: \n", sum_check);
		 	// continue;

			
		 // uart_drv_write(uart_dev, cReadByte,sizeof(cReadByte));
		}


		CommandValue = uiUart_From_MCU_Buffer_Temp[2];
		switch (CommandValue)
		{
			case 0x01 :
			{
				hap_val_set_int(&val, uiUart_From_MCU_Buffer_Temp[3]*33);
				hap_char_set_val(hc_rotation_speed, &val);


				if (4 == uiUart_From_MCU_Buffer_Temp[3])
				{
					hap_val_set_int(&val, 1);
					hap_char_set_val(hc_target_air_purifier_state, &val);

				}
				else
				{
					hap_val_set_int(&val, 0);
					hap_char_set_val(hc_target_air_purifier_state, &val);
				}

			}
			break;
			default:
			break;

		}
		SendToPowerBoard(uiUart_From_MCU_Buffer_Temp);

	}

}

void i2c_powe_bd_rd(os_thread_arg_t data)
{
	uint8_t  iic_From_MCU_Buffer_Temp[UART_FROM_MCU_BUFFER_SIZE];
	int len;

	while (1) 
	{
		/* enable I2C port */
		i2c_drv_enable(i2c1);

		len = i2c_drv_read(i2c1, &iic_From_MCU_Buffer_Temp, UART_FROM_MCU_BUFFER_SIZE);
		hap_d("#####iic receive  [%s]: \n", iic_From_MCU_Buffer_Temp);

	}
}
 /*************************************************************************//**
 * @brief		HAP  Initialization Process
 *
 * @param[in]	void
 *
 * @return		Return Code
 *
 * @retval		WM_SUCCESS
 * @retval		WM_FAIL
 *
 * @outline		1.HAP configuration. <BR>
 * 				2.Initialize the HAP framework. <BR>
 * 				3.Register the accessory with the HAP Framework. <BR>
 * 				4.Creat The Accessory Service. <BR>
 * 				5.Add the  service to the above accessory. <BR>
 * 				6.Start non-ios provisioning service. <BR>
 *
 * @date        @Jerry 2016.07.07 New Creation
 *
 ****************************************************************************/
int hap_inintialization_process()
{
	/*-------------------------------*
	 * Variable Declaration
	 *-------------------------------*/

	/* HAP Accessory object. */
	hap_acc_t *ha;
	/* HAP Service object. */
	hap_serv_t *hs;
	/* Return Value. */
	int ret;
	/* HAP configuration */
	hap_cfg_t hap_cfg;
	/* prov_pin_for_non_ios_device */
	// char prov_pin_for_non_ios_device[] = PROV_PIN;
	/*-------------------------------*
	 * Initialize
	 *-------------------------------*/
	ret = WM_FAIL;

#ifdef APPCONFIG_DEBUG_ENABLE
	/* Initialize sysinfo module to get debugging information */
	sysinfo_init();
#endif /* APPCONFIG_DEBUG_ENABLE */





	/* HAP configuration */

	// accessory_cfg.name = shared_ldtp->device.dname_stem;
	accessory_cfg.name = HAP_ACCESSORY_CFG_NAME;
	/* Manufacturer specific model of the accessory. */
	//accessory_cfg.model = shared_ldtp->device.dmodel;
	accessory_cfg.model = HAP_ACCESSORY_CFG_MODEL;
	/* Manufacturer name. */
	//accessory_cfg.mfrr = shared_ldtp->device.dmfrr;
	accessory_cfg.mfrr = HAP_ACCESSORY_CFG_MANUFACTURE;
	/* Manufacturer specific serial number. */
	/* Needed to take out from  the persistent storage*/
	// default, in case serial failed to load
	accessory_cfg.serial_no = HAP_ACCESSORY_CFG_SERIAL_NO;


	/** External Protocol */
	// hap_add_wac_mfi_protocols(external_protocol,1);

	/*-------------------------------*
	 * Process
	 *-------------------------------*/
	/** Function to initialize MFG partition*/
	// mfg_psm_init();
	/** Get The Serial Number*/
	// ret = mfg_get_variable("serial_num", buffer, sizeof(buffer));
	// accessory_cfg.serial_no = shared_ldtp->device.dsn;
	// hap_d("serial_num of device: %s \n",shared_ldtp->device.dsn);

//	/* Change The MAX number of The Services, Characteristics and Accessories */
	// hap_get_configuration(&hap_cfg);
	// hap_cfg.num_acc = HAP_ACCESSORY_NO_MAX;
	// hap_cfg.num_serv = HAP_SERVICES_NO_MAX;
	// hap_cfg.num_char = HAP_CHARACTERISTICS_NO_MAX;
	// hap_set_configuration(&hap_cfg);
	


	/* Initialize the HAP framework */
//	setup_custom_mac_addr();
	ret = hap_init();
	if (ret != WM_SUCCESS) 
	{
		hap_e("#####***hap_init error HAP_ERRNO_BASE = %d HAP_ERR_NO_MEM= %d\n",ret,HAP_ERR_NO_MEM);
		hap_critical_error(-CRIT_ERR_APP, NULL);
	}

#ifdef APPCONFIG_DEBUG_ENABLE
	/* Initialize sysinfo module to get debugging information */
	sysinfo_init();
#endif /* APPCONFIG_DEBUG_ENABLE */

     configure_reset_to_factory_button();

     /* Init the UART */
	ret = uart_drv_init(UART1_ID, UART_8BIT);
	if (ret != WM_SUCCESS)
	{
		hap_e("#####uart_drv_init error\n");
		hap_critical_error(-CRIT_ERR_APP, NULL);
	}
	uart_drv_blocking_read(UART1_ID, true);
	uart_dev = uart_drv_open(UART1_ID, 38400);
	
	if (uart_dev == NULL)
	{
		hap_e("#####uart open error\n");
		hap_critical_error(-CRIT_ERR_APP, NULL);
	}
	/* Creat a Thread To Get The Device State From The Uart */
	ret = os_thread_create(&uart_scan_thread_handle,
				"uart_scan_task",
				uart_scan_task,
				0,
				&uart_scan_thread_stack,
				OS_PRIO_4);

	/* Initialize I2C Driver */
	i2c_drv_init(I2C1_PORT);

	/* I2C1_PORT is configured as slave */
	i2c1 = i2c_drv_open(I2C1_PORT, I2C_DEVICE_SLAVE
			    | I2C_SLAVEADR(I2C_SLV_ADDR >> 1));

	ret = os_thread_create(&app_thread_read,	/* thread handle */
			"i2c_powe_bd_rd",	/* thread name */
			i2c_powe_bd_rd,	/* entry function */
			0,	/* argument */
			&app_stack_read,	/* stack */
			OS_PRIO_4);	/* priority - medium low */


	/* Create a new accessory with the same configuration as above
	 * and the identify routine. */


	ha = hap_acc_new(&accessory_cfg, identify_routine, APP_FW_VERSION);
	// ha = hap_acc_new(&accessory_cfg, identify_routine); // for SDK 0.xx
	if (!ha) 
	{
		hap_e("#####hap_acc_new error\n");
		hap_critical_error(-CRIT_ERR_APP, NULL);
	}
	
//	/* Create and add characteristic for software revision */
//	hap_d("hap_acc_info_add_sw_ver");
//	ret = hap_acc_info_add_sw_ver(ha, APP_SW_VERSION);
//	if (ret != WM_SUCCESS)
//	{
//		hap_e("#####hap_acc_info_add_sw_ver error\n");
//		hap_critical_error(-CRIT_ERR_APP, NULL);
//	}

	/* Create The Air Purifier  Service */
	hap_d("AirPurifier ServiceNew");
	hs = AirPurifierServiceNew();
	if (!hs)
	{
		hap_e("##### Create Air Quality Sensor error\n");
		hap_critical_error(-CRIT_ERR_APP, NULL);
	}
	ret = hap_serv_set_ops_write(hs, air_purifierwrite);

	hap_serv_add_property(hs, HAP_SERV_PROP_PRIMARY);
	/* Add the The Air Purifier    service to the above accessory */
	ret = hap_acc_add_serv(ha, hs);
	if (ret != WM_SUCCESS)
	{
		hap_e("##### hap_acc_add_serv error\n");
		hap_serv_thermostat_delete(hs);
		hap_critical_error(-CRIT_ERR_APP, NULL);
	}


#if 1

	/* Create Filter Maintenance Service */
	hs = FilterMaintenanceServiceNew();
	if (!hs)
	{
		hap_e("#####Create Battery Service error\n");
		hap_critical_error(-CRIT_ERR_APP, NULL);
	}
	ret = hap_serv_set_ops_write(hs, air_purifierwrite);

	/* Add the Filter Maintenance Service to the above accessory */
	ret = hap_acc_add_serv(ha, hs);
	if (ret != WM_SUCCESS)
	{
		hap_serv_thermostat_delete(hs);
		hap_critical_error(-CRIT_ERR_APP, NULL);
	}

#endif
	/* Create and add the firmware upgrade service which is a Marvell
	 * extension to homekit. Please refer Developer Guide for more
	 * information about how this works.
	 */

//  devjun: cannot change FW_UPGRADE_TYPE_ALL to FW_UPGRADE_TYPE_URL because you have to change PP:
	hs = mext_serv_fw_upgrade_new(FW_UPGRADE_TYPE_URL, NULL);
//  devjun: So let's change PP:
//	hs = mext_serv_fw_upgrade_new(FW_UPGRADE_TYPE_URL, NULL);
	if (!hs)
	{
		hap_critical_error(-CRIT_ERR_APP, NULL);
	}

	ret = hap_acc_add_serv(ha, hs);
	if (ret != WM_SUCCESS)
	{
		mext_serv_fw_upgrade_delete(hs);
		hap_critical_error(-CRIT_ERR_APP, NULL);
	}
	hs_upgrade = hs;


	/* Register the accessory with the HAP Framework */
	ret = hap_acc_register(ha);
	if (ret != WM_SUCCESS) 
	{
		hap_acc_delete(ha);
		hap_critical_error(-CRIT_ERR_APP, NULL);
	}

#ifdef ICLOUD_ENABLED
	/* Enabled iCloud support if required */
	// hap_init_icloud_support();
#endif

// 	/** enable random pairing pin generator*/
// #if FIX_PIN != 1
// 	hap_enable_random_pin(NULL);
// #endif
// 	/** Function to initialize MFG partition*/
// 	mfg_psm_init();
// 	/* Start non-ios provisioning service  */

// 	mext_prov_init(prov_pin_for_non_ios_device);
// 	//hap_d("PROV_PIN :12345678 ");

	return ret;
}




/****************************************************************************
 *
 *  Brief: 			Get device _status data. (!!Not Used)
 *  Paramater[in]:	DEV_STATUS_DATA p_dev_status
 *  Return: 			WM_SUCCESS
 *  Version: 		@jerry 2016/06/30
 *
****************************************************************************/
// int get_dev_status_data(DEV_STATUS_DATA * p_dev_status)
// {

// 	p_dev_status->str_quality_name = hap_char_get_val(hc_air_quality_name).v.s;
// 	p_dev_status->air_particulate_density = hap_char_get_val(hc_air_particulate_density).v.f;
// 	p_dev_status->air_particulate_size = hap_char_get_val(hc_air_particulate_size).v.i;
// 	p_dev_status->air_quality = hap_char_get_val(hc_air_quality).v.i;
// 	p_dev_status->battery_level = hap_char_get_val(hc_battery_level).v.i;
// 	p_dev_status->status_low_battery = hap_char_get_val(hc_low_battery).v.i;
// 	p_dev_status->charging_state = hap_char_get_val(hc_charging_state).v.i;
	
	
// 	return WM_SUCCESS;
// }

/****************************************************************************
 *
 *  Brief: 			Perform Reset to Factory. 
 *  Paramater[in]:	pin data
 *  Return: 			WM_SUCCESS
 *  Version: 		@jerry 2016/06/04
 *
****************************************************************************/
void reset_to_factory(int pin, void *data)
{
	hap_mutex_get(OS_WAIT_FOREVER);
	hap_reset_to_factory();
	hap_mutex_put();
}
void hap_char_add_constraint_int_all(hap_char_t *hc, int min,int max,int step)
{
	hap_char_add_constraint_int(hc,CONSTRAINT_TYPE_MIN_VAL, min);
	hap_char_add_constraint_int(hc,CONSTRAINT_TYPE_MAX_VAL, max);
	hap_char_add_constraint_int(hc,CONSTRAINT_TYPE_STEPPING, step);
}
/****************************************************************************
 *
 *  brief: 	Creat Air Quality Sensor Service  and 
 * 			added the related characteristics to it . 
 *  return:		 hap_serv_t
 *  @param[in]:	void
 *  @date: 	@jerry 2016/06/30
 *
****************************************************************************/
hap_serv_t *AirPurifierServiceNew()
{
	/*-------------------------------*
	 * Variable Declaration
	 *-------------------------------*/
	int ret ;
	hap_char_t *hc;

	/*-------------------------------*
	 * Initialize
	 *-------------------------------*/
	 ret = WM_FAIL;

	 /*-------------------------------*
	 * Create service
	 *-------------------------------*/
	/* create the new service:air_quality */
	hs_air_purifier = hap_serv_new(HAP_SERV_TYPE_AIR_PURIFIER);

	if (!hs_air_purifier)
		return NULL;
	
   /*-------------------------------*
	* Create characteristics
	*-------------------------------*/
	hc_status_active = hap_char_status_active_new(0);

	ret = hap_serv_add_char(hs_air_purifier, hc_status_active);
	if (ret != WM_SUCCESS) 
	{
	hap_e("%d: %s", __LINE__, __FUNCTION__);
		hap_serv_delete(hs_air_purifier);
		return NULL;
	}


	hc = hap_char_uint8_new(HAP_CHAR_TYPE_CURRENT_AIR_PURIFIER_STATE,0, PERM_READ | PERM_EVENT_NOTIFY);
	hc_current_air_purifier_state = hc;
	if (!hc)
	{
		hap_e("%d: %s", __LINE__, __FUNCTION__);
		hap_serv_delete(hs_air_purifier);
		return NULL;
	}

	 hap_char_add_constraint_int_all(hc, 0,2,1);

	 ret = hap_serv_add_char(hs_air_purifier, hc);
	if (ret != WM_SUCCESS) 
	{
		hap_e("%d: %s", __LINE__, __FUNCTION__);
		hap_serv_delete(hs_air_purifier);
		return NULL;
	}


	hc = hap_char_uint8_new(HAP_CHAR_TYPE_TARGET_AIR_PURIFIER_STATE,0, PERM_READ | PERM_EVENT_NOTIFY);
	hc_target_air_purifier_state = hc;

	if (!hc)
	{
		hap_e("%d: %s", __LINE__, __FUNCTION__);
		hap_serv_delete(hs_air_purifier);
		return NULL;
	}
	 hap_char_add_constraint_int_all(hc, 0,1,1);

	 ret = hap_serv_add_char(hs_air_purifier, hc);
	if (ret != WM_SUCCESS) 
	{
		hap_serv_delete(hs_air_purifier);
		return NULL;
	}

	hc = hap_char_float_new(HAP_CHAR_TYPE_ROTATION_SPEED,0, PERM_RW | PERM_EVENT_NOTIFY);
	hc_rotation_speed = hc;

	if (!hc)
		{
		hap_e("%d: %s", __LINE__, __FUNCTION__);
		hap_serv_delete(hs_air_purifier);
		return NULL;
	}
	hap_char_add_constraint_float(hc,CONSTRAINT_TYPE_MIN_VAL,0);
	hap_char_add_constraint_float(hc,CONSTRAINT_TYPE_MAX_VAL,100);
	hap_char_add_constraint_float(hc,CONSTRAINT_TYPE_STEPPING,1);

	 ret = hap_serv_add_char(hs_air_purifier, hc);
	if (ret != WM_SUCCESS) 
	{
		hap_e("%d: %s", __LINE__, __FUNCTION__);
		hap_serv_delete(hs_air_purifier);
		return NULL;
	}

	hc = hap_char_uint8_new(HAP_CHAR_TYPE_LOCK_PHYSICAL_CONTROLS,0, PERM_RW | PERM_EVENT_NOTIFY);
	hc_lock_physical_controls = hc;
	if (!hc)
		return NULL;
	 hap_char_add_constraint_int_all(hc, 0,1,1);

	 ret = hap_serv_add_char(hs_air_purifier, hc);
	if (ret != WM_SUCCESS) 
	{
		hap_serv_delete(hs_air_purifier);
		hap_e("%d: %s", __LINE__, __FUNCTION__);
		return NULL;
	}


	hc = hap_char_name_new("Air Purifier");
	if (NULL == hc)
	{
		goto err;
	}
	// hap_char_add_description(hc, "Name");
	ret = hap_serv_add_char(hs_air_purifier, hc);
	if (ret != WM_SUCCESS)
	{
		goto err;
	}



	return hs_air_purifier;
err:
	hap_serv_delete(hs_air_purifier);
	return NULL;
}


/*************************************************************************//**
 * 
 * @brief 		Creat  Battery Service  and added the related characteristics to it 
 *
 * @param[in]	void
 *
 * @return	    hap_serv_t
 *
 * @date        @Jerry 2016.07.07 New Creation
 *
****************************************************************************/
hap_serv_t *FilterMaintenanceServiceNew()
{
	/*-------------------------------*
	 * Variable Declaration
	 *-------------------------------*/
	int ret ;
	hap_char_t *hc;

	/*-------------------------------*
	 * Initialize
	 *-------------------------------*/
	 ret = WM_FAIL;
	/*-------------------------------*
	 * Create service
	 *-------------------------------*/

	/* create the new service: FilterMaintenanceServiceNew */
	 hs_fliter_mantenance = hap_serv_new(HAP_SERV_TYPE_FILTER_MAINTENANCE);

	if (!hs_fliter_mantenance)
	{
		hap_e("%d: %s", __LINE__, __FUNCTION__);
		hap_serv_delete(hs_fliter_mantenance);
		return NULL;
	}

	hc = hap_char_uint8_new(HAP_CHAR_TYPE_FILTER_CHANGE_INDICATION,0, PERM_READ | PERM_EVENT_NOTIFY);
	hc_filter_change_indication = hc;
	if (!hc)
	{
		hap_e("%d: %s", __LINE__, __FUNCTION__);
		hap_serv_delete(hs_fliter_mantenance);
		return NULL;
	}

	 hap_char_add_constraint_int_all(hc, 0,1,1);

	 ret = hap_serv_add_char(hs_fliter_mantenance, hc);
	if (ret != WM_SUCCESS) 
	{
		hap_e("%d: %s", __LINE__, __FUNCTION__);
		hap_serv_delete(hs_fliter_mantenance);
		return NULL;
	}
#if 1
	hc = hap_char_float_new(HAP_CHAR_TYPE_FILTER_LIFE_LEVEL,0, PERM_READ | PERM_EVENT_NOTIFY);
	hc_filter_life_level = hc;

	if (!hc)
		{
		hap_e("%d: %s", __LINE__, __FUNCTION__);
		hap_serv_delete(hs_fliter_mantenance);
		return NULL;
	}
	hap_char_add_constraint_float(hc,CONSTRAINT_TYPE_MIN_VAL,0);
	hap_char_add_constraint_float(hc,CONSTRAINT_TYPE_MAX_VAL,100);
	hap_char_add_constraint_float(hc,CONSTRAINT_TYPE_STEPPING,1);

	 ret = hap_serv_add_char(hs_fliter_mantenance, hc);
	if (ret != WM_SUCCESS) 
	{
		hap_serv_delete(hs_fliter_mantenance);
		return NULL;
	}

	hc = hap_char_uint8_new(HAP_CHAR_TYPE_REST_FILTER_INDICATION,0, PERM_WRITE | PERM_EVENT_NOTIFY);
	hc_rest_filter_indication = hc;
	if (!hc)
		return NULL;
	 hap_char_add_constraint_int_all(hc, 1,1,1);

	 ret = hap_serv_add_char(hs_fliter_mantenance, hc);
	if (ret != WM_SUCCESS) 
	{
		hap_serv_delete(hs_fliter_mantenance);
		return NULL;
	}
#endif

	

	return hs_fliter_mantenance;
}

/*************************************************************************//**
 * @brief		Temperature Service  and added the related characteristics to it 
 *
 * @param[in]	HAP_AIR_T* 
 *
 * @return		hap_serv_t
 *
 *
 * @date        @Jerry 2016.07.29 New Creation
 *
 ****************************************************************************/
// hap_serv_t *TemperatureSensorServiceNew()
// {
// 	/*-------------------------------*
// 	 * Variable Declaration
// 	 *-------------------------------*/
// 	int ret ;

// 	/*-------------------------------*
// 	 * Initialize
// 	 *-------------------------------*/
// 	 ret = WM_FAIL;

// 	 /*-------------------------------*
// 	 * Create service
// 	 *-------------------------------*/

// 	/* create the new service: Temperature */
// 	hs_temperature = hap_serv_temperature_sensor_new(HAP_CHAR_INIT_CURRENT_TEMPERATURE);

// 	if (NULL == hs_temperature){
// 		return NULL;
// 	}

// 	hc_temperature = hap_serv_char_by_type(hs_temperature, HAP_CHAR_TYPE_CUR_TEMPERATURE);

// 	if (NULL == hc_temperature){
// 		hap_d("hc_temperature Not Found \n");
// 		return NULL;
// 	}

// 	if (WM_SUCCESS!=hap_char_add_unit(hc_temperature,"celsius")){
// 		hap_d("add unit to hc_temperature failed\n");
// 		return NULL;
// 	}

// 	/*-------------------------------*
// 	 * Create characteristics
// 	 *-------------------------------*/

// 	/* Create new characteristic of type name and added to the service */
// 	hc_temperature_name = hap_char_name_new("Temperature Sensor");
// 	if (NULL == hc_temperature_name){
// 		return NULL;
// 	}

// 	hap_char_add_description(hc_temperature_name, "Name");

// 	ret = hap_serv_add_char(hs_temperature, hc_temperature_name);

// 	if (ret != WM_SUCCESS){
// 		return NULL;
// 	}
	
// 	return hs_temperature;
// }

/*************************************************************************//**
 * @brief		Humidity Service  and added the related characteristics to it 
 *
 * @param[in]	HAP_AIR_T* 
 *
 * @return		hap_serv_t
 *
 *
 * @date        @Jerry 2016.07.29 New Creation
 *
 ****************************************************************************/
// hap_serv_t *HumiditySensorServiceNew()
// {
// 	/*-------------------------------*
// 	 * Variable Declaration
// 	 *-------------------------------*/
// 	int ret ;

// 	/*-------------------------------*
// 	 * Initialize
// 	 *-------------------------------*/
// 	 ret = WM_FAIL;

// 	 /*-------------------------------*
// 	 * Create service
// 	 *-------------------------------*/

// 	/* create the new service: Humidity */
// 	hs_humidity = hap_serv_humidity_sensor_new(HAP_CHAR_INIT_CURRENT_HUMIDITY);
// 	if (NULL == hs_humidity)
// 	{
// 		hap_d("hs_humidity Create error \n");
// 		return NULL;
// 	}

// 	hc_humidity = hap_serv_char_by_type(hs_humidity, HAP_CHAR_TYPE_RELATIVE_HUMIDITY);
// 	if (NULL == hc_humidity){
// 		hap_d("hc_humidity Not Found \n");
// 		return NULL;
// 	}

// 	hap_char_add_unit(hc_humidity,"percentage");

// 	/*-------------------------------*
// 	 * Create characteristics
// 	 *-------------------------------*/

// 	/* Create new characteristic of type name and added to the service */
// 	hc_humidity_name = hap_char_name_new("Humidity Sensor");
// 	if (NULL == hc_humidity_name)
// 	{
// 		return NULL;
// 	}
// 	hap_char_add_description(hc_humidity_name, "Name");
// 	ret = hap_serv_add_char(hs_humidity, hc_humidity_name);
// 	if (ret != WM_SUCCESS)
// 	{
// 		return NULL;
// 	}
	
// 	return hs_humidity;
// }
/*************************************************************************//**
 * @brief		Air Sensor Data Update Process
 *
 * @param[in]	HAP_AIR_T* p_AirSensorData
 *
 * @return		Return Code
 *
 * @retval		WM_SUCCESS
 * @retval		WM_FAIL
 *
 * @date        @Jerry 2016.07.07 New Creation
 *
 ****************************************************************************/
int hap_air_update(HAP_AIR_T* p_AirSensorData)
{
	/*-------------------------------*
	 * Variable Declaration
	 *-------------------------------*/
	hap_val_t t_HapData;
	// hap_char_t* p_HapChar;

	/*-------------------------------*
	 * Initialize
	 *-------------------------------*/
	 // p_HapChar = NULL;


	 /*-------------------------------*
	 * Process
	 *-------------------------------*/
	 /* Check Para */
	 if (NULL == p_AirSensorData  )
	 {

	 	return WM_FAIL;
	 }

	 /* Update Air Particulate Density */
	hap_val_set_float(&t_HapData, p_AirSensorData->rotation_speed);
	hap_char_set_val(hc_rotation_speed, &t_HapData);


	hap_val_set_uint8(&t_HapData, p_AirSensorData->air_status_active);
	hap_char_set_val(hc_status_active, &t_HapData);

	hap_val_set_uint8(&t_HapData, p_AirSensorData->current_air_purifier_state);
	hap_char_set_val(hc_current_air_purifier_state, &t_HapData);

	hap_val_set_uint8(&t_HapData, p_AirSensorData->target_air_purifier_state);
	hap_char_set_val(hc_target_air_purifier_state, &t_HapData);

	hap_val_set_uint8(&t_HapData, p_AirSensorData->lock_physical_controls);
	hap_char_set_val(hc_lock_physical_controls, &t_HapData);

	hap_val_set_uint8(&t_HapData, p_AirSensorData->filter_change_indication);
	hap_char_set_val(hc_filter_change_indication, &t_HapData);

	hap_val_set_float(&t_HapData, p_AirSensorData->filter_life_level);
	hap_char_set_val(hc_filter_life_level, &t_HapData);

	hap_val_set_uint8(&t_HapData, p_AirSensorData->rest_filter_indication);
	hap_char_set_val(hc_rest_filter_indication, &t_HapData);


	return WM_SUCCESS;
}








void hap_set_wlan_country_code(char *code){
		/** China */
	if (strcmp(code,"CN") == 0)
		hap_wlan_set_country(COUNTRY_CN);
		/** IC Canada */
	else if (strcmp(code,"CA") == 0)
		hap_wlan_set_country(COUNTRY_CA);
		/** Singapore */
	else if (strcmp(code,"SG") == 0)
		hap_wlan_set_country(COUNTRY_SG);
		/** ETSI */
	else if (strcmp(code,"EU") == 0)
		hap_wlan_set_country(COUNTRY_EU);
		/** Australia */
	else if (strcmp(code,"AU") == 0)
		hap_wlan_set_country(COUNTRY_AU);
		/** Republic Of Korea */
	else if (strcmp(code,"KR") == 0)
		hap_wlan_set_country(COUNTRY_KR);
		/** France */
	else if (strcmp(code,"FR") == 0)
		hap_wlan_set_country(COUNTRY_FR);
		/** Japan */
	else if (strcmp(code,"JP") == 0)
		hap_wlan_set_country(COUNTRY_JP);
		/** US FCC */
	else
		hap_wlan_set_country(COUNTRY_US);
}
