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

  Module	: NCSI Core API

  Revision	: 1.0  

  Changelog : 1.0 - Initial Version  [SC]

 *****************************************************************/
#include <linux/module.h>
#include <linux/types.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/sched.h>

#include <linux/netdevice.h>
#include <asm/byteorder.h>
#include <linux/delay.h>
#include <linux/string.h>
#include <linux/rtnetlink.h>
#include "interfaces.h"
#include "ncsi.h"


extern int verbose;
extern unsigned int UserLastResponse[NCSI_COMMAND_SIZE];

static UINT32 NCSIValidateCheckSum(NCSI_IF_INFO *info,UINT16* pData,int cs_offset);
static void FormIntelOemNCSIPacket(NCSI_IF_INFO *info,UINT8 Channel,UINT8 *PayLoad, int LastByte_offset);

extern struct workqueue_struct *ncsi_wq;
DECLARE_WAIT_QUEUE_HEAD(NCSI_Process);

void
DisplayLinkStatus(NCSI_IF_INFO* info, UINT32 Link, UINT8 local_verbose)
{
	UINT32 Speed;

	if (!(Link & LINK_STATUS_UP))
	{
		if((local_verbose == 1) || (verbose & (SHOW_LINK_INFO | SHOW_MESSAGES)))	// Ignore printing the link status in case of polling
			printk("NCSI(%s): Link is Down\n", info->dev->name);
		if(info->AutoSelect == 0)
		{
			if (rtnl_trylock())
			{
				netdev_link_down(info->dev);
				rtnl_unlock();
			}
			else
			{
				netdev_link_down(info->dev);
			}
		}
	}
	else
	{
		if((local_verbose == 1) || (verbose & (SHOW_LINK_INFO | SHOW_MESSAGES)))	// Ignore printing the link status in case of polling
			printk("NCSI(%s): Link is Up\n", info->dev->name);
		if(info->AutoSelect == 0)
		{
			if (rtnl_trylock())
			{
				netdev_link_up(info->dev);
				rtnl_unlock();
			}
			else
			{
				netdev_link_up(info->dev);
			}
		}

		if (info->AutoSelect == 1)
		{
			GetEthtoolInfoFromLink(info,Link);
			queue_work(ncsi_wq,&info->enable_work);
		}
		/* Fall Thru */
	}

	if (info->AutoSelect == 0)
	{
		/* Fill the Ethtool information from Link */
		if (GetEthtoolInfoFromLink(info,Link) == 0)
			return; /* Link Down */
	}

	if((local_verbose == 1) || (verbose & (SHOW_LINK_INFO | SHOW_MESSAGES)))	// Ignore printing the link status in case of polling
	{
		Speed = Link & LINK_SPEED_DUPLEX_MASK;
		switch (Speed)
		{
				case  LINK_10THD:			
					printk("NCSI(%s): 10Base-T Half Duplex  \n",info->dev->name);
					break;
				case  LINK_10TFD:						
					printk("NCSI(%s): 10Base-T Full Duplex  \n",info->dev->name);
					break;
				case  LINK_100THD:			
					printk("NCSI(%s): 100Base-TX Half Duplex\n",info->dev->name);
					break;
				case  LINK_100T4:			
					printk("NCSI(%s): 100Base-T4            \n",info->dev->name);
					break;
				case  LINK_100TFD:			
					printk("NCSI(%s): 100Base-TX Full Duplex\n",info->dev->name);
					break;
				case  LINK_1000THD:			
					printk("NCSI(%s): 1000Base-T Half Duplex\n",info->dev->name);
					break;
				case  LINK_1000TFD:		
					printk("NCSI(%s): 1000Base-T Full Duplex\n",info->dev->name);
					break;
				case  LINK_10GT:		
					printk("NCSI(%s): 10G-Base-T            \n",info->dev->name);
					break;
				default:
					printk("NCSI(%s): Unknown Speed and Duplex\n",info->dev->name);
					break;
		}
	}

	if (!(verbose & SHOW_LINK_INFO))
		return;
	
	if (Link & LINK_AUTO_NEG_FLAG)
	{
		printk( "NCSI(%s): Auto Negotiation Enabled\n",info->dev->name);
		if (Link & LINK_AUTO_NEG_COMPLETE)
			printk( "NCSI(%s): Auto Negotiation Complete\n",info->dev->name);
		else
			printk( "NCSI(%s): Auto Negotiation Not Complete\n",info->dev->name);
	}

	if (Link & LINK_PARALLEL_DET_FLAG)
		printk( "NCSI(%s): Link Partner does not support Auto Negotiation \n",info->dev->name);

	if (Link & LINK_TX_FLOW_FLAG)
		printk( "NCSI(%s): Pause Flow Control enabled on TX \n",info->dev->name);

	if (Link & LINK_RX_FLOW_FLAG)
		printk( "NCSI(%s): Pause Flow Control enabled on RX \n",info->dev->name);

	if (Link & LINK_SERDES)
	{
		printk( "NCSI(%s): SerDes is being used \n",info->dev->name);
		return;
	}

	/* Link Partner values valid SerDes=0 AutoNegFlag=1 and AutoNegComplete=1 */
	if ((!(Link & LINK_AUTO_NEG_FLAG)) ||(!(Link & LINK_AUTO_NEG_COMPLETE)))
		return;

	if (Link & LINK_PARTNER_1000TFD)
		printk( "NCSI(%s): Link Partner supports 1000Base-T Full Duplex \n",info->dev->name);
	if (Link & LINK_PARTNER_1000THD)
		printk( "NCSI(%s): Link Partner supports 1000Base-T Half Duplex \n",info->dev->name);
	if (Link & LINK_PARTNER_100T4)
		printk( "NCSI(%s): Link Partner supports 100Base-T4\n",info->dev->name);
	if (Link & LINK_PARTNER_100TXFD)
		printk( "NCSI(%s): Link Partner supports 100Base-TX Full Duplex \n",info->dev->name);
	if (Link & LINK_PARTNER_100TXHD)
		printk( "NCSI(%s): Link Partner supports 100Base-TX Half Duplex \n",info->dev->name);
	if (Link & LINK_PARTNER_10TFD)
		printk( "NCSI(%s): Link Partner supports 10Base-T Full Duplex \n",info->dev->name);
	if (Link & LINK_PARTNER_10THD)
		printk( "NCSI(%s): Link Partner supports 10Base-T Half Duplex \n",info->dev->name);

	return;
}

#ifdef CONFIG_SPX_FEATURE_NCSI_GET_LINK_STATUS_FOR_NON_AEN_SUPPORTED_CONTROLLERS
/*
*@fn CheckAENSupport
*@brief This function is used to check whether the NC-SI controller supports AEN Packets or not
*@param:info- network interface structure, PackageID - NCSI Pkg ID, ChannelID - NCSI Channel ID
*@returns 0 either if it doesn't support AEN or on error, 1 if it supports AEN
*/
int
CheckAENSupport(NCSI_IF_INFO *info, UINT8 PackageID, UINT8 ChannelID)
{
    UINT32 ConfigFlags = 0;
    UINT32 AENControl = 0;
    int RetryCount = NCSI_RETRIES;

    netif_carrier_on(info->dev);
    /* Setup AEN Messages before trying to check if AEN support is available or not*/
    if(NCSI_Issue_EnableAEN(info, (UINT8)PackageID, (UINT8)ChannelID, 1, 1, 1) != 0)
    {
        printk("NCSI(%s):%d.%d Enable AEN Failed\n", info->dev->name, PackageID, ChannelID);
        return 0;
    }

    /* Issue Get Parameters Command */
    while(RetryCount)
    {
        if(NCSI_Issue_GetParameters(info, PackageID, ChannelID, &ConfigFlags, &AENControl) == NCSI_ERR_SUCCESS)
            break;
        else
            RetryCount--;
    }
    if(RetryCount == 0)
    {
        printk("NCSI(%s):%d.%d Get Parameters Command Failed\n", info->dev->name, PackageID, ChannelID);
        return 0;
    }

    if(verbose & SHOW_MESSAGES)
        printk("NCSI(%s):%d.%d AENControl = %ld.\n", info->dev->name, PackageID, ChannelID, AENControl);

    if(AENControl & 0x7) //The first 3 least significant bits of AEN Control field
    {
        if(verbose & SHOW_MESSAGES)
            printk("NCSI(%s):%d.%d Network Controller supports AEN\n", info->dev->name, PackageID, ChannelID);
        return 1;
    }

    if(verbose & SHOW_MESSAGES)
        printk("NCSI(%s):%d.%d Network Controller does not support AEN\n", info->dev->name, PackageID, ChannelID);

    return 0;
}
#endif

int
CheckIfAEN(NCSI_IF_INFO *info, struct sk_buff *skb)
{
	AEN_PKT *aen;
	AEN_LINK_STATUS_CHANGE_PKT *aen_link;
	AEN_HOST_NC_DRIVER_STATUS_CHANGE_PKT *aen_host;
	
	aen = (AEN_PKT *)skb_network_header(skb);

	if (aen->hdr.MC_ID	!= 0x00)
		return 0;
	if (aen->hdr.ONE   	!= 0x01)
		return 0;
	if (aen->hdr.IID   	!= 0x00)
		return 0;
	if (aen->hdr.ALL_FF	!= 0xFF)
		return 0;

	if (verbose & SHOW_AEN_INFO)
		printk("NCSI(%s): AEN Received : ",info->dev->name);

	switch (aen->AEN_Type)
	{
		case AEN_TYPE_LINK_STATUS_CHANGE:
			if (aen->hdr.PayLen !=  AEN_TYPE_LINK_STATUS_CHANGE_LEN)
				break;
			if (NCSI_ERR_SUCCESS != NCSIValidateCheckSum(info,
						(UINT16*)skb_network_header(skb),
						AEN_TYPE_LINK_STATUS_CHANGE_CHKSUM))
				break;
			if (verbose & SHOW_AEN_INFO)
				printk("Link Status Change \n");
			aen_link = (AEN_LINK_STATUS_CHANGE_PKT *)(skb_network_header(skb));
			DisplayLinkStatus(info, be32_to_cpu(aen_link->LinkStatus), 1);
			if (info->AutoSelect == 1)
			{
				//Keven+++
				info->AENEvent = 1;
				info->AENLinkStatus = be32_to_cpu(aen_link->LinkStatus);
				info->AENPackageID = GET_PACKAGE_ID(aen_link->AenHdr.OrgChannelID);
				info->AENChannelID = GET_CHANNEL_ID(aen_link->AenHdr.OrgChannelID);
				SendWakeEvent(EVENT_ID_DO_AEN);
				//queue_work(ncsi_wq,&info->enable_work);
				//Keven+++
			}
			return 1;

		case  AEN_TYPE_CONFIG_REQUIRED:
			if (aen->hdr.PayLen !=  AEN_TYPE_CONFIG_REQUIRED_LEN)
				break;
			if (NCSI_ERR_SUCCESS != NCSIValidateCheckSum(info,
						(UINT16*)skb_network_header(skb),
						AEN_TYPE_CONFIG_REQUIRED_CHKSUM))
				break;
			if (verbose & SHOW_AEN_INFO)
				printk("Config Required \n");
			printk( "NCSI (%s): AEN Config Required\n",info->dev->name);
			return 1;

		case  AEN_TYPE_HOST_DRIVER_CHANGE:
			if (aen->hdr.PayLen !=  AEN_TYPE_HOST_DRIVER_CHANGE_LEN)
				break;
			if (NCSI_ERR_SUCCESS != NCSIValidateCheckSum(info,
						(UINT16*)skb_network_header(skb),
						AEN_TYPE_HOST_DRIVER_CHANGE_CHKSUM))
				break;
			if (verbose & SHOW_AEN_INFO)
				printk("Host driver Status Change\n");
			aen_host = (AEN_HOST_NC_DRIVER_STATUS_CHANGE_PKT *)(skb_network_header(skb));
			if ((be32_to_cpu(aen_host->HostNC_DriverStatus)) & 0x1)
				printk("NCSI (%s): Host Network is Up\n",info->dev->name);
			else
				printk("NCSI (%s): Host Network is Down\n",info->dev->name);
			return 1;
	}
	printk("Unknown AEN %d\n",aen->AEN_Type);
	return 0;
}

