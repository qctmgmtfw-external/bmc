/*****************************************************************
 *****************************************************************
 ***                                                            **
 ***    (C)Copyright 2005-2006, American Megatrends Inc.        **
 ***                                                            **
 ***            All Rights Reserved.                            **
 ***                                                            **
 ***        6145-F, Northbelt Parkway, Norcross,                **
 ***                                                            **
 ***        Georgia - 30071, USA. Phone-(770)-246-8600.         **
 ***                                                            **
 *****************************************************************
 *****************************************************************
 ******************************************************************
 *
 * support.h
 * supported commands Macros
 *
 *  Author: Basavaraj Astekar <basavaraja@ami.com>
 *
 ******************************************************************/
#ifndef SUPPORT_H
#define SUPPORT_H
#include "Types.h"


/*---------------------------------------------------------------------------*
 * DEVICES SUPPORTED
 *---------------------------------------------------------------------------*/
#define IPM_DEVICE					1
#define APP_DEVICE					1

#define CHASSIS_DEVICE				1

#define EVENT_PROCESSING_DEVICE		1
#define PEF_DEVICE					1
#define SENSOR_DEVICE				1

#define SDR_DEVICE					1
#define	SEL_DEVICE					1
#define FRU_DEVICE					1

#define BRIDGE_DEVICE				1

#define AMI_DEVICE				    1

#define OEM_DEVICE				    1

/*---------------------------------------------------------------------------*
 * FEATURE SUPPORTED
 *---------------------------------------------------------------------------*/
#define FW_UPGRADE					1
#define FRB_SUPPORT					0
#define TERMINAL_MODE_SUPPORT       1
#define INTERNAL_PSGOOD_MONITORING  0
#define NO_WDT_PRETIMEOUT_INTERRUPT 0


/*---------------------------------------------------------------------------*
 * IPMI 2.0 SPECIFIC DEFINITIONS
 *---------------------------------------------------------------------------*/
#define IPMI20_SUPPORT				1
#define MAX_SOL_IN_PAYLD_SIZE       259
#define MAX_SOL_OUT_PAYLD_SIZE      259
#define MAX_IN_PAYLD_SIZE			1024
#define MAX_OUT_PAYLD_SIZE			1024
#define MAX_PYLDS_SUPPORT			2
#define MAX_PAYLD_INST				15  /* 1 to 15 only */
#define SYS_SERIAL_PORT_NUM			0

/*---------------------------------------------------------------------------*
 * OEM CONFIGURATION DATA SUPPORTED & CONFIGURATION DATA SIZE
 *---------------------------------------------------------------------------*/
#define	OEM_CONFIG_DATA_SUPPORTED	1
#define	MAX_OEM_CONFIG_DATA_SIZE	100


/*-------------------------------------------------------------------------*
 * Define the Unimplemented function based on the choice
 *-------------------------------------------------------------------------*/
#if defined UNIMPLEMENTED_AS_FUNC

#define UNIMPLEMENTED   UnImplementedFunc
extern int UnImplementedFunc (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes, int BMCInst);

#elif defined AMI_UNIMPLEMENTED_AS_FUNC

#define UNIMPLEMENTED   UnImplementedFuncAMI
extern int UnImplementedFuncAMI (_NEAR_ INT8U* pReq, INT32U ReqLen, _NEAR_ INT8U* pRes, int BMCInst);

#else

#define UNIMPLEMENTED 	-1

#endif


#define GET_DCMI_CAPABILITY_INFO                GetDCMICapabilityInfo               /* UNIMPLEMENTED */
#define GET_POWER_READING                       GetPowerReading                     /* UNIMPLEMENTED */
#define GET_POWER_LIMIT                         GetPowerLimit                       /* UNIMPLEMENTED */
#define SET_POWER_LIMIT                         SetPowerLimit                       /* UNIMPLEMENTED */
#define ACTIVATE_POWER_LIMIT                    ActivatePowerLimit                  /* UNIMPLEMENTED */
#define GET_ASSET_TAG                           GetAssetTag                         /* UNIMPLEMENTED */
#define GET_DCMI_SENSOR_INFO                    GetDCMISensorInfo                   /* UNIMPLEMENTED */
#define SET_ASSET_TAG                           SetAssetTag                         /* UNIMPLEMENTED */
#define GET_MANAGEMENT_CONTROLLER_ID_STRING     GetManagementControllerIdString     /* UNIMPLEMENTED */
#define SET_MANAGEMENT_CONTROLLER_ID_STRING     SetManagementControllerIdString     /* UNIMPLEMENTED */

/*---------------------------------------------------------------------------*
 * ENABLE (OR) DISABLE INDIVIDUAL COMMANDS
 *---------------------------------------------------------------------------*/

