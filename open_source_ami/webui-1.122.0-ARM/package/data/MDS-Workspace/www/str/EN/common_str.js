//;*****************************************************************;
//;*****************************************************************;
//;**                                                             **;
//;**     (C) COPYRIGHT American Megatrends Inc. 2008-2009        **;
//;**                     ALL RIGHTS RESERVED                     **;
//;**                                                             **;
//;**  This computer software, including display screens and      **;
//;**  all related materials, are confidential and the            **;
//;**  exclusive property of American Megatrends, Inc.  They      **;
//;**  are available for limited use, but only pursuant to        **;
//;**  a written license agreement distributed with this          **;
//;**  computer software.  This computer software, including      **;
//;**  display screens and all related materials, shall not be    **;
//;**  copied, reproduced, published or distributed, in whole     **;
//;**  or in part, in any medium, by any means, for any           **;
//;**  purpose without the express written consent of American    **;
//;**  Megatrends, Inc.                                           **;
//;**                                                             **;
//;**                                                             **;
//;**                American Megatrends, Inc.                    **;
//;**           5555 Oakbook Parkway, Building 200                **;
//;**     Norcross,  Georgia - 30071, USA. Phone-(770)-246-8600.  **;
//;**                                                             **;
//;*****************************************************************;
//;*****************************************************************;

eLang.common_strings = {};

/*Dashboard*/
eLang.common_strings["STR_DASHBOARD_GETDEVICEID"] = "There was a problem while getting device ID";
eLang.common_strings["STR_DASHBOARD_GETBUILDDATE"] = "There was a problem while getting Firmware build date and time : ";
eLang.common_strings["STR_DASHBOARD_NETWORK_EDIT"] = "Edit";
eLang.common_strings["STR_DASHBOARD_NETWORK_VIEW"] = "View";

/*FRU Information*/
eLang.common_strings["STR_FRU_INFO_GETVAL"] = "There was a problem while getting FRU Information";

/*Configure Active Directory*/
eLang.common_strings["STR_CONF_AD_HEAD1"] = "Role Group ID";
eLang.common_strings["STR_CONF_AD_HEAD2"] = "Group Name";
eLang.common_strings["STR_CONF_AD_HEAD3"] = "Group Domain";
eLang.common_strings["STR_CONF_AD_HEAD4"] = "Group Privilege";
eLang.common_strings["STR_CONF_AD_RG_CNT"] = "Number of configured Role groups: ";
eLang.common_strings["STR_CONF_AD_ADENABLE_DESC"] = "To Configure Active Directory Server Settings, click 'Advanced Settings'";
eLang.common_strings["STR_CONF_AD_ADDISABLE_DESC"] = "The 'Active Directory' is currently disabled. To enable Active Directory and configure its settings. Click on 'Advanced Settings' button.";
eLang.common_strings["STR_CONF_AD_ADV_TITLE"] = "Advanced Active Directory Settings";
eLang.common_strings["STR_CONF_AD_ADV_ENABLEAD"] = "Active Directory Authentication";
eLang.common_strings["STR_CONF_AD_ADV_DOMAINNAME"] = "User Domain Name";
eLang.common_strings["STR_CONF_AD_ADV_TIMEOUT"] ="Time Out";
eLang.common_strings["STR_CONF_AD_ADV_DOMAINSRVR1"] = "Domain Controller Server Address1";
eLang.common_strings["STR_CONF_AD_ADV_DOMAINSRVR2"] = "Domain Controller Server Address2";
eLang.common_strings["STR_CONF_AD_ADV_DOMAINSRVR3"] = "Domain Controller Server Address3";
eLang.common_strings["STR_CONF_AD_ADV_SAVE"] = "Save";
eLang.common_strings["STR_CONF_AD_RG_ADD_TITLE"] = "Add Role Group";
eLang.common_strings["STR_CONF_AD_RG_MODIFY_TITLE"] = "Modify Role Group";
eLang.common_strings["STR_CONF_AD_RG_NAME"] = "Role Group Name";
eLang.common_strings["STR_CONF_AD_RG_DOMAIN"] = "Role Group Domain";
eLang.common_strings["STR_CONF_AD_RG_PRIV"] = "Role Group Privilege";

/*Configure Active Directory Error Strings*/
eLang.common_strings["STR_CONF_AD_GETINFO"] = " There was a problem while getting Active Directory information";
eLang.common_strings["STR_CONF_AD_SETINFO"] = " There was a problem while setting Active Directory information";
eLang.common_strings["STR_CONF_AD_RG_GETINFO"] = "There was a problem while getting role group information";
eLang.common_strings["STR_CONF_AD_RG_ADDINFO"] = "There was a problem while adding role group information";
eLang.common_strings["STR_CONF_AD_RG_MODINFO"] = "There was a problem while modifying role group information";
eLang.common_strings["STR_CONF_AD_RG_DELINFO"] = "There was a problem while deleting role group information";
eLang.common_strings["STR_CONF_AD_RG_ADMINPRIV"] ="You need to have administrator privileges to add/modify/delete role groups.";
eLang.common_strings["STR_CONF_AD_ADV_ADMINPRIV"] ="You need to have administrator privileges to configure Active Directory settings.";
eLang.common_strings["STR_CONF_AD_RG_ERR1"] ="Please select a slot in the Role Group list";
eLang.common_strings["STR_CONF_AD_RG_ERR2"] ="There is no role group configured in the slot you selected. Please select a different role group";
eLang.common_strings["STR_CONF_AD_RG_ERR3"] ="The Role Group Name already exists with this Role Group Domain";
eLang.common_strings["STR_CONF_AD_CONFIRM1"] ="This slot has already been configured with a role group. Would you like to modify that role group instead?";
eLang.common_strings["STR_CONF_AD_CONFIRM2"] ="This slot is currently empty. Would you like to add a new role group?";
eLang.common_strings["STR_CONF_AD_RG_DELETE_SUCCESS"] ="The Role Group has been deleted";
eLang.common_strings["STR_CONF_AD_RG_ADV_SAVE_SUCCESS"] ="The Active Directory configuration has been successfully set";
eLang.common_strings["STR_CONF_AD_ADV_INVALID_DOMAINNAME"] ="Invalid User Domain Name!";
eLang.common_strings["STR_CONF_AD_ADV_INVALID_DOMAINSRVR1"] ="Invalid Domain Controller Server Address1";
eLang.common_strings["STR_CONF_AD_ADV_INVALID_DOMAINSRVR2"] ="Invalid Domain Controller Server Address2";
eLang.common_strings["STR_CONF_AD_ADV_INVALID_DOMAINSRVR3"] ="Invalid Domain Controller Server Address3";
eLang.common_strings["STR_CONF_AD_ADV_INVALID_DOMAINSRVR"] ="Please enter at least one Domain controller server address";
eLang.common_strings["STR_CONF_AD_ADV_DIFF_DOMAINSRVR"] = "The Domain Controller Server's address should be different for the three servers";
eLang.common_strings["STR_CONF_AD_RG_INVALID_RGNAME"] ="Invalid Role Group Name!";
eLang.common_strings["STR_CONF_AD_RG_INVALID_RGDOMAIN"] ="Invalid Role Group Domain Name!";

/*Role Group Strings*/
eLang.common_strings["STR_CONF_RG_TITLE_1"] = "Add Role Group";
eLang.common_strings["STR_CONF_RG_TITLE_2"] = "Modify Role Group";
eLang.common_strings["STR_CONF_RG_CNT"] = "Number of configured Role groups: ";
eLang.common_strings["STR_CONF_RG_HEAD1"] = "Role Group ID";
eLang.common_strings["STR_CONF_RG_HEAD2"] = "Group Name";
eLang.common_strings["STR_CONF_RG_HEAD4"] = "Group Privilege";
eLang.common_strings["STR_CONF_RG_NAME"] = "Role Group Name";
eLang.common_strings["STR_CONF_RG_PRIV"] = "Role Group Privilege";
eLang.common_strings["STR_CONF_RG_SEARCHBASE"] = "Role Group Search Base";
eLang.common_strings["STR_CONF_RG_GETINFO"] = "There was a problem while getting role group information";
eLang.common_strings["STR_CONF_RG_SETINFO"] = "There was a problem while configuring role group information";
eLang.common_strings["STR_CONF_RG_DELINFO"] = "There was a problem while deleting role group information";
eLang.common_strings["STR_CONF_RG_ERR1"] ="Please select a slot in the Role Group list";
eLang.common_strings["STR_CONF_RG_ERR2"] ="There is no role group configured in the slot you selected. Please select a different role group";
eLang.common_strings["STR_CONF_RG_ERR3"] ="Role Group Name already exists with this Role Group Search Base";
eLang.common_strings["STR_CONF_RG_CONFIRM1"] ="This slot has already been configured with a role group. Would you like to modify that role group instead?";
eLang.common_strings["STR_CONF_RG_CONFIRM2"] ="This slot is currently empty. Would you like to add a new role group?";
eLang.common_strings["STR_CONF_RG_DELETE_SUCCESS"] ="Role Group has been deleted";
eLang.common_strings["STR_CONF_RG_SAVE_SUCCESS_1"] ="Role Group was added successfully";
eLang.common_strings["STR_CONF_RG_SAVE_SUCCESS_2"] ="Role Group was modified successfully";

/*Configure LDAP*/
eLang.common_strings["STR_CONF_LDAP_DESC_1"] = "To Configure LDAP/E-Directory Server Settings. Click on 'Advanced Settings' button";
eLang.common_strings["STR_CONF_LDAP_DESC_0"] = "LDAP/E-Directory is currently disabled. To enable LDAP/E-Directory and configure its settings. Click on 'Advanced Settings' button.";
eLang.common_strings["STR_CONF_LDAP_GETINFO"] = " There was a problem while getting LDAP/E-Directory information";
eLang.common_strings["STR_CONF_LDAP_SETINFO"] = " There was a problem while setting LDAP/E-Directory information";
eLang.common_strings["STR_CONF_LDAP_SAVE_SUCCESS"] ="LDAP/E-Directory configuration has been successfully set";
eLang.common_strings["STR_CONF_LDAP_RG_SEARCHBASE"] = "Group Search Base";
eLang.common_strings["STR_CONF_LDAP_TITLE"] = "Advanced LDAP/E-Directory Settings";
eLang.common_strings["STR_CONF_LDAP_ENABLE"] = "LDAP/E-Directory Authentication";
eLang.common_strings["STR_CONF_LDAP_IP"] = "IP Address";
eLang.common_strings["STR_CONF_LDAP_PORT"] ="Port";
eLang.common_strings["STR_CONF_LDAP_BINDDN"] ="Bind DN";
eLang.common_strings["STR_CONF_LDAP_SEARCHBASE"] ="Search Base";
eLang.common_strings["STR_CONF_LDAP_ATTRIBUTE_OF_USER_LOGIN"] ="Attribute of User Login";
eLang.common_strings["STR_CONF_LDAP_INVALID_BINDDN"] ="Invalid Bind DN";
eLang.common_strings["STR_CONF_LDAP_INVALID_ATTRIBUTE_OF_USERLOGIN"] ="Invalid Attribute of User Login";