static
UINT32
NCSICalculateCheckSum (UINT16* pData, int Size)
{
	UINT32 CheckSum = 0;
	while (Size > 0)
	{
		CheckSum += be16_to_cpu(*pData);
		pData++;
		Size -= 2;
	}

	CheckSum = ~CheckSum + 1;
	return cpu_to_be32(CheckSum);
}

static
UINT32
NCSIValidateCheckSum(NCSI_IF_INFO *info,UINT16* pData,int cs_offset)
{
	UINT32 CheckSum = 0, GotCS;
	UINT32 i;

	GotCS = *(UINT32*)(((UINT8*)pData) + cs_offset);
	GotCS = be32_to_cpu(GotCS);
	for(i=0;i<cs_offset;i+=2)
	{
		CheckSum += be16_to_cpu(*pData);
		pData++;
	}
	CheckSum = ~CheckSum + 1;

	if ((GotCS == CheckSum) || (GotCS == 0))
		return NCSI_ERR_SUCCESS;

	printk("NCSI(%s): CheckSum Error: Received = 0x%08lx, Calculated = 0x%08lx\n",
						info->dev->name,GotCS,CheckSum);
	return NCSI_ERR_CHECKSUM;
}

static char *CmdStr[] =
{
	"Clear Initial State",
	"Select Package",
	"Deselect Package",
	"Enable Channel",
	"Disable Channel",
	"Reset Channel",
	"Enable Channel Network TX",
	"Disable Channel Network TX",
	"AEN Enable",
	"Set Link",
	"Get Link Status",
	"Set VLAN Filter",
	"Enable VLAN",
	"Disable VLAN",
	"Set MAC Address",
	"0x0F",
	"Enable Broadcast Filering",
	"Disable Broadcast Filering",
	"Enable Global Multicast Filering",
	"Disable Gloable Multicast Filering",
	"Set NC-SI Flow Control",
	"Get Version ID",
	"Get Capabilities",
	"Get Parameters",
	"Get Controller Packet Statistics",
	"Get NC-SI Statistics",
	"Get NC-SI Pass-through Statistics",
};


static char *CommandNum = "0xXX";

static
char *
Cmd2Str(NCSI_IF_INFO *info)
{
	if ((info->LastCommand > 0x1A) || (info->LastCommand == 0x0F))
	{	
		sprintf(CommandNum,"0x%02x",info->LastCommand);
		return CommandNum;
	}

	return CmdStr[info->LastCommand];
}

static
int
ProcessSetLinkReasonCode(NCSI_IF_INFO *info,UINT16 ReasonCode)
{
	switch (ReasonCode)
	{
		case  REASON_NONE:
			printk("No Reason\n");
			break;
		case  SET_LINK_REASON_HOST_CONFLICT:
			printk("Host OS/Driver Conflict\n");
			break;
		case  SET_LINK_REASON_MEDIA_CONFLICT:
			printk("Media Conflict\n");
			break;
		case  SET_LINK_REASON_PARAMETER_CONFLICT:
			printk("Parameter Conflict\n");
			break;
		case  SET_LINK_REASON_POWERMODE_CONFLICT:
			printk("Power Mode Conflict\n");
			break;
		case  SET_LINK_REASON_SPEED_CONFLICT:
			printk("Speed  Conflict\n");
			break;
		case  SET_LINK_REASON_HW_ACCESS_FAILED:
			printk("Hardware Failure\n");
			return NCSI_ERR_FAILED;
			break;
		default:
			printk("Unknown Reason\n");
			return NCSI_ERR_FAILED;
			break;			
	}

	return NCSI_ERR_CONFLICT;
}

static
int
ProcessResponseCode(NCSI_IF_INFO *info,UINT16 ResponseCode, UINT16 ReasonCode)
{
	switch (be16_to_cpu(ResponseCode))
	{
		int i = 0;
		
		case  COMMAND_COMPLETED:
			if(verbose & SHOW_MESSAGES)
			{
				printk("NCSI(%s): %s : Completed.", info->dev->name, Cmd2Str(info));
				for(i = 0; i < info->RecvLen; i = i+4)
					printk("%02d:   %02X %02X %02X %02X\n", i+0xc, info->RecvBuffer[i], info->RecvBuffer[i+1], info->RecvBuffer[i+2], info->RecvBuffer[i+3]);
			}
			return NCSI_ERR_SUCCESS;
		case  COMMAND_FAILED:
			printk("NCSI(%s): %s : Failed.",info->dev->name,Cmd2Str(info));
			break;
		case  COMMAND_UNAVAILABLE:			
			printk("NCSI(%s): %s : Unavailable.",info->dev->name,Cmd2Str(info));	
			break;
		case  COMMAND_UNSUPPORTED:
			printk("NCSI(%s): %s : Unsupported.",info->dev->name,Cmd2Str(info));
			break;
		default:
			printk("NCSI(%s): %s : Unknown ResponseCode.",info->dev->name,Cmd2Str(info));
			break;
	}

	ReasonCode = be16_to_cpu(ReasonCode);

	if ((ReasonCode & 0xFF00) == 0)
	{
		/* Generic Reason Code */
		switch (ReasonCode&0xFF)
		{
			case  REASON_NONE:
				printk("No Reason\n");
				break;
			case  REASON_INIT_REQUIRED:		
				printk("Init Required\n");
				return NCSI_INIT_REQUIRED;
			case  REASON_PARAMS_INVALID:				
				printk("Parameters Invalid\n");
				break;
			case  REASON_CHANNEL_NOT_READY:			
				printk("Channel Not Ready\n");
				break;
			case  REASON_PACKAGE_NOT_READY:			
				printk("Package Not Ready\n");
				break;
			case  REASON_LENGTH_INVALID:				
				printk("Invalid Length\n");
				break;
			case  REASON_MAC_ADDR_ZERO:
				printk("MAC Address is zero\n");
				break;
			default:
				printk("Unknown Reason\n");
				break;			
		}
		return NCSI_ERR_FAILED;
	}
	/* Command Specific Reason Code */
	if (((ReasonCode & 0xFF00) >> 8) ==  CMD_SET_LINK)
		return ProcessSetLinkReasonCode(info,ReasonCode&0xFF);


	return NCSI_ERR_FAILED;
}


static
void
FormNCSIPacket(NCSI_IF_INFO *info,UINT8 Command,UINT8 Channel,UINT8 *PayLoad, int cs_offset)
{
	ETH_HDR  		*EthHdr;
	NCSI_HDR		*NcsiHdr;
	UINT8   		*NcsiPkt,*NcsiPayLoad;
	UINT32			*CheckSum;
	UINT32			PayLoadLen;

	memset(info->SendData,0,BUFFER_SIZE);
	/* Get the locations of various fields */
	EthHdr  	= (ETH_HDR *)(info->SendData);
	NcsiPkt		=  info->SendData + sizeof(ETH_HDR);
	NcsiHdr 	= (NCSI_HDR *)NcsiPkt;
	NcsiPayLoad     =  info->SendData + sizeof(ETH_HDR) + sizeof(NCSI_HDR);
	CheckSum	= (UINT32 *)(NcsiPkt+cs_offset);
	PayLoadLen	=  (UINT32)CheckSum - (UINT32)NcsiPayLoad;

	/* Fill in Ethernet Header */
	memset(EthHdr->DestMACAddr,0xFF,6);
	memset(EthHdr->SrcMACAddr,0xFF,6);
	EthHdr->EtherType = cpu_to_be16(NCSI_ETHER_TYPE);
	
	/* Fill in NCSI Header */
	memset(NcsiHdr,0, sizeof(NCSI_HDR));
	NcsiHdr->MC_ID 		= NCSI_MC_ID;
	NcsiHdr->HdrRev 	= NCSI_HDR_REV;
	info->LastInstanceID++;
	/* Instance ID  = 1 to 0xFF . 0 is invalid */
	info->LastInstanceID= (info->LastInstanceID == 0)?1:info->LastInstanceID;	
	NcsiHdr->I_ID		= info->LastInstanceID;
	NcsiHdr->Command 	= Command;
	NcsiHdr->CH_ID		= Channel;
	NcsiHdr->PayloadLen = cpu_to_be16(PayLoadLen);

	/* Save some info */
	info->LastCommand      = Command;
	info->LastChannelID    = Channel;
	info->LastManagementID = NCSI_MC_ID;

	/* Copy the PayLoad */
	memcpy(NcsiPayLoad,PayLoad,PayLoadLen);

	/* Calculate the checksum for NCSI packet */
	*CheckSum = NCSICalculateCheckSum((UINT16 *)(NcsiPkt),PayLoadLen+sizeof(NCSI_HDR));
	return;
}

/*****************************************************************************************
								NC-SI Commands
*****************************************************************************************/

int
NCSI_Issue_SelectPackage(NCSI_IF_INFO *info,UINT8 PackageID,UINT8 HwArbitDisable)
{
	SELECT_PACKAGE_REQ_PKT Pkt;
	SELECT_PACKAGE_RES_PKT *ResPkt;

	UINT8 Channel;
	UINT8 *PayLoad;
	int cs_offset;
	int retval;

	/* Sleep for response or timeout */
	wait_event_interruptible_timeout(NCSI_Process, (info->ProcessPending == 0), NCSI_TIMEOUT);
	info->ProcessPending = 1;
	/* Basic Initialization */
	memset(&Pkt ,0, sizeof(Pkt));
	PayLoad = ((UINT8 *)&Pkt) + sizeof(NCSI_HDR);
	cs_offset = (UINT32)(&(Pkt.CheckSum)) - (UINT32)(&Pkt);

	/* "Select Package" Specific Parameters */
	Channel = MK_CH_ID(PackageID,0x1F);
	Pkt.HwArbitDisable = HwArbitDisable;

	/* Form NCSI Packet and send */
	FormNCSIPacket(info,CMD_SELECT_PACKAGE,Channel,PayLoad,cs_offset);
	info->SendLen = sizeof(Pkt);
	retval = SendNCSICommand (info);
	if (retval != NCSI_ERR_SUCCESS)
		goto exit;

	/* Validate Response pkt */
	ResPkt = (SELECT_PACKAGE_RES_PKT *)info->RecvData;
	if (info->RecvLen != sizeof(SELECT_PACKAGE_RES_PKT))
	{
		printk("NCSI(%s): Expected Response Size = %zu Got %d\n",info->dev->name,
			sizeof(SELECT_PACKAGE_RES_PKT),info->RecvLen);
		retval = NCSI_ERR_RESPONSE;
		goto exit;
	}

	/* Validate Response checksum */
	cs_offset = (UINT32)(&(ResPkt->CheckSum)) - (UINT32)(ResPkt);
	retval = NCSIValidateCheckSum(info,(UINT16*)info->RecvData,cs_offset);
	if (retval != NCSI_ERR_SUCCESS)
		goto exit;

	/* Check the Response code and reason */
	retval = ProcessResponseCode(info,ResPkt->ResponseCode,ResPkt->ReasonCode);
	
	exit:
	info->ProcessPending = 0;
	wake_up_interruptible(&NCSI_Process);
	return retval;
}