/*----------------- IPMI Device "Global" Commands ---------------------------*/
#define GET_DEV_ID				GetDevID				/*UNIMPLEMENTED*/
#define BROADCAST_GET_DEV_ID  /*BroadCastGetDevID*/		  UNIMPLEMENTED
#define COLD_RESET				ColdReset				/*UNIMPLEMENTED*/
#define WARM_RESET			  	WarmReset				/*UNIMPLEMENTED*/
#define GET_SELF_TEST_RESULTS	GetSelfTestResults		/*UNIMPLEMENTED*/
#define MFG_TEST_ON				MfgTestOn				/*UNIMPLEMENTED*/
#define SET_ACPI_PWR_STATE		SetACPIPwrState			/*UNIMPLEMENTED*/
#define GET_ACPI_PWR_STATE		GetACPIPwrState			/*UNIMPLEMENTED*/
#define GET_DEV_GUID			GetDevGUID				/*UNIMPLEMENTED*/
#define GET_NETFN_SUP			GetNetFnSup				/*UNIMPLEMENTED*/
#define GET_CMD_SUP				GetCmdSup				/*UNIMPLEMENTED*/
#define GET_SUBFN_SUP			GetSubFnSup				/*UNIMPLEMENTED*/
#define GET_CONFIG_CMDS			GetConfigCmds			/*UNIMPLEMENTED*/
#define GET_CONFIG_SUB_FNS		GetConfigSubFns			/*UNIMPLEMENTED*/
#define SET_CMD_ENABLES			SetCmdEnables			/*UNIMPLEMENTED*/
#define GET_CMD_ENABLES			GetCmdEnables			/*UNIMPLEMENTED*/
#define SET_SUBFN_ENABLES		SetSubFnEnables			/*UNIMPLEMENTED*/
#define GET_SUBFN_ENABLES		GetSubFnEnables			/*UNIMPLEMENTED*/
#define GET_OEM_NETFN_IANA_SUPPORT	GetOEMNetFnIANASupport		/*UNIMPLEMENTED*/

/*------------------ BMC Watchdog Timer Commands ----------------------------*/
#define RESET_WDT				ResetWDT				/*UNIMPLEMENTED*/
#define SET_WDT					SetWDT					/*UNIMPLEMENTED*/
#define GET_WDT					GetWDT					/*UNIMPLEMENTED*/

/*------------------ BMC Device and Messaging Commands ----------------------*/
#define SET_BMC_GBL_ENABLES		SetBMCGlobalEnables		/*UNIMPLEMENTED*/
#define GET_BMC_GBL_ENABLES		GetBMCGlobalEnables		/*UNIMPLEMENTED*/
#define CLR_MSG_FLAGS			ClrMsgFlags				/*UNIMPLEMENTED*/
#define GET_MSG_FLAGS			GetMsgFlags				/*UNIMPLEMENTED*/
#define ENBL_MSG_CH_RCV			EnblMsgChannelRcv		/*UNIMPLEMENTED*/
#define GET_MSG					GetMessage				/*UNIMPLEMENTED*/
#define SEND_MSG				SendMessage				/*UNIMPLEMENTED*/
#define READ_EVT_MSG_BUFFER		ReadEvtMsgBuffer		/*UNIMPLEMENTED*/
#define GET_BTIFC_CAP		    /*GetBTIfcCap*/			UNIMPLEMENTED
#define GET_SYSTEM_GUID			GetSystemGUID			/*UNIMPLEMENTED*/
#define GET_CH_AUTH_CAP			GetChAuthCap			/*UNIMPLEMENTED*/
#define GET_SESSION_CHALLENGE	GetSessionChallenge		/*UNIMPLEMENTED*/
#define ACTIVATE_SESSION		ActivateSession			/*UNIMPLEMENTED*/
#define SET_SESSION_PRIV_LEVEL	SetSessionPrivLevel		/*UNIMPLEMENTED*/
#define CLOSE_SESSION			CloseSession			/*UNIMPLEMENTED*/
#define GET_SESSION_INFO		GetSessionInfo			/*UNIMPLEMENTED*/
#define GET_AUTH_CODE			GetAuthCode				/*UNIMPLEMENTED*/
#define SET_CH_ACCESS			SetChAccess				/*UNIMPLEMENTED*/
#define GET_CH_ACCESS			GetChAccess				/*UNIMPLEMENTED*/
#define GET_CH_INFO				GetChInfo				/*UNIMPLEMENTED*/
#define SET_USER_ACCESS 		SetUserAccess			/*UNIMPLEMENTED*/
#define GET_USER_ACCESS 		GetUserAccess			/*UNIMPLEMENTED*/
#define SET_USER_NAME			SetUserName				/*UNIMPLEMENTED*/
#define GET_USER_NAME			GetUserName				/*UNIMPLEMENTED*/
#define SET_USER_PASSWORD		SetUserPassword			/*UNIMPLEMENTED*/
#define GET_SYSTEM_INFO_PARAM	GetSystemInfoParam /*UNIMPLEMENTED*/
#define SET_SYSTEM_INFO_PARAM	SetSystemInfoParam /*UNIMPLEMENTED*/

#define ACTIVATE_PAYLOAD		ActivatePayload      	/*UNIMPLEMENTED*/
#define DEACTIVATE_PAYLOAD		DeactivatePayload		/*UNIMPLEMENTED*/
#define GET_PAYLD_ACT_STATUS	GetPayldActStatus		/*UNIMPLEMENTED*/
#define GET_PAYLD_INST_INFO		GetPayldInstInfo		/*UNIMPLEMENTED*/
#define SET_USR_PAYLOAD_ACCESS	SetUsrPayloadAccess		/*UNIMPLEMENTED*/
#define GET_USR_PAYLOAD_ACCESS	GetUsrPayloadAccess		/*UNIMPLEMENTED*/
#define GET_CH_PAYLOAD_SUPPORT	GetChPayloadSupport		/*UNIMPLEMENTED*/
#define GET_CH_PAYLOAD_VER		GetChPayloadVersion		/*UNIMPLEMENTED*/
#define GET_CH_OEM_PAYLOAD_INFO	GetChOemPayloadInfo		/*UNIMPLEMENTED*/

#define MASTER_WRITE_READ		MasterWriteRead			/*UNIMPLEMENTED*/