/*Configure Local Media Area*/
eLang.common_strings["STR_CONF_LMEDIA_CFG_GETVAL"] = "There was a problem while getting local media configuration";
eLang.common_strings["STR_CONF_LMEDIA_CFG_SETVAL"] = "There was a problem while setting local media configuration";
eLang.common_strings["STR_CONF_LMEDIA_CFG_TITLE"] = "Advanced Local Media Settings";
eLang.common_strings["STR_CONF_LMEDIA_DESC_1"] = "To Configure Local Media Settings. Click on 'Advanced Settings' button";
eLang.common_strings["STR_CONF_LMEDIA_DESC_0"] = "Local Media is currently disabled. To enable Local media settings. Click on 'Advanced Settings' button.";
eLang.common_strings["STR_CONF_LMEDIA_ENABLE"] = "Local Media Support";
eLang.common_strings["STR_CONF_LMEDIA_CFG_CONFIRM"] = "Changing the Local media configuration would require to close all the existing virtual media redirection session and restarting them. Click OK to proceed.";
eLang.common_strings["STR_CONF_LMEDIA_CFG_SUCCESS"] = "Local media configuration was saved successfully";
eLang.common_strings["STR_CONF_LMEDIA_IMG_GETVAL"] = "There was a problem while getting local media area contents";
eLang.common_strings["STR_CONF_LMEDIA_IMG_SETVAL"] = "There was a problem while setting local media image";
eLang.common_strings["STR_CONF_LMEDIA_IMGTYPE"] = "Image Type";
eLang.common_strings["STR_CONF_LMEDIA_IMGNAME"] = "Image Name";
eLang.common_strings["STR_CONF_LMEDIA_IMGINFO"] = "Image Information";
eLang.common_strings["STR_CONF_LMEDIA_IMGFILE"] = "Image File";
eLang.common_strings["STR_CONF_LMEDIA_IMG_CNT"] = "Number of available Images: ";
eLang.common_strings["STR_CONF_LMEDIA_ERR1"] = "Please select a slot in the Image list";
eLang.common_strings["STR_CONF_LMEDIA_ERR2"] = "There is no Image available in the slot you selected.";
eLang.common_strings["STR_CONF_LMEDIA_CONFIRM1"] = "This slot has already been configured with an image. Would you like to replace that Image instead?";
eLang.common_strings["STR_CONF_LMEDIA_CONFIRM2"] = "This slot is currently empty. Would you like to add a new Image?";
eLang.common_strings["STR_CONF_LMEDIA_ADD_IMAGE"] = "Add Image";
eLang.common_strings["STR_CONF_LMEDIA_REPLACE_IMAGE"] = "Replace Image";
eLang.common_strings["STR_CONF_LMEDIA_IMG_SUCCESS1"] = "Image was added successfully";
eLang.common_strings["STR_CONF_LMEDIA_IMG_SUCCESS2"] = "The image is replaced successfully";
eLang.common_strings["STR_CONF_LMEDIA_IMG_SUCCESS3"] = "The image has been deleted";
eLang.common_strings["STR_CONF_LMEDIA_CNFM_UPLOAD1"] = "More time will be taken to upload the image. In the meantime, no other rpc request will be served. \nDo you want to continue?";
eLang.common_strings["STR_CONF_LMEDIA_CNFM_UPLOAD2"] = "Click OK if you want to continue overwriting the existing image \n NOTE: It will take more time to upload an image. In meantime, no other rpc request will be served.";
eLang.common_strings["STR_CONF_LMEDIA_INVALID_FILE1"] = "Please browse the Image file.";
eLang.common_strings["STR_CONF_LMEDIA_INVALID_FILE2"] = "The image file already exists, Please browse some other image file.";
eLang.common_strings["STR_CONF_LMEDIA_INVALID_CDIMG"] = "CD/DVD Image file should end with .iso";
eLang.common_strings["STR_CONF_LMEDIA_INVALID_FDIMG"] = "Floppy/Harddisk Image file should end with .img";

/*Configure Mouse Mode*/
eLang.common_strings["STR_CONF_MOUSE_GETVAL"] ="Error in finding the current status of host";
eLang.common_strings["STR_CONF_MOUSE_SETVAL"] ="Error in setting Mouse Mode";
eLang.common_strings["STR_CONF_MOUSE_ABSOLUTE"] ="The current Mouse Mode is ABSOLUTE.";
eLang.common_strings["STR_CONF_MOUSE_RELATIVE"] ="The current Mouse Mode is RELATIVE.";
eLang.common_strings["STR_CONF_MOUSE_UNKNOWN"] ="Unknown Mouse Mode";
eLang.common_strings["STR_CONF_MOUSE_CONFIRM"] ="Are you sure you want to change the mouse mode? Please close any currently open redirection console, and then reopen for the change to be seen.";
eLang.common_strings["STR_CONF_MOUSE_ALERT1"] ="Please select a mouse mode";
eLang.common_strings["STR_CONF_MOUSE_ALERT2"] ="There is no change in the setting.";

/*Configure NCSI*/
eLang.common_strings["STR_CONF_NCSI_GETVAL"] = "There was a problem while getting NCSI configuration";
eLang.common_strings["STR_CONF_NCSI_SETVAL"] = "There was a problem while configuring NCSI Information";
eLang.common_strings["STR_CONF_NCSI_SAVE_SUCCESS"] = "The NCSI settings are saved successfully.";

/*Configure Network*/
eLang.common_strings["STR_CONF_NW_GETVAL"] = "There was a problem while getting Network configuration";
eLang.common_strings["STR_CONF_NW_SETVAL"] = "There was a problem while setting Network configuration.\n";
eLang.common_strings["STR_CONF_NW_GETERR1"] = "Could not get network settings";
eLang.common_strings["STR_CONF_NW_SETERR1"] = "Unable to set the supplied network parameters ";
eLang.common_strings["STR_CONF_NW_SAVE_CONFIRM"] = "This function may change the IP address of the device, and you may lose connectivity in this browser session.\nPlease reconnect using a new browser session after applying the changes.";
eLang.common_strings["STR_CONF_NW_RESET_TITLE"] = "Network configuration has been reset";
eLang.common_strings["STR_CONF_NW_RESET_DESC"] = "Network configuration has been reset successfully. It will take few seconds to bring up the interface again. Please close this browser session and open a new browser session to connect to the device with new IP. ";
eLang.common_strings["STR_CONF_NW_ERR_222"] = "Either IP Address or Default Gateway IP is out of range.";
eLang.common_strings["STR_CONF_NW_ERR_223"] = "Invalid IPv4 IP Address";
eLang.common_strings["STR_CONF_NW_ERR_224"] = "Invalid IPv4 Default Gateway";
eLang.common_strings["STR_CONF_NW_ERR_225"] = "Invalid IPv4 Subnet mask";
eLang.common_strings["STR_CONF_NW_ERR_226"] = "Invalid IPv6 IP Address";
eLang.common_strings["STR_CONF_NW_ERR_227"] = "Invalid IPv6 Default Gateway";
eLang.common_strings["STR_CONF_NW_ERR_469"] = "VLAN ID cannot be changed " + 
	"directly. To change the VLAN ID, disable the already enabled VLAN " +
	"configuration, then try enabling the VLAN configuration with the " +
	"new VLAN ID.";
eLang.common_strings["STR_CONF_NW_INVALID_V6SUBNET"] = "Invalid IPv6 Subnet prefix length";
eLang.common_strings["STR_CONF_NW_INVALID_VLANID"] = "Invalid VLAN ID";
eLang.common_strings["STR_CONF_NW_INVALID_VLANPRIORITY"] = "Invalid VLAN Priority";
eLang.common_strings["STR_CONF_NW_WARNING"] = "\n\nWARNING! Network changes may " +
	"affect the existing services availability on the network and they may " +
	"become unavailable on the newly configured network. Please refer to " +
	"'Services Configuration' page to modify the services availability " +
	"criteria after the network configuration change. " +
	"\n\nDo you wish to continue with this change?";

/*Configure Network Bonding*/
eLang.common_strings["STR_NW_BOND_IFC_1"] = "eth0";
eLang.common_strings["STR_NW_BOND_IFC_2"] = "eth1";
eLang.common_strings["STR_NW_BOND_IFC_3"] = "both";
eLang.common_strings["STR_CONF_NW_BOND_GETVAL"] = "There was a problem while getting Network Bond configuration.";
eLang.common_strings["STR_CONF_NW_BOND_SETVAL"] = "There was a problem while setting Network Bond configuration.";
eLang.common_strings["STR_CONF_NW_BOND_SETVAL_128"] = "Network bonding cannot " + 
	"be enabled, due to insufficient number of network interfaces.";
eLang.common_strings["STR_CONF_NW_BOND_SETVAL_130"] = "Bond cannot be enabled," + 
	" As VLAN is enabled for Slave interfaces. \nVLAN can be disabled, " +
	"Network under Configuration menu.";
eLang.common_strings["STR_CONF_NW_BOND_NOT_SUPPORT"] = "Network bonding cannot be supported, due to insufficient number of network interfaces.";
eLang.common_strings["STR_CONF_NW_BOND_CNFM_BOND"] = "Disabling bond will disable the Bonding VLAN configuration.\n";
eLang.common_strings["STR_CONF_NW_BOND_CNFM_AUTO1"] = "Auto configuration is" +
	" enabled, So all the services will be restarted automatically. Click OK" +
	" to proceed.";
eLang.common_strings["STR_CONF_NW_BOND_CNFM_AUTO0"] = "Auto configuration is" +
	"disabled, So the interfaces for services can be configured via IPMI " +
	"command. Click OK to proceed.";
eLang.common_strings["STR_CONF_NW_BOND_SAVE_SUCCESS"] = "The Network Bonding configuration are saved successfully.";

