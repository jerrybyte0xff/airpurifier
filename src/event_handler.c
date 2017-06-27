/*
 *  Copyright (C) 2008-2014, Marvell International Ltd.
 *  All Rights Reserved.
 */

#include "event_handler.h"

#include <wm_os.h>


void hap_event_hap_services_started(void *data)
{
	hap_d("HAP Services started, custom register http handlers.....");

	// int ret_http =  0;

	// // ret_http = register_httpd_handlers();
	// if (ret_http != WM_SUCCESS)
	// 	hap_d("Failed to register HTTPD handlers");
}

/****************************************************************************
*  Brief: 			Callback Of Connected Event. 
*  Function: 		It Triggered When Device Connected The NetWork
*					*inform the device that the network connected
*					*resister Web-Service hander
*  Paramater[in]:	pin data
*  Return: 			WM_SUCCESS
*  Version: 		@jerry 2016/07/04 New
****************************************************************************/

void hap_event_sta_connected(void *data)
{
	/*-------------------------------*
	 * Variable Declaration
	 *-------------------------------*/
	/* Inform The Device That the net work connected */

	
	/*-------------------------------*
	 * Process
	 *-------------------------------*/
	/* TODO: send infor */
	hap_d("Network Connected.....");		
	/* resister the http  hander */
#if 0
	int ret_http =  0;

	ret_http = register_httpd_handlers();
	if (ret_http != WM_SUCCESS)
	hap_d("Failed to register HTTPD handlers");
#endif

	// devjun: shared ldtp in event handler

	/* check if just waced, leave 5 seconds for bonjour to finish */

	
}

/****************************************************************************
*  Brief: 			Callback Of Connected Failed Event. 
*  Function: 		It Triggered When Device Connected The NetWork
*					*inform the device that the network connected failed
*  Paramater[in]:	pin data
*  Return: 			WM_SUCCESS
*  Version: 		@jerry 2016/07/04 New
****************************************************************************/
void hap_event_sta_connect_failed(void *data)
{
	/* TODO: send infor */
	hap_d(" Network Connected Fail\r\n");
	
}
/****************************************************************************
*  Brief: 			Callback Of  link lost Event. 
*  Function: 		It Triggered When Device Connected The NetWork
*					*inform the device that the network link lost
*  Paramater[in]:	pin data
*  Return: 			WM_SUCCESS
*  Version: 		@jerry 2016/07/04 New
****************************************************************************/
void hap_event_sta_connecting(void *data)
{
	
}

void hap_event_sta_link_lost(void *data)
{
	
	/* TODO: */
}

/****************************************************************************
 * Brief: 		This function can be used to notify the user (through LEDs, etc.)
 * 				of some critical error in the system.
 * 				This particular application will however just stall when a critical
 * 				error has been encountered.
****************************************************************************/
// void hap_critical_error(hap_critical_errno_t errno, void *data)
// {
// 	hap_e("%d: %s", errno, hap_crit_err_msg(errno));
// 	while (1)
// 		;

// }

 
/****************************************************************************
*  Brief: 			This is the identify routine for the accessory. 
*  Paramater[in]:	hap_acc_t 
*  Return: 			WM_SUCCESS
*  Version: 		@jerry 2016/06/30
****************************************************************************/
int identify_routine(hap_acc_t *ha)
{
	hap_d("Identifying this Air Quality Monitor\r\n");
	
	return WM_SUCCESS;
}



void hap_event_acc_paired(void *data){

}
void hap_event_acc_unpaired(void *data){
}

void hap_event_pairing_added(void *data){

}


/****************************************************************************
*  Brief: 			This event is valid only if the application can display
					a random pairing PIN and has used hap_enable_random_-pin().
					It will be received whenever there is a new pairing PIN to
					 be displayed. Specifically, this will be received in 3 scenarios

 					1) Accessory boots up in unpaired state and then connects to a network.
 					2) An incorrect PIN is entered during pair setup.
 					3) Accessory is paired but then all controllers are removed using remove-pairing..
*  Paramater[in]:	hap_acc_t
*  Return: 			WM_SUCCESS
*  Version: 		@jerry 2016/06/30
****************************************************************************/

// char a_PinData[]= "101-00-101";
// void hap_event_random_pin_generated(void *data)
// {
// 	/*-------------------------------*
// 	 * Variable Declaration
// 	 *-------------------------------*/
// 	// int ret;
// //	char a_PinData[9]= "10100101";
// //	char * p_PinData = a_PinData;
// //	char * p_PinData_Untoken = (char *)data;
// //	int icount = 0;

// 	/*-------------------------------*
// 	 * Process
// 	 *-------------------------------*/
// 	hap_d("PIN: [%s]\r\n",data);

// 	if (!shared_ldtp) return;

// 	shared_ldtp->runtime.PIN_data = data;			// devjun:issue 4

// #if FIX_PIN==1
// 	shared_ldtp->runtime.PIN_data = a_PinData;
// #endif

// 	shared_ldtp->runtime.pin_changed = 1;
// 	if (data) {
// 		if (shared_ldtp->device.dnetwork_configured &&
// 				shared_ldtp->runtime.pin_shown_after_wac_or_waced_before_boot_up == 0) {
// 			shared_ldtp->runtime.pin_shown_after_wac_or_waced_before_boot_up = 1;
// 			shared_ldtp->runtime.setting_page_conf.type = PAGE_SETTINGS_PAIRING;
// 			shared_ldtp->runtime.is_setting_page_active = 1;
// 			shared_ldtp->runtime.display_page_entering = 1;
// 		}
// 	} else {
// 		if (shared_ldtp->runtime.is_setting_page_active &&
// 				(shared_ldtp->runtime.setting_page_conf.type == PAGE_SETTINGS_PAIRING ||
// 					shared_ldtp->runtime.setting_page_conf.type == PAGE_SETTINGS_WIFI_NOT_CONFIGURED)) {
// 			shared_ldtp->runtime.is_setting_page_active = 0;
// 			shared_ldtp->runtime.display_page_entering = 1;
// 		}
// 	}
// }


void hap_event_wac_started(void *data)
{
	
	
}
void hap_event_wac_timeout(void *data)
{
//	int ret = hap_restart_provisioning();
//	hap_d("\r\nIn timeout, hap_restart_provisioning: %d\r\n",ret);
	// shared_ldtp->runtime.wifi_status = WiFi_wac_stopped;
}