#define GET_CH_CIPHER_SUITES	GetChCipherSuites		/*UNIMPLEMENTED*/
#define SUS_RES_PAYLOAD_ENCRYPT SusResPayldEncrypt		/*UNIMPLEMENTED*/
#define SET_CH_SECURITY_KEYS	SetChSecurityKeys		/*UNIMPLEMENTED*/
#define GET_SYS_IFC_CAPS		GetSysIfcCaps   		/*UNIMPLEMENTED*/

/*--------------------- Chassis Device Commands ----------------------------*/
#define GET_CHASSIS_CAPABILITIES	GetChassisCaps			/*UNIMPLEMENTED*/
#define GET_CHASSIS_STATUS			GetChassisStatus		/*UNIMPLEMENTED*/
#define CHASSIS_CONTROL				ChassisControl			/*UNIMPLEMENTED*/
#define CHASSIS_RESET_CMD			/*ChassisReset*/		  UNIMPLEMENTED
#define CHASSIS_IDENTIFY_CMD   		GetChassisIdentify		/*UNIMPLEMENTED*/
#define SET_CHASSIS_CAPABILITIES	SetChassisCaps			/*UNIMPLEMENTED*/
#define SET_POWER_RESTORE_POLICY	SetPowerRestorePolicy	/*UNIMPLEMENTED*/
#define GET_SYSTEM_RESTART_CAUSE	GetSysRestartCause		/*UNIMPLEMENTED*/
#define SET_SYSTEM_BOOT_OPTIONS		SetSysBOOTOptions		/*UNIMPLEMENTED*/
#define GET_SYSTEM_BOOT_OPTIONS		GetSysBOOTOptions		/*UNIMPLEMENTED*/
#define GET_POH_COUNTER				GetPOHCounter			/*UNIMPLEMENTED*/
#define SET_FP_BTN_ENABLES			SetFPButtonEnables		/*UNIMPLEMENTED*/
#define SET_POWER_CYCLE_INTERVAL	SetPowerCycleInterval	/*UNIMPLEMENTED*/

/*----------------------- Event Commands -----------------------------------*/
#define SET_EVENT_RECEIVER			SetEventReceiver		/*UNIMPLEMENTED*/
#define GET_EVENT_RECEIVER			GetEventReceiver		/*UNIMPLEMENTED*/
#define PLATFORM_EVENT				PlatformEventMessage	/*UNIMPLEMENTED*/

/*--------------------- PEF and Alerting Commands --------------------------*/
#define GET_PEF_CAPABILITIES		GetPEFCapabilities		/*UNIMPLEMENTED*/
#define ARM_PEF_POSTPONE_TIMER		ArmPEFPostponeTimer		/*UNIMPLEMENTED*/
#define SET_PEF_CONFIG_PARAMS		SetPEFConfigParams		/*UNIMPLEMENTED*/
#define GET_PEF_CONFIG_PARAMS		GetPEFConfigParams		/*UNIMPLEMENTED*/
#define SET_LAST_PROCESSED_EVENT_ID	SetLastProcessedEventId	/*UNIMPLEMENTED*/
#define GET_LAST_PROCESSED_EVENT_ID	GetLastProcessedEventId	/*UNIMPLEMENTED*/
#define ALERT_IMMEDIATE				AlertImmediate			/*UNIMPLEMENTED*/
#define PET_ACKNOWLEDGE				PETAcknowledge			/*UNIMPLEMENTED*/

/*----------------------- Sensor Device Commands -------------------------*/
#define GET_DEV_SDR_INFO		   GetDevSDRInfo		  /*UNIMPLEMENTED*/
#define GET_DEV_SDR				   GetDevSDR		  	  /*UNIMPLEMENTED*/
#define RESERVE_DEV_SDR_REPOSITORY ReserveDevSDRRepository /*UNIMPLEMENTED*/
#define GET_SENSOR_READING_FACTORS  GetSensorReadingFactors	/*UNIMPLEMENTED*/
#define SET_SENSOR_HYSTERISIS		SetSensorHysterisis		/*UNIMPLEMENTED*/
#define GET_SENSOR_HYSTERISIS		GetSensorHysterisis		/*UNIMPLEMENTED*/
#define SET_SENSOR_THRESHOLDS		SetSensorThresholds		/*UNIMPLEMENTED*/
#define GET_SENSOR_THRESHOLDS		GetSensorThresholds		/*UNIMPLEMENTED*/
#define SET_SENSOR_EVENT_ENABLE		SetSensorEventEnable	/*UNIMPLEMENTED*/
#define GET_SENSOR_EVENT_ENABLE		GetSensorEventEnable	/*UNIMPLEMENTED*/
#define REARM_SENSOR_EVENTS		    ReArmSensor			    /*UNIMPLEMENTED*/
#define GET_SENSOR_EVENT_STATUS	    GetSensorEventStatus	/*  UNIMPLEMENTED*/
#define GET_SENSOR_READING		    GetSensorReading		/*UNIMPLEMENTED*/
#define SET_SENSOR_READING		    SetSensorReading		/*UNIMPLEMENTED*/
#define SET_SENSOR_TYPE			    SetSensorType			/*UNIMPLEMENTED*/
#define GET_SENSOR_TYPE			    GetSensorType			/*UNIMPLEMENTED*/

/*------------------------ FRU Device Commands ----------------------------*/
#define GET_FRU_INVENTORY_AREA_INFO GetFRUAreaInfo			/*UNIMPLEMENTED*/
#define READ_FRU_DATA				ReadFRUData				/*UNIMPLEMENTED*/
#define WRITE_FRU_DATA				WriteFRUData			/*UNIMPLEMENTED*/

