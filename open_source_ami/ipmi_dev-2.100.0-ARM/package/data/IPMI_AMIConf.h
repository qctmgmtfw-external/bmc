/*****************************************************************
******************************************************************
***                                                            ***
***        (C)Copyright 2010, American Megatrends Inc.         ***
***                                                            ***
***                    All Rights Reserved                     ***
***                                                            ***
***       5555 Oakbrook Parkway, Norcross, GA 30093, USA       ***
***                                                            ***
***                     Phone 770.246.8600                     ***
***                                                            ***
******************************************************************
******************************************************************
******************************************************************
*
* IPMI_AMIConf.h
* AMI specific configuration commands
*
* Author: Benson Chuang <bensonchuang@ami.com.tw>
*
******************************************************************/

#ifndef __IPMI_AMICONF_H__
#define __IPMI_AMICONF_H__

#include <Types.h>
#include "nwcfg.h"
#include "Iptables.h"
#include "IPMI_LANConfig.h"
#include "IPMI_AppDevice.h"
#include "IPMI_SDRRecord.h"


/* Network Interface EnableState */
#define  DISABLE_V4_V6 0x00
#define  ENABLE_V4     0x01
#define  ENABLE_V6     0x02
#define  ENABLE_V4_V6  0x03

#define  GET_CMD_MODE  0x00 
#define  SET_CMD_MODE  0x01 

#define AMI_IFACE_STATE_ETH             0x00
#define AMI_IFACE_STATE_BOND           0x01
#define AMI_IFACE_BOND_ENABLED      0x02
#define AMI_GET_IFACE_COUNT             0x03
#define AMI_GET_IFACE_CHANNEL           0x04
#define AMI_GET_IFACE_NAME              0x05
#define AMI_BOND_ACTIVE_SLAVE           0x06
#define AMI_BOND_VLAN_ENABLED           0x07
#define MAX_SERVICES 			7
#define SERVICE_NAME_SIZE 16
#define MAX_LAN_CHANNEL             0x05
#define MAX_IFACE_NAME                16
#define FULL_SEL_ENTRIES     0xFF
#define PARTIAL_SEL_ENTRIES   0x00
#define SEL_EMPTY_REPOSITORY 0x00
#define THRESHOLD_RESERVED_BIT  0xC0
#define DISCRETE_RESERVED_BIT   0x80

#define AMI_DNS_CONF_HOST_NAME                      0x01
#define AMI_DNS_CONF_REGISTER                          0x02
#define AMI_DNS_CONF_DOMAIN_SETTINGS           0x03
#define AMI_DNS_CONF_DOMAIN_NAME                  0x04
#define AMI_DNS_CONF_DNS_SETTING                    0x05
#define AMI_DNS_CONF_DNS_IP                               0x06
#define AMI_DNS_CONF_DNS_RESTART                    0x07

#define MAX_DNS_IP_ADDRESS                            0x03
#define MAX_DOMAIN_BLOCK_SIZE                      64
#define MAX_BLOCK                                   0x04

#pragma pack (1)

#define MAX_BOND_IFACE_SLAVES       4
#define DEFAULT_MII_INTERVAL        100

/*
 * Service related struct
 */
#define MAX_SERVICE_IFACE_NAME_SIZE 16

typedef struct
{
    INT32U ServiceID;
    INT8U	 Enable;
    /* The following fields are meaningful only when the service is enabled */
    INT8S   InterfaceName[MAX_SERVICE_IFACE_NAME_SIZE+1]; /* Interface name */
    INT32U  NonSecureAccessPort;               /* Non-secure access port number */
    INT32U  SecureAccessPort;                  /* Secure access port number */                        
    INTU    SessionInactivityTimeout;      	   /* Service session inactivity yimeout in seconds*/
    INT8U   MaxAllowSession;                   /* Maximum allowed simultaneous sessions */
    INT8U   CurrentActiveSession;              /* Number of current active sessions */
    
} PACKED AMIServiceConfig_T;

//GET
typedef struct
{
    INT32U ServiceID;
} PACKED  AMIGetServiceConfReq_T;

typedef struct
{
    INT8U   CompletionCode;
    AMIServiceConfig_T servicecfg;
} PACKED  AMIGetServiceConfRes_T;

//SET
typedef struct
{
    AMIServiceConfig_T servicecfg;
} PACKED  AMISetServiceConfReq_T;

typedef struct 
{
    INT8S   ServiceName [SERVICE_NAME_SIZE+1]; /* Service name */
    AMIServiceConfig_T GetAllSeviceCfg;
}GetAllServiceConf_T;

typedef struct
{
    INT8U ServiceCnt;
    GetAllServiceConf_T ServiceInfo [MAX_SERVICES];
}GetServiceInfo_T;

