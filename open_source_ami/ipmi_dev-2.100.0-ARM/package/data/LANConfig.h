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
 *****************************************************************
 *
 * lanconfig.h
 * Lan configuration command handler
 *
 * Author: Bakka Ravinder Reddy <bakkar@ami.com>
 * 
 *****************************************************************/
#ifndef LANCONFIG_H
#define LANCONFIG_H

#include "Types.h"

#define LAN_PARAM_SET_IN_PROGRESS               0
#define LAN_PARAM_AUTH_TYPE_SUPPORT             1
#define LAN_PARAM_AUTH_TYPE_ENABLES             2
#define LAN_PARAM_IP_ADDRESS                    3
#define LAN_PARAM_IP_ADDRESS_SOURCE             4
#define LAN_PARAM_MAC_ADDRESS                   5
#define LAN_PARAM_SUBNET_MASK                   6
#define LAN_PARAM_IPv4_HEADER                   7
#define LAN_PARAM_PRI_RMCP_PORT                 8
#define LAN_PARAM_SEC_RMCP_PORT                 9
#define LAN_PARAM_BMC_GENERATED_ARP_CONTROL     10
#define LAN_PARAM_GRATITIOUS_ARP_INTERVAL       11
#define LAN_PARAM_DEFAULT_GATEWAY_IP            12
#define LAN_PARAM_DEFAULT_GATEWAY_MAC           13
#define LAN_PARAM_BACKUP_GATEWAY_IP             14
#define LAN_PARAM_BACKUP_GATEWAY_MAC            15
#define LAN_PARAM_COMMUNITY_STRING              16
#define LAN_PARAM_DEST_NUM                      17
#define LAN_PARAM_SELECT_DEST_TYPE              18
#define LAN_PARAM_SELECT_DEST_ADDR              19
#define LAN_PARAM_VLAN_ID                       20
#define LAN_PARAM_VLAN_PRIORITY                 21
#define LAN_PARAM_CIPHER_SUITE_ENTRY_SUP        22
#define LAN_PARAM_CIPHER_SUITE_ENTRIES          23
#define LAN_PARAM_CIPHER_SUITE_PRIV_LEVELS      24
#define LAN_PARAM_VLAN_TAGS                     25
#define LAN_PARAMS_BAD_PASSWORD_THRESHOLD       26

#define LAN_PARAMS_AMI_OEM_VLANIFC_ENABLE 			192
#define LAN_PARAMS_AMI_OEM_SNMPV6_DEST_ADDR 		193
#define LAN_PARAMS_AMI_OEM_ENABLE_SET_MAC			194

#define LAN_PARAMS_AMI_OEM_IPV6_ENABLE			    195
#define LAN_PARAMS_AMI_OEM_IPV6_IP_ADDR_SOURCE		196
#define LAN_PARAMS_AMI_OEM_IPV6_IP_ADDR			    197
#define LAN_PARAMS_AMI_OEM_IPV6_PREFIX_LENGTH		198
#define LAN_PARAMS_AMI_OEM_IPV6_GATEWAY_IP			199

/* Parameter No. 200 to 202 are specified as SSI OEM LAN Parameters */
#define LAN_PARAMS_SSI_OEM_2ND_PRI_ETH_MAC_ADDR 	200
#define LAN_PARAMS_SSI_OEM_LINK_CTRL				201
#define LAN_PARAMS_SSI_OEM_CMM_IP_ADDR				202

#define LAN_PARAMS_AMI_OEM_IPV6_DNS_SETTINGS		203
#ifdef CONFIG_SPX_FEATURE_NCSI_IPMI_COMMAND_SUPPORT
#define LAN_PARAMS_AMI_OEM_NCSI_CONFIG_NUM			204
#define LAN_PARAMS_AMI_OEM_NCSI_SETTINGS			205
#endif
#define LAN_PARAMS_AMI_OEM_PHY_SETTINGS             206

#define UNSPECIFIED_IP_SOURCE 	0x00
#define STATIC_IP_SOURCE 		0x01
#define DHCP_IP_SOURCE   		0x02
#define BIOS_IP_SOURCE   		0x03
#define BMC_OTHER_SOURCE 		0x04

/**
 * @defgroup lcc LAN Configuration Command handlers
 * @ingroup devcfg
 * IPMI LAN interface configuration command handlers.
 * Get/Set commands allow retrieval and updation of various LAN parameters.
 * @{
**/
extern int SetLanConfigParam  (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int GetLanConfigParam  (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int SuspendBMCArps (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int GetIPUDPRMCPStats (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);

/** @} */

/**
 * @brief Initialize LAN Configuration Data.
**/
extern void InitLanConfigData(void);

/**
 * @brief Update ARP Status information.
 * @param EthIndex - Ethernet index
 * @param IsTimerRunning - indicates timer state.
**/
extern INT8U UpdateArpStatus   (INT8U EthIndex, BOOL IsTimerRunning, int BMCInst);

/**
 * @brief Gratuitous ARP generation task. 
**/
extern void* GratuitousARPTask (INT8U *Addr);

#endif /* LANCONFIG_H */
