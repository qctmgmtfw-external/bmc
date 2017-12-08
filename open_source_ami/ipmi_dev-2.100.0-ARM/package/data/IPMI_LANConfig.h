/****************************************************************
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2005-2006, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        6145-F, Northbelt Parkway, Norcross,                **
 **                                                            **
 **        Georgia - 30071, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 ****************************************************************
 ****************************************************************
 ****************************************************************
 *
 * ipmi_lanconfig.h
 * Lan configuration command handler
 *
 * Author: Bakka Ravinder Reddy <bakkar@ami.com>
 *
 *****************************************************************/
#ifndef IPMI_LANCONFIG_H
#define IPMI_LANCONFIG_H

#include "Types.h"

/*** External Definitions ***/
#define IP_ADDR_LEN             4
#define IP6_ADDR_LEN             16
#define MAC_ADDR_LEN            6
#define MAX_COMM_STRING_SIZE    18
#define MAX_NUM_CIPHER_SUITE_PRIV_LEVELS    10

#define MAX_IPV6_ADDR_STRLEN  41
#define MAX_NUM_LAN_ALERT_DESTINATIONS		15			//1h to Fh

#define MAX_LAN_CONF_PARAM				0x1A
#define MIN_LAN_OEM_CONF_PARAM			192
#define MAX_LAN_OEM_CONF_PARAM			255

/**
 * @struct AuthTypeEnables_T
 * @brief Authentication Type Enables
**/
#pragma pack(1)
typedef struct
{
    INT8U   AuthTypeCallBack;
    INT8U   AuthTypeUser;
    INT8U   AuthTypeOperator;
    INT8U   AuthTypeAdmin;
    INT8U   AuthTypeOem;

} PACKED  AuthTypeEnables_T;

/**
 * @struct LANDestType_T
 * @brief LAN Destination Type
**/
typedef struct
{
    INT8U   SetSelect;
    INT8U   DestType;
    INT8U   AlertAckTimeout;
    INT8U   Retries;

} PACKED  LANDestType_T;


/**
 * @struct LANDestAddr_T
 * @brief LAN Destination Address
**/
typedef struct
{
    INT8U   SetSelect;
    INT8U   AddrFormat;
    INT8U   GateWayUse;
    INT8U   IPAddr  [IP_ADDR_LEN];
    INT8U   MACAddr [MAC_ADDR_LEN];

} PACKED  LANDestAddr_T;

typedef struct
{
    INT8U   SetSelect;
    INT8U   AddrFormat;
    INT8U   GateWayUse;
    INT8U   IPAddr  [IP6_ADDR_LEN];
    INT8U   MACAddr [MAC_ADDR_LEN];
} PACKED  LANDestv6Addr_T;

/**
 * @struct IPv4HdrParams_T
 * @brief IPv4 Header Parameters
**/
typedef struct
{
    INT8U   TimeToLive;
    INT8U   IpHeaderFlags;
    INT8U   TypeOfService;

} PACKED  IPv4HdrParams_T;

typedef struct
{
    INT8U GenEvent;
    INT8U ThreshNum;
    INT16U ResetInterval;
    INT16U LockoutInterval;
}PACKED BadPassword_T;

typedef struct
{
	INT8U EthIndex;
	INT8U MACAddress[MAC_ADDR_LEN];
}PACKED EnableSetMACAddress_T;

typedef struct
{
    unsigned char v6DNSDhcpEnable;                             /*Specifies that the DNS server IPv6 addresses should be assigned from the DHCP server*/
    unsigned char v6DNSServerEthIndex;                         /*Specifies that from which DHCP server the IPv6 addresses should be assigned */
    unsigned char v6DNSServer1[IP6_ADDR_LEN];                   /*Specifies the IPv6 address for DNS server 1*/
    unsigned char v6DNSServer2[IP6_ADDR_LEN];                   /*Specifies the IPv6 address for DNS server 2*/
   
}  PACKED V6DNS_CONFIG;

#ifdef CONFIG_SPX_FEATURE_NCSI_IPMI_COMMAND_SUPPORT
typedef struct
{
	INT8U Interface;
	INT8U PackageId;
	INT8U ChannelId;
} PACKED NCSIPortConfig_T;
#endif

#ifdef CONFIG_SPX_FEATURE_PHY_SUPPORT
typedef struct
{
    INT8U AutoNegotiationEnable;
    INT16U Speed;
    INT8U Duplex;
    INT8U Interface;
    INT8U CapabilitiesSupported;
} PACKED PHYConfig_T;
#endif