typedef struct
{
    INT8U   CompletionCode;

} PACKED  AMISetServiceConfRes_T;


/*
 * DNS related struct
 */

typedef struct
{
    INT8U   CompletionCode;
    V6DNS_CONFIG v6dnscfg;
    
}PACKED AMIGetV6DNSConfRes_T;

typedef struct
{
    INT8U   DomainDHCP;
    INT8U   DomainIndex;
    INT8U   Domainpriority;
    INT8U   DomainLen;
}PACKED DomainSetting;

typedef struct
{
    INT8U   DNSDHCP;
    INT8U   DNSIndex;
    INT8U   IPPriority;
}PACKED DNSSetting;

typedef union
{
    HOSTNAMECONF        HName;
    INT8U                       RegDNSConf[MAX_LAN_CHANNEL];
    DomainSetting           DomainConf;
    INT8U                       DomainName[MAX_DOMAIN_BLOCK_SIZE];
    DNSSetting                DNSConf;
    INT8U                       DNSIPAddr[IP6_ADDR_LEN];
} DNSConf_T;

typedef struct
{
    INT8U   ParamSelect;
    INT8U   Blockselector;
    DNSConf_T    DnsConfig;
}PACKED AMISetDNSConfReq_T;

typedef struct
{
    INT8U   CompletionCode;
    
}PACKED AMISetDNSConfRes_T;

typedef struct
{
    INT8U   CompletionCode;
    DNSConf_T   DNSCfg;
}PACKED AMIGetDNSConfRes_T;

typedef struct
{
    INT8U   Param;
    INT8U   Blockselect;
}PACKED AMIGetDNSConfReq_T;

typedef struct
{
    V6DNS_CONFIG    v6dnscfg;
}PACKED AMISetV6DNSConfReq_T;

typedef struct
{
    INT8U   CompletionCode;
}PACKED AMISetV6DNSConfRes_T;

/*
* LinkDown Resilent struct
*/
typedef struct
{
    INT8U LinkDownEnable;
}PACKED AMILinkDownResilentReq_T;

typedef struct
{
    INT8U CompletionCode;
    INT8U LinkEnableStatus;
}PACKED AMILinkDownResilentRes_T;

typedef struct
{
    INT8U   EthIndex;
    INT8U   EnableState;
    /* EnableState - Bit 0 is represented IPv4 and Bit 1 is represented IPv6,
     *               this byte will be ignored if the command is the "get" command.
     *                  DISABLE_V4_V6  (0x00) - Disable IPv4 and IPv6     
     *                  ENABLE_V4         (0x01) - Enable IPv4 only   
     *                  ENABLE_V6         (0x02) - Enable IPv6 only is not allowed 
     *                  ENABLE_V4_V6    (0x03) - Enable IPv4 and IPv6
     */
}PACKED EthIfaceState;

typedef struct
{
    INT8U   Count;
    INT8U   EthIndex[MAX_LAN_CHANNEL];
}PACKED LANIfcCount_T;

typedef struct
{
    INT8U   Channel;
}PACKED GetIfcChannel_T;

typedef struct
{
    char   IfcName[MAX_IFACE_NAME];
}PACKED GetIfcName_T;

typedef struct
{
    INT8U   Enabled;
    INT8U   BondIndex;
}PACKED BondEnabled_T;

typedef struct
{
    INT8U BondIndex;
    INT8U ActiveIndex;
}PACKED ActiveSlave_T;

typedef struct
{
    INT8U Enabled;
}PACKED BondVLAN_T;

typedef union
{
    EthIfaceState   EthIface;
    BondIface        BondIface;
    BondEnabled_T   BondEnable;
    LANIfcCount_T     LANCount;
    GetIfcChannel_T     IfcChannel;
    GetIfcName_T        IfcName;
    ActiveSlave_T       ActiveSlave;
    BondVLAN_T          BondVLAN;
}PACKED IfaceConfigFn;

/*
 * Network Interface Enable/Disable struct
 */
typedef struct
{
    INT8U   Params;
    IfaceConfigFn   ConfigData;
}PACKED AMISetIfaceStateReq_T;

typedef struct
{
    INT8U   Params;
    INT8U   SetSelect;
    INT8U   BlockSelect;
}PACKED AMIGetIfaceStateReq_T;

typedef struct
{
    INT8U   CompletionCode;
}PACKED AMISetIfaceStateRes_T;

typedef struct
{
    INT8U   CompletionCode;
    IfaceConfigFn   ConfigData;
 
}PACKED AMIGetIfaceStateRes_T;


/*---------------- Function Definitions for Firewall Command Implementation ---------------------*/ 

typedef struct
{
	INT8U Param;
	INT8U State;
	FirewallConfUn_T CMD_INFO;
		
} PACKED AMISetFirewallReq_T;