/*Configure Network Link*/
eLang.common_strings["STR_PHY_LINKSPEED_10"] = "10 Mbps";
eLang.common_strings["STR_PHY_LINKSPEED_100"] = "100 Mbps";
eLang.common_strings["STR_PHY_LINKSPEED_1000"] = "1000 Mbps";
eLang.common_strings["STR_PHY_DUPLEXMODE_HALF"] = "Half Duplex";
eLang.common_strings["STR_PHY_DUPLEXMODE_FULL"] = "Full Duplex";
eLang.common_strings["STR_CONF_PHY_GETVAL"] = "There was a problem while getting Network Link configuration.";
eLang.common_strings["STR_CONF_PHY_SETVAL"] = "There was a problem while setting Network Link configuration.";
eLang.common_strings["STR_CONF_PHY_SUPPORT_GETVAL"] = "Unable to get the supported capabilities for the LAN interface";
eLang.common_strings["STR_CONF_PHY_SAVE_SUCCESS"] = "The Network Link configuration has been saved successfully.";

/*Configure DNS*/
eLang.common_strings["STR_CONF_DNS_INVALID_HOST"] = "Invalid Host Name";
eLang.common_strings["STR_CONF_DNS_INVALID_DOMAIN"] = "Invalid Domain Name";
eLang.common_strings["STR_CONF_DNS_INVALID_DNS"] = "Invalid DNS Server Address";
eLang.common_strings["STR_CONF_DNS_V6DISABLE"] = "\nNOTE: IPv6 is disabled in network configuration.";
eLang.common_strings["STR_CONF_DNS_BLANK"] = "DNS Server addresses cannot be blank.";
eLang.common_strings["STR_CONF_DNS_DIFF"] = "DNS Server addresses should be different.";

/*Configure NTP*/
eLang.common_strings["STR_CONF_NTP_GETVAL"] = "There was a problem while getting NTP configuration";
eLang.common_strings["STR_CONF_NTP_SETVAL"] = "There was a problem while setting NTP configuration";
eLang.common_strings["STR_CONF_DATE_TIME_GETVAL"] = "There was a problem while getting Date and Time values.";
eLang.common_strings["STR_CONF_DATE_TIME_SETVAL"] = "There was a problem while setting Date and Time values.";
eLang.common_strings["STR_CONF_NTP_INVALID_HOUR"] = "Invalid Hour.";
eLang.common_strings["STR_CONF_NTP_INVALID_MINS"] = "Invalid Minutes.";
eLang.common_strings["STR_CONF_NTP_INVALID_SECS"] = "Invalid Seconds.";
eLang.common_strings["STR_CONF_NTP_INVALID_DATE"] = "Invalid Date, ";
eLang.common_strings["STR_CONF_NTP_DATE_RANGE"] = "Date is out of range. Please configure.";
eLang.common_strings["STR_CONF_NTP_INVALID_LEAP"] = "LEAP Year Feb contains only 29 days.";
eLang.common_strings["STR_CONF_NTP_INVALID_FEB"] = "Feb contains only 28 days.";
eLang.common_strings["STR_CONF_NTP_INVALID_MONTH"] = "Selected month contains only 30 days.";
eLang.common_strings["STR_CONF_NTP_INVALID_SERVER"] = "Invalid NTP Server";
eLang.common_strings["STR_CONF_NTP_SERVER_FAIL"] = "Temporary failure in synchronizing with NTP Server!";
eLang.common_strings["STR_CONF_NTP_CONFIRM"] = "The configuration changed! Do you want to save those changes?";
eLang.common_strings["STR_CONF_NTP_SAVE"] = "Configuration has been successfully set.";

/*Configure PAM Ordering*/
eLang.common_strings["STR_CONF_PAM_GETVAL"] = "There was a problem while getting PAM order configuration.";
eLang.common_strings["STR_CONF_PAM_SETVAL"] = "There was a problem while setting PAM order configuration.";
eLang.common_strings["STR_CONF_PAM_SUCCESS"] = "PAM Order was configured successfully.";
eLang.common_strings["STR_CONF_PAM_ERR"] = "There is no change in PAM Order with existing PAM Order.";
eLang.common_strings["STR_CONF_PAM_CONFIRM"] = "Web server will be restarted, Click OK to continue?";

/*Configure PEF -> Event Filter*/
eLang.common_strings["STR_CONF_PEF_ID"] = "PEF ID";
eLang.common_strings["STR_CONF_PEF_CONFIGURATION"] = "Filter Configuration";
eLang.common_strings["STR_CONF_PEF_ACTION"] = "Event Filter Action";
eLang.common_strings["STR_CONF_PEF_EVENT_SEVERITY"] = "Event Severity";
eLang.common_strings["STR_CONF_PEF_SENSOR_NAME"] = "Sensor Name";
eLang.common_strings["STR_CONF_PEF_CNT"] = "Configured Event Filter count: ";
eLang.common_strings["STR_CONF_PEF_GETINFO"] = "There was a problem while getting all PEF configuration";
eLang.common_strings["STR_CONF_PEF_DELINFO"] = "There was a problem in deleting the PEF configuration";
eLang.common_strings["STR_CONF_PEF_ERR1"] = "Please select a slot in the PEF list";
eLang.common_strings["STR_CONF_PEF_ERR2"] = "There is no PEF configured in the slot you selected.";
eLang.common_strings["STR_CONF_PEF_CONFIRM1"] = "This slot has already been configured with a PEF. Would you like to modify this PEF instead?";
eLang.common_strings["STR_CONF_PEF_CONFIRM2"] = "This slot is currently empty. Would you like to add a new PEF?";
eLang.common_strings["STR_CONF_PEF_DELETE_SUCCESS"] = "PEF entry has been deleted";

eLang.common_strings["STR_PEF_SEVERITY"] = "Unspecified";
eLang.common_strings["STR_PEF_SEVERITY_0"] = "Monitor";
eLang.common_strings["STR_PEF_SEVERITY_1"] = "Information";
eLang.common_strings["STR_PEF_SEVERITY_2"] = "Normal";
eLang.common_strings["STR_PEF_SEVERITY_3"] = "Non-Critical";
eLang.common_strings["STR_PEF_SEVERITY_4"] = "Critical";
eLang.common_strings["STR_PEF_SEVERITY_5"] = "Non-Recoverable";
eLang.common_strings["STR_PEF_POWER_1"] = "Power Down";
eLang.common_strings["STR_PEF_POWER_2"] = "Power Reset";
eLang.common_strings["STR_PEF_POWER_3"] = "Power Cycle";

/*Configure PEF -> Add or Modify Event Filter*/
eLang.common_strings["STR_ADD_PEF_TITLE"] = "Add Event Filter entry";
eLang.common_strings["STR_MODIFY_PEF_TITLE"] = "Modify Event Filter entry";
eLang.common_strings["STR_ADD_PEF_DESC"] = "Use this page to add new Event Filter entry. Click 'Add' to save the newly configured event filter.";
eLang.common_strings["STR_MODIFY_PEF_DESC"] = "Use this page to modify the existing Event Filter entry. Click 'Modify' to accept the modification.";
eLang.common_strings["STR_CONF_PEF_CFGINFO"] = "There was a problem while configuring PEF";
eLang.common_strings["STR_CONF_PEF_ADD_SUCCESS"] = "The PEF entry was added successfully!";
eLang.common_strings["STR_CONF_PEF_MOD_SUCCESS"] = "The PEF entry has been modified successfully";
eLang.common_strings["STR_PEF_EVENT_LOW"] = "Going Low";
eLang.common_strings["STR_PEF_EVENT_HIGH"] = "Going High";
eLang.common_strings["STR_PEF_ALL_EVENTS"] = "All Events";
eLang.common_strings["STR_PEF_SENSOR_EVENTS"] = "Sensor Events";
eLang.common_strings["STR_PEF_ALERT_ERR"] = "Event Filter Action, Alert must be enabled!";
eLang.common_strings["STR_PEF_EVT_TRIG_ERR"] = "Invalid Event Trigger.";
eLang.common_strings["STR_PEF_EVT1_MASK_ERR"] = "Invalid Event data 1 AND Mask.";
eLang.common_strings["STR_PEF_EVT1_CMP1_ERR"] = "Invalid Event Data 1 Compare 1.";
eLang.common_strings["STR_PEF_EVT1_CMP2_ERR"] = "Invalid Event Data 1 Compare 2.";
eLang.common_strings["STR_PEF_EVT2_MASK_ERR"] = "Invalid Event data 2 AND Mask.";
eLang.common_strings["STR_PEF_EVT2_CMP1_ERR"] = "Invalid Event Data 2 Compare 1.";
eLang.common_strings["STR_PEF_EVT2_CMP2_ERR"] = "Invalid Event Data 2 Compare 2.";
eLang.common_strings["STR_PEF_EVT3_MASK_ERR"] = "Invalid Event data 3 AND Mask.";
eLang.common_strings["STR_PEF_EVT3_CMP1_ERR"] = "Invalid Event Data 3 Compare 1.";
eLang.common_strings["STR_PEF_EVT3_CMP2_ERR"] = "Invalid Event Data 3 Compare 2.";

/*Configure PEF -> Alert Policy*/
eLang.common_strings["STR_POLICY_ENTRY"] = "Policy Entry #";
eLang.common_strings["STR_POLICY_NO"] = "Policy Number";
eLang.common_strings["STR_POLICY_SETTING"] = "Policy Configuration";
eLang.common_strings["STR_POLICY_SET"] = "Policy Set";
eLang.common_strings["STR_CHANNEL_NO"] = "Channel Number";
eLang.common_strings["STR_DEST_SELECT"] = "Destination Selector";
eLang.common_strings["STR_POLICY_CNT"] = "Configured Alert Policy count: ";
eLang.common_strings["STR_POLICY_GETVAL"] = "There was a problem while getting all Policy configuration";
eLang.common_strings["STR_POLICY_DELETEVAL"] = "There was a problem while deleting Alert Policy configuration";
eLang.common_strings["STR_CONF_POLICY_ERR1"] = "Please select a slot in the Alert Policy list";
eLang.common_strings["STR_CONF_POLICY_ERR2"] = "There is no alert policy configured in the slot you selected.";
eLang.common_strings["STR_CONF_POLICY_CONFIRM1"] = "This slot has already been configured with a Alert Policy. Would you like to modify this entry instead?";
eLang.common_strings["STR_CONF_POLICY_CONFIRM2"] = "This slot is currently empty. Would you like to add a new alert policy?";
eLang.common_strings["STR_CONF_POLICY_DELETE_SUCCESS"] = "Alert Policy entry has been deleted";

