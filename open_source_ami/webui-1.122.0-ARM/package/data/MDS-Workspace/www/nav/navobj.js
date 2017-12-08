//;*****************************************************************;
//;*****************************************************************;
//;**                                                             **;
//;**     (C) COPYRIGHT American Megatrends Inc. 2008-2011        **;
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

// File Name  : navobj.js
// Brief      : 
// Author Name:
TOPNAV={
	items:[
		{label:"STR_TOPNAV_DASHBOARD", level:0, func:"", page:"/page/dashboard.html", nav:"", enabled:1, userprops:{"feature":'COMMON'}},
		{label:"STR_TOPNAV_FRU_INFORMATION", level:0, func:"", page:"/page/fru_info.html", nav:"", enabled:1, userprops:{"feature":'COMMON'}},
		{label:"STR_TOPNAV_SERVER_HEALTH", level:0, func:"", page:"", nav:"LN_SERVER_HEALTH", enabled:1, userprops:{}},
		{label:"STR_TOPNAV_CONFIGURATION", level:0, func:"", page:"", nav:"LN_CONFIG", enabled:1, userprops:{}},
		{label:"STR_TOPNAV_REMOTE_CONTROL", level:0, func:"", page:"", nav:"LN_REMOTE_CONTROL", enabled:1, userprops:{}},
		{label:"STR_TOPNAV_AUTO_VIDEO_RECORDING", level:0, func:"", page:"", nav:"LN_VIDEO_RECORD", enabled:0, userprops:{"feature":'AUTOVDORECORD'}},
		{label:"STR_TOPNAV_MAINTENANCE", level:0, func:"", page:"", nav:"LN_MAINTENANCE", enabled:1, userprops:{}}
	]
};
LN_SERVER_HEALTH={
	items:[
		{label:"STR_LN_SERVER_HEALTH_SENSOR_READINGS", level:0, func:"", page:"/page/sensor_readings.html", nav:"", enabled:1, userprops:{"feature":'COMMON'}},
		{label:"STR_LN_SERVER_HEALTH_EVENT_LOG", level:0, func:"", page:"/page/event_log.html", nav:"", enabled:1, userprops:{"feature":'COMMON'}},
		{label:"STR_LN_SERVER_HEALTH_SYSTEM_AND_AUDIT_LOG", level:0, func:"", page:"/page/system_audit_log.html", nav:"", enabled:1, userprops:{"feature":'COMMON'}}
	]
};
LN_CONFIG={
	items:[
		{label:"STR_LN_CONFIG_ACTIVE_DIRECTORY", level:0, func:"", page:"/page/configure_active_directory.html", nav:"", enabled:1, userprops:{"feature":'COMMON'}},
		{label:"STR_LN_CONFIG_DNS", level:0, func:"", page:"/page/configure_dns.html", nav:"", enabled:1, userprops:{"feature":'COMMON'}},
		{label:"STR_LN_CONFIG_LDAP_E_DIRECTORY", level:0, func:"", page:"/page/configure_ldap.html", nav:"", enabled:1, userprops:{"feature":'COMMON'}},
		{label:"STR_LN_CONFIG_LOCAL_MEDIA", level:0, func:"", page:"/page/configure_lmedia.html", nav:"", enabled:0, userprops:{"feature":'LMEDIA'}},
		{label:"STR_LN_CONFIG_MOUSE_MODE", level:0, func:"", page:"/page/configure_mouse_mode.html", nav:"", enabled:1, userprops:{"feature":'COMMON'}},
		{label:"STR_LN_CONFIG_NCSI", level:0, func:"", page:"/page/configure_ncsi.html", nav:"", enabled:1, userprops:{"feature":'COMMON'}},
		{label:"STR_LN_CONFIG_NETWORK", level:0, func:"", page:"/page/configure_network.html", nav:"", enabled:1, userprops:{"feature":'COMMON'}},
		{label:"STR_LN_CONFIG_NETWORK_BOND", level:0, func:"", page:"/page/configure_nw_bond.html", nav:"", enabled:0, userprops:{"feature":'NWBONDING'}},
		{label:"STR_LN_CONFIG_NETWORK_LINK", level:0, func:"", page:"/page/configure_phy.html", nav:"", enabled:0, userprops:{"feature":'NWLINK'}},
		{label:"STR_LN_CONFIG_NTP", level:0, func:"", page:"/page/configure_ntp.html", nav:"", enabled:1, userprops:{"feature":'COMMON'}},
		{label:"STR_LN_CONFIG_PAM_ORDER", level:0, func:"", page:"/page/configure_pamorder.html", nav:"", enabled:0, userprops:{"feature":'PAM_REORDERING'}},
		{label:"STR_LN_CONFIG_PEF", level:0, func:"", page:"/page/configure_pef.html", nav:"", enabled:1, userprops:{"feature":'COMMON'}},
		{label:"STR_LN_CONFIG_RADIUS", level:0, func:"", page:"/page/configure_radius.html", nav:"", enabled:1, userprops:{"feature":'COMMON'}},
		{label:"STR_LN_CONFIG_REMOTE_SESSION", level:0, func:"", page:"/page/configure_remote_session.html", nav:"", enabled:1, userprops:{"feature":'COMMON'}},
		{label:"STR_LN_CONFIG_SERVICES", level:0, func:"", page:"/page/configure_services.html", nav:"", enabled:0, userprops:{"feature":'SERVICES'}},
		{label:"STR_LN_CONFIG_SMTP", level:0, func:"", page:"/page/configure_smtp.html", nav:"", enabled:1, userprops:{"feature":'COMMON'}},
		{label:"STR_LN_CONFIG_SSL", level:0, func:"", page:"/page/configure_ssl.html", nav:"", enabled:1, userprops:{"feature":'COMMON'}},
		{label:"STR_LN_CONFIG_SYSTEM_AND_AUDIT_LOG", level:0, func:"", page:"/page/configure_sys_audit_log.html", nav:"", enabled:1, userprops:{"feature":'COMMON'}},
		{label:"STR_LN_CONFIG_USERS", level:0, func:"", page:"/page/configure_users.html", nav:"", enabled:1, userprops:{"feature":'COMMON'}},
		{label:"STR_LN_CONFIG_VIRTUAL_MEDIA", level:0, func:"", page:"/page/configure_vmedia_devices.html", nav:"", enabled:1, userprops:{"feature":'COMMON'}}
	]
};
LN_REMOTE_CONTROL={
	items:[
		{label:"STR_LN_REMOTE_CONTROL_CONSOLE_REDIRECTION", level:0, func:"", page:"/page/console_redirection.html", nav:"", enabled:1, userprops:{"feature":'COMMON'}},
		{label:"STR_LN_REMOTE_CONTROL_SERVER_POWER_CONTROL", level:0, func:"", page:"/page/server_power_control.html", nav:"", enabled:1, userprops:{"feature":'COMMON'}}
	]
};
LN_VIDEO_RECORD={
	items:[
		{label:"STR_LN_VIDEO_RECORD_TRIGGERS_CONFIGURATION", level:0, func:"", page:"/page/configure_triggers.html", nav:"", enabled:0, userprops:{"feature":'AUTOVDORECORD'}},
		{label:"STR_LN_VIDEO_RECORD_RECORDED_VIDEO", level:0, func:"", page:"/page/video_record.html", nav:"", enabled:0, userprops:{"feature":'AUTOVDORECORD'}}
	]
};
LN_MAINTENANCE={
	items:[
		{label:"STR_LN_MAINTENANCE_FIRMWARE_UPDATE", level:0, func:"", page:"/page/fw_update.html", nav:"", enabled:1, userprops:{"feature":'COMMON'}},
		{label:"STR_LN_MAINTENANCE_PRESERVE_CONFIGURATION", level:0, func:"", page:"/page/preserve_cfg.html", nav:"", enabled:0, userprops:{"feature":'PRESERVECONF'}},
		{label:"STR_LN_MAINTENANCE_RESTORE_FACTORY_DEFAULTS", level:0, func:"", page:"/page/restore_factory.html", nav:"", enabled:1, userprops:{"feature":'COMMON'}},
		{label:"STR_LN_MAINTENANCE_SYSTEM_ADMINISTRATOR", level:0, func:"", page:"/page/configure_root.html", nav:"", enabled:1, userprops:{"feature":'COMMON'}}
	]
};