/*----------------------- SDR Device Commands -----------------------------*/
#define GET_SDR_REPOSITORY_INFO		GetSDRRepositoryInfo	/*UNIMPLEMENTED*/
#define GET_SDR_REPOSITORY_ALLOCATION_INFO GetSDRRepositoryAllocInfo	/*UNIMPLEMENTED*/
#define RESERVE_SDR_REPOSITORY		ReserveSDRRepository	/*UNIMPLEMENTED*/
#define GET_SDR				GetSDR			/*UNIMPLEMENTED*/
#define ADD_SDR						AddSDR					/*UNIMPLEMENTED*/
#define PARTIAL_ADD_SDR				PartialAddSDR			/*UNIMPLEMENTED*/
#define DELETE_SDR			DeleteSDR                   	/* UNIMPLEMENTED*/
#define CLEAR_SDR_REPOSITORY		ClearSDRRepository		/*UNIMPLEMENTED*/
#define GET_SDR_REPOSITORY_TIME		 GetSDRRepositoryTime	 /*UNIMPLEMENTED*/
#define SET_SDR_REPOSITORY_TIME		 /*SetSDRRepositoryTime*/  UNIMPLEMENTED
#define ENTER_SDR_REPOSITORY_UPDATE_MODE /*EnterSDRUpdateMode*/	UNIMPLEMENTED
#define EXIT_SDR_REPOSITORY_UPDATE_MODE	 /*ExitSDRUpdateMode*/    UNIMPLEMENTED
#define RUN_INITIALIZATION_AGENT	 RunInitializationAgent /*UNIMPLEMENTED*/

/*------------------------- SEL Device Commands ---------------------------*/
#define GET_SEL_INFO				GetSELInfo				/*UNIMPLEMENTED*/
#define GET_SEL_ALLOCATION_INFO		GetSELAllocationInfo	/*UNIMPLEMENTED*/
#define RESERVE_SEL					ReserveSEL				/*UNIMPLEMENTED*/
#define GET_SEL_ENTRY				GetSELEntry				/*UNIMPLEMENTED*/
#define ADD_SEL_ENTRY				AddSELEntry				/*UNIMPLEMENTED*/
#define PARTIAL_ADD_SEL_ENTRY	    PartialAddSELEntry      /*UNIMPLEMENTED*/
#define DELETE_SEL_ENTRY			DeleteSELEntry		    /*UNIMPLEMENTED*/
#define CLEAR_SEL				    ClearSEL				/*UNIMPLEMENTED*/
#define GET_SEL_TIME				GetSELTime				/*UNIMPLEMENTED*/
#define SET_SEL_TIME				SetSELTime				/*UNIMPLEMENTED*/
#define GET_AUXILIARY_LOG_STATUS    /*GetAuxiliaryLogStatus*/UNIMPLEMENTED
#define SET_AUXILIARY_LOG_STATUS    /*SetAuxiliaryLogStatus*/UNIMPLEMENTED
#define GET_SEL_TIME_UTC_OFFSET		GetSELTimeUTC_Offset	/*UNIMPLEMENTED*/
#define SET_SEL_TIME_UTC_OFFSET		SetSELTimeUTC_Offset	/*UNIMPLEMENTED*/

/*------------------------- LAN Device Commands ---------------------------*/
#define SET_LAN_CONFIGURATION_PARAMETERS SetLanConfigParam	/*UNIMPLEMENTED*/
#define GET_LAN_CONFIGURATION_PARAMETERS GetLanConfigParam	/*UNIMPLEMENTED*/
#define SUSPEND_BMC_ARPS			     SuspendBMCArps      /*UNIMPLEMENTED*/
#define GET_IP_UDP_RMCP_STATISTICS	   /*GetIPUDPRMCPStats*/  UNIMPLEMENTED

/*----------------------- Serial/Modem Device Commands --------------------*/
#define SET_SERIAL_MODEM_CONFIGURATION SetSerialModemConfig  	  		/*UNIMPLEMENTED*/
#define GET_SERIAL_MODEM_CONFIGURATION GetSerialModemConfig				/*UNIMPLEMENTED*/
#define SET_SERIAL_MODEM_MUX		   SetSerialModemMUX			  	/*UNIMPLEMENTED*/
#define GET_TAP_RESPONSE			   GetTAPResponseCodes				/*UNIMPLEMENTED*/
#define SET_PPP_UDP_PROXY_TRANSMIT_DATA/*SetPPPUDPProxyTransmitData*/	  UNIMPLEMENTED
#define GET_PPP_UDP_PROXY_TRANSMIT_DATA/*GetPPPUDPProxyTransmitData*/	  UNIMPLEMENTED
#define SEND_PPP_UDP_PROXY_PACKET	   /*SendPPPUDPProxyPacket*/		  UNIMPLEMENTED
#define GET_PPP_UDP_PROXY_RECEIVE_DATA /*GetPPPUDPProxyReceiveData*/	  UNIMPLEMENTED
#define SERIAL_MODEM_CONNECTION_ACTIVITY /*SerialModemConnectActive*/ 	  UNIMPLEMENTED
#define CALLBACK					   CallBack						  	/*UNIMPLEMENTED*/
#define SET_USER_CALLBACK_OPTIONS	   SetUserCallBackOptions		    /*UNIMPLEMENTED*/
#define GET_USER_CALLBACK_OPTIONS	   GetUserCallBackOptions		  	/*UNIMPLEMENTED*/
#define SOL_ACTIVATING_COMMAND		   SOLActivating		  			/*UNIMPLEMENTED*/
#define GET_SOL_CONFIGURATION		   GetSOLConfig		  				/*UNIMPLEMENTED*/
#define SET_SOL_CONFIGURATION		   SetSOLConfig		  				/*UNIMPLEMENTED*/