int
NCSI_Issue_DeSelectPackage(NCSI_IF_INFO *info,UINT8 PackageID)
{
	DESELECT_PACKAGE_REQ_PKT Pkt;
	DESELECT_PACKAGE_RES_PKT *ResPkt;

	UINT8 Channel;
	UINT8 *PayLoad;
	int cs_offset;
	int retval;

	/* Sleep for response or timeout */
	wait_event_interruptible_timeout(NCSI_Process, (info->ProcessPending == 0), NCSI_TIMEOUT);
	info->ProcessPending = 1;
	/* Basic Initialization */
	memset(&Pkt ,0, sizeof(Pkt));
	PayLoad = ((UINT8 *)&Pkt) + sizeof(NCSI_HDR);
	cs_offset = (UINT32)(&(Pkt.CheckSum)) - (UINT32)(&Pkt);

	/* "DeSelect Package" Specific Parameters */
	Channel = MK_CH_ID(PackageID,0x1F);

	/* Form NCSI Packet and send */
	FormNCSIPacket(info,CMD_DESELECT_PACKAGE,Channel,PayLoad,cs_offset);
	info->SendLen = sizeof(Pkt);
	retval = SendNCSICommand (info);
	if (retval != NCSI_ERR_SUCCESS)
		goto exit;

	/* Validate Response pkt */
	ResPkt = (DESELECT_PACKAGE_RES_PKT *)info->RecvData;
	if (info->RecvLen != sizeof(DESELECT_PACKAGE_RES_PKT))
	{
		printk("NCSI(%s): Expected Response Size = %zu Got %d\n",info->dev->name,
			sizeof(DESELECT_PACKAGE_RES_PKT),info->RecvLen);
		retval = NCSI_ERR_RESPONSE;
		goto exit;
	}

	/* Validate Response checksum */
	cs_offset = (UINT32)(&(ResPkt->CheckSum)) - (UINT32)(ResPkt);
	retval = NCSIValidateCheckSum(info,(UINT16*)info->RecvData,cs_offset);
	if (retval != NCSI_ERR_SUCCESS)
		goto exit;

	/* Check the Response code and reason */
	retval = ProcessResponseCode(info,ResPkt->ResponseCode,ResPkt->ReasonCode);
	
	exit:
	info->ProcessPending = 0;
	wake_up_interruptible(&NCSI_Process);
	return retval;
}


int
NCSI_Issue_ClearInitialState(NCSI_IF_INFO *info,UINT8 PackageID,UINT8 ChannelID)
{
	CLEAR_INITIAL_STATE_REQ_PKT Pkt;
	CLEAR_INITIAL_STATE_RES_PKT *ResPkt;

	UINT8 Channel;
	UINT8 *PayLoad;
	int cs_offset;
	int retval;

	/* Sleep for response or timeout */
	wait_event_interruptible_timeout(NCSI_Process, (info->ProcessPending == 0), NCSI_TIMEOUT);
	info->ProcessPending = 1;
	/* Basic Initialization */
	memset(&Pkt ,0, sizeof(Pkt));
	PayLoad = ((UINT8 *)&Pkt) + sizeof(NCSI_HDR);
	cs_offset = (UINT32)(&(Pkt.CheckSum)) - (UINT32)(&Pkt);

	Channel = MK_CH_ID(PackageID,ChannelID);

	/* Form NCSI Packet and send */
	FormNCSIPacket(info,CMD_CLEAR_INITIAL_STATE,Channel,PayLoad,cs_offset);
	info->SendLen = sizeof(Pkt);
	retval = SendNCSICommand (info);
	if (retval != NCSI_ERR_SUCCESS)
		goto exit;

	/* Validate Response pkt */
	ResPkt = (CLEAR_INITIAL_STATE_RES_PKT *)info->RecvData;
	if (info->RecvLen != sizeof(CLEAR_INITIAL_STATE_RES_PKT))
	{
		printk("NCSI(%s): Expected Response Size = %zu Got %d\n",info->dev->name,
			sizeof(CLEAR_INITIAL_STATE_RES_PKT),info->RecvLen);
		retval = NCSI_ERR_RESPONSE;
		goto exit;
	}

	/* Validate Response checksum */
	cs_offset = (UINT32)(&(ResPkt->CheckSum)) - (UINT32)(ResPkt);
	retval = NCSIValidateCheckSum(info,(UINT16*)info->RecvData,cs_offset);
	if (retval != NCSI_ERR_SUCCESS)
		goto exit;

	/* Check the Response code and reason */
	retval = ProcessResponseCode(info,ResPkt->ResponseCode,ResPkt->ReasonCode);
	
	exit:
	info->ProcessPending = 0;
	wake_up_interruptible(&NCSI_Process);
	return retval;
}


int
NCSI_Issue_ChannelCommands(NCSI_IF_INFO *info,UINT8 Command, UINT8 PackageID,UINT8 ChannelID)
{
	NCSI_DEFAULT_REQ_PKT Pkt;
	NCSI_DEFAULT_RES_PKT *ResPkt;

	UINT8 Channel;
	UINT8 *PayLoad;
	int cs_offset;
	int retval;

	/* Sleep for response or timeout */
	wait_event_interruptible_timeout(NCSI_Process, (info->ProcessPending == 0), NCSI_TIMEOUT);
	info->ProcessPending = 1;
	/* Basic Initialization */
	memset(&Pkt ,0, sizeof(Pkt));
	PayLoad = ((UINT8 *)&Pkt) + sizeof(NCSI_HDR);
	cs_offset = (UINT32)(&(Pkt.CheckSum)) - (UINT32)(&Pkt);

	Channel = MK_CH_ID(PackageID,ChannelID);

	/* Form NCSI Packet and send */
	FormNCSIPacket(info,Command,Channel,PayLoad,cs_offset);
	info->SendLen = sizeof(Pkt);
	retval = SendNCSICommand (info);
	if (retval != NCSI_ERR_SUCCESS)
		goto exit;

	/* Validate Response pkt */
	ResPkt = (NCSI_DEFAULT_RES_PKT *)info->RecvData;
	if (info->RecvLen != sizeof(NCSI_DEFAULT_RES_PKT))
	{
		printk("NCSI(%s): Expected Response Size = %zu Got %d\n",info->dev->name,
			sizeof(NCSI_DEFAULT_RES_PKT),info->RecvLen);
		retval = NCSI_ERR_RESPONSE;
		goto exit;
	}

	/* Validate Response checksum */
	cs_offset = (UINT32)(&(ResPkt->CheckSum)) - (UINT32)(ResPkt);
	retval = NCSIValidateCheckSum(info,(UINT16*)info->RecvData,cs_offset);
	if (retval != NCSI_ERR_SUCCESS)
		goto exit;

	/* Check the Response code and reason */
	retval = ProcessResponseCode(info,ResPkt->ResponseCode,ResPkt->ReasonCode);
	
	exit:
	info->ProcessPending = 0;
	wake_up_interruptible(&NCSI_Process);
	return retval;
}


int
NCSI_Issue_DisableChannel(NCSI_IF_INFO *info,UINT8 PackageID,UINT8 ChannelID,UINT8 AllowLinkDown)
{
	DISABLE_CHANNEL_REQ_PKT Pkt;
	DISABLE_CHANNEL_RES_PKT *ResPkt;

	UINT8 Channel;
	UINT8 *PayLoad;
	int cs_offset;
	int retval;

	/* Sleep for response or timeout */
	wait_event_interruptible_timeout(NCSI_Process, (info->ProcessPending == 0), NCSI_TIMEOUT);
	info->ProcessPending = 1;
	/* Basic Initialization */
	memset(&Pkt ,0, sizeof(Pkt));
	PayLoad = ((UINT8 *)&Pkt) + sizeof(NCSI_HDR);
	cs_offset = (UINT32)(&(Pkt.CheckSum)) - (UINT32)(&Pkt);

	/* i"Disable Channel" Specific Parameters */
	Channel = MK_CH_ID(PackageID,ChannelID);
	Pkt.AllowLinkDown = AllowLinkDown;

	/* Form NCSI Packet and send */
	FormNCSIPacket(info,CMD_DISABLE_CHANNEL,Channel,PayLoad,cs_offset);
	info->SendLen = sizeof(Pkt);
	retval = SendNCSICommand (info);
	if (retval != NCSI_ERR_SUCCESS)
		goto exit;

	/* Validate Response pkt */
	ResPkt = (DISABLE_CHANNEL_RES_PKT *)info->RecvData;
	if (info->RecvLen != sizeof(DISABLE_CHANNEL_RES_PKT))
	{
		printk("NCSI(%s): Expected Response Size = %zu Got %d\n",info->dev->name,
			sizeof(DISABLE_CHANNEL_RES_PKT),info->RecvLen);
		retval = NCSI_ERR_RESPONSE;
		goto exit;
	}

	/* Validate Response checksum */
	cs_offset = (UINT32)(&(ResPkt->CheckSum)) - (UINT32)(ResPkt);
	retval = NCSIValidateCheckSum(info,(UINT16*)info->RecvData,cs_offset);
	if (retval != NCSI_ERR_SUCCESS)
		goto exit;

	/* Check the Response code and reason */
	retval = ProcessResponseCode(info,ResPkt->ResponseCode,ResPkt->ReasonCode);
	
	exit:
	info->ProcessPending = 0;
	wake_up_interruptible(&NCSI_Process);
	return retval;
}

int
NCSI_Issue_ResetChannel(NCSI_IF_INFO *info,UINT8 PackageID,UINT8 ChannelID)
{
	RESET_CHANNEL_REQ_PKT Pkt;
	RESET_CHANNEL_RES_PKT *ResPkt;

	UINT8 Channel;
	UINT8 *PayLoad;
	int cs_offset;
	int retval;

	/* Sleep for response or timeout */
	wait_event_interruptible_timeout(NCSI_Process, (info->ProcessPending == 0), NCSI_TIMEOUT);
	info->ProcessPending = 1;
	/* Basic Initialization */
	memset(&Pkt ,0, sizeof(Pkt));
	PayLoad = ((UINT8 *)&Pkt) + sizeof(NCSI_HDR);
	cs_offset = (UINT32)(&(Pkt.CheckSum)) - (UINT32)(&Pkt);

	/* "Reset Channel" Specific Parameters */
	Channel = MK_CH_ID(PackageID,ChannelID);
	Pkt.Reserved = 0;

	/* Form NCSI Packet and send */
	FormNCSIPacket(info,CMD_RESET_CHANNEL,Channel,PayLoad,cs_offset);
	info->SendLen = sizeof(Pkt);
	retval = SendNCSICommand (info);
	if (retval != NCSI_ERR_SUCCESS)
		goto exit;

	/* Validate Response pkt */
	ResPkt = (RESET_CHANNEL_RES_PKT *)info->RecvData;
	if (info->RecvLen != sizeof(RESET_CHANNEL_RES_PKT))
	{
		printk("NCSI(%s): Expected Response Size = %zu Got %d\n",info->dev->name,
			sizeof(RESET_CHANNEL_RES_PKT),info->RecvLen);
		retval = NCSI_ERR_RESPONSE;
		goto exit;
	}

	/* Validate Response checksum */
	cs_offset = (UINT32)(&(ResPkt->CheckSum)) - (UINT32)(ResPkt);
	retval = NCSIValidateCheckSum(info,(UINT16*)info->RecvData,cs_offset);
	if (retval != NCSI_ERR_SUCCESS)
		goto exit;

	/* Check the Response code and reason */
	retval = ProcessResponseCode(info,ResPkt->ResponseCode,ResPkt->ReasonCode);
	
	exit:
	info->ProcessPending = 0;
	wake_up_interruptible(&NCSI_Process);
	return retval;
}


