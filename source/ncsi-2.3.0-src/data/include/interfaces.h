/*
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2009-2015, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 */
/****************************************************************
  Author	: Samvinesh Christopher

  Module	: NCSI Interface and Miscellaniour

  Revision	: 1.0  

  Changelog : 1.0 - Initial Version  [SC]

 *****************************************************************/
#ifndef __INTERFACES_H__
#define __INTERFACES_H__

#include <linux/workqueue.h>
#include <linux/ethtool.h>
#include "types.h"

/* Driver Modules Version Number */
#define NCSI_DRIVER_MAJOR  1
#define NCSI_DRIVER_MINOR  1

#define MAX_CHANNELS	8		/* Max Channels Per Interfaces */
#define BUFFER_SIZE  	1528		/* Should be multiple of 4 bytes */	
#define MAX_NET_IF 	(CONFIG_SPX_FEATURE_GLOBAL_NIC_COUNT)
#define MAX_IF_NAME_LEN 8
#define NCSI_COMMAND_SIZE	64

#define NCSI_POLL_LINK_STATUS yes // Quatna define polling link status 
#ifndef CONFIG_SPX_FEATURE_NCSI_TIMER_DEALAY_FOR_GET_LINK_STATUS
// Default interval time of Get Link Status in seconds 
#define CONFIG_SPX_FEATURE_NCSI_TIMER_DEALAY_FOR_GET_LINK_STATUS 5
#endif

typedef struct
{	
	UINT8 Valid;
	UINT8 PackageID;
	UINT8 ChannelID;
	UINT8 ArbitSupport;
	UINT32 Caps;
	UINT32 BcastCaps;
	UINT32 McastCaps;
	UINT32 AENCaps;
	UINT8  Enabled;
	/* Add Vlan if needed */
} CHANNEL_INFO;


typedef struct ncsi_if_info
{
	UINT8  SendBuffer[BUFFER_SIZE];	/* Align at 32 bit */
	UINT8  RecvBuffer[BUFFER_SIZE];	/* Align at 32 bit */
	UINT8  *SendData;
	UINT8  *RecvData;
	int    SendLen;
	int    RecvLen;
	struct ncsi_if_info *next;
	struct net_device *dev;
	struct work_struct detect_work;
	struct work_struct enable_work;
	struct work_struct disable_work;
	struct work_struct aen_work;
	struct work_struct check_link_work;
	CHANNEL_INFO ChannelInfo[MAX_CHANNELS];
	int TotalChannels;
	int AutoSelect;		/* Software to autoselect Package/Channel */	
	int ForcePackage;	/* User specified (AutoSelect is off */
	int ForceChannel;	/* User specified (AutoSelect is off */
	int vlanID;		
	UINT8 LastCommand;
	UINT8 LastInstanceID;
	UINT8 LastChannelID;
	UINT8 LastManagementID;
	UINT8 Timeout;
	int VetoBit;	
	UINT32 IANA_ManID;
	UINT8 ProcessPending;
	UINT8 AENEnabled;
	int EnabledChannelID;
	int EnabledPackageID;
	
	/* The following are used for ethtool commands */
	UINT8 LinkStatus;
	struct ethtool_cmd ecmd;

    //Quanta+++
	int NcsiStateMachine;
	UINT8 AENEvent;
	UINT32 AENLinkStatus;
	UINT8 AENChannelID;
	UINT8 AENPackageID;
	UINT8 StopIssueNCSICmd;
    //Quanta+++
	
} NCSI_IF_INFO;

typedef struct {
  struct work_struct SetUserSettingsWork;
  char InterfaceName[8];
  int AutoSelect;
  int PackageId;
  int ChannelId;
  int VLANId;
} SetUserSettingsReq_T;

typedef struct {
  struct work_struct SetUserLinkWork;
  char InterfaceName[8];
  int PackageId;
  int ChannelId;
  int Duplex;
  int Speed;
  int AutoNeg;
} SetUserLinkReq_T;

typedef struct {
  struct work_struct SetUserVetoBitWork;
  char InterfaceName[8];  
  int PackageId;
  int ChannelId;  
  int VetoBit;
} SetUserVetoBitReq_T;

#if defined (CONFIG_SPX_FEATURE_NCSI_GET_LINK_STATUS_FOR_NON_AEN_SUPPORTED_CONTROLLERS) ||\
	defined (NCSI_POLL_LINK_STATUS)
typedef struct{
  struct work_struct GetLinkStatusWork;
  char InterfaceName[8];
  int PackageId;
  int ChannelId;
}GetLinkStatusReq_T;
#endif

typedef struct {
  struct work_struct SendUserCommandWork;
  char InterfaceName[8];
  int PackageId;
  int ChannelId;
  unsigned int Length;
  unsigned int Command;
  unsigned int Data[NCSI_COMMAND_SIZE];
} SendUserCommandReq_T;

#ifdef CONFIG_SPX_FEATURE_NCSI_MANUAL_DETECT
typedef struct {
  struct work_struct SendUserCommandWork;
} UserDetectReq_T;
#endif

// Quanta ---
typedef enum
{
    NCSI_NONE = 0,
    NCSI_DETECT_INFO_GOING,
    NCSI_DETECT_INFO_READY,
    NCSI_CHANNEL_DISABLE,
    NCSI_CHANNEL_ENABLE_GOING,
    NCSI_CHANNEL_ENABLE_READY,
    
    NCSI_DETECT_INFO_ERR = 0x80,
    NCSI_CHANNEL_ENABLE_ERR = 0x81,
    NCSI_CHANNEL_GETLINK_ERR = 0x82,
} NCSI_State_Machine_e;
// --- Quanta

NCSI_IF_INFO *AddNetInterface(struct net_device *dev);
NCSI_IF_INFO *GetInterfaceInfo(struct net_device *dev);
NCSI_IF_INFO *GetInterfaceInfoByName(char *interface);
int  RemoveNetInterface(struct net_device *dev);
void RemoveAllNetInterfaces(void);

#ifdef CONFIG_SPX_FEATURE_NCSI_AUTO_FAILOVER
void NCSI_AenWork(struct work_struct *data);        //Quanta
void NCSI_CheckLinkWork(struct work_struct *data);  //Quanta
#endif

void NCSI_Detect(struct work_struct *data);
void NCSI_Enable(struct work_struct *data);
void NCSI_Disable(struct work_struct *data);

void NCSI_Detect_Info(NCSI_IF_INFO *info);
int  NCSI_Enable_Info(NCSI_IF_INFO *info);

int  NCSI_Net_Driver_Register(NCSI_IF_INFO *info);
int  NCSI_Net_Driver_DeRegister(NCSI_IF_INFO *info);


void InitEthtoolInfo(NCSI_IF_INFO *info);
int GetEthtoolInfoFromLink (NCSI_IF_INFO *info, UINT32 LinkStatus);

int InvokeCallbackForEachInterface ( void (*callback)(NCSI_IF_INFO*) );

#endif