typedef struct
{
	INT8U CompletionCode;
	
} PACKED AMISetFirewallRes_T;


typedef struct
{
	INT8U Param;
	INT8U EntryNo;
	
} PACKED AMIGetFirewallReq_T;


typedef struct
{
	INT8U CompletionCode;
    
}PACKED GetFWCC_T;

typedef union
{
    INT8U TotalCount;
    INT8U IsBlockAll;
    GetFirewallConf_T Info;
    
} GetFirewallConfUn_T;

typedef struct
{
    GetFWCC_T    CCParam;
    GetFirewallConfUn_T FWInfo;

}PACKED AMIGetFirewallRes_T;

/*------------------------------- End of Firewall Command Declarations -------------------------*/

//SNMP:
#ifdef CONFIG_SPX_FEATURE_SNMP_SUPPORT

/*
 * SNMP related struct
 */

typedef struct
{
    INT8U   CompletionCode;
    INT8U   Username[MAX_USERNAME_LEN];
    //INT8U   Password[MAX_PASSWORD_LEN];
    INT8U snmp_enable;                          
    INT8U snmp_access_type;                     
    INT8U snmp_enc_type_1;
    INT8U snmp_enc_type_2;
    
}PACKED AMIGetSNMPConfRes_T;

typedef struct
{
    INT8U   CompletionCode;
    INT8U   Username[MAX_USERNAME_LEN];
    //INT8U   Password[MAX_PASSWORD_LEN];
    INT8U snmp_enable;                          
    INT8U snmp_access_type;                     
    INT8U snmp_enc_type_1;
    INT8U snmp_enc_type_2;
    
}PACKED AMIGetSNMPConfReq_T;

typedef struct
{
    INT8U   Username[MAX_USERNAME_LEN];
    INT8U   Password[MAX_PASSWORD_LEN];
    INT8U snmp_enable;                          
    INT8U snmp_access_type;                     
    INT8U snmp_enc_type_1;
    INT8U snmp_enc_type_2;
}PACKED AMISetSNMPConfReq_T;

typedef struct
{
    INT8U   CompletionCode;
    
}PACKED AMISetSNMPConfRes_T;

#endif //SNMP_SUPPORT


/*
 * AMIGetSELPolicyRes_T
 */
typedef struct
{
    INT8U   CompletionCode;
    INT8U   SELPolicy;

} PACKED AMIGetSELPolicyRes_T;

/*
 * AMISetSELPolicyReq_T
 */
typedef struct
{
    INT8U   SELPolicy;

} PACKED AMISetSELPolicyReq_T;

/*
 * AMISetSELPolicyRes_T
 */
typedef struct
{
    INT8U   CompletionCode;

} PACKED AMISetSELPolicyRes_T;

typedef struct
{
    INT8U ServiceName;
    INT8U SleepSeconds;
}PACKED RestartService_T;

typedef struct
{
    INT32U Noofentretrieved;
}PACKED AMIGetSELEntriesReq_T;

typedef struct
{
    INT8U   CompletionCode;
    INT32U   Noofentries;
    INT16U   LastRecID;
    INT8U   Status;
}PACKED AMIGetSELEntriesRes_T;

typedef struct
{
    INT8U   CompletionCode;
    INT8U   Noofentries;
}PACKED AMIGetSensorInfoRes_T;

typedef struct
{
    SDRRecHdr_T    hdr;
    INT8U               OwnerID;
    INT8U               OwnerLUN;
    INT8U               SensorNumber;
    INT8U               SensorReading;
    INT8U               MaxReading;
    INT8U               MinReading;
    INT8U               Flags;
    INT8U               ComparisonStatus;
    INT8U               OptionalStatus;
    INT8U               SensorTypeCode;
    INT8U               EventTypeCode;
    INT8U               Units1;
    INT8U               Units2;
    INT8U               Units3;
    INT8U               Linearization;
    INT8U               M_LSB;
    INT8U               M_MSB_Tolerance;
    INT8U               B_LSB;
    INT8U               B_MSB_Accuracy;
    INT8U               Accuracy_MSB_Exp;
    INT8U               RExp_BExp;
    INT8U               LowerNonCritical;
    INT8U               LowerCritical;
    INT8U               LowerNonRecoverable;
    INT8U               UpperNonCritical;
    INT8U               UpperCritical;
    INT8U               UpperNonRecoverable;
    INT8U               AssertionEventByte1;
    INT8U               AssertionEventByte2;
    INT8U               DeassertionEventByte1;
    INT8U               DeassertionEventByte2;
    INT8S               SensorName[MAX_ID_STR_LEN];

}PACKED SenInfo_T;
#pragma pack ()


#endif //__IPMI_AMICONF_H__