int 
NCSI_Issue_GetVersionID(NCSI_IF_INFO *info,UINT8 PackageID, UINT8 ChannelID, 
															UINT32 *Ver1,UINT32 *Ver2,UINT32 *Ver3)
{
	GET_VERSION_ID_REQ_PKT Pkt;
	GET_VERSION_ID_RES_PKT *ResPkt;

	UINT8 Channel;
	UINT8 *PayLoad;
	int cs_offset;
	int retval;

	*Ver1 = *Ver2 = *Ver3 = 0;

	/* Sleep for response or timeout */
	wait_event_interruptible_timeout(NCSI_Process, (info->ProcessPending == 0), NCSI_TIMEOUT);
	info->ProcessPending = 1;
	/* Basic Initialization */
	memset(&Pkt ,0, sizeof(Pkt));
	PayLoad = ((UINT8 *)&Pkt) + sizeof(NCSI_HDR);
	cs_offset = (UINT32)(&(Pkt.CheckSum)) - (UINT32)(&Pkt);

	Channel = MK_CH_ID(PackageID,ChannelID);

	/* Form NCSI Packet and send */
	FormNCSIPacket(info,CMD_GET_VERSION_ID,Channel,PayLoad,cs_offset);
	info->SendLen = sizeof(Pkt);
	retval = SendNCSICommand (info);
	if (retval != NCSI_ERR_SUCCESS)
		goto exit;

	/* Validate Response pkt */
	ResPkt = (GET_VERSION_ID_RES_PKT *)info->RecvData;
	if (info->RecvLen != sizeof(GET_VERSION_ID_RES_PKT))
	{
		printk("NCSI(%s): Expected Response Size = %zu Got %d\n",info->dev->name,
			sizeof(GET_VERSION_ID_RES_PKT),info->RecvLen);
		retval = NCSI_ERR_RESPONSE;
		goto exit;
	}

	/* Validate Response checksum */
	cs_offset = (UINT32)(&(ResPkt->CheckSum)) - (UINT32)(ResPkt);
	retval = NCSIValidateCheckSum(info,(UINT16*)info->RecvData,cs_offset);
	if (retval != NCSI_ERR_SUCCESS)
		goto exit;

	/* Check the Response code and reason */
	retval =  ProcessResponseCode(info,ResPkt->ResponseCode,ResPkt->ReasonCode);
	if (retval == NCSI_ERR_SUCCESS)
	{
		*Ver1 |= ResPkt->NCSIVerMajor << 24;
		*Ver1 |= ResPkt->NCSIVerMinor << 16;
		*Ver1 |= ResPkt->NCSIVerUpdate<< 8;
		*Ver1 |= ResPkt->NCSIVerAlpha1<< 0;

		*Ver2 = ResPkt->NCSIVerAlpha2;
		*Ver3 = ResPkt->IANA_ManID;
	}
	
	exit:
	info->ProcessPending = 0;
	wake_up_interruptible(&NCSI_Process);
	return retval;
}

int 
NCSI_Issue_GetCapabilities(NCSI_IF_INFO *info,UINT8 PackageID, UINT8 ChannelID,UINT32 *Caps)
{

	GET_CAPABILITIES_REQ_PKT Pkt;
	GET_CAPABILITIES_RES_PKT *ResPkt;

	UINT8 Channel;
	UINT8 *PayLoad;
	int cs_offset;
	int retval;

	/* Sleep for response or timeout */
	wait_event_interruptible_timeout(NCSI_Process, (info->ProcessPending == 0), NCSI_TIMEOUT);
	info->ProcessPending = 1;
	/* Basic Initialization */
	memset(&Pkt ,0, sizeof(Pkt));
	PayLoad = ((UINT8 *)&Pkt) + sizeof(NCSI_HDR);
	cs_offset = (UINT32)(&(Pkt.CheckSum)) - (UINT32)(&Pkt);

	Channel = MK_CH_ID(PackageID,ChannelID);

	/* Form NCSI Packet and send */
	FormNCSIPacket(info,CMD_GET_CAPABILITIES,Channel,PayLoad,cs_offset);
	info->SendLen = sizeof(Pkt);
	retval = SendNCSICommand (info);
	if (retval != NCSI_ERR_SUCCESS)
		goto exit;

	/* Validate Response pkt */
	ResPkt = (GET_CAPABILITIES_RES_PKT *)info->RecvData;
	if (info->RecvLen != sizeof(GET_CAPABILITIES_RES_PKT))
	{
		printk("NCSI(%s): Expected Response Size = %zu Got %d\n",info->dev->name,
			sizeof(GET_CAPABILITIES_RES_PKT),info->RecvLen);
		retval = NCSI_ERR_RESPONSE;
		goto exit;
	}

	/* Validate Response checksum */
	cs_offset = (UINT32)(&(ResPkt->CheckSum)) - (UINT32)(ResPkt);
	retval = NCSIValidateCheckSum(info,(UINT16*)info->RecvData,cs_offset);
	if (retval != NCSI_ERR_SUCCESS)
		goto exit;

	/* Check the Response code and reason */
	retval =  ProcessResponseCode(info,ResPkt->ResponseCode,ResPkt->ReasonCode);
	if (retval == NCSI_ERR_SUCCESS)
	{
		*Caps = be32_to_cpu(ResPkt->CapFlags);
	}
	
	exit:
	info->ProcessPending = 0;
	wake_up_interruptible(&NCSI_Process);
	return retval;
}

int 
NCSI_Issue_SetMacAddress(NCSI_IF_INFO *info,UINT8 PackageID, UINT8 ChannelID, UINT8 *MacAddr,
					UINT8 MacFilterNo,UINT8 MacType)
{
	SET_MAC_ADDR_REQ_PKT Pkt;
	SET_MAC_ADDR_RES_PKT *ResPkt;

	UINT8 Channel;
	UINT8 *PayLoad;
	int cs_offset;
	int retval;

	/* Sleep for response or timeout */
	wait_event_interruptible_timeout(NCSI_Process, (info->ProcessPending == 0), NCSI_TIMEOUT);
	info->ProcessPending = 1;
	/* Basic Initialization */
	memset(&Pkt ,0, sizeof(Pkt));
	PayLoad = ((UINT8 *)&Pkt) + sizeof(NCSI_HDR);
	cs_offset = (UINT32)(&(Pkt.CheckSum)) - (UINT32)(&Pkt);

	/* "SetMacAddr" Specific Parameters */
	Channel = MK_CH_ID(PackageID,ChannelID);
	Pkt.MACAddr5   = MacAddr[5];
	Pkt.MACAddr4   = MacAddr[4];
	Pkt.MACAddr3   = MacAddr[3];
	Pkt.MACAddr2   = MacAddr[2];
	Pkt.MACAddr1   = MacAddr[1];
	Pkt.MACAddr0   = MacAddr[0];
	Pkt.MacAddrNum = MacFilterNo;
	Pkt.TypeEnable  = (MacType << 5) | ENABLE_MAC_FILTER;	

	/* Form NCSI Packet and send */
	FormNCSIPacket(info,CMD_SET_MAC_ADDR,Channel,PayLoad,cs_offset);
	info->SendLen = sizeof(Pkt);
	retval = SendNCSICommand (info);
	if (retval != NCSI_ERR_SUCCESS)
		goto exit;

	/* Validate Response pkt */
	ResPkt = (SET_MAC_ADDR_RES_PKT *)info->RecvData;
	if (info->RecvLen != sizeof(SET_MAC_ADDR_RES_PKT))
	{
		printk("NCSI(%s): Expected Response Size = %zu Got %d\n",info->dev->name,
			sizeof(SET_MAC_ADDR_RES_PKT),info->RecvLen);
		retval = NCSI_ERR_RESPONSE;
		goto exit;
	}

	/* Validate Response checksum */
	cs_offset = (UINT32)(&(ResPkt->CheckSum)) - (UINT32)(ResPkt);
	retval = NCSIValidateCheckSum(info,(UINT16*)info->RecvData,cs_offset);
	if (retval != NCSI_ERR_SUCCESS)
		goto exit;

	/* Check the Response code and reason */
	retval = ProcessResponseCode(info,ResPkt->ResponseCode,ResPkt->ReasonCode);
	
	exit:
	info->ProcessPending = 0;
	wake_up_interruptible(&NCSI_Process);
	return retval;
}


int 
NCSI_Issue_EnableBcastFilter(NCSI_IF_INFO *info,UINT8 PackageID,UINT8 ChannelID, 
					UINT8 Arp,UINT8 DhcpC,UINT8 DhcpS,UINT8 NetBios)
{
	ENABLE_BCAST_FILTER_REQ_PKT Pkt;
	ENABLE_BCAST_FILTER_RES_PKT *ResPkt;

	UINT8 Channel;
	UINT8 *PayLoad;
	int cs_offset;
	int retval;

	/* Sleep for response or timeout */
	wait_event_interruptible_timeout(NCSI_Process, (info->ProcessPending == 0), NCSI_TIMEOUT);
	info->ProcessPending = 1;
	/* Basic Initialization */
	memset(&Pkt ,0, sizeof(Pkt));
	PayLoad = ((UINT8 *)&Pkt) + sizeof(NCSI_HDR);
	cs_offset = (UINT32)(&(Pkt.CheckSum)) - (UINT32)(&Pkt);

	Channel = MK_CH_ID(PackageID,ChannelID);
	Pkt.Settings = cpu_to_be32(Arp | DhcpC << 1 | DhcpS << 2 | NetBios << 3);

	/* Form NCSI Packet and send */
	FormNCSIPacket(info,CMD_ENABLE_BCAST_FILTER,Channel,PayLoad,cs_offset);
	info->SendLen = sizeof(Pkt);
	retval = SendNCSICommand (info);
	if (retval != NCSI_ERR_SUCCESS)
		goto exit;

	/* Validate Response pkt */
	ResPkt = (ENABLE_BCAST_FILTER_RES_PKT *)info->RecvData;
	if (info->RecvLen != sizeof(ENABLE_BCAST_FILTER_RES_PKT))
	{
		printk("NCSI(%s): Expected Response Size = %zu Got %d\n",info->dev->name,
			sizeof(ENABLE_BCAST_FILTER_RES_PKT),info->RecvLen);
		retval = NCSI_ERR_RESPONSE;
		goto exit;
	}

	/* Validate Response checksum */
	cs_offset = (UINT32)(&(ResPkt->CheckSum)) - (UINT32)(ResPkt);
	retval = NCSIValidateCheckSum(info,(UINT16*)info->RecvData,cs_offset);
	if (retval != NCSI_ERR_SUCCESS)
		goto exit;

	/* Check the Response code and reason */
	retval = ProcessResponseCode(info,ResPkt->ResponseCode,ResPkt->ReasonCode);
	
	exit:
	info->ProcessPending = 0;
	wake_up_interruptible(&NCSI_Process);
	return retval;
}