/*--------------------- Bridge Management Commands (ICMB) ------------------*/
#define GET_BRIDGE_STATE              GetBridgeState            /*UNIMPLEMENTED*/
#define SET_BRIDGE_STATE              SetBridgeState            /*UNIMPLEMENTED*/
#define GET_ICMB_ADDR                 GetICMBAddr				/*UNIMPLEMENTED*/
#define SET_ICMB_ADDR                 SetICMBAddr				/*UNIMPLEMENTED*/
#define SET_BRIDGE_PROXY_ADDR	      SetBridgeProxyAddr        /*UNIMPLEMENTED*/
#define GET_BRIDGE_STATISTICS         GetBridgeStatistics       /*UNIMPLEMENTED*/
#define GET_ICMB_CAPABILITIES         GetICMBCaps               /*UNIMPLEMENTED*/
#define CLEAR_BRIDGE_STATISTICS       ClearBridgeStatistics     /*UNIMPLEMENTED*/
#define GET_BRIDGE_PROXY_ADDR	      GetBridgeProxyAddr        /*UNIMPLEMENTED*/
#define GET_ICMB_CONNECTOR_INFO       GetICMBConnectorInfo      /*UNIMPLEMENTED*/
#define GET_ICMB_CONNECTION_ID        /*GetICMBConnectionID*/   UNIMPLEMENTED
#define SEND_ICMB_CONNECTION_ID       /*SendICMBConnectionID*/  UNIMPLEMENTED

/*--------------------- Discovery Commands (ICMB) --------------------------*/
#define PREPARE_FOR_DISCOVERY		  PrepareForDiscovery		  /*UNIMPLEMENTED*/
#define GET_ADDRESSES				  GetAddresses				  /*UNIMPLEMENTED*/
#define SET_DISCOVERED				  SetDiscovered				  /*UNIMPLEMENTED*/
#define GET_CHASSIS_DEVICE_ID		  GetChassisDeviceID		  /*UNIMPLEMENTED*/
#define SET_CHASSIS_DEVICE_ID		  SetChassisDeviceID		  /*UNIMPLEMENTED*/

/*--------------------- Bridging Commands (ICMB) ---------------------------*/
#define BRIDGE_REQUEST				  BridgeReq					/*UNIMPLEMENTED*/
#define BRIDGE_MESSAGE				  BridgeMsg					/*UNIMPLEMENTED*/

/*-------------------- Event Commands (ICMB) -------------------------------*/
#define GET_EVENT_COUNT               GetEventCount             /*UNIMPLEMENTED*/
#define SET_EVENT_DESTINATION         SetEventDest				/*UNIMPLEMENTED*/
#define SET_EVENT_RECEPTION_STATE     SetEventReceptionState    /*UNIMPLEMENTED*/
#define SEND_ICMB_EVENT_MESSAGE       SendICMBEventMsg		    /*UNIMPLEMENTED*/
#define GET_EVENT_DESTINATION         GetEventDest              /*UNIMPLEMENTED*/
#define GET_EVENT_RECEPTION_STATE     GetEventReceptionState    /*UNIMPLEMENTED*/

/*---------------- OEM Commands for Bridge NetFn ---------------------------*/
//#define OEM										/* OemCmdHandler */						UNIMPLEMENTED

/*----------------- Other Bridge Commands ----------------------------------*/
#define ERROR_REPORT				  /*ErrorReport*/				UNIMPLEMENTED

/*-------------------- AMI Specific Commands -------------------------------*/
#define SET_SMTP_CONFIG_PARAMS     SetSMTPConfigParams /*UNIMPLEMENTED*/
#define GET_SMTP_CONFIG_PARAMS     GetSMTPConfigParams /*UNIMPLEMENTED*/

#define AMI_GET_CHANNEL_NUM               AMIGetChNum /*UNIMPLEMENTED*/
#define AMI_GET_ETH_INDEX               AMIGetEthIndex /*UNIMPLEMENTED*/


/* AMI RESET PASSWORD AND USER EMAIL COMMANDS */
#define AMI_SET_EMAIL_USER			AMISetEmailForUser /*UNIMPLEMENTED*/
#define AMI_GET_EMAIL_USER			AMIGetEmailForUser /*UNIMPLEMENTED*/
#define AMI_RESET_PASS				AMIResetPassword /*UNIMPLEMENTED*/
#define AMI_SET_EMAILFORMAT_USER			AMISetEmailFormatUser /*UNIMPLEMENTED*/
#define AMI_GET_EMAILFORMAT_USER			AMIGetEmailFormatUser /*UNIMPLEMENTED*/

//Linux Root User Access Commands
#define AMI_GET_ROOT_USER_ACCESS		AMIGetRootUserAccess   	/*UNIMPLEMENTED*/
#define AMI_SET_ROOT_PASSWORD			AMISetRootPassword    	/*UNIMPLEMENTED*/