/*Configure PEF -> Add or Modify Alert Policy*/
eLang.common_strings["STR_ADD_ALERT_POLICY"] = "Add Alert Policy entry";
eLang.common_strings["STR_MODIFY_ALERT_POLICY"] = "Modify Alert Policy entry";
eLang.common_strings["STR_CONF_POLICY_ADD_SUCCESS"] = "The Alert Policy entry was added successfully!";
eLang.common_strings["STR_CONF_POLICY_MOD_SUCCESS"] = "The Alert Policy was modified successfully";
eLang.common_strings["STR_POLICY_SETVAL"] = "There was a problem while setting Alert Policy configuration";
eLang.common_strings["STR_ALERT_STRING"] = "Alert String";
eLang.common_strings["STR_ALERT_STRING_KEY"] = "Alert String Key";
eLang.common_strings["STR_EVENT_SPECIFIC"] = "Event Specific";

eLang.common_strings["STR_POLICY_SET_STR_0"] = "Always send alert to this destination";
eLang.common_strings["STR_POLICY_SET_STR_1"] = "If alert to previous destination was successful, do not send alert to this destination. Proceed to next entry in this policy set.";
eLang.common_strings["STR_POLICY_SET_STR_2"] = "If alert to previous destination was successful, do not send alert to this destination. Do not process any more entries in this policy set.";
eLang.common_strings["STR_POLICY_SET_STR_3"] = "If alert to previous destination was successful, do not send alert to this destination. Proceed to next entry in this policy set that is to a different channel.";
eLang.common_strings["STR_POLICY_SET_STR_4"] = "If alert to previous destination was successful, do not send alert to this destination. Proceed to next entry in this policy set that is to a different destination type.";

/*Configure PEF -> LAN Destination*/
eLang.common_strings["STR_LAN_DEST_HEAD1"] = "LAN Destination";
eLang.common_strings["STR_LAN_DEST_HEAD2"] = "Destination Type";
eLang.common_strings["STR_LAN_DEST_HEAD3"] = "Destination Address";
eLang.common_strings["STR_LAN_DEST_CNT"] = "Configured LAN Destination count: ";
eLang.common_strings["STR_LAN_DEST_GETVAL"] = "There was a problem while getting all LAN Destination";
eLang.common_strings["STR_LAN_DEST_DELETEVAL"] = "There was a problem while deleting LAN Destination";
eLang.common_strings["STR_LAN_DEST_ALERT_FAILURE"] ="There was a problem while sending test alerts.";
eLang.common_strings["STR_LAN_DEST_ERR1"] = "Please select a slot in the LAN destination list";
eLang.common_strings["STR_LAN_DEST_ERR2"] = "There is no LAN destination configured in the slot you selected.";
eLang.common_strings["STR_LAN_DEST_CONFIRM1"] = "This slot has already been configured with a LAN destination. Would you like to modify this entry instead?";
eLang.common_strings["STR_LAN_DEST_CONFIRM2"] = "This slot is currently empty. Would you like to add a new LAN destination entry?";
eLang.common_strings["STR_LAN_DEST_DEL_SUCCESS"] = "The LAN destination entry has been deleted";
eLang.common_strings["STR_LAN_DEST_ALERT_SUCCESS"] ="A test alert has been sent to the destination. Please check to see if you have received the alert";

/*Configure PEF -> Add or Modify LAN Destination*/
eLang.common_strings["STR_ADD_LAN_DEST"] = "Add LAN Destination entry";
eLang.common_strings["STR_MODIFY_LAN_DEST"] = "Modify LAN Destination entry";
eLang.common_strings["STR_LAN_DEST_SETVAL"] = "There was a problem while setting LAN Destination";
eLang.common_strings["STR_LAN_DEST_ADD_SUCCESS"] = "The LAN destination entry was added successfully!";
eLang.common_strings["STR_LAN_DEST_MOD_SUCCESS"] = "The LAN Destination entry was modified successfully";

eLang.common_strings["STR_DEST_TYPE_0"] = "Snmp Trap";
eLang.common_strings["STR_DEST_TYPE_6"] = "Email Alert";
eLang.common_strings["STR_EMAIL_SUBJECT"] = "Subject";
eLang.common_strings["STR_EMAIL_MESSAGE"] = "Message";
eLang.common_strings["STR_LAN_DEST_ADDR_ERR"] = "Invalid Destination Address.";
eLang.common_strings["STR_LAN_NO_EMAIL_CONFIRM"] = "Email address is not configured for this User. Do you want to continue anyway?";
eLang.common_strings["STR_LAN_EMAIL_SUB_ERR"] = "The subject field should not be blank.";
eLang.common_strings["STR_LAN_EMAIL_MSG_ERR"] = "The message field should not be blank.";

/*Configure RADIUS*/
eLang.common_strings["STR_CONF_RADIUS_SAVE_SUCCESS"] = "The RADIUS configuration has been successfully set.";
eLang.common_strings["STR_CONF_RADIUS_GETVAL"] = "There was a problem while getting RADIUS values";
eLang.common_strings["STR_CONF_RADIUS_SETVAL"] = "There was a problem while configuring RADIUS values";

/*Configure Remote Session*/
eLang.common_strings["STR_CONF_RMT_SESS_GETVAL"] = "There was a problem while getting Remote Session configuration";
eLang.common_strings["STR_CONF_RMT_SESS_SETVAL"] = "There was a problem while configuring Remote Session Information";
eLang.common_strings["STR_CONF_RMT_SESS_CONFIRM"] = "On changing the settings, It will automatically close the existing remote redirection(KVM or Virtual Media) sessions, if needed. Do you want to proceed further?"
eLang.common_strings["STR_CONF_RMT_SESS_SAVE_SUCCESS"] = "The Remote Session settings are saved successfully.";
eLang.common_strings["STR_CONF_RMT_SESS_VMATTACH_0"] = "Attach";
eLang.common_strings["STR_CONF_RMT_SESS_VMATTACH_1"] = "Auto Attach";
eLang.common_strings["STR_CONF_RMT_SESS_KEYLANG_0"] = "English (EN)"; 
eLang.common_strings["STR_CONF_RMT_SESS_KEYLANG_1"] = "French (FR)"; 
eLang.common_strings["STR_CONF_RMT_SESS_KEYLANG_2"] = "German (DE)"; 
eLang.common_strings["STR_CONF_RMT_SESS_KEYLANG_3"] = "Spanish (ES)"; 
eLang.common_strings["STR_CONF_RMT_SESS_KEYLANG_INDEX_0"] = "EN"; 
eLang.common_strings["STR_CONF_RMT_SESS_KEYLANG_INDEX_1"] = "FR"; 
eLang.common_strings["STR_CONF_RMT_SESS_KEYLANG_INDEX_2"] = "DE"; 
eLang.common_strings["STR_CONF_RMT_SESS_KEYLANG_INDEX_3"] = "ES";

/*Configure Services*/
eLang.common_strings["STR_CONF_SERVICES_GETVAL"] = "There was a problem while getting services configuration";
eLang.common_strings["STR_CONF_SERVICES_SETVAL"] = "There was a problem while setting services configuration";
eLang.common_strings["STR_CONF_SERVICES_NAME"] = "Service Name";
eLang.common_strings["STR_CONF_SERVICES_STATE"] = "Current State";
eLang.common_strings["STR_CONF_SERVICES_IFC"] = "Interfaces";
eLang.common_strings["STR_CONF_SERVICES_NSPORT"] = "Nonsecure Port";
eLang.common_strings["STR_CONF_SERVICES_SECPORT"] = "Secure Port";
eLang.common_strings["STR_CONF_SERVICES_TIMEOUT"] = "Timeout";
eLang.common_strings["STR_CONF_SERVICES_MAXSESS"] = "Maximum Sessions";
eLang.common_strings["STR_CONF_SERVICES_ACTIVESESS"] = "Active Sessions";
eLang.common_strings["STR_CONF_SERVICES_CNT"] = "Number of Services: ";
eLang.common_strings["STR_CONF_SERVICES_ERR"] = "Please select a slot in the Services list";
eLang.common_strings["STR_CONF_SERVICES_MODIFY"] = "Modify Service";
eLang.common_strings["STR_CONF_SERVICES_SUCCESS"] = "Service was configured successfully";
eLang.common_strings["STR_CONF_SERVICES_SETERR_132"] = "Port value is already in use";
eLang.common_strings["STR_CONF_SERVICES_SETERR_134"] = "Invalid Maximum sessions or Active sessions";
eLang.common_strings["STR_CONF_SERVICES_CONFIRM"] = "On changing the configuration, already opened sessions for the service will be affected, also the service will be restarted. Click OK to continue?";
eLang.common_strings["STR_CONF_SERVICES_CNFMWEB"] = "\nNOTE: Login session will be logged out.";
eLang.common_strings["STR_CONF_SERVICES_CNFMSSH"] = "\nNOTE: Configured timeout value is applied to Telnet service also.";
eLang.common_strings["STR_CONF_SERVICES_CNFMTELNET"] = "\nNOTE: Configured timeout value is applied to SSH service also.";

/*Configure SMTP*/
eLang.common_strings["STR_CONF_SMTP_GETVAL"] = "There was a problem while getting SMTP configuration.";
eLang.common_strings["STR_CONF_SMTP_SETVAL"] = "There was a problem while setting SMTP configuration.";
eLang.common_strings["STR_CONF_SMTP_SAVE_SUCCESS"] ="SMTP Server configuration has been modified successfully.";
eLang.common_strings["STR_CONF_SMTP_SENDERADDR"] = "Sender Address";
eLang.common_strings["STR_CONF_SMTP_INVALID_MACHINENAME"] = "Invalid Machine Name format";
eLang.common_strings["STR_CONF_SMTP_PRIMARY"] = "Primary SMTP Server - ";
eLang.common_strings["STR_CONF_SMTP_SECONDARY"] = "Secondary SMTP Server - ";
eLang.common_strings["STR_CONF_SMTP_DIFF_SMTPSRVR"] = "The Primary SMTP Server address should be different from Secondary SMTP Server address.";

/*SMTP Error Strings*/
eLang.common_strings["STR_SMTP_FAILURE1"] = "The Authentication type is not supported by SMTP Server";
eLang.common_strings["STR_SMTP_FAILURE2"] = "The SMTP Server Authentication Failure";
eLang.common_strings["STR_SMTP_FAILURE3"] = "Unable to connect to SMTP Server";

/*EMail Error Strings*/
eLang.common_strings["STR_EMAIL_FAILURE1"] = "No such user exists";
eLang.common_strings["STR_EMAIL_FAILURE2"] = "Email ID is not configured for the User";
eLang.common_strings["STR_EMAIL_TESTALERT"] = "Unable to send test alert to the configured email ID";
eLang.common_strings["STR_EMAIL_USER_ACCESS"] = "Unable to send test alert, since the user access is disabled";
eLang.common_strings["STR_EMAIL_FRGT_PSWD"] = "Unable to send newly generated password to the configured email ID";