int 
NCSI_Issue_DisableBcastFilter(NCSI_IF_INFO *info,UINT8 PackageID,UINT8 ChannelID)
{
	DISABLE_BCAST_FILTER_REQ_PKT Pkt;
	DISABLE_BCAST_FILTER_RES_PKT *ResPkt;

	UINT8 Channel;
	UINT8 *PayLoad;
	int cs_offset;
	int retval;

	/* Sleep for response or timeout */
	wait_event_interruptible_timeout(NCSI_Process, (info->ProcessPending == 0), NCSI_TIMEOUT);
	info->ProcessPending = 1;
	/* Basic Initialization */
	memset(&Pkt ,0, sizeof(Pkt));
	PayLoad = ((UINT8 *)&Pkt) + sizeof(NCSI_HDR);
	cs_offset = (UINT32)(&(Pkt.CheckSum)) - (UINT32)(&Pkt);

	Channel = MK_CH_ID(PackageID,ChannelID);

	/* Form NCSI Packet and send */
	FormNCSIPacket(info,CMD_DISABLE_BCAST_FILTER,Channel,PayLoad,cs_offset);
	info->SendLen = sizeof(Pkt);
	retval = SendNCSICommand (info);
	if (retval != NCSI_ERR_SUCCESS)
		goto exit;

	/* Validate Response pkt */
	ResPkt = (DISABLE_BCAST_FILTER_RES_PKT *)info->RecvData;
	if (info->RecvLen != sizeof(DISABLE_BCAST_FILTER_RES_PKT))
	{
		printk("NCSI(%s): Expected Response Size = %zu Got %d\n",info->dev->name,
			sizeof(DISABLE_BCAST_FILTER_RES_PKT),info->RecvLen);
		retval = NCSI_ERR_RESPONSE;
		goto exit;
	}

	/* Validate Response checksum */
	cs_offset = (UINT32)(&(ResPkt->CheckSum)) - (UINT32)(ResPkt);
	retval = NCSIValidateCheckSum(info,(UINT16*)info->RecvData,cs_offset);
	if (retval != NCSI_ERR_SUCCESS)
		goto exit;

	/* Check the Response code and reason */
	retval = ProcessResponseCode(info,ResPkt->ResponseCode,ResPkt->ReasonCode);
	
	exit:
	info->ProcessPending = 0;
	wake_up_interruptible(&NCSI_Process);
	return retval;
}

int 
NCSI_Issue_EnableMcastFilter(NCSI_IF_INFO *info,UINT8 PackageID,UINT8 ChannelID, 
		UINT8 IPv6_Neighbour_Adv, UINT8 IPv6_Router_Adv, UINT8 IPv6_Multicast)
{
	ENABLE_MCAST_FILTER_REQ_PKT Pkt;
	ENABLE_MCAST_FILTER_RES_PKT *ResPkt;

	UINT8 Channel;
	UINT8 *PayLoad;
	int cs_offset;
	int retval;

	/* Sleep for response or timeout */
	wait_event_interruptible_timeout(NCSI_Process, (info->ProcessPending == 0), NCSI_TIMEOUT);
	info->ProcessPending = 1;
	/* Basic Initialization */
	memset(&Pkt ,0, sizeof(Pkt));
	PayLoad = ((UINT8 *)&Pkt) + sizeof(NCSI_HDR);
	cs_offset = (UINT32)(&(Pkt.CheckSum)) - (UINT32)(&Pkt);

	Channel = MK_CH_ID(PackageID,ChannelID);
	Pkt.Settings =cpu_to_be32(IPv6_Neighbour_Adv |  IPv6_Router_Adv << 1 | IPv6_Multicast << 2);

	/* Form NCSI Packet and send */
	FormNCSIPacket(info,CMD_ENABLE_MCAST_FILTER,Channel,PayLoad,cs_offset);
	info->SendLen = sizeof(Pkt);
	retval = SendNCSICommand (info);
	if (retval != NCSI_ERR_SUCCESS)
		goto exit;

	/* Validate Response pkt */
	ResPkt = (ENABLE_MCAST_FILTER_RES_PKT *)info->RecvData;
	if (info->RecvLen != sizeof(ENABLE_MCAST_FILTER_RES_PKT))
	{
		printk("NCSI(%s): Expected Response Size = %zu Got %d\n",info->dev->name,
			sizeof(ENABLE_MCAST_FILTER_RES_PKT),info->RecvLen);
		retval = NCSI_ERR_RESPONSE;
		goto exit;
	}

	/* Validate Response checksum */
	cs_offset = (UINT32)(&(ResPkt->CheckSum)) - (UINT32)(ResPkt);
	retval = NCSIValidateCheckSum(info,(UINT16*)info->RecvData,cs_offset);
	if (retval != NCSI_ERR_SUCCESS)
		goto exit;

	/* Check the Response code and reason */
	retval = ProcessResponseCode(info,ResPkt->ResponseCode,ResPkt->ReasonCode);
	
	exit:
	info->ProcessPending = 0;
	wake_up_interruptible(&NCSI_Process);
	return retval;
}



int 
NCSI_Issue_DisableMcastFilter(NCSI_IF_INFO *info,UINT8 PackageID,UINT8 ChannelID)
{
	DISABLE_MCAST_FILTER_REQ_PKT Pkt;
	DISABLE_MCAST_FILTER_RES_PKT *ResPkt;

	UINT8 Channel;
	UINT8 *PayLoad;
	int cs_offset;
	int retval;

	/* Sleep for response or timeout */
	wait_event_interruptible_timeout(NCSI_Process, (info->ProcessPending == 0), NCSI_TIMEOUT);
	info->ProcessPending = 1;
	/* Basic Initialization */
	memset(&Pkt ,0, sizeof(Pkt));
	PayLoad = ((UINT8 *)&Pkt) + sizeof(NCSI_HDR);
	cs_offset = (UINT32)(&(Pkt.CheckSum)) - (UINT32)(&Pkt);

	Channel = MK_CH_ID(PackageID,ChannelID);

	/* Form NCSI Packet and send */
	FormNCSIPacket(info,CMD_DISABLE_MCAST_FILTER,Channel,PayLoad,cs_offset);
	info->SendLen = sizeof(Pkt);
	retval = SendNCSICommand (info);
	if (retval != NCSI_ERR_SUCCESS)
		goto exit;

	/* Validate Response pkt */
	ResPkt = (DISABLE_MCAST_FILTER_RES_PKT *)info->RecvData;
	if (info->RecvLen != sizeof(DISABLE_MCAST_FILTER_RES_PKT))
	{
		printk("NCSI(%s): Expected Response Size = %zu Got %d\n",info->dev->name,
			sizeof(DISABLE_MCAST_FILTER_RES_PKT),info->RecvLen);
		retval = NCSI_ERR_RESPONSE;
	}

	/* Validate Response checksum */
	cs_offset = (UINT32)(&(ResPkt->CheckSum)) - (UINT32)(ResPkt);
	retval = NCSIValidateCheckSum(info,(UINT16*)info->RecvData,cs_offset);
	if (retval != NCSI_ERR_SUCCESS)
		goto exit;

	/* Check the Response code and reason */
	retval = ProcessResponseCode(info,ResPkt->ResponseCode,ResPkt->ReasonCode);
	
	exit:
	info->ProcessPending = 0;
	wake_up_interruptible(&NCSI_Process);
	return retval;
}


int 
NCSI_Issue_EnableAEN(NCSI_IF_INFO *info,UINT8 PackageID,UINT8 ChannelID,UINT8 LinkAEN,
						UINT8 ConfigAEN,UINT8 HostAEN)
{
	AEN_ENABLE_REQ_PKT Pkt;
	AEN_ENABLE_RES_PKT *ResPkt;

	UINT8 Channel;
	UINT8 *PayLoad;
	int cs_offset;
	int retval;

	/* Sleep for response or timeout */
	wait_event_interruptible_timeout(NCSI_Process, (info->ProcessPending == 0), NCSI_TIMEOUT);
	info->ProcessPending = 1;
	/* Basic Initialization */
	memset(&Pkt ,0, sizeof(Pkt));
	PayLoad = ((UINT8 *)&Pkt) + sizeof(NCSI_HDR);
	cs_offset = (UINT32)(&(Pkt.CheckSum)) - (UINT32)(&Pkt);

	Channel = MK_CH_ID(PackageID,ChannelID);
	Pkt.AenCtrl   = cpu_to_be32(LinkAEN | ConfigAEN << 1 | HostAEN << 2);
	Pkt.AEN_MC_ID = NCSI_MC_ID;

	/* Form NCSI Packet and send */
	FormNCSIPacket(info,CMD_AEN_ENABLE,Channel,PayLoad,cs_offset);
	info->SendLen = sizeof(Pkt);
	retval = SendNCSICommand (info);
	if (retval != NCSI_ERR_SUCCESS)
		goto exit;

	/* Validate Response pkt */
	ResPkt = (AEN_ENABLE_RES_PKT *)info->RecvData;
	if (info->RecvLen != sizeof(AEN_ENABLE_RES_PKT))
	{
		printk("NCSI(%s): Expected Response Size = %zu Got %d\n",info->dev->name,
			sizeof(AEN_ENABLE_RES_PKT),info->RecvLen);
		retval = NCSI_ERR_RESPONSE;
		goto exit;
	}

	/* Validate Response checksum */
	cs_offset = (UINT32)(&(ResPkt->CheckSum)) - (UINT32)(ResPkt);
	retval = NCSIValidateCheckSum(info,(UINT16*)info->RecvData,cs_offset);
	if (retval != NCSI_ERR_SUCCESS)
		goto exit;

	/* Check the Response code and reason */
	retval = ProcessResponseCode(info,ResPkt->ResponseCode,ResPkt->ReasonCode);
	
	exit:
	info->ProcessPending = 0;
	wake_up_interruptible(&NCSI_Process);
	return retval;
}