/* AMI Restore Default commands */
#define AMI_RESTORE_DEF			AMIRestoreDefaults /*UNIMPLEMENTED*/
#define AMI_SET_PRESERVE_CONF			AMISetPreserveConfStatus /*UNIMPLEMENTED*/
#define AMI_GET_PRESERVE_CONF			AMIGetPreserveConfStatus /*UNIMPLEMENTED*/

/* AMI log configuration commands */
#define AMI_GET_LOG_CONF			AMIGetLogConf /*UNIMPLEMENTED*/
#define AMI_SET_LOG_CONF			AMISetLogConf /*UNIMPLEMENTED*/
/* AMI Get Bios code */
#define AMI_GET_BIOS_CODE           AMIGetBiosCode /*UNIMPLEMENTED*/

#define AMI_YAFU_GET_FLASH_INFO        		AMIYAFUGetFlashInfo              /*UNIMPLEMENTED*/
#define AMI_YAFU_GET_FIRMWARE_INFO 		AMIYAFUGetFirmwareInfo       /*UNIMPLEMENTED*/
#define AMI_YAFU_GET_FMH_INFO            		AMIYAFUGetFMHInfo               /*UNIMPLEMENTED*/
#define AMI_YAFU_GET_STATUS                		AMIYAFUGetStatus                  /*UNIMPLEMENTED*/
#define AMI_YAFU_ACTIVATE_FLASH         		AMIYAFUActivateFlashMode   /*UNIMPLEMENTED*/
#define AMI_YAFU_ALLOCATE_MEMORY      		AMIYAFUAllocateMemory       /*UNIMPLEMENTED*/
#define AMI_YAFU_FREE_MEMORY              		AMIYAFUFreeMemory             /*UNIMPLEMENTED*/
#define AMI_YAFU_READ_FLASH                 		AMIYAFUReadFlash                 /*UNIMPLEMENTED*/
#define AMI_YAFU_WRITE_FLASH		      		AMIYAFUWriteFlash	             /*UNIMPLEMENTED*/
#define AMI_YAFU_ERASE_FLASH               		AMIYAFUEraseFlash              /*UNIMPLEMENTED*/
#define AMI_YAFU_PROTECT_FLASH           		AMIYAFUProtectFlash		/*UNIMPLEMENTED*/
#define AMI_YAFU_ERASE_COPY_FLASH     		AMIYAFUEraseCopyFlash      /*UNIMPLEMENTED*/
#define AMI_YAFU_GET_ECF_STATUS			AMIYAFUGetECFStatus
#define AMI_YAFU_GET_VERIFY_STATUS		AMIYAFUGetVerifyStatus
#define AMI_YAFU_VERIFY_FLASH              		AMIYAFUVerifyFlash              /*UNIMPLEMENTED*/
#define AMI_YAFU_READ_MEMORY              		AMIYAFUReadMemory           /*UNIMPLEMENTED*/
#define AMI_YAFU_WRITE_MEMORY            		AMIYAFUWriteMemory          /*UNIMPLEMENTED*/
#define AMI_YAFU_COPY_MEMORY              		AMIYAFUCopyMemory           /*UNIMPLEMENTED*/
#define AMI_YAFU_COMPARE_MEMORY       		AMIYAFUCompareMemory    /*UNIMPLEMENTED*/
#define AMI_YAFU_CLEAR_MEMORY            		AMIYAFUClearMemory          /*UNIMPLEMENTED*/
#define AMI_YAFU_GET_BOOT_CONFIG      		AMIYAFUGetBootConfig        /*UNIMPLEMENTED*/
#define AMI_YAFU_SET_BOOT_CONFIG      		AMIYAFUSetBootConfig        /*UNIMPLEMENTED*/
#define AMI_YAFU_GET_BOOT_VARS          		AMIYAFUGetBootVars          /*UNIMPLEMENTED*/
#define AMI_YAFU_DEACTIVATE_FLASH_MODE   AMIYAFUDeactivateFlash   /*UNIMPLEMENTED*/
#define AMI_YAFU_RESET_DEVICE   			AMIYAFUResetDevice         /*UNIMPLEMENTED*/

/*--------------------------AMI Service Commands ------------------------------------*/
#define AMI_GET_SERVICE_CONF                AMIGetServiceConf             /*UNIMPLEMENTED*/
#define AMI_SET_SERVICE_CONF                AMISetServiceConf             /*UNIMPLEMENTED*/
#define AMI_LINK_DOWN_RESILENT           AMILinkDownResilent         /*UNIMPLEMENTED*/

/*--------------------------AMI DNS Commands ------------------------------------*/
#define AMI_GET_DNS_CONF                AMIGetDNSConf             /*UNIMPLEMENTED*/
#define AMI_SET_DNS_CONF                AMISetDNSConf             /*UNIMPLEMENTED*/

#define AMI_GET_V6DNS_CONF                AMIGetV6DNSConf             /*UNIMPLEMENTED*/
#define AMI_SET_V6DNS_CONF                AMISetV6DNSConf             /*UNIMPLEMENTED*/

/*--------------------------AMI Network Interface State Commands ------------------------------------*/
#define AMI_SET_IFACE_STATE         AMISetIfaceState             /*UNIMPLEMENTED*/
#define AMI_GET_IFACE_STATE         AMIGetIfaceState            /*UNIMPLEMENTED*/

/*------------------------ AMI FIREWALL - Iptables Commands -----------------------------------------*/
#define AMI_SET_FIREWALL			AMISetFirewall				/*UNIMPLEMENTED*/
#define AMI_GET_FIREWALL			AMIGetFirewall				/*UNIMPLEMENTED*/