/*Configure SSL*/
eLang.common_strings["STR_CONF_SSL_FILE_GETVAL"] = "There was a problem while getting SSL certificate status.";
eLang.common_strings["STR_CONF_SSL_INVALID_CERT1"] = "Please select a SSL Certificate file";
eLang.common_strings["STR_CONF_SSL_INVALID_CERT2"] = "SSL Certificate file should end with .pem";
eLang.common_strings["STR_CONF_SSL_INVALID_PRIVKEY1"] = "Please select a SSL Privacy key file";
eLang.common_strings["STR_CONF_SSL_INVALID_PRIVKEY2"] = "SSL Privacy key file should end with .pem";
eLang.common_strings["STR_CONF_SSL_UPLD_CONFIRM0"] = "Uploading a  new SSL "+
	"certificate will restart the HTTPs Service.\nDo you want to continue?";
eLang.common_strings["STR_CONF_SSL_UPLD_CONFIRM1"] = "A SSL Certificate already " +
	"exists. Loading a new SSL certificate will replace the existing " +
	"certificate and will restart the HTTPs Service. \nClick OK to continue?";
eLang.common_strings["STR_CONF_SSL_SAVE_SUCCESS"] = "The Certificate and Key are uploaded successfully.";
eLang.common_strings["STR_CONF_SSL_CERT_ERR1"] = "The SSL Certificate or key file does not exists";
eLang.common_strings["STR_CONF_SSL_CERT_ERR2"] = "The SSL Certificate is encrypted. Please upload unencrypted certificate.";
eLang.common_strings["STR_CONF_SSL_CERT_ERR3"] = "The SSL certificate validation failed. Please try to upload the certificate and key again.";
eLang.common_strings["STR_CONF_SSL_CERT_ERR4"] = "Uploading SSL certificate failed. Please try uploading the certificate and key again.";
eLang.common_strings["STR_CONF_SSL_CERT_ERR5"] = "The SSL certificate or key file size exceeds";
eLang.common_strings["STR_CONF_SSL_CERT_ERR128"] = "The SSL Certificate is " +
	"expired. Please upload valid certificate.\nNOTE: Please check the BMC " +
	"current time in NTP under Configuration menu.";
eLang.common_strings["STR_CONF_SSL_CERT_ERR129"] = "The SSL Certificate is " +
	"untrusted. Please upload trusted certificate.";
eLang.common_strings["STR_CONF_SSL_VALIDATE_ERR"] = "There was a problem while validating SSL Certificate and Key";
eLang.common_strings["STR_CONF_SSLCERTUPLOAD_ABORT"] = "By navigating to other pages the SSL upload process will be aborted.\nDo you want to continue?";
eLang.common_strings["STR_CONF_SSLCERTUPLOAD_ALERT"] = "Closing the web session causes the SSL upload process to be aborted!"
eLang.common_strings["STR_CONF_SSL_GNRT_CERT"] = "There was a problem while generating SSL certificate.";
eLang.common_strings["STR_CONF_SSL_GNRT_CONFIRM"] = "Generating a new SSL "+
	"certificate will restart the HTTPs service. \nClick 'OK' to continue?";
eLang.common_strings["STR_CONF_SSL_COMMON_NAME_ERR"] = "Invalid Common Name.";
eLang.common_strings["STR_CONF_SSL_ORGANIZATION_ERR"] = "Invalid Organization.";
eLang.common_strings["STR_CONF_SSL_ORG_UNIT_ERR"] = "Invalid Organization Unit.";
eLang.common_strings["STR_CONF_SSL_CITY_ERR"] = "Invalid City or Locality.";
eLang.common_strings["STR_CONF_SSL_STATE_ERR"] = "Invalid State or Province.";
eLang.common_strings["STR_CONF_SSL_COUNTRY_ERR"] = "Invalid Country.";
eLang.common_strings["STR_CONF_SSL_VALID_FOR_ERR"] = "Invalid Valid count of days.";
eLang.common_strings["STR_CONF_SSL_GNRT_SUCCESS"] = "The SSL Certificate was generated successfully.";
eLang.common_strings["STR_CONF_SSL_VIEW_CERT"] = "There was a problem while viewing SSL certificate.";

/*Configure System and Audit Log*/
eLang.common_strings["STR_CONF_SYS_AUDIT_GETVAL"] = "There was a problem while getting Audit and System Log values";
eLang.common_strings["STR_CONF_SYS_AUDIT_SETVAL"] = "There was a problem while configuring Audit and System Log values";
eLang.common_strings["STR_CONF_SYS_AUDIT_SAVE_SUCCESS"] = "The System and Audit Log configuration has been successfully set.";
eLang.common_strings["STR_CONF_SYS_AUDIT_INVALID_FILESIZE"] = "Invalid File Size.";
eLang.common_strings["STR_CONF_SYS_AUDIT_INVALID_ROTATECNT"] = "Invalid Rotate Count value.";
eLang.common_strings["STR_CONF_SYS_AUDIT_INVALID_LOGTYPE"] = "Invalid Log Type. Choose any one of the log type";

/*Configure Users*/
eLang.common_strings["STR_CONF_USER_ID"] = "UserID";
eLang.common_strings["STR_CONF_USER_NAME"] = "Username";
eLang.common_strings["STR_CONF_USER_PWORDSIZE"] = "Password Size";
eLang.common_strings["STR_CONF_USER_CNFMPWORD"] = "Confirm Password";
eLang.common_strings["STR_CONF_USER_ACCESS"] = "User Access";
eLang.common_strings["STR_CONF_USER_EMAIL"] = "Email ID";
eLang.common_strings["STR_CONF_USER_EMAIL_FORMAT"] = "Email Format";
eLang.common_strings["STR_CONF_USER_NWPRIV"] = "Network Privilege";
eLang.common_strings["STR_CONF_USER_SRLPRIV"] = "Serial Privilege";
eLang.common_strings["STR_CONF_USER_UPLOADSSH"] = "Uploaded SSH Key";
eLang.common_strings["STR_CONF_USER_NEWSSH"] = "New SSH Key";
eLang.common_strings["STR_ADD_USER"] = "Add User";
eLang.common_strings["STR_MODIFY_USER"] = "Modify User";

eLang.common_strings["STR_CONF_USER_CNT"] = "Number of configured users: ";
eLang.common_strings["STR_CONF_USER_GETINFO"] = " There was a problem while getting all the user information";
eLang.common_strings["STR_CONF_USER_SETINFO"] = "There was a problem while configuring a user. ";
eLang.common_strings["STR_CONF_USER_DELINFO"] = "There was a problem while deleting the user information";
eLang.common_strings["STR_CONF_EMAILFORMAT_GETINFO"] = " There was a problem while getting all email format information";
eLang.common_strings["STR_CONF_USER_CONFIRM1"] = "This slot has already been configured with an user. Would you like to modify this user instead?";
eLang.common_strings["STR_CONF_USER_CONFIRM2"] = "This slot is currently empty. Would you like to add a new user?";
eLang.common_strings["STR_CONF_USER_LOGCONFIRM"] = "Logged in User details are changed, Login session will be logged out automatically. Click OK to continue?";
eLang.common_strings["STR_CONF_USER_ERR1"] = "Please select a user in the user list";
eLang.common_strings["STR_CONF_USER_ERR3"] = "There is no user configured in the slot you selected.";
eLang.common_strings["STR_CONF_USER_ERR4"] = "This user is a logged in user and cannot be deleted.";
eLang.common_strings["STR_CONF_USER_ERR5"] = "Username already exists, please try with some other username";
eLang.common_strings["STR_CONF_USER_ERR6"] = "Email ID already exists, configure different Email ID";
eLang.common_strings["STR_CONF_USER_ERR7"] = "Reserved username, please try with some other username";
eLang.common_strings["STR_CONF_USER_DELETE_SUCCESS"] = "User has been deleted";
eLang.common_strings["STR_CONF_USER_SUCCESS0"] = "The user was added successfully!";
eLang.common_strings["STR_CONF_USER_SUCCESS1"] = "Modified user successfully";
eLang.common_strings["STR_CONF_SSH_VALIDATE_ERR"] = "The SSH key validation failed. Please try to upload the key again.";
eLang.common_strings["STR_CONF_SSH_ERR1"] = "The SSH key file does not exists.";
eLang.common_strings["STR_CONF_SSH_ERR4"] = "Uploading SSL key failed. Please try uploading the key again.";
eLang.common_strings["STR_CONF_SSH_ERR5"] = "The SSH key file size exceeds.";

/*Configure Users-SNMP*/
eLang.common_strings["STR_CONF_SNMP_STATUS"] = "SNMP Status";
eLang.common_strings["STR_CONF_SNMP_ACCESS"] = "SNMP Access";
eLang.common_strings["STR_CONF_SNMP_AUTHPROT"] = "Authentication Protocol";
eLang.common_strings["STR_CONF_SNMP_PRIVPROT"] = "Privacy Protocol";

eLang.common_strings["STR_CONF_SNMP_GETVAL"] = "There was a problem while getting SNMP configuration";
eLang.common_strings["STR_CONF_SNMP_SETVAL"] = "There was a problem while configuring SNMP Information";
eLang.common_strings["STR_READ_ONLY"] = "Read Only";
eLang.common_strings["STR_READ_WRITE"] = "Read Write";
eLang.common_strings["STR_CONF_SNMP_INVALID_COMM_STR"] = "Invalid Community String";
eLang.common_strings["STR_CONF_SNMP_INVALID_USER_ACCOUNT"] = "Invalid User Account";
eLang.common_strings["STR_CONF_SNMP_INVALID_AUTH_PASS"] = "Invalid Authentication Passphrase";
eLang.common_strings["STR_CONF_SNMP_INVALID_PRIV_PASS"] = "Invalid Privacy Passphrase";
eLang.common_strings["STR_CONF_SNMP_SAVE_SUCCESS"] ="SNMP configuration has been saved successfully.";


/*Configure Virtual Media Devices*/
eLang.common_strings["STR_CONF_VMEDIA_GETVAL"] = "There was a problem while getting virtual media device configuration";
eLang.common_strings["STR_CONF_VMEDIA_SETVAL"] = "There was a problem while setting virtual media device configuration";
eLang.common_strings["STR_CONF_VMEDIA_CONFIRM"] = "Changing the Virtual media configuration would require to close all the existing virtual media redirection session and restarting them. Click OK to proceed.";
eLang.common_strings["STR_CONF_VMEDIA_SAVE_SUCCESS"] = "The Virtual Media device configuration settings have been saved successfully.";