int 
NCSI_Issue_GetLinkStatus(NCSI_IF_INFO *info,UINT8 PackageID, UINT8 ChannelID, UINT32 *LinkStatus)
{
	GET_LINK_STATUS_REQ_PKT Pkt;
	GET_LINK_STATUS_RES_PKT *ResPkt;

	UINT8 Channel;
	UINT8 *PayLoad;
	int cs_offset;
	int retval;

	*LinkStatus = 0;

	/* Sleep for response or timeout */
	wait_event_interruptible_timeout(NCSI_Process, (info->ProcessPending == 0), NCSI_TIMEOUT);
	info->ProcessPending = 1;
	/* Basic Initialization */
	memset(&Pkt ,0, sizeof(Pkt));
	PayLoad = ((UINT8 *)&Pkt) + sizeof(NCSI_HDR);
	cs_offset = (UINT32)(&(Pkt.CheckSum)) - (UINT32)(&Pkt);

	Channel = MK_CH_ID(PackageID,ChannelID);

	/* Form NCSI Packet and send */
	FormNCSIPacket(info,CMD_GET_LINK_STATUS,Channel,PayLoad,cs_offset);
	info->SendLen = sizeof(Pkt);
	retval = SendNCSICommand (info);
	if (retval != NCSI_ERR_SUCCESS)
		goto exit;

	/* Validate Response pkt */
	ResPkt = (GET_LINK_STATUS_RES_PKT *)info->RecvData;
	if (info->RecvLen != sizeof(GET_LINK_STATUS_RES_PKT))
	{
		printk("NCSI(%s): Expected Response Size = %zu Got %d\n",info->dev->name,
			sizeof(GET_LINK_STATUS_RES_PKT),info->RecvLen);
		retval = NCSI_ERR_RESPONSE;
		goto exit;
	}

	/* Validate Response checksum */
	cs_offset = (UINT32)(&(ResPkt->CheckSum)) - (UINT32)(ResPkt);
	retval = NCSIValidateCheckSum(info,(UINT16*)info->RecvData,cs_offset);
	if (retval != NCSI_ERR_SUCCESS)
		goto exit;

	/* Check the Response code and reason */
	retval =  ProcessResponseCode(info,ResPkt->ResponseCode,ResPkt->ReasonCode);
	if (retval == NCSI_ERR_SUCCESS)
	{
		*LinkStatus = be32_to_cpu(ResPkt->LinkStatus);
//		printk("Link Status = 0x%08X\n",be32_to_cpu(ResPkt->LinkStatus));
//		printk("OEM Link Status = 0x%08X\n",be32_to_cpu(ResPkt->OEMLinkStatus));
//		printk("Other Indicators = 0x%08X\n",be32_to_cpu(ResPkt->OtherIndicators));
	}
	
	exit:
	info->ProcessPending = 0;
	wake_up_interruptible(&NCSI_Process);
	return retval;
}

int 
NCSI_Issue_GetParameters(NCSI_IF_INFO *info, UINT8 PackageID, UINT8 ChannelID, UINT32 *ConfigFlags, UINT32 *AENControl)
{
	GET_PARAMETERS_REQ_PKT Pkt;
	GET_PARAMETERS_RES_PKT *ResPkt;

	UINT8 Channel;
	UINT8 *PayLoad;
	int cs_offset;
	int retval;

	/* Sleep for response or timeout */
	wait_event_interruptible_timeout(NCSI_Process, (info->ProcessPending == 0), NCSI_TIMEOUT);
	info->ProcessPending = 1;
	/* Basic Initialization */
	memset(&Pkt ,0, sizeof(Pkt));
	PayLoad = ((UINT8 *)&Pkt) + sizeof(NCSI_HDR);
	cs_offset = (UINT32)(&(Pkt.CheckSum)) - (UINT32)(&Pkt);

	Channel = MK_CH_ID(PackageID,ChannelID);

	/* Form NCSI Packet and send */
	FormNCSIPacket(info,CMD_GET_PARAMETERS,Channel,PayLoad,cs_offset);
	info->SendLen = sizeof(Pkt);
	retval = SendNCSICommand (info);
	if (retval != NCSI_ERR_SUCCESS)
		goto exit;

	/* Validate Response pkt */
	ResPkt = (GET_PARAMETERS_RES_PKT *)info->RecvData;
	if (info->RecvLen != sizeof(GET_PARAMETERS_RES_PKT))
	{
		printk("NCSI(%s): Expected Response Size = %zu Got %d\n",info->dev->name,
					sizeof(GET_PARAMETERS_RES_PKT),info->RecvLen);
		retval = NCSI_ERR_RESPONSE;
		goto exit;
	}

	/* Validate Response checksum */
	cs_offset = (UINT32)(&(ResPkt->CheckSum)) - (UINT32)(ResPkt);
	retval = NCSIValidateCheckSum(info,(UINT16*)info->RecvData,cs_offset);
	if (retval != NCSI_ERR_SUCCESS)
		goto exit;

	/* Check the Response code and reason */
	retval =  ProcessResponseCode(info,ResPkt->ResponseCode,ResPkt->ReasonCode);
	if (retval == NCSI_ERR_SUCCESS)
	{
		printk("NCSI(%s): Config = 0x%lx\n",info->dev->name,ResPkt->ConfigurationFlags);
		*ConfigFlags = be32_to_cpu(ResPkt->ConfigurationFlags);
		*AENControl = be32_to_cpu(ResPkt->AENControl);
			/*TODO: Send back other useful info ? */
	}

	exit:
	info->ProcessPending = 0;
	wake_up_interruptible(&NCSI_Process);
	return retval;
}

int 
NCSI_Issue_OEM_SetIntelManagementControlCommand(NCSI_IF_INFO *info,UINT8 PackageID, UINT8 ChannelID,UINT8 IntelManagementControl) 
{ 
    INTEL_OEM_SET_MANAGEMENT_CONTROL_REQ_PKT Pkt; 
    INTEL_OEM_SET_MANAGEMENT_CONTROL_RES_PKT *ResPkt; 

    UINT8 Channel; 
    UINT8 *PayLoad; 
    int IntelMangeControl_offset; 
    int retval; 

    /* Sleep for response or timeout */
    wait_event_interruptible_timeout(NCSI_Process, (info->ProcessPending == 0), NCSI_TIMEOUT);
    info->ProcessPending = 1;

    /* Basic Initialization */ 
    memset(&Pkt ,0, sizeof(Pkt)); 
    PayLoad = ((UINT8 *)&Pkt) + sizeof(NCSI_HDR); 
    IntelMangeControl_offset = (UINT32)(&(Pkt.IntelManagementControl)) - (UINT32)(&Pkt); 

    /* "Set Intel Management control" Specific Parameters */ 
    Channel = MK_CH_ID(PackageID,ChannelID); 
    Pkt.ManufacturerId = cpu_to_be32(0x157); 
    Pkt.IntelOemCommand = 0x20; 
    Pkt.Reserved01 = 0x0; 
    Pkt.IntelManagementControl = IntelManagementControl; 

    /* Form NCSI Packet and send */ 
    FormIntelOemNCSIPacket(info,Channel,PayLoad, IntelMangeControl_offset); 
    info->SendLen = sizeof(Pkt); 
    retval = SendNCSICommand (info); 
    if (retval != NCSI_ERR_SUCCESS) 
        goto exit; 

    /* Validate Response pkt */ 
    ResPkt = (INTEL_OEM_SET_MANAGEMENT_CONTROL_RES_PKT *)info->RecvData; 
    if (info->RecvLen != sizeof(INTEL_OEM_SET_MANAGEMENT_CONTROL_RES_PKT)) 
    { 
        printk("NCSI(%s): Expected Response Size = %d Got %d\n",info->dev->name, 
        sizeof(INTEL_OEM_SET_MANAGEMENT_CONTROL_RES_PKT),info->RecvLen); 
        retval = NCSI_ERR_RESPONSE;
        goto exit;
    } 

    /* Check the Response code and reason */ 
    retval = ProcessResponseCode(info,ResPkt->ResponseCode,ResPkt->ReasonCode);
    
    exit:
    info->ProcessPending = 0;
    wake_up_interruptible(&NCSI_Process);
    return retval;
} 

static 
void 
FormIntelOemNCSIPacket(NCSI_IF_INFO *info,UINT8 Channel,UINT8 *PayLoad, int LastByte_offset) 
{ 
    ETH_HDR         *EthHdr; 
    NCSI_HDR        *NcsiHdr; 
    UINT8           *NcsiPkt,*NcsiPayLoad; 
    UINT32                  *LastByte; 
    UINT32          PayLoadLen; 

    memset(info->SendData,0,BUFFER_SIZE); 
    /* Get the locations of various fields */ 
    EthHdr      = (ETH_HDR *)(info->SendData); 
    NcsiPkt        =  info->SendData + sizeof(ETH_HDR); 
    NcsiHdr     = (NCSI_HDR *)NcsiPkt; 
    NcsiPayLoad     =  info->SendData + sizeof(ETH_HDR) + sizeof(NCSI_HDR); 
    LastByte        = (UINT32 *)(NcsiPkt+LastByte_offset + 1); 
    PayLoadLen      =  (UINT32)LastByte - (UINT32)NcsiPayLoad; 

    /* Fill in Ethernet Header */ 
    memset(EthHdr->DestMACAddr,0xFF,6); 
    memset(EthHdr->SrcMACAddr,0xFF,6); 
    EthHdr->EtherType = cpu_to_be16(NCSI_ETHER_TYPE); 

    /* Fill in NCSI Header */ 
    memset(NcsiHdr,0, sizeof(NCSI_HDR)); 
    NcsiHdr->MC_ID         = NCSI_MC_ID; 
    NcsiHdr->HdrRev     = NCSI_HDR_REV; 
    info->LastInstanceID++; 
    /* Instance ID  = 1 to 0xFF . 0 is invalid */ 
    info->LastInstanceID= (info->LastInstanceID == 0)?1:info->LastInstanceID;     
    NcsiHdr->I_ID        = info->LastInstanceID; 
    NcsiHdr->Command     = CMD_OEM_CMD; 
    NcsiHdr->CH_ID        = Channel; 
    NcsiHdr->PayloadLen = cpu_to_be16(PayLoadLen); 

    /* Save some info */ 
    info->LastCommand      = CMD_OEM_CMD; 
    info->LastChannelID    = Channel; 
    info->LastManagementID = NCSI_MC_ID; 
    
    /* Copy the PayLoad */ 
    memcpy(NcsiPayLoad,PayLoad,PayLoadLen); 
    return; 
}  

int
NCSI_Issue_SetLink(NCSI_IF_INFO *info,UINT8 PackageID,UINT8 ChannelID, UINT8 AutoNeg,
	UINT32 Speed, UINT32 Duplex)
{
	SET_LINK_REQ_PKT Pkt;
	SET_LINK_RES_PKT *ResPkt;
	UINT32 LinkSettings;

	UINT8 Channel;
	UINT8 *PayLoad;
	int cs_offset;
	int retval, retry = 0;

	/* Sleep for response or timeout */
	wait_event_interruptible_timeout(NCSI_Process, (info->ProcessPending == 0), NCSI_TIMEOUT);
	info->ProcessPending = 1;
	
	/*
	 * Procedure:
	 * 		1. Try with enabling 10G and 1G 
	 * 		2. If it returns parameter conflict error then #3 else come out of loop.
	 * 		3. Try with disabling 10G, check if error then #4 else come out of loop
	 * 		4. Try with disabling 10G and 1G.
	 * 		5. Respond with error if all above fails.
	 */
	do
	{
		/* Basic Initialization */
		memset(&Pkt ,0, sizeof(Pkt));
		PayLoad = ((UINT8 *)&Pkt) + sizeof(NCSI_HDR);
		cs_offset = (UINT32)(&(Pkt.CheckSum)) - (UINT32)(&Pkt);
	
		/* "SetLink" Specific Parameters */
		Channel = MK_CH_ID(PackageID,ChannelID);
	
		LinkSettings = LINK_ENABLE_PAUSE_CAPS;
		LinkSettings |= LINK_ENABLE_ASYNC_PAUSE_CAPS;
		if (AutoNeg)
		{
			LinkSettings |= LINK_ENABLE_AUTO_NEG;
			LinkSettings |= LINK_ENABLE_10_MBPS;
			LinkSettings |= LINK_ENABLE_100_MBPS;
	#if NCSI_1G_SUPPORT
			if (retry < 2)
				LinkSettings |= LINK_ENABLE_1000_MBPS;
	#endif
	#if NCSI_10G_SUPPORT
			if (retry < 1)
				LinkSettings |= LINK_ENABLE_10_GBPS;
	#endif
			LinkSettings |= LINK_ENABLE_HALF_DUPLEX;
			LinkSettings |= LINK_ENABLE_FULL_DUPLEX;
		}
		else
		{
			LinkSettings |= Speed | Duplex;
		}
		Pkt.LinkSettings = cpu_to_be32(LinkSettings);
		
	
		/* Form NCSI Packet and send */
		FormNCSIPacket(info,CMD_SET_LINK,Channel,PayLoad,cs_offset);
		info->SendLen = sizeof(Pkt);
		retval = SendNCSICommand (info);
		if (retval != NCSI_ERR_SUCCESS)
			goto exit;
	
		/* Validate Response pkt */
		ResPkt = (SET_LINK_RES_PKT *)info->RecvData;
		if (info->RecvLen != sizeof(SET_LINK_RES_PKT))
		{
			printk("NCSI(%s): Expected Response Size = %zu Got %d\n",info->dev->name,
				sizeof(DISABLE_CHANNEL_RES_PKT),info->RecvLen);
			retval = NCSI_ERR_RESPONSE;
			goto exit;
		}
	
		/* Validate Response checksum */
		cs_offset = (UINT32)(&(ResPkt->CheckSum)) - (UINT32)(ResPkt);
		retval = NCSIValidateCheckSum(info,(UINT16*)info->RecvData,cs_offset);
		if (retval != NCSI_ERR_SUCCESS)
			goto exit;
	
		/* Check the Response code and reason */
		retval = ProcessResponseCode(info,ResPkt->ResponseCode,ResPkt->ReasonCode);
		retry++;
				
		if (retval == NCSI_ERR_SUCCESS)
			break;
				
	} while ( AutoNeg && ( (be16_to_cpu(ResPkt->ReasonCode) & 0xFF) == SET_LINK_REASON_PARAMETER_CONFLICT ) && (retry < 3) );
			
	
	exit:
	info->ProcessPending = 0;
	wake_up_interruptible(&NCSI_Process);
	return retval;
}