/*------------------------ AMI FRU Details Commands -----------------------------------------*/
#define AMI_GET_FRU_DETAILS			AMIGetFruDetails			/*UNIMPLEMENTED*/

/*------------------------ AMI PAM Reordering Command---------------------------------------*/
#define AMI_SET_PAM_ORDER			AMISetPamOrder			/*UNIMPLEMENTED*/
#define AMI_GET_PAM_ORDER			AMIGetPamOrder			/*UNIMPLEMENTED*/

/*                                           AMI SNMP Commands----------------------------------------------*/
#define AMI_GET_SNMP_CONF         AMIGetSNMPConf                          /* UNIMPLEMENTED */
#define AMI_SET_SNMP_CONF         AMISetSNMPConf                          /* UNIMPLEMENTED */

/*----------------------------- AMI SEL Commands -----------------------------*/
#define AMI_GET_SEL_POLICY          AMIGetSELPolicy         /* UNIMPLEMENTED */
#define AMI_SET_SEL_POLICY          AMISetSELPolicy         /* UNIMPLEMENTED */
#define AMI_GET_SEL_ENTRIES           AMIGetSELEntires       /* UNIMPLEMENTED */

/*------------------------------AMI SensorInfo Commands -----------------------*/
#define AMI_GET_SENSOR_INFO         AMIGetSenforInfo        /*UNIMPLEMENTED*/
/*--------------------------APML Specific Commands ------------------------------------*/
//SB-RMI
#define APML_GET_INTERFACE_VERSION	ApmlGetInterfaceVersion   /*UNIMPLEMENTED*/ //APML
#define APML_READ_RMI_REG			ApmlReadRMIReg	/*UNIMPLEMENTED*/
#define APML_WRITE_RMI_REG			ApmlWriteRMIReg	/*UNIMPLEMENTED*/
#define APML_READ_CPUID				ApmlReadCPUId	/*UNIMPLEMENTED*/
#define APML_READ_HTC_REG			ApmlReadHTCReg	/*UNIMPLEMENTED*/
#define APML_WRITE_HTC_REG			ApmlWriteHTCReg	/*UNIMPLEMENTED*/
#define APML_READ_PSTATE			ApmlReadPState	/*UNIMPLEMENTED*/
#define APML_READ_MAX_PSTATE		ApmlReadMaxPState	/*UNIMPLEMENTED*/
#define APML_READ_PSTATE_LIMIT		ApmlReadPStateLimit	/*UNIMPLEMENTED*/
#define APML_WRITE_PSTATE_LIMIT		ApmlWritePStateLimit	/*UNIMPLEMENTED*/
#define APML_READ_MCR				ApmlReadMCR	/*UNIMPLEMENTED*/
#define APML_WRITE_MCR				ApmlWriteMCR	/*UNIMPLEMENTED*/
// SB-TSI
#define APML_READ_TSI_REG			ApmlReadTSIReg	/*UNIMPLEMENTED*/
#define APML_WRITE_TSI_REG			ApmlWriteTSIReg	/*UNIMPLEMENTED*/
#define APML_READ_TDP_LIMIT_REG                 ApmlReadTDPLimitReg       /*UNIMPLEMENTED*/ 
#define APML_WRITE_TDP_LIMIT_REG                ApmlWriteTDPLimitReg      /*UNIMPLEMENTED*/ 
#define APML_READ_PROCESSOR_POWER_REG           ApmlReadProcessorPowerReg /*UNIMPLEMENTED*/ 
#define APML_READ_POWER_AVERAGING_REG           ApmlReadPowerAveragingReg /*UNIMPLEMENTED*/ 
#define APML_READ_DRAM_THROTTLE_REG             ApmlReadDramThrottleReg   /*UNIMPLEMENTED*/ 
#define APML_WRITE_DRAM_THROTTLE_REG            ApmlWriteDramThrottleReg  /*UNIMPLEMENTED*/

/*------------------------APML Commands ends here--------------------------------------*/

/*------------------------OPMA Specific Commands ---------------------------------------*/

#define SET_SENSOR_READING_OFFSET               SetSensorReadingOffset      /*UNIMPLEMENTED*/
#define GET_SENSOR_READING_OFFSET               GetSensorReadingOffset      /*UNIMPLEMENTED*/

#define SET_SYSTEM_TYPE_IDENTIFIER              SetSystemTypeIdentifier     /*UNIMPLEMENTED*/
#define GET_SYSTEM_TYPE_IDENTIFIER              GetSystemTypeIdentifier     /*UNIMPLEMENTED*/
#define GET_MCARD_CAPABLITITES                      GetmCardCapabilities            /*UNIMPLEMENTED*/
#define CLEAR_CMOS                                              ClearCMOS                               /*UNIMPLEMENTED*/
#define SET_LOCAL_ACCESS_LOCKOUT_STATE      SetLocalAccessLockOutState      /*UNIMPLEMENTED*/
#define GET_LOCAL_ACCESS_LOCKOUT_STATE      GetLocalAccessLockOutState      /*UNIMPLEMENTED*/
#define GET_SUPPORTED_HOST_IDS                      GetSupportedHostIDs                 /*UNIMPLEMENTED*/

/*------------------------OPMA Commands ends here--------------------------------------*/

/*--------------------------PNM Specific Commands ------------------------------------*/
#define PNM_OEM_GET_READING	        PnmOemGetReading   /*UNIMPLEMENTED*/
#define PNM_OEM_ME_POWER_STATE_CHANGE  PnmOemMePowerStateChange	/*UNIMPLEMENTED*/