/*Event Log*/
eLang.common_strings["STR_EVENT_LOG_CNT"] ="Event Log: ";
eLang.common_strings["STR_EVENT_LOG_ENTRIES"] =" event entries";
eLang.common_strings["STR_EVENT_LOG_PAGES"] ="page(s)";
eLang.common_strings["STR_EVENT_LOG_HEAD1"] ="Event ID";
eLang.common_strings["STR_EVENT_LOG_HEAD2"] ="Time Stamp";
eLang.common_strings["STR_EVENT_LOG_HEAD3"] ="Sensor Name";
eLang.common_strings["STR_EVENT_LOG_HEAD4"] ="Sensor Type";
eLang.common_strings["STR_EVENT_LOG_HEAD5"] ="Description";
eLang.common_strings["STR_EVENT_LOG_TYPE0"] ="All Events";
eLang.common_strings["STR_EVENT_LOG_TYPE1"] ="System Event Records";
eLang.common_strings["STR_EVENT_LOG_TYPE2"] ="OEM Event Records";
eLang.common_strings["STR_EVENT_LOG_TYPE3"] ="BIOS Generated Events";
eLang.common_strings["STR_EVENT_LOG_TYPE4"] ="SMI Handler Events";
eLang.common_strings["STR_EVENT_LOG_TYPE5"] ="System Management Software Events";
eLang.common_strings["STR_EVENT_LOG_TYPE6"] ="System Software - OEM Events";
eLang.common_strings["STR_EVENT_LOG_TYPE7"] ="Remote Console software Events";
eLang.common_strings["STR_EVENT_LOG_TYPE8"] ="Terminal Mode Remote Console software Events";
eLang.common_strings["STR_EVENT_LOG_CLEAR_SUCCESS"] ="Sensor Event Log has been cleared";
eLang.common_strings["STR_EVENT_LOG_CLEAR_CONFIRM"] ="This will clear all the events in the log. Click Ok if you want to proceed?";
eLang.common_strings["STR_EVENT_LOG_GETVAL"] = "There was a problem while getting event logs";
eLang.common_strings["STR_EVENT_LOG_CLEARLOG"] = "There was a problem while clearing event logs";
eLang.common_strings["STR_EVENT_LOG_ASSERT"] ="Asserted";
eLang.common_strings["STR_EVENT_LOG_DEASSERT"] ="Deasserted";
eLang.common_strings["STR_EVENT_LOG_VIEW"] = "View All Event Logs";
eLang.common_strings["STR_UTC_OFFSET"] ="UTC Offset: ";
eLang.common_strings["STR_GMT"] ="(GMT";
eLang.common_strings["STR_PRE_INIT_TIMESTAMP"] = "Pre-init Timestamp";

/*Sensor Monitoring*/
eLang.common_strings["STR_SENSOR_GETVAL"] = "There was a problem while getting sensor values";
eLang.common_strings["STR_SENSOR_CNT"] ="Sensor Count: ";
eLang.common_strings["STR_SENSOR_SENSORS"] =" sensors";

/*System and Audit Log*/
eLang.common_strings["STR_LOG_LEVEL_TYPE1"] = "Alert";
eLang.common_strings["STR_LOG_LEVEL_TYPE2"] = "Critical";
eLang.common_strings["STR_LOG_LEVEL_TYPE3"] = "Error";
eLang.common_strings["STR_LOG_LEVEL_TYPE4"] = "Notification";
eLang.common_strings["STR_LOG_LEVEL_TYPE5"] = "Warning";
eLang.common_strings["STR_LOG_LEVEL_TYPE6"] = "Debug";
eLang.common_strings["STR_LOG_LEVEL_TYPE7"] = "Emergency";
eLang.common_strings["STR_LOG_LEVEL_TYPE8"] = "Information";
eLang.common_strings["STR_SYSTEM_EVENT_GETINFO"] = "There was a problem while getting System Events";
eLang.common_strings["STR_LOG_EVENT_ID"] = "Event ID";
eLang.common_strings["STR_LOG_TIMESTAMP"] = "Time Stamp";
eLang.common_strings["STR_LOG_HOSTNAME"] = "HostName";
eLang.common_strings["STR_LOG_DESCRIPTION"] = "Description";
eLang.common_strings["STR_SYSTEM_LOG_CNT"] ="This Filter: ";

/* Tooltip Strings */
eLang.common_strings["STR_REFRESH_TOOLTIP"] = "Reloads the current page";
eLang.common_strings["STR_PRINT_TOOLTIP"] = "Prints the current page";
eLang.common_strings["STR_LOGOUT_TOOLTIP"] = "Logout";
eLang.common_strings["STR_HELP_TOOLTIP"] = "The Help button allows the Help information to be displayed. If left open, it will automatically refresh to the appropriate contents for each UI page being viewed or it can be clicked whenever the user has a question regarding the page being viewed.";
eLang.common_strings["STR_USER_TOOLTIP"] = "Name and privileges of currently logged user";

/*Console Redirection*/
eLang.common_strings["STR_CONSOLE_JAVA"] = "Java Console";
eLang.common_strings["STR_CONSOLE_ACTIVEX"] = "ActiveX Console";
eLang.common_strings["STR_CONSOLE_ACTIVEX_GETVAL"] = "There was a problem while getting ActiveX configuration";
eLang.common_strings["STR_ACTIVEX_GETTOKEN_FAILURE"] = "Session token generation failed. Maximum session limit might have reached. Please close other sessions and try again.";
eLang.common_strings["STR_ACTIVEX_ADVISER_GETVAL"] = "There was a problem while getting Adviser configuration for ActiveX console";
eLang.common_strings["STR_ACTIVEX_VMEDIA_GETVAL"] = "There was a problem while getting VMedia configuration for ActiveX console";
eLang.common_strings["STR_ACTIVEX_FAILURE"] = "Unable to invoke the ActiveX object";

/*Server Power Control*/
eLang.common_strings["STR_SERVER_RETRYING"] ="Retrying...please wait. Retries Left : ";
eLang.common_strings["STR_SERVER_ERR1"] ="Performing soft power off failed. This may be because the host OS is taking unusual long time to gracefully shut down.";
eLang.common_strings["STR_SERVER_ERR2"] ="Performing power action failed.";
eLang.common_strings["STR_SERVER_HOSTOFF"] ="Host is currently off";
eLang.common_strings["STR_SERVER_HOSTON"] ="Host is currently on";
eLang.common_strings["STR_SERVER_GETSTATUS"] ="There was a problem while receiving current status of host";
eLang.common_strings["STR_SERVER_SETACTION"] ="There was a problem while receiving response for power action";
eLang.common_strings["STR_SERVER_STATE_NOTAVAIL"] ="Current state not available";
eLang.common_strings["STR_SERVER_POWER_ACTION_WAIT"] ="Performing Power Action..Please Wait";
eLang.common_strings["STR_SERVER_EXTERNAL_BMC"] ="Your external BMC configuration for power control.";
eLang.common_strings["STR_SERVER_FEATURE_CABLE"] ="The feature connector cables.";

/*Configure Triggers*/
eLang.common_strings["STR_CONF_TRIG_GETVAL"] = "There was a problem while " +
	"getting Event Trigger configuration";
eLang.common_strings["STR_CONF_TRIG_SETVAL"] = "There was a problem while " +
	"setting Event Trigger configuration for,";
eLang.common_strings["STR_CONF_TRIG_EVT0"] = "Temperature/Voltage Critical Events";
eLang.common_strings["STR_CONF_TRIG_EVT1"] = "Temperature/Voltage Non Critical Events";
eLang.common_strings["STR_CONF_TRIG_EVT2"] = "Temperature/Voltage Non Recoverable Events";
eLang.common_strings["STR_CONF_TRIG_EVT3"] = "Fan state changed Events";
eLang.common_strings["STR_CONF_TRIG_EVT4"] = "Watchdog Timer Events";
eLang.common_strings["STR_CONF_TRIG_EVT5"] = "Chassis Power on Event";
eLang.common_strings["STR_CONF_TRIG_EVT6"] = "Chassis Power off Event";
eLang.common_strings["STR_CONF_TRIG_EVT7"] = "Chassis Reset Event";
eLang.common_strings["STR_CONF_TRIG_EVT8"] = "Particular Date and Time Event";
eLang.common_strings["STR_CONF_TRIG_EVT9"] = "LPC Reset Event";
eLang.common_strings["STR_CONF_TRIG_EVT_ERR8"] = "Elapsed time value";
eLang.common_strings["STR_CONF_TRIG_SUCCESS"] = "Event Trigger was configured successfully.";
eLang.common_strings["STR_CONF_TRIG_KVMDISABLE"] = "\nNOTE: KVM Service should be enabled to perform auto-video recording.";
eLang.common_strings["STR_CONF_TRIG_YEAR2038"] = "\nNOTE: As a year 2038 problem exists, the acceptable maximum date range is 01-18-2038.";

/*Recorded Video*/
eLang.common_strings["STR_VIDEO_RCRD_FILENAME"] = "File Name";
eLang.common_strings["STR_VIDEO_RCRD_FILEINFO"] = "File Information";
eLang.common_strings["STR_VIDEO_RCRD_GETVAL"] = "There was a problem while getting Video file Information";
eLang.common_strings["STR_VIDEO_RCRD_DELVAL"] = "There was a problem while deleting Video file Information";
eLang.common_strings["STR_VIDEO_RCRD_FILE_CNT"] = "Number of available Video files : ";
eLang.common_strings["STR_VIDEO_RCRD_CNT_ZERO"] = "There are no recorded video files";
eLang.common_strings["STR_VIDEO_RCRD_FILESEL_ERR"] = "Select a video file entry";
eLang.common_strings["STR_VIDEO_RCRD_DEL_SUCCESS"] = "Video file is deleted successfully.";