/*
*********************************************************************************************************
*                                            NCSI_Issue_SetVLANFilter()
*
* Description : Set the VLAN ID filter to NCSI controller to append the VLAN ID to out going packets and checks the VLANID for incoming packets  :

* Argument(s) : NCSI_IF_INFO pointer, PackageID, ChannelID,  VLANId, FilterSelector.
*
* Return(s)   : Returns the Command response code.
*
* Caller(s)   : FormNCSIPacket(), SendNCSICommand(), NCSIValidateCheckSum(),ProcessResponseCode() 
*
* Note(s)     : none.
*********************************************************************************************************
*/
int 
NCSI_Issue_SetVLANFilter(NCSI_IF_INFO *info,UINT8 PackageID, UINT8 ChannelID, UINT16 VLANId, 
									UINT8 FilterSelector)
{
	SET_VLAN_FILTER_REQ_PKT Pkt;
	SET_VLAN_FILTER_RES_PKT *ResPkt;

	UINT8 Channel;
	UINT8 *PayLoad;
	int cs_offset;
	int retval;

	/* Sleep for response or timeout */
	wait_event_interruptible_timeout(NCSI_Process, (info->ProcessPending == 0), NCSI_TIMEOUT);
	info->ProcessPending = 1;
	/* Basic Initialization */
	memset(&Pkt ,0, sizeof(Pkt));
	PayLoad = ((UINT8 *)&Pkt) + sizeof(NCSI_HDR);
	cs_offset = (UINT32)(&(Pkt.CheckSum)) - (UINT32)(&Pkt);

	/*Set the VLAN ID specific inforamtion to NCSI packet  */
	Channel = MK_CH_ID(PackageID,ChannelID);
	Pkt.UserPri_CFI_VLANID = cpu_to_be16(VLANId);
	Pkt.FilterSelector = FilterSelector;
	Pkt.Enable   = TRUE;
	
	/* Form NCSI Packet and send */
	FormNCSIPacket(info,CMD_SET_VLAN_FILTER,Channel,PayLoad,cs_offset);
	info->SendLen = sizeof(Pkt);
	retval = SendNCSICommand (info);
	if (retval != NCSI_ERR_SUCCESS)
		goto exit;

	/* Validate Response pkt */
	ResPkt = (AEN_ENABLE_RES_PKT *)info->RecvData;
	if (info->RecvLen != sizeof(AEN_ENABLE_RES_PKT))
	{
		printk("NCSI(%s): Expected Response Size = %zu Got %d\n",info->dev->name,
			sizeof(AEN_ENABLE_RES_PKT),info->RecvLen);
		retval = NCSI_ERR_RESPONSE;
		goto exit;
	}

	/* Validate Response checksum */
	cs_offset = (UINT32)(&(ResPkt->CheckSum)) - (UINT32)(ResPkt);
	retval = NCSIValidateCheckSum(info,(UINT16*)info->RecvData,cs_offset);
	if (retval != NCSI_ERR_SUCCESS)
		goto exit;

	/* Check the Response code and reason */
	retval = ProcessResponseCode(info,ResPkt->ResponseCode,ResPkt->ReasonCode);
	
	exit:
	info->ProcessPending = 0;
	wake_up_interruptible(&NCSI_Process);
	return retval;
}


/*
*********************************************************************************************************
*                                            NCSI_Issue_EnableVLAN()
*
* Description : Enable VLAN filter mode to accept VLAN tagged packets:

* Argument(s) : NCSI_IF_INFO pointer, PackageID, ChannelID, VLANFilterMode .
*
* Return(s)   : Returns the Command response code.
*
* Caller(s)   : FormNCSIPacket(), SendNCSICommand(), NCSIValidateCheckSum(),ProcessResponseCode() 
*
* Note(s)     : none.
*********************************************************************************************************
*/
int 
NCSI_Issue_EnableVLAN(NCSI_IF_INFO *info,UINT8 PackageID, UINT8 ChannelID, 
								UINT8 VLANFilterMode)
{
	ENABLE_VLAN_REQ_PKT Pkt;
	ENABLE_VLAN_RES_PKT *ResPkt;

	UINT8 Channel;
	UINT8 *PayLoad;
	int cs_offset;
	int retval;

	/* Sleep for response or timeout */
	wait_event_interruptible_timeout(NCSI_Process, (info->ProcessPending == 0), NCSI_TIMEOUT);
	info->ProcessPending = 1;
	/* Basic Initialization */
	memset(&Pkt ,0, sizeof(Pkt));
	PayLoad = ((UINT8 *)&Pkt) + sizeof(NCSI_HDR);
	cs_offset = (UINT32)(&(Pkt.CheckSum)) - (UINT32)(&Pkt);

	/*Set the Enable VLAN filter  */
	Channel = MK_CH_ID(PackageID,ChannelID);
	Pkt.Mode = VLANFilterMode;
	
	/* Form NCSI Packet and send */
	FormNCSIPacket(info,CMD_ENABLE_VLAN,Channel,PayLoad,cs_offset);
	info->SendLen = sizeof(Pkt);
	retval = SendNCSICommand (info);
	if (retval != NCSI_ERR_SUCCESS)
		goto exit;

	/* Validate Response pkt */
	ResPkt = (AEN_ENABLE_RES_PKT *)info->RecvData;
	if (info->RecvLen != sizeof(AEN_ENABLE_RES_PKT))
	{
		printk("NCSI(%s): Expected Response Size = %zu Got %d\n",info->dev->name,
			sizeof(AEN_ENABLE_RES_PKT),info->RecvLen);
		retval = NCSI_ERR_RESPONSE;
		goto exit;
	}

	/* Validate Response checksum */
	cs_offset = (UINT32)(&(ResPkt->CheckSum)) - (UINT32)(ResPkt);
	retval = NCSIValidateCheckSum(info,(UINT16*)info->RecvData,cs_offset);
	if (retval != NCSI_ERR_SUCCESS)
		goto exit;

	/* Check the Response code and reason */
	retval = ProcessResponseCode(info,ResPkt->ResponseCode,ResPkt->ReasonCode);
	
	exit:
	info->ProcessPending = 0;
	wake_up_interruptible(&NCSI_Process);
	return retval;
}

/*
*********************************************************************************************************
*                                            NCSI_Issue_DisableVLAN()
*
* Description : Disable VLAN filter mode not to accept VLAN tagged packets:

* Argument(s) : NCSI_IF_INFO pointer, PackageID, ChannelID, VLANFilterMode .
*
* Return(s)   : Returns the Command response code.
*
* Caller(s)   : FormNCSIPacket(), SendNCSICommand(), NCSIValidateCheckSum(),ProcessResponseCode() 
*
* Note(s)     : none.
*********************************************************************************************************
*/
int 
NCSI_Issue_DisableVLAN(NCSI_IF_INFO *info,UINT8 PackageID, UINT8 ChannelID)
{
	ENABLE_VLAN_REQ_PKT Pkt;
	ENABLE_VLAN_RES_PKT *ResPkt;

	UINT8 Channel;
	UINT8 *PayLoad;
	int cs_offset;
	int retval;

	/* Sleep for response or timeout */
	wait_event_interruptible_timeout(NCSI_Process, (info->ProcessPending == 0), NCSI_TIMEOUT);
	info->ProcessPending = 1;
	/* Basic Initialization */
	memset(&Pkt ,0, sizeof(Pkt));
	PayLoad = ((UINT8 *)&Pkt) + sizeof(NCSI_HDR);
	cs_offset = (UINT32)(&(Pkt.CheckSum)) - (UINT32)(&Pkt);

	/************************************************************************/
	/* For this command needs to send  just command and no data is required for this command */	
	/************************************************************************/
	Channel = MK_CH_ID(PackageID,ChannelID);
	
	/* Form NCSI Packet and send */
	FormNCSIPacket(info,CMD_DISABLE_VLAN,Channel,PayLoad,cs_offset);
	info->SendLen = sizeof(Pkt);
	retval = SendNCSICommand (info);
	if (retval != NCSI_ERR_SUCCESS)
		goto exit;

	/* Validate Response pkt */
	ResPkt = (AEN_ENABLE_RES_PKT *)info->RecvData;
	if (info->RecvLen != sizeof(AEN_ENABLE_RES_PKT))
	{
		printk("NCSI(%s): Expected Response Size = %zu Got %d\n",info->dev->name,
			sizeof(AEN_ENABLE_RES_PKT),info->RecvLen);
		retval = NCSI_ERR_RESPONSE;
		goto exit;
	}

	/* Validate Response checksum */
	cs_offset = (UINT32)(&(ResPkt->CheckSum)) - (UINT32)(ResPkt);
	retval = NCSIValidateCheckSum(info,(UINT16*)info->RecvData,cs_offset);
	if (retval != NCSI_ERR_SUCCESS)
		goto exit;

	/* Check the Response code and reason */
	retval = ProcessResponseCode(info,ResPkt->ResponseCode,ResPkt->ReasonCode);
	
	exit:
	info->ProcessPending = 0;
	wake_up_interruptible(&NCSI_Process);
	return retval;
}


