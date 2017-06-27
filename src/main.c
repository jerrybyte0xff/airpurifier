/*****************************************************************************
 * @file    smart_origins.h
 * @brief   smart_origins Prototype
 * @author  jerry
 * @date    2016.7.01.
*****************************************************************************/

 #include "event_handler.h"
 #include "smart_origins.h"





int main()
{
	int ret;

	hap_inintialization_process();
	/* Create any other new accessories/services/characteristics as
	 * required. When all of these have been added, start the HAP
	 * framework.
	 * This call will actually start the WLAN and various network services
	 * like HTTP web server, mDNS service announcements, etc.
	 */
	ret = hap_start(HAP_CID_AIR_PURIFIER, HAP_CONFIG_VERSION);
	if (ret != WM_SUCCESS) {
		hap_critical_error(-CRIT_ERR_APP, NULL);
	}

	return 1;
	
}