/*Firmware Update*/
eLang.common_strings["STR_FW_UPDATE_CONFIRM1"] ="You will not be able to perform any other tasks until firmware upgrade is complete and the device is rebooted. Click Ok if you want to enter the update mode?";
eLang.common_strings["STR_FW_UPDATE_CONFIRM2"] ="Are you sure to abort the firmware upgrade process? The device will have to be reset in order to go back to normal operating mode";
eLang.common_strings["STR_FW_UPDATE_CONFIRM3"] ="New firmware image size is different from the existing firmware image size. 'Configuration' can not be preserved in this situation. Do you want to continue?";
eLang.common_strings["STR_FW_UPDATE_CONFIRM4"] ="Clicking 'OK' will start the actual upgrade operation, where the storage is written with the new firmware image. It is essential that the upgrade operation is not interrupted once it starts. Do you wish to proceed?";
eLang.common_strings["STR_FW_UPDATE_CONFIRM5"] ="Error in overwriting the files, Clicking 'OK' will start the actual upgrade operation without overwrite the files, where the storage is written with the new firmware image. It is essential that the upgrade operation is not interrupted once it starts. Do you wish to proceed?";
eLang.common_strings["STR_FW_UPDATE_ERR1"] ="Please enter a valid image file";
eLang.common_strings["STR_FW_UPDATE_ERR2"] ="Image Verification unsuccessful. Please check if you uploaded the correct Image. In order to try upgrading again you need to reset the device. Press OK to reset the device now.";
eLang.common_strings["STR_FW_UPDATE_ERR3"] ="Image Flashing unsuccessful. In order to try upgrading again you need to reset the device. Press OK to reset the device now.";
eLang.common_strings["STR_FW_UPDATE_ERR4"] ="Image Verification unsuccessful. Please check if you uploaded the correct SignImage Public Key. In order to try upgrading again you need to reset the device. Press OK to reset the device now.";
eLang.common_strings["STR_FW_FLASH_PROGRESS"] ="Flash is in Progress! Please try later";
eLang.common_strings["STR_FW_VERIFY_DIFFVERSION"] ="The firmware image has been verified. The uploaded image is a different version of the existing device firmware.";
eLang.common_strings["STR_FW_VERIFY_DIFFSIZE"] ="The firmware image has been verified. The uploaded image size is different from the existing device firmware.";
eLang.common_strings["STR_FW_VERIFY_SAME"] ="The firmware image has been verified. The uploaded image appears to be the same as the existing device firmware.";
eLang.common_strings["STR_FW_PREPARE_FLASH"] ="There was a problem while preparing device for firmware upgrade.";

eLang.common_strings["STR_FW_SIGNKEY_GETVAL"] ="There was a problem while getting SignImage public key information.";
eLang.common_strings["STR_FW_SIGNKEY_TITLE"] = "Upload  SignImage Public Key";
eLang.common_strings["STR_FW_SIGNKEY_INFO"] = "Uploaded  SignImage Public Key";
eLang.common_strings["STR_FW_NEW_SIGN_KEY"] = "New SignImage Public Key";
eLang.common_strings["STR_FW_SIGNKEY_INVALID1"] = "Please enter a SignImage Public key file";
eLang.common_strings["STR_FW_SIGNKEY_INVALID2"] = "SignImage Public key file name should end with .pem";
eLang.common_strings["STR_FW_SIGNKEY_SUCCESS"] = "SignImage Public Key uploaded successfully";
eLang.common_strings["STR_FW_SIGNKEY_ERR1"] = "The SignImage Public Key file does not exists";
eLang.common_strings["STR_FW_SIGNKEY_ERR3"] = "The SignImage Public Key validation failed. Please try to upload the SignImage Public Key again.";
eLang.common_strings["STR_FW_SIGNKEY_ERR4"] = "Uploading SignImage Public Key failed. Please try uploading the SignImage Public Key again.";
eLang.common_strings["STR_FW_SIGNKEY_ERR5"] = "The SignImage Public Key file size exceeds";
eLang.common_strings["STR_FW_SIGNKEY_VALIDATE_ERR"] = "There was a problem while validating SignImage Public Key";
eLang.common_strings["STR_FW_PRSRV_CFG_DESC"] = "Irrespect of the individual items to be preserved/overwritten in the below list.";

/*Preserve Configuration*/
eLang.common_strings["STR_PRSRV_GETVAL"] = "There was a problem while getting Preserve configuration information.";
eLang.common_strings["STR_PRSRV_SETVAL"] = "There was a problem while setting Preserve configuration information.";
eLang.common_strings["STR_PRSRV_SUCCESS"] = "Preserve configuration settings have been saved successfully.";
eLang.common_strings["STR_PRSRV_CFG_ITEM"] = "Preserve Configuration Item";
eLang.common_strings["STR_PRSRV_STATUS"] = "Preserve Status";
eLang.common_strings["STR_PRSRV_CFG_CNT"] = "Number of Preserved Items: ";

/*Restore Factory Defaults*/
eLang.common_strings["STR_RSTR_FCTRY_CNFRM_TITLE"] = "Restore Factory Defaults Confirmation";
eLang.common_strings["STR_RSTR_FCTRY_CNFRM"] = "Click OK if you want to continue restoring factory defaults.\n WARNING: Restoring Factory defaults will restart the device.";

/*System Administrator*/
eLang.common_strings["STR_CFG_ROOT_GETVAL"] ="There was a problem while getting Root user configuration.";
eLang.common_strings["STR_CFG_ROOT_SETVAL"] ="There was a problem while setting Root user configuration.";
eLang.common_strings["STR_CFG_ROOT_SUCCESS"] ="System Administrator configuration saved successfully.";

/*General Strings*/
eLang.common_strings["STR_PERMISSION_DENIED"] = "Permission denied to view this content.";
eLang.common_strings["STR_CONF_ADMIN_PRIV"] = "You need to have administrator privileges to perform this action.";
eLang.common_strings["STR_TIME_OUT"] = "The BMC seems to be taking too long to respond. Please check the cables and configuration";
eLang.common_strings["STR_NOT_CONFIGURE"] = "Not Configured";
eLang.common_strings["STR_NOT_AVAILABLE"] = "Not Available";
eLang.common_strings["STR_DATA"] = "Data ";
eLang.common_strings["STR_NOT_APPLICABLE"] = "N/A";
eLang.common_strings["STR_NOT_SUPPORT"] ="Not Supported";
eLang.common_strings["STR_NONE"] = "None";
eLang.common_strings["STR_MANUAL"] = "Manual";
eLang.common_strings["STR_AUTO"] = "Automatic";
eLang.common_strings["STR_BLANK"] = "&nbsp;";			// Empty string
eLang.common_strings["STR_EMPTY"] = " ";			// Empty string
eLang.common_strings["STR_NEWLINE"] = "\n";
eLang.common_strings['STR_HELP'] = 'HELP';
eLang.common_strings['STR_AND_ABOVE'] = ' and Above';
eLang.common_strings['STR_ANY'] = 'Any';
eLang.common_strings['STR_PROCESS_ABORT'] = "Ongoing process will be aborted.";
eLang.common_strings['STR_FWFLASH_ABORT'] = "Closing the web session during firmware flash causes device to be restarted.";
eLang.common_strings['STR_LOGOUT_SUCCESS'] = "You have been logged out successfully.";
eLang.common_strings["STR_EVENT_ENTRIES"] =" event entries";
eLang.common_strings["STR_SAVE"] = "Save";
eLang.common_strings["STR_ADD"] = "Add";
eLang.common_strings["STR_MODIFY"] = "Modify";
eLang.common_strings["STR_REPLACE"] = "Replace";
eLang.common_strings["STR_CANCEL"] = "Cancel";
eLang.common_strings["STR_ENABLED"] = "Enabled";
eLang.common_strings["STR_DISABLED"] = "Disabled";
eLang.common_strings["STR_HASH"] = "#";
eLang.common_strings["STR_PASSWORD"] = "Password";
eLang.common_strings["STR_PRESERVE"] = "Preserve";
eLang.common_strings["STR_OVERWRITE"] = "Overwrite";
eLang.common_strings["STR_INACTIVE"] = "Inactive";
eLang.common_strings["STR_ACTIVE"] = "Active";

/*General Error Strings*/
eLang.common_strings["STR_IPMI_ERROR"] = "  IPMI ERROR:";
eLang.common_strings["STR_HELP_INFO"] = " Refer 'Help' for more Information.";
eLang.common_strings["STR_INVALID_FIELDS"] = "Following are the Invalid field(s),\n";
eLang.common_strings["STR_INVALID_PORT"] = "Invalid Port Number";
eLang.common_strings["STR_INVALID_IP"] = "Invalid IP Address format.";
eLang.common_strings["STR_INVALID_TIMEOUT"] = "Invalid Time Out.";
eLang.common_strings["STR_INVALID_SERVERADDR"] = "Invalid Server Address, you can use either Hostname or IP Address.";
eLang.common_strings["STR_INVALID_PASSWORD"] = "Invalid Password.";
eLang.common_strings["STR_INVALID_CPWORD"] = "Password and confirmation password do not match";
eLang.common_strings["STR_INVALID_EMAILADDR"] = "Invalid Email Address.";
eLang.common_strings["STR_INVALID_DOMAIN"] ="Invalid Domain Name.";
eLang.common_strings["STR_INVALID_SEARCHBASE"] = "Invalid Search base.";
eLang.common_strings["STR_INVALID_RGNAME"] ="Invalid Role Group Name.";
eLang.common_strings["STR_INVALID_PAGENO"] ="Invalid Page Number.";
eLang.common_strings["STR_NETWORK_ERROR"] ="There is some problem in network connection.";

eLang.common_strings["STR_APP_STR_ALL_DEASSERTED"] = "All deasserted";
eLang.common_strings["STR_IPV4_ADDR0"] = "0.0.0.0";
eLang.common_strings["STR_IPV6_ADDR0"] = "::";

eLang.common_strings["NO_SEL_STRING"] = "There are no event log entries present at this time.";
eLang.common_strings["NO_SAL_STRING"] = "There are no entries in this log.";
eLang.common_strings["NO_FRU_STRING"] = "There are no FRU devices present in the system.";
eLang.common_strings["NO_SENSOR_STRING"] = "There are no sensors present in the system.";
eLang.common_strings["NO_ALERTENTRY_STRING"] = "There are no alert entries present in the system.";
eLang.common_strings["NO_PEFENTRY_STRING"] = "There are no event filter entries present in the system.";
eLang.common_strings["NO_LANALERTDESTS_STRING"] = "There are no LAN Alert destinations configured in the system.";
eLang.common_strings["INVALID_OFFSET"]= "<b>Invalid Offset for this SensorType</b>";

/*General - Confirmation messages*/
eLang.common_strings["STR_CONFIRM_DELETE"] = "Click OK if you want to continue deleting this entry.";
eLang.common_strings["STR_GENERAL_COMMAND_CONFIRM"] = "Are you sure to execute this command";
eLang.common_strings["STR_GENERAL_LOGOUT"] = "Click OK if you want to continue logging out.";
eLang.common_strings["STR_GENERAL_DISCONNECT"] = "Are you sure to disconnect from this server";
eLang.common_strings["STR_CONSOLE_CONNECTED"] = "The Active console window will be closed.";
eLang.common_strings["STR_SSLCERT_ABORT"] = "SSL certificate upload will be aborted.";
eLang.common_strings["STR_IPMI_LIBRARY_ERROR"] = "IPMI library not responding. Please check if the IPMI process is running.";