int 
NCSI_Issue_UserCommand (NCSI_IF_INFO *info,UINT8 PackageID, UINT8 ChannelID, unsigned int Command, unsigned int *Data, unsigned int Length)
{
	SEND_USER_COMMAND_REQ_PKT Pkt;
	SEND_USER_COMMAND_RES_PKT *ResPkt;

	UINT8 Channel;
	UINT8 *PayLoad;
	int cs_offset;
	int retval, i;

	/* Sleep for response or timeout */
	wait_event_interruptible_timeout(NCSI_Process, (info->ProcessPending == 0), NCSI_TIMEOUT);
	info->ProcessPending = 1;
	
	/* Basic Initialization */
	memset(&Pkt ,0, sizeof(Pkt));
	PayLoad = ((UINT8 *)&Pkt) + sizeof(NCSI_HDR);
	cs_offset = Length + sizeof(NCSI_HDR);

	Channel = MK_CH_ID(PackageID, ChannelID);
	for (i = 0; i < Length; i++)
	{
		Pkt.ReqData[i] = Data[i];
	}
	
	/* Form NCSI Packet and send */
	if (Command == CMD_OEM_CMD)
		FormIntelOemNCSIPacket(info, Channel, PayLoad, cs_offset - 1);
	else
		FormNCSIPacket(info, Command, Channel, PayLoad, cs_offset);

	info->SendLen = sizeof(Pkt);
	retval = SendNCSICommand (info);
	if (retval != NCSI_ERR_SUCCESS)
		goto exit;
	
	ResPkt = (SEND_USER_COMMAND_RES_PKT *)info->RecvData;
	UserLastResponse[0] = Command;
	UserLastResponse[1] = info->RecvLen;
	for (i = 0; i < info->RecvLen; i++)
	{
		UserLastResponse[i + 2] = info->RecvData[i];
	}

	/* Check the Response code and reason */
	retval = ProcessResponseCode(info, ResPkt->ResponseCode, ResPkt->ReasonCode);
	
	exit:
	info->ProcessPending = 0;
	wake_up_interruptible(&NCSI_Process);
	return retval;
}


int IOCTL_issue_ncsi_command(NCSI_IF_INFO *info, ncsi_ioctl_data_T *ncsi_ioctl_data)
{
	NCSI_SEND_USER_COMMAND_GET_RESPONSE_REQ_PKT Pkt;
	NCSI_SEND_USER_COMMAND_GET_RESPONSE_RES_PKT *ResPkt;

	UINT8 Channel;
	UINT8 *PayLoad;
	int cs_offset;
	int retval, i;
	
		/* Sleep for response or timeout */
		wait_event_interruptible_timeout(NCSI_Process, (info->ProcessPending == 0), NCSI_TIMEOUT);
		info->ProcessPending = 1;
		
		/* Basic Initialization */
		memset(&Pkt ,0, sizeof(Pkt));
		PayLoad = ((UINT8 *)&Pkt) + sizeof(NCSI_HDR);
		cs_offset = ncsi_ioctl_data->req_packet_payload_length + sizeof(NCSI_HDR);
	
		Channel = MK_CH_ID(ncsi_ioctl_data->packageid, ncsi_ioctl_data->channelid);
		for (i = 0; i < ncsi_ioctl_data->req_packet_payload_length; i++)
		{
			Pkt.ReqData[i] = ncsi_ioctl_data->req_packet_payload[i];
		}
		
		
		FormNCSIPacket(info, ncsi_ioctl_data->cmd, Channel, PayLoad, cs_offset);
	

	info->SendLen = sizeof(Pkt);
	retval = SendNCSICommand (info);
	if (retval != NCSI_ERR_SUCCESS)
		goto exit;

	ResPkt = (NCSI_SEND_USER_COMMAND_GET_RESPONSE_RES_PKT *)info->RecvData;
	
	/* Validate Response checksum */
	cs_offset = be16_to_cpu (ResPkt->NcsiHdr.PayloadLen) + sizeof(NCSI_HDR);
	retval = NCSIValidateCheckSum(info,(UINT16*)info->RecvData,cs_offset);
	if (retval != NCSI_ERR_SUCCESS)
		goto exit;
	
	
		UserLastResponse[0] = ncsi_ioctl_data->cmd;
	UserLastResponse[1] = info->RecvLen;
	for (i = 0; i < info->RecvLen; i++)
	{
		UserLastResponse[i + 2] = info->RecvData[i];
	}


	/* Check the Response code and reason */
	retval = ProcessResponseCode(info, ResPkt->ResponseCode, ResPkt->ReasonCode);

	if (retval == NCSI_ERR_SUCCESS)
	{
			ncsi_ioctl_data->res_packet_payload_length = be16_to_cpu (ResPkt->NcsiHdr.PayloadLen);
		
			memset (ncsi_ioctl_data->res_packet_payload, 0, sizeof(unsigned char)*PAYLOAD_BUFFER_SIZE);
	
			memcpy(ncsi_ioctl_data->res_packet_payload, &ResPkt->ResponseCode,2);
			memcpy(ncsi_ioctl_data->res_packet_payload + 2,&ResPkt->ReasonCode,2);
			memcpy(ncsi_ioctl_data->res_packet_payload + 4,ResPkt->ResData,ncsi_ioctl_data->res_packet_payload_length - 4); //copy the rest of payloadbe16_to_cpu (ResPkt->NcsiHdr.PayloadLen)
					
		}
		
		exit:
		info->ProcessPending = 0;
		wake_up_interruptible(&NCSI_Process);
		return retval;
}



#ifdef CONFIG_SPX_FEATURE_QUANTA_FB

static void FormNCSIOEMPacket(NCSI_IF_INFO *info, UINT8 Command, UINT8 Channel, UINT8 *PayLoad, int cs_offset)
{
	ETH_HDR   *EthHdr;
	NCSI_HDR  *NcsiHdr;
	UINT8     *NcsiPkt, *NcsiPayLoad;
	UINT32    *CheckSum;
	UINT32    PayLoadLen;

	memset(info->SendData, 0, BUFFER_SIZE);
	/* Get the locations of various fields */
	EthHdr      = (ETH_HDR *)(info->SendData);
	NcsiPkt     = info->SendData + sizeof(ETH_HDR);
	NcsiHdr     = (NCSI_HDR *)NcsiPkt;
	NcsiPayLoad = info->SendData + sizeof(ETH_HDR) + sizeof(NCSI_HDR);
	CheckSum    = (UINT32 *)(NcsiPkt+cs_offset);
	PayLoadLen  = (UINT32)CheckSum - (UINT32)NcsiPayLoad;

	/* Fill in Ethernet Header */
	memset(EthHdr->DestMACAddr, 0xFF, 6);
	memset(EthHdr->SrcMACAddr, 0xFF, 6);
	EthHdr->EtherType = cpu_to_be16(NCSI_ETHER_TYPE);

	/* Fill in NCSI Header */
	memset(NcsiHdr, 0, sizeof(NCSI_HDR));
	NcsiHdr->MC_ID       = NCSI_MC_ID;
	NcsiHdr->HdrRev      = NCSI_HDR_REV;
	info->LastInstanceID++;
	/* Instance ID  = 1 to 0xFF . 0 is invalid */
	info->LastInstanceID = (info->LastInstanceID == 0)?1:info->LastInstanceID;
	NcsiHdr->I_ID        = info->LastInstanceID;
	NcsiHdr->Command     = Command;
	NcsiHdr->CH_ID       = Channel;
	NcsiHdr->PayloadLen  = cpu_to_be16(PayLoadLen);

	/* Save some info */
	info->LastCommand      = Command;
	info->LastChannelID    = Channel;
	info->LastManagementID = NCSI_MC_ID;

	/* Copy the PayLoad */
	memcpy(NcsiPayLoad, PayLoad, PayLoadLen);
	return;
}
int 
NCSI_Issue_GetIntelMacAddress(NCSI_IF_INFO *info,UINT8 PackageID, UINT8 ChannelID, UINT8 *MacAddr)
{
	OEM_GET_MAC_ADDR_REQ_PKT Pkt;
	OEM_GET_MAC_ADDR_RES_PKT *ResPkt;

	UINT8 Channel;
	UINT8 *PayLoad;
	int cs_offset;
	int retval;

	/* Basic Initialization */
	memset(&Pkt, 0, sizeof(Pkt));
	PayLoad   = ((UINT8 *)&Pkt) + sizeof(NCSI_HDR);
	cs_offset = (UINT32)(sizeof(NCSI_HDR)) + (UINT32)5;

	Channel      = MK_CH_ID(PackageID, ChannelID);
	Pkt.ManufID  = cpu_to_be32(INTEL_IANA_ID);
	Pkt.OemCmdID = INTEL_OEM_GET_MAC_ADDR;
	
	/* Form NCSI Packet and send */
	FormNCSIOEMPacket(info, CMD_OEM_CMD, Channel, PayLoad, cs_offset);
	info->SendLen = sizeof(Pkt);
	retval = SendNCSICommand(info);
	if (retval != NCSI_ERR_SUCCESS) {
		printk("SendNCSICommand() failed: %d\n", retval);
		return retval;
	}
	/* Validate Response pkt */
	ResPkt = (OEM_GET_MAC_ADDR_RES_PKT *)info->RecvData;
	if (info->RecvLen != sizeof(OEM_GET_MAC_ADDR_RES_PKT)) {
		printk("NCSI(%s): Expected Response Size = %d Got %d\n", info->dev->name,
			   sizeof(OEM_GET_MAC_ADDR_RES_PKT), info->RecvLen);
		return NCSI_ERR_RESPONSE;
	}

	memcpy(MacAddr,ResPkt->MAC_Addr,6);
	/* Check the Response code and reason */
	return ProcessResponseCode(info, ResPkt->ResponseCode, ResPkt->ReasonCode);
}

int 
NCSI_Issue_GetMellanoxMacAddress(NCSI_IF_INFO *info,UINT8 PackageID, UINT8 ChannelID, UINT8 *MacAddr)
{
	OEM_GET_MELLANOX_MAC_ADDR_REQ_PKT Pkt;
	OEM_GET_MELLANOX_MAC_ADDR_RES_PKT *ResPkt;

	UINT8 Channel;
	UINT8 *PayLoad;
	int cs_offset;
	int retval;

	/* Basic Initialization */
	memset(&Pkt, 0, sizeof(Pkt));
	PayLoad   = ((UINT8 *)&Pkt) + sizeof(NCSI_HDR);
	cs_offset = (UINT32)(sizeof(NCSI_HDR)) + (UINT32)7;

	Channel      = MK_CH_ID(PackageID, ChannelID);
	Pkt.ManufID  = cpu_to_be32(MELLANOX_IANA_ID);
	Pkt.OemCmdID = MELLANOX_OEM_CMD;
	Pkt.Parameter = MELLANOX_OEM_GET_MAC_ADDR_PARAMETER;
	
	/* Form NCSI Packet and send */
	FormNCSIOEMPacket(info, CMD_OEM_CMD, Channel, PayLoad, cs_offset);
	info->SendLen = sizeof(Pkt);
	retval = SendNCSICommand(info);
	if (retval != NCSI_ERR_SUCCESS) {
		printk("SendNCSICommand() failed: %d\n", retval);
		return retval;
	}
	/* Validate Response pkt */
	ResPkt = (OEM_GET_MELLANOX_MAC_ADDR_RES_PKT *)info->RecvData;
	if (info->RecvLen != sizeof(OEM_GET_MELLANOX_MAC_ADDR_RES_PKT)) {
		printk("NCSI(%s): Expected Response Size = %d Got %d\n", info->dev->name,
			   sizeof(OEM_GET_MELLANOX_MAC_ADDR_RES_PKT), info->RecvLen);
		return NCSI_ERR_RESPONSE;
	}

	memcpy(MacAddr,ResPkt->MAC_Addr,6);
	/* Check the Response code and reason */
	return ProcessResponseCode(info, ResPkt->ResponseCode, ResPkt->ReasonCode);
}

#endif