/*------------------------PNM Commands ends here--------------------------------------*/


/*------------------------User Shell related commands --------------------------------------*/

#define AMI_SET_USER_SHELLTYPE  AMISetUserShelltype	/*UNIMPLEMENTED*/
#define AMI_GET_USER_SHELLTYPE  AMIGetUserShelltype	/*UNIMPLEMENTED*/

/*------------------------User Shell related commands --------------------------------------*/


/*------------------------Set Trigger Event Configuration-------------------------------------------------*/
#define AMI_SET_TRIGGER_EVT    AMISetTriggerEvent              /*UNIMPLEMENTED*/
/*------------------------Set Trigger Event Configuration-------------------------------------------------*/

/*------------------------Get Trigger Event Configuration-------------------------------------------------*/
#define AMI_GET_TRIGGER_EVT    AMIGetTriggerEvent              /*UNIMPLEMENTED*/
/*------------------------Get Trigger Event Configuration-------------------------------------------------*/

/*------------------------Get SOL Configuration-------------------------------------------------*/
#define AMI_GET_SOL_CONF    AMIGetSolConf              /*UNIMPLEMENTED*/
/*------------------------Get SOL Configuration-------------------------------------------------*/

/*------------------------Set Login Audit Configuration-------------------------------------------------*/
#define AMI_SET_LOGIN_AUDIT_CFG    AMISetLoginAuditConfig              /*UNIMPLEMENTED*/
/*------------------------Set Login Audit Configuration-------------------------------------------------*/

/*------------------------Get Login Audit Configuration-------------------------------------------------*/
#define AMI_GET_LOGIN_AUDIT_CFG    AMIGetLoginAuditConfig              /*UNIMPLEMENTED*/
/*------------------------Get Login Audit Configuration-------------------------------------------------*/

/*-------------------------- SSI Compute Blade Specific Commands ---------------------------------------*/
#ifdef CONFIG_SPX_FEATURE_SSICOMPUTEBLADE_SUPPORT
#define SSICB_GET_COMPUTE_BLADE_PROPERTIES  SSICB_GetComputeBladeProperties /* UNIMPLEMENTED */
#define SSICB_GET_ADDR_INFO                 SSICB_GetAddrInfo               /* UNIMPLEMENTED */
#define SSICB_PLATFORM_EVENT_MESSAGE        SSICB_PlatformEventMessage      /* UNIMPLEMENTED */
#define SSICB_MODULE_BMI_CONTROL            SSICB_ModuleBMIControl          /* UNIMPLEMENTED */
#define SSICB_MODULE_PAYLOAD_CONTROL        SSICB_ModulePayloadControl      /* UNIMPLEMENTED */
#define SSICB_SET_SYSTEM_EVENT_LOG_POLICY   SSICB_SetSystemEventLogPolicy   /* UNIMPLEMENTED */
#define SSICB_SET_MODULE_ACTIVATION_POLICY  SSICB_SetModuleActivationPolicy /* UNIMPLEMENTED */
#define SSICB_GET_MODULE_ACTIVATION_POLICY  SSICB_GetModuleActivationPolicy /* UNIMPLEMENTED */
#define SSICB_SET_MODULE_ACTIVATION         SSICB_SetModuleActivation       /* UNIMPLEMENTED */
#define SSICB_SET_POWER_LEVEL               SSICB_SetPowerLevel             /* UNIMPLEMENTED */
#define SSICB_GET_POWER_LEVEL               SSICB_GetPowerLevel             /* UNIMPLEMENTED */
#define SSICB_RENEGOTIATE_POWER             SSICB_RenegotiatePower          /* UNIMPLEMENTED */
#define SSICB_GET_SERVICE_INFO              SSICB_GetServiceInfo            /* UNIMPLEMENTED */
#define SSICB_GET_APPLET_PACKAGE_URI        SSICB_GetAppletPackageURI       /* UNIMPLEMENTED */
#define SSICB_GET_SERVICE_ENABLE_STATE      SSICB_GetServiceEnableState     /* UNIMPLEMENTED */
#define SSICB_SET_SERVICE_ENABLE_STATE      SSICB_SetServiceEnableState     /* UNIMPLEMENTED */
#define SSICB_SET_SERVICE_TICKET            SSICB_SetServiceTicket          /* UNIMPLEMENTED */
#define SSICB_STOP_SERVICE_SESSION          SSICB_StopServiceSession        /* UNIMPLEMENTED */
#endif
/*-------------------------- SSI Compute Blade Commands ends here --------------------------------------*/

/*---------------------------------------------------------------------------*
 * ENABLE (OR) DISABLE INDIVIDUAL DEVICES BASED ON INTERFACE SELECTED
 *---------------------------------------------------------------------------*/
#if	SUPPORT_ICMB_IFC == 0
#undef	BRIDGE_DEVICE
#define BRIDGE_DEVICE				0
#endif


#if EVENT_PROCESSING_DEVICE == 1
#define EVENT_GENERATOR         1
#define EVENT_RECEIVER          1
#else
#define EVENT_GENERATOR         0
#define EVENT_RECEIVER          0
#endif


#if	DELETE_SDR 	!= UNIMPLEMENTED
#define MARK_FOR_DELETION_SUPPORT  0x00	/* 0x01 to mark records for deletion */
#endif



/***************************  Oem -Specific Commands **********************************/
#if OEM_DEVICE	==   1
#include "OemCmdSupport.h"
#endif // OEM_DEVICE	!=   1

#endif /* SUPPORT_H */