/*General - Strings*/
eLang.common_strings["STR_OK"] = "OK";
eLang.common_strings["STR_WAIT"] = "Loading";
eLang.common_strings["STR_WARNING"] = "Warning: ";
eLang.common_strings["STR_ON"] = "On";
eLang.common_strings["STR_OFF"] = "Off";
eLang.common_strings["STR_GO"] = "Go!";
eLang.common_strings["STR_LOGIN_BUTTON"]="Log In";
eLang.common_strings["STR_LOGIN_NOSPACE"]="Login";
eLang.common_strings["STR_LOGIN_PLEASE"]="Please Login";
eLang.common_strings["STR_SIDEBAR_LOGOUT"]   = "Log out";
eLang.common_strings["STR_SIDEBAR_DISCONNECT"]   = "Disconnect";
eLang.common_strings["STR_APPLYCHANGES"]      = "Apply Changes";
eLang.common_strings["STR_ENABLE"]      = "Enable";
eLang.common_strings["STR_DISABLE"]      = "Disable";
eLang.common_strings["STR_CONNECT"]      = "Connect";
eLang.common_strings["STR_DISCONNECT"]      = "Disconnect";
eLang.common_strings["STR_UPLOAD"]      = "Upload";
eLang.common_strings["STR_UNKNOWN"] ="Unknown";

/*Login Strings*/
eLang.common_strings["STR_LOGINWELCOME"]="Please type your user name and password";
eLang.common_strings["STR_LOGINFAILED"]="Login failed. Please try again";
eLang.common_strings["STR_LOGIN_ERROR"]="Invalid Authentication";
eLang.common_strings["STR_LOGIN_NOACCESS"]="No Privilege user";
eLang.common_strings["STR_LOGIN_BWSRNOTSPRT"]="NOT SUPPORTED BROWSER";
eLang.common_strings["STR_LOGIN_BWSRMSG"]="This software does not support this browser version." +  
	"Please check User's Guide for supported browsers."
eLang.common_strings["STR_LOGIN_SESSION_EXPIRED"]="Session Expired";
eLang.common_strings["STR_LOGIN"]="Authentication Required";
eLang.common_strings["STR_LOGIN_ERROR_3"]="Maximum number of sessions already in use";
eLang.common_strings["STR_USER_REQUIRED"] = "Username is required";
eLang.common_strings["STR_PWD_REQUIRED"] = "Password is required";
eLang.common_strings["STR_COOKIES_ENABLE"] = "Please enable the cookies in order to log into the system";
eLang.common_strings["STR_FIRST_COOKIES_BLOCK"] = "Cookies are enabled, First party cookies are still blocked";
eLang.common_strings["STR_USER_ROLE_ERROR"] = "Couldn't get role for the user";
eLang.common_strings["STR_LAN_CHANNEL_ERROR"] = "Couldn't retrieve lan channel information";
eLang.common_strings["STR_PROJECT_CFG_ERROR"] = "Couldn't retrieve project configuration";
eLang.common_strings["STR_USER_LENGTH"] = "Username length cannot exceed more than 800 characters";
eLang.common_strings["STR_PWD_LENGTH"] = "Password length cannot exceed more than 300 characters";

/*Password Reset Strings*/
eLang.common_strings["STR_RESET_PSWD_CONFIRM"] = "Click OK if you want to continue resetting the User's password.";
eLang.common_strings["STR_RESET_PSWD_SUCCESS"] = "New password has been sent to the configured Email";
eLang.common_strings["STR_RESET_PSWD_FAILURE"] = "There was a problem while resetting new password";
eLang.common_strings["STR_RESET_PSWD_FAILURE1"] = "Unable to reset the Password for the User";

/*Reset Strings*/
eLang.common_strings["STR_WEB_RESET_TITLE"] = "Web Server has been Reset";
eLang.common_strings["STR_WEB_RESET_DESC"] = "The web server has been restarted for the changes to take effect. Please close this browser session and open a new browser session to reconnect to the device.";
eLang.common_strings["STR_HTTPS_RESET_DESC"] = "The HTTPS Service has been restarted for the changes to take effect. Please close this browser session and open a new browser session to reconnect to the device.";
eLang.common_strings["STR_DEVICE_RESET_TITLE"] = "Device has been reset";
eLang.common_strings["STR_DEVICE_RESET_DESC"] = "The device has been reset. Please close this browser session and open a new browser session to reconnect to the device. <br/><br/>The device may take about a minute to boot up.";
eLang.common_strings["STR_DEVICE_FLASHMODE_TITLE"] = "Device in Flash mode";
eLang.common_strings["STR_DEVICE_FLASHMODE_DESC"] = "The device is in Flash mode. Please close this browser session and open a new browser session to reconnect to the device. <br/><br/>The device may take few minutes to flash and boot up.";
eLang.common_strings["STR_FLASH_NETWORK_DISCONN_TITLE"] = "Network Connection is disconnected";
eLang.common_strings["STR_FLASH_NETWORK_DISCONN_DESC"] = "Network Connection is disconnected during Flash mode. Please close this browser session and open a new browser session to reconnect to the device. <br/><br/>The device may take few minutes to flash and boot up.";

/*General Threshold Strings*/
eLang.common_strings["STR_SENSOR_THRESHOLD"] = [];
eLang.common_strings["STR_SENSOR_THRESHOLD"][0] = "Lower Non-Critical";
eLang.common_strings["STR_SENSOR_THRESHOLD"][1] = "Lower Critical";
eLang.common_strings["STR_SENSOR_THRESHOLD"][2] = "Lower Non-Recoverable";
eLang.common_strings["STR_SENSOR_THRESHOLD"][3] = "Upper Non-Critical";
eLang.common_strings["STR_SENSOR_THRESHOLD"][4] = "Upper Critical";
eLang.common_strings["STR_SENSOR_THRESHOLD"][5] = "Upper Non-Recoverable";


/*Sensor Type Strings*/
eLang.common_strings["STR_SENSOR_TYPES"] = [];
eLang.common_strings["STR_SENSOR_TYPES"][0x00] = "All Sensors";
eLang.common_strings["STR_SENSOR_TYPES"][0x01] = "Temperature Sensors";
eLang.common_strings["STR_SENSOR_TYPES"][0x02] = "Voltage Sensors";
eLang.common_strings["STR_SENSOR_TYPES"][0x03] = "Current Sensors";
eLang.common_strings["STR_SENSOR_TYPES"][0x04] = "Fan Sensors";
eLang.common_strings["STR_SENSOR_TYPES"][0x05] = "Physical Security";
eLang.common_strings["STR_SENSOR_TYPES"][0x06] = "Platform Security Violation Attempt";
eLang.common_strings["STR_SENSOR_TYPES"][0x07] = "Processor";
eLang.common_strings["STR_SENSOR_TYPES"][0x08] = "Power Supply";
eLang.common_strings["STR_SENSOR_TYPES"][0x09] = "Power Unit";
eLang.common_strings["STR_SENSOR_TYPES"][0x0A] = "Cooling Device";
eLang.common_strings["STR_SENSOR_TYPES"][0x0B] = "Other Units-based Sensor";
eLang.common_strings["STR_SENSOR_TYPES"][0x0C] = "Memory";
eLang.common_strings["STR_SENSOR_TYPES"][0x0D] = "Drive Slot";
eLang.common_strings["STR_SENSOR_TYPES"][0x0E] = "POST Memory Resize";
eLang.common_strings["STR_SENSOR_TYPES"][0x0F] = "System Firmware Progress";
eLang.common_strings["STR_SENSOR_TYPES"][0x10] = "Event Logging Disabled";
eLang.common_strings["STR_SENSOR_TYPES"][0x11] = "Watchdog 1";
eLang.common_strings["STR_SENSOR_TYPES"][0x12] = "System Event";
eLang.common_strings["STR_SENSOR_TYPES"][0x13] = "Critical Interrupt";
eLang.common_strings["STR_SENSOR_TYPES"][0x14] = "Button / Switch";
eLang.common_strings["STR_SENSOR_TYPES"][0x15] = "Module / Board";
eLang.common_strings["STR_SENSOR_TYPES"][0x16] = "Microcontroller / Coprocessor";
eLang.common_strings["STR_SENSOR_TYPES"][0x17] = "Add-in Card";
eLang.common_strings["STR_SENSOR_TYPES"][0x18] = "Chassis";
eLang.common_strings["STR_SENSOR_TYPES"][0x19] = "Chip Set";
eLang.common_strings["STR_SENSOR_TYPES"][0x1A] = "Other FRU";
eLang.common_strings["STR_SENSOR_TYPES"][0x1B] = "Cable / Interconnect";
eLang.common_strings["STR_SENSOR_TYPES"][0x1C] = "Terminator";
eLang.common_strings["STR_SENSOR_TYPES"][0x1D] = "System Boot Initiated";
eLang.common_strings["STR_SENSOR_TYPES"][0x1E] = "Boot Error";
eLang.common_strings["STR_SENSOR_TYPES"][0x1F] = "OS Boot";
eLang.common_strings["STR_SENSOR_TYPES"][0x20] = "OS Critical Stop";
eLang.common_strings["STR_SENSOR_TYPES"][0x21] = "Slot / Connector";
eLang.common_strings["STR_SENSOR_TYPES"][0x22] = "System ACPI Power State";
eLang.common_strings["STR_SENSOR_TYPES"][0x23] = "Watchdog 2";
eLang.common_strings["STR_SENSOR_TYPES"][0x24] = "Platform Alert";
eLang.common_strings["STR_SENSOR_TYPES"][0x25] = "Entity Presence";
eLang.common_strings["STR_SENSOR_TYPES"][0x26] = "Monitor ASIC / IC";
eLang.common_strings["STR_SENSOR_TYPES"][0x27] = "LAN";
eLang.common_strings["STR_SENSOR_TYPES"][0x28] = "Management Subsystem Health";
eLang.common_strings["STR_SENSOR_TYPES"][0x29] = "Battery";
eLang.common_strings["STR_SENSOR_TYPES"][0x2A] = "Session Audit";
eLang.common_strings["STR_SENSOR_TYPES"][0x2B] = "Version Change";
eLang.common_strings["STR_SENSOR_TYPES"][0x2C] = "FRU State";