/**
 * @struct LANConfigUn_T
 * @brief LAN Configuration Parameters.
**/
typedef union {

    INT8U               SetInProgress;
    INT8U               AuthTypeSupport;
    AuthTypeEnables_T   AuthTypeEnables;
    INT8U               IPAddr [4];
    INT8U               IPAddrSrc;
    INT8U               MACAddr [6];
    INT8U               SubNetMask [4];
    IPv4HdrParams_T     Ipv4HdrParam;
    INT16U              PrimaryRMCPPort;
    INT16U              SecondaryPort;
    INT8U               BMCGeneratedARPControl;
    INT8U               GratitousARPInterval;
    INT8U               DefaultGatewayIPAddr [IP_ADDR_LEN];
    INT8U               DefaultGatewayMACAddr [MAC_ADDR_LEN];
    INT8U               BackupGatewayIPAddr [IP_ADDR_LEN];
    INT8U               BackupGatewayMACAddr [MAC_ADDR_LEN];
    INT8U               CommunityStr [MAX_COMM_STRING_SIZE];
    INT8U               NumDest;
    LANDestType_T       DestType;
    LANDestAddr_T       DestAddr;
    LANDestv6Addr_T		Destv6Addr;
    INT16U              VLANID;
    INT8U               VLANPriority;
    INT8U               CipherSuiteSup;
    INT8U               CipherSuiteEntries [17];
    INT8U               CipherSuitePrivLevels [MAX_NUM_CIPHER_SUITE_PRIV_LEVELS];
    BadPassword_T		BadPasswd;
    INT8U				EthIndex;
    INT8U				ChangeMACEnabled;
    INT8U               IPv6_Enable;
    INT8U               IPv6_IPAddrSrc;
    INT8U               IPv6_IPAddr [IP6_ADDR_LEN];
    INT8U               IPv6_PrefixLen;
    INT8U               IPv6_GatewayIPAddr[IP6_ADDR_LEN];
    V6DNS_CONFIG V6DNSConfig;
#ifdef CONFIG_SPX_FEATURE_NCSI_IPMI_COMMAND_SUPPORT
	INT8U				NumNCSIPortConfigs;
    NCSIPortConfig_T	NCSIPortConfig;
#endif
#ifdef CONFIG_SPX_FEATURE_PHY_SUPPORT
    PHYConfig_T	PHYConfig;
#endif
#ifdef CONFIG_SPX_FEATURE_SSICOMPUTEBLADE_SUPPORT	/*SSI OEM LAN Parameters */
    INT8U               SSI2ndPriEthMACAddr[MAC_ADDR_LEN];
    INT8U               SSILinkControl;
    INT8U               CMMIPAddr[IP_ADDR_LEN];
#endif
    
} LANConfigUn_T;


/* GetLanCCRev_T */
typedef struct
{
    INT8U   CompletionCode;
    INT8U   ParamRevision;

} PACKED  GetLanCCRev_T;


/* GetLanConfigReq_T */
typedef struct 
{
    INT8U   ChannelNum;
    INT8U   ParameterSelect;
    INT8U   SetSelect;
    INT8U   BlockSelect;

} PACKED  GetLanConfigReq_T;


/* GetLanConfigRes_T */
typedef struct
{
    GetLanCCRev_T   CCParamRev;
    LANConfigUn_T   ConfigData;

} PACKED  GetLanConfigRes_T;

/* GetLanConfigOEM_T */
typedef struct
{
    GetLanCCRev_T   CCParamRev;
    /* OEM parameter should be added below*/
} PACKED GetLanConfigOEMRes_T;

/* SetLanConfigReq_T */
typedef struct
{
    INT8U           ChannelNum;
    INT8U           ParameterSelect;
    /* OEM parameter should be added below*/
} PACKED  SetLanConfigOEMReq_T;

/* SetLanConfigReq_T */
typedef struct 
{
    INT8U           ChannelNum;
    INT8U           ParameterSelect;
    LANConfigUn_T   ConfigData;

} PACKED  SetLanConfigReq_T;


/* SetLanConfigRes_T */
typedef struct 
{
    INT8U   CompletionCode;

} PACKED  SetLanConfigRes_T;


/* SuspendBMCArpsReq_T */
typedef struct 
{
    INT8U   ChannelNo;
    INT8U   ArpSuspend;

} PACKED  SuspendBMCArpsReq_T;


/* SuspendBMCArpsRes_T  */
typedef struct 
{
    INT8U   CompletionCode;
    INT8U   ArpSuspendStatus;

} PACKED  SuspendBMCArpsRes_T;


/* GetIPUDPRMCPStatsReq_T */
typedef struct 
{
    INT8U   ChannelNo;
    INT8U   ClearStatus;

} PACKED  GetIPUDPRMCPStatsReq_T;

/* GetIPUDPRMCPStatsRes_T  */
typedef struct 
{
    INT8U   CompletionCode;
    INT16U   IPPacketsRecv;
    INT16U   IPHeaderErr;
    INT16U   IPAddrErr;
    INT16U   FragIPPacketsRecv;
    INT16U   IPPacketsTrans;
    INT16U   UDPPacketsRecv;
    INT16U   ValidRMCPPackets;

} PACKED  GetIPUDPRMCPStatsRes_T;

#pragma pack()
#endif /* IPMI_LANCONFIG_H */
