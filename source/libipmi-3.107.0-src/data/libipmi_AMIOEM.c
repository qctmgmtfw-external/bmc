/*****************************************************************
******************************************************************
***                                                            ***
***        (C)Copyright 2008, American Megatrends Inc.         ***
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
* Filename: libipmi_AMIOEM.c
*
******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if ( _WIN64 || _WIN32 )
#include<ws2tcpip.h>
#define snprintf sprintf_s
#endif
#include "dbgout.h"
#include "libipmi_AppDevice.h"
#include "libipmi_AMIOEM.h"
#include "libipmi_session.h"
#include "libipmi_errorcodes.h"
#include "libipmi_struct.h"
#include "IPMI_AMIDevice.h"
#include "IPMI_AMISyslogConf.h"
#include "IPMI_AMIFirmwareUpdate.h"
#include "std_macros.h"
#include "IPMI_AMISmtp.h"
#include "AMIDevice.h"
#include "IPMI_AMIResetPass.h"
#include "IPMI_AMIConf.h"
#include "ncml.h"
#include "AMIRestoreDefaults.h"
#include "libipmi_StorDevice.h"
#include "SELRecord.h"
#include "SEL.h"
#include "IPMI_SEL.h"
#include "IPMI_HPM.h"
#include "IPMI_HPMCmds.h"
#include "IPMI_AMITimeZoneCmds.h"
#include "IPMI_AMIBiosCode.h"
#include "AMITimeZoneCmds.h"

#define MAX_FILEINFO_SIZE   128
#define HOSTNAME_LEN	64

#if LIBIPMI_IS_OS_LINUX()
#include <time.h>
#include <sys/types.h>
#include <pwd.h>
#include "pam_helper.h"
#include "unix.h"
#endif
#include "coreTypes.h"
#include "AMIFirmwareRecovery.h"
#include "PendTask.h"
#include "AMIServiceConf.h"

#if LIBIPMI_IS_OS_LINUX()
/**
 *@fn EnableAuditLogs
 *@brief verify login audits are enabled or not for specific recordtype
 *@param Service- (webgo/sshd/login).
 *@param Auditmask-EventMask
 *@param RecType- the record type
 *@return:on success reutrns 0
**/
static int EnableAuditLogs (char *Service, uint8 *Auditmask,int state, uint8 *RecType);
#endif

#define MSG_PAYLOAD_SIZE    (60 * 1024)

/***** Firewall structures *****/

#if 0
typedef struct
{
    int pos;
    INT8U state;
    INT8U IPAddr[IP_ADDR_LEN];

} PACKED IPRule;

typedef struct
{
    int pos;
    INT8U state;
    INT16U PortNo;

} PACKED PortRule;


typedef struct
{
    int pos;
    INT8U state;
    INT8U IPAddr1[IP_ADDR_LEN];
    INT8U IPAddr2[IP_ADDR_LEN];

} PACKED IPRangeRule;

typedef struct
{
    int pos;
    INT8U state;
    INT16U PortNo1;
    INT16U PortNo2;

} PACKED PortRangeRule;

typedef union
{
    IPRule IP;
    PortRule Port;
    IPRangeRule IPRange;
    PortRangeRule PortRange;

}FWRuleUn;

typedef struct
{
   int type;
   FWRuleUn Rule;

}FWT;
#endif
#define PRIV_NONE		0x00
#define PRIV_USER		0x02
#define PRIV_OPERATOR	0x03
#define PRIV_ADMIN		0x04
#define PRIV_OEM		0x05

#define GET_BIOS_TABLE 0x1
#define SET_BIOS_TABLE 0x2
#define TABLE_NOT_FOUND 0x83

static INT8U NetFn_OEM;

int g_accesslevel[5]={PRIV_ADMIN,PRIV_OPERATOR,PRIV_USER,PRIV_OEM,PRIV_NONE};

/***** Firewall structures *****/
#if LIBIPMI_IS_OS_LINUX()

typedef struct
{
    char *Srvce ;/*Telnet or ssh or webgo*/
    int Status[LOGIN_STATE_TYPE];/*Gives the service's login or logout record type*/
}AuditLogRec_T;

AuditLogRec_T AugitRecNo[AUTH_REQ_SERVICES]=
{
    {WEB_AUDIT_SERVICE_NAME, \
    {WEB_LOGIN_RECTYPE, WEB_LOGOUT_RECTYPE, WEB_AUTOLOGOUT_RECTYPE, WEB_LOGINFAILED_RECTYPE}},
    {TELNET_AUDIT_SERVCE_NAME, \
    {SSH_LOGIN_RECTYPE, SSH_LOGOUT_RECTYPE, SSH_AUTOLOGOUT_RECTYPE, SSH_LOGFAILED_RECTYPE}},
    {SSH_AUDIT_SERVCE_NAME, \
    {SSH_LOGIN_RECTYPE, SSH_LOGOUT_RECTYPE, SSH_AUTOLOGOUT_RECTYPE, SSH_LOGFAILED_RECTYPE}},
    {KVM_SERVICE_NAME, \
    {KVM_START_RCTYPE, KVM_STOP_RCTYPE, KVM_AUTOLOGOUT_RECTYPE, KVM_AUTHFAILED_RCTYPE}}
};
#endif


int VmediaAttachMode[5] = {VMEDIA_CD_ATTACH_MODE,
						VMEDIA_FD_ATTACH_MODE,
						VMEDIA_HD_ATTACH_MODE,
						VMEDIA_RESTART,
						-1}; //VMEDIA_REMOTE_ATTACH_MODE
int VmediaNumInstance[10] ={VMEDIA_ENABLE_BOOT_ONCE,
						VMEDIA_NUM_CD,
						VMEDIA_NUM_FD,
						VMEDIA_NUM_HD,
						VMEDIA_SDMEDIA_ENABLE_STATUS,
						KVM_NUM_CD,
						KVM_NUM_FD,
						KVM_NUM_HD,
						VMEDIA_RESTART,
						-1}; //VMEDIA_DEVICE_INSTANCE
int VmediaLMediaStatus[3] ={VMEDIA_LMEDIA_ENABLE_STATUS,
						VMEDIA_RESTART,
						-1};//VMEDIA_LMEDIA_ENABLE
int VmediaRMediaStatus[3] = {VMEDIA_RMEDIA_ENABLE_STATUS,
						VMEDIA_RESTART,
						-1};//VMEDIA_RMEDIA_ENABLE

uint16 IPMICMD_AMIGetFirewallRuleCount( IPMI20_SESSION_T *pSession, int    *TotalCount,
                                                                    int timeout )
{
    uint16 wRet;
    uint32 dwResLen;
    AMIGetFirewallReq_T GetFWConfReq;
    AMIGetFirewallRes_T GetFWConfRes;

    memset(&GetFWConfReq,0,sizeof(AMIGetFirewallReq_T));
    memset(&GetFWConfRes,0,sizeof(AMIGetFirewallRes_T));


    /*Fill the parameter selector */
    GetFWConfReq.Param = GET_IPTABLE_COUNT;
    dwResLen = 2;

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
    										( ( NETFN_AMI << 2 ) + 0x00 ), CMD_AMI_GET_FIREWALL,
    										(uint8 *) &GetFWConfReq,
    										sizeof(AMIGetFirewallReq_T)-1,
    										(uint8 *) &GetFWConfRes, &dwResLen, timeout );

    memcpy(TotalCount, &GetFWConfRes.FWInfo.TotalCount, 1);

    return( wRet );
}

uint16 IPMICMD_AMIGet_IsBlockAll( IPMI20_SESSION_T *pSession, INT8U    *block,
                                                                    int timeout )
{
    uint16 wRet;
    uint32 dwResLen;
    AMIGetFirewallReq_T GetFWConfReq;
    AMIGetFirewallRes_T GetFWConfRes;

    memset(&GetFWConfReq,0,sizeof(AMIGetFirewallReq_T));
    memset(&GetFWConfRes,0,sizeof(AMIGetFirewallRes_T));


    /*Fill the parameter selector */
    GetFWConfReq.Param = IS_BLOCK_ALL;
    //dwResLen = 2;

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
    										( ( NETFN_AMI << 2 ) + 0x00 ), CMD_AMI_GET_FIREWALL,
    										(uint8 *) &GetFWConfReq,
    										sizeof(AMIGetFirewallReq_T)-1,
    										(uint8 *) &GetFWConfRes, &dwResLen, timeout );

    *block = GetFWConfRes.FWInfo.IsBlockAll;
    TDBG("\n block - %d \n GetFWConfRes.FWInfo.IsBlockAll - %d \n", *block, GetFWConfRes.FWInfo.IsBlockAll);

    return( wRet );
}


uint16 IPMICMD_AMIGetFirewallRule( IPMI20_SESSION_T *pSession,
								                        int    *pos,
								                        AMIGetFirewallReq_T *GetFWConfReq,
								                        AMIGetFirewallRes_T *GetFWConfRes,
                                                                                int timeout )
{
    uint16 wRet;
    uint32 dwResLen;
    int count=0;

    /* Get the Total Count value */
    wRet = IPMICMD_AMIGetFirewallRuleCount( pSession, &count, timeout);
    if(wRet != 0)
    {
    	TCRIT("Error getting number of Firewall rules\n");
    	return wRet;
    }

    /* Get Firewall Rule now */
    memset(GetFWConfReq,0,sizeof(AMIGetFirewallReq_T));
    memset(GetFWConfRes,0,sizeof(AMIGetFirewallRes_T));

    /*Fill the parameter selector */
    GetFWConfReq->Param = GET_IPV4_ENTRY_INFO;
    GetFWConfReq->EntryNo = *pos;
    dwResLen = sizeof(AMIGetFirewallRes_T);

        wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
        										( ( NETFN_AMI << 2 ) + 0x00 ), CMD_AMI_GET_FIREWALL,
        										(uint8 *) GetFWConfReq, sizeof(AMIGetFirewallReq_T),
        										(uint8 *) GetFWConfRes, &dwResLen, timeout );

        TDBG("\n GetFWConfRes.FWInfo.Info.TYPE - %d \n GetFWConfRes.FWInfo.Info.State - %d \n",
        GetFWConfRes->FWInfo.Info.TYPE, GetFWConfRes->FWInfo.Info.State);
        //Rules->type = GetFWConfRes.FWInfo.Info.TYPE;

#if 0
        if( GetFWConfRes.FWInfo.Info.TYPE == IP_IPV4 )
        {
            Rules->Rule.IP.pos = *pos;
            Rules->Rule.IP.state = GetFWConfRes.FWInfo.Info.State;
            memcpy(Rules->Rule.IP.IPAddr,GetFWConfRes.FWInfo.Info.Entry_info.IPAddr_ipv4,4);

            for(j=0;j<4;j++)
            	TDBG(" IP[%d] - %d", j,Rules->Rule.IP.IPAddr[j]);
            TDBG("\n");
        }

        else if( GetFWConfRes.FWInfo.Info.TYPE == PORT )
        {
            Rules->Rule.Port.pos = *pos;
            Rules->Rule.Port.state = GetFWConfRes.FWInfo.Info.State;
            memcpy(&Rules->Rule.Port.PortNo,&GetFWConfRes.FWInfo.Info.Entry_info.Port_NO,2);

            TDBG(" Port.PortNo - %d \n",Rules->Rule.Port.PortNo);
        }

        else if( GetFWConfRes.FWInfo.Info.TYPE == IPS_IPV4 )
        {
            Rules->Rule.IPRange.pos = *pos;
            Rules->Rule.IPRange.state = GetFWConfRes.FWInfo.Info.State;
            memcpy(Rules->Rule.IPRange.IPAddr1,GetFWConfRes.FWInfo.Info.Entry_info.IPRange_ipv4.starting_ip,4);
            for(j=0;j<4;j++)
            	TDBG(" IP[%d] - %d", j,Rules->Rule.IPRange.IPAddr1[j]);
            TDBG("\n");

            memcpy(Rules->Rule.IPRange.IPAddr2,GetFWConfRes.FWInfo.Info.Entry_info.IPRange_ipv4.closing_ip,4);
            for(j=0;j<4;j++)
            	TDBG(" IP[%d] - %d", j, Rules->Rule.IPRange.IPAddr2[j]);
            TDBG("\n");

        }

        else if( GetFWConfRes.FWInfo.Info.TYPE == PORT_RANGE)
        {
            Rules->Rule.PortRange.pos = *pos;
            Rules->Rule.PortRange.state = GetFWConfRes.FWInfo.Info.State;

            memcpy(&Rules->Rule.PortRange.PortNo1,&GetFWConfRes.FWInfo.Info.Entry_info.Port_Range.starting_port,2);
            TDBG(" Port.PortNo - %d \n",Rules->Rule.PortRange.PortNo1);

            memcpy(&Rules->Rule.PortRange.PortNo2,&GetFWConfRes.FWInfo.Info.Entry_info.Port_Range.closing_port,2);
            TDBG(" Port.PortNo - %d \n",Rules->Rule.PortRange.PortNo2);
        }
#endif

    return( wRet );
}


uint16 IPMICMD_AMIGet_BlockAll_Timeout( IPMI20_SESSION_T *pSession,int *Rulecount, GetTimeout_T *fwtimeout,int timeout )
{
    uint16 wRet;
    AMIGetFirewallReq_T GetFWConfReq;
    AMIGetFirewallRes_T GetFWConfRes;
    int i=0,count=0;
    memset(&GetFWConfReq,0,sizeof(AMIGetFirewallReq_T));
    memset(&GetFWConfRes,0,sizeof(AMIGetFirewallRes_T));


    /* Get the Total Count value */
    wRet = IPMICMD_AMIGetFirewallRuleCount( pSession, &count, timeout);
    if(wRet != 0)
    {
        TCRIT("Error getting number of Firewall rules\n");
        return wRet;
    }
    for(i=0;i<count;i++)
    {
		memset(&GetFWConfRes, 0, sizeof(AMIGetFirewallRes_T));
        wRet = IPMICMD_AMIGetFirewallRule(pSession, &i, &GetFWConfReq, &GetFWConfRes, timeout );
        //Rules++;

        if( GetFWConfRes.FWInfo.Info.TYPE == DISABLE_ALL_TIMEOUT  )
        {
            GetFWConfRes.FWInfo.Info.Entry_info.Block.block=0;//memset
//            memset(&GetFWConfRes.FWInfo.Info.Entry_info.Block.block,0,3);//All is 0x3
            memcpy(fwtimeout,&GetFWConfRes.FWInfo.Info.Entry_info.Block.timeout,12);
            fwtimeout++;
            (*Rulecount)++;
        }
}

    return( wRet );
}

uint16 IPMICMD_AMIGet_IPV6_BlockAll_Timeout( IPMI20_SESSION_T *pSession,int *Rulecount, GetTimeout_T *fwtimeout,int timeout )
{
    uint16 wRet;
    AMIGetFirewallReq_T GetFWConfReq;
    AMIGetFirewallRes_T GetFWConfRes;
    int i=0,count=0;
    memset(&GetFWConfReq,0,sizeof(AMIGetFirewallReq_T));
    memset(&GetFWConfRes,0,sizeof(AMIGetFirewallRes_T));


    /* Get the Total Count value */
    wRet = IPMICMD_AMIGetFirewall_IPv6_RuleCount( pSession, &count, timeout);
    if(wRet != 0)
    {
        TCRIT("Error getting number of Firewall rules\n");
        return wRet;
    }
    for(i=0;i<count;i++)
    {
		memset(&GetFWConfRes, 0, sizeof(AMIGetFirewallRes_T));
        wRet = IPMICMD_AMIGetFirewall_IPv6_Rule(pSession, &i, &GetFWConfReq, &GetFWConfRes, timeout );
        //Rules++;

        if( GetFWConfRes.FWInfo.Info.TYPE == DISABLE_ALL_TIMEOUT  )
        {
            GetFWConfRes.FWInfo.Info.Entry_info.Block.block = 0;//memset
//            memset(&GetFWConfRes.FWInfo.Info.Entry_info.Block.block,0,3);//All is 0x3
            memcpy(fwtimeout,&GetFWConfRes.FWInfo.Info.Entry_info.Block.timeout,12);
            fwtimeout++;
            (*Rulecount)++;
        }
}

    return( wRet );
}



uint16 IPMICMD_AMIGetAllFirewallRule( IPMI20_SESSION_T *pSession, AllRules *Rules,
                                                            int timeout )
{
    uint16 wRet;
    AMIGetFirewallReq_T GetFWConfReq;
    AMIGetFirewallRes_T GetFWConfRes;

    int i=0,count=0;

    /* Get the Total Count value */
    wRet = IPMICMD_AMIGetFirewallRuleCount( pSession, &count, timeout);
    if(wRet != 0)
    {
    	TCRIT("Error getting number of Firewall rules\n");
    	return wRet;
    }
    /* Get firewall Rule Count is done... */

    for(i=0;i<count;i++)
    {
        wRet = IPMICMD_AMIGetFirewallRule(pSession, &i, &GetFWConfReq, &GetFWConfRes, timeout );
        if(wRet != 0)
        {
            TCRIT("Error getting Firewall rule : %d\n",i);
            return wRet;
        }
        //Rules++;
        if( GetFWConfRes.FWInfo.Info.TYPE == IP_IPV4 )
        {
            Rules->IPRules.type = GetFWConfRes.FWInfo.Info.TYPE ;
            Rules->IPRules.pos = i;
            Rules->IPRules.state = GetFWConfRes.FWInfo.Info.State;
            memcpy(&Rules->IPRules.IPAddr1, &GetFWConfRes.FWInfo.Info.Entry_info.IPAddr_ipv4, 4);
            memset(&Rules->IPRules.IPAddr2, 0, 4);
            Rules++;
        }
        else if( GetFWConfRes.FWInfo.Info.TYPE == IPS_IPV4 )
        {
            Rules->IPRules.type = GetFWConfRes.FWInfo.Info.TYPE;
            Rules->IPRules.pos = i;
            Rules->IPRules.state = GetFWConfRes.FWInfo.Info.State;
            memcpy(&Rules->IPRules.IPAddr1, &GetFWConfRes.FWInfo.Info.Entry_info.IPRange_ipv4.starting_ip, 4);
            memcpy(&Rules->IPRules.IPAddr2, &GetFWConfRes.FWInfo.Info.Entry_info.IPRange_ipv4.closing_ip, 4);
            Rules++;
        }
        else if( GetFWConfRes.FWInfo.Info.TYPE == PORT_IPV4 )
        {
            Rules->PortRules.type = GetFWConfRes.FWInfo.Info.TYPE;
            Rules->PortRules.pos = i;
            Rules->PortRules.state = GetFWConfRes.FWInfo.Info.State;
            memcpy(&Rules->PortRules.port1,
            &GetFWConfRes.FWInfo.Info.Entry_info.Port_Data.Port_NO, 2);
            memset(&Rules->PortRules.port2, 0, 2);
            Rules++;
        }
        else if( GetFWConfRes.FWInfo.Info.TYPE == PORT_RANGE_IPV4)
        {
            Rules->PortRules.type = GetFWConfRes.FWInfo.Info.TYPE;
            Rules->PortRules.pos = i;
            Rules->PortRules.state = GetFWConfRes.FWInfo.Info.State;
            memcpy(&Rules->PortRules.port1, &GetFWConfRes.FWInfo.Info.Entry_info.Port_Range.starting_port, 2);
            memcpy(&Rules->PortRules.port2, &GetFWConfRes.FWInfo.Info.Entry_info.Port_Range.closing_port, 2);
            Rules++;
        }
        else if(GetFWConfRes.FWInfo.Info.TYPE == IP_IPV4_TIMEOUT )
        {
            Rules->IPRules.type = GetFWConfRes.FWInfo.Info.TYPE ;
            Rules->IPRules.pos = i;
            Rules->IPRules.state = GetFWConfRes.FWInfo.Info.State;
            memcpy(&Rules->IPRules.IPAddr1, &GetFWConfRes.FWInfo.Info.Entry_info.IPAddr_ipv4.ipaddr_ipv4, 4);
            memset(&Rules->IPRules.IPAddr2, 0, 4);
            memcpy(&Rules->IPRules.timeout,&GetFWConfRes.FWInfo.Info.Entry_info.IPAddr_ipv4.timeout,12);
            Rules++;
        }

        else if(GetFWConfRes.FWInfo.Info.TYPE == IPS_IPV4_TIMEOUT )
        {
            Rules->IPRules.type = GetFWConfRes.FWInfo.Info.TYPE;
            Rules->IPRules.pos = i;
            Rules->IPRules.state = GetFWConfRes.FWInfo.Info.State;
            memcpy(&Rules->IPRules.IPAddr1, &GetFWConfRes.FWInfo.Info.Entry_info.IPRange_ipv4.starting_ip, 4);
            memcpy(&Rules->IPRules.IPAddr2, &GetFWConfRes.FWInfo.Info.Entry_info.IPRange_ipv4.closing_ip, 4);
            memcpy(&Rules->IPRules.timeout,&GetFWConfRes.FWInfo.Info.Entry_info.IPRange_ipv4.timeout,12);
            Rules++;
        }
        else if(GetFWConfRes.FWInfo.Info.TYPE == PORT_IPV4_TIMEOUT )
        {
            Rules->PortRules.type = GetFWConfRes.FWInfo.Info.TYPE;
            Rules->PortRules.pos = i;
            Rules->PortRules.state = GetFWConfRes.FWInfo.Info.State;
            memcpy(&Rules->PortRules.port1,&GetFWConfRes.FWInfo.Info.Entry_info.Port_Data.Port_NO, 2);
            memset(&Rules->PortRules.port2, 0, 2);
            memcpy(&Rules->PortRules.timeout,&GetFWConfRes.FWInfo.Info.Entry_info.Port_Data.timeout,12);
            Rules++;
        }
        else if(GetFWConfRes.FWInfo.Info.TYPE == PORT_RANGE_IPV4_TIMEOUT )
        {
            Rules->PortRules.type = GetFWConfRes.FWInfo.Info.TYPE;
            Rules->PortRules.pos = i;
            Rules->PortRules.state = GetFWConfRes.FWInfo.Info.State;
            memcpy(&Rules->PortRules.port1, &GetFWConfRes.FWInfo.Info.Entry_info.Port_Range.starting_port, 2);
            memcpy(&Rules->PortRules.port2, &GetFWConfRes.FWInfo.Info.Entry_info.Port_Range.closing_port, 2);
            memcpy(&Rules->PortRules.timeout,&GetFWConfRes.FWInfo.Info.Entry_info.Port_Range.timeout,12);
            Rules++;
        }
    }

    count=0;
    /* Get the Total Count value */
    wRet = IPMICMD_AMIGetFirewall_IPv6_RuleCount( pSession, &count, timeout);
    if(wRet != 0)
    {
    	TCRIT("Error getting number of Firewall rules\n");
    	return wRet;
    }
    /* Get firewall Rule Count is done... */

    for(i=0;i<count;i++)
    {
        wRet = IPMICMD_AMIGetFirewall_IPv6_Rule(pSession, &i, &GetFWConfReq, &GetFWConfRes, timeout );

        if( GetFWConfRes.FWInfo.Info.TYPE == IP_IPV6 )
        {
            Rules->IPv6Rules.type = GetFWConfRes.FWInfo.Info.TYPE ;
            Rules->IPv6Rules.pos = i;
            Rules->IPv6Rules.state = GetFWConfRes.FWInfo.Info.State;
            memcpy(&Rules->IPv6Rules.IPv6Addr1, &GetFWConfRes.FWInfo.Info.Entry_info.IPAddr_ipv6.ipaddr_ip, IP6_ADDR_LEN);
            memset(&Rules->IPv6Rules.IPv6Addr2, 0, IP6_ADDR_LEN);
            Rules++;
        }
        else if( GetFWConfRes.FWInfo.Info.TYPE == IPS_IPV6 )
        {
            Rules->IPv6Rules.type = GetFWConfRes.FWInfo.Info.TYPE;
            Rules->IPv6Rules.pos = i;
            Rules->IPv6Rules.state = GetFWConfRes.FWInfo.Info.State;
            memcpy(&Rules->IPv6Rules.IPv6Addr1, &GetFWConfRes.FWInfo.Info.Entry_info.IPRange_ipv6.starting_ip, IP6_ADDR_LEN);
            memcpy(&Rules->IPv6Rules.IPv6Addr2, &GetFWConfRes.FWInfo.Info.Entry_info.IPRange_ipv6.closing_ip, IP6_ADDR_LEN);
            Rules++;
        }
        else if( GetFWConfRes.FWInfo.Info.TYPE == PORT_IPV6 )
        {
            Rules->PortRules.type = GetFWConfRes.FWInfo.Info.TYPE;
            Rules->PortRules.pos = i;
            Rules->PortRules.state = GetFWConfRes.FWInfo.Info.State;
            memcpy(&Rules->PortRules.port1,
            &GetFWConfRes.FWInfo.Info.Entry_info.Port_Data.Port_NO, 2);
            memset(&Rules->PortRules.port2, 0, 2);
            Rules++;
        }
        else if( GetFWConfRes.FWInfo.Info.TYPE == PORT_RANGE_IPV6)
        {
            Rules->PortRules.type = GetFWConfRes.FWInfo.Info.TYPE;
            Rules->PortRules.pos = i;
            Rules->PortRules.state = GetFWConfRes.FWInfo.Info.State;
            memcpy(&Rules->PortRules.port1, &GetFWConfRes.FWInfo.Info.Entry_info.Port_Range.starting_port, 2);
            memcpy(&Rules->PortRules.port2, &GetFWConfRes.FWInfo.Info.Entry_info.Port_Range.closing_port, 2);
            Rules++;
        }
        else if(GetFWConfRes.FWInfo.Info.TYPE == IP_IPV6_TIMEOUT )
        {
            Rules->IPv6Rules.type = GetFWConfRes.FWInfo.Info.TYPE ;
            Rules->IPv6Rules.pos = i;
            Rules->IPv6Rules.state = GetFWConfRes.FWInfo.Info.State;
            memcpy(&Rules->IPv6Rules.IPv6Addr1, &GetFWConfRes.FWInfo.Info.Entry_info.IPAddr_ipv6.ipaddr_ip, IP6_ADDR_LEN);
            memset(&Rules->IPv6Rules.IPv6Addr2, 0, IP6_ADDR_LEN);
            memcpy(&Rules->IPv6Rules.timeout,&GetFWConfRes.FWInfo.Info.Entry_info.IPAddr_ipv6.timeout,12);
            Rules++;
        }

        else if(GetFWConfRes.FWInfo.Info.TYPE == IPS_IPV6_TIMEOUT )
        {
            Rules->IPv6Rules.type = GetFWConfRes.FWInfo.Info.TYPE;
            Rules->IPv6Rules.pos = i;
            Rules->IPv6Rules.state = GetFWConfRes.FWInfo.Info.State;
            memcpy(&Rules->IPv6Rules.IPv6Addr1, &GetFWConfRes.FWInfo.Info.Entry_info.IPRange_ipv6.starting_ip, IP6_ADDR_LEN);
            memcpy(&Rules->IPv6Rules.IPv6Addr2, &GetFWConfRes.FWInfo.Info.Entry_info.IPRange_ipv6.closing_ip, IP6_ADDR_LEN);
            memcpy(&Rules->IPv6Rules.timeout,&GetFWConfRes.FWInfo.Info.Entry_info.IPRange_ipv6.timeout, 12);
            Rules++;
        }
        else if(GetFWConfRes.FWInfo.Info.TYPE == PORT_IPV6_TIMEOUT )
        {
            Rules->PortRules.type = GetFWConfRes.FWInfo.Info.TYPE;
            Rules->PortRules.pos = i;
            Rules->PortRules.state = GetFWConfRes.FWInfo.Info.State;
            memcpy(&Rules->PortRules.port1,&GetFWConfRes.FWInfo.Info.Entry_info.Port_Data.Port_NO, 2);
            memset(&Rules->PortRules.port2, 0, 2);
            memcpy(&Rules->PortRules.timeout,&GetFWConfRes.FWInfo.Info.Entry_info.Port_Data.timeout,12);
            Rules++;
        }
        else if(GetFWConfRes.FWInfo.Info.TYPE == PORT_RANGE_IPV6_TIMEOUT )
        {
            Rules->PortRules.type = GetFWConfRes.FWInfo.Info.TYPE;
            Rules->PortRules.pos = i;
            Rules->PortRules.state = GetFWConfRes.FWInfo.Info.State;
            memcpy(&Rules->PortRules.port1, &GetFWConfRes.FWInfo.Info.Entry_info.Port_Range.starting_port, 2);
            memcpy(&Rules->PortRules.port2, &GetFWConfRes.FWInfo.Info.Entry_info.Port_Range.closing_port, 2);
            memcpy(&Rules->PortRules.timeout,&GetFWConfRes.FWInfo.Info.Entry_info.Port_Range.timeout,12);
            Rules++;
        }
    }

    return( wRet );
}

uint16 IPMICMD_AMIGetFirewall_IPv6_RuleCount( IPMI20_SESSION_T *pSession, int *TotalCount,
                                                                    int timeout )
{
    uint16 wRet;
    uint32 dwResLen;
    AMIGetFirewallReq_T GetFWConfReq;
    AMIGetFirewallRes_T GetFWConfRes;

    memset(&GetFWConfReq,0,sizeof(AMIGetFirewallReq_T));
    memset(&GetFWConfRes,0,sizeof(AMIGetFirewallRes_T));


    /*Fill the parameter selector */
    GetFWConfReq.Param = GET_IP6TABLE_COUNT;
   // dwResLen = 2;

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
    										( ( NETFN_AMI << 2 ) + 0x00 ), CMD_AMI_GET_FIREWALL,
    										(uint8 *) &GetFWConfReq,
    										sizeof(AMIGetFirewallReq_T)-1,
    										(uint8 *) &GetFWConfRes, &dwResLen, timeout );

    memcpy(TotalCount, &GetFWConfRes.FWInfo.TotalCount, 1);

    return( wRet );
}

uint16 IPMICMD_AMIGetFirewall_IPv6_Rule( IPMI20_SESSION_T *pSession,
								                        int    *pos,
								                        AMIGetFirewallReq_T *GetFWConfReq,
								                        AMIGetFirewallRes_T *GetFWConfRes,
                                                                                int timeout )
{
    uint16 wRet;
    uint32 dwResLen;
    int count=0;

    /* Get the Total Count value */
    wRet = IPMICMD_AMIGetFirewall_IPv6_RuleCount( pSession, &count, timeout);
    if(wRet != 0)
    {
    	TCRIT("Error getting number of Firewall rules\n");
    	return wRet;
    }

    /* Get Firewall Rule now */
    memset(GetFWConfReq,0,sizeof(AMIGetFirewallReq_T));
    memset(GetFWConfRes,0,sizeof(AMIGetFirewallRes_T));

    /*Fill the parameter selector */
    GetFWConfReq->Param = GET_IPV6_ENTRY_INFO;
    GetFWConfReq->EntryNo = *pos;
    dwResLen = sizeof(AMIGetFirewallRes_T);

        wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
        										( ( NETFN_AMI << 2 ) + 0x00 ), CMD_AMI_GET_FIREWALL,
        										(uint8 *) GetFWConfReq, sizeof(AMIGetFirewallReq_T),
        										(uint8 *) GetFWConfRes, &dwResLen, timeout );

        TDBG("\n GetFWConfRes.FWInfo.Info.TYPE - %d \n GetFWConfRes.FWInfo.Info.State - %d \n",
        GetFWConfRes->FWInfo.Info.TYPE, GetFWConfRes->FWInfo.Info.State);

    return( wRet );
}


/****************/
uint16 LIBIPMI_HL_AMIGetFirewall_IPRule( IPMI20_SESSION_T *pSession, int *IPCount,
								                        GetIPRule *IPRules,
                                                                                int timeout )
{
    uint16 wRet;
    AMIGetFirewallReq_T GetFWConfReq;
    AMIGetFirewallRes_T GetFWConfRes;

    int i=0,count=0;

    /* Get the Total Count value */
    wRet = IPMICMD_AMIGetFirewallRuleCount( pSession, &count, timeout);
    if(wRet != 0)
    {
    	TCRIT("Error getting number of Firewall rules\n");
    	return wRet;
    }
    /* Get firewall Rule Count is done... */


    for(i=0;i<count;i++)
    {
    	memset(&GetFWConfRes, 0, sizeof(AMIGetFirewallRes_T));
        wRet = IPMICMD_AMIGetFirewallRule(pSession, &i, &GetFWConfReq, &GetFWConfRes, timeout );

        if( GetFWConfRes.FWInfo.Info.TYPE == IP_IPV4 )
        {
            IPRules->type = GetFWConfRes.FWInfo.Info.TYPE ;
            IPRules->pos = i;
            IPRules->state = GetFWConfRes.FWInfo.Info.State;
            memcpy(&IPRules->IPAddr1, &GetFWConfRes.FWInfo.Info.Entry_info.IPAddr_ipv4, 4);
            memset(&IPRules->IPAddr2, 0, 4);
            //do memset all the data of timeout
             memset(&IPRules->timeout, 0, 12);
            IPRules++;
            (*IPCount)++;
        }
        else if( GetFWConfRes.FWInfo.Info.TYPE == IPS_IPV4 )
        {
            IPRules->type = GetFWConfRes.FWInfo.Info.TYPE;
            IPRules->pos = i;
            IPRules->state = GetFWConfRes.FWInfo.Info.State;
            memcpy(&IPRules->IPAddr1, &GetFWConfRes.FWInfo.Info.Entry_info.IPRange_ipv4.starting_ip, 4);
            memcpy(&IPRules->IPAddr2, &GetFWConfRes.FWInfo.Info.Entry_info.IPRange_ipv4.closing_ip, 4);
            //do memset all the data of timeout
            memset(&IPRules->timeout, 0, 12);

            IPRules++;
            (*IPCount)++;
        }
        else if(GetFWConfRes.FWInfo.Info.TYPE == IP_IPV4_TIMEOUT )
        {
            IPRules->type = GetFWConfRes.FWInfo.Info.TYPE ;
            IPRules->pos = i;
            IPRules->state = GetFWConfRes.FWInfo.Info.State;
            memcpy(&IPRules->IPAddr1, &GetFWConfRes.FWInfo.Info.Entry_info.IPAddr_ipv4.ipaddr_ipv4, 4);
            memset(&IPRules->IPAddr2, 0, 4);
            memcpy(&IPRules->timeout,&GetFWConfRes.FWInfo.Info.Entry_info.IPAddr_ipv4.timeout,12);
            IPRules++;
            (*IPCount)++;
        }

        else if(GetFWConfRes.FWInfo.Info.TYPE == IPS_IPV4_TIMEOUT )
        {
            IPRules->type = GetFWConfRes.FWInfo.Info.TYPE;
            IPRules->pos = i;
            IPRules->state = GetFWConfRes.FWInfo.Info.State;
            memcpy(&IPRules->IPAddr1, &GetFWConfRes.FWInfo.Info.Entry_info.IPRange_ipv4.starting_ip, 4);
            memcpy(&IPRules->IPAddr2, &GetFWConfRes.FWInfo.Info.Entry_info.IPRange_ipv4.closing_ip, 4);
            memcpy(&IPRules->timeout,&GetFWConfRes.FWInfo.Info.Entry_info.IPRange_ipv4.timeout,12);
            IPRules++;
            (*IPCount)++;
        }

    }
    return( wRet );
}

uint16 LIBIPMI_HL_AMIGetFirewall_PortRule( IPMI20_SESSION_T *pSession, int *PortCount,
								                        GetPortRule *PortRules,
                                                                                int timeout )
{
    uint16 wRet;
//    FWT Rules;
    AMIGetFirewallReq_T GetFWConfReq;
    AMIGetFirewallRes_T GetFWConfRes;

    int i=0,count=0;

    /* Get the Total Count value */
    wRet = IPMICMD_AMIGetFirewallRuleCount( pSession, &count, timeout);
    if(wRet != 0)
    {
    	TCRIT("Error getting number of Firewall rules\n");
    	return wRet;
    }
    /* Get firewall Rule Count is done... */

    for(i=0;i<count;i++)
    {
    	memset(&GetFWConfRes, 0, sizeof(AMIGetFirewallRes_T));
        wRet = IPMICMD_AMIGetFirewallRule (pSession, &i, &GetFWConfReq, &GetFWConfRes, timeout );

        if( GetFWConfRes.FWInfo.Info.TYPE == PORT_IPV4)
        {
            PortRules->type = GetFWConfRes.FWInfo.Info.TYPE;
            PortRules->pos = i;
            PortRules->state = GetFWConfRes.FWInfo.Info.State;
            PortRules->protocol = GetFWConfRes.FWInfo.Info.Entry_info.Port_Data.protocol;
            memcpy(&PortRules->port1, &GetFWConfRes.FWInfo.Info.Entry_info.Port_Data.Port_NO, 2);
            memset(&PortRules->port2, 0, 2);
            PortRules++;
            (*PortCount)++;
        }
        else if( GetFWConfRes.FWInfo.Info.TYPE == PORT_RANGE_IPV4)
        {
            PortRules->type = GetFWConfRes.FWInfo.Info.TYPE;
            PortRules->pos = i;
            PortRules->state = GetFWConfRes.FWInfo.Info.State;
            PortRules->protocol = GetFWConfRes.FWInfo.Info.Entry_info.Port_Data.protocol;
            memcpy(&PortRules->port1, &GetFWConfRes.FWInfo.Info.Entry_info.Port_Range.starting_port, 2);
            memcpy(&PortRules->port2, &GetFWConfRes.FWInfo.Info.Entry_info.Port_Range.closing_port, 2);
            PortRules++;
            (*PortCount)++;
        }
        else if(GetFWConfRes.FWInfo.Info.TYPE == PORT_IPV4_TIMEOUT )
        {
            PortRules->type = GetFWConfRes.FWInfo.Info.TYPE;
            PortRules->pos = i;
            PortRules->state = GetFWConfRes.FWInfo.Info.State;
            PortRules->protocol = GetFWConfRes.FWInfo.Info.Entry_info.Port_Data.protocol;
            memcpy(&PortRules->port1,&GetFWConfRes.FWInfo.Info.Entry_info.Port_Data.Port_NO, 2);
            memset(&PortRules->port2, 0, 2);
            memcpy(&PortRules->timeout,&GetFWConfRes.FWInfo.Info.Entry_info.Port_Data.timeout,12);
            PortRules++;
            (*PortCount)++;
        }
        else if(GetFWConfRes.FWInfo.Info.TYPE == PORT_RANGE_IPV4_TIMEOUT )
        {
            PortRules->type = GetFWConfRes.FWInfo.Info.TYPE;
            PortRules->pos = i;
            PortRules->state = GetFWConfRes.FWInfo.Info.State;
            PortRules->protocol = GetFWConfRes.FWInfo.Info.Entry_info.Port_Data.protocol;
            memcpy(&PortRules->port1, &GetFWConfRes.FWInfo.Info.Entry_info.Port_Range.starting_port, 2);
            memcpy(&PortRules->port2, &GetFWConfRes.FWInfo.Info.Entry_info.Port_Range.closing_port, 2);
            memcpy(&PortRules->timeout,&GetFWConfRes.FWInfo.Info.Entry_info.Port_Range.timeout,12);
            PortRules++;
            (*PortCount)++;
        }
    }
    return( wRet );
}

uint16 LIBIPMI_HL_AMIGetFirewall_IPv6_Rule( IPMI20_SESSION_T *pSession, int *IPCount,
                                                            GetIPv6Rule *IPv6Rules, int timeout )
{
    uint16 wRet;
    AMIGetFirewallReq_T GetFWConfReq;
    AMIGetFirewallRes_T GetFWConfRes;

    int i=0,count=0;

    /* Get the Total Count value */
    wRet = IPMICMD_AMIGetFirewall_IPv6_RuleCount( pSession, &count, timeout);
    if(wRet != 0)
    {
        TCRIT("Error getting number of Firewall rules\n");
        return wRet;
    }
    /* Get firewall Rule Count is done... */

    for(i=0;i<count;i++)
    {
        memset(&GetFWConfRes, 0, sizeof(AMIGetFirewallRes_T));
        wRet = IPMICMD_AMIGetFirewall_IPv6_Rule(pSession, &i, &GetFWConfReq, &GetFWConfRes, timeout );

        if( GetFWConfRes.FWInfo.Info.TYPE == IP_IPV6 )
        {
            IPv6Rules->type = GetFWConfRes.FWInfo.Info.TYPE ;
            IPv6Rules->pos = i;
            IPv6Rules->state = GetFWConfRes.FWInfo.Info.State;
            memcpy(&IPv6Rules->IPv6Addr1, &GetFWConfRes.FWInfo.Info.Entry_info.IPAddr_ipv6.ipaddr_ip, IP6_ADDR_LEN);
            memset(&IPv6Rules->IPv6Addr2, 0, IP6_ADDR_LEN);
            //do memset all the data of timeout
            memset(&IPv6Rules->timeout, 0, 12);
            IPv6Rules++;
            (*IPCount)++;
        }
        else if( GetFWConfRes.FWInfo.Info.TYPE == IPS_IPV6 )
        {
            IPv6Rules->type = GetFWConfRes.FWInfo.Info.TYPE;
            IPv6Rules->pos = i;
            IPv6Rules->state = GetFWConfRes.FWInfo.Info.State;
            memcpy(&IPv6Rules->IPv6Addr1, &GetFWConfRes.FWInfo.Info.Entry_info.IPRange_ipv6.starting_ip, IP6_ADDR_LEN);
            memcpy(&IPv6Rules->IPv6Addr2, &GetFWConfRes.FWInfo.Info.Entry_info.IPRange_ipv6.closing_ip, IP6_ADDR_LEN);
            //do memset all the data of timeout
            memset(&IPv6Rules->timeout, 0, 12);

            IPv6Rules++;
            (*IPCount)++;
        }
        else if(GetFWConfRes.FWInfo.Info.TYPE == IP_IPV6_TIMEOUT )
        {
            IPv6Rules->type = GetFWConfRes.FWInfo.Info.TYPE ;
            IPv6Rules->pos = i;
            IPv6Rules->state = GetFWConfRes.FWInfo.Info.State;
            memcpy(&IPv6Rules->IPv6Addr1, &GetFWConfRes.FWInfo.Info.Entry_info.IPAddr_ipv6.ipaddr_ip, IP6_ADDR_LEN);
            memset(&IPv6Rules->IPv6Addr2, 0, IP6_ADDR_LEN);
            memcpy(&IPv6Rules->timeout,&GetFWConfRes.FWInfo.Info.Entry_info.IPAddr_ipv6.timeout, 12);
            IPv6Rules++;
            (*IPCount)++;
        }

        else if(GetFWConfRes.FWInfo.Info.TYPE == IPS_IPV6_TIMEOUT )
        {
            IPv6Rules->type = GetFWConfRes.FWInfo.Info.TYPE;
            IPv6Rules->pos = i;
            IPv6Rules->state = GetFWConfRes.FWInfo.Info.State;
            memcpy(&IPv6Rules->IPv6Addr1, &GetFWConfRes.FWInfo.Info.Entry_info.IPRange_ipv6.starting_ip, IP6_ADDR_LEN);
            memcpy(&IPv6Rules->IPv6Addr2, &GetFWConfRes.FWInfo.Info.Entry_info.IPRange_ipv6.closing_ip, IP6_ADDR_LEN);
            memcpy(&IPv6Rules->timeout,&GetFWConfRes.FWInfo.Info.Entry_info.IPRange_ipv6.timeout, 12);
            IPv6Rules++;
            (*IPCount)++;
        }

    }
    return( wRet );
}

uint16 LIBIPMI_HL_AMIGetFirewall_IPv6_PortRule( IPMI20_SESSION_T *pSession, int *PortCount,
                                                            GetPortRule *PortRules, int timeout )
{
    uint16 wRet;
//    FWT Rules;
    AMIGetFirewallReq_T GetFWConfReq;
    AMIGetFirewallRes_T GetFWConfRes;

    int i=0,count=0;

    /* Get the Total Count value */
    wRet = IPMICMD_AMIGetFirewall_IPv6_RuleCount( pSession, &count, timeout);
    if(wRet != 0)
    {
    	TCRIT("Error getting number of Firewall rules\n");
    	return wRet;
    }
    /* Get firewall Rule Count is done... */

    for(i=0;i<count;i++)
    {
    	memset(&GetFWConfRes, 0, sizeof(AMIGetFirewallRes_T));
        wRet = IPMICMD_AMIGetFirewall_IPv6_Rule (pSession, &i, &GetFWConfReq, &GetFWConfRes, timeout );

        if( GetFWConfRes.FWInfo.Info.TYPE == PORT_IPV6)
        {
            PortRules->type = GetFWConfRes.FWInfo.Info.TYPE;
            PortRules->pos = i;
            PortRules->state = GetFWConfRes.FWInfo.Info.State;
            PortRules->protocol = GetFWConfRes.FWInfo.Info.Entry_info.Port_Data.protocol;
            memcpy(&PortRules->port1, &GetFWConfRes.FWInfo.Info.Entry_info.Port_Data.Port_NO, 2);
            memset(&PortRules->port2, 0, 2);
            PortRules++;
            (*PortCount)++;
        }
        else if( GetFWConfRes.FWInfo.Info.TYPE == PORT_RANGE_IPV6)
        {
            PortRules->type = GetFWConfRes.FWInfo.Info.TYPE;
            PortRules->pos = i;
            PortRules->state = GetFWConfRes.FWInfo.Info.State;
            PortRules->protocol = GetFWConfRes.FWInfo.Info.Entry_info.Port_Data.protocol;
            memcpy(&PortRules->port1, &GetFWConfRes.FWInfo.Info.Entry_info.Port_Range.starting_port, 2);
            memcpy(&PortRules->port2, &GetFWConfRes.FWInfo.Info.Entry_info.Port_Range.closing_port, 2);
            PortRules++;
            (*PortCount)++;
        }
        else if(GetFWConfRes.FWInfo.Info.TYPE == PORT_IPV6_TIMEOUT )
        {
            PortRules->type = GetFWConfRes.FWInfo.Info.TYPE;
            PortRules->pos = i;
            PortRules->state = GetFWConfRes.FWInfo.Info.State;
            PortRules->protocol = GetFWConfRes.FWInfo.Info.Entry_info.Port_Data.protocol;
            memcpy(&PortRules->port1,&GetFWConfRes.FWInfo.Info.Entry_info.Port_Data.Port_NO, 2);
            memset(&PortRules->port2, 0, 2);
            memcpy(&PortRules->timeout,&GetFWConfRes.FWInfo.Info.Entry_info.Port_Data.timeout,12);
            PortRules++;
            (*PortCount)++;
        }
        else if(GetFWConfRes.FWInfo.Info.TYPE == PORT_RANGE_IPV6_TIMEOUT )
        {
            PortRules->type = GetFWConfRes.FWInfo.Info.TYPE;
            PortRules->pos = i;
            PortRules->state = GetFWConfRes.FWInfo.Info.State;
            PortRules->protocol = GetFWConfRes.FWInfo.Info.Entry_info.Port_Data.protocol;
            memcpy(&PortRules->port1, &GetFWConfRes.FWInfo.Info.Entry_info.Port_Range.starting_port, 2);
            memcpy(&PortRules->port2, &GetFWConfRes.FWInfo.Info.Entry_info.Port_Range.closing_port, 2);
            memcpy(&PortRules->timeout,&GetFWConfRes.FWInfo.Info.Entry_info.Port_Range.timeout,12);
            PortRules++;
            (*PortCount)++;
        }
    }
    return( wRet );
}

#if 0
uint16 IPMICMD_AMIGetFirewallRule( IPMI20_SESSION_T *pSession,
								                        int    *TotalCount,
                                                                                int timeout )
{
    uint16 wRet;
    uint32 dwResLen;
    AMIGetFirewallReq_T GetFWConfReq;
    AMIGetFirewallRes_T GetFWConfRes;
    int i=0,count=0;

    IPRule IP;
    PortRule Port;
    IPRangeRule IPRange;
    PortRangeRule PortRange;
    int j=0;

    /* Get the Total Count value */

    memset(&GetFWConfReq,0,sizeof(AMIGetFirewallReq_T));
    memset(&GetFWConfRes,0,sizeof(AMIGetFirewallRes_T));

    /*Fill the parameter selector */
    GetFWConfReq.Param = GET_IPTABLE_COUNT;
    dwResLen = 2;

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
    										( ( NETFN_AMI << 2 ) + 0x00 ), CMD_AMI_GET_FIREWALL,
    										(uint8 *) &GetFWConfReq,
    										sizeof(AMIGetFirewallReq_T)-1,
    										(uint8 *) &GetFWConfRes, &dwResLen, timeout );

    memcpy(&count, &GetFWConfRes.FWInfo.TotalCount, 1);
    printf("\n TotalCount = %d \n", count);
    /* Get firewall Rule Count is done... */

    /* Get Firewall Rule now */
    memset(&GetFWConfReq,0,sizeof(AMIGetFirewallReq_T));
    memset(&GetFWConfRes,0,sizeof(AMIGetFirewallRes_T));

    /*Fill the parameter selector */
    GetFWConfReq.Param = GET_ENTRY_INFO;
    dwResLen = sizeof(AMIGetFirewallRes_T);

    for(i=0;i<count;i++)
    {
        GetFWConfReq.EntryNo = i;

        wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
        										( ( NETFN_AMI << 2 ) + 0x00 ), CMD_AMI_GET_FIREWALL,
        										(uint8 *) &GetFWConfReq, sizeof(AMIGetFirewallReq_T),
        										(uint8 *) &GetFWConfRes, &dwResLen, timeout );

        printf("\n GetFWConfRes.FWInfo.Info.TYPE - %d \n GetFWConfRes.FWInfo.Info.State - %d \n", GetFWConfRes.FWInfo.Info.TYPE, GetFWConfRes.FWInfo.Info.State);

        if( GetFWConfRes.FWInfo.Info.TYPE == IP_IPV4 )
        {
            IP.pos = i;
            IP.state = GetFWConfRes.FWInfo.Info.State;
            memcpy(IP.IPAddr,GetFWConfRes.FWInfo.Info.Entry_info.IPAddr_ipv4,4);

            for(j=0;j<4;j++)
                printf(" IP[%d] - %d", j, IP.IPAddr[j]);
            printf("\n");
        }

        else if( GetFWConfRes.FWInfo.Info.TYPE == PORT )
        {
            Port.pos = i;
            Port.state = GetFWConfRes.FWInfo.Info.State;
            memcpy(&Port.PortNo,&GetFWConfRes.FWInfo.Info.Entry_info.Port_NO,2);

            printf(" Port.PortNo - %d \n",Port.PortNo);
        }

        else if( GetFWConfRes.FWInfo.Info.TYPE == IPS_IPV4 )
        {
            IPRange.pos = i;
            IPRange.state = GetFWConfRes.FWInfo.Info.State;
            memcpy(IPRange.IPAddr1,GetFWConfRes.FWInfo.Info.Entry_info.IPRange_ipv4.starting_ip,4);
            for(j=0;j<4;j++)
                printf(" IP[%d] - %d", j, IPRange.IPAddr1[j]);
            printf("\n");

            memcpy(IPRange.IPAddr2,GetFWConfRes.FWInfo.Info.Entry_info.IPRange_ipv4.closing_ip,4);
            for(j=0;j<4;j++)
                printf(" IP[%d] - %d", j, IPRange.IPAddr2[j]);
            printf("\n");

        }

        else if( GetFWConfRes.FWInfo.Info.TYPE == PORT_RANGE)
        {
            PortRange.pos = i;
            PortRange.state = GetFWConfRes.FWInfo.Info.State;

            memcpy(&PortRange.PortNo1,&GetFWConfRes.FWInfo.Info.Entry_info.Port_Range.starting_port,2);
            printf(" Port.PortNo - %d \n",PortRange.PortNo1);

            memcpy(&PortRange.PortNo2,&GetFWConfRes.FWInfo.Info.Entry_info.Port_Range.closing_port,2);
            printf(" Port.PortNo - %d \n",PortRange.PortNo2);
        }

    }

    //memcpy(TotalCount,&GetFWConfRes.FWInfo.TotalCount,2);
    //printf("\n TotalCount = %d\n", *TotalCount);

    return( wRet );
}
#endif

/******** set command for Firewall ***************/

uint16 IPMICMD_AMISetAddFirewall_IPRule( IPMI20_SESSION_T *pSession, GetIPRule *SetRule,
                                                                    int timeout )
{
    uint16 wRet;
    uint32 dwResLen;
    uint32 dwReqLen = 0;
    AMISetFirewallReq_T SetFWConfReq;
    AMISetFirewallRes_T SetFWConfRes;

    memset(&SetFWConfReq,0,sizeof(AMISetFirewallReq_T));
    memset(&SetFWConfRes,0,sizeof(AMISetFirewallRes_T));

    /*Fill the parameter selector */

    if(SetRule->type == IP_IPV4 )
    {
            SetFWConfReq.Param = IP_IPV4;
            SetFWConfReq.State = SetRule->state;
            memcpy( &SetFWConfReq.CMD_INFO.IPAddr_ipv4.ipaddr_ipv4, SetRule->IPAddr1,4);
            dwReqLen = 4+2;
    }
    else if(SetRule->type == IPS_IPV4 )
    {
            SetFWConfReq.Param = IPS_IPV4;
            SetFWConfReq.State = SetRule->state;
            memcpy( &SetFWConfReq.CMD_INFO.IPRange_ipv4.starting_ip, SetRule->IPAddr1,4);
            memcpy( &SetFWConfReq.CMD_INFO.IPRange_ipv4.closing_ip, SetRule->IPAddr2,4);
            dwReqLen = 8+2;
    }

    else if(SetRule->type == IP_IPV4_TIMEOUT  )
    {
            SetFWConfReq.Param = IP_IPV4_TIMEOUT;
            SetFWConfReq.State = SetRule->state;
            memcpy( &SetFWConfReq.CMD_INFO.IPAddr_ipv4.ipaddr_ipv4, SetRule->IPAddr1,4);
            memcpy( &SetFWConfReq.CMD_INFO.IPAddr_ipv4.timeout, &SetRule->timeout,12);
            dwReqLen = 4+12+2;

    }
    else if(SetRule->type == IPS_IPV4_TIMEOUT  )
    {
            SetFWConfReq.Param = IPS_IPV4_TIMEOUT;
            SetFWConfReq.State = SetRule->state;
            memcpy( &SetFWConfReq.CMD_INFO.IPRange_ipv4.starting_ip, SetRule->IPAddr1,4);
            memcpy( &SetFWConfReq.CMD_INFO.IPRange_ipv4.closing_ip, SetRule->IPAddr2,4);

            memcpy( &SetFWConfReq.CMD_INFO.IPRange_ipv4.timeout, &SetRule->timeout,12);
            dwReqLen = 8+12+2;
    }

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                            ( ( NETFN_AMI << 2 ) + 0x00 ), CMD_AMI_SET_FIREWALL,
                                            (uint8 *) &SetFWConfReq, dwReqLen,
                                            (uint8 *) &SetFWConfRes, &dwResLen, timeout );

    return( wRet );
}

uint16 IPMICMD_AMISetAddFirewall_PortRule( IPMI20_SESSION_T *pSession, GetPortRule *SetRule,
                                                                    int timeout )
{
    uint16 wRet;
    uint32 dwResLen;
    uint32 dwReqLen = 0;
    AMISetFirewallReq_T SetFWConfReq;
    AMISetFirewallRes_T SetFWConfRes;

    memset(&SetFWConfReq,0,sizeof(AMISetFirewallReq_T));
    memset(&SetFWConfRes,0,sizeof(AMISetFirewallRes_T));


    /*Fill the parameter selector */

    if(SetRule->type == PORT_IPV4)
    {
            SetFWConfReq.Param = PORT_IPV4;
            SetFWConfReq.CMD_INFO.Port_Data.protocol = SetRule->protocol;
            SetFWConfReq.State = SetRule->state;
            memcpy( &SetFWConfReq.CMD_INFO.Port_Data.Port_NO, &SetRule->port1, 2);
            dwReqLen = 2+3;
    }
    else if(SetRule->type == PORT_RANGE_IPV4)
    {
            SetFWConfReq.Param = PORT_RANGE_IPV4;
            SetFWConfReq.CMD_INFO.Port_Data.protocol = SetRule->protocol;
            SetFWConfReq.State = SetRule->state;
            memcpy( &SetFWConfReq.CMD_INFO.Port_Range.starting_port, &SetRule->port1, 2);
            memcpy( &SetFWConfReq.CMD_INFO.Port_Range.closing_port, &SetRule->port2, 2);
            dwReqLen = 4+3;
    }
    else if(SetRule->type == PORT_IPV4_TIMEOUT)
    {
            SetFWConfReq.Param = PORT_IPV4_TIMEOUT;
            SetFWConfReq.CMD_INFO.Port_Data.protocol = SetRule->protocol;
            SetFWConfReq.State = SetRule->state;
            memcpy( &SetFWConfReq.CMD_INFO.Port_Data.Port_NO, &SetRule->port1, 2);
            memcpy( &SetFWConfReq.CMD_INFO.Port_Data.timeout, &SetRule->timeout,12);
            dwReqLen = 2+3+12;
    }
    else if(SetRule->type == PORT_RANGE_IPV4_TIMEOUT)
    {
            SetFWConfReq.Param = PORT_RANGE_IPV4_TIMEOUT;
            SetFWConfReq.CMD_INFO.Port_Data.protocol = SetRule->protocol;
            SetFWConfReq.State = SetRule->state;
            memcpy( &SetFWConfReq.CMD_INFO.Port_Range.starting_port, &SetRule->port1, 2);
            memcpy( &SetFWConfReq.CMD_INFO.Port_Range.closing_port, &SetRule->port2, 2);
            memcpy( &SetFWConfReq.CMD_INFO.Port_Range.timeout, &SetRule->timeout,12);
            dwReqLen = 4+3+12;
    }

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                            ( ( NETFN_AMI << 2 ) + 0x00 ), CMD_AMI_SET_FIREWALL,
                                            (uint8 *) &SetFWConfReq, dwReqLen,
                                            (uint8 *) &SetFWConfRes, &dwResLen, timeout );

    return( wRet );
}

uint16 IPMICMD_AMISetDelFirewall_IPRule( IPMI20_SESSION_T *pSession, GetIPRule *SetRule,
                                                                    int timeout )
{
    uint16 wRet;
    uint32 dwResLen;
    uint32 dwReqLen = 0;
    AMISetFirewallReq_T SetFWConfReq;
    AMISetFirewallRes_T SetFWConfRes;

    memset(&SetFWConfReq,0,sizeof(AMISetFirewallReq_T));
    memset(&SetFWConfRes,0,sizeof(AMISetFirewallRes_T));

    /*Fill the parameter selector */

    if(SetRule->type == IP_IPV4_UNLOCK)
    {
            SetFWConfReq.Param = IP_IPV4_UNLOCK;
            SetFWConfReq.State = SetRule->state;
            memcpy( &SetFWConfReq.CMD_INFO.IPAddr_ipv4.ipaddr_ipv4, SetRule->IPAddr1,4);
            dwReqLen = 4+2;
    }
    else if(SetRule->type == IPS_IPV4_UNLOCK)
    {
            SetFWConfReq.Param = IPS_IPV4_UNLOCK;
            SetFWConfReq.State = SetRule->state;
            memcpy( &SetFWConfReq.CMD_INFO.IPRange_ipv4.starting_ip, SetRule->IPAddr1,4);
            memcpy( &SetFWConfReq.CMD_INFO.IPRange_ipv4.closing_ip, SetRule->IPAddr2,4);
            dwReqLen = 8+2;
    }
    else if(SetRule->type == IP_IPV4_TIMEOUT_UNLOCK)
    {
            SetFWConfReq.Param = IP_IPV4_TIMEOUT_UNLOCK;
            SetFWConfReq.State = SetRule->state;
            memcpy( &SetFWConfReq.CMD_INFO.IPAddr_ipv4.ipaddr_ipv4, SetRule->IPAddr1,4);
            memcpy( &SetFWConfReq.CMD_INFO.IPAddr_ipv4.timeout, &SetRule->timeout,12);
            dwReqLen = 4+2+12;
    }
    else if(SetRule->type == IPS_IPV4_TIMEOUT_UNLOCK)
    {
            SetFWConfReq.Param = IPS_IPV4_TIMEOUT_UNLOCK;
            SetFWConfReq.State = SetRule->state;
            memcpy( &SetFWConfReq.CMD_INFO.IPRange_ipv4.starting_ip, SetRule->IPAddr1,4);
            memcpy( &SetFWConfReq.CMD_INFO.IPRange_ipv4.closing_ip, SetRule->IPAddr2,4);
            memcpy( &SetFWConfReq.CMD_INFO.IPRange_ipv4.timeout, &SetRule->timeout,12);

            dwReqLen = 8+2+12;
    }
    //dwResLen = 2;

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                            ( ( NETFN_AMI << 2 ) + 0x00 ), CMD_AMI_SET_FIREWALL,
                                            (uint8 *) &SetFWConfReq, dwReqLen,
                                            (uint8 *) &SetFWConfRes, &dwResLen, timeout );

    return( wRet );
}

uint16 IPMICMD_AMISetDelFirewall_PortRule( IPMI20_SESSION_T *pSession, GetPortRule *SetRule,
                                                                    int timeout )
{
    uint16 wRet;
    uint32 dwResLen;
    uint32 dwReqLen = 0;
    AMISetFirewallReq_T SetFWConfReq;
    AMISetFirewallRes_T SetFWConfRes;

    memset(&SetFWConfReq,0,sizeof(AMISetFirewallReq_T));
    memset(&SetFWConfRes,0,sizeof(AMISetFirewallRes_T));

    /*Fill the parameter selector */

    if(SetRule->type == PORT_IPV4_RELEASE)
    {
            SetFWConfReq.Param = PORT_IPV4_RELEASE;
            SetFWConfReq.CMD_INFO.Port_Data.protocol = SetRule->protocol;
            SetFWConfReq.State = SetRule->state;
            memcpy( &SetFWConfReq.CMD_INFO.Port_Data.Port_NO, &SetRule->port1, 2);
            dwReqLen = 2+3;
    }
    else if(SetRule->type == PORT_RANGE_IPV4_RELEASE)
    {
            SetFWConfReq.Param = PORT_RANGE_IPV4_RELEASE;
            SetFWConfReq.CMD_INFO.Port_Data.protocol = SetRule->protocol;
            SetFWConfReq.State = SetRule->state;
            memcpy( &SetFWConfReq.CMD_INFO.Port_Range.starting_port, &SetRule->port1, 2);
            memcpy( &SetFWConfReq.CMD_INFO.Port_Range.closing_port, &SetRule->port2, 2);
            dwReqLen = 4+3;
    }

    else if(SetRule->type == PORT_IPV4_TIMEOUT_RELEASE)
    {
            SetFWConfReq.Param = PORT_IPV4_TIMEOUT_RELEASE;
            SetFWConfReq.CMD_INFO.Port_Data.protocol = SetRule->protocol;
            SetFWConfReq.State = SetRule->state;
            memcpy( &SetFWConfReq.CMD_INFO.Port_Data.Port_NO, &SetRule->port1, 2);
            memcpy( &SetFWConfReq.CMD_INFO.Port_Data.timeout, &SetRule->timeout,12);

            dwReqLen = 2+3+12;
    }
    else if(SetRule->type == PORT_RANGE_IPV4_TIMEOUT_RELEASE)
    {
            SetFWConfReq.Param = PORT_RANGE_IPV4_TIMEOUT_RELEASE;
            SetFWConfReq.CMD_INFO.Port_Data.protocol = SetRule->protocol;
            SetFWConfReq.State = SetRule->state;
            memcpy( &SetFWConfReq.CMD_INFO.Port_Range.starting_port, &SetRule->port1, 2);
            memcpy( &SetFWConfReq.CMD_INFO.Port_Range.closing_port, &SetRule->port2, 2);
            memcpy( &SetFWConfReq.CMD_INFO.Port_Range.timeout, &SetRule->timeout,12);

            dwReqLen = 4+3+12;
    }
    //dwResLen = 2;

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                            ( ( NETFN_AMI << 2 ) + 0x00 ), CMD_AMI_SET_FIREWALL,
                                            (uint8 *) &SetFWConfReq, dwReqLen,
                                            (uint8 *) &SetFWConfRes, &dwResLen, timeout );

    return( wRet );
}

uint16 IPMICMD_AMISetAddFirewall_IPv6_Rule( IPMI20_SESSION_T *pSession, GetIPv6Rule *SetRule,
                                                                    int timeout )
{
    uint16 wRet;
    uint32 dwResLen;
    uint32 dwReqLen = 0;
    AMISetFirewallReq_T SetFWConfReq;
    AMISetFirewallRes_T SetFWConfRes;

    memset(&SetFWConfReq,0,sizeof(AMISetFirewallReq_T));
    memset(&SetFWConfRes,0,sizeof(AMISetFirewallRes_T));

    /*Fill the parameter selector */

    if(SetRule->type == IP_IPV6 )
    {
            SetFWConfReq.Param = IP_IPV6;
            SetFWConfReq.State = SetRule->state;
            memcpy( &SetFWConfReq.CMD_INFO.IPAddr_ipv6.ipaddr_ip, SetRule->IPv6Addr1,16);
            dwReqLen = 16+2;
    }
    else if(SetRule->type == IPS_IPV6 )
    {
            SetFWConfReq.Param = IPS_IPV6;
            SetFWConfReq.State = SetRule->state;
            memcpy( &SetFWConfReq.CMD_INFO.IPRange_ipv6.starting_ip, SetRule->IPv6Addr1,16);
            memcpy( &SetFWConfReq.CMD_INFO.IPRange_ipv6.closing_ip, SetRule->IPv6Addr2,16);
            dwReqLen = 32+2;
    }
    else if(SetRule->type == IP_IPV6_TIMEOUT  )
    {
            SetFWConfReq.Param = IP_IPV6_TIMEOUT;
            SetFWConfReq.State = SetRule->state;
            memcpy( &SetFWConfReq.CMD_INFO.IPRange_ipv6.starting_ip, SetRule->IPv6Addr1,16);
            memcpy( &SetFWConfReq.CMD_INFO.IPAddr_ipv6.timeout, &SetRule->timeout,12);
            dwReqLen = 16+12+2;

    }
    else if(SetRule->type == IPS_IPV6_TIMEOUT  )
    {
            SetFWConfReq.Param = IPS_IPV6_TIMEOUT;
            SetFWConfReq.State = SetRule->state;
            memcpy( &SetFWConfReq.CMD_INFO.IPRange_ipv6.starting_ip, SetRule->IPv6Addr1,16);
            memcpy( &SetFWConfReq.CMD_INFO.IPRange_ipv6.closing_ip, SetRule->IPv6Addr2,16);

            memcpy( &SetFWConfReq.CMD_INFO.IPRange_ipv6.timeout, &SetRule->timeout,12);
            dwReqLen = 32+12+2;
    }

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                            ( ( NETFN_AMI << 2 ) + 0x00 ), CMD_AMI_SET_FIREWALL,
                                            (uint8 *) &SetFWConfReq, dwReqLen,
                                            (uint8 *) &SetFWConfRes, &dwResLen, timeout );

    return( wRet );
}

uint16 IPMICMD_AMISetAddFirewall_IPv6_PortRule( IPMI20_SESSION_T *pSession, GetPortRule *SetRule,
                                                                    int timeout )
{
    uint16 wRet;
    uint32 dwResLen;
    uint32 dwReqLen = 0;
    AMISetFirewallReq_T SetFWConfReq;
    AMISetFirewallRes_T SetFWConfRes;

    memset(&SetFWConfReq,0,sizeof(AMISetFirewallReq_T));
    memset(&SetFWConfRes,0,sizeof(AMISetFirewallRes_T));


    /*Fill the parameter selector */

    if(SetRule->type == PORT_IPV6)
    {
            SetFWConfReq.Param = PORT_IPV6;
            SetFWConfReq.CMD_INFO.Port_Data.protocol = SetRule->protocol;
            SetFWConfReq.State = SetRule->state;
            memcpy( &SetFWConfReq.CMD_INFO.Port_Data.Port_NO, &SetRule->port1, 2);
            dwReqLen = 2+3;
    }
    else if(SetRule->type == PORT_RANGE_IPV6)
    {
            SetFWConfReq.Param = PORT_RANGE_IPV6;
            SetFWConfReq.CMD_INFO.Port_Data.protocol = SetRule->protocol;
            SetFWConfReq.State = SetRule->state;
            memcpy( &SetFWConfReq.CMD_INFO.Port_Range.starting_port, &SetRule->port1, 2);
            memcpy( &SetFWConfReq.CMD_INFO.Port_Range.closing_port, &SetRule->port2, 2);
            dwReqLen = 4+3;
    }
    else if(SetRule->type == PORT_IPV6_TIMEOUT)
    {
            SetFWConfReq.Param = PORT_IPV6_TIMEOUT;
            SetFWConfReq.CMD_INFO.Port_Data.protocol = SetRule->protocol;
            SetFWConfReq.State = SetRule->state;
            memcpy( &SetFWConfReq.CMD_INFO.Port_Data.Port_NO, &SetRule->port1, 2);
            memcpy( &SetFWConfReq.CMD_INFO.Port_Data.timeout, &SetRule->timeout,12);
            dwReqLen = 2+3+12;
    }
    else if(SetRule->type == PORT_RANGE_IPV6_TIMEOUT)
    {
            SetFWConfReq.Param = PORT_RANGE_IPV6_TIMEOUT;
            SetFWConfReq.CMD_INFO.Port_Data.protocol = SetRule->protocol;
            SetFWConfReq.State = SetRule->state;
            memcpy( &SetFWConfReq.CMD_INFO.Port_Range.starting_port, &SetRule->port1, 2);
            memcpy( &SetFWConfReq.CMD_INFO.Port_Range.closing_port, &SetRule->port2, 2);
            memcpy( &SetFWConfReq.CMD_INFO.Port_Range.timeout, &SetRule->timeout,12);
            dwReqLen = 4+3+12;
    }

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                            ( ( NETFN_AMI << 2 ) + 0x00 ), CMD_AMI_SET_FIREWALL,
                                            (uint8 *) &SetFWConfReq, dwReqLen,
                                            (uint8 *) &SetFWConfRes, &dwResLen, timeout );

    return( wRet );
}

uint16 IPMICMD_AMISetDelFirewall_IPv6_Rule( IPMI20_SESSION_T *pSession, GetIPv6Rule *SetRule,
                                                                    int timeout )
{
    uint16 wRet;
    uint32 dwResLen;
    uint32 dwReqLen = 0;
    AMISetFirewallReq_T SetFWConfReq;
    AMISetFirewallRes_T SetFWConfRes;

    memset(&SetFWConfReq,0,sizeof(AMISetFirewallReq_T));
    memset(&SetFWConfRes,0,sizeof(AMISetFirewallRes_T));

    /*Fill the parameter selector */

    if(SetRule->type == IP_IPV6_UNLOCK)
    {
            SetFWConfReq.Param = IP_IPV6_UNLOCK;
            SetFWConfReq.State = SetRule->state;
            memcpy( &SetFWConfReq.CMD_INFO.IPAddr_ipv6.ipaddr_ip, SetRule->IPv6Addr1,16);
            dwReqLen = 16+2;
    }
    else if(SetRule->type == IPS_IPV6_UNLOCK)
    {
            SetFWConfReq.Param = IPS_IPV6_UNLOCK;
            SetFWConfReq.State = SetRule->state;
            memcpy( &SetFWConfReq.CMD_INFO.IPRange_ipv6.starting_ip, SetRule->IPv6Addr1,16);
            memcpy( &SetFWConfReq.CMD_INFO.IPRange_ipv6.closing_ip, SetRule->IPv6Addr2,16);
            dwReqLen = 32+2;
    }
    else if(SetRule->type == IP_IPV6_TIMEOUT_UNLOCK)
    {
            SetFWConfReq.Param = IP_IPV6_TIMEOUT_UNLOCK;
            SetFWConfReq.State = SetRule->state;
            memcpy( &SetFWConfReq.CMD_INFO.IPAddr_ipv6.ipaddr_ip, SetRule->IPv6Addr1,16);
            memcpy( &SetFWConfReq.CMD_INFO.IPAddr_ipv6.timeout, &SetRule->timeout,12);
            dwReqLen = 16+2+12;
    }
    else if(SetRule->type == IPS_IPV6_TIMEOUT_UNLOCK)
    {
            SetFWConfReq.Param = IPS_IPV6_TIMEOUT_UNLOCK;
            SetFWConfReq.State = SetRule->state;
            memcpy( &SetFWConfReq.CMD_INFO.IPRange_ipv6.starting_ip, SetRule->IPv6Addr1,16);
            memcpy( &SetFWConfReq.CMD_INFO.IPRange_ipv6.closing_ip, SetRule->IPv6Addr2,16);
            memcpy( &SetFWConfReq.CMD_INFO.IPRange_ipv6.timeout, &SetRule->timeout,12);

            dwReqLen = 32+2+12;
    }
    //dwResLen = 2;

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                            ( ( NETFN_AMI << 2 ) + 0x00 ), CMD_AMI_SET_FIREWALL,
                                            (uint8 *) &SetFWConfReq, dwReqLen,
                                            (uint8 *) &SetFWConfRes, &dwResLen, timeout );

    return( wRet );
}

uint16 IPMICMD_AMISetDelFirewall_IPv6_PortRule( IPMI20_SESSION_T *pSession, GetPortRule *SetRule,
                                                                    int timeout )
{
    uint16 wRet;
    uint32 dwResLen;
    uint32 dwReqLen = 0;
    AMISetFirewallReq_T SetFWConfReq;
    AMISetFirewallRes_T SetFWConfRes;

    memset(&SetFWConfReq,0,sizeof(AMISetFirewallReq_T));
    memset(&SetFWConfRes,0,sizeof(AMISetFirewallRes_T));

    /*Fill the parameter selector */

    if(SetRule->type == PORT_IPV6_RELEASE)
    {
            SetFWConfReq.Param = PORT_IPV6_RELEASE;
            SetFWConfReq.CMD_INFO.Port_Data.protocol = SetRule->protocol;
            SetFWConfReq.State = SetRule->state;
            memcpy( &SetFWConfReq.CMD_INFO.Port_Data.Port_NO, &SetRule->port1, 2);
            dwReqLen = 2+3;
    }
    else if(SetRule->type == PORT_RANGE_IPV6_RELEASE)
    {
            SetFWConfReq.Param = PORT_RANGE_IPV6_RELEASE;
            SetFWConfReq.CMD_INFO.Port_Data.protocol = SetRule->protocol;
            SetFWConfReq.State = SetRule->state;
            memcpy( &SetFWConfReq.CMD_INFO.Port_Range.starting_port, &SetRule->port1, 2);
            memcpy( &SetFWConfReq.CMD_INFO.Port_Range.closing_port, &SetRule->port2, 2);
            dwReqLen = 4+3;
    }

    else if(SetRule->type == PORT_IPV6_TIMEOUT_RELEASE)
    {
            SetFWConfReq.Param = PORT_IPV6_TIMEOUT_RELEASE;
            SetFWConfReq.CMD_INFO.Port_Data.protocol = SetRule->protocol;
            SetFWConfReq.State = SetRule->state;
            memcpy( &SetFWConfReq.CMD_INFO.Port_Data.Port_NO, &SetRule->port1, 2);
            memcpy( &SetFWConfReq.CMD_INFO.Port_Data.timeout, &SetRule->timeout,12);

            dwReqLen = 2+3+12;
    }
    else if(SetRule->type == PORT_RANGE_IPV6_TIMEOUT_RELEASE)
    {
            SetFWConfReq.Param = PORT_RANGE_IPV6_TIMEOUT_RELEASE;
            SetFWConfReq.CMD_INFO.Port_Data.protocol = SetRule->protocol;
            SetFWConfReq.State = SetRule->state;
            memcpy( &SetFWConfReq.CMD_INFO.Port_Range.starting_port, &SetRule->port1, 2);
            memcpy( &SetFWConfReq.CMD_INFO.Port_Range.closing_port, &SetRule->port2, 2);
            memcpy( &SetFWConfReq.CMD_INFO.Port_Range.timeout, &SetRule->timeout,12);

            dwReqLen = 4+3+12;
    }
    //dwResLen = 2;

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                            ( ( NETFN_AMI << 2 ) + 0x00 ), CMD_AMI_SET_FIREWALL,
                                            (uint8 *) &SetFWConfReq, dwReqLen,
                                            (uint8 *) &SetFWConfRes, &dwResLen, timeout );

    return( wRet );
}


uint16 LIBIPMI_HL_AMISetFirewallCfg( IPMI20_SESSION_T *pSession, INT8U blockAll,
	INT8U flushAll, int timeout )
{
	uint16 wRet;
	if (flushAll == 1)
	{
		wRet = IPMICMD_AMIFLUSHALL_IPRULES(pSession, timeout);
		if(wRet != 0)
		{
			TDBG("Error in setting Flush all::%x\n", wRet);
		}
	}
	else
	{
		wRet = IPMICMD_AMISET_BlockALL_IPRULES(pSession, blockAll, flushAll, timeout);
		if(wRet != 0)
		{
			TDBG("Error in setting BlockAll configuration::%x\n", wRet);
		}
	}
	return (wRet);
}

int IPMICMD_AMIFLUSHALL_IPRULES(IPMI20_SESSION_T *pSession,int timeout )
{
    uint16 wRet;
    AMISetFirewallReq_T SetFWConfReq;
    AMISetFirewallRes_T SetFWConfRes;
    uint32 dwResLen;
	uint32 dwReqLen = 1;

    memset(&SetFWConfReq,0,sizeof(SetFWConfReq));
    memset(&SetFWConfRes,0,sizeof(SetFWConfRes));

    SetFWConfReq.Param = FLUSH;
        wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
    										( ( NETFN_AMI << 2 ) + 0x00 ), CMD_AMI_SET_FIREWALL,
    										(uint8 *) &SetFWConfReq, dwReqLen,
    										(uint8 *) &SetFWConfRes, &dwResLen, timeout );

    return( wRet );
}

int IPMICMD_AMISET_BlockALL_IPRULES(IPMI20_SESSION_T *pSession,INT8U Block,INT8U Param,int timeout )
{
    uint16 wRet;
    AMISetFirewallReq_T SetFWConfReq;
    AMISetFirewallRes_T SetFWConfRes;
    uint32 dwResLen;
    uint32 dwReqLen = 3;

    memset(&SetFWConfReq,0,sizeof(SetFWConfReq));
    memset(&SetFWConfRes,0,sizeof(SetFWConfRes));

    if(Param == 0)
        SetFWConfReq.Param = DISABLE_ALL;
    else
        SetFWConfReq.Param = REMOVE_DISABLE_ALL;

    SetFWConfReq.CMD_INFO.Block.block = Block;/*1 - IPv4 only ; 2 - IPv6 only ; 3 - Both*/
        wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
    										( ( NETFN_AMI << 2 ) + 0x00 ), CMD_AMI_SET_FIREWALL,
    										(uint8 *) &SetFWConfReq, dwReqLen,
    										(uint8 *) &SetFWConfRes, &dwResLen, timeout );

    return( wRet );
}

int IPMICMD_AMISET_BlockALL_IPRULES_TIMEOUT(IPMI20_SESSION_T *pSession,INT8U Block,INT8U Param,GetTimeout_T *fwtimeout,int timeout )
{
	uint16 wRet;
    AMISetFirewallReq_T SetFWConfReq;
    AMISetFirewallRes_T SetFWConfRes;
    uint32 dwResLen;
    uint32 dwReqLen = 3+12;

    memset(&SetFWConfReq,0,sizeof(SetFWConfReq));
    memset(&SetFWConfRes,0,sizeof(SetFWConfRes));

    if(Param == 1)
        SetFWConfReq.Param = DISABLE_ALL_TIMEOUT;
    else
        SetFWConfReq.Param = REMOVE_DISABLE_ALL_TIMEOUT;

    SetFWConfReq.CMD_INFO.Block.block = Block;/*1 - IPv4 only ; 2 - IPv6 only ; 3 - Both*/
    memcpy(&SetFWConfReq.CMD_INFO.Block.timeout,fwtimeout,12);
        wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                                ( ( NETFN_AMI << 2 ) + 0x00 ), CMD_AMI_SET_FIREWALL,
                                                (uint8 *) &SetFWConfReq, dwReqLen,
                                                (uint8 *) &SetFWConfRes, &dwResLen, timeout );

    return( wRet );
}

/******** set command for Firewall ***************/

uint16 IPMICMD_AMIGetSMTPServerInfo( IPMI20_SESSION_T *pSession,
								char	*smtpServerIP, unsigned char Channel,
                             int timeout )
{
    uint16 wRet;
    uint32 dwResLen;
	GetSMTPConfigReq_T GetSMTPServerCfgReq;
	GetSMTPConfigRes_T GetSMTPServerCfgRes;
	unsigned char ip[4];

	memset(&GetSMTPServerCfgReq,0,sizeof(GetSMTPConfigReq_T));
	memset(&GetSMTPServerCfgRes,0,sizeof(GetSMTPConfigRes_T));

	/* Now get the SMTP server IP */
	/*At present Hardcoded ,Web have to find the app Channel no */
	GetSMTPServerCfgReq.Channel = Channel;
	GetSMTPServerCfgReq.ParameterSelect = 1;
	GetSMTPServerCfgReq.BlockSelector = 0;
	GetSMTPServerCfgReq.SetSelector = 0;
	dwResLen = 5;

	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
											( ( NETFN_AMI << 2 ) + 0x00 ), CMD_GET_SMTP_CONFIG_PARAMS,
											(uint8 *) &GetSMTPServerCfgReq, sizeof(GetSMTPConfigReq_T),
											(uint8 *) &GetSMTPServerCfgRes, &dwResLen, timeout );
	memcpy(ip,GetSMTPServerCfgRes.ConfigData.ServerAddr,IP_ADDR_LEN);


	if( INET6_ADDRSTRLEN <= snprintf(smtpServerIP, INET6_ADDRSTRLEN, "%d.%d.%d.%d",ip[0],ip[1],ip[2],ip[3]))
		TCRIT("IPMICMD_AMIGetSMTPServerInfo - Source buffer is truncated");
    return( wRet );
}

uint16 IPMICMD_AMIGetSMTPServer2Info( IPMI20_SESSION_T *pSession,
								char	*smtpServerIP, unsigned char Channel,
                             int timeout )
{
    uint16 wRet;
    uint32 dwResLen;
	GetSMTPConfigReq_T GetSMTPServerCfgReq;
	GetSMTPConfigRes_T GetSMTPServerCfgRes;
	unsigned char ip[4];

	memset(&GetSMTPServerCfgReq,0,sizeof(GetSMTPConfigReq_T));
	memset(&GetSMTPServerCfgRes,0,sizeof(GetSMTPConfigRes_T));

	/* Now get the SMTP 2 server IP */
	/*At present Hardcoded ,Web have to find the app Channel no */
	GetSMTPServerCfgReq.Channel = Channel;
	GetSMTPServerCfgReq.ParameterSelect = OEM_PARAM_SMTP_2_SERVR_ADDR;
	GetSMTPServerCfgReq.BlockSelector = 0;
	GetSMTPServerCfgReq.SetSelector = 0;
	dwResLen = 5;

	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
											( ( NETFN_AMI << 2 ) + 0x00 ), CMD_GET_SMTP_CONFIG_PARAMS,
											(uint8 *) &GetSMTPServerCfgReq, sizeof(GetSMTPConfigReq_T),
											(uint8 *) &GetSMTPServerCfgRes, &dwResLen, timeout );
	memcpy(ip,GetSMTPServerCfgRes.ConfigData.Server2Addr,IP_ADDR_LEN);


	if( INET6_ADDRSTRLEN <= snprintf(smtpServerIP, INET6_ADDRSTRLEN, "%d.%d.%d.%d",ip[0],ip[1],ip[2],ip[3]))
		TCRIT("IPMICMD_AMIGetSMTPServer2Info - Source buffer is truncated.");
    return( wRet );
}
/* ipv6 */
uint16 IPMICMD_AMIGetSMTPServerInfo_IPv6( IPMI20_SESSION_T *pSession,
								char	*smtpServerIP, unsigned char Channel,
                             int timeout )
{
    uint16 wRet;
    uint32 dwResLen;
	GetSMTPConfigReq_T GetSMTPServerCfgReq;
	GetSMTPConfigRes_T GetSMTPServerCfgRes;

	memset(&GetSMTPServerCfgReq,0,sizeof(GetSMTPConfigReq_T));
	memset(&GetSMTPServerCfgRes,0,sizeof(GetSMTPConfigRes_T));

	/* Now get the SMTP server IP */
	/*At present Hardcoded ,Web have to find the app Channel no */
	GetSMTPServerCfgReq.Channel = Channel;
	GetSMTPServerCfgReq.ParameterSelect = OEM_PARAM_SMTP_IPV6_SERVR_ADDR;
	GetSMTPServerCfgReq.BlockSelector = 0;
	GetSMTPServerCfgReq.SetSelector = 0;
	dwResLen = 5;

	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
											( ( NETFN_AMI << 2 ) + 0x00 ), CMD_GET_SMTP_CONFIG_PARAMS,
											(uint8 *) &GetSMTPServerCfgReq, sizeof(GetSMTPConfigReq_T),
											(uint8 *) &GetSMTPServerCfgRes, &dwResLen, timeout );

	memcpy(smtpServerIP,GetSMTPServerCfgRes.ConfigData.IP6_ServerAddr,IP6_ADDR_LEN);
	TDBG ("SMTP v6 Server IP Address::%x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x\n",
						smtpServerIP[0],smtpServerIP[1],smtpServerIP[2],smtpServerIP[3],
						smtpServerIP[4],smtpServerIP[5],smtpServerIP[6],smtpServerIP[7],
						smtpServerIP[8],smtpServerIP[9],smtpServerIP[10],smtpServerIP[11],
						smtpServerIP[12],smtpServerIP[13],smtpServerIP[14],smtpServerIP[15]);
	return( wRet );
}

/* ipv6 */
uint16 IPMICMD_AMIGetSMTPServer2Info_IPv6( IPMI20_SESSION_T *pSession,
								char	*smtpServerIP, unsigned char Channel,
                             int timeout )
{
    uint16 wRet;
    uint32 dwResLen;
	GetSMTPConfigReq_T GetSMTPServerCfgReq;
	GetSMTPConfigRes_T GetSMTPServerCfgRes;

	memset(&GetSMTPServerCfgReq,0,sizeof(GetSMTPConfigReq_T));
	memset(&GetSMTPServerCfgRes,0,sizeof(GetSMTPConfigRes_T));

	/* Now get the SMTP 2 server IP */
	/*At present Hardcoded ,Web have to find the app Channel no */
	GetSMTPServerCfgReq.Channel = Channel;
	GetSMTPServerCfgReq.ParameterSelect = OEM_PARAM_SMTP_2_IPV6_SERVR_ADDR;
	GetSMTPServerCfgReq.BlockSelector = 0;
	GetSMTPServerCfgReq.SetSelector = 0;
	dwResLen = 5;

	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
											( ( NETFN_AMI << 2 ) + 0x00 ), CMD_GET_SMTP_CONFIG_PARAMS,
											(uint8 *) &GetSMTPServerCfgReq, sizeof(GetSMTPConfigReq_T),
											(uint8 *) &GetSMTPServerCfgRes, &dwResLen, timeout );

	memcpy(smtpServerIP,GetSMTPServerCfgRes.ConfigData.IP6_Server2Addr,IP6_ADDR_LEN);
	TDBG ("SMTP v6 Server IP Address::%x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x\n",
						smtpServerIP[0],smtpServerIP[1],smtpServerIP[2],smtpServerIP[3],
						smtpServerIP[4],smtpServerIP[5],smtpServerIP[6],smtpServerIP[7],
						smtpServerIP[8],smtpServerIP[9],smtpServerIP[10],smtpServerIP[11],
						smtpServerIP[12],smtpServerIP[13],smtpServerIP[14],smtpServerIP[15]);
	return( wRet );
}

uint16 IPMICMD_AMISetSMTPServerInfo(IPMI20_SESSION_T *pSession, char *smtpServerIP, unsigned char Channel,int timeout)
{
    uint16 wRet;
    uint32 dwResLen;
	SetSMTPConfigReq_T SetSMTPServerCfgReq;
	SetSMTPConfigRes_T SetSMTPServerCfgRes;

	memset(&SetSMTPServerCfgReq,0,sizeof(SetSMTPConfigReq_T));
	memset(&SetSMTPServerCfgRes,0,sizeof(SetSMTPConfigRes_T));

	/* Now Set the SMTP server IP */
	SetSMTPServerCfgReq.Channel=Channel;
	SetSMTPServerCfgReq.ParameterSelect = 1;
	SetSMTPServerCfgReq.BlockSelector = 0;
	SetSMTPServerCfgReq.SetSelector = 0;
	memcpy((char *)&SetSMTPServerCfgReq.ConfigData.ServerAddr[0],smtpServerIP,4);
	dwResLen = 1;

	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
											( ( NETFN_AMI << 2 ) + 0x00 ), CMD_SET_SMTP_CONFIG_PARAMS,
											(uint8 *) &SetSMTPServerCfgReq, 8,
											(uint8 *) &SetSMTPServerCfgRes, &dwResLen, timeout );
	if(wRet != 0)
	{
		TCRIT("Setting SMTP ServerIP failed, compcode %x\n",SetSMTPServerCfgRes.CompletionCode);
	}
    return( wRet );
}
uint16 IPMICMD_AMISetSMTPServer2Info(IPMI20_SESSION_T *pSession, char *smtpServerIP, unsigned char Channel,int timeout)
{
    uint16 wRet;
    uint32 dwResLen;
	SetSMTPConfigReq_T SetSMTPServerCfgReq;
	SetSMTPConfigRes_T SetSMTPServerCfgRes;

	memset(&SetSMTPServerCfgReq,0,sizeof(SetSMTPConfigReq_T));
	memset(&SetSMTPServerCfgRes,0,sizeof(SetSMTPConfigRes_T));

	/* Now Set the SMTP 2 server IP */
	SetSMTPServerCfgReq.Channel=Channel;
	SetSMTPServerCfgReq.ParameterSelect = OEM_PARAM_SMTP_2_SERVR_ADDR;
	SetSMTPServerCfgReq.BlockSelector = 0;
	SetSMTPServerCfgReq.SetSelector = 0;
	memcpy((char *)&SetSMTPServerCfgReq.ConfigData.Server2Addr[0],smtpServerIP,4);
	dwResLen = 1;

	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
											( ( NETFN_AMI << 2 ) + 0x00 ), CMD_SET_SMTP_CONFIG_PARAMS,
											(uint8 *) &SetSMTPServerCfgReq, 8,
											(uint8 *) &SetSMTPServerCfgRes, &dwResLen, timeout );
	if(wRet != 0)
	{
		TCRIT("Setting SMTP ServerIP failed, compcode %x\n",SetSMTPServerCfgRes.CompletionCode);
	}
    return( wRet );
}

/* ipv6 */
uint16 IPMICMD_AMISetSMTPServerInfo_IPv6(IPMI20_SESSION_T *pSession, char *
smtpServerIP, unsigned char Channel,int timeout)
{
    uint16 wRet;
    uint32 dwResLen;
	SetSMTPConfigReq_T SetSMTPServerCfgReq;
	SetSMTPConfigRes_T SetSMTPServerCfgRes;

	memset(&SetSMTPServerCfgReq,0,sizeof(SetSMTPConfigReq_T));
	memset(&SetSMTPServerCfgRes,0,sizeof(SetSMTPConfigRes_T));

	/* Now Set the SMTP server IP */
	SetSMTPServerCfgReq.Channel=Channel;
	SetSMTPServerCfgReq.ParameterSelect = OEM_PARAM_SMTP_IPV6_SERVR_ADDR;
	SetSMTPServerCfgReq.BlockSelector = 0;
	SetSMTPServerCfgReq.SetSelector = 0;
	memcpy((char *)&SetSMTPServerCfgReq.ConfigData.IP6_ServerAddr[0],smtpServerIP,IP6_ADDR_LEN);
	dwResLen = 1;

	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
											( ( NETFN_AMI << 2 ) + 0x00 ), CMD_SET_SMTP_CONFIG_PARAMS,
											(uint8 *) &SetSMTPServerCfgReq, 8+12,
											(uint8 *) &SetSMTPServerCfgRes, &dwResLen, timeout );
	if(wRet != 0)
	{
		TCRIT("Setting SMTP ServerIP failed, compcode %x\n",SetSMTPServerCfgRes.CompletionCode);
	}
    return( wRet );
}

/* ipv6 */
uint16 IPMICMD_AMISetSMTPServer2Info_IPv6(IPMI20_SESSION_T *pSession, char *
smtpServerIP, unsigned char Channel,int timeout)
{
    uint16 wRet;
    uint32 dwResLen;
	SetSMTPConfigReq_T SetSMTPServerCfgReq;
	SetSMTPConfigRes_T SetSMTPServerCfgRes;

	memset(&SetSMTPServerCfgReq,0,sizeof(SetSMTPConfigReq_T));
	memset(&SetSMTPServerCfgRes,0,sizeof(SetSMTPConfigRes_T));

	/* Now Set the SMTP 2 server IP */
	SetSMTPServerCfgReq.Channel=Channel;
	SetSMTPServerCfgReq.ParameterSelect = OEM_PARAM_SMTP_2_IPV6_SERVR_ADDR;
	SetSMTPServerCfgReq.BlockSelector = 0;
	SetSMTPServerCfgReq.SetSelector = 0;
	memcpy((char *)&SetSMTPServerCfgReq.ConfigData.IP6_Server2Addr[0],smtpServerIP,IP6_ADDR_LEN);
	dwResLen = 1;

	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
											( ( NETFN_AMI << 2 ) + 0x00 ), CMD_SET_SMTP_CONFIG_PARAMS,
											(uint8 *) &SetSMTPServerCfgReq, 8+12,
											(uint8 *) &SetSMTPServerCfgRes, &dwResLen, timeout );
	if(wRet != 0)
	{
		TCRIT("Setting SMTP ServerIP failed, compcode %x\n",SetSMTPServerCfgRes.CompletionCode);
	}
    return( wRet );
}

uint16 IPMICMD_AMISetEmailDest(IPMI20_SESSION_T *pSession,
					uint8 DestSel,
					uint8 Userid,
					uint8* in_pSubject,
					uint8* in_pMsg,
					uint8 Channel,
					int timeout)
{
    uint16 wRet;
    uint32 dwResLen;
	SetSMTPConfigReq_T SetSMTPServerCfgReq;
	SetSMTPConfigRes_T SetSMTPServerCfgRes;
	int i = 0;

	char SubjectBuf[MAX_SUB_BLOCKS * MAX_SUB_BLOCK_SIZE];
        char MsgBuf[MAX_MSG_BLOCKS * MAX_MSG_BLOCK_SIZE];
	char *pSubject = &SubjectBuf[0];
	char *pMsg	= &MsgBuf[0];
	int tempret;/*To check return value of snprintf, it is not used any where else*/

	memset(pSubject,0,sizeof(SubjectBuf));
	memset(pMsg,0,sizeof(MsgBuf));
	tempret=snprintf(pSubject, sizeof(SubjectBuf),"%s",(char *) in_pSubject);
	if((tempret<0)||(tempret>= (MAX_SUB_BLOCKS * MAX_SUB_BLOCK_SIZE)))
        {
            TCRIT("Buffer Overflow");
            return -1;
        }
	tempret=snprintf(pMsg,sizeof(MsgBuf),"%s", (char *)in_pMsg);
	if((tempret<0)||(tempret>=(MAX_MSG_BLOCKS * MAX_MSG_BLOCK_SIZE)))
        {
            TCRIT("Buffer Overflow");
            return -1;
        }


	memset(&SetSMTPServerCfgReq,0,sizeof(SetSMTPConfigReq_T));
	memset(&SetSMTPServerCfgRes,0,sizeof(SetSMTPConfigRes_T));

	TDBG("libipmi: destsel = %d, UserID %x, Msg = %s\n",DestSel,Userid,pMsg);

	SetSMTPServerCfgReq.Channel= Channel;
	SetSMTPServerCfgReq.ParameterSelect = 5;
	SetSMTPServerCfgReq.SetSelector = DestSel;
	SetSMTPServerCfgReq.BlockSelector = 0;
	SetSMTPServerCfgReq.ConfigData.UserID=Userid;
	dwResLen = 1;
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
											( ( NETFN_AMI << 2 ) + 0x00 ), CMD_SET_SMTP_CONFIG_PARAMS,
											(uint8 *) &SetSMTPServerCfgReq, (sizeof(uint8)+4),
											(uint8 *) &SetSMTPServerCfgRes, &dwResLen, timeout );
	if(wRet != 0)
	{
		TCRIT("Error setting Email alert destination\n");
		return wRet;
	}

	memset(&SetSMTPServerCfgReq,0,sizeof(SetSMTPConfigReq_T));
	memset(&SetSMTPServerCfgRes,0,sizeof(SetSMTPConfigRes_T));
	for(i = 0; i < MAX_SUB_BLOCKS; i++)
	{
		SetSMTPServerCfgReq.Channel= Channel;
		SetSMTPServerCfgReq.ParameterSelect = 6;
		SetSMTPServerCfgReq.SetSelector = DestSel;
		SetSMTPServerCfgReq.BlockSelector = i;
		memcpy(SetSMTPServerCfgReq.ConfigData.Subject,pSubject,MAX_SUB_BLOCK_SIZE);
		dwResLen = 1;
		TDBG("\n Subject in Libipmi_AMI :%s\n",SetSMTPServerCfgReq.ConfigData.Subject);
		wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
												( ( NETFN_AMI << 2 ) + 0x00 ), CMD_SET_SMTP_CONFIG_PARAMS,
												(uint8 *) &SetSMTPServerCfgReq,(MAX_SUB_BLOCK_SIZE+4),
												(uint8 *) &SetSMTPServerCfgRes, &dwResLen, timeout );
		if(wRet != 0)
		{
			TCRIT("Error setting Email alert destination :Subject \n");
			return wRet;
		}
		pSubject += MAX_SUB_BLOCK_SIZE;
	}

	memset(&SetSMTPServerCfgReq,0,sizeof(SetSMTPConfigReq_T));
	memset(&SetSMTPServerCfgRes,0,sizeof(SetSMTPConfigRes_T));
	for(i = 0; i < MAX_MSG_BLOCKS; i++)
	{
		SetSMTPServerCfgReq.Channel= Channel;
		SetSMTPServerCfgReq.ParameterSelect = 7;
		SetSMTPServerCfgReq.SetSelector = DestSel;
		SetSMTPServerCfgReq.BlockSelector = i;
		memcpy(SetSMTPServerCfgReq.ConfigData.Msg,pMsg,MAX_MSG_BLOCK_SIZE);
		dwResLen = 1;
		wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
												( ( NETFN_AMI << 2 ) + 0x00 ), CMD_SET_SMTP_CONFIG_PARAMS,
												(uint8 *) &SetSMTPServerCfgReq, (MAX_MSG_BLOCK_SIZE+4),
												(uint8 *) &SetSMTPServerCfgRes, &dwResLen, timeout );
		if(wRet != 0)
		{
			TCRIT("Error setting Email alert destination\n");
			return wRet;
		}
		pMsg += MAX_MSG_BLOCK_SIZE;
	}

    return( wRet );
}


/*uint16 IPMICMD_AMISetSMTPInfo(IPMI20_SESSION_T *pSession,
					uint8 *Username,
					uint8 *Passwd,
					uint8* iSenderAddr,
					uint8* iMachinename,
					uint8 Channel,
					int timeout)*/
uint16 IPMICMD_AMISetSMTPInfo(IPMI20_SESSION_T *pSession,
		            SmtpInfoParam_T *SmtpInfo,
		            uint8 Channel,
		            int timeout)
{
    uint16 wRet;
    uint32 dwResLen=1,dwReqLen;
	SetSMTPConfigReq_T SetSMTPServerCfgReq;
	SetSMTPConfigRes_T SetSMTPServerCfgRes;
	int i = 0;
	int tempret=0;/*To check return value of snprintf, it is not used any where else*/

	uint8 SenderAddrBuf[MAX_EMAIL_ADDR_BLOCKS * MAX_EMAIL_BLOCK_SIZE];
	uint8 MachinenameBuf[MAX_SRV_NAME_BLOCKS * MAX_SRV_NAME_BLOCK_SIZE];
	uint8 *SenderAddr = &SenderAddrBuf[0];
	uint8 *Machinename = &MachinenameBuf[0];

	memset(&SetSMTPServerCfgReq, 0, sizeof(SetSMTPConfigReq_T));
	memset(&SetSMTPServerCfgRes, 0, sizeof(SetSMTPConfigRes_T));
	SetSMTPServerCfgReq.Channel = Channel;
	SetSMTPServerCfgReq.ParameterSelect = OEM_ENABLE_DISABLE_SMTP;
	SetSMTPServerCfgReq.SetSelector = 0;
	SetSMTPServerCfgReq.BlockSelector = 0;
	SetSMTPServerCfgReq.ConfigData.EnableDisableSMTP = SmtpInfo->EnableDisableSMTP;
	dwResLen = 1;
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
		( ( NETFN_AMI << 2 ) + 0x00 ), CMD_SET_SMTP_CONFIG_PARAMS,
		(uint8 *) &SetSMTPServerCfgReq, MAX_SMTP_AUTH_FLAG_LEN+4,
		(uint8 *) &SetSMTPServerCfgRes, &dwResLen, timeout );
	if(wRet != 0)
	{
		TCRIT("Error in setting SMTP Enable \n");
		return wRet;
	}

	memset(SenderAddr,0,sizeof(SenderAddrBuf));
	memset(Machinename,0,sizeof(MachinenameBuf));
	tempret=snprintf((char *)SenderAddr,sizeof(SenderAddrBuf),"%s",(char *)SmtpInfo->SenderAddr);
	if((tempret<0)||(tempret>= (MAX_EMAIL_ADDR_BLOCKS * MAX_EMAIL_BLOCK_SIZE)))
        {
            TCRIT("Buffer Overflow");
            return -1;
        }

	tempret=snprintf((char *)Machinename,sizeof(MachinenameBuf),"%s",(char *)SmtpInfo->MachineName);
	if((tempret<0)||(tempret>= (MAX_SRV_NAME_BLOCKS * MAX_SRV_NAME_BLOCK_SIZE)))
        {
            TCRIT("Buffer Overflow");
            return -1;
        }

	if (SmtpInfo->EnableDisableSmtpAuth)
	{
		memset(&SetSMTPServerCfgReq,0,sizeof(SetSMTPConfigReq_T));
		memset(&SetSMTPServerCfgRes,0,sizeof(SetSMTPConfigRes_T));
		SetSMTPServerCfgReq.Channel= Channel;
		SetSMTPServerCfgReq.ParameterSelect = OEM_PARAM_SMTP_USER_NAME;
		SetSMTPServerCfgReq.SetSelector = 0;
		SetSMTPServerCfgReq.BlockSelector = 0;
		memcpy(SetSMTPServerCfgReq.ConfigData.UserName,SmtpInfo->UserName,MAX_SMTP_USERNAME_LEN-1);
		dwResLen = 1;
		dwReqLen = MAX_SMTP_USERNAME_LEN - 1;
		wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
													( ( NETFN_AMI << 2 ) + 0x00 ), CMD_SET_SMTP_CONFIG_PARAMS,
													(uint8 *) &SetSMTPServerCfgReq,dwReqLen+4,
													(uint8 *) &SetSMTPServerCfgRes, &dwResLen, timeout );
		if(wRet != 0)
		{
			TCRIT("Error setting the username \n");
			return wRet;
		}

		if (0!= strcmp((char *)SmtpInfo->Passwd,""))
		{
			memset(&SetSMTPServerCfgReq,0,sizeof(SetSMTPConfigReq_T));
			memset(&SetSMTPServerCfgRes,0,sizeof(SetSMTPConfigRes_T));
			SetSMTPServerCfgReq.Channel= Channel;
			SetSMTPServerCfgReq.ParameterSelect = OEM_PARAM_SMTP_PASSWD;
			SetSMTPServerCfgReq.SetSelector = 0;
			SetSMTPServerCfgReq.BlockSelector = 0;
			memcpy(SetSMTPServerCfgReq.ConfigData.Passwd,SmtpInfo->Passwd,MAX_SMTP_PASSWD_LEN-1);
			dwResLen = 1;
			dwReqLen = MAX_SMTP_PASSWD_LEN - 1;
			wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
													( ( NETFN_AMI << 2 ) + 0x00 ), CMD_SET_SMTP_CONFIG_PARAMS,
													(uint8 *) &SetSMTPServerCfgReq, dwReqLen+4,
													(uint8 *) &SetSMTPServerCfgRes, &dwResLen, timeout );
			if(wRet != 0)
			{
				TCRIT("Error setting the Password \n");
				return wRet;
			}
		}
	}


	memset(&SetSMTPServerCfgReq,0,sizeof(SetSMTPConfigReq_T));
	memset(&SetSMTPServerCfgRes,0,sizeof(SetSMTPConfigRes_T));

	for(i = 0; i < MAX_EMAIL_ADDR_BLOCKS; i++)
	{
		SetSMTPServerCfgReq.Channel= Channel;
		SetSMTPServerCfgReq.ParameterSelect = OEM_PARAM_SMTP_SENDER_ADDR;
		SetSMTPServerCfgReq.SetSelector = 0;
		SetSMTPServerCfgReq.BlockSelector = i;
		memcpy(SetSMTPServerCfgReq.ConfigData.SenderAddr,SenderAddr,MAX_EMAIL_BLOCK_SIZE);
		dwResLen = 1;
		wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
												( ( NETFN_AMI << 2 ) + 0x00 ), CMD_SET_SMTP_CONFIG_PARAMS,
												(uint8 *) &SetSMTPServerCfgReq, MAX_EMAIL_BLOCK_SIZE+4,
												(uint8 *) &SetSMTPServerCfgRes, &dwResLen, timeout );
		if(wRet != 0)
		{
			TCRIT("Error setting Sender Email address\n");
			return wRet;
		}
		SenderAddr += MAX_EMAIL_BLOCK_SIZE;
	}


	memset(&SetSMTPServerCfgReq,0,sizeof(SetSMTPConfigReq_T));
	memset(&SetSMTPServerCfgRes,0,sizeof(SetSMTPConfigRes_T));
    for(i = 0; i < MAX_SRV_NAME_BLOCKS; i++)
    {
        SetSMTPServerCfgReq.Channel= Channel;
        SetSMTPServerCfgReq.ParameterSelect = OEM_PARAM_SMTP_HOST_NAME;
        SetSMTPServerCfgReq.SetSelector = 0;
        SetSMTPServerCfgReq.BlockSelector = i;
	    memcpy(SetSMTPServerCfgReq.ConfigData.Servername,Machinename,MAX_SRV_NAME_BLOCK_SIZE);
        dwResLen = 1;
        wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                                ( ( NETFN_AMI << 2 ) + 0x00 ), CMD_SET_SMTP_CONFIG_PARAMS,
                                                (uint8 *) &SetSMTPServerCfgReq, MAX_SRV_NAME_BLOCK_SIZE+4,
                                                (uint8 *) &SetSMTPServerCfgRes, &dwResLen, timeout );
        if(wRet != 0)
        {
                TCRIT("Error setting Sender Name\n");
                return wRet;
        }
        Machinename += MAX_SRV_NAME_BLOCK_SIZE;
	}


    memset(&SetSMTPServerCfgReq,0,sizeof(SetSMTPConfigReq_T));
    memset(&SetSMTPServerCfgRes,0,sizeof(SetSMTPConfigRes_T));
    SetSMTPServerCfgReq.Channel= Channel;
    SetSMTPServerCfgReq.ParameterSelect = OEM_ENABLE_DISABLE_SMTP_AUTH;
    SetSMTPServerCfgReq.SetSelector = 0;
    SetSMTPServerCfgReq.BlockSelector = 0;
    //memcpy(SetSMTPServerCfgReq.ConfigData.EnableDisableSmtpAuth,SmtpInfo->EnableDisableSmtpAuth,MAX_SMTP_AUTH_FLAG_LEN);
    SetSMTPServerCfgReq.ConfigData.EnableDisableSmtpAuth=SmtpInfo->EnableDisableSmtpAuth;
    dwResLen = 1;
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
    													( ( NETFN_AMI << 2 ) + 0x00 ), CMD_SET_SMTP_CONFIG_PARAMS,
    													(uint8 *) &SetSMTPServerCfgReq, MAX_SMTP_AUTH_FLAG_LEN+4,
    													(uint8 *) &SetSMTPServerCfgRes, &dwResLen, timeout );
    if(wRet != 0)
    {
    	TCRIT("Error in setting SMTP Authentication \n");
    	return wRet;
    }

    memset(&SetSMTPServerCfgReq,0,sizeof(SetSMTPConfigReq_T));
    memset(&SetSMTPServerCfgRes,0,sizeof(SetSMTPConfigRes_T));
    SetSMTPServerCfgReq.Channel= Channel;
    SetSMTPServerCfgReq.ParameterSelect = OEM_PARAM_SMTP_PORT;
    SetSMTPServerCfgReq.SetSelector = 0;
    SetSMTPServerCfgReq.BlockSelector = 0;
    //memcpy(SetSMTPServerCfgReq.ConfigData.SmtpPort,SmtpInfo->SmtpPort,MAX_SMTP_PORT_SIZE);
    SetSMTPServerCfgReq.ConfigData.SmtpPort=SmtpInfo->SmtpPort;
    dwResLen = 1;
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
            													( ( NETFN_AMI << 2 ) + 0x00 ), CMD_SET_SMTP_CONFIG_PARAMS,
            													(uint8 *) &SetSMTPServerCfgReq, sizeof(INT16U)+4,
            													(uint8 *) &SetSMTPServerCfgRes, &dwResLen, timeout );
    if(wRet != 0)
    {
       	TCRIT("Error in setting SMTP Port \n");
       	return wRet;
    }
    memset(&SetSMTPServerCfgReq,0,sizeof(SetSMTPConfigReq_T));
    memset(&SetSMTPServerCfgRes,0,sizeof(SetSMTPConfigRes_T));
    SetSMTPServerCfgReq.Channel= Channel;
    SetSMTPServerCfgReq.ParameterSelect = OEM_PARAM_SMTP_SECURE_PORT;
    SetSMTPServerCfgReq.SetSelector = 0;
    SetSMTPServerCfgReq.BlockSelector = 0;
    //memcpy(SetSMTPServerCfgReq.ConfigData.SmtpPort,SmtpInfo->SmtpPort,MAX_SMTP_PORT_SIZE);
    SetSMTPServerCfgReq.ConfigData.SmtpSecurePort=SmtpInfo->SmtpSecurePort;
    dwResLen = 1;
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                													( ( NETFN_AMI << 2 ) + 0x00 ), CMD_SET_SMTP_CONFIG_PARAMS,
                													(uint8 *) &SetSMTPServerCfgReq, sizeof(INT16U)+4,
                													(uint8 *) &SetSMTPServerCfgRes, &dwResLen, timeout );
    if(wRet != 0)
    {
    	TCRIT("Error in setting SMTP Secure Port \n");
    	return wRet;
    }

	memset(&SetSMTPServerCfgReq, 0, sizeof(SetSMTPConfigReq_T));
	memset(&SetSMTPServerCfgRes, 0, sizeof(SetSMTPConfigRes_T));
	SetSMTPServerCfgReq.Channel = Channel;
	SetSMTPServerCfgReq.ParameterSelect = OEM_PARAM_ENABLE_SMTP_STARTTLS_SUPPORT;
	SetSMTPServerCfgReq.SetSelector = 0;
	SetSMTPServerCfgReq.BlockSelector = 0;
	SetSMTPServerCfgReq.ConfigData.EnableSTARTTLSSupport = SmtpInfo->EnableSTARTTLSSupport;
	dwResLen = 1;
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
		( ( NETFN_AMI << 2 ) + 0x00 ), CMD_SET_SMTP_CONFIG_PARAMS,
		(uint8 *) &SetSMTPServerCfgReq, MAX_SMTP_AUTH_FLAG_LEN+4,
		(uint8 *) &SetSMTPServerCfgRes, &dwResLen, timeout );
	if(wRet != 0)
	{
		TCRIT("Error in setting SMTP Enable STARTTLS Support Flag.\n");
		return wRet;
	}
	memset(&SetSMTPServerCfgReq, 0, sizeof(SetSMTPConfigReq_T));
		memset(&SetSMTPServerCfgRes, 0, sizeof(SetSMTPConfigRes_T));
		SetSMTPServerCfgReq.Channel = Channel;
		SetSMTPServerCfgReq.ParameterSelect = OEM_PARAM_ENABLE_SMTP_SSLTLS_SUPPORT;
		SetSMTPServerCfgReq.SetSelector = 0;
		SetSMTPServerCfgReq.BlockSelector = 0;
		SetSMTPServerCfgReq.ConfigData.EnableSSLTLSSupport = SmtpInfo->EnableSSLTLSSupport;
		dwResLen = 1;
		wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
			( ( NETFN_AMI << 2 ) + 0x00 ), CMD_SET_SMTP_CONFIG_PARAMS,
			(uint8 *) &SetSMTPServerCfgReq, MAX_SMTP_AUTH_FLAG_LEN+4,
			(uint8 *) &SetSMTPServerCfgRes, &dwResLen, timeout );
		if(wRet != 0)
		{
			TCRIT("Error in setting SMTP EnableSSLTLS Support Flag., %d \n",wRet);
			return wRet;
		}

	return wRet;
}
/*uint16 IPMICMD_AMISetSMTP2Info(IPMI20_SESSION_T *pSession,
					uint8 *Username,
					uint8 *Passwd,
					uint8* iSenderAddr,
					uint8* iMachinename,
					uint8 Channel,
					int timeout)*/
uint16 IPMICMD_AMISetSMTP2Info(IPMI20_SESSION_T *pSession,
		            SmtpInfoParam_T *SmtpInfo,
		            uint8 Channel,
		            int timeout)
{
    uint16 wRet;
    uint32 dwResLen=1,dwReqLen;
	SetSMTPConfigReq_T SetSMTPServerCfgReq;
	SetSMTPConfigRes_T SetSMTPServerCfgRes;
	int i = 0;

	uint8 SenderAddrBuf[MAX_EMAIL_ADDR_BLOCKS * MAX_EMAIL_BLOCK_SIZE];
	uint8 MachinenameBuf[MAX_SRV_NAME_BLOCKS * MAX_SRV_NAME_BLOCK_SIZE];
	uint8 *SenderAddr = &SenderAddrBuf[0];
	uint8 *Machinename = &MachinenameBuf[0];
	int tempret=0;/*To check return value of snprintf, it is not used any where else*/

	memset(&SetSMTPServerCfgReq, 0, sizeof(SetSMTPConfigReq_T));
	memset(&SetSMTPServerCfgRes, 0, sizeof(SetSMTPConfigRes_T));
	SetSMTPServerCfgReq.Channel = Channel;
	SetSMTPServerCfgReq.ParameterSelect = OEM_ENABLE_DISABLE_SMTP2;
	SetSMTPServerCfgReq.SetSelector = 0;
	SetSMTPServerCfgReq.BlockSelector = 0;
	SetSMTPServerCfgReq.ConfigData.EnableDisableSMTP2 = SmtpInfo->EnableDisableSMTP;
	dwResLen = 1;
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
		( ( NETFN_AMI << 2 ) + 0x00 ), CMD_SET_SMTP_CONFIG_PARAMS,
		(uint8 *) &SetSMTPServerCfgReq, MAX_SMTP_AUTH_FLAG_LEN+4,
		(uint8 *) &SetSMTPServerCfgRes, &dwResLen, timeout );
	if(wRet != 0)
	{
		TCRIT("Error in setting SMTP Enable \n");
		return wRet;
	}

	memset(SenderAddr,0,sizeof(SenderAddrBuf));
	memset(Machinename,0,sizeof(MachinenameBuf));
	tempret=snprintf((char *)SenderAddr,sizeof(SenderAddrBuf),"%s",(char *)SmtpInfo->SenderAddr);
	if((tempret<0)||(tempret>=(MAX_EMAIL_ADDR_BLOCKS * MAX_EMAIL_BLOCK_SIZE)))
        {
            TCRIT("Buffer Overflow");
            return -1;
        }

	tempret=snprintf((char *)Machinename,sizeof(MachinenameBuf),"%s",(char *)SmtpInfo->MachineName);
	if((tempret<0)||(tempret>=(MAX_SRV_NAME_BLOCKS * MAX_SRV_NAME_BLOCK_SIZE)))
        {
            TCRIT("Buffer Overflow");
            return -1;
        }
	if (SmtpInfo->EnableDisableSmtpAuth)
	{
		memset(&SetSMTPServerCfgReq,0,sizeof(SetSMTPConfigReq_T));
		memset(&SetSMTPServerCfgRes,0,sizeof(SetSMTPConfigRes_T));
		SetSMTPServerCfgReq.Channel= Channel;
		SetSMTPServerCfgReq.ParameterSelect = OEM_PARAM_SMTP_2_USER_NAME;
		SetSMTPServerCfgReq.SetSelector = 0;
		SetSMTPServerCfgReq.BlockSelector = 0;
		memcpy(SetSMTPServerCfgReq.ConfigData.UserName2,SmtpInfo->UserName,MAX_SMTP_USERNAME_LEN-1);
		dwResLen = 1;
		dwReqLen = MAX_SMTP_USERNAME_LEN - 1;
		wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
													( ( NETFN_AMI << 2 ) + 0x00 ), CMD_SET_SMTP_CONFIG_PARAMS,
													(uint8 *) &SetSMTPServerCfgReq, dwReqLen+4,
													(uint8 *) &SetSMTPServerCfgRes, &dwResLen, timeout );
		if(wRet != 0)
		{
			TCRIT("Error setting the username \n");
			return wRet;
		}

		if (0!= strcmp((char *)SmtpInfo->Passwd,""))
		{
			memset(&SetSMTPServerCfgReq,0,sizeof(SetSMTPConfigReq_T));
			memset(&SetSMTPServerCfgRes,0,sizeof(SetSMTPConfigRes_T));
			SetSMTPServerCfgReq.Channel= Channel;
			SetSMTPServerCfgReq.ParameterSelect = OEM_PARAM_SMTP_2_PASSWD;
			SetSMTPServerCfgReq.SetSelector = 0;
			SetSMTPServerCfgReq.BlockSelector = 0;
			memcpy(SetSMTPServerCfgReq.ConfigData.Passwd2,SmtpInfo->Passwd,MAX_SMTP_PASSWD_LEN-1);
			dwResLen = 1;
			dwReqLen = MAX_SMTP_PASSWD_LEN - 1;
			wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
													( ( NETFN_AMI << 2 ) + 0x00 ), CMD_SET_SMTP_CONFIG_PARAMS,
													(uint8 *) &SetSMTPServerCfgReq, dwReqLen+4,
													(uint8 *) &SetSMTPServerCfgRes, &dwResLen, timeout );
			if(wRet != 0)
			{
				TCRIT("Error setting the Password \n");
				return wRet;
			}
		}
	}

	memset(&SetSMTPServerCfgReq,0,sizeof(SetSMTPConfigReq_T));
	memset(&SetSMTPServerCfgRes,0,sizeof(SetSMTPConfigRes_T));

	for(i = 0; i < MAX_EMAIL_ADDR_BLOCKS; i++)
	{
		SetSMTPServerCfgReq.Channel= Channel;
		SetSMTPServerCfgReq.ParameterSelect = OEM_PARAM_SMTP_2_SENDER_ADDR;
		SetSMTPServerCfgReq.SetSelector = 0;
		SetSMTPServerCfgReq.BlockSelector = i;
		memcpy(SetSMTPServerCfgReq.ConfigData.Sender2Addr,SenderAddr,MAX_EMAIL_BLOCK_SIZE);
		dwResLen = 1;
		wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
												( ( NETFN_AMI << 2 ) + 0x00 ), CMD_SET_SMTP_CONFIG_PARAMS,
												(uint8 *) &SetSMTPServerCfgReq, MAX_EMAIL_BLOCK_SIZE+4,
												(uint8 *) &SetSMTPServerCfgRes, &dwResLen, timeout );
		if(wRet != 0)
		{
			TCRIT("Error setting Sender Email address\n");
			return wRet;
		}
		SenderAddr += MAX_EMAIL_BLOCK_SIZE;
	}


	memset(&SetSMTPServerCfgReq,0,sizeof(SetSMTPConfigReq_T));
	memset(&SetSMTPServerCfgRes,0,sizeof(SetSMTPConfigRes_T));
    for(i = 0; i < MAX_SRV_NAME_BLOCKS; i++)
    {
        SetSMTPServerCfgReq.Channel= Channel;
        SetSMTPServerCfgReq.ParameterSelect = OEM_PARAM_SMTP_2_HOST_NAME;
        SetSMTPServerCfgReq.SetSelector = 0;
        SetSMTPServerCfgReq.BlockSelector = i;
	    memcpy(SetSMTPServerCfgReq.ConfigData.Server2name,Machinename,MAX_SRV_NAME_BLOCK_SIZE);
        dwResLen = 1;
        wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                                ( ( NETFN_AMI << 2 ) + 0x00 ), CMD_SET_SMTP_CONFIG_PARAMS,
                                                (uint8 *) &SetSMTPServerCfgReq, MAX_SRV_NAME_BLOCK_SIZE+4,
                                                (uint8 *) &SetSMTPServerCfgRes, &dwResLen, timeout );
        if(wRet != 0)
        {
                TCRIT("Error setting Sender Name\n");
                return wRet;
        }
        Machinename += MAX_SRV_NAME_BLOCK_SIZE;
	}


    memset(&SetSMTPServerCfgReq,0,sizeof(SetSMTPConfigReq_T));
    memset(&SetSMTPServerCfgRes,0,sizeof(SetSMTPConfigRes_T));
    SetSMTPServerCfgReq.Channel= Channel;
    SetSMTPServerCfgReq.ParameterSelect = OEM_ENABLE_DISABLE_SMTP_2_AUTH;
    SetSMTPServerCfgReq.SetSelector = 0;
    SetSMTPServerCfgReq.BlockSelector = 0;
    //memcpy(SetSMTPServerCfgReq.ConfigData.EnableDisableSmtpAuth,SmtpInfo->EnableDisableSmtpAuth,MAX_SMTP_AUTH_FLAG_LEN);
    SetSMTPServerCfgReq.ConfigData.EnableDisableSmtp2Auth=SmtpInfo->EnableDisableSmtpAuth;
    dwResLen = 1;
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
    													( ( NETFN_AMI << 2 ) + 0x00 ), CMD_SET_SMTP_CONFIG_PARAMS,
    													(uint8 *) &SetSMTPServerCfgReq, MAX_SMTP_AUTH_FLAG_LEN+4,
    													(uint8 *) &SetSMTPServerCfgRes, &dwResLen, timeout );
    if(wRet != 0)
    {
    	TCRIT("Error in setting SMTP Authentication \n");
    	return wRet;
    }

    memset(&SetSMTPServerCfgReq,0,sizeof(SetSMTPConfigReq_T));
    memset(&SetSMTPServerCfgRes,0,sizeof(SetSMTPConfigRes_T));
    SetSMTPServerCfgReq.Channel= Channel;
    SetSMTPServerCfgReq.ParameterSelect = OEM_PARAM_SMTP_2_PORT;
    SetSMTPServerCfgReq.SetSelector = 0;
    SetSMTPServerCfgReq.BlockSelector = 0;
    //memcpy(SetSMTPServerCfgReq.ConfigData.SmtpPort,SmtpInfo->SmtpPort,MAX_SMTP_PORT_SIZE);
    SetSMTPServerCfgReq.ConfigData.Smtp2Port=SmtpInfo->SmtpPort;
    dwResLen = 1;
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
            													( ( NETFN_AMI << 2 ) + 0x00 ), CMD_SET_SMTP_CONFIG_PARAMS,
            													(uint8 *) &SetSMTPServerCfgReq, sizeof(INT16U)+4,
            													(uint8 *) &SetSMTPServerCfgRes, &dwResLen, timeout );
    if(wRet != 0)
    {
       	TCRIT("Error in setting SMTP Port \n");
       	return wRet;
    }
    memset(&SetSMTPServerCfgReq,0,sizeof(SetSMTPConfigReq_T));
    memset(&SetSMTPServerCfgRes,0,sizeof(SetSMTPConfigRes_T));
    SetSMTPServerCfgReq.Channel= Channel;
    SetSMTPServerCfgReq.ParameterSelect = OEM_PARAM_SMTP_2_SECURE_PORT;
    SetSMTPServerCfgReq.SetSelector = 0;
    SetSMTPServerCfgReq.BlockSelector = 0;
    //memcpy(SetSMTPServerCfgReq.ConfigData.SmtpPort,SmtpInfo->SmtpPort,MAX_SMTP_PORT_SIZE);
    SetSMTPServerCfgReq.ConfigData.Smtp2SecurePort=SmtpInfo->SmtpSecurePort;
    dwResLen = 1;
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
    		( ( NETFN_AMI << 2 ) + 0x00 ), CMD_SET_SMTP_CONFIG_PARAMS,
    		(uint8 *) &SetSMTPServerCfgReq, sizeof(INT16U)+4,
    		(uint8 *) &SetSMTPServerCfgRes, &dwResLen, timeout );
    if(wRet != 0)
    {
    	TCRIT("Error in setting SMTP  Secure Port \n");
    	return wRet;
    }

	memset(&SetSMTPServerCfgReq, 0, sizeof(SetSMTPConfigReq_T));
	memset(&SetSMTPServerCfgRes, 0, sizeof(SetSMTPConfigRes_T));
	SetSMTPServerCfgReq.Channel = Channel;
	SetSMTPServerCfgReq.ParameterSelect = OEM_PARAM_ENABLE_SMTP_2_STARTTLS_SUPPORT;
	SetSMTPServerCfgReq.SetSelector = 0;
	SetSMTPServerCfgReq.BlockSelector = 0;
	SetSMTPServerCfgReq.ConfigData.Smtp2EnableSTARTTLSSupport = SmtpInfo->EnableSTARTTLSSupport;
	dwResLen = 1;
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
		( ( NETFN_AMI << 2 ) + 0x00 ), CMD_SET_SMTP_CONFIG_PARAMS,
		(uint8 *) &SetSMTPServerCfgReq, MAX_SMTP_AUTH_FLAG_LEN+4,
		(uint8 *) &SetSMTPServerCfgRes, &dwResLen, timeout );
	if(wRet != 0)
	{
		TCRIT("Error in setting SMTP Enable STARTTLS Support Flag.\n");
		return wRet;
	}
	memset(&SetSMTPServerCfgReq, 0, sizeof(SetSMTPConfigReq_T));
	memset(&SetSMTPServerCfgRes, 0, sizeof(SetSMTPConfigRes_T));
	SetSMTPServerCfgReq.Channel = Channel;
	SetSMTPServerCfgReq.ParameterSelect = OEM_PARAM_ENABLE_SMTP_2_SSLTLS_SUPPORT;
	SetSMTPServerCfgReq.SetSelector = 0;
	SetSMTPServerCfgReq.BlockSelector = 0;
	SetSMTPServerCfgReq.ConfigData.Smtp2EnableSSLTLSSupport = SmtpInfo->EnableSSLTLSSupport;
	dwResLen = 1;
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
		( ( NETFN_AMI << 2 ) + 0x00 ), CMD_SET_SMTP_CONFIG_PARAMS,
		(uint8 *) &SetSMTPServerCfgReq, MAX_SMTP_AUTH_FLAG_LEN+4,
		(uint8 *) &SetSMTPServerCfgRes, &dwResLen, timeout );
	if(wRet != 0)
	{
		TCRIT("Error in setting SMTP Enable SSLTLS Support Flag.\n");
		return wRet;
	}



	return wRet;
}


/*uint16 IPMICMD_AMIGetSMTPInfo(IPMI20_SESSION_T *pSession,
					uint8 *Username,
					uint8 *passwd,
					uint8* SenderAddr,
					uint8* Machinename,
					uint8 Channel,
					int timeout)*/
uint16 IPMICMD_AMIGetSMTPInfo(IPMI20_SESSION_T *pSession,
		            SmtpInfoParam_T *SmtpInfo,
		            uint8 Channel,
		            int timeout)
{

	uint16 wRet;
	uint32 dwResLen;
	GetSMTPConfigReq_T GetSMTPServerCfgReq;
	GetSMTPConfigRes_T GetSMTPServerCfgRes;
	int       i 	= 0;

	memset(&GetSMTPServerCfgReq,0,sizeof(GetSMTPConfigReq_T));
	memset(&GetSMTPServerCfgRes,0,sizeof(GetSMTPConfigRes_T));

	GetSMTPServerCfgReq.Channel = Channel;
	GetSMTPServerCfgReq.ParameterSelect = OEM_ENABLE_DISABLE_SMTP;
	GetSMTPServerCfgReq.SetSelector = 0;
	GetSMTPServerCfgReq.BlockSelector = 0;
	dwResLen = MAX_SMTP_AUTH_FLAG_LEN + 1;
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
		 ( ( NETFN_AMI << 2 ) + 0x00 ), CMD_GET_SMTP_CONFIG_PARAMS,
		 (uint8 *) &GetSMTPServerCfgReq, sizeof(GetSMTPConfigReq_T),
		 (uint8 *) &GetSMTPServerCfgRes, &dwResLen, timeout );

	if(wRet != 0)
	{
		TCRIT("Getting SMTP Enable Flag Failed");
		return wRet;
	}

	SmtpInfo->EnableDisableSMTP = GetSMTPServerCfgRes.ConfigData.EnableDisableSMTP;

	memset(&GetSMTPServerCfgReq,0,sizeof(GetSMTPConfigReq_T));
	memset(&GetSMTPServerCfgRes,0,sizeof(GetSMTPConfigRes_T));

	GetSMTPServerCfgReq.Channel =Channel;
	GetSMTPServerCfgReq.ParameterSelect = OEM_PARAM_SMTP_USER_NAME;
	GetSMTPServerCfgReq.SetSelector = 0;
	GetSMTPServerCfgReq.BlockSelector =0;
	dwResLen = MAX_SMTP_USERNAME_LEN + 1;

	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
										 ( ( NETFN_AMI << 2 ) + 0x00 ), CMD_GET_SMTP_CONFIG_PARAMS,
										 (uint8 *) &GetSMTPServerCfgReq, sizeof(GetSMTPConfigReq_T),
										 (uint8 *) &GetSMTPServerCfgRes, &dwResLen, timeout );


	if(wRet != 0)
	{
		TCRIT("Getting Sender Address  failed %d\n",i);
		return wRet;
	}
	memcpy(SmtpInfo->UserName,GetSMTPServerCfgRes.ConfigData.UserName,MAX_SMTP_USERNAME_LEN);

	memset(&GetSMTPServerCfgReq,0,sizeof(GetSMTPConfigReq_T));
	memset(&GetSMTPServerCfgRes,0,sizeof(GetSMTPConfigRes_T));

	GetSMTPServerCfgReq.Channel =Channel;
	GetSMTPServerCfgReq.ParameterSelect = OEM_PARAM_SMTP_PASSWD;
	GetSMTPServerCfgReq.SetSelector = 0;
	GetSMTPServerCfgReq.BlockSelector =0;
	dwResLen = MAX_SMTP_PASSWD_LEN + 1;

	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
											 ( ( NETFN_AMI << 2 ) + 0x00 ), CMD_GET_SMTP_CONFIG_PARAMS,
											 (uint8 *) &GetSMTPServerCfgReq, sizeof(GetSMTPConfigReq_T),
											 (uint8 *) &GetSMTPServerCfgRes, &dwResLen, timeout );


	if(wRet != 0)
	{
		TCRIT("Getting Sender Address  failed %d\n",i);
		return wRet;
	}

	memcpy(SmtpInfo->Passwd,GetSMTPServerCfgRes.ConfigData.Passwd,MAX_SMTP_PASSWD_LEN);

	memset(&GetSMTPServerCfgReq,0,sizeof(GetSMTPConfigReq_T));
	memset(&GetSMTPServerCfgRes,0,sizeof(GetSMTPConfigRes_T));

	for(i = 0; i < MAX_EMAIL_ADDR_BLOCKS;i++)
	{

		GetSMTPServerCfgReq.Channel =Channel;
		GetSMTPServerCfgReq.ParameterSelect = OEM_PARAM_SMTP_SENDER_ADDR;
		GetSMTPServerCfgReq.SetSelector = 0;
		GetSMTPServerCfgReq.BlockSelector =i;
		dwResLen = MAX_EMAIL_BLOCK_SIZE + 1;

      	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
									 ( ( NETFN_AMI << 2 ) + 0x00 ), CMD_GET_SMTP_CONFIG_PARAMS,
									 (uint8 *) &GetSMTPServerCfgReq, sizeof(GetSMTPConfigReq_T),
									 (uint8 *) &GetSMTPServerCfgRes, &dwResLen, timeout );
		if(wRet != 0)
				{
				    TCRIT("Getting Sender Address  failed %d\n",i);
					return wRet;
				}

		memcpy(&SmtpInfo->SenderAddr[i*MAX_EMAIL_BLOCK_SIZE],GetSMTPServerCfgRes.ConfigData.SenderAddr,MAX_EMAIL_BLOCK_SIZE);
	}

	memset(&GetSMTPServerCfgReq,0,sizeof(GetSMTPConfigReq_T));
	memset(&GetSMTPServerCfgRes,0,sizeof(GetSMTPConfigRes_T));

	for(i = 0; i < MAX_SRV_NAME_BLOCKS;i++)
	{
		GetSMTPServerCfgReq.Channel =Channel;
		GetSMTPServerCfgReq.ParameterSelect = OEM_PARAM_SMTP_HOST_NAME;
		GetSMTPServerCfgReq.SetSelector = 0;
		GetSMTPServerCfgReq.BlockSelector =i;
		dwResLen = MAX_SRV_NAME_BLOCK_SIZE + 1;

		wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
										( ( NETFN_AMI << 2 ) + 0x00 ), CMD_GET_SMTP_CONFIG_PARAMS,
										(uint8 *) &GetSMTPServerCfgReq, sizeof(GetSMTPConfigReq_T),
										(uint8 *) &GetSMTPServerCfgRes, &dwResLen, timeout );

		if(wRet != 0)
		{
		    TCRIT("Getting Machine name   failed %d\n",i);
			return wRet;
		}
		memcpy(&SmtpInfo->MachineName[i*MAX_SRV_NAME_BLOCK_SIZE],GetSMTPServerCfgRes.ConfigData.Servername,MAX_SRV_NAME_BLOCK_SIZE);
	}

    memset(&GetSMTPServerCfgReq,0,sizeof(GetSMTPConfigReq_T));
	memset(&GetSMTPServerCfgRes,0,sizeof(GetSMTPConfigRes_T));

	GetSMTPServerCfgReq.Channel =Channel;
	GetSMTPServerCfgReq.ParameterSelect = OEM_ENABLE_DISABLE_SMTP_AUTH;
	GetSMTPServerCfgReq.SetSelector = 0;
	GetSMTPServerCfgReq.BlockSelector =0;
	dwResLen = MAX_SMTP_AUTH_FLAG_LEN + 1;
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
													 ( ( NETFN_AMI << 2 ) + 0x00 ), CMD_GET_SMTP_CONFIG_PARAMS,
													 (uint8 *) &GetSMTPServerCfgReq, sizeof(GetSMTPConfigReq_T),
													 (uint8 *) &GetSMTPServerCfgRes, &dwResLen, timeout );


	if(wRet != 0)
	{
		TCRIT("Getting SMTP Authenticaion Flag Failed");
		return wRet;
	}

	SmtpInfo->EnableDisableSmtpAuth=GetSMTPServerCfgRes.ConfigData.EnableDisableSmtpAuth;

	memset(&GetSMTPServerCfgReq,0,sizeof(GetSMTPConfigReq_T));
	memset(&GetSMTPServerCfgRes,0,sizeof(GetSMTPConfigRes_T));

	GetSMTPServerCfgReq.Channel =Channel;
	GetSMTPServerCfgReq.ParameterSelect = OEM_PARAM_SMTP_PORT;
	GetSMTPServerCfgReq.SetSelector = 0;
	GetSMTPServerCfgReq.BlockSelector =0;
	dwResLen = MAX_SMTP_PORT_SIZE + 1;
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
		 														 ( ( NETFN_AMI << 2 ) + 0x00 ), CMD_GET_SMTP_CONFIG_PARAMS,
		 														 (uint8 *) &GetSMTPServerCfgReq, sizeof(GetSMTPConfigReq_T),
		 														 (uint8 *) &GetSMTPServerCfgRes, &dwResLen, timeout );


	if(wRet != 0)
	{
		TCRIT("Getting SMTP Port Failed");
		return wRet;
	}

	SmtpInfo->SmtpPort=GetSMTPServerCfgRes.ConfigData.SmtpPort;
	memset(&GetSMTPServerCfgReq,0,sizeof(GetSMTPConfigReq_T));
	memset(&GetSMTPServerCfgRes,0,sizeof(GetSMTPConfigRes_T));

	GetSMTPServerCfgReq.Channel =Channel;
	GetSMTPServerCfgReq.ParameterSelect = OEM_PARAM_SMTP_SECURE_PORT;
	GetSMTPServerCfgReq.SetSelector = 0;
	GetSMTPServerCfgReq.BlockSelector =0;
	dwResLen = MAX_SMTP_PORT_SIZE + 1;
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
		 														 ( ( NETFN_AMI << 2 ) + 0x00 ), CMD_GET_SMTP_CONFIG_PARAMS,
		 														 (uint8 *) &GetSMTPServerCfgReq, sizeof(GetSMTPConfigReq_T),
		 														 (uint8 *) &GetSMTPServerCfgRes, &dwResLen, timeout );


	if(wRet != 0)
	{
		TCRIT("Getting SMTP Secure Port Failed");
		return wRet;
	}

	SmtpInfo->SmtpSecurePort=GetSMTPServerCfgRes.ConfigData.SmtpSecurePort;

	memset(&GetSMTPServerCfgReq,0,sizeof(GetSMTPConfigReq_T));
	memset(&GetSMTPServerCfgRes,0,sizeof(GetSMTPConfigRes_T));

	GetSMTPServerCfgReq.Channel = Channel;
	GetSMTPServerCfgReq.ParameterSelect = OEM_PARAM_ENABLE_SMTP_STARTTLS_SUPPORT;
	GetSMTPServerCfgReq.SetSelector = 0;
	GetSMTPServerCfgReq.BlockSelector = 0;
	dwResLen = MAX_SMTP_AUTH_FLAG_LEN + 1;
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
		 ( ( NETFN_AMI << 2 ) + 0x00 ), CMD_GET_SMTP_CONFIG_PARAMS,
		 (uint8 *) &GetSMTPServerCfgReq, sizeof(GetSMTPConfigReq_T),
		 (uint8 *) &GetSMTPServerCfgRes, &dwResLen, timeout );

	if(wRet != 0)
	{
		TCRIT("Getting SMTP Enable STARTTLS Support Flag Failed");
		return wRet;
	}

	SmtpInfo->EnableSTARTTLSSupport = GetSMTPServerCfgRes.ConfigData.EnableSTARTTLSSupport;
	memset(&GetSMTPServerCfgReq,0,sizeof(GetSMTPConfigReq_T));
	memset(&GetSMTPServerCfgRes,0,sizeof(GetSMTPConfigRes_T));

	GetSMTPServerCfgReq.Channel = Channel;
	GetSMTPServerCfgReq.ParameterSelect = OEM_PARAM_ENABLE_SMTP_SSLTLS_SUPPORT;
	GetSMTPServerCfgReq.SetSelector = 0;
	GetSMTPServerCfgReq.BlockSelector = 0;
	dwResLen = MAX_SMTP_AUTH_FLAG_LEN + 1;
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
			( ( NETFN_AMI << 2 ) + 0x00 ), CMD_GET_SMTP_CONFIG_PARAMS,
			(uint8 *) &GetSMTPServerCfgReq, sizeof(GetSMTPConfigReq_T),
			(uint8 *) &GetSMTPServerCfgRes, &dwResLen, timeout );

	if(wRet != 0)
	{
		TCRIT("Getting SMTP Enable SSLTLS Support Flag Failed");
		return wRet;
	}

	SmtpInfo->EnableSSLTLSSupport = GetSMTPServerCfgRes.ConfigData.EnableSSLTLSSupport;

	return wRet;

}
/*uint16 IPMICMD_AMIGetSMTP2Info(IPMI20_SESSION_T *pSession,
					uint8 *Username,
					uint8 *passwd,
					uint8* SenderAddr,
					uint8* Machinename,
					uint8 Channel,
					int timeout)*/
uint16 IPMICMD_AMIGetSMTP2Info(IPMI20_SESSION_T *pSession,
		            SmtpInfoParam_T *SmtpInfo,
		            uint8 Channel,
		            int timeout)
{

	uint16 wRet;
	uint32 dwResLen;
	GetSMTPConfigReq_T GetSMTPServerCfgReq;
	GetSMTPConfigRes_T GetSMTPServerCfgRes;
	int       i 	= 0;

	memset(&GetSMTPServerCfgReq,0,sizeof(GetSMTPConfigReq_T));
	memset(&GetSMTPServerCfgRes,0,sizeof(GetSMTPConfigRes_T));

	GetSMTPServerCfgReq.Channel = Channel;
	GetSMTPServerCfgReq.ParameterSelect = OEM_ENABLE_DISABLE_SMTP2;
	GetSMTPServerCfgReq.SetSelector = 0;
	GetSMTPServerCfgReq.BlockSelector = 0;
	dwResLen = MAX_SMTP_AUTH_FLAG_LEN + 1;
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
		 ( ( NETFN_AMI << 2 ) + 0x00 ), CMD_GET_SMTP_CONFIG_PARAMS,
		 (uint8 *) &GetSMTPServerCfgReq, sizeof(GetSMTPConfigReq_T),
		 (uint8 *) &GetSMTPServerCfgRes, &dwResLen, timeout );

	if(wRet != 0)
	{
		TCRIT("Getting SMTP Enable Flag Failed");
		return wRet;
	}

	SmtpInfo->EnableDisableSMTP = GetSMTPServerCfgRes.ConfigData.EnableDisableSMTP2;


	memset(&GetSMTPServerCfgReq,0,sizeof(GetSMTPConfigReq_T));
	memset(&GetSMTPServerCfgRes,0,sizeof(GetSMTPConfigRes_T));

	GetSMTPServerCfgReq.Channel =Channel;
	GetSMTPServerCfgReq.ParameterSelect = OEM_PARAM_SMTP_2_USER_NAME;
	GetSMTPServerCfgReq.SetSelector = 0;
	GetSMTPServerCfgReq.BlockSelector =0;
	dwResLen = MAX_SMTP_USERNAME_LEN + 1;

	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
										 ( ( NETFN_AMI << 2 ) + 0x00 ), CMD_GET_SMTP_CONFIG_PARAMS,
										 (uint8 *) &GetSMTPServerCfgReq, sizeof(GetSMTPConfigReq_T),
										 (uint8 *) &GetSMTPServerCfgRes, &dwResLen, timeout );


	if(wRet != 0)
	{
		TCRIT("Getting Sender Address  failed %d\n",i);
		return wRet;
	}
	memcpy(SmtpInfo->UserName,GetSMTPServerCfgRes.ConfigData.UserName2,MAX_SMTP_USERNAME_LEN);

	memset(&GetSMTPServerCfgReq,0,sizeof(GetSMTPConfigReq_T));
	memset(&GetSMTPServerCfgRes,0,sizeof(GetSMTPConfigRes_T));

	GetSMTPServerCfgReq.Channel =Channel;
	GetSMTPServerCfgReq.ParameterSelect = OEM_PARAM_SMTP_2_PASSWD;
	GetSMTPServerCfgReq.SetSelector = 0;
	GetSMTPServerCfgReq.BlockSelector =0;
	dwResLen = MAX_SMTP_PASSWD_LEN + 1;

	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
												 ( ( NETFN_AMI << 2 ) + 0x00 ), CMD_GET_SMTP_CONFIG_PARAMS,
												 (uint8 *) &GetSMTPServerCfgReq, sizeof(GetSMTPConfigReq_T),
												 (uint8 *) &GetSMTPServerCfgRes, &dwResLen, timeout );


	if(wRet != 0)
	{
		TCRIT("Getting Sender Address  failed %d\n",i);
		return wRet;
	}

	memcpy(SmtpInfo->Passwd,GetSMTPServerCfgRes.ConfigData.Passwd2,MAX_SMTP_PASSWD_LEN);

	memset(&GetSMTPServerCfgReq,0,sizeof(GetSMTPConfigReq_T));
	memset(&GetSMTPServerCfgRes,0,sizeof(GetSMTPConfigRes_T));

	for(i = 0; i < MAX_EMAIL_ADDR_BLOCKS;i++)
	{

		GetSMTPServerCfgReq.Channel =Channel;
		GetSMTPServerCfgReq.ParameterSelect = OEM_PARAM_SMTP_2_SENDER_ADDR;
		GetSMTPServerCfgReq.SetSelector = 0;
		GetSMTPServerCfgReq.BlockSelector =i;
		dwResLen = MAX_EMAIL_BLOCK_SIZE + 1;

      	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
									 ( ( NETFN_AMI << 2 ) + 0x00 ), CMD_GET_SMTP_CONFIG_PARAMS,
									 (uint8 *) &GetSMTPServerCfgReq, sizeof(GetSMTPConfigReq_T),
									 (uint8 *) &GetSMTPServerCfgRes, &dwResLen, timeout );
		if(wRet != 0)
		{
		    TCRIT("Getting Sender Address  failed %d\n",i);
			return wRet;
		}

		memcpy(&SmtpInfo->SenderAddr[i*MAX_EMAIL_BLOCK_SIZE],GetSMTPServerCfgRes.ConfigData.Sender2Addr,MAX_EMAIL_BLOCK_SIZE);
	}

	memset(&GetSMTPServerCfgReq,0,sizeof(GetSMTPConfigReq_T));
	memset(&GetSMTPServerCfgRes,0,sizeof(GetSMTPConfigRes_T));

	for(i = 0; i < MAX_SRV_NAME_BLOCKS;i++)
	{
		GetSMTPServerCfgReq.Channel =Channel;
		GetSMTPServerCfgReq.ParameterSelect = OEM_PARAM_SMTP_2_HOST_NAME;
		GetSMTPServerCfgReq.SetSelector = 0;
		GetSMTPServerCfgReq.BlockSelector =i;
		dwResLen = MAX_SRV_NAME_BLOCK_SIZE + 1;

		wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
											( ( NETFN_AMI << 2 ) + 0x00 ), CMD_GET_SMTP_CONFIG_PARAMS,
											(uint8 *) &GetSMTPServerCfgReq, sizeof(GetSMTPConfigReq_T),
											(uint8 *) &GetSMTPServerCfgRes, &dwResLen, timeout );

		if(wRet != 0)
		{
		    TCRIT("Getting Machine name   failed %d\n",i);
			return wRet;
		}
	    memcpy(&SmtpInfo->MachineName[i*MAX_SRV_NAME_BLOCK_SIZE],GetSMTPServerCfgRes.ConfigData.Server2name,MAX_SRV_NAME_BLOCK_SIZE);
	}

    memset(&GetSMTPServerCfgReq,0,sizeof(GetSMTPConfigReq_T));
	memset(&GetSMTPServerCfgRes,0,sizeof(GetSMTPConfigRes_T));

	GetSMTPServerCfgReq.Channel =Channel;
	GetSMTPServerCfgReq.ParameterSelect = OEM_ENABLE_DISABLE_SMTP_2_AUTH;
	GetSMTPServerCfgReq.SetSelector = 0;
	GetSMTPServerCfgReq.BlockSelector =0;
	dwResLen = MAX_SMTP_AUTH_FLAG_LEN + 1;
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
													 ( ( NETFN_AMI << 2 ) + 0x00 ), CMD_GET_SMTP_CONFIG_PARAMS,
													 (uint8 *) &GetSMTPServerCfgReq, sizeof(GetSMTPConfigReq_T),
													 (uint8 *) &GetSMTPServerCfgRes, &dwResLen, timeout );


	if(wRet != 0)
	{
		TCRIT("Getting SMTP Authenticaion Flag Failed");
		return wRet;
	}

	SmtpInfo->EnableDisableSmtpAuth=GetSMTPServerCfgRes.ConfigData.EnableDisableSmtp2Auth;

	memset(&GetSMTPServerCfgReq,0,sizeof(GetSMTPConfigReq_T));
	memset(&GetSMTPServerCfgRes,0,sizeof(GetSMTPConfigRes_T));

	GetSMTPServerCfgReq.Channel =Channel;
	GetSMTPServerCfgReq.ParameterSelect = OEM_PARAM_SMTP_2_PORT;
	GetSMTPServerCfgReq.SetSelector = 0;
	GetSMTPServerCfgReq.BlockSelector =0;
	dwResLen = MAX_SMTP_PORT_SIZE + 1;
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
		 														 ( ( NETFN_AMI << 2 ) + 0x00 ), CMD_GET_SMTP_CONFIG_PARAMS,
		 														 (uint8 *) &GetSMTPServerCfgReq, sizeof(GetSMTPConfigReq_T),
		 														 (uint8 *) &GetSMTPServerCfgRes, &dwResLen, timeout );


	if(wRet != 0)
	{
		TCRIT("Getting SMTP Port Failed");
		return wRet;
	}

	SmtpInfo->SmtpPort=GetSMTPServerCfgRes.ConfigData.Smtp2Port;
	memset(&GetSMTPServerCfgReq,0,sizeof(GetSMTPConfigReq_T));
	memset(&GetSMTPServerCfgRes,0,sizeof(GetSMTPConfigRes_T));

	GetSMTPServerCfgReq.Channel =Channel;
	GetSMTPServerCfgReq.ParameterSelect = OEM_PARAM_SMTP_2_SECURE_PORT;
	GetSMTPServerCfgReq.SetSelector = 0;
	GetSMTPServerCfgReq.BlockSelector =0;
	dwResLen = MAX_SMTP_PORT_SIZE + 1;
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
		 														 ( ( NETFN_AMI << 2 ) + 0x00 ), CMD_GET_SMTP_CONFIG_PARAMS,
		 														 (uint8 *) &GetSMTPServerCfgReq, sizeof(GetSMTPConfigReq_T),
		 														 (uint8 *) &GetSMTPServerCfgRes, &dwResLen, timeout );


	if(wRet != 0)
	{
		TCRIT("Getting SMTP Secure Port Failed");
		return wRet;
	}

	SmtpInfo->SmtpSecurePort=GetSMTPServerCfgRes.ConfigData.Smtp2SecurePort;


	memset(&GetSMTPServerCfgReq,0,sizeof(GetSMTPConfigReq_T));
	memset(&GetSMTPServerCfgRes,0,sizeof(GetSMTPConfigRes_T));

	GetSMTPServerCfgReq.Channel = Channel;
	GetSMTPServerCfgReq.ParameterSelect = OEM_PARAM_ENABLE_SMTP_2_STARTTLS_SUPPORT;
	GetSMTPServerCfgReq.SetSelector = 0;
	GetSMTPServerCfgReq.BlockSelector = 0;
	dwResLen = MAX_SMTP_AUTH_FLAG_LEN + 1;
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
		 ( ( NETFN_AMI << 2 ) + 0x00 ), CMD_GET_SMTP_CONFIG_PARAMS,
		 (uint8 *) &GetSMTPServerCfgReq, sizeof(GetSMTPConfigReq_T),
		 (uint8 *) &GetSMTPServerCfgRes, &dwResLen, timeout );

	if(wRet != 0)
	{
		TCRIT("Getting SMTP Enable STARTTLS Support Flag Failed");
		return wRet;
	}

	SmtpInfo->EnableSTARTTLSSupport = GetSMTPServerCfgRes.ConfigData.Smtp2EnableSTARTTLSSupport;
		memset(&GetSMTPServerCfgReq,0,sizeof(GetSMTPConfigReq_T));
		memset(&GetSMTPServerCfgRes,0,sizeof(GetSMTPConfigRes_T));

		GetSMTPServerCfgReq.Channel = Channel;
		GetSMTPServerCfgReq.ParameterSelect = OEM_PARAM_ENABLE_SMTP_2_SSLTLS_SUPPORT;
		GetSMTPServerCfgReq.SetSelector = 0;
		GetSMTPServerCfgReq.BlockSelector = 0;
		dwResLen = MAX_SMTP_AUTH_FLAG_LEN + 1;
		wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
				( ( NETFN_AMI << 2 ) + 0x00 ), CMD_GET_SMTP_CONFIG_PARAMS,
				(uint8 *) &GetSMTPServerCfgReq, sizeof(GetSMTPConfigReq_T),
				(uint8 *) &GetSMTPServerCfgRes, &dwResLen, timeout );

		if(wRet != 0)
		{
			TCRIT("Getting SMTP Enable SSLTLS Support Flag Failed");
			return wRet;
		}

		SmtpInfo->EnableSSLTLSSupport = GetSMTPServerCfgRes.ConfigData.Smtp2EnableSSLTLSSupport;




	return wRet;


}
uint16 IPMICMD_AMIGetEmailDest( IPMI20_SESSION_T *pSession,
								uint8	index,
								uint8 *Userid,
								char *Subject,
								char *Msg,
	                          			 uint8 Channel,
                             int timeout )
{
    uint16 wRet;
    uint32 dwResLen;
	GetSMTPConfigReq_T GetSMTPServerCfgReq;
	GetSMTPConfigRes_T GetSMTPServerCfgRes;
	int i = 0;
	char *tmp ;

	memset(&GetSMTPServerCfgReq,0,sizeof(GetSMTPConfigReq_T));
	memset(&GetSMTPServerCfgRes,0,sizeof(GetSMTPConfigRes_T));

	TDBG("index = %d\n",index);

	GetSMTPServerCfgReq.Channel =Channel;
	GetSMTPServerCfgReq.ParameterSelect = 5;
	GetSMTPServerCfgReq.SetSelector = index;
	GetSMTPServerCfgReq.BlockSelector = i;
	dwResLen =  sizeof (Userid)+1;


	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
											( ( NETFN_AMI << 2 ) + 0x00 ), CMD_GET_SMTP_CONFIG_PARAMS,
											(uint8 *) &GetSMTPServerCfgReq, sizeof(GetSMTPConfigReq_T),
											(uint8 *) &GetSMTPServerCfgRes, &dwResLen, timeout );

	*Userid=GetSMTPServerCfgRes.ConfigData.UserID;


	tmp = Subject;
	for(i = 0; i < MAX_SUB_BLOCKS; i++)
	{

		GetSMTPServerCfgReq.Channel =Channel;
		GetSMTPServerCfgReq.ParameterSelect = 6;
		GetSMTPServerCfgReq.SetSelector = index;
		GetSMTPServerCfgReq.BlockSelector = i;
		dwResLen = MAX_SUB_BLOCK_SIZE + 1;


		wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
												( ( NETFN_AMI << 2 ) + 0x00 ), CMD_GET_SMTP_CONFIG_PARAMS,
												(uint8 *) &GetSMTPServerCfgReq, sizeof(GetSMTPConfigReq_T),
												(uint8 *) &GetSMTPServerCfgRes, &dwResLen, timeout );
		if(wRet != 0)
		{
			TCRIT("Getting Subject failed %d\n",i);
			return wRet;
		}
		memcpy(tmp,GetSMTPServerCfgRes.ConfigData.Subject,MAX_SUB_BLOCK_SIZE);
		tmp += MAX_SUB_BLOCK_SIZE;
	}
	//printf("Subject : %s\n",Subject);
	//Subject[MAX_SUB_BLOCKS*MAX_SUB_BLOCK_SIZE] = 0;
	//printf("Subject : %s\n",Subject);

	tmp = Msg;
	for(i = 0; i < MAX_MSG_BLOCKS; i++)
	{

		GetSMTPServerCfgReq.Channel =Channel;
		GetSMTPServerCfgReq.ParameterSelect = 7;
		GetSMTPServerCfgReq.SetSelector = index;
		GetSMTPServerCfgReq.BlockSelector = i;
		dwResLen = MAX_MSG_BLOCK_SIZE + 1;

		wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
												( ( NETFN_AMI << 2 ) + 0x00 ), CMD_GET_SMTP_CONFIG_PARAMS,
												(uint8 *) &GetSMTPServerCfgReq, sizeof(GetSMTPConfigReq_T),
												(uint8 *) &GetSMTPServerCfgRes, &dwResLen, timeout );
		if(wRet != 0)
		{
			TCRIT("Getting msg failed %d\n",i);
			return wRet;
		}

		memcpy(tmp,GetSMTPServerCfgRes.ConfigData.Msg,MAX_MSG_BLOCK_SIZE);
		tmp += MAX_MSG_BLOCK_SIZE;
	}
	//Msg[MAX_MSG_BLOCKS*MAX_MSG_BLOCK_SIZE] = 0;

	TDBG("IPMICMD_AMIGetEmailDest:UserID: %x\n",Userid);
    return( wRet );
}


/**
*@fn IPMICMD_AMIGetChNum
*@brief
*@param pSession    [in]Session handle
*@param pAMIGetChNumReq Request data of AMIGetChNum
*@param pAMIGetChNumRes Response data of AMIGetChNum
*@param timeout timeout value in seconds.
*@param ReqLen Length of the Request Data
*@retval Returns LIBIPMI_STATUS_SUCCESS on success and error codes on failure
*/
uint16 IPMICMD_AMIGetChNum ( IPMI20_SESSION_T *pSession,
                                                     AMIGetChNumRes_T* pAMIGetChNumRes,
                                                     int timeout)
{
        uint16 wRet;
        uint32 dwResLen;
        uint8 *pReq = NULL;

        wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession,PAYLOAD_TYPE_IPMI,
                                                                       ((NETFN_AMI << 2) + 0x00), CMD_AMI_GET_CHANNEL_NUM,
                                                                       pReq,0,
                                                                       (uint8*)pAMIGetChNumRes,&dwResLen,
                                                                       timeout);
        return wRet;
}
/**
*@fn IPMICMD_AMIGetEthIndex
*@brief This Function Sends the raw command to get the Corresponding EthIndex
        for the Requested Channel Number
*@param pSession    [in]Session handle
*@param pAMIGetChNumReq Request data of AMIGetEthIndex
*@param pAMIGetChNumRes Response data of AMIGetEthIndex
*@param timeout timeout value in seconds.
*@param ReqLen Length of the Request Data
*@retval Returns LIBIPMI_STATUS_SUCCESS on success and error codes on failure
*/

uint16 IPMICMD_AMIGetEthIndex( IPMI20_SESSION_T *pSession,
                                                     AMIGetEthIndexReq_T *pGetEthIndexReq,
                                                     AMIGetEthIndexRes_T *pGetEthIndexRes,
                                                     int timeout )
{
    uint16 wRet;
    uint32 dwResLen;

    dwResLen = sizeof( AMIGetEthIndexRes_T );
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                                                                          ((NETFN_AMI << 2 ) + 0x00 ), CMD_AMI_GET_ETH_INDEX,
                                                                                          (uint8 *)pGetEthIndexReq, sizeof( *pGetEthIndexReq ),
                                                                                          (uint8 *)pGetEthIndexRes, &dwResLen, timeout );
       return( wRet );
}

uint16 LIBIPMI_HL_AMIGetEthIndex(IPMI20_SESSION_T *pSession,INT8U Channel,INT8U *EthIndex,int timeout)
{
    uint16 wRet;
    AMIGetEthIndexReq_T Req = {0};
    AMIGetEthIndexRes_T Res = {0,0} ;

    Req.ChannelNum = Channel;

    wRet = IPMICMD_AMIGetEthIndex(pSession,
                                  &Req,
                                  &Res,
                                  timeout );
    if(wRet != 0)
    {
        TCRIT("Error in Getting Ethindex for Channle :%d\n",Channel);
    }
    *EthIndex = Res.EthIndex;

    return wRet;

}
uint16 IPMICMD_AMIGetUserEmailFormat( IPMI20_SESSION_T *pSession,
				uint8 uid,
				char *pEmailformat,
				int timeout )
{
	uint16 wRet;
	uint32 dwResLen;

	AMIGetUserEmailFormatRes_T emailformat_res;

	dwResLen = sizeof( AMIGetUserEmailFormatRes_T );
	memset(&emailformat_res, 0, dwResLen);

	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                             ((NETFN_AMI << 2) + 0x00),
                                             CMD_AMI_GET_EMAILFORMAT_USER,
                                             (uint8 *)&uid, sizeof(uint8),
                                             (uint8 *)&emailformat_res, &dwResLen, timeout );

	if( wRet == 0)
	{
		memcpy(pEmailformat,emailformat_res.EMailFormat, EMAIL_FORMAT_SIZE);
		pEmailformat[EMAIL_FORMAT_SIZE - 1] = 0;
	}

	return( wRet );
}

uint16 IPMICMD_AMIGetUserEmail( IPMI20_SESSION_T *pSession,
				uint8 uid,
				char *pEmailAddr,
				int timeout )
{
	uint16 wRet;
	uint32 dwResLen;

	AMIGetUserEmailRes_T emailInfo_res;

	dwResLen = sizeof( AMIGetUserEmailRes_T );
	memset(&emailInfo_res, 0, dwResLen);

	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                             ((NETFN_AMI << 2) + 0x00), CMD_AMI_GET_EMAIL_USER,
                                             (uint8 *)&uid, sizeof(uint8),
                                             (uint8 *)&emailInfo_res, &dwResLen, timeout );

	if( wRet == 0)
	{
		if( EMAIL_ADDR_SIZE <= snprintf(pEmailAddr, EMAIL_ADDR_SIZE, emailInfo_res.EMailID ))/*Email_ID might be 64 bytes*/
			TCRIT("IPMICMD_AMIGetUserEmail - Source buffer is truncated.");
	}

	return( wRet );
}

uint16 IPMICMD_AMISetUserEmailFormat( IPMI20_SESSION_T *pSession,
				uint8 uid,
				char *pEmailFormat,
				int timeout )
{
	uint16 wRet;
	uint32 dwResLen;

	AMISetUserEmailFormatReq_T emailformat_req;
	AMISetUserEmailFormatRes_T emailformat_res;
	emailformat_req.UserID = uid;
	memset(emailformat_req.EMailFormat,0,EMAIL_FORMAT_SIZE);
	strncpy((char*)emailformat_req.EMailFormat, pEmailFormat,EMAIL_FORMAT_SIZE);
	emailformat_req.EMailFormat[EMAIL_FORMAT_SIZE - 1] = 0;

	dwResLen = sizeof( AMISetUserEmailFormatRes_T );
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                             ((NETFN_AMI << 2) + 0x00), CMD_AMI_SET_EMAILFORMAT_USER,
                                             (uint8 *)&emailformat_req, sizeof(AMISetUserEmailFormatReq_T),
                                             (uint8 *)&emailformat_res, &dwResLen, timeout );



	return( wRet );
}

uint16 IPMICMD_AMISetUserEmail( IPMI20_SESSION_T *pSession,
				uint8 uid,
				char *pEmailAddr,
				int timeout )
{
	uint16 wRet;
	uint32 dwResLen;

	AMISetUserEmailReq_T emailInfo_req;
	AMISetUserEmailRes_T emailInfo_res;
	emailInfo_req.UserID = uid;
	strncpy(emailInfo_req.EMailID, pEmailAddr, EMAIL_ADDR_SIZE);/*Email_ID might be 64 bytes*/

	dwResLen = sizeof( AMISetUserEmailRes_T );
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                             ((NETFN_AMI << 2) + 0x00), CMD_AMI_SET_EMAIL_USER,
                                             (uint8 *)&emailInfo_req, sizeof(AMISetUserEmailReq_T),
                                             (uint8 *)&emailInfo_res, &dwResLen, timeout );



	return( wRet );
}

uint16 IPMICMD_AMIResetUserPassword (IPMI20_SESSION_T *pSession,
					uint8 uid,
					uint8 channel,
					int timeout )
{

	uint16 wRet;
	AMIResetPasswordReq_T cmdReq;
	AMIResetPasswordRes_T cmdRes;
	uint32 dwResLen = sizeof( AMIResetPasswordRes_T );

	cmdReq.UserID = uid;
	cmdReq.Channel = channel;

	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
					((NETFN_AMI << 2) + 0x00), CMD_AMI_RESET_PASS,
					(uint8 *)&cmdReq, sizeof(AMIResetPasswordReq_T),
					(uint8 *)&cmdRes, &dwResLen, timeout );

	return( wRet );

}

uint16 IPMICMD_AMIRestoreFactoryDefaults (IPMI20_SESSION_T *pSession,
					int timeout )
{
	uint16 wRet;
	uint8 cmdRes;
	uint32 dwResLen = sizeof(uint8);
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
					((NETFN_AMI << 2) + 0x00), CMD_AMI_RESTORE_DEF,
					NULL, 0,
					(uint8 *)&cmdRes, &dwResLen, timeout );

	return( wRet );
}

/* To get the FRU details */
LIBIPMI_API uint16    IPMICMD_FRUDetails( IPMI20_SESSION_T *pSession,
                                    AMIGetFruDetailReq_T *pReqGetFruDet,
                                    AMIGetFruDetailRes_T *pResGetFruDet,
                                    int timeout)
{
    uint16    wRet;
    uint32    dwResLen;

    dwResLen = sizeof(AMIGetFruDetailRes_T);
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession,PAYLOAD_TYPE_IPMI ,
										((NETFN_AMI << 2) + 0x00), CMD_AMI_GET_FRU_DETAILS,
										(uint8*)pReqGetFruDet, sizeof(AMIGetFruDetailReq_T),
										(uint8 *)pResGetFruDet, &dwResLen,
										timeout);
    return wRet;
}

/*
*@fn IPMICMD_AMIGetRootUserAccess
*@brief This command helps in get linux root user access state
*		@param pReqGetRootAccess - Request for the command
*		@param pResGetRootAccess - Respose for the command
*		@param timeout           - timeout
*		@return Returns completion code
*/
uint16  IPMICMD_AMIGetRootUserAccess(IPMI20_SESSION_T *pSession,
				AMIGetRootUserAccessRes_T    *pResGetRootAccess,
				int timeout)
{
	uint16  wRet;
	uint32  dwResLen;
        uint8 *pReq = NULL;
	dwResLen = sizeof(AMIGetRootUserAccessRes_T);

	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
						((NETFN_AMI << 2) + 0x00), CMD_AMI_GET_ROOT_USER_ACCESS,
						pReq, 0,
						(uint8 *)pResGetRootAccess, &dwResLen,
						timeout);
	return wRet;
}

/*
*@fn IPMICMD_AMISetRootPassword
*@brief This command helps in set linux root user password
*		@param pSession    	   - IPMI SESSION
*		@param pReqSetRootPswd - Request for the command
*		@param pResSetRootPswd - Respose for the command
*		@param timeout         - Timeout
*		@return Returns completion code
*/
uint16  IPMICMD_AMISetRootPassword(IPMI20_SESSION_T *pSession,
				AMISetRootPasswordReq_T    *pReqSetRootPswd,
				AMISetRootPasswordRes_T    *pResSetRootPswd,
				int timeout)
{
	uint16  wRet;
	uint32  dwResLen;
	int ReqLen;

	dwResLen = sizeof(AMISetRootPasswordRes_T);
        ReqLen =  strlen((char *)pReqSetRootPswd->Password) + 1;     //Total Request Length Password Length and operation field(1 byte)

 	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
						((NETFN_AMI << 2) + 0x00), CMD_AMI_SET_ROOT_PASSWORD,
						(uint8*)pReqSetRootPswd, ReqLen,
						(uint8 *)pResSetRootPswd, &dwResLen,
						timeout);

return wRet;
}


/*
*@fn LIBIPMI_HL_AMIGetRootUserAccess
*@brief This command helps in get linux root user access state
*		@param pSession        - IPMI SESSION
*		@param UserID   	   - User ID
*		@param UserAccessState - Access State of the user
*		@param timeout         - Timeout
*		@return Returns completion code
*/
uint16 LIBIPMI_HL_AMIGetRootUserAccess( IPMI20_SESSION_T *pSession,
				INT8U UserID,INT8U *UserAccessState,
				int timeout )
{
	uint16 wRet;

	AMIGetRootUserAccessRes_T    ResGetRootAccess = {0,0};
	if(0)
	{
		UserID=UserID;  /*-Wextra: Flag added for strict compilation.*/
	}

	wRet = IPMICMD_AMIGetRootUserAccess(pSession,&ResGetRootAccess,timeout);
	if(wRet != 0)
	{
		TCRIT("Error getting Linux Root user access..\n");
		return wRet;
	}
	*UserAccessState = ResGetRootAccess.CurrentUserIDState;

return 0;
}

/*
*@fn LIBIPMI_HL_AMISetRootPassword
*@brief This command helps in set linux root user password
*		@param pSession    - IPMI SESSION
*		@param UserID      - User ID
*		@param IsPwdChange - Requested operation
*		@param ClearPswd   - Password of the root to modifie
*		@param EnableUser  - Requested operation
*		@param timeout      - Timeout
*		@return Returns completion code
*/
uint16 LIBIPMI_HL_AMISetRootPassword( IPMI20_SESSION_T *pSession,
					INT8U UserID,INT8U IsPwdChange,
					char* ClearPswd,INT8U EnableUser,
					int timeout )
{
	uint16 wRet;

	AMISetRootPasswordReq_T    ReqSetRootPswd = {0,"\0"};
	AMISetRootPasswordRes_T    ResSetRootPswd = {0};
	if(0)
	{
		UserID=UserID;  /*-Wextra: Flag added for strict compilation.*/
	}

	if(EnableUser)
	{
		TDBG("Doing a enable user\n");

		ReqSetRootPswd.Operation = 0x1;//Enable Root user operation code
		wRet = IPMICMD_AMISetRootPassword(pSession,&ReqSetRootPswd,&ResSetRootPswd,timeout);
		if(wRet != 0)
		{
			TCRIT("Error setting Linux Root user password..enable operation\n");
			return wRet;
		}
		if(IsPwdChange)
		{
			ReqSetRootPswd.Operation = 0x02; //for set password operation code
			strncpy((char*)ReqSetRootPswd.Password,(char*)ClearPswd,MAX_LINUX_ROOT_USER_PASSWORD_LEN);
			wRet = IPMICMD_AMISetRootPassword(pSession,&ReqSetRootPswd,&ResSetRootPswd,timeout);
			if(wRet != 0)
			{
				TCRIT("Error setting Linux Root user password..Set password operation\n");
				return wRet;
			}
		}
		else
		{
			TDBG("Skipping pswd change..no password change\n");
		}
	}
	else
	{
		TDBG("Doing a disable user\n");


		ReqSetRootPswd.Operation = 0x0;//Disable Root user operation code
		wRet = IPMICMD_AMISetRootPassword(pSession,&ReqSetRootPswd,&ResSetRootPswd,timeout);
		if(wRet != 0)
		{
			TCRIT("Error setting Linux Root user password..disable operation\n");
			return wRet;
		}
	}

return 0;
}

uint16 IPMICMD_Set_DNS_Conf( IPMI20_SESSION_T *pSession,
                             AMISetDNSConfReq_T* pSetDNSConfReq,
                             unsigned int ReqDataLen,
                             AMISetDNSConfRes_T *pSetDNSConfRes,
                             int timeout )
{
    uint16 wRet;
    uint32 dwResLen;
    dwResLen = sizeof(AMISetDNSConfRes_T);

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                                                                            (NETFN_AMI << 2), CMD_AMI_SET_DNS_CONF,
                                                                            (uint8*)pSetDNSConfReq, ReqDataLen,
                                                                            (uint8*)pSetDNSConfRes, &dwResLen,
                                                                            timeout);
    if(wRet != 0)
    {
        TCRIT("Error in Setting DNS Configuration in raw IPMI command layer::%d\n", wRet);
    }
    return (wRet);
}

uint16 IPMICMD_Get_DNS_Conf( IPMI20_SESSION_T *pSession,
                             AMIGetDNSConfReq_T *pGetDNSConfReq,
                             AMIGetDNSConfRes_T *pGetDNSConfRes,
                             uint32* ResLen,
                             int timeout )
{
    uint16 wRet;
    uint32 dwResLen;
    dwResLen = sizeof(AMIGetDNSConfRes_T);

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                        (NETFN_AMI << 2), CMD_AMI_GET_DNS_CONF,
                        (uint8*)pGetDNSConfReq, sizeof(AMIGetDNSConfReq_T),
                        (uint8*)pGetDNSConfRes, &dwResLen,
                        timeout);
    if(wRet != 0)
    {
        TCRIT("Error in getting DNS configuration in raw IPMI command layer::%d\n", wRet);
    }

    *ResLen = dwResLen;
    return (wRet);
}

uint16 IPMICMD_AMIGetChannelType(IPMI20_SESSION_T *pSession, AMIGetChannelTypeReq_T *pGetChannelTypeReq,
                                                 AMIGetChannelTypeRes_T *pGetChannelTypeRes, int timeout)
{
    uint16 wRet;
    uint32 dwResLen = sizeof(AMIGetChannelTypeRes_T);

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                                                      (NETFN_AMI << 2), CMD_AMI_GET_CHANNEL_TYPE,
                                                      (uint8*)pGetChannelTypeReq, sizeof(AMIGetChannelTypeReq_T),
                                                      (uint8*)pGetChannelTypeRes, &dwResLen,
                                                      timeout);

    return wRet;
}

uint16 LIBIPMI_HL_AMISetDNSHostSetting(IPMI20_SESSION_T *pSession,INT8U* HostSetting,int timeout)
{
    uint16 wRet;
    AMISetDNSConfReq_T SetDNSConfReq;
    AMISetDNSConfRes_T SetDNSConfRes;
    unsigned int ReqDataLen = 0;

    memset(&SetDNSConfReq,0,sizeof(AMISetDNSConfReq_T));

    SetDNSConfReq.ParamSelect = AMI_DNS_CONF_HOST_NAME;
    memcpy(&SetDNSConfReq.DnsConfig.HName,HostSetting,sizeof(HOSTNAMECONF));

    ReqDataLen = 2 + sizeof(HOSTNAMECONF);
    wRet = IPMICMD_Set_DNS_Conf(pSession,&SetDNSConfReq,ReqDataLen,&SetDNSConfRes,timeout);
    if(wRet != 0)
    {
        TCRIT("Error in Setting DNS Host Configuration\n");
    }
    return wRet;
}

uint16 LIBIPMI_HL_AMIGetDNSServiceStatus(IPMI20_SESSION_T *pSession,INT8U *Status, int timeout)
{
    uint16 wRet;
    AMIGetDNSConfReq_T GetDNSConfReq;
    AMIGetDNSConfRes_T GetDNSConfRes;
    uint32 ResLen = 0;

    memset(&GetDNSConfReq, 0, sizeof(AMIGetDNSConfReq_T));

    GetDNSConfReq.Param = AMI_DNS_CONF_DNS_ENABLE;
    GetDNSConfReq.Blockselect = 0;

    wRet = IPMICMD_Get_DNS_Conf(pSession, &GetDNSConfReq, &GetDNSConfRes, &ResLen, timeout);
    if(wRet != 0)
    {
        TCRIT("Error in getting DNS Service Configuration::%x\n", wRet);
    }
    *Status = GetDNSConfRes.DNSCfg.DNSEnable;
    return wRet;
}

uint16 LIBIPMI_HL_AMISetDNSServiceStatus(IPMI20_SESSION_T *pSession,INT8U Status, int timeout)
{
    uint16 wRet;
    AMISetDNSConfReq_T SetDNSConfReq;
    AMISetDNSConfRes_T SetDNSConfRes;
    unsigned int ReqDataLen = 0;

    memset(&SetDNSConfReq, 0, sizeof(AMISetDNSConfReq_T));
    SetDNSConfReq.ParamSelect = AMI_DNS_CONF_DNS_ENABLE;
    SetDNSConfReq.Blockselector = 0;
    SetDNSConfReq.DnsConfig.DNSEnable = Status;
    ReqDataLen = 2 + 1; /* 1 is for DNS Service Status */
    wRet = IPMICMD_Set_DNS_Conf(pSession, &SetDNSConfReq, ReqDataLen, &SetDNSConfRes, timeout);
    if(wRet != 0)
    {
        TCRIT("Error in Setting DNS Service Configuration::%x\n", wRet);
    }
    return wRet;
}


uint16 LIBIPMI_HL_AMIGetDNSHostSetting(IPMI20_SESSION_T *pSession,INT8U*HostSetting,int timeout)
{
    uint16 wRet;
    AMIGetDNSConfReq_T GetDNSConfReq;
    AMIGetDNSConfRes_T GetDNSConfRes;
    uint32 ResLen= 0 ;

    memset(&GetDNSConfReq,0,sizeof(AMIGetDNSConfReq_T));

    GetDNSConfReq.Param = AMI_DNS_CONF_HOST_NAME;
    GetDNSConfReq.Blockselect = 0;

    wRet = IPMICMD_Get_DNS_Conf(pSession,&GetDNSConfReq,&GetDNSConfRes,&ResLen,timeout);
    if(wRet != 0)
    {
        TCRIT("Error in getting DNS Host Configuration\n");
    }

    memcpy(HostSetting,&GetDNSConfRes.DNSCfg.HName,sizeof(HOSTNAMECONF));
    return wRet;
}

uint16 LIBIPMI_HL_AMISetDNSRegister(IPMI20_SESSION_T *pSession,INT8U* RegisterBMC,int timeout)
{
    uint16 wRet;
    AMISetDNSConfReq_T SetDNSConfReq;
    AMISetDNSConfRes_T SetDNSConfRes;
    unsigned int ReqDataLen = 0;

    memset(&SetDNSConfReq,0,sizeof(AMISetDNSConfReq_T));

    SetDNSConfReq.ParamSelect = AMI_DNS_CONF_REGISTER;
    memcpy(SetDNSConfReq.DnsConfig.RegDNSConf, RegisterBMC,MAX_CHANNEL);

    ReqDataLen = 2 + (MAX_CHANNEL - 1); /*This check will be removed once we added the support for 4 NIC*/
    wRet = IPMICMD_Set_DNS_Conf(pSession,&SetDNSConfReq,ReqDataLen,&SetDNSConfRes,timeout);
    if(wRet != 0)
    {
        TCRIT("Error in Setting DNS Register Configuration\n");
    }

    return wRet;
}

uint16 LIBIPMI_HL_AMIGetDNSRegister(IPMI20_SESSION_T *pSession,INT8U* RegisterBMC,int timeout)
{
    uint16 wRet;
    AMIGetDNSConfReq_T GetDNSConfReq;
    AMIGetDNSConfRes_T GetDNSConfRes;
    uint32 ResLen= 0 ;

    memset(&GetDNSConfReq,0,sizeof(AMIGetDNSConfReq_T));

    GetDNSConfReq.Param = AMI_DNS_CONF_REGISTER;
    GetDNSConfReq.Blockselect = 0;

    wRet = IPMICMD_Get_DNS_Conf(pSession,&GetDNSConfReq,&GetDNSConfRes,&ResLen,timeout);
    if(wRet != 0)
    {
        TCRIT("Error in getting DNS Register Configuration\n");
    }

    memcpy(RegisterBMC,GetDNSConfRes.DNSCfg.RegDNSConf,MAX_CHANNEL);
    return wRet;
}

uint16 LIBIPMI_HL_AMISetDNSDomainSetting(IPMI20_SESSION_T *pSession,INT8U *DomainConf,int timeout)
{
    uint16 wRet;
    AMISetDNSConfReq_T SetDNSConfReq;
    AMISetDNSConfRes_T SetDNSConfRes;
    unsigned int ReqDataLen = 0;

    memset(&SetDNSConfReq,0,sizeof(AMISetDNSConfReq_T));

    SetDNSConfReq.ParamSelect = AMI_DNS_CONF_DOMAIN_SETTINGS;
    memcpy(&SetDNSConfReq.DnsConfig.DomainConf,DomainConf,sizeof(DomainSetting));

    ReqDataLen = 2 + sizeof(DomainSetting);
    wRet = IPMICMD_Set_DNS_Conf(pSession,&SetDNSConfReq,ReqDataLen,&SetDNSConfRes,timeout);
    if(wRet != 0)
    {
        TCRIT("Error in Setting DNS Domain settings Configuration\n");
    }

    return wRet;
}

uint16 LIBIPMI_HL_AMIGetDNSDomainSetting(IPMI20_SESSION_T *pSession,INT8U *DomainConf,int timeout)
{
    uint16 wRet;
    AMIGetDNSConfReq_T GetDNSConfReq;
    AMIGetDNSConfRes_T GetDNSConfRes;
    uint32 ResLen= 0 ;

    memset(&GetDNSConfReq,0,sizeof(AMIGetDNSConfReq_T));

    GetDNSConfReq.Param = AMI_DNS_CONF_DOMAIN_SETTINGS;
    GetDNSConfReq.Blockselect = 0;

    wRet = IPMICMD_Get_DNS_Conf(pSession,&GetDNSConfReq,&GetDNSConfRes,&ResLen,timeout);
    if(wRet != 0)
    {
        TCRIT("Error in getting DNS Host Configuration\n");
    }

    memcpy(DomainConf,&GetDNSConfRes.DNSCfg.DomainConf,sizeof(DomainSetting));
    return wRet;
}

uint16 LIBIPMI_HL_AMISetDNSDomainName(IPMI20_SESSION_T *pSession,INT8U Block,INT8U *DomainName,int timeout)
{
    uint16 wRet;
    AMISetDNSConfReq_T SetDNSConfReq;
    AMISetDNSConfRes_T SetDNSConfRes;
    unsigned int ReqDataLen = 0;

    memset(&SetDNSConfReq,0,sizeof(AMISetDNSConfReq_T));

    SetDNSConfReq.ParamSelect = AMI_DNS_CONF_DOMAIN_NAME;
    SetDNSConfReq.Blockselector = Block;
    memcpy(SetDNSConfReq.DnsConfig.DomainName,DomainName,MAX_DOMAIN_BLOCK_SIZE);

    ReqDataLen = 2 + MAX_DOMAIN_BLOCK_SIZE;
    wRet = IPMICMD_Set_DNS_Conf(pSession,&SetDNSConfReq,ReqDataLen,&SetDNSConfRes,timeout);
    if(wRet != 0)
    {
        TCRIT("Error in Setting DNS Domain Name Configuration\n");
    }

    return wRet;
}

uint16 LIBIPMI_HL_AMIGetDNSDomainName(IPMI20_SESSION_T *pSession,INT8U Block,INT8U *DomainName,int timeout)
{
    uint16 wRet;
    AMIGetDNSConfReq_T GetDNSConfReq;
    AMIGetDNSConfRes_T GetDNSConfRes;
    uint32 ResLen= 0 ;

    memset(&GetDNSConfReq,0,sizeof(AMIGetDNSConfReq_T));

    GetDNSConfReq.Param = AMI_DNS_CONF_DOMAIN_NAME;
    GetDNSConfReq.Blockselect = Block;

    wRet = IPMICMD_Get_DNS_Conf(pSession,&GetDNSConfReq,&GetDNSConfRes,&ResLen,timeout);
    if(wRet != 0)
    {
        TCRIT("Error in getting DNS Host Configuration\n");
    }

    memcpy(DomainName,GetDNSConfRes.DNSCfg.DomainName,MAX_DOMAIN_BLOCK_SIZE);
    return wRet;
}

uint16 LIBIPMI_HL_AMISetDNSSetting(IPMI20_SESSION_T *pSession,INT8U* DNSConf,int timeout)
{
    uint16 wRet;
    AMISetDNSConfReq_T SetDNSConfReq;
    AMISetDNSConfRes_T SetDNSConfRes;
    unsigned int ReqDataLen = 0;

    memset(&SetDNSConfReq,0,sizeof(AMISetDNSConfReq_T));

    SetDNSConfReq.ParamSelect = AMI_DNS_CONF_DNS_SETTING;
    memcpy(&SetDNSConfReq.DnsConfig.DNSConf,DNSConf,sizeof(DNSSetting));

    ReqDataLen = 2 + sizeof(DNSSetting);
    wRet = IPMICMD_Set_DNS_Conf(pSession,&SetDNSConfReq,ReqDataLen,&SetDNSConfRes,timeout);
    if(wRet != 0)
    {
        TCRIT("Error in Setting DNS Host Configuration\n");
    }

    return wRet;
}

uint16 LIBIPMI_HL_AMIGetDNSSetting(IPMI20_SESSION_T *pSession,INT8U* DNSConf,int timeout)
{
    uint16 wRet;
    AMIGetDNSConfReq_T GetDNSConfReq;
    AMIGetDNSConfRes_T GetDNSConfRes;
    uint32 ResLen= 0 ;

    memset(&GetDNSConfReq,0,sizeof(AMIGetDNSConfReq_T));

    GetDNSConfReq.Param = AMI_DNS_CONF_DNS_SETTING;
    GetDNSConfReq.Blockselect = 0;

    wRet = IPMICMD_Get_DNS_Conf(pSession,&GetDNSConfReq,&GetDNSConfRes,&ResLen,timeout);
    if(wRet != 0)
    {
        TCRIT("Error in getting DNS Host Configuration\n");
    }

    memcpy(DNSConf,&GetDNSConfRes.DNSCfg.DNSConf,sizeof(DNSSetting));
    return wRet;
}

uint16 LIBIPMI_HL_AMISetDNSIPAddress(IPMI20_SESSION_T *pSession,INT8U Block,INT8U *IPAddr, INT8U AddrLen,int timeout)
{
    uint16 wRet;
    AMISetDNSConfReq_T SetDNSConfReq;
    AMISetDNSConfRes_T SetDNSConfRes;
    unsigned int ReqDataLen = 0;

    memset(&SetDNSConfReq,0,sizeof(AMISetDNSConfReq_T));

    SetDNSConfReq.ParamSelect = AMI_DNS_CONF_DNS_IP;
    SetDNSConfReq.Blockselector = Block;
    memcpy(SetDNSConfReq.DnsConfig.DNSIPAddr,IPAddr,AddrLen);

    ReqDataLen = 2 + AddrLen;
    wRet = IPMICMD_Set_DNS_Conf(pSession,&SetDNSConfReq,ReqDataLen,&SetDNSConfRes,timeout);
    if(wRet != 0)
    {
        TCRIT("Error in Setting DNS Host Configuration\n");
    }

    return wRet;
}

uint16 LIBIPMI_HL_AMIGetDNSIPAddress(IPMI20_SESSION_T *pSession,INT8U Block,INT8U *IPAddr,INT8U* AddrLen,int timeout)
{
    uint16 wRet;
    AMIGetDNSConfReq_T GetDNSConfReq;
    AMIGetDNSConfRes_T GetDNSConfRes;
    uint32 ResLen= 0 ;

    memset(&GetDNSConfReq,0,sizeof(AMIGetDNSConfReq_T));

    GetDNSConfReq.Param = AMI_DNS_CONF_DNS_IP;
    GetDNSConfReq.Blockselect = Block;

    wRet = IPMICMD_Get_DNS_Conf(pSession,&GetDNSConfReq,&GetDNSConfRes,&ResLen,timeout);
    if(wRet != 0)
    {
        TCRIT("Error in getting DNS Host Configuration\n");
    }

    memcpy(IPAddr,GetDNSConfRes.DNSCfg.DNSIPAddr,ResLen -1);
    *AddrLen = (INT8U)(ResLen -1);
    return wRet;
}

uint16 LIBIPMI_HL_AMISetDNSRestart(IPMI20_SESSION_T *pSession,int timeout)
{
    uint16 wRet;
    AMISetDNSConfReq_T SetDNSConfReq;
    AMISetDNSConfRes_T SetDNSConfRes;
    unsigned int ReqDataLen = 0;

    memset(&SetDNSConfReq,0,sizeof(AMISetDNSConfReq_T));

    SetDNSConfReq.ParamSelect = AMI_DNS_CONF_DNS_RESTART;

    ReqDataLen = 2;
    wRet = IPMICMD_Set_DNS_Conf(pSession,&SetDNSConfReq,ReqDataLen,&SetDNSConfRes,timeout);
    if(wRet != 0)
    {
        TCRIT("Error in Setting DNS Host Configuration\n");
    }

    return wRet;
}


uint16 IPMICMD_AMISetServiceConf(IPMI20_SESSION_T *pSession, void* p_SetServiceConf_Req
                                               , void* p_SetServiceConf_Res, int timeout)
{
    uint16 wRet;
    uint32 dwResLen;
    AMISetServiceConfReq_T	*pAMISetServiceConfReq = (AMISetServiceConfReq_T *)(p_SetServiceConf_Req);
    AMISetServiceConfRes_T	*pAMISetServiceConfRes = (AMISetServiceConfRes_T *)(p_SetServiceConf_Res);

    dwResLen = sizeof(AMISetServiceConfRes_T);

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                        (NETFN_AMI << 2), CMD_AMI_SET_SERVICE_CONF,
                        (uint8 *)pAMISetServiceConfReq, sizeof(AMISetServiceConfReq_T),
                        (uint8*)pAMISetServiceConfRes, &dwResLen,
                        timeout);

    if(wRet != 0)
    {
        TCRIT("Error in Setting Service Configuration in raw IPMI command layer::%d\n", wRet);
    }
    return (wRet);

}
uint16 IPMICMD_AMIGetServiceConf(IPMI20_SESSION_T *pSession, void* p_GetServiceConf_Req
                                              , void* p_GetServiceConf_Res, int timeout)
{
    uint16 wRet;
    uint32 dwResLen;
    AMIGetServiceConfReq_T* pAMIGetServiceConfReq = (AMIGetServiceConfReq_T *)(p_GetServiceConf_Req);
    AMIGetServiceConfRes_T* pAMIGetServiceConfRes = (AMIGetServiceConfRes_T *)(p_GetServiceConf_Res);

    dwResLen = sizeof(AMIGetServiceConfRes_T);

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                        (NETFN_AMI << 2), CMD_AMI_GET_SERVICE_CONF,
                        (uint8 *)pAMIGetServiceConfReq, sizeof(AMIGetServiceConfReq_T),
                        (uint8*)pAMIGetServiceConfRes, &dwResLen,
                        timeout);

    if(wRet != 0)
    {
        TCRIT("Error in Get Service Configuration in raw IPMI command layer::%d\n", wRet);
    }
    return (wRet);
}

uint16 LIBIPMI_HL_AMISetServiceConf(IPMI20_SESSION_T *pSession, AMISetServiceConfReq_T *pSetServiceCfgReq, int timeout)
{
    uint16 wRet;
    AMISetServiceConfReq_T pSetServiceConfReq;
    AMISetServiceConfRes_T Res = {0};

    memset (&pSetServiceConfReq, 0, sizeof (AMISetServiceConfReq_T));
    memcpy (&pSetServiceConfReq, pSetServiceCfgReq, sizeof (AMISetServiceConfReq_T));
    wRet =  IPMICMD_AMISetServiceConf(pSession,
                             &pSetServiceConfReq,
                             &Res,
                             timeout);

    if(wRet != 0)
    {
        TCRIT("Error While Setting service configuration %d\n",wRet);
        return wRet;
    }
    return (uint16)Res.CompletionCode;
}

uint16 LIBIPMI_HL_AMIGetServiceConf(IPMI20_SESSION_T *pSession,INT32U ServiceID, AMIGetServiceConfRes_T *pAMIGetServiceCfgRes, int timeout)
{
    uint16 wRet;
    AMIGetServiceConfRes_T pAMIGetServiceConfRes;
    AMIGetServiceConfReq_T pAMIGetServiceConfReq = {0};

    memset (&pAMIGetServiceConfRes, 0 , sizeof (AMIGetServiceConfRes_T));
    pAMIGetServiceConfReq.ServiceID = ServiceID;
    wRet =  IPMICMD_AMIGetServiceConf(pSession,
                             &pAMIGetServiceConfReq,
                             &pAMIGetServiceConfRes, timeout );

    if(wRet != 0)
    {
        TCRIT("Error While Getting service configuration %d\n",wRet);
        return wRet;
    }
     memcpy(pAMIGetServiceCfgRes, &pAMIGetServiceConfRes, sizeof(AMIGetServiceConfRes_T));
    return wRet;
}


uint16 LIBIPMI_HL_GetAllSeviceConf (IPMI20_SESSION_T *pSession, GetServiceInfo_T *GetServiceInfo, int timeout)
{
    uint16 wRet;
    AMIGetServiceConfRes_T pAMIGetServiceConfRes;
    AMIGetServiceConfReq_T pAMIGetServiceConfReq = {0};
    /*Don't Modify this Value*/
    INT32U ServiceID = 1;
    INT8U Index = 0;
    INT8U serviceCnt = 0;
    char *ServiceNameList[MAX_SERVICES] = {
                                              WEB_SERVICE_NAME,
                                              //KVM_SERVICE_NAME,//Quanat--
                                              //CDMEDIA_SERVICE_NAME,//Quanat--
                                              //FDMEDIA_SERVICE_NAME,//Quanat--
                                              //HDMEDIA_SERVICE_NAME,//Quanat--
                                              SSH_SERVICE_NAME,
                                              //TELNET_SERVICE_NAME,  //Quanat--
                                              //SOLSSH_SERVICE_NAME,  //Quanta--
                                          };


    for (Index = 0;Index < MAX_SERVICES; Index++)
    {
        memset (&pAMIGetServiceConfRes, 0 , sizeof (AMIGetServiceConfRes_T));
        pAMIGetServiceConfReq.ServiceID = (ServiceID << Index);
        wRet =  IPMICMD_AMIGetServiceConf(pSession,
                                 &pAMIGetServiceConfReq,
                                 &pAMIGetServiceConfRes, timeout );

        if(wRet != 0)
        {
            TCRIT("Error While Getting service configuration %d\n",wRet);
            if (wRet == STATUS_CODE(IPMI_ERROR_FLAG, CC_SERVICE_ABSENT))
            	continue;
            else
                return wRet;
        }

        memcpy(&GetServiceInfo->ServiceInfo [serviceCnt].GetAllSeviceCfg, &pAMIGetServiceConfRes, sizeof(AMIGetServiceConfRes_T));
        strncpy (GetServiceInfo->ServiceInfo [serviceCnt].ServiceName, ServiceNameList [Index],SERVICE_NAME_SIZE-1);
        serviceCnt++;
    }
    GetServiceInfo->ServiceCnt = serviceCnt;
    return 0;
}

/*
*@fn IPMICMD_AMIGetSOLConf
*@brief This command helps in getting SOL Configurations
*		@param pResGetSOLConf - Respose for the command
*		@param timeout           - timeout
*		@return Returns completion code
*/
uint16  IPMICMD_AMIGetSOLConf(IPMI20_SESSION_T *pSession,
				AMIGetSOLConfRes_T    *pResGetSOLConf,
				int timeout)
{
	uint16  wRet;
	uint32  dwResLen;
    uint8 *pReq = NULL;
	dwResLen = sizeof(AMIGetSOLConfRes_T);

	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
						((NETFN_AMI << 2) + 0x00), CMD_AMI_GET_SOL_CONFIG_PARAMS,
						pReq, 0,
						(uint8 *)pResGetSOLConf, &dwResLen,
						timeout);
	return wRet;
}

/*
*@fn IPMICMD_AMISetPamOrder
*@brief This command set the PAM Order (ipmi, ldap, ad).
*@param pSession - IPMI SESSION
*@param pAMISetPamReq - PAM order to set
*@param pAMISetPamRes - response code
*@param timeout     - Timeout
*@return Returns completion code
*/
uint16 IPMICMD_AMISetPamOrder(IPMI20_SESSION_T *pSession,
                           AMISetPamReq_T    *pAMISetPamReq,
                           int count,
                           AMISetPamRes_T *pAMISetPamRes,
                           int timeout)
{
    uint16 wRet;
    uint32 dwResLen;
    dwResLen = sizeof(AMISetPamRes_T);
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                        (NETFN_AMI << 2), CMD_AMI_SET_PAM_ORDER,
                        (uint8 *)pAMISetPamReq, count,
                        (uint8*)pAMISetPamRes, &dwResLen,
                        timeout);

    if(wRet != 0)
    {
        TCRIT("Error in Setting PAM Order in raw IPMI command layer::%d\n", wRet);
    }
    return (wRet);
}

/*
*@fn LIBIPMI_HL_AMISetPamOrder
*@brief This command set the PAM Order (ipmi, ldap, ad).
*@param pSession - IPMI SESSION
*@param pAMISetPamReq - PAM order to set
*@param timeout     - Timeout
*@return Returns completion code
*/
uint16 LIBIPMI_HL_AMISetPamOrder (IPMI20_SESSION_T *pSession, AMISetPamReq_T *pAMISetPamReq, int count, int timeout)
{
    uint16 wRet;
    AMISetPamRes_T Res = {0};

    wRet =  IPMICMD_AMISetPamOrder (pSession,
                             pAMISetPamReq,
                             count,
                             &Res,
                             timeout);

    if(wRet != 0)
    {
        TCRIT("Error While Setting the PAM Order %d\n", wRet);
        return wRet;
    }
    return (uint16)Res.CompletionCode;
}

/*
*@fn IPMICMD_AMIGetPamOrder
*@brief This command helps to get PAM order
*@param pAMIGetPamRes - Respose for the command
*@param timeout           - timeout
*@return Returns completion code
*/
uint16  IPMICMD_AMIGetPamOrder (IPMI20_SESSION_T *pSession,
            AMIGetPamRes_T    *pAMIGetPamRes, int timeout)
{
    uint16  wRet;
    uint32  dwResLen;
    uint8 *pReq = NULL;
    dwResLen = sizeof(AMIGetPamRes_T);
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                        ((NETFN_AMI << 2) + 0x00), CMD_AMI_GET_PAM_ORDER,
                        pReq, 0,
                        (uint8 *)pAMIGetPamRes, &dwResLen,
                        timeout);

    if(wRet != 0)
    {
        TCRIT("LIBIPMI: Error in getting the PAM order in raw IPMI command layer::%d\n", wRet);
    }

    return wRet;
}

/*
*@fn LIBIPMI_HL_AMIGetPamOrder
*@brief This command gives the PAM Order (ipmi, ldap, ad).
*@param pSession - IPMI SESSION
*@param PamOrder - Array to hold the PAM order.
*@param timeout     - Timeout
*@return Returns completion code
*/
uint16 LIBIPMI_HL_AMIGetPamOrder (IPMI20_SESSION_T *pSession, INT8U *PamOrder, int timeout)
{
    uint16 wRet;
    AMIGetPamRes_T pAMIGetPamRes;

    memset (&pAMIGetPamRes, 0 , sizeof (AMIGetPamRes_T));
    wRet =  IPMICMD_AMIGetPamOrder (pSession,
                             &pAMIGetPamRes, timeout);

    if(wRet != 0)
    {
        TCRIT("Error While Getting the PAM Order %d\n", wRet);
        return wRet;
    }
    memcpy(PamOrder, &pAMIGetPamRes.Seqnce, sizeof(pAMIGetPamRes.Seqnce));
    return wRet;
}

/*
*@fn IPMICMD_AMIGetLoginAuditConfig
*@brief This command helps to get login audit enable state
*		@param pResGetLoginAuditCfg - Respose for the command
*		@param timeout           - timeout
*		@return Returns completion code
*/
uint16  IPMICMD_AMIGetLoginAuditConfig(IPMI20_SESSION_T *pSession,
		AMIGetLoginAuditCfgRes_T    *pResGetLoginAuditCfg, int timeout)
{
	uint16  wRet;
	uint32  dwResLen;
    uint8 *pReq = NULL;
	dwResLen = sizeof(AMIGetLoginAuditCfgRes_T);

	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
						((NETFN_AMI << 2) + 0x00), CMD_AMI_GET_LOGIN_AUDIT_CFG,
						pReq, 0,
						(uint8 *)pResGetLoginAuditCfg, &dwResLen,
						timeout);

    if(wRet != 0)
    {
        TCRIT("LIBIPMI: Error in getting Login Audit Configuration in raw IPMI command layer::%d\n", wRet);
    }

	return wRet;
}

uint16 LIBIPMI_HL_AMILoginAuditConf(IPMI20_SESSION_T *pSession,
	INT8U *pEvtCfg, int timeout)
{
    uint16 wRet;
    AMIGetLoginAuditCfgRes_T Res = {0,0,0,0,0,0};

    wRet = IPMICMD_AMIGetLoginAuditConfig(pSession, &Res, timeout );

    memcpy(&pEvtCfg[0], &Res.WebLogAuditCfg, sizeof(Res.WebLogAuditCfg));
    memcpy(&pEvtCfg[1], &Res.TelnetLogAuditCfg, sizeof(Res.TelnetLogAuditCfg));
    memcpy(&pEvtCfg[2], &Res.SSHLogAuditCfg, sizeof(Res.SSHLogAuditCfg));
    memcpy(&pEvtCfg[3], &Res.KVMLogAuditCfg, sizeof(Res.KVMLogAuditCfg));
    return wRet;

}

uint16 LIBIPMI_HL_AMIGetLogAuditConf(IPMI20_SESSION_T *pSession, INT8U*pWebLogAuditCfg,
	INT8U*pIPMILogAuditCfg , INT8U* pTelnetLogAuditCfg, INT8U* pSSHLogAuditCfg,INT8U* pKVMLogAuditCfg, int timeout)
{
    uint16 wRet;
    AMIGetLoginAuditCfgRes_T Res = {0,0,0,0,0,0};

    wRet = IPMICMD_AMIGetLoginAuditConfig(pSession, &Res, timeout );

    memcpy(pWebLogAuditCfg,&Res.WebLogAuditCfg,sizeof(Res.WebLogAuditCfg));
    memcpy(pIPMILogAuditCfg,&Res.IPMILogAuditCfg,sizeof(Res.IPMILogAuditCfg));
    memcpy(pTelnetLogAuditCfg,&Res.TelnetLogAuditCfg,sizeof(Res.TelnetLogAuditCfg));
    memcpy(pSSHLogAuditCfg,&Res.SSHLogAuditCfg,sizeof(Res.SSHLogAuditCfg));
    memcpy(pKVMLogAuditCfg,&Res.KVMLogAuditCfg,sizeof(Res.KVMLogAuditCfg));
    return wRet;
}
/**
 * *@fn IPMICMD_AMISetLoginAuditConfig
 * *@brief This command is used to enable/disable login audit based on the parameter input
 *		@param pResSetLoginAuditCfg - Respose for the command
 *		@param timeout           - timeout
 *		@return Returns completion code
 **/
 uint16  IPMICMD_AMISetLoginAuditConfig(IPMI20_SESSION_T *pSession,
		 AMISetLoginAuditCfgReq_T* pReqSetLoginAuditCfg,
		   AMISetLoginAuditCfgRes_T *pResSetLoginAuditCfg,int timeout)
{
    uint16  wRet;
    uint32  dwResLen;
    dwResLen = sizeof(AMISetLoginAuditCfgRes_T);
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession,PAYLOAD_TYPE_IPMI,
				((NETFN_AMI << 2) + 0x00),CMD_AMI_SET_LOGIN_AUDIT_CFG,
				(uint8 *)pReqSetLoginAuditCfg,sizeof(AMISetLoginAuditCfgReq_T),
				(uint8 *)pResSetLoginAuditCfg, &dwResLen, timeout);
    return wRet;
}

 uint16 LIBIPMI_HL_AMISetLogAuditConf(IPMI20_SESSION_T *pSession,
	INT8U WebLogAuditCfg, INT8U IPMILogAuditCfg, INT8U pTelnetLogAuditCfg, INT8U pSSHLogAuditCfg,
	INT8U pKVMLogAuditCfg,int timeout)
{
     uint16 wRet;
     AMISetLoginAuditCfgRes_T Res = {0};
     AMISetLoginAuditCfgReq_T pReqSetLoginAuditCfg;
     pReqSetLoginAuditCfg.WebLogAuditCfg=WebLogAuditCfg;
     pReqSetLoginAuditCfg.IPMILogAuditCfg=IPMILogAuditCfg;
     pReqSetLoginAuditCfg.TelnetLogAuditCfg=pTelnetLogAuditCfg;
     pReqSetLoginAuditCfg.SSHLogAuditCfg=pSSHLogAuditCfg;
     pReqSetLoginAuditCfg.KVMLogAuditCfg=pKVMLogAuditCfg;

     wRet = IPMICMD_AMISetLoginAuditConfig(pSession,&pReqSetLoginAuditCfg,&Res,timeout );
     if(wRet != 0)
        {
    	 TCRIT("LIBIPMI: Error in setting Login Audit Configuration in raw IPMI command layer\n");
           return(wRet);
        }
     return(uint8) Res.CompletionCode;
}

/*
*@fn IPMICMD_AMIGetSELPolicy
*@brief This command helps to get SEL Policy
*@param pResGetSELPolicy - Respose for the command
*@param timeout          - timeout
*@return Returns completion code
*/

uint16  IPMICMD_AMIGetSELPolicy(IPMI20_SESSION_T *pSession, AMIGetSELPolicyRes_T *pResGetSELPolicy, int timeout)
{
    uint16  wRet;
    uint32  dwResLen;
    uint8 *pReq = NULL;
    dwResLen = sizeof(AMIGetSELPolicyRes_T);

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                ((NETFN_AMI << 2) + 0x00), CMD_AMI_GET_SEL_POLICY,pReq, 0,
                (uint8 *)pResGetSELPolicy, &dwResLen,timeout);

    if(wRet != 0)
    {
         TCRIT("LIBIPMI: Error in getting SEL Policy in raw IPMI command layer::%d\n", wRet);
    }

    return wRet;
}

/*
*@fn LIBIPMI_HL_AMIGetSELPolicy
*@brief This command helps to get SEL Policy.
*@param pSession - IPMI SESSION
*@param pSELPolicy - variable to get SEL policy
*@param timeout     - Timeout
*@return Returns completion code
*/

uint16 LIBIPMI_HL_AMIGetSELPolicy(IPMI20_SESSION_T *pSession, INT8U *pSELPolicy, int timeout)
{
    uint16 wRet;
    AMIGetSELPolicyRes_T Res = {0,0};

    wRet = IPMICMD_AMIGetSELPolicy(pSession, &Res, timeout);
    if(wRet != 0)
    {
        TCRIT("LIBIPMI: Error in getting SEL Policy in raw IPMI command layer::%d\n", wRet);
        return wRet;
    }
    memcpy(pSELPolicy,&Res.SELPolicy,sizeof(Res.SELPolicy));
    return wRet;
}

/*
*@fn IPMICMD_AMISetSELPolicy
*@brief This command helps to set SEL Policy
*@param pReqAMISetSELPolicy - Request for the command
*@param pResAMISetSELPolicy - Respose for the command
*@param timeout           - timeout
*@return Returns completion code
*/

uint16 IPMICMD_AMISetSELPolicy (IPMI20_SESSION_T *pSession,
        AMISetSELPolicyReq_T *pReqAMISetSELPolicy,
        AMISetSELPolicyRes_T *pResAMISetSELPolicy,int timeout)
{
    uint16  wRet;
    uint32  dwResLen;
    dwResLen = sizeof(AMISetSELPolicyRes_T);
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession,PAYLOAD_TYPE_IPMI,
                ((NETFN_AMI << 2) + 0x00),CMD_AMI_SET_SEL_POLICY,
                (uint8 *)pReqAMISetSELPolicy,sizeof(AMISetSELPolicyReq_T),
                (uint8 *)pResAMISetSELPolicy, &dwResLen, timeout);
    return wRet;
}

/*
*@fn LIBIPMI_HL_AMISetSELPolicy
*@brief This command helps to set SEL Policy.
*@param pSession - IPMI SESSION
*@param SELPolicy - variable to set SEL policy
*@param timeout     - Timeout
*@return Returns completion code
*/
uint16 LIBIPMI_HL_AMISetSELPolicy(IPMI20_SESSION_T *pSession, INT8U SELPolicy, int timeout)
{
    uint16 wRet;
    AMISetSELPolicyRes_T Res = {0};
    AMISetSELPolicyReq_T ReqSetSELPolicy;
    ReqSetSELPolicy.SELPolicy = SELPolicy;

    wRet = IPMICMD_AMISetSELPolicy(pSession,&ReqSetSELPolicy,&Res,timeout);
    if(wRet != 0)
    {
        TCRIT("LIBIPMI: Error in setting SEL Policy in raw IPMI command layer\n");
        return(wRet);
    }
    return wRet;
}

/*
*@fn IPMICMD_AMISetTriggerEvent
*@brief This command helps in setting the trigger event configuration
*		@param pReqSetTriggerEvent - Request for the command
		@param ReqDataLen			 - Request length
*		@param pResSetTriggerEvent - Response for the command
*		@param timeout           - timeout
*		@return Returns completion code
*/
uint16  IPMICMD_AMISetTriggerEvent(IPMI20_SESSION_T *pSession,
				AMISetTriggerEventReq_T    *pReqSetTriggerEvent,uint32  ReqDataLen,AMISetTriggerEventRes_T *pResSetTriggerEvent,
				int timeout)
{
	uint16  wRet;
	uint32  dwResLen;
	dwResLen = sizeof(AMISetTriggerEventRes_T);

	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
						((NETFN_AMI << 2) + 0x00), CMD_AMI_SET_TRIGGER_EVT,
						 (uint8 *)pReqSetTriggerEvent, ReqDataLen,
                       				 (uint8*)pResSetTriggerEvent, &dwResLen,
                      				  timeout);
	return wRet;
}

/*
*@fn IPMICMD_AMIGetTriggerEvent
*@brief This command helps in getting the trigger event configuration
*		@param pReqGetTriggerEvent - Request for the command
*		@param pResGetTriggerEvent - Response for the command
*		@param timeout           - timeout
*		@return Returns completion code
*/
uint16  IPMICMD_AMIGetTriggerEvent(IPMI20_SESSION_T *pSession,
				AMIGetTriggerEventReq_T    *pReqGetTriggerEvent, AMIGetTriggerEventRes_T *pResGetTriggerEvent,
				int timeout)
{
	uint16  wRet;
	uint32  dwResLen;
	dwResLen = sizeof(AMIGetTriggerEventRes_T);

	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
						((NETFN_AMI << 2) + 0x00), CMD_AMI_GET_TRIGGER_EVT,
						 (uint8 *)pReqGetTriggerEvent, sizeof(AMIGetTriggerEventReq_T),
                       				 (uint8*)pResGetTriggerEvent, &dwResLen,
                      				  timeout);
	return wRet;
}

uint16 LIBIPMI_HL_AMISetTriggerEvent(IPMI20_SESSION_T *pSession, INT8U TriggerParam, INT8U EnableDisableFlag, void  *Data,int timeout)
{
    uint16 wRet;
    AMISetTriggerEventReq_T    ReqSetTriggerEvent;
    AMISetTriggerEventRes_T    ResSetTriggerEvent;
    uint32  ReqDataLen;

    ReqSetTriggerEvent.TriggerParam = TriggerParam;
    ReqSetTriggerEvent.EnableDisableFlag = EnableDisableFlag;
    if(Data == NULL)
    {
       ReqSetTriggerEvent.TriggerData.Time = 0;
       ReqDataLen = 2;
    }
    else
    {
    	ReqSetTriggerEvent.TriggerData.Time = *((INT32U *)Data);
        ReqDataLen = 2 + sizeof(ReqSetTriggerEvent.TriggerData.Time);

    }

   wRet = IPMICMD_AMISetTriggerEvent(pSession,&ReqSetTriggerEvent , ReqDataLen ,&ResSetTriggerEvent ,timeout );
   if (wRet != 0){
   	TCRIT("Error in setting AMI set trigger event::%x\n", wRet);
   	}

   return wRet;

}


uint16 LIBIPMI_HL_AMIGetTriggerEvent(IPMI20_SESSION_T *pSession, INT8U TriggerParam, INT8U *EnableDisableFlag, INT32U *Data,int timeout)
{
    uint16 wRet;
    AMIGetTriggerEventReq_T    ReqGetTriggerEvent;
    AMIGetTriggerEventRes_T    ResGetTriggerEvent;

    ReqGetTriggerEvent.TriggerParam = TriggerParam;
	memset (&ResGetTriggerEvent, 0, sizeof(AMIGetTriggerEventRes_T));
   wRet = IPMICMD_AMIGetTriggerEvent(pSession,&ReqGetTriggerEvent , &ResGetTriggerEvent ,timeout );
   if(wRet !=0)
   	return wRet;

    *EnableDisableFlag = ResGetTriggerEvent.AMIGetTriggerEvent.EnableDisableFlag;
     if( ResGetTriggerEvent.TriggerData.Time!=0) {
    	*Data = ResGetTriggerEvent.TriggerData.Time;
		printf("LIBIPMI, Data::%u\n", *Data);
     }

   return wRet;

}

/*
*@fn IPMICMD_AMISetSOLTriggerEvent
*@brief This command helps in setting the trigger event configuration
*		@param pReqSetSOLTriggerEvent - Request for the command
		@param ReqDataLen			 - Request length
*		@param pResSetSOLTriggerEvent - Response for the command
*		@param timeout           - timeout
*		@return Returns completion code
*/
uint16  IPMICMD_AMISetSOLTriggerEvent(IPMI20_SESSION_T *pSession,
				AMISetSOLTriggerEventReq_T    *pReqSetSOLTriggerEvent,uint32  ReqDataLen,AMISetSOLTriggerEventRes_T *pResSetSOLTriggerEvent,
				int timeout)
{
	uint16  wRet;
	uint32  dwResLen;
	dwResLen = sizeof(AMISetSOLTriggerEventRes_T);

	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
						((NETFN_AMI << 2) + 0x00), CMD_AMI_SET_SOLTRIGGER_EVT,
						 (uint8 *)pReqSetSOLTriggerEvent, ReqDataLen,
                       				 (uint8*)pResSetSOLTriggerEvent, &dwResLen,
                      				  timeout);
	return wRet;
}

/*
*@fn IPMICMD_AMIGetSOLTriggerEvent
*@brief This command helps in getting the trigger event configuration
*		@param pReqGetTriggerEvent - Request for the command
*		@param pResGetTriggerEvent - Response for the command
*		@param timeout           - timeout
*		@return Returns completion code
*/
uint16  IPMICMD_AMIGetSOLTriggerEvent(IPMI20_SESSION_T *pSession,
				AMIGetSOLTriggerEventReq_T    *pReqGetSOLTriggerEvent, AMIGetSOLTriggerEventRes_T *pResGetSOLTriggerEvent,
				int timeout)
{
	uint16  wRet;
	uint32  dwResLen;
	dwResLen = sizeof(AMIGetSOLTriggerEventRes_T);

	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
						((NETFN_AMI << 2) + 0x00), CMD_AMI_GET_SOLTRIGGER_EVT,
						 (uint8 *)pReqGetSOLTriggerEvent, sizeof(AMIGetSOLTriggerEventReq_T),
                       				 (uint8*)pResGetSOLTriggerEvent, &dwResLen,
                      				  timeout);
	return wRet;
}

uint16 LIBIPMI_HL_AMISetSOLTriggerEvent(IPMI20_SESSION_T *pSession, INT8U TriggerParam, INT8U EnableDisableFlag, void  *Data,int timeout)
{
    uint16 wRet;
    AMISetSOLTriggerEventReq_T    ReqSetSOLTriggerEvent;
    AMISetSOLTriggerEventRes_T    ResSetSOLTriggerEvent;
    uint32  ReqDataLen;

    ReqSetSOLTriggerEvent.TriggerParam = TriggerParam;
    ReqSetSOLTriggerEvent.EnableDisableFlag = EnableDisableFlag;
    if(Data == NULL)
    {
       ReqSetSOLTriggerEvent.TriggerData.Time = 0;
       ReqDataLen = 2;
    }
    else
    {
        ReqSetSOLTriggerEvent.TriggerData.Time = *((INT32U *)Data);
        ReqDataLen = 2 + sizeof(ReqSetSOLTriggerEvent.TriggerData.Time);
    }

    wRet = IPMICMD_AMISetSOLTriggerEvent(pSession,&ReqSetSOLTriggerEvent , ReqDataLen ,&ResSetSOLTriggerEvent ,timeout );
    if (wRet != 0)
       TCRIT("Error in setting AMI SOL set trigger event::%x\n", wRet);

   return wRet;

}


uint16 LIBIPMI_HL_AMIGetSOLTriggerEvent(IPMI20_SESSION_T *pSession, INT8U TriggerParam, INT8U *EnableDisableFlag, INT32U *Data,int timeout)
{
    uint16 wRet;
    AMIGetSOLTriggerEventReq_T    ReqGetSOLTriggerEvent;
    AMIGetSOLTriggerEventRes_T    ResGetSOLTriggerEvent;

    ReqGetSOLTriggerEvent.TriggerParam = TriggerParam;
	memset (&ResGetSOLTriggerEvent, 0, sizeof(AMIGetSOLTriggerEventRes_T));
   wRet = IPMICMD_AMIGetSOLTriggerEvent(pSession,&ReqGetSOLTriggerEvent , &ResGetSOLTriggerEvent ,timeout );
   if(wRet !=0)
   	return wRet;

    *EnableDisableFlag = ResGetSOLTriggerEvent.AMIGetTriggerEvent.EnableDisableFlag;
     if( ResGetSOLTriggerEvent.TriggerData.Time!=0) {
    	*Data = ResGetSOLTriggerEvent.TriggerData.Time;
		printf("LIBIPMI, Data::%u\n", *Data);
     }

   return wRet;

}

uint16 IPMICMD_AMIGetSNMPConf( IPMI20_SESSION_T *pSession,
                                                       AMIGetSNMPConfReq_T *pGetSNMPConfReq,
                             AMIGetSNMPConfRes_T *pGetSNMPConfRes,
                             int timeout ){
     uint16 wRet;
     uint32 dwResLen;

     dwResLen = sizeof(AMIGetSNMPConfRes_T);
     wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                        (NETFN_AMI << 2), CMD_AMI_GET_SNMP_CONF,
                        (uint8*)pGetSNMPConfReq, sizeof(AMIGetSNMPConfReq_T),
                        (uint8*)pGetSNMPConfRes, &dwResLen,
                        timeout);

     if(wRet != 0)
     {
        TCRIT("Error in getting SNMP Configuration in raw IPMI command layer::%d\n", wRet);
     }
     return (wRet);
 }

uint16 IPMICMD_AMISetSNMPConf(IPMI20_SESSION_T *pSession,AMISetSNMPConfReq_T *pAMISetSNMPconfReq
                                                                        ,AMISetSNMPConfRes_T *pAMISetSNMPConfRes,int timeout){
     uint16 wRet;
     uint32 dwResLen;
     dwResLen = sizeof(AMISetSNMPConfRes_T);
     wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                        (NETFN_AMI << 2), CMD_AMI_SET_SNMP_CONF,
                        (uint8 *)pAMISetSNMPconfReq, sizeof(AMISetSNMPConfReq_T),
                        (uint8*)pAMISetSNMPConfRes, &dwResLen,
                        timeout);
     if(wRet != 0)
     {
        TCRIT("Error in Setting SNMP Configuration in raw IPMI command layer::%d\n", wRet);
        return pAMISetSNMPConfRes->CompletionCode;//Quanta
     }
     return (wRet);
 }

uint16 LIBIPMI_HL_AMIGetSNMPConf(IPMI20_SESSION_T *pSession,
		INT8U UserId,INT8U *SNMPStatus,
		INT8U *SNMPAccess, INT8U *AUTHProtocol, INT8U *PrivProtocol,
		int timeout){
     uint16 wRet;
     AMIGetSNMPConfReq_T Req ;
     AMIGetSNMPConfRes_T Res = {0,0,0,0,0,0};
     INT8U snmp_enable;
     INT8U snmp_access_type;
     INT8U snmp_enc_type_1;
     INT8U snmp_enc_type_2;

     Req.UserID = UserId;
     wRet = IPMICMD_AMIGetSNMPConf(pSession,
                                               &Req,
                             &Res,
                             timeout );

     if(wRet != 0)
     {
        TCRIT("Error While getting SNMP configuration %d\n",wRet);
     }
     snmp_enable = Res.snmp_enable;
     snmp_access_type = Res.snmp_access_type;
     snmp_enc_type_1 = Res.snmp_enc_type_1;
     snmp_enc_type_2 = Res.snmp_enc_type_2;

     memcpy(SNMPStatus, &snmp_enable, sizeof(INT8U));
     memcpy(SNMPAccess, &snmp_access_type, sizeof(INT8U));
     memcpy(AUTHProtocol, &snmp_enc_type_1, sizeof(INT8U));
     memcpy(PrivProtocol, &snmp_enc_type_2, sizeof(INT8U));

     return wRet;
 }

uint16 LIBIPMI_HL_AMISetSNMPConf(IPMI20_SESSION_T *pSession,INT8U UserId,INT8U SNMPStatus, INT8U SNMPAccess, INT8U AUTHProtocol, INT8U PrivProtocol, int timeout){
     uint16 wRet;
     AMISetSNMPConfReq_T Req = {0,0,0,0,0};
     AMISetSNMPConfRes_T Res = {0};

     Req.UserID = UserId;
     Req.snmp_enable = SNMPStatus;
     Req.snmp_access_type = SNMPAccess;
     Req.snmp_enc_type_1 = AUTHProtocol;
     Req.snmp_enc_type_2 = PrivProtocol;

     wRet = IPMICMD_AMISetSNMPConf(pSession,
                                               &Req,
                             &Res,
                             timeout );
     if(wRet != 0)
     {
        TCRIT("Error While getting SNMP configuration %d\n",wRet);
     }
     return wRet;
 }

uint16 IPMICMD_AMIGetpreserveConfStatus(IPMI20_SESSION_T *pSession, GetPreserveConfigReq_T* pGetPreserveConfReq
                                               , GetPreserveConfigRes_T* pAMIGetPreserveConfRes, int timeout)
{
    uint16 wRet;
    uint32 dwResLen;
    dwResLen = sizeof(GetPreserveConfigRes_T);

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                        (NETFN_AMI << 2), CMD_AMI_GET_PRESERVE_CONF,
                        (uint8 *)pGetPreserveConfReq, sizeof(GetPreserveConfigReq_T),
                        (uint8*)pAMIGetPreserveConfRes, &dwResLen,
                        timeout);

    if(wRet != 0)
    {
        TCRIT("Error in Getting Preserve Configuration in raw IPMI command layer::%d\n", wRet);
    }
    return (wRet);

}

uint16 LIBIPMI_HL_AMIGetPreserveConfStatus(IPMI20_SESSION_T *pSession, UINT8 selector, UINT8 *status, int timeout)
{
    uint16 wRet;
    GetPreserveConfigReq_T pGetPreserveConfReq;
    GetPreserveConfigRes_T Res = {0,0};

    memset (&pGetPreserveConfReq, 0, sizeof (GetPreserveConfigReq_T));
	pGetPreserveConfReq.Selector = selector;
    wRet =  IPMICMD_AMIGetpreserveConfStatus(pSession,
                             &pGetPreserveConfReq,
                             &Res,
                             timeout);

    if(wRet != 0)
    {
        TCRIT("Error While Getting Preserve Configuration Status%d\n",wRet);
        return wRet;
    }
    *status = Res.Status;
    return wRet;
}


uint16 IPMICMD_AMISetpreserveConfStatus(IPMI20_SESSION_T *pSession, SetPreserveConfigReq_T* pSetPreserveConfReq
                                               , SetPreserveConfigRes_T* pAMISetPreserveConfRes, int timeout)
{
    uint16 wRet;
    uint32 dwResLen;
    dwResLen = sizeof(SetPreserveConfigRes_T);

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                        (NETFN_AMI << 2), CMD_AMI_SET_PRESERVE_CONF,
                        (uint8 *)pSetPreserveConfReq, sizeof(SetPreserveConfigReq_T),
                        (uint8*)pAMISetPreserveConfRes, &dwResLen,
                        timeout);

    if(wRet != 0)
    {
        TCRIT("Error in Setting Preserve Configuration in raw IPMI command layer::%d\n", wRet);
    }
    return (wRet);

}


uint16 LIBIPMI_HL_AMISetPreserveConfStatus(IPMI20_SESSION_T *pSession, UINT8 selector, UINT8 status, int timeout)
{
    uint16 wRet;
    SetPreserveConfigReq_T pSetPreserveConfReq;
    SetPreserveConfigRes_T Res = {0};

    memset (&pSetPreserveConfReq, 0, sizeof (SetPreserveConfigReq_T));
	pSetPreserveConfReq.Selector = selector;
	pSetPreserveConfReq.Status = status;
//    memcpy (&pSetPreserveConfReq, pPreserveStatus, sizeof (AMIServiceConfig_T));
    wRet =  IPMICMD_AMISetpreserveConfStatus(pSession,
                             &pSetPreserveConfReq,
                             &Res,
                             timeout);

    if(wRet != 0)
    {
        TCRIT("Error While Setting Preserve Configuration Status%d\n",wRet);
        return wRet;
    }
    return (uint16)Res.CompletionCode;
}

/*
*@fn IPMICMD_AMIGetSELEntires
*@brief
*@param pSession - IPMI Session Handle
*@param pAMIGetSelEntriesReq - Pointer to the request of AMI Get SEL Entries command
*@param pAMIGetSelEntiresRes - Pointer to the response of AMI Get SEL Entries command
*@param timeout - Timeout Value
**/
uint16 IPMICMD_AMIGetSELEntires(IPMI20_SESSION_T *pSession,AMIGetSELEntriesReq_T *pAMIGetSelEntriesReq,
                                                   AMIGetSELEntriesRes_T *pAMIGetSelEntiresRes,int timeout)
{
    uint16 wRet;
    uint32 dwResLen;
    dwResLen = sizeof(AMIGetSELEntriesRes_T);

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                        (NETFN_AMI << 2), CMD_AMI_GET_SEL_ENTIRES,
                        (uint8 *)pAMIGetSelEntriesReq, sizeof(AMIGetSELEntriesReq_T),
                        (uint8*)pAMIGetSelEntiresRes, &dwResLen,
                        timeout);

    return (wRet);
}

/*
*@fn LIBIPMI_HL_AMIGetSELEntires
*@brief High Level command to retrieve the total SEL Entries
*@param pSession - IPMI Session Handle
*@param pSELEntriesBuffer - Pointer to hold the retrieved SEL Entries
*@param nNumSelEntries - Pointer which hold the no. of SEL Entries retrieved
*@param timeout - Timeout Value
*@return Returns LIBIPMI_E_SUCCESS on success
*@return Returns proper completion code on error
**/
uint16 LIBIPMI_HL_AMIGetSELEntires(IPMI20_SESSION_T *pSession,SELEventRecordWithSensorName_T *pSELEntriesBuffer,uint32 *nNumSelEntries,int timeout)
{
    AMIGetSELEntriesReq_T pAMIGetSelEntriesReq = {0};
    AMIGetSELEntriesRes_T *pAMIGetSelEntiresRes;
    SELEventRecordWithSensorName_T *CircSELEntries;
    uint8 *SELEntries;
    uint16 wRet = 0;
    int j=0;
    uint16 i=0,recindex=0;
    uint16 LastRecID = 0xFFFF,MaxSELEntries=0;
    SELRec_T *ptrToSELRecord = NULL; // Quanta

    SELEntries = (uint8 *)malloc(MAX_SEL_RETRIEVAL_SIZE);
    if(SELEntries == NULL)
    {
        TCRIT("Memory allocation error in LIBIPMI_HL_AMIGetSELEntires \n");
        wRet = 0xFF;
        return wRet;
    }

    memset(SELEntries,0,MAX_SEL_RETRIEVAL_SIZE);
    pAMIGetSelEntiresRes = (AMIGetSELEntriesRes_T *)SELEntries;

    do
    {
        wRet = IPMICMD_AMIGetSELEntires(pSession,&pAMIGetSelEntriesReq,(AMIGetSELEntriesRes_T *)SELEntries,timeout);
        if(wRet == LIBIPMI_E_SUCCESS)
        {
            for(i = 0; i < pAMIGetSelEntiresRes->Noofentries; i++)
            {
				//Quanta++
				ptrToSELRecord = (SELRec_T *)(&(SELEntries)[sizeof(AMIGetSELEntriesRes_T) + (i * sizeof(SELRec_T))]);
				if((ptrToSELRecord->EvtRecord.hdr.Type >= 0xC0) && (ptrToSELRecord->EvtRecord.hdr.Type <= 0xDF))
				{
					memcpy((uint8*)&pSELEntriesBuffer[i+*nNumSelEntries].OEM1EventRecord, (uint8*)&(ptrToSELRecord->EvtRecord), sizeof(SELOEM1Record_T));
				}
				else if((ptrToSELRecord->EvtRecord.hdr.Type >= 0xE0)) // 0xE0 - 0xFF
				{
					memcpy((uint8*)&pSELEntriesBuffer[i+*nNumSelEntries].OEM2EventRecord, (uint8*)&(ptrToSELRecord->EvtRecord), sizeof(SELOEM2Record_T));
				}
				//Quanta--
                memcpy((uint8 *)&pSELEntriesBuffer[i+*nNumSelEntries].EventRecord,&(SELEntries)[sizeof(AMIGetSELEntriesRes_T) + (i * sizeof(SELRec_T))],
                               sizeof(SELRec_T));
            }

            if(pAMIGetSelEntriesReq.Noofentretrieved == 0x00)
            {
                LastRecID = pAMIGetSelEntiresRes->LastRecID;
            }

            if(pAMIGetSelEntiresRes->Status == FULL_SEL_ENTRIES)
            {
                *nNumSelEntries += pAMIGetSelEntiresRes->Noofentries;
                break;
            }
            else
            {
                *nNumSelEntries += pAMIGetSelEntiresRes->Noofentries;
                pAMIGetSelEntriesReq.Noofentretrieved = *nNumSelEntries;
                memset(SELEntries,0,MAX_SEL_RETRIEVAL_SIZE);
                continue;
            }
        }
        else if((wRet & 0xFF) == LIBIPMI_E_SEL_CLEARED)
        {
            memset(SELEntries,0,MAX_SEL_RETRIEVAL_SIZE);
            memset((uint8*)&pSELEntriesBuffer[0],0,sizeof(SELEventRecordWithSensorName_T) * *nNumSelEntries);
            *nNumSelEntries =  pAMIGetSelEntriesReq.Noofentretrieved = 0;
            continue;
        }
        else if((wRet & 0xFF)  == LIBIPMI_E_SEL_EMPTY)
        {
            /* Returning LIBIPMI_E_SUCCESS when sel repo is empty as
                 per WebUI expectations. Will be removed once changes are made in WebUI*/
            wRet = LIBIPMI_E_SUCCESS;
            return wRet;
        }
        else
        {
            break;
        }
    }while(1);

    free(SELEntries);


    if(wRet == LIBIPMI_E_SUCCESS)
    {
        if(LastRecID != 0xFFFF)
        {
            wRet = LIBIPMI_HL_GetMaxPossibleSELEntries(pSession,&MaxSELEntries,timeout);
            if(wRet == LIBIPMI_E_SUCCESS)
            {
                 if(LastRecID > MaxSELEntries)
                {
                    for(i = 0; i < *nNumSelEntries; i++)
                    {
                        if(pSELEntriesBuffer[i].EventRecord.EvtRecord.hdr.ID == LastRecID)
                        {
                            recindex = i;
                            break;
                        }
                    }

                    /* Last Record is not found if reached till end of SEL Entries*/
                    if(i == *nNumSelEntries)
                    {
                        wRet = 0xFF;
                        return wRet;
                    }

                    /*SEL Records added lastely will be displayed first */
                    CircSELEntries = (SELEventRecordWithSensorName_T *)malloc (sizeof(SELEventRecordWithSensorName_T) * (*nNumSelEntries));
                    if(CircSELEntries != NULL)
                    {
                        memset(CircSELEntries,0,sizeof(SELEventRecordWithSensorName_T) * (*nNumSelEntries));
                        j = recindex; i = 0;
                        while(j >= 0)
                        {
                            memcpy((uint8 *)&CircSELEntries[i],(uint8*)&pSELEntriesBuffer[j],sizeof(SELEventRecordWithSensorName_T));
                            j--;i++;
                        }

                        for(j = recindex + 1,i = 1; j < (signed)*nNumSelEntries; j++,i++)
                        {
                            memcpy((uint8*)&CircSELEntries[j],(uint8*)&pSELEntriesBuffer[*nNumSelEntries - i ],sizeof(SELEventRecordWithSensorName_T));
                        }

                        for(i = 0,j=1;i< *nNumSelEntries; i++,j++)
                        {
                            memcpy((uint8*)&pSELEntriesBuffer[i],(uint8*)&CircSELEntries[*nNumSelEntries - j],sizeof(SELEventRecordWithSensorName_T));
                        }

                        free(CircSELEntries);
                    }
                }
            }
        }
    }

    return (wRet);
}

/*
*@fn IPMICMD_AMIGetSensorInfo
*@brief
*@param pSession - IPMI Session Handle
*@param pAMIGetSensorInfoReq - Pointer to the request of AMI Get SensorInfo command
*@param pAMIGetSensorInfoRes - Pointer to the response of AMI Get SensorInfo command
*@param timeout - Timeout Value
**/
uint16 IPMICMD_AMIGetSensorInfo(IPMI20_SESSION_T *pSession, AMIGetSensorInfoRes_T *pAMIGetSensorInfoRes,int timeout)
{
    uint16 wRet;
    uint32 dwResLen;
    uint8 Req[20];
    dwResLen = sizeof(AMIGetSensorInfoRes_T);

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                        (NETFN_AMI << 2), CMD_AMI_GET_SENSOR_INFO,
                        Req, 0,
                        (uint8*)pAMIGetSensorInfoRes, &dwResLen,
                        timeout);

    return (wRet);
}

uint16 LIBIPMI_HL_AMIGetSensorInfo(IPMI20_SESSION_T *pSession,SenInfo_T *pSensorInfo,uint32 *nNumSensor,int timeout)
{
    uint16 wRet = 0;
    AMIGetSensorInfoRes_T *pAMIGetSensorInfoRes;
    uint8 *SensorEntries;
    uint32 i;

    SensorEntries = (uint8 *)malloc(MAX_SENSOR_INFO_SIZE);
    if(SensorEntries == NULL)
    {
        TCRIT("Memory allocation error in LIBIPMI_HL_AMIGetSELEntires \n");
        wRet = 0xFF;
        return wRet;
    }

    memset(SensorEntries,0,MAX_SENSOR_INFO_SIZE);
    pAMIGetSensorInfoRes = (AMIGetSensorInfoRes_T *)SensorEntries;
    wRet = IPMICMD_AMIGetSensorInfo(pSession,(AMIGetSensorInfoRes_T *)SensorEntries,timeout);
    if(wRet == LIBIPMI_E_SUCCESS)
    {
        if(*nNumSensor > pAMIGetSensorInfoRes->Noofentries)
            *nNumSensor = pAMIGetSensorInfoRes->Noofentries;
        for(i = 0; i < *nNumSensor; i++)
        {
            memcpy((uint8 *)&pSensorInfo[i],&SensorEntries[sizeof(AMIGetSensorInfoRes_T) + (i * sizeof(SenInfo_T))],sizeof(SenInfo_T) );
        }
    }
    else
    {
        TCRIT("error getting sensor info %d\n",wRet);
    }
    free(SensorEntries);
    return (wRet);
}

/*
*@fn IPMICMD_GetIPv6Addr
*@brief command to get the ipv6 address
*@param pSession - IPMI Session Handle
*@param AMIGetIPv6AddrReq - Pointer to the request of AMI Get IPv6 address command
*@param AMIGetIPv6AddrRes - Pointer to the response of AMI Get IPv6address command
*@param timeout - Timeout Value
**/
uint16 IPMICMD_GetIPv6Addr(IPMI20_SESSION_T *pSession,AMIGetIPv6AddrReq_T *AMIGetIPv6AddrReq,
                                                              AMIGetIPv6AddrRes_T *AMIGetIPv6AddrRes, int timeout)
{
    uint16 wRet;
    uint32 dwResLen;
    dwResLen = sizeof(AMIGetIPv6AddrRes_T);

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                        (NETFN_AMI << 2), CMD_AMI_GET_IPV6_ADDRESS,
                        (uint8*)AMIGetIPv6AddrReq, sizeof(AMIGetIPv6AddrReq_T),
                        (uint8*)AMIGetIPv6AddrRes, &dwResLen,
                        timeout);

    return (wRet);
}

/*
*@fn LIBIPMI_HL_GetAllIPv6Address
*@brief High Level command to retrieve the total ipv6 address
*@param pSession - IPMI Session Handle
*@param Channel - Channel Number
*@param Address - Pointer to hold the retrieved ipv6 address
*@param AddrCnt - Pointer which hold the no. of ipv6 Entries retrieved
*@param timeout - Timeout Value
*@return Returns LIBIPMI_E_SUCCESS on success
*@return Returns proper completion code on error
**/
uint16 LIBIPMI_HL_GetAllIPv6Address(IPMI20_SESSION_T *pSession, INT8U Channel, INT8U *Address,INT8U *AddrCnt, int timeout)
{
    uint16 wRet = 0;
    AMIGetIPv6AddrReq_T AMIGetIPv6AddrReq;
    AMIGetIPv6AddrRes_T AMIGetIPv6AddrRes;
    GetIPv6AddrRes_T *GetIPv6AddrRes = (GetIPv6AddrRes_T*)Address;
    unsigned char NewAddr[16]={0};
    int Cnt =0, ndx = 0, j = 0;

    AMIGetIPv6AddrReq.ChannelNum = Channel;
    AMIGetIPv6AddrReq.IPCnt = sizeof (AMIGetIPv6AddrRes.GlobalPrefix);
    memset (NewAddr, 0, 16);

    for (ndx = 0; ndx < 2; ndx ++)
    {	if(sizeof (GetIPv6AddrRes_T) <= sizeof(AMIGetIPv6AddrRes))
    	{
//        	memset (&AMIGetIPv6AddrRes, 0, sizeof (GetIPv6AddrRes));  //Quanta coverity
    		memset (&AMIGetIPv6AddrRes, 0, sizeof (AMIGetIPv6AddrRes)); //Quanta coverity
    	}
    	else
    	{
    		TCRIT("Buffer overflow in %s\n",__FUNCTION__);
    		return 0;
    	}
        AMIGetIPv6AddrReq.Index = j;
        wRet = IPMICMD_GetIPv6Addr (pSession, &AMIGetIPv6AddrReq, &AMIGetIPv6AddrRes, timeout);
        if(wRet != 0)
        {
            TCRIT("IPMICMD_GetIPv6Addr: Error getting ipv6 addresses %d\n", Channel);
            return wRet;
        }
        for (j = 0; j < AMIGetIPv6AddrReq.IPCnt; j++)
        {
            if(0 != memcpy (&NewAddr, AMIGetIPv6AddrRes.GlobalIPAddr [j], sizeof (AMIGetIPv6AddrRes.GlobalIPAddr [0])))
            {
                memcpy (&GetIPv6AddrRes->GlobalIPAddr [Cnt], &AMIGetIPv6AddrRes.GlobalIPAddr [j], sizeof (AMIGetIPv6AddrRes.GlobalIPAddr [0]));
                GetIPv6AddrRes->GlobalPrefix[Cnt] = AMIGetIPv6AddrRes.GlobalPrefix[j];
                Cnt++;
            }
        }
    }
    *AddrCnt = Cnt;

    return wRet;
}

#if LIBIPMI_IS_OS_LINUX()

/**************************************************************************
 ** @fn ConvertIPStrToNWFormat
 ** @ brief This function converts the ipaddress from string format to network related
 **             address and validates ipaddress is IPV6 or IPV4 address.
 ** @ param IPStr ipaddress in string format
 ** @ param IPNwFormat ipaddress converted to network format.
 ** @ return  IPversion(4 or 6) - Success, -1 - Failure
 ***************************************************************************/
int ConvertIPStrToNWFormat (const char * IPStr, uint8 *IPNwFormat)
{

    uint8 ipv6Add [IP6_ADDR_LEN];

    memset (ipv6Add, 0, IP6_ADDR_LEN);
    if (inet_pton (AF_INET6, IPStr, ipv6Add) > 0)
    {
        if(IN6_IS_ADDR_V4MAPPED(ipv6Add))
        {
            /* The last bytes of IP6 contains IP4 address */
            memcpy(IPNwFormat, &ipv6Add[IP6_ADDR_LEN - IP_ADDR_LEN], sizeof (struct in_addr));
            return IPV4_VERSION_NUM;
        }
        else
        {
            memcpy(IPNwFormat, ipv6Add, sizeof (struct in6_addr));
            return IPV6_VERSION_NUM;
        }
    }
    else if (inet_pton (AF_INET, IPStr, IPNwFormat) > 0)
    {
        TDBG ("Valid IPV4 address\n\n");
        return IPV4_VERSION_NUM;
    }

    TDBG ("invalid IP address\n");
    return IPV_FAILED;
}

/**
 *@fn EnableAuditLogs
 *@brief verify login audits are enabled or not for specific recordtype
 *@param Service- (webgo/sshd/login).
 *@param Auditmask-EventMask
 *@param RecType- the record type
 *@return:on success reutrns 0
**/
static int EnableAuditLogs (char *Service, uint8 *Auditmask, int state, uint8 *Rectype)
{
    uint8 Index =0;

    for (Index =0; (Index < AUTH_REQ_SERVICES) && (state < LOGIN_STATE_TYPE); Index++)
    {
        /*Check Audits are enabled for particular service and fill the record type*/
        if ((0 == strcasecmp ( AugitRecNo[Index].Srvce, Service))
             && (Auditmask[Index] & (1 << state)))
        {
            *Rectype = AugitRecNo[Index].Status[state];
            break;
        }
    }
    if (Index == AUTH_REQ_SERVICES)
    {
        TDBG ("service not found\n");
        return -1;
    }

    return 0;
}
/*-----------------------------------------------------------------------------------
 ** @fn MapUid_to_SELUid
 *
 * @ brief This function is used to Get the UID
 * @ uname  session_user_name
 * @ shell  holds the shell
 * @ return uid  - Success , 0 - invalid UID
 *------------------------------------------------------------------------------------*/
uint16 MapUid_to_SELUid ( int8 *username, char *shell)
{
    struct passwd *pwd = NULL;
    uint16 retuid = 0xff;
    int tempret=0;/*To check return value of snprintf, it is not used any where else*/
    int shellsize=0;

    if (username == NULL)
        return retuid;

    pwd = getpwnam( (char *)username);

    if ( pwd == NULL )
    {
        return retuid;
    }
    retuid = pwd->pw_uid;
    if (NULL != shell){
		/*FORTIFY FIX  removing strcpy (shell,pwd->pw_shell)
		Since Shell is always null, using default strlen*/
		shellsize=strlen(shell);
                tempret=snprintf (shell,shellsize,"%s",pwd->pw_shell);
                if((tempret<0)||(tempret>=shellsize))
                {
                    TCRIT("Buffer Overflow");
                    return -1;
                }
    }

    if ( (pwd->pw_uid >= IPMI_FIRST_UID) && (pwd->pw_uid < IPMI_LAST_UID) )
    {
        TDBG ("check point 2 pwd->uid = %d,  uid = %x\n", pwd->pw_uid, retuid);
        retuid = pwd->pw_uid - 1000;
        return retuid;
    }
    else if ( (pwd->pw_uid >= LDAP_FIRST_UID) && (pwd->pw_uid < RADIUS_LAST_UID) )
        retuid = pwd->pw_uid;

    TDBG ("check point 2 pwd->uid = %d,  uid = %x\n", pwd->pw_uid, retuid);
    return retuid;
}

/**
 *@fn LIBIPMI_HL_LoginLogoutAudit
 *@brief This function is used to log audit event for different services.
 *@param pSession - IPMI Session Handle
 *@param Service- (webgo/sshd/login/kvm).
 *@param uname--username
 *@param ipstr--ip address
 *@param state--audit event
 *@return:on success reutrns 0
**/
uint16 LIBIPMI_HL_LoginLogoutAudit(IPMI20_SESSION_T *pSession, char *service, char*uname, char *ipstr, uint8 state, uint8 count)
{
    uint16 wRet;
    uint8 MaxUsers;
    uint16 uid = 0xff;
    uint8 rectype = 0;
    AddSELRes_T SELResData;
    uint8 AuditEvtMask[AUTH_REQ_SERVICES];
    uint8 ipaddr[IP6_ADDR_LEN];
    int IPVersion = 0;
    uint8 Index =0;

    memset (ipaddr, 0, IP6_ADDR_LEN);
    /*Check the given ipaddress is ipv4 or ipv6 and convert it to network format*/
    IPVersion = ConvertIPStrToNWFormat (ipstr, ipaddr);
    if (IPV_FAILED == IPVersion)
    {
        TDBG ("invlaid ip address\n");
        return IPV_FAILED;
    }
    SELOEM1Record_T  OEMSELRec = {
                                 0x00, /* ID */
                                 0xc3, /* Type */ // Modify Login info record type
                                 0, /* TimeStamp */
                                 {0x00, 0x00, 0x00}, /*MftrID*/
                                 {rectype, uid, ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3]} /*OEMData*/
                                  };

    wRet = LIBIPMI_HL_AMILoginAuditConf (pSession, AuditEvtMask, LIBIPMI_REQUEST_TIMEOUT);
    if( wRet != LIBIPMI_E_SUCCESS )
    {
         TDBG ("Error getting Web User Log status\n");
         return wRet;
    }
    /*Get the record type for particular service's login failure or success*/
    if (state == SEL_PUBKEY_LOGIN_FAILED)
    {
        OEMSELRec.OEMData[0] = state;
    }
    else if (state == SEL_MUL_LOGIN_FAILED)
    {
        for (Index =0; Index < AUTH_REQ_SERVICES; Index++)
        {
            if ((0 == strcasecmp ( AugitRecNo[Index].Srvce, service)) && (AuditEvtMask[Index] & SEL_MUL_LOGIN_FAILED))
            {
                OEMSELRec.OEMData[0] = SEL_LOGIN_FAILED_REC + Index;
                break;
            }
        }
        if (Index == AUTH_REQ_SERVICES)
            return 1;
    }
    else
    {
        /*Get the record type for particular service's login failure or success*/
        if (-1 == EnableAuditLogs (service, AuditEvtMask, state, &rectype))
        {
              wRet = 1;
              return wRet;
        }
        OEMSELRec.OEMData[0] = rectype;
    }
    uid = MapUid_to_SELUid ((int8 *)uname,NULL);
    if (uid == 0xff)
    {
         TDBG ( "Mapping uid to SEL device uid is failed \n" );
    }
#ifdef CONFIG_SPX_FEATURE_IANA_2
    /*Fill the IANA and Timestamp*/
    OEMSELRec.MftrID [0] = CONFIG_SPX_FEATURE_IANA_2;
    OEMSELRec.MftrID [1] = CONFIG_SPX_FEATURE_IANA_1;
    OEMSELRec.MftrID [2] = CONFIG_SPX_FEATURE_IANA_0;
#endif
    OEMSELRec.TimeStamp = time(NULL);
    if ( uid >= LDAP_FIRST_UID && uid < RADIUS_LAST_UID )
    {
         wRet = LIBIPMI_HL_GetMaxNumUsers (pSession, &MaxUsers, LIBIPMI_REQUEST_TIMEOUT);
         if( wRet != LIBIPMI_E_SUCCESS )
         {
               TDBG ("Error getting max num users\n");
               return wRet; //when Get max num of users fails..
         }
         /*this is temporary.. user id for ldap and AD going to be maxipmiusers + 1 or 2...*/
         OEMSELRec.OEMData[1] = MaxUsers + ( ((uid >= RADIUS_FIRST_UID)&&(uid < RADIUS_LAST_UID)) ? 3 : (((uid >= LDAP_FIRST_UID)&&(uid < LDAP_LAST_UID))? 1 : 2));
    }
    else
    {
         OEMSELRec.OEMData[1] = uid;
    }
    if (state == SEL_MUL_LOGIN_FAILED)
    {
         OEMSELRec.OEMData[1] = count;
    }

    if (IPV6_VERSION_NUM == IPVersion)
    {
         /**  These printf statements are for testing purpose only
         **  OEM should replace their implementation to add IPV6 based Event Logs
         **/
         printf("\n IPV6 Log Audit: MfrID: 0x%X 0x%X 0x%X Event Type: 0x%X User ID: 0x%X",
                      OEMSELRec.MftrID [0], OEMSELRec.MftrID [1], OEMSELRec.MftrID [2], rectype, uid);
         printf("\n inet6 addr: %s \n", ipstr);
         /*IPV6 Audit Success*/
         memset(&OEMSELRec.OEMData[2],0xFF,IP_ADDR_LEN);
    }
    wRet = IPMICMD_AddSELEntry (pSession, (SELEventRecord_T *) &OEMSELRec, &SELResData, LIBIPMI_REQUEST_TIMEOUT );
    if (( wRet == LIBIPMI_E_SUCCESS ) && ( SELResData.CompletionCode == CC_SUCCESS ))
    {
        TDBG ("EVENT Logged successfully\n");
    }
    return wRet;
}
#endif
static uint16 IPMICMD_AMIGetFWProtocol(IPMI20_SESSION_T *pSession,
                       AMIGetFWProtocolRes_T* pAMIGetFWProtocolRes,
                       int timeout)
{
    uint16 wRet;
    uint32 dwResLen;

    dwResLen = sizeof( AMIGetFWProtocolRes_T );
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                             ( ( NETFN_AMI << 2 ) + 0x00 ), CMD_AMI_GET_FW_PROTOCOL,
                                             NULL, 0,
                                             (uint8 *)pAMIGetFWProtocolRes, &dwResLen, timeout );
    return( wRet );
}

uint16 LIBIPMI_HL_AMIGetFWProtocol(IPMI20_SESSION_T *pSession,
                       int* protocol_type,
                       int timeout)
{
    uint16 wRet;
    AMIGetFWProtocolRes_T pAMIGetFWProtocolRes;

    wRet = IPMICMD_AMIGetFWProtocol(pSession, &pAMIGetFWProtocolRes, timeout);
    *protocol_type = pAMIGetFWProtocolRes.ProtocolType;

    return( wRet );
}

uint16 IPMICMD_AMIStartTFTPFwupdate(IPMI20_SESSION_T *pSession,
		AMIStartFwUpdateReq_T *pAMIStartFwUpdateReq,
		AMIStartFwUpdateRes_T *pAMIStartFwUpdateRes,
                       int timeout)
{
    uint16 wRet;
    uint32 dwResLen;
    uint32 data_size = sizeof(AMIStartFwUpdateReq_T);

    dwResLen = sizeof( AMIStartFwUpdateRes_T );
   // printf("IPMICMD_AMISetFWHostCfg() --> Parameter: %d,", pAMISetFWCfgReq->Parameter);
    printf("Size: %d \n", data_size);
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                             ( ( NETFN_AMI << 2 ) + 0x00 ), CMD_AMI_START_TFTP_FW_UPDATE,
                                             (uint8 *)pAMIStartFwUpdateReq, sizeof(AMIStartFwUpdateReq_T),
                                             (uint8 *)pAMIStartFwUpdateRes, &dwResLen, timeout );
    return( wRet );
}


uint16 IPMICMD_AMISetFWProtocol(IPMI20_SESSION_T *pSession,
                       AMISetFWProtocolReq_T* pAMISetFWProtocolReq,
                       AMISetFWProtocolRes_T* pAMISetFWProtocolRes,
                       int timeout)
{
    uint16 wRet;
    uint32 dwResLen;

    dwResLen = sizeof( AMISetFWProtocolRes_T );
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                             ( ( NETFN_AMI << 2 ) + 0x00 ), CMD_AMI_SET_FW_PROTOCOL,
                                             (uint8 *)pAMISetFWProtocolReq, sizeof(AMISetFWProtocolReq_T),
                                             (uint8 *)pAMISetFWProtocolRes, &dwResLen, timeout );
    return( wRet );
}

uint16 LIBIPMI_HL_AMISetFWProtocol(IPMI20_SESSION_T *pSession,
                       int protocol_type,
                       int timeout)
{
    uint16 wRet;
    AMISetFWProtocolReq_T pAMISetFWProtocolReq;
    AMISetFWProtocolRes_T pAMISetFWProtocolRes;

    pAMISetFWProtocolReq.ProtocolType = protocol_type;

    wRet = IPMICMD_AMISetFWProtocol(pSession, &pAMISetFWProtocolReq, &pAMISetFWProtocolRes, timeout);

    return( wRet );
}

uint16 IPMICMD_AMIGetFWCfg(IPMI20_SESSION_T *pSession,
					   AMIGetFWCfgReq_T* pAMIGetFWCfgReq,
					   AMIGetFWCfgRes_T* pAMIGetFWCfgRes,
                       int timeout)
{
    uint16 wRet;
    uint32 dwResLen;

    dwResLen = sizeof( AMIGetFWCfgRes_T );
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                             ( ( NETFN_AMI << 2 ) + 0x00 ), CMD_AMI_GET_FW_CONFIGURATION,
                                             (uint8 *)pAMIGetFWCfgReq, sizeof(AMIGetFWCfgReq_T),
                                             (uint8 *)pAMIGetFWCfgRes, &dwResLen, timeout );
    return( wRet );
}

uint16 LIBIPMI_HL_AMIGetFWCfg(IPMI20_SESSION_T *pSession,
				       AMIConfigTFTP_T *AMIConfigTFTP,
                       int timeout)
{
    uint16 wRet;
    AMIGetFWCfgReq_T pAMIGetFWCfgReq;
    AMIGetFWCfgRes_T pAMIGetFWCfgRes;

    //Get Host
    memset(&pAMIGetFWCfgReq, 0, sizeof(AMIGetFWCfgReq_T));
    pAMIGetFWCfgReq.Parameter = CONNECTION_INFO_HOST;
    wRet = IPMICMD_AMIGetFWCfg(pSession, &pAMIGetFWCfgReq, &pAMIGetFWCfgRes, timeout);
    memcpy(&AMIConfigTFTP->Host, pAMIGetFWCfgRes.Data, sizeof(AMIConfigTFTP->Host));

    //Get File Path
    memset(&pAMIGetFWCfgReq, 0, sizeof(AMIGetFWCfgReq_T));
    pAMIGetFWCfgReq.Parameter = CONNECTION_INFO_FILE_PATH;
    wRet = IPMICMD_AMIGetFWCfg(pSession, &pAMIGetFWCfgReq, &pAMIGetFWCfgRes, timeout);
    memcpy(&AMIConfigTFTP->RemoteFilePath, pAMIGetFWCfgRes.Data, sizeof(AMIConfigTFTP->RemoteFilePath));

    //Get Retry num
    memset(&pAMIGetFWCfgReq, 0, sizeof(AMIGetFWCfgReq_T));
    pAMIGetFWCfgReq.Parameter = CONNECTION_INFO_RETRY;
    wRet = IPMICMD_AMIGetFWCfg(pSession, &pAMIGetFWCfgReq, &pAMIGetFWCfgRes, timeout);
    memcpy(&AMIConfigTFTP->Retry, pAMIGetFWCfgRes.Data, sizeof(AMIConfigTFTP->Retry));

    return( wRet );
}

uint16 IPMICMD_AMISetFWHostCfg(IPMI20_SESSION_T *pSession,
		               AMISetFWCfgReq_T* pAMISetFWCfgReq,
		               AMISetFWCfgRes_T* pAMISetFWCfgRes,
                       int timeout)
{
    uint16 wRet;
    uint32 dwResLen;

    dwResLen = sizeof( AMIGetFWCfgRes_T );
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                             ( ( NETFN_AMI << 2 ) + 0x00 ), CMD_AMI_SET_FW_CONFIGURATION,
                                             (uint8 *)pAMISetFWCfgReq, sizeof(AMISetFWCfgReq_T),
                                             (uint8 *)pAMISetFWCfgRes, &dwResLen, timeout );
    return( wRet );
}

uint16 IPMICMD_AMISetFWFilePathCfg(IPMI20_SESSION_T *pSession,
		               AMISetFWCfgReq_T* pAMISetFWCfgReq,
		               AMISetFWCfgRes_T* pAMISetFWCfgRes,
                       int timeout)
{
    uint16 wRet;
    uint32 dwResLen;
    uint32 data_size = sizeof(AMISetFWCfgReq_T);

    dwResLen = sizeof( AMIGetFWCfgRes_T );
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                             ( ( NETFN_AMI << 2 ) + 0x00 ), CMD_AMI_SET_FW_CONFIGURATION,
                                             (uint8 *)pAMISetFWCfgReq, data_size,
                                             (uint8 *)pAMISetFWCfgRes, &dwResLen, timeout );
    return( wRet );
}

static uint16 IPMICMD_AMISetFWRetryCfg(IPMI20_SESSION_T *pSession,
		               AMISetFWCfgReq_T* pAMISetFWCfgReq,
		               AMISetFWCfgRes_T* pAMISetFWCfgRes,
                       int timeout)
{
    uint16 wRet;
    uint32 dwResLen;
    uint32 data_size = (sizeof(AMISetFWCfgReq_T) - 200 + 1);

    dwResLen = sizeof( AMIGetFWCfgRes_T );
    printf("IPMICMD_AMISetFWRetryCfg() --> Parameter: %d,", pAMISetFWCfgReq->Parameter);
    printf("Size: %d\n", data_size);

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                             ( ( NETFN_AMI << 2 ) + 0x00 ), CMD_AMI_SET_FW_CONFIGURATION,
                                             (uint8 *)pAMISetFWCfgReq, data_size,
                                             (uint8 *)pAMISetFWCfgRes, &dwResLen, timeout );
    return( wRet );
}

uint16 LIBIPMI_HL_AMISetFWCfg(IPMI20_SESSION_T *pSession,
		               AMIConfigTFTP_T *AMIConfigTFTP,
                       int timeout)
{
    uint16 wRet;
    //uint32 dwResLen;
    AMISetFWCfgReq_T pAMISetFWCfgReq;
    AMISetFWCfgRes_T pAMISetFWCfgRes;

    //Set Host
    memset(&pAMISetFWCfgReq, 0, sizeof(pAMISetFWCfgReq));
    pAMISetFWCfgReq.Parameter = CONNECTION_INFO_HOST;
    memcpy(pAMISetFWCfgReq.Data, AMIConfigTFTP->Host, sizeof(AMIConfigTFTP->Host));

    //dwResLen = sizeof( AMISetFWCfgRes_T );
    wRet = IPMICMD_AMISetFWHostCfg(pSession, &pAMISetFWCfgReq, &pAMISetFWCfgRes, timeout);

    //Set File Path
    memset(&pAMISetFWCfgReq, 0, sizeof(pAMISetFWCfgReq));
    pAMISetFWCfgReq.Parameter = CONNECTION_INFO_FILE_PATH;
    memcpy(pAMISetFWCfgReq.Data, AMIConfigTFTP->RemoteFilePath, sizeof(AMIConfigTFTP->RemoteFilePath));

    //dwResLen = sizeof( AMISetFWCfgRes_T );
    wRet = IPMICMD_AMISetFWFilePathCfg(pSession, &pAMISetFWCfgReq, &pAMISetFWCfgRes, timeout);

    //Set Retry
    memset(&pAMISetFWCfgReq, 0, sizeof(pAMISetFWCfgReq));
    pAMISetFWCfgReq.Parameter = CONNECTION_INFO_RETRY;
    memcpy(pAMISetFWCfgReq.Data, &AMIConfigTFTP->Retry, sizeof(AMIConfigTFTP->Retry));

    //dwResLen = sizeof( AMISetFWCfgRes_T );
    wRet = IPMICMD_AMISetFWRetryCfg(pSession, &pAMISetFWCfgReq, &pAMISetFWCfgRes, timeout);

    return( wRet );
}

/**
*@fn IPMICMD_AMIGetIPMISessionTimeOut
*@brief command to get the IPMI session timeout
*@param pSession - IPMI Session Handle
*@param pAMIGetIPMISessionTimeOutRes - Pointer to the response of AMI IPMI session timeout
*@param timeout - Timeout Value
**/
uint16 IPMICMD_AMIGetIPMISessionTimeOut(IPMI20_SESSION_T *pSession, AMIGetIPMISessionTimeOutRes_T *pAMIGetIPMISessionTimeOutRes, int timeout)
{
    uint16 wRet;
    uint32 dwResLen;
    dwResLen = sizeof(AMIGetIPMISessionTimeOutRes_T);

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                        (NETFN_AMI << 2), CMD_AMI_GET_IPMI_SESSION_TIMEOUT,
                        NULL, 0,
                        (uint8*)pAMIGetIPMISessionTimeOutRes, &dwResLen,
                        timeout);

    return (wRet);
}

/*
*@fn LIBIPMI_HL_AMIGetIPMISessionTimeOut
*@brief command to get the IPMI session timeout
*@param pSession - IPMI Session Handle
*@param pSessionTimeOut - Pointer to the IPMI session timeout
*@param timeout - Timeout Value
*@return Returns completion code
*/

uint16 LIBIPMI_HL_AMIGetIPMISessionTimeOut(IPMI20_SESSION_T *pSession, INT8U *pSessionTimeOut, int timeout)
{
    uint16 wRet;
    AMIGetIPMISessionTimeOutRes_T Res = {0,0};

    wRet = IPMICMD_AMIGetIPMISessionTimeOut(pSession, &Res, timeout);
    if(wRet != 0)
    {
        TCRIT("LIBIPMI: Error in getting IPMI session timeout in raw IPMI command layer::%d\n", wRet);
        return wRet;
    }
    memcpy(pSessionTimeOut,&Res.IPMISessionTimeOut,sizeof(Res.IPMISessionTimeOut));
    return wRet;
}
#if LIBIPMI_IS_OS_LINUX()
/*
*@fn LIBIPMI_HL_AMIGetUDSInfo
*@brief High Level command to retrieve the LAN Channel,BMC Instance using IP Address.
*@param BMCInst - Pointer which points to BMC Instance obtained.
*@param ChannelNum - Pointer which points to the received lan channel.
*@param timeout - Timeout Value.
*@return Returns LIBIPMI_E_SUCCESS on success.
*@return Returns proper completion code on error.
**/
uint16 LIBIPMI_HL_AMIGetUDSInfo(INT8U *IPAddr/*IN*/,INT8U *ChannelType/*OUT*/,INT8U *ChannelNum/*OUT*/,INT8U *BMCInst/*OUT*/,int timeout/*IN*/)
{
    uint16 wRet = 0;
    IPMI20_SESSION_T pSession;
    uint8_t byPrivLevel = PRIV_LEVEL_ADMIN;
    AMIGetUDSInfoRes_T pAMIGetUDSInfoRes;
    AMIGetUDSInfoReq_T pAMIGetUDSInfoReq;

    /* Establish Local Session with UDS Channel Number 0x0a for gettting LAN Channel and BMC  Instance to communicate */
    wRet = LIBIPMI_Create_IPMI_Local_Session(&pSession,"","",&byPrivLevel,NULL,AUTH_BYPASS_FLAG,timeout);
    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TCRIT("Cannot Establish IPMI Local Session\n");
        return wRet;
    }
    else
    {

        memset(&pAMIGetUDSInfoReq,0,sizeof(AMIGetUDSInfoReq_T));
        memcpy(pAMIGetUDSInfoReq.SessionIPAddr,IPAddr,sizeof(AMIGetUDSInfoReq_T));

        wRet = IPMICMD_AMIGetUDSInfo(&pSession,&pAMIGetUDSInfoReq,&pAMIGetUDSInfoRes,timeout);
        if(wRet == CC_NORMAL)
        {
             if(NULL != ChannelNum)
             {
                *ChannelNum = pAMIGetUDSInfoRes.ChannelNum;
             }
             if(NULL != ChannelType)
             {
                *ChannelType = pAMIGetUDSInfoRes.ChannelType;
             }
             if(NULL != BMCInst)
             {
                *BMCInst = pAMIGetUDSInfoRes.BMCInstance;
             }
            LIBIPMI_CloseSession(&pSession);
            return wRet;
        }
        else
        {
             if(NULL != ChannelNum)
             {
                *ChannelNum = 0xFF;
             }
             if(NULL != ChannelType)
             {
                *ChannelType = 0xFF;
             }
             if(NULL != BMCInst)
             {
                *BMCInst = 0;
             }
            TCRIT("Cannot get LAN Channel and BMC Instance used to create IPMI Local Session\n");
            LIBIPMI_CloseSession(&pSession);
            return wRet;
        }
    }

    return wRet;
}

/*
*@fn IPMICMD_AMIGetUDSInfo
*@brief
*@param pSession - IPMI Session Handle
*@param pAMIGetSensorInfoReq - Pointer to the request of AMI Get UDS Info command
*@param pAMIGetSensorInfoRes - Pointer to the response of AMI Get UDS Info command
*@param timeout - Timeout Value
**/
uint16 IPMICMD_AMIGetUDSInfo(IPMI20_SESSION_T *pSession,AMIGetUDSInfoReq_T *AMIGetUDSInfoReq,AMIGetUDSInfoRes_T *AMIGetUDSInfoRes,int timeout)
{
  uint16 wRet;
  uint32 dwResLen;

  dwResLen = sizeof(AMIGetUDSInfoRes_T);

  wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
		 (NETFN_AMI << 2), CMD_AMI_GET_UDS_CHANNEL_INFO,
                (uint8 *)AMIGetUDSInfoReq, sizeof(AMIGetUDSInfoReq_T),
                (uint8*)AMIGetUDSInfoRes, &dwResLen,
                timeout);

  return (wRet);
}
#endif

uint16 LIBIPMI_HL_AMIGetDualImage_FWBootSelector(IPMI20_SESSION_T *pSession,INT8U *FWBootSelector,int timeout)
{

    uint16 wRet = CC_NORMAL;
    INT8U ReqLen = 0;
    AMIDualImageSupReq_T pAMIDualImageSupReq;
    AMIDualImageSupRes_T pAMIDualImageSupRes;


    ReqLen = sizeof(AMIDualImageSupReq_T)-1;

    pAMIDualImageSupReq.Parameter = GETFWBOOTSELECTOR;

    wRet = IPMICMD_AMIDualImageSupport(pSession,
                       &pAMIDualImageSupReq,&pAMIDualImageSupRes,
                       ReqLen,timeout);
    if(CC_NORMAL == wRet)
    {
        *FWBootSelector = pAMIDualImageSupRes.BootSelOpt.GetBootSelector;
    }
    else
    {
        TCRIT("Unable to Get Firmware Boot Selector Value %x\n",wRet);
    }

    return wRet;
}

uint16 LIBIPMI_HL_AMIGetDualImage_FWUploadSelector(IPMI20_SESSION_T *pSession,INT8U *FWUploadSelector,int timeout)
{
    uint16 wRet = CC_NORMAL;
    INT8U ReqLen = 0;
    AMIDualImageSupReq_T pAMIDualImageSupReq;
    AMIDualImageSupRes_T pAMIDualImageSupRes;

    ReqLen = sizeof(AMIDualImageSupReq_T) - 1;

    pAMIDualImageSupReq.Parameter = GETFWUPLOADSELECTOR;

    wRet = IPMICMD_AMIDualImageSupport(pSession,
                       &pAMIDualImageSupReq,&pAMIDualImageSupRes,
                       ReqLen,timeout);
    if(CC_NORMAL == wRet)
    {
        *FWUploadSelector = pAMIDualImageSupRes.BootSelOpt.GetUploadSelector;
    }
    else
    {
        TCRIT("Unable to Get Firmware Upload Selector Value %x\n",wRet);
    }

    return wRet;
}

uint16 LIBIPMI_HL_AMIGetDualImage_RebootStatus(IPMI20_SESSION_T *pSession,INT8U *RebootStatus,int timeout)
{
    uint16 wRet = CC_NORMAL;
    INT8U ReqLen = 0;
    AMIDualImageSupReq_T pAMIDualImageSupReq;
    AMIDualImageSupRes_T pAMIDualImageSupRes;

    ReqLen = sizeof(AMIDualImageSupReq_T) - 1;

    pAMIDualImageSupReq.Parameter = GETREBOOTSTATUS;

    wRet = IPMICMD_AMIDualImageSupport(pSession,
                        &pAMIDualImageSupReq,&pAMIDualImageSupRes,
                        ReqLen,timeout);
    if(CC_NORMAL == wRet)
    {
        *RebootStatus = pAMIDualImageSupRes.BootSelOpt.GetRebootStatus;
    }
    else
    {
        TCRIT("Unable to get Firmware Reboot status %x\n",wRet);
    }

    return wRet;
}

uint16 LIBIPMI_HL_AMIGetDualImage_CurActiveImg(IPMI20_SESSION_T *pSession,INT8U *ActiveImg,int timeout)
{
    uint16 wRet = CC_NORMAL;
    INT8U ReqLen = 0;
    AMIDualImageSupReq_T pAMIDualImageSupReq;
    AMIDualImageSupRes_T pAMIDualImageSupRes;

    ReqLen = sizeof(AMIDualImageSupReq_T) - 1;

    pAMIDualImageSupReq.Parameter = GETCURACTIVEIMG;

    wRet = IPMICMD_AMIDualImageSupport(pSession,
                        &pAMIDualImageSupReq,&pAMIDualImageSupRes,
                        ReqLen,timeout);
    if(CC_NORMAL == wRet)
    {
        *ActiveImg = pAMIDualImageSupRes.BootSelOpt.GetCurActiveImg;
    }
    else
    {
        TCRIT("Unable to get Current Active Image %x\n",wRet);
    }

    return wRet;
}

uint16 LIBIPMI_HL_AMIGetDualImage_GetFWVersion(IPMI20_SESSION_T *pSession,INT8U Image, INT8U *MajVer, INT8U *MinVer,int timeout)
{
    uint16 wRet = CC_NORMAL;
    INT8U ReqLen = 0;
    AMIDualImageSupReq_T pAMIDualImageSupReq;
    AMIDualImageSupRes_T pAMIDualImageSupRes;

    ReqLen = sizeof(AMIDualImageSupReq_T);

    pAMIDualImageSupReq.Parameter = GETDUALIMGFWINFO;
    pAMIDualImageSupReq.BootSelector = Image;

    wRet = IPMICMD_AMIDualImageSupport(pSession,
                        &pAMIDualImageSupReq,&pAMIDualImageSupRes,
                        ReqLen,timeout);
    if(CC_NORMAL == wRet)
    {
        *MajVer = pAMIDualImageSupRes.BootSelOpt.GetFwVersion.MajVer;
        *MinVer = pAMIDualImageSupRes.BootSelOpt.GetFwVersion.MinVer;
    }
    else
    {
        TCRIT("Unable to get Current Active Image %x\n",wRet);
    }

    return wRet;
}

uint16 LIBIPMI_HL_AMIGetDualImage_GetFullFWVersion(IPMI20_SESSION_T *pSession,INT8U Image, INT8U *MajVer, INT8U *MinVer,INT32U *AuxVer, int timeout)
{
    uint16 wRet = CC_NORMAL;
    INT8U ReqLen = 0;
    AMIDualImageSupReq_T pAMIDualImageSupReq;
    AMIDualImageSupRes_T pAMIDualImageSupRes;

    ReqLen = sizeof(AMIDualImageSupReq_T);

    pAMIDualImageSupReq.Parameter = GETDUALIMGFULLFWINFO;
    pAMIDualImageSupReq.BootSelector = Image;

    wRet = IPMICMD_AMIDualImageSupport(pSession,
                        &pAMIDualImageSupReq,&pAMIDualImageSupRes,
                        ReqLen,timeout);
    if(CC_NORMAL == wRet)
    {
        *MajVer = pAMIDualImageSupRes.BootSelOpt.GetFullFwVersion.MajVer;
        *MinVer = pAMIDualImageSupRes.BootSelOpt.GetFullFwVersion.MinVer;
        *AuxVer = pAMIDualImageSupRes.BootSelOpt.GetFullFwVersion.Revision;
    }
    else
    {
        TCRIT("Unable to get Current Active Image %x\n",wRet);
    }

    return wRet;
}

uint16 LIBIPMI_HL_AMISetDualImage_FWBootSelector(IPMI20_SESSION_T *pSession,INT8U FWBootSelector,int timeout)
{
    uint16 wRet = CC_NORMAL;
    INT8U ReqLen = 0;
    AMIDualImageSupReq_T pAMIDualImageSupReq;
    AMIDualImageSupRes_T pAMIDualImageSupRes;

    ReqLen = sizeof(AMIDualImageSupReq_T);

    pAMIDualImageSupReq.Parameter = SETFWBOOTSELECTOR;
    pAMIDualImageSupReq.BootSelector = FWBootSelector;

    wRet = IPMICMD_AMIDualImageSupport(pSession,
                       &pAMIDualImageSupReq,&pAMIDualImageSupRes,
                       ReqLen,timeout);
    if(CC_NORMAL != wRet)
    {
        TCRIT("Unable to Set Firmware Boot Selector Value %x\n",wRet);
    }

    return wRet;
}

uint16 LIBIPMI_HL_AMISetDualImage_FWUploadSelector(IPMI20_SESSION_T *pSession,INT8U FWUploadSelector,int timeout)
{
    uint16 wRet = CC_NORMAL;
    INT8U ReqLen = 0;
    AMIDualImageSupReq_T pAMIDualImageSupReq;
    AMIDualImageSupRes_T pAMIDualImageSupRes;

    ReqLen = sizeof(AMIDualImageSupReq_T);

    pAMIDualImageSupReq.Parameter = SETFWUPLOADSELECTOR;
    pAMIDualImageSupReq.BootSelector = FWUploadSelector;

    wRet = IPMICMD_AMIDualImageSupport(pSession,
                       &pAMIDualImageSupReq,&pAMIDualImageSupRes,
                       ReqLen,timeout);
    if(CC_NORMAL != wRet)
    {
        TCRIT("Unable to Set Firmware Upload Selector Value %x\n",wRet);
    }

    return wRet;
}

uint16 LIBIPMI_HL_AMISetDualImage_RebootStatus(IPMI20_SESSION_T *pSession,INT8U RebootStatus,int timeout)
{
    uint16 wRet = CC_NORMAL;
    INT8U ReqLen = 0;
    AMIDualImageSupReq_T pAMIDualImageSupReq;
    AMIDualImageSupRes_T pAMIDualImageSupRes;

    ReqLen = sizeof(AMIDualImageSupReq_T);

    pAMIDualImageSupReq.Parameter = SETREBOOTSTATUS;
    pAMIDualImageSupReq.BootSelector = RebootStatus;

    wRet = IPMICMD_AMIDualImageSupport(pSession,
                        &pAMIDualImageSupReq,&pAMIDualImageSupRes,
                        ReqLen,timeout);
    if(CC_NORMAL != wRet)
    {
        TCRIT("Unable to Set Firmware Reboot status %x\n",wRet);
    }

    return wRet;
}

uint16 IPMICMD_AMIDualImageSupport(IPMI20_SESSION_T *pSession,
                       AMIDualImageSupReq_T *pAMIDualImageSupReq,
                       AMIDualImageSupRes_T *pAMIDualImageSupRes,
                       INT8U ReqLen,int timeout)
{
    uint16 wRet;
    uint32 dwResLen;

    dwResLen = sizeof(AMIDualImageSupRes_T);

    if(0xFF == pSession->g_NetFnOEM)
    {
        NetFn_OEM = NETFN_AMI;
    }
    else
    {
        NetFn_OEM = pSession->g_NetFnOEM;
    }

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                                                                                   ( ( NetFn_OEM << 2 ) + 0x00 ),CMD_AMI_DUAL_IMG_SUPPORT,
                                                                                   (uint8 *)pAMIDualImageSupReq,ReqLen,
                                                                                   (uint8 *)pAMIDualImageSupRes, &dwResLen, timeout);
    return( wRet);
}

/*-------------------------------------U-Boot Memory Test--------------------------------------------*/
uint16 LIBIPMI_HL_AMISetUBootMemtest(IPMI20_SESSION_T *pSession,INT8U Enablememtest,int timeout)
{
    uint16 wRet = CC_NORMAL;
    INT8U ReqLen = 0;
    AMISetUBootMemtestReq_T pAMISetUBootMemtestReq;
    AMISetUBootMemtestRes_T pAMISetUBootMemtestRes;

    ReqLen = sizeof(pAMISetUBootMemtestReq);

    pAMISetUBootMemtestReq.EnableMemoryTest = Enablememtest;

    wRet = IPMICMD_AMISetUBootMemtest(pSession,
                        &pAMISetUBootMemtestReq,&pAMISetUBootMemtestRes,
                        ReqLen,timeout);
    if(CC_NORMAL != wRet)
    {
        TCRIT("Unable to Set U-Boot Memory Test%x\n",wRet);
    }

    return wRet;

}

uint16 LIBIPMI_HL_AMIGetUBootMemtestStatus(IPMI20_SESSION_T *pSession,INT8U *memtest,int timeout)
{
    uint16 wRet = CC_NORMAL;
    AMIGetUBootMemtestStatusRes_T pAMISetUBootMemtestRes;


    wRet = IPMICMD_AMIGetUBootMemtestStatus(pSession,
                        &pAMISetUBootMemtestRes,
                        timeout);
    if(CC_NORMAL != wRet)
    {
        TCRIT("Unable to Set U-Boot Memory Test%x\n",wRet);
    }

    *memtest = pAMISetUBootMemtestRes.MemtestStatus;
    return wRet;

}

uint16 IPMICMD_AMISetUBootMemtest(IPMI20_SESSION_T *pSession,
                       AMISetUBootMemtestReq_T *pAMISetUBootMemtestReq,
                       AMISetUBootMemtestRes_T *pAMISetUBootMemtestRes,
                       INT8U ReqLen,int timeout)
{
    uint16 wRet;
    uint32 dwResLen;

    dwResLen = sizeof(AMISetUBootMemtestRes_T);

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                                                                                   ( ( NETFN_AMI << 2 ) + 0x00 ),CMD_AMI_SET_UBOOT_MEMTEST,
                                                                                   (uint8 *)pAMISetUBootMemtestReq,ReqLen,
                                                                                   (uint8 *)pAMISetUBootMemtestRes, &dwResLen, timeout);
    return( wRet);
}

uint16 IPMICMD_AMIGetUBootMemtestStatus(IPMI20_SESSION_T *pSession,
                       AMIGetUBootMemtestStatusRes_T *pAMIGetUBootMemtestStatusRes,
                       int timeout)
{
    uint8 Req [20];
    uint16 wRet;
    uint32 dwResLen;

    dwResLen = sizeof(AMIGetUBootMemtestStatusRes_T);

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                                                                                   ( ( NETFN_AMI << 2 ) + 0x00 ),CMD_AMI_GET_UBOOT_MEMTEST_STATUS,
                                                                                   Req,0,
                                                                                   (uint8 *)pAMIGetUBootMemtestStatusRes, &dwResLen, timeout);
    return( wRet);
}

uint16 IPMICMD_AMIGetExtendedPrivilege( IPMI20_SESSION_T *pSession,
                                uint8 uid,
                                INT32U *ExtendedPriv,
                                int timeout )
{
        uint16 wRet;
        uint32 dwResLen;

        AMIGetExtendedPrivRes_T ExtendedPrivStatus;

        dwResLen = sizeof( AMIGetExtendedPrivRes_T  );
        memset(&ExtendedPrivStatus, 0, dwResLen);

        wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                             ((NETFN_AMI << 2) + 0x00), CMD_AMI_GET_EXTENDED_PRIV,
                                             (uint8 *)&uid, sizeof(AMIGetExtendedPrivReq_T),
                                             (uint8 *)&ExtendedPrivStatus, &dwResLen, timeout );

        if( wRet == 0)
        {
            *ExtendedPriv = ExtendedPrivStatus.Extendedpriv;
        }

        return( wRet );
}

uint16 IPMICMD_AMISetExtendedPrivilege( IPMI20_SESSION_T *pSession,
                                uint8 uid,
                                INT32U ExtendedPriv,
                                int timeout )
{
        uint16 wRet;
        uint32 dwResLen;
        uint32 dwReqLen;
        INT8U *pRes;

        AMISetExtendedPrivReq_T ExtendedPrivStatus;
        dwReqLen = sizeof( AMISetExtendedPrivReq_T );
        memset(&ExtendedPrivStatus, 0, dwReqLen);
        ExtendedPrivStatus.UserID = uid;
        ExtendedPrivStatus.Extendedpriv = ExtendedPriv;


        wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                             ((NETFN_AMI << 2) + 0x00), CMD_AMI_SET_EXTENDED_PRIV,
                                             (uint8 *)&ExtendedPrivStatus, sizeof(AMISetExtendedPrivReq_T),
                                             (uint8 *)&pRes, &dwResLen, timeout );


        return( wRet );
}

uint16 IPMICMD_AMISetFWSelectCfg(IPMI20_SESSION_T *pSession,
		               AMISetFWCfgReq_T* pAMISetFWCfgReq,
		               AMISetFWCfgRes_T* pAMISetFWCfgRes,
                       int timeout)
{
    uint16 wRet;
    uint32 dwResLen;
    uint32 data_size = (sizeof(AMISetFWCfgReq_T) - 200 + 1);

    dwResLen = sizeof( AMIGetFWCfgRes_T );

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                             ( ( NETFN_AMI << 2 ) + 0x00 ), CMD_AMI_SET_FW_CONFIGURATION,
                                             (uint8 *)pAMISetFWCfgReq, data_size,
                                             (uint8 *)pAMISetFWCfgRes, &dwResLen, timeout );
    return( wRet );
}


uint16 LIBIPMI_HL_AMISetFWSelectCfg(IPMI20_SESSION_T *pSession,
		               AMIConfigFWUpdateSelect_T *AMIConfigFWUpdateSelect,
                       int timeout)
{
    uint16 wRet;
//    uint32 dwResLen;
    AMISetFWCfgReq_T pAMISetFWCfgReq;
    AMISetFWCfgRes_T pAMISetFWCfgRes;

    //Set FWSelect
    memset(&pAMISetFWCfgReq, 0, sizeof(pAMISetFWCfgReq));
    pAMISetFWCfgReq.Parameter = FW_UPDATE_SELECT;
    memcpy(pAMISetFWCfgReq.Data, &AMIConfigFWUpdateSelect->FWSelect, sizeof(AMIConfigFWUpdateSelect->FWSelect));

  //  dwResLen = sizeof( AMISetFWCfgRes_T );
    wRet = IPMICMD_AMISetFWSelectCfg(pSession, &pAMISetFWCfgReq, &pAMISetFWCfgRes, timeout);

    return( wRet );
}

uint16 LIBIPMI_HL_AMIGetFWSelectCfg(IPMI20_SESSION_T *pSession,
				       AMIConfigFWUpdateSelect_T *AMIConfigFWUpdateSelect,
                       int timeout)
{
    uint16 wRet;
    AMIGetFWCfgReq_T pAMIGetFWCfgReq;
    AMIGetFWCfgRes_T pAMIGetFWCfgRes;

    //Get Retry num
    memset(&pAMIGetFWCfgReq, 0, sizeof(AMIGetFWCfgReq_T));
    pAMIGetFWCfgReq.Parameter = FW_UPDATE_SELECT;
    wRet = IPMICMD_AMIGetFWCfg(pSession, &pAMIGetFWCfgReq, &pAMIGetFWCfgRes, timeout);
    memcpy(&AMIConfigFWUpdateSelect->FWSelect, pAMIGetFWCfgRes.Data, sizeof(AMIConfigFWUpdateSelect->FWSelect));

    return( wRet );
}

/**
*@fn IPMICMD_AMISetTimeZone
*@brief Command used to set the time zone.
*@param pSession    [in]Session handle
*@param pAMISetTimeZoneReq Request data time zone string
*@param timeout timeout value in seconds.
*@param ReqLen Length of the Requset data
*@retval Returns LIBIPMI_STATUS_SUCCESS on success and error codes on failure
*/
uint16 IPMICMD_AMISetTimeZone(IPMI20_SESSION_T *pSession,
               AMISetTimeZone_T* pAMISetTimeZoneReq,int ReqLen, int timeout)
{
    uint16 wRet;
    uint32 dwResLen = 0;
    INT8U pAMISetTimeZoneRes;

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                                           ( ( NETFN_AMI << 2 ) + 0x00 ),CMD_AMI_SET_TIMEZONE,
                                           (uint8 *)pAMISetTimeZoneReq,ReqLen,
                                           (uint8 *)&pAMISetTimeZoneRes, &dwResLen, timeout);
    return( wRet);
}

/**
*@fn IPMICMD_AMIGetTimeZone
*@brief Command gives the current time zone.
*@param pSession    [in]Session handle
*@param pAMIGetTimeZoneRes Response data of time zone string
*@param timeout timeout value in seconds.
*@param pResLen Length of the Response data
*@retval Returns LIBIPMI_STATUS_SUCCESS on success and error codes on failure
*/
uint16 IPMICMD_AMIGetTimeZone( IPMI20_SESSION_T *pSession,
               AMISetTimeZone_T* pAMIGetTimeZoneRes, uint32 *pResLen, int timeout )
{
    uint16 wRet;
    uint8 *pReq = NULL;
    AMIGetTimeZone_T TimeZoneRes;

    memset(&TimeZoneRes, 0, sizeof (AMIGetTimeZone_T));
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                         ((NETFN_AMI << 2) + 0x00), CMD_AMI_GET_TIMEZONE,
                                         pReq, 0,
                                         (uint8 *)&TimeZoneRes, pResLen, timeout );

    memcpy (pAMIGetTimeZoneRes->ZoneName, TimeZoneRes.ZoneName, *pResLen);
    return( wRet );
}

uint16 LIBIPMI_HL_AMIGetTimeZone(IPMI20_SESSION_T *pSession,
	AMISetTimeZone_T* TimeZoneRes, int timeout)
{
	uint16 wRet;
	uint32 dwResLen = 0;
	wRet = IPMICMD_AMIGetTimeZone(pSession, TimeZoneRes, &dwResLen, timeout);
	if (wRet != 0)
	{
		TCRIT("Getting Timezone configuration error::%d\n", wRet);
	}
	return (wRet);
}

uint16 LIBIPMI_HL_AMISetTimeZone(IPMI20_SESSION_T *pSession,
	char* timezone, int timeout)
{
	uint16 wRet;
	uint32 ReqLen = TIME_ZONE_LEN - 1;
	AMISetTimeZone_T TimeZoneRes;
	memset(&TimeZoneRes, 0, sizeof(AMISetTimeZone_T));
	strncpy((char*)TimeZoneRes.ZoneName, timezone, TIME_ZONE_LEN-1);
	wRet = IPMICMD_AMISetTimeZone(pSession, &TimeZoneRes, ReqLen, timeout);
	if (wRet != 0)
	{
		TCRIT("Error in setting Timezone configurations::%d", wRet);
	}
	return (wRet);
}

/**
*@fn IPMICMD_AMIGetAllPreserveConfStatus
*@brief Command gives the current preserve configuration.
*@param pSession    [in]Session handle
*@param pGetAllPreserveConfigRes  Response data of Preserve Configuration Status
*@param timeout timeout value in seconds.
*@param ReqLen Length of the Request data
*@retval Returns LIBIPMI_STATUS_SUCCESS on success and error codes on failure
*/

uint16 IPMICMD_AMIGetAllPreserveConfStatus(IPMI20_SESSION_T *pSession,
                       GetAllPreserveConfigRes_T *pGetAllPreserveConfigRes,
                       INT8U ReqLen,int timeout)
{
    uint16 wRet;
    uint32 dwResLen;

    dwResLen = sizeof(GetAllPreserveConfigRes_T);

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                                                                                   ( ( NETFN_AMI << 2 ) + 0x00 ),CMD_AMI_GET_ALL_PRESERVE_CONF,
                                                                                   (uint8 *)NULL,ReqLen,
                                                                                   (uint8 *)pGetAllPreserveConfigRes, &dwResLen, timeout);
    return( wRet);
}

/**
*@fn IPMICMD_AMISetAllPreserveConfStatus
*@brief Command sets the preserve configuration.
*@param pSession    [in]Session handle
*@param pSetAllPreserveConfigReq  Request data of Set Preserve Configuration Status
*@param pSetAllPreserveConfigRes  Response data of Set Preserve Configuration Status
*@param timeout timeout value in seconds.
*@param ReqLen Length of the Request data
*@retval Returns LIBIPMI_STATUS_SUCCESS on success and error codes on failure
*/

uint16 IPMICMD_AMISetAllPreserveConfStatus(IPMI20_SESSION_T *pSession,
                       SetAllPreserveConfigReq_T *pSetAllPreserveConfigReq,
                       SetAllPreserveConfigRes_T *pSetAllPreserveConfigRes,
                       INT8U ReqLen,int timeout)
{
    uint16 wRet;
    uint32 dwResLen;

    dwResLen = sizeof(SetAllPreserveConfigRes_T);

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                                                                                   ( ( NETFN_AMI << 2 ) + 0x00 ),CMD_AMI_SET_ALL_PRESERVE_CONF,
                                                                                   (uint8 *)pSetAllPreserveConfigReq,ReqLen,
                                                                                   (uint8 *)pSetAllPreserveConfigRes, &dwResLen, timeout);
    return( wRet);
}



/**
*@fn IPMICMD_AMIGetNTPCfg
*@brief Command gets the NTP  configuration.
*@param pSession    [in]Session handle
*@param pAMIGetNTPCfgRes  Response data of Get NTP Configuration
*@param timeout timeout value in seconds.
*@retval Returns LIBIPMI_STATUS_SUCCESS on success and error codes on failure
*/

uint16 IPMICMD_AMIGetNTPCfg( IPMI20_SESSION_T *pSession,
               AMIGetNTPCfgRes_T* pAMIGetNTPCfgRes, int timeout )
{
    uint16 wRet;
    uint8 *pReq = NULL;
    AMIGetNTPCfgRes_T NTPCfgRes;
    uint32 ResLen=sizeof(AMIGetNTPCfgRes_T);

    memset(&NTPCfgRes, 0, sizeof (AMIGetNTPCfgRes_T));
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                         ((NETFN_AMI << 2) + 0x00), CMD_AMI_GET_NTP_CFG,
                                         pReq, 0,
                                         (uint8 *)&NTPCfgRes,& ResLen, timeout );

    memcpy (pAMIGetNTPCfgRes,&NTPCfgRes, sizeof(AMIGetNTPCfgRes_T));
    return( wRet );
}

/**
*@fn IPMICMD_AMISetNTPCfg
*@brief Command sets the NTP  configuration.
*@param pSession    [in]Session handle
*@param pAMISetNTPCfgReq  Request data of Set NTP Configuration
*@param timeout timeout value in seconds.
*@retval Returns LIBIPMI_STATUS_SUCCESS on success and error codes on failure
*/

uint16 IPMICMD_AMISetNTPCfg( IPMI20_SESSION_T *pSession,
               AMISetNTPCfgReq_T* pAMISetNTPCfgReq,INT8U ReqLen, int timeout )
{
    uint16 wRet;
    AMISetNTPCfgRes_T NTPCfgRes;
    uint32 wResLen = sizeof(AMISetNTPCfgRes_T);
    memset(&NTPCfgRes, 0, sizeof (AMISetNTPCfgRes_T));
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                         ((NETFN_AMI << 2) + 0x00), CMD_AMI_SET_NTP_CFG,
                                         (uint8 *)pAMISetNTPCfgReq,ReqLen,
                                         (uint8 *)&NTPCfgRes,&wResLen, timeout );
    return( wRet );
}

uint16 LIBIPMI_HL_AMIGetNTPCfg(IPMI20_SESSION_T *pSession,
	AMIGetNTPCfgRes_T* pAMIGetNTPCfgRes, int timeout)
{
	uint16 wRet;
	wRet=IPMICMD_AMIGetNTPCfg( pSession, pAMIGetNTPCfgRes, timeout );
	return( wRet );
}

uint16 LIBIPMI_HL_AMISetNTPCfg(IPMI20_SESSION_T *pSession,
	char * server_name1, char * server_name2, int status, int timeout)
{
	uint16 wRet= 0;
	int Reqlen = 0;
	AMISetNTPCfgReq_T AMISetNTPCfgReq;
	memset (&AMISetNTPCfgReq , 0, sizeof(AMISetNTPCfgReq_T));
	if( status == AUTO )
	{
	Reqlen = MAX_SERVER_LEN + 1;
	if (strlen(server_name1) != 0)
	{
		AMISetNTPCfgReq.param = PARAM_SERVER_PRIM;
		strncpy((char*)AMISetNTPCfgReq.ntpconf.Server, server_name1, MAX_SERVER_LEN-1);
		wRet = IPMICMD_AMISetNTPCfg( pSession,&AMISetNTPCfgReq,Reqlen,timeout );

		if( wRet != 0)
		{
			TCRIT("Error in setting NTP primary server::%d\n", wRet);
		 	return wRet;
		}
	}
	AMISetNTPCfgReq.param = PARAM_SERVER_SEC;
	strncpy((char*)AMISetNTPCfgReq.ntpconf.Server, server_name2, MAX_SERVER_LEN-1);
	wRet = IPMICMD_AMISetNTPCfg( pSession,&AMISetNTPCfgReq,Reqlen,timeout );

	if( wRet != 0)
	{
		TCRIT("Error in setting NTP secondary server::%d\n", wRet);
		return wRet;
	}

}
	Reqlen = 2;
	AMISetNTPCfgReq.param = PARAM_STATUS;
	AMISetNTPCfgReq.ntpconf.Status = status;
	wRet = IPMICMD_AMISetNTPCfg( pSession,&AMISetNTPCfgReq,Reqlen,timeout );
	return (wRet);
}

/**
*@fn IPMICMD_AMIYAFUReplaceSignedImageKey
*@brief Command sets the Public Key
*@param pSession    [in]Session handle
*@param pAMITYafuSignedImageKeyReq  Request data(Public Key).
*@param timeout timeout value in seconds.
*@retval Returns LIBIPMI_STATUS_SUCCESS on success and error codes on failure
*/
uint16 IPMICMD_AMIYAFUReplaceSignedImageKey( IPMI20_SESSION_T *pSession,
               INT8U* pReq,INT32U ReqLen, int timeout )
{
    uint16 wRet;
    AMIYafuSignedImageKeyRes_T pAMITYafuSignedImageKeyRes;
    uint32 wResLen = sizeof(AMIYafuSignedImageKeyRes_T);

    memset(&pAMITYafuSignedImageKeyRes, 0, sizeof (AMIYafuSignedImageKeyRes_T));

    if(0xFF == pSession->g_NetFnOEM)
    {
        NetFn_OEM = NETFN_AMI;
    }
    else
    {
        NetFn_OEM = pSession->g_NetFnOEM;
    }

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                         ((NetFn_OEM << 2) + 0x00), CMD_AMI_YAFU_SIGNIMAGEKEY_REPLACE,
                                         pReq,ReqLen,
                                         (uint8 *)&pAMITYafuSignedImageKeyRes,&wResLen, timeout );
    return( wRet );
}

uint16 LIBIPMI_HL_AMIYAFUReplaceSignedImageKey(IPMI20_SESSION_T *pSession,INT8U *PubKey,INT32U Size,int timeout)
{
    uint16 wRet = CC_NORMAL;
    INT32U ReqLen = 0;
    AMIYafuSignedImageKeyReq_T pAMITYafuSignedImageKeyReq;

    ReqLen = Size;

    memcpy(&pAMITYafuSignedImageKeyReq.PubKey[0],PubKey,Size);

     wRet = IPMICMD_AMIYAFUReplaceSignedImageKey(pSession, (INT8U *)&pAMITYafuSignedImageKeyReq,ReqLen,timeout);
    return wRet;
}

uint16 LIBIPMI_HL_AMIPublicKeyUpload(IPMI20_SESSION_T *pSession,INT8U FuncID,INT8U *PubKey,INT32U Size,int timeout)
{
    uint16 wRet = CC_NORMAL;
    INT32U ReqLen = 0;
    AMIPublicKeyUploadReq_T AMIPublicKeyUploadReq;

    ReqLen = Size + MAX_KEY_SIG_SIZE + sizeof(INT8U);

    AMIPublicKeyUploadReq.FuncID = FuncID;
    memcpy(&AMIPublicKeyUploadReq.KeySig, PUB_KEY_SIG, MAX_KEY_SIG_SIZE);
    memcpy(&AMIPublicKeyUploadReq.PubKey[0],PubKey,Size);

     wRet = IPMICMD_AMIYAFUReplaceSignedImageKey(pSession, (INT8U *)&AMIPublicKeyUploadReq,ReqLen,timeout);
    return wRet;
}

/**
*@fn IPMICMD_AMIVirtualDeviceSetStatus
*@brief Command sets the Virtual Device Enable/Disable status
*@param pSession    [in]Session handle
*@param pAMIVirtualDeviceSetStatusReq  Request data enable/disable the Virtual Device.
*@param timeout timeout value in seconds.
*@retval Returns LIBIPMI_STATUS_SUCCESS on success and error codes on failure
*/
uint16 IPMICMD_AMIVirtualDeviceSetStatus( IPMI20_SESSION_T *pSession,
		AMIVirtualDeviceSetStatusReq_T* pAMIVirtualDeviceSetStatusReq, int timeout )
{
    uint16 wRet;
    AMIVirtualDeviceSetStatusRes_T pAMIVirtualDeviceSetStatusRes;
    uint32 wResLen = sizeof(AMIVirtualDeviceSetStatusRes_T);
    uint32 wReqLen = sizeof(AMIVirtualDeviceSetStatusReq_T);
    memset(&pAMIVirtualDeviceSetStatusRes, 0, sizeof (AMIVirtualDeviceSetStatusRes_T));

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                         ((NETFN_AMI << 2) + 0x00), CMD_AMI_VIRTUAL_DEVICE_SET_STATUS,
                                         (uint8 *)pAMIVirtualDeviceSetStatusReq,wReqLen,
                                         (uint8 *)&pAMIVirtualDeviceSetStatusRes,&wResLen, timeout );
    return( wRet );
}

uint16 LIBIPMI_HL_AMIVirtualDeviceSetStatus(IPMI20_SESSION_T *pSession,INT8U Command,int timeout)
{
    uint16 wRet = CC_NORMAL;
    AMIVirtualDeviceSetStatusReq_T pAMIVirtualDeviceSetStatusReq;
    memset(&pAMIVirtualDeviceSetStatusReq, 0, sizeof(AMIVirtualDeviceSetStatusReq_T));

    pAMIVirtualDeviceSetStatusReq.Status = Command;
    wRet = IPMICMD_AMIVirtualDeviceSetStatus(pSession,&pAMIVirtualDeviceSetStatusReq,timeout);
    return wRet;
}

/**
*@fn IPMICMD_AMISetHostLockFeatureStatus
*@brief Command sets the Virtual Device Enable/Disable status
*@param pSession    [in]Session handle
*@param pAMIVirtualDeviceSetStatusReq  Request data enable/disable the Virtual Device.
*@param timeout timeout value in seconds.
*@retval Returns LIBIPMI_STATUS_SUCCESS on success and error codes on failure
*/
uint16 IPMICMD_AMISetHostLockFeatureStatus( IPMI20_SESSION_T *pSession,
		AMISetHostLockFeatureStatusReq_T* pAMISetHostLockFeatureStatusReq, int timeout )
{
    uint16 wRet;

    AMISetHostLockFeatureStatusRes_T pAMISetHostLockFeatureStatusRes;
    uint32 wResLen = sizeof(AMISetHostLockFeatureStatusRes_T);
    uint32 wReqLen = sizeof(AMISetHostLockFeatureStatusReq_T);
    memset(&pAMISetHostLockFeatureStatusRes, 0, sizeof (AMISetHostLockFeatureStatusRes_T));

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                         ((NETFN_AMI << 2) + 0x00), CMD_AMI_SET_HOST_LOCK_FEATURE_STATUS,
                                         (uint8 *)pAMISetHostLockFeatureStatusReq,wReqLen,
                                         (uint8 *)&pAMISetHostLockFeatureStatusRes,&wResLen, timeout );
    return( wRet );
}

uint16 LIBIPMI_HL_AMISetHostLockFeatureStatus(IPMI20_SESSION_T *pSession,INT8U Command,int timeout)
{
    uint16 wRet = CC_NORMAL;
    AMISetHostLockFeatureStatusReq_T pAMISetHostLockFeatureStatusReq;
    memset(&pAMISetHostLockFeatureStatusReq, 0, sizeof(AMISetHostLockFeatureStatusReq_T));

    pAMISetHostLockFeatureStatusReq.LockFeatureStatus = Command;
    wRet = IPMICMD_AMISetHostLockFeatureStatus(pSession,&pAMISetHostLockFeatureStatusReq,timeout);
    return wRet;
}
/**
*@fn IPMICMD_AMIVirtualDeviceGetStatus
*@brief Command sets the Virtual Device Enable/Disable status
*@param pSession    [in]Session handle
*@param pAMIVirtualDeviceGetStatusReq  Request data enable/disable the Virtual Device.
*@param timeout timeout value in seconds.
*@retval Returns LIBIPMI_STATUS_SUCCESS on success and error codes on failure
*/
uint16 IPMICMD_AMIVirtualDeviceGetStatus( IPMI20_SESSION_T *pSession,
		 AMIVirtualDeviceGetStatusRes_T *pAMIVirtualDeviceGetStatusRes, int timeout )
{
    uint16 wRet;
    uint32 wResLen = sizeof(AMIVirtualDeviceGetStatusRes_T);
    uint8 *pReq = NULL;

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                         ((NETFN_AMI << 2) + 0x00), CMD_AMI_VIRTUAL_DEVICE_GET_STATUS,
                                         (uint8 *)pReq,0,
                                         (uint8 *)pAMIVirtualDeviceGetStatusRes,&wResLen, timeout );
    return( wRet );
}

uint16 LIBIPMI_HL_AMIVirtualDeviceGetStatus(IPMI20_SESSION_T *pSession,INT8U *Status,int timeout)
{
    uint16 wRet = CC_NORMAL;

    AMIVirtualDeviceGetStatusRes_T pAMIVirtualDeviceGetStatusRes;
    memset(&pAMIVirtualDeviceGetStatusRes, 0, sizeof (AMIVirtualDeviceGetStatusRes_T));

    wRet = IPMICMD_AMIVirtualDeviceGetStatus(pSession,&pAMIVirtualDeviceGetStatusRes,timeout);
    *Status = pAMIVirtualDeviceGetStatusRes.Status;
    return wRet;
}


/**
*@fn IPMICMD_AMIGetHostLockFeatureStatus
*@brief Command Gets theHost lock feature is Enabled/disabled at runtime
*@param pSession    [in]Session handle
*@param pAMIVirtualDeviceGetStatusReq  Request data enable/disable the Virtual Device.
*@param timeout timeout value in seconds.
*@retval Returns LIBIPMI_STATUS_SUCCESS on success and error codes on failure
*/
uint16 IPMICMD_AMIGetHostLockFeatureStatus( IPMI20_SESSION_T *pSession,
		AMIGetHostLockFeatureStatusRes_T *pAMIGetHostLockFeatureStatusRes, int timeout )
{
    uint16 wRet;
    uint32 wResLen = sizeof(AMIGetHostLockFeatureStatusRes_T);
    uint8 *pReq = NULL;

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                         ((NETFN_AMI << 2) + 0x00), CMD_AMI_GET_HOST_LOCK_FEATURE_STATUS,
                                         (uint8 *)pReq,0,
                                         (uint8 *)pAMIGetHostLockFeatureStatusRes,&wResLen, timeout );
    return( wRet );
}

uint16 LIBIPMI_HL_AMIGetHostLockFeatureStatus(IPMI20_SESSION_T *pSession,INT8U *Status,int timeout)
{
    uint16 wRet = CC_NORMAL;

    AMIGetHostLockFeatureStatusRes_T pAMIGetHostLockFeatureStatusRes;
    memset(&pAMIGetHostLockFeatureStatusRes, 0, sizeof (AMIGetHostLockFeatureStatusRes_T));

    wRet = IPMICMD_AMIGetHostLockFeatureStatus(pSession,&pAMIGetHostLockFeatureStatusRes,timeout);
    *Status = pAMIGetHostLockFeatureStatusRes.LockFeatureStatus;
    return wRet;
}

/**
*@fn IPMICMD_AMIGetHostLockFeatureStatus
*@brief Command Gets theHost lock feature is Enabled/disabled at runtime
*@param pSession    [in]Session handle
*@param pAMIVirtualDeviceGetStatusReq  Request data enable/disable the Virtual Device.
*@param timeout timeout value in seconds.
*@retval Returns LIBIPMI_STATUS_SUCCESS on success and error codes on failure
*/
uint16 IPMICMD_AMIGetHostAutoLockStatus( IPMI20_SESSION_T *pSession,
		AMIGetHostAutoLockStatusRes_T *pAMIGetHostAutoLockStatusRes, int timeout )
{
    uint16 wRet;
    uint32 wResLen = sizeof(AMIGetHostAutoLockStatusRes_T);
    uint8 *pReq = NULL;

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                         ((NETFN_AMI << 2) + 0x00), CMD_AMI_GET_HOST_AUTO_LOCK_STATUS,
                                         (uint8 *)pReq,0,
                                         (uint8 *)pAMIGetHostAutoLockStatusRes,&wResLen, timeout );
    return( wRet );
}

uint16 LIBIPMI_HL_AMIGetHostAutoLockStatus(IPMI20_SESSION_T *pSession,INT8U *Status,int timeout)
{
    uint16 wRet = CC_NORMAL;

    AMIGetHostAutoLockStatusRes_T pAMIGetHostAutoLockStatusRes;
    memset(&pAMIGetHostAutoLockStatusRes, 0, sizeof (AMIGetHostAutoLockStatusRes_T));

    wRet = IPMICMD_AMIGetHostAutoLockStatus(pSession,&pAMIGetHostAutoLockStatusRes,timeout);
    *Status = pAMIGetHostAutoLockStatusRes.LockStatus;
    return wRet;
}

/**
*@fn IPMICMD_AMISetHostLockFeatureStatus
*@brief Command sets the Virtual Device Enable/Disable status
*@param pSession    [in]Session handle
*@param pAMIVirtualDeviceSetStatusReq  Request data enable/disable the Virtual Device.
*@param timeout timeout value in seconds.
*@retval Returns LIBIPMI_STATUS_SUCCESS on success and error codes on failure
*/
uint16 IPMICMD_AMISetHostAutoLockStatus( IPMI20_SESSION_T *pSession,
		AMISetHostAutoLockStatusReq_T* pAMISetHostAutoLockStatusReq, int timeout )
{
    uint16 wRet;

    AMISetHostAutoLockStatusRes_T pAMISetHostAutoLockStatusRes;
    uint32 wResLen = sizeof(AMISetHostAutoLockStatusRes_T);
    uint32 wReqLen = sizeof(AMISetHostAutoLockStatusReq_T);
    memset(&pAMISetHostAutoLockStatusRes, 0, sizeof (AMISetHostAutoLockStatusRes_T));

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                         ((NETFN_AMI << 2) + 0x00), CMD_AMI_SET_HOST_AUTO_LOCK_STATUS,
                                         (uint8 *)pAMISetHostAutoLockStatusReq,wReqLen,
                                         (uint8 *)&pAMISetHostAutoLockStatusRes,&wResLen, timeout );
    return( wRet );
}

uint16 LIBIPMI_HL_AMISetHostAutoLockStatus(IPMI20_SESSION_T *pSession,INT8U Command,int timeout)
{
    uint16 wRet = CC_NORMAL;
    AMISetHostAutoLockStatusReq_T pAMISetHostAutoLockStatusReq;
    memset(&pAMISetHostAutoLockStatusReq, 0, sizeof(AMISetHostAutoLockStatusReq_T));

    pAMISetHostAutoLockStatusReq.LockStatus = Command;
    wRet = IPMICMD_AMISetHostAutoLockStatus(pSession,&pAMISetHostAutoLockStatusReq,timeout);
    return wRet;
}
uint16 LIBIPMI_HL_AMIGetUserShelltype(IPMI20_SESSION_T *pSession,INT8U UserID,INT8U *ShellType,int timeout)
{
    uint16 wRet = CC_NORMAL;
    uint32 wResLen = sizeof(AMIGetUserShelltypeRes_T);
    AMIGetUserShelltypeRes_T  GetUserShellRes;

    memset(&GetUserShellRes, 0x0, sizeof(AMIGetUserShelltypeRes_T));
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                             ((NETFN_AMI << 2) + 0x00), CMD_AMI_GET_USER_SHELLTYPE,
                                             (uint8 *)&UserID,sizeof(INT8U),
                                             (uint8 *)&GetUserShellRes,&wResLen, timeout );
    if(0 == wRet)
    {
        *ShellType = GetUserShellRes.Shelltype;
    }

    return wRet;
}

uint16 IPMICMD_AMIAddLicenseKey(IPMI20_SESSION_T *pSession,
                        AMIAddLicenseKeyReq_T *pAMIAddLicenseKeyReq,
                        int timeout)
{
    uint16 wRet;
    uint32 wResLen = 0;
    uint32 ReqDataLen = sizeof(AMIAddLicenseKeyReq_T);
    AMIAddLicenseKeyRes_T *pAMIAddLicenseKeyRes;
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                               ((NETFN_AMI << 2) + 0x00), CMD_AMI_ADD_LICENSE_KEY,
                               (uint8 *)pAMIAddLicenseKeyReq,ReqDataLen,
                               (uint8 *)&pAMIAddLicenseKeyRes,&wResLen, timeout );
    return wRet;
}

uint16 IPMICMD_AMIGetLicenseValidity(IPMI20_SESSION_T *pSession,
                AMIGetLicenseValidityRes_T *pAMIGetLicenseValidityRes,
                int timeout)
{
    uint16 wRet;
    uint8 *pReq = NULL;
    uint32 wResLen = 0;
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                               ((NETFN_AMI << 2) + 0x00), CMD_AMI_GET_LICENSE_VALIDITY,
                                pReq, 0,
                               (uint8 *)pAMIGetLicenseValidityRes, &wResLen, timeout );
return wRet;
}

uint16 IPMICMD_AMIGetSSLCertStatus(IPMI20_SESSION_T *pSession,
				AMIGetSSLCertStatusReq_T *pAMIGetSSLCertStatusReq,
				AMIGetSSLCertStatusRes_T *pAMIGetSSLCertStatusRes,
				int timeout)
{
	uint16 wRet;
	uint32 wResLen = sizeof(AMIGetSSLCertStatusRes_T);
	if(pAMIGetSSLCertStatusReq->Param == 0x00)
	{
		wResLen = (uint32)(sizeof(AMIGetSSLCertStatusReq_T)-sizeof(SSLCertInfo_T)+1);
	}
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
								((NETFN_AMI << 2) + 0x00), CMD_AMI_GET_SSL_CERT_STATUS,
								(uint8 *)pAMIGetSSLCertStatusReq,sizeof(AMIGetSSLCertStatusReq_T),
								(uint8 *)pAMIGetSSLCertStatusRes, &wResLen, timeout );
	return wRet;
}

uint16 IPMICMD_AMISetSSLCert(IPMI20_SESSION_T *pSession,
                                AMISetSSLCertReq_T *pAMISetSSLCertReqBuff,
                                AMISetSSLCertRes_T *pAMISetSSLCertResBuff,
                                int timeout)
{
    uint16 wRet;
    uint32 wResLen = sizeof(AMISetSSLCertRes_T);
    if(pAMISetSSLCertReqBuff->Param == CERTIFICATE_VALID)
    {
        wResLen = sizeof(AMISetSSLCertReq_T)-sizeof(SSL_T)+1;
    }

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                                                ((NETFN_AMI << 2) + 0x00), CMD_AMI_SET_SSL_CERT,
                                                                (uint8 *)pAMISetSSLCertReqBuff,sizeof(AMISetSSLCertReq_T),
                                                                (uint8 *)pAMISetSSLCertResBuff, &wResLen, timeout );
    return wRet;
}


uint16 LIBIPMI_HL_AMISetSSLCert(IPMI20_SESSION_T *pSession,SSL_T *sslCfg,int timeout)
{
        AMISetSSLCertReq_T SetSSLCertReqBuff;
        AMISetSSLCertRes_T SetSSLCertResBuff;
        uint16 wRet;
        memset (&SetSSLCertReqBuff, 0, sizeof(AMISetSSLCertReq_T));
        memset (&SetSSLCertResBuff, 0, sizeof(AMISetSSLCertRes_T));
        SetSSLCertReqBuff.Param = CERTIFICATE_GEN;

        memcpy(&SetSSLCertReqBuff.SSLCertdetails, sslCfg,sizeof(SSL_T));

        wRet = IPMICMD_AMISetSSLCert(pSession, &SetSSLCertReqBuff,
                                                                        &SetSSLCertResBuff, timeout);
        if (wRet != CC_SUCCESS)
        {
                TCRIT("Error in Genereating the SSL Certificate\n");
                return wRet;
        }

        return wRet;
}

uint16 LIBIPMI_HL_AMISetSSLCertValidate(IPMI20_SESSION_T *pSession,int *sslValid, int timeout)
{
	AMISetSSLCertReq_T SetSSLCertReqBuff;
	AMISetSSLCertRes_T SetSSLCertResBuff;
	uint16 wRet;
	memset (&SetSSLCertReqBuff, 0, sizeof(AMISetSSLCertReq_T));
	memset (&SetSSLCertResBuff, 0, sizeof(AMISetSSLCertRes_T));
	SetSSLCertReqBuff.Param = CERTIFICATE_VALID;

	wRet = IPMICMD_AMISetSSLCert(pSession, &SetSSLCertReqBuff,
									&SetSSLCertResBuff, timeout);

	*sslValid = SetSSLCertResBuff.CompletionCode;

	return wRet;
}


uint16 LIBIPMI_HL_AMIGetSSLCerDetails(IPMI20_SESSION_T *pSession,SSL_Config_T *ssldtls,int timeout)
{
	AMIGetSSLCertStatusReq_T GetSSLCertStatusReqBuff;
	AMIGetSSLCertStatusRes_T GetSSLCertStatusResBuff;
	uint16 wRet;
	memset (&GetSSLCertStatusReqBuff, 0, sizeof(AMIGetSSLCertStatusReq_T));
	memset (&GetSSLCertStatusResBuff, 0, sizeof(AMIGetSSLCertStatusRes_T));
	GetSSLCertStatusReqBuff.Param = CERTIFICATE_DETAILS;

	wRet = IPMICMD_AMIGetSSLCertStatus(pSession, &GetSSLCertStatusReqBuff,
									&GetSSLCertStatusResBuff, timeout);

	if (wRet != CC_SUCCESS)
	{
		TCRIT("Error in Getting the SSL Certificate Information\n");
		return wRet;
	}

	memcpy(ssldtls, &GetSSLCertStatusResBuff.SSLCertInfo.sslcfg, sizeof(SSL_Config_T));

	return wRet;
}



uint16 LIBIPMI_HL_AMIGetSSLCertStatus(IPMI20_SESSION_T *pSession,INT8U *CertExists,int timeout)
{
	AMIGetSSLCertStatusReq_T GetSSLCertStatusReqBuff;
	AMIGetSSLCertStatusRes_T GetSSLCertStatusResBuff;
	uint16 wRet;
	memset (&GetSSLCertStatusReqBuff, 0, sizeof(AMIGetSSLCertStatusReq_T));
	memset (&GetSSLCertStatusResBuff, 0, sizeof(AMIGetSSLCertStatusRes_T));
	GetSSLCertStatusReqBuff.Param = CERTIFICATE_STATUS;

	wRet = IPMICMD_AMIGetSSLCertStatus(pSession, &GetSSLCertStatusReqBuff,
									&GetSSLCertStatusResBuff, timeout);
	if (wRet != CC_SUCCESS)
	{
		TCRIT("Error in Getting the SSL Certificate Status\n");
		return wRet;
	}
	*CertExists = GetSSLCertStatusResBuff.SSLCertInfo.Status;
	return wRet;
}

uint16 LIBIPMI_HL_AMIGetSSLCertInfo(IPMI20_SESSION_T *pSession,char *CerInfo,int timeout)
{
	AMIGetSSLCertStatusReq_T GetSSLCertStatusReqBuff;
	AMIGetSSLCertStatusRes_T GetSSLCertStatusResBuff;
	uint16 wRet;
	memset (&GetSSLCertStatusReqBuff, 0, sizeof(AMIGetSSLCertStatusReq_T));
	memset (&GetSSLCertStatusResBuff, 0, sizeof(AMIGetSSLCertStatusRes_T));
	GetSSLCertStatusReqBuff.Param = CERIFICATE_INFO;

	wRet = IPMICMD_AMIGetSSLCertStatus(pSession, &GetSSLCertStatusReqBuff,
									&GetSSLCertStatusResBuff, timeout);

	if (wRet != CC_SUCCESS)
	{
		TCRIT("Error in Getting the SSL Certificate Information\n");
		return wRet;
	}
	if( MAX_FILEINFO_SIZE <= snprintf(CerInfo, MAX_FILEINFO_SIZE, "%s", (char *)GetSSLCertStatusResBuff.SSLCertInfo.CertInfo))
			TCRIT("LIBIPMI_HL_AMIGetSSLCertInfo - Source Buffer is truncated");
	return wRet;
}

uint16 LIBIPMI_HL_AMIGetSSLPrivateKeyInfo(IPMI20_SESSION_T *pSession,char *PrivateKeyInfo,int timeout)
{
	AMIGetSSLCertStatusReq_T GetSSLCertStatusReqBuff;
	AMIGetSSLCertStatusRes_T GetSSLCertStatusResBuff;
	uint16 wRet;
	memset (&GetSSLCertStatusReqBuff, 0, sizeof(AMIGetSSLCertStatusReq_T));
	memset (&GetSSLCertStatusResBuff, 0, sizeof(AMIGetSSLCertStatusRes_T));
	GetSSLCertStatusReqBuff.Param = PRIVATE_KEY_INFO;

	wRet = IPMICMD_AMIGetSSLCertStatus(pSession, &GetSSLCertStatusReqBuff,
									&GetSSLCertStatusResBuff, timeout);

	if (wRet != CC_SUCCESS)
	{
		TCRIT("Error in Getting the SSL Private key Information\n");
		return wRet;
	}
	if( MAX_FILE_INFO_SIZE <= snprintf(PrivateKeyInfo, MAX_FILE_INFO_SIZE, "%s", (char *)GetSSLCertStatusResBuff.SSLCertInfo.PrivateKeyInfo))
		TCRIT("LIBIPMI_HL_AMIGetSSLPrivateKeyInfo - Source buffer is truncated");
	return wRet;
}

uint16 IPMICMD_AMIGetRemoteKVMCfg(IPMI20_SESSION_T *pSession,
                AMIGetRemoteKVMCfgReq_T *pAMIGetRemoteKVMCfgReq,
                AMIGetRemoteKVMCfgRes_T *pAMIGetRemoteKVMCfgRes,
                int timeout)
{
	uint16 wRet;
	// since the size of structure AMISetRemoteKVMCfgReq_T is increase to 7
        // subtracting sizeof RemoteKVMCfg_T from the size so that the request length remains 2
	uint32 wResLen = sizeof(AMIGetRemoteKVMCfgRes_T) - sizeof(RemoteKVMCfg_T) + 1;
	if(pAMIGetRemoteKVMCfgReq->ParameterSelect == KVM_KBD_LAYOUT)
	{
		wResLen = KEYBRD_LANG_SIZE;
	}
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
	                               ((NETFN_AMI << 2) + 0x00), CMD_AMI_GET_REMOTEKVM_CONF,
	                               (uint8 *)pAMIGetRemoteKVMCfgReq,sizeof(AMIGetRemoteKVMCfgReq_T),
	                               (uint8 *)pAMIGetRemoteKVMCfgRes, &wResLen, timeout );
	return wRet;
}

uint16 LIBIPMI_HL_AMIGetRemoteKVM_Mouse_Mode(IPMI20_SESSION_T *pSession,INT8U *Mouse_Mode,int timeout)
{
	AMIGetRemoteKVMCfgReq_T GetRemoteKVMCfgReq;
	AMIGetRemoteKVMCfgRes_T GetRemoteKVMCfgRes;
	uint16 wRet;

	GetRemoteKVMCfgReq.ParameterSelect = KVM_MOUSE_MODE;
	wRet = IPMICMD_AMIGetRemoteKVMCfg(pSession,&GetRemoteKVMCfgReq,&GetRemoteKVMCfgRes,timeout);

	if(wRet == CC_NORMAL)
	{
		*Mouse_Mode = GetRemoteKVMCfgRes.RemoteKVMCfg.Mouse_Mode;
	}

	return wRet;
}

uint16 LIBIPMI_HL_AMIGetRemoteKVM_Retry_Count(IPMI20_SESSION_T *pSession,INT8U *retry_count,int timeout)
{
	AMIGetRemoteKVMCfgReq_T GetRemoteKVMCfgReq;
	AMIGetRemoteKVMCfgRes_T GetRemoteKVMCfgRes;
	uint16 wRet;

	GetRemoteKVMCfgReq.ParameterSelect = KVM_RETRY_COUNT;
	wRet = IPMICMD_AMIGetRemoteKVMCfg(pSession,&GetRemoteKVMCfgReq,&GetRemoteKVMCfgRes,timeout);

	if(wRet == CC_NORMAL)
	{
		*retry_count = GetRemoteKVMCfgRes.RemoteKVMCfg.retry_count;
	}

	return wRet;
}

uint16 LIBIPMI_HL_AMISetRemoteKVM_Retry_Count(IPMI20_SESSION_T *pSession,INT8U retryCount, int timeout)
{
	AMISetRemoteKVMCfgReq_T SetRemoteKVMCfgReq = {0,{0}};
	AMISetRemoteKVMCfgRes_T SetRemoteKVMCfgRes = {0};
	uint16 wRet;

	SetRemoteKVMCfgReq.ParameterSelect = KVM_RETRY_COUNT;
	SetRemoteKVMCfgReq.RemoteKVMcfg.retry_count = retryCount;
	wRet = IPMICMD_AMISetRemoteKVMCfg(pSession,&SetRemoteKVMCfgReq,&SetRemoteKVMCfgRes,timeout);

	if(wRet != CC_NORMAL)
	{
		TCRIT("Error in Setting the KVM Retry count::%x\n",wRet);
		return wRet;
	}

	return wRet;
}

uint16 LIBIPMI_HL_AMIGetRemoteKVM_Retry_Interval(IPMI20_SESSION_T *pSession,INT8U *retry_interval,int timeout)
{
	AMIGetRemoteKVMCfgReq_T GetRemoteKVMCfgReq;
	AMIGetRemoteKVMCfgRes_T GetRemoteKVMCfgRes;
	uint16 wRet;

	GetRemoteKVMCfgReq.ParameterSelect = KVM_RETRY_INTERVAL;
	wRet = IPMICMD_AMIGetRemoteKVMCfg(pSession,&GetRemoteKVMCfgReq,&GetRemoteKVMCfgRes,timeout);

	if(wRet == CC_NORMAL)
	{
		*retry_interval = GetRemoteKVMCfgRes.RemoteKVMCfg.retry_interval;
	}

	return wRet;
}

uint16 LIBIPMI_HL_AMISetRemoteKVM_Retry_Interval(IPMI20_SESSION_T *pSession,INT8U retryTimeInterval, int timeout)
{
	AMISetRemoteKVMCfgReq_T SetRemoteKVMCfgReq = {0,{0}};
	AMISetRemoteKVMCfgRes_T SetRemoteKVMCfgRes = {0};
	uint16 wRet;

	SetRemoteKVMCfgReq.ParameterSelect = KVM_RETRY_INTERVAL;
	SetRemoteKVMCfgReq.RemoteKVMcfg.retry_interval = retryTimeInterval;
	wRet = IPMICMD_AMISetRemoteKVMCfg(pSession,&SetRemoteKVMCfgReq,&SetRemoteKVMCfgRes,timeout);

	if(wRet != CC_NORMAL)
	{
		TCRIT("Error in Setting the KVM Retry Time Interval::%x\n",wRet);
		return wRet;
	}

	return wRet;
}

uint16 LIBIPMI_HL_AMIGetRemoteKVM_Keyboard_Layout(IPMI20_SESSION_T *pSession,char *kbd_layout,int timeout)
{
	AMIGetRemoteKVMCfgReq_T GetRemoteKVMCfgReq = {0};
	AMIGetRemoteKVMCfgRes_T GetRemoteKVMCfgRes = {0,{0}};
	uint16 wRet;

	GetRemoteKVMCfgReq.ParameterSelect = KVM_KBD_LAYOUT;
	wRet = IPMICMD_AMIGetRemoteKVMCfg(pSession,&GetRemoteKVMCfgReq,&GetRemoteKVMCfgRes,timeout);

	if(wRet == CC_NORMAL)
	{
            snprintf(kbd_layout,KEYBRD_LANG_SIZE,"%s",(char *)GetRemoteKVMCfgRes.RemoteKVMCfg.keyboard_layout);
	}

	return wRet;
}

uint16 LIBIPMI_HL_AMIGetRemoteKVM_Encryption(IPMI20_SESSION_T *pSession,INT8U *Secure_status,int timeout)
{
	AMIGetRemoteKVMCfgReq_T GetRemoteKVMCfgReq = {0};
	AMIGetRemoteKVMCfgRes_T GetRemoteKVMCfgRes = {0,{0}};
	uint16 wRet;

	GetRemoteKVMCfgReq.ParameterSelect = KVM_ENCRYPTION;
	wRet = IPMICMD_AMIGetRemoteKVMCfg(pSession,&GetRemoteKVMCfgReq,&GetRemoteKVMCfgRes,timeout);

	if(wRet == CC_NORMAL)
	{
		*Secure_status = GetRemoteKVMCfgRes.RemoteKVMCfg.secure_status;
	}

	return wRet;
}

uint16 IPMICMD_AMISetRemoteKVMCfg(IPMI20_SESSION_T *pSession,
                AMISetRemoteKVMCfgReq_T *pAMISetRemoteKVMCfgReq,
                AMISetRemoteKVMCfgRes_T *pAMISetRemoteKVMCfgRes,
                int timeout)
{
	uint16 wRet;
	// since the size of structure AMISetRemoteKVMCfgReq_T is increase to 7
        // subtracting sizeof RemoteKVMCfg_T from the size so that the request length remains 2
	uint32 wReqLen = sizeof(AMISetRemoteKVMCfgReq_T) - sizeof(RemoteKVMCfg_T) + 1;
	uint32 wResLen = sizeof(AMISetRemoteKVMCfgRes_T) ;
	if(pAMISetRemoteKVMCfgReq->ParameterSelect == KVM_KBD_LAYOUT)
	{
		wReqLen = KEYBRD_LANG_SIZE;
	}
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
	                               ((NETFN_AMI << 2) + 0x00), CMD_AMI_SET_REMOTEKVM_CONF,
	                               (uint8 *)pAMISetRemoteKVMCfgReq,wReqLen ,
	                               (uint8 *)pAMISetRemoteKVMCfgRes, &wResLen, timeout );
	return wRet;
}

uint16 LIBIPMI_HL_AMISetRemoteKVM_Mouse_Mode(IPMI20_SESSION_T *pSession,INT8U Mouse_Mode,int timeout)
{
	AMISetRemoteKVMCfgReq_T SetRemoteKVMCfgReq = {0,{0}};
	AMISetRemoteKVMCfgRes_T SetRemoteKVMCfgRes = {0};
	uint16 wRet;

	SetRemoteKVMCfgReq.ParameterSelect = KVM_MOUSE_MODE;
	SetRemoteKVMCfgReq.RemoteKVMcfg.Mouse_Mode = Mouse_Mode;
	wRet = IPMICMD_AMISetRemoteKVMCfg(pSession,&SetRemoteKVMCfgReq,&SetRemoteKVMCfgRes,timeout);

	if(wRet != CC_NORMAL)
	{
		TCRIT("Error in Setting the Mouse Mode::%x\n",wRet);
		return wRet;
	}

	return wRet;
}

uint16 LIBIPMI_HL_AMISetRemoteKVM_Keyboard_Layout(IPMI20_SESSION_T *pSession,char *kbd_layout,int timeout)
{
	AMISetRemoteKVMCfgReq_T SetRemoteKVMCfgReq = {0,{0}};
	AMISetRemoteKVMCfgRes_T SetRemoteKVMCfgRes = {0};
	uint16 wRet;
	SetRemoteKVMCfgReq.ParameterSelect = KVM_KBD_LAYOUT;
	strncpy((char *)SetRemoteKVMCfgReq.RemoteKVMcfg.keyboard_layout,kbd_layout,KEYBRD_LANG_SIZE-1);
	wRet = IPMICMD_AMISetRemoteKVMCfg(pSession,&SetRemoteKVMCfgReq,&SetRemoteKVMCfgRes,timeout);

	if(wRet != CC_NORMAL)
	{
		TCRIT("Error in Setting the Key board layout::%x\n",wRet);
		return wRet;
	}

	return wRet;
}

uint16 LIBIPMI_HL_AMISetRemoteKVM_Encryption(IPMI20_SESSION_T *pSession,INT8U secure_status,int timeout)
{
	AMISetRemoteKVMCfgReq_T SetRemoteKVMCfgReq = {0,{0}};
	AMISetRemoteKVMCfgRes_T SetRemoteKVMCfgRes = {0};
	uint16 wRet;

	SetRemoteKVMCfgReq.ParameterSelect = KVM_ENCRYPTION;
	SetRemoteKVMCfgReq.RemoteKVMcfg.secure_status = secure_status;
	wRet = IPMICMD_AMISetRemoteKVMCfg(pSession,&SetRemoteKVMCfgReq,&SetRemoteKVMCfgRes,timeout);

	if(wRet != CC_NORMAL)
	{
		TCRIT("Error in Setting the Mouse Mode::%x\n",wRet);
		return wRet;
	}

	return wRet;
}

uint16 IPMICMD_AMIGetLogConf(IPMI20_SESSION_T *pSession, AMIGetLogConfRes_T *pAMIGetLogConfRes, int timeout)
{
	uint16 wRet;
	uint32 wReqLen = 0x00;
	uint32 wResLen = sizeof(AMIGetLogConfRes_T);

	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
				((NETFN_AMI << 2) + 0x00), CMD_AMI_GET_LOG_CONF,
				(uint8 *)NULL,wReqLen ,
				(uint8 *)pAMIGetLogConfRes, &wResLen, timeout );
	return wRet;
}

uint16 LIBIPMI_HL_AMIGetRemoteLogConf(IPMI20_SESSION_T *pSession,INT8U *Sysstatus, INT8U *Auditstatus, char *hostname, INT32U *portno, int timeout)
{
	AMIGetLogConfRes_T GetLogConfRes = {0,0,0,{{"0",0}}};
	uint16 wRet;
	wRet = IPMICMD_AMIGetLogConf(pSession,&GetLogConfRes,timeout);

	if (wRet != 0)
	{
		TCRIT("Getting SysLog configuration error::%d\n", wRet);
		return wRet;
	}

	*Sysstatus = GetLogConfRes.SysStatus;
	*Auditstatus = GetLogConfRes.AuditStatus;

	TWARN("IN LIBIPMI_HL_AMIGetRemoteLogConf -- Sys status is - %d --- Portno is - %d", GetLogConfRes.SysStatus, GetLogConfRes.Config.Remote.HostPort);

	if (GetLogConfRes.SysStatus == REMOTE_SYSLOG){
		strncpy(hostname,GetLogConfRes.Config.Remote.HostName, HOSTNAME_LENGTH-1);
		*portno=GetLogConfRes.Config.Remote.HostPort;
	}

	return wRet;
}

uint16 LIBIPMI_HL_AMIGetLocalLogConf(IPMI20_SESSION_T *pSession,INT32U *File_size, INT8U *Rotate,int timeout)
{
	AMIGetLogConfRes_T GetLogConfRes = {0,0,0,{{"0",0}}};
	uint16 wRet;
	wRet = IPMICMD_AMIGetLogConf(pSession,&GetLogConfRes,timeout);

	if (wRet != 0)
	{
		TCRIT("Getting SysLog configuration error::%d\n", wRet);
		return wRet;
	}
	if (GetLogConfRes.SysStatus == LOCAL_SYSLOG)
	{
		*File_size = GetLogConfRes.Config.Local.Size;
		*Rotate = GetLogConfRes.Config.Local.Rotate;
	}
	return wRet;
}
uint16 IPMICMD_AMISetLogConf(IPMI20_SESSION_T *pSession, AMISetLogConfReq_T *pAMISetLogConfReq, int timeout)
{
	AMISetLogConfRes_T  SetLogConfResBuff = {0};
	uint16 wRet;
	uint32 wReqLen = sizeof(AMISetLogConfReq_T)- sizeof(SyslogConfig_T);
	uint32 wResLen = 0;
	if(pAMISetLogConfReq->Cmd == SYSLOG)
	{
		switch(pAMISetLogConfReq->Status)
		{
		case SYSLOG_DISABLE:
			wReqLen = sizeof(AMISetLogConfReq_T)- sizeof(SyslogConfig_T);
			break;
		case LOCAL_SYSLOG:
			wReqLen += sizeof(pAMISetLogConfReq->Config.Local);
			break;
		case REMOTE_SYSLOG:
			wReqLen += sizeof(pAMISetLogConfReq->Config.Remote);
			//pAMISetLogConfReq->Config.Remote.HostPort = SYSLOG_DEFAULT_PORT;
			break;
		}
	}

	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
				((NETFN_AMI << 2) + 0x00), CMD_AMI_SET_LOG_CONF,
				(uint8 *)pAMISetLogConfReq,wReqLen ,
				(uint8 *)&SetLogConfResBuff, &wResLen, timeout );

	return wRet;
}

uint16 LIBIPMI_HL_AMISetSysLogConf(IPMI20_SESSION_T *pSession, INT8U Sysenable, INT8U Auditenable, char *hostname,INT32U portno,int timeout)
{
	AMISetLogConfReq_T SetLogConfReqBuff = {0,0,{{"0",0}}};
	uint16 wRet;

	SetLogConfReqBuff.Cmd = SYSLOG;
	SetLogConfReqBuff.Status = Sysenable;

	if(Sysenable == REMOTE_SYSLOG){
		strncpy(SetLogConfReqBuff.Config.Remote.HostName,hostname,(sizeof(SetLogConfReqBuff.Config.Remote.HostName)-1));
		SetLogConfReqBuff.Config.Remote.HostPort=portno;
	}

	wRet = IPMICMD_AMISetLogConf(pSession, &SetLogConfReqBuff, timeout);
	if(wRet != CC_SUCCESS)
	{
		TCRIT("Error in Setting the Syslog Configurations ::%x\n",wRet);
		return wRet;
	}

	SetLogConfReqBuff.Cmd = AUDITLOG;
	SetLogConfReqBuff.Status = Auditenable;
	wRet = IPMICMD_AMISetLogConf(pSession, &SetLogConfReqBuff, timeout);
	if(wRet != CC_SUCCESS)
	{
		TCRIT("Error in Setting the Audit log enable ::%x\n",wRet);
		return wRet;
	}
	return wRet;
}
uint16 LIBIPMI_HL_AMISetLocalLogConf(IPMI20_SESSION_T *pSession, INT32U filesize, INT8U Rotate,int timeout)
{
	AMISetLogConfReq_T SetLogConfReqBuff = {0,0,{{"0",0}}};
	uint16 wRet;
	SetLogConfReqBuff.Cmd = SYSLOG;
	SetLogConfReqBuff.Status = LOCAL_SYSLOG;
	SetLogConfReqBuff.Config.Local.Size = filesize;
	SetLogConfReqBuff.Config.Local.Rotate = Rotate;
	wRet = IPMICMD_AMISetLogConf(pSession, &SetLogConfReqBuff, timeout);
	if(wRet != CC_SUCCESS)
	{
		TCRIT("Error in Setting the SysLocal log configurations ::%x\n",wRet);
		return wRet;
	}
	return wRet;
}
uint16 IPMICMD_AMIActivedcloseSession(IPMI20_SESSION_T *pSession,
		AMIActivedcloseSessionReq_T *pActivedcloseSessionReq,
		AMIActivedcloseSessionRes_T *pActivedcloseSessionRes,
                int timeout)
{
	uint16 wRet;
	uint32 wReqLen = sizeof(AMIActivedcloseSessionReq_T);
	uint32 wResLen = sizeof(AMIActivedcloseSessionRes_T) ;
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
		                               ((NETFN_AMI << 2) + 0x00), CMD_AMI_ACTIVE_SESSIONS_CLOSE,
		                               (uint8 *)pActivedcloseSessionReq,wReqLen ,
		                               (uint8 *)pActivedcloseSessionRes, &wResLen, timeout );
		return wRet;

}
uint16 LIBIPMI_HL_AMIActivedcloseSession(IPMI20_SESSION_T *pSession,INT32U racsession_id,int timeout)
{
	uint16 wRet;
	AMIActivedcloseSessionReq_T ActivedcloseSessionReq_T={0};
	AMIActivedcloseSessionRes_T ActivedcloseSessionRes_T={0};
	ActivedcloseSessionReq_T.racsessionid = racsession_id;

	wRet = IPMICMD_AMIActivedcloseSession(pSession,&ActivedcloseSessionReq_T,&ActivedcloseSessionRes_T,timeout);

	if(wRet != CC_NORMAL)
	{
		TCRIT("Error in Setting the Mouse Mode::%x\n",wRet);
		return wRet;
	}

	return wRet;
}

uint16 IPMICMD_AMIGetVmediaCfg(IPMI20_SESSION_T *pSession,
						AMIGetVmediaCfgReq_T *pAMIGetVmediaCfgReq,
						AMIGetVmediaCfgRes_T *pAMIGetVmediaCfgRes,
						int timeout)
{
	uint16 wRet;
	uint32 wReqLen = sizeof(AMIGetVmediaCfgReq_T);
	uint32 wResLen = 0;

	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
						((NETFN_AMI << 2) + 0x00), CMD_AMI_GET_VMEDIA_CONF,
						(uint8 *)pAMIGetVmediaCfgReq,wReqLen ,
						(uint8 *)pAMIGetVmediaCfgRes, &wResLen, timeout );
	return wRet;
}

uint16 LIBIPMI_HL_AMIGetVmediaCfg(IPMI20_SESSION_T *pSession,VMediaCfgParam_T *pvmedia_cfg, int timeout)
{
	AMIGetVmediaCfgReq_T GetVmediaCfgReq;
	AMIGetVmediaCfgRes_T GetVmediaCfgRes;
	uint16 wRet = -1;
	int param = 0;
	for(param = VMEDIA_CD_ATTACH_MODE; param <= KVM_NUM_HD;param++)
	{
		memset(&GetVmediaCfgReq,0,sizeof(AMIGetVmediaCfgReq_T));
		memset(&GetVmediaCfgRes,0,sizeof(AMIGetVmediaCfgRes_T));

		GetVmediaCfgReq.Param = param;
		wRet = IPMICMD_AMIGetVmediaCfg(pSession, &GetVmediaCfgReq, &GetVmediaCfgRes, timeout);

		switch(param)
		{
		case VMEDIA_CD_ATTACH_MODE:
			if (wRet == CC_SUCCESS)
				pvmedia_cfg->attach_cd = GetVmediaCfgRes.VMediaConfig.attach_cd;
			break;
		case VMEDIA_FD_ATTACH_MODE:
			if (wRet == CC_SUCCESS)
				pvmedia_cfg->attach_fd = GetVmediaCfgRes.VMediaConfig.attach_fd;
			break;
		case VMEDIA_HD_ATTACH_MODE:
			if (wRet == CC_SUCCESS)
				pvmedia_cfg->attach_hd = GetVmediaCfgRes.VMediaConfig.attach_hd;
			break;
		case VMEDIA_ENABLE_BOOT_ONCE:
			if (wRet == CC_SUCCESS)
			pvmedia_cfg->enable_boot_once = GetVmediaCfgRes.VMediaConfig.enable_boot_once;
			break;
		case VMEDIA_NUM_CD:
			if (wRet == CC_SUCCESS)
				pvmedia_cfg->num_cd = GetVmediaCfgRes.VMediaConfig.num_cd;
			break;
		case VMEDIA_NUM_FD:
			if (wRet == CC_SUCCESS)
				pvmedia_cfg->num_fd = GetVmediaCfgRes.VMediaConfig.num_fd;
			break;
		case VMEDIA_NUM_HD:
			if (wRet == CC_SUCCESS)
				pvmedia_cfg->num_hd = GetVmediaCfgRes.VMediaConfig.num_hd;
			break;
		case VMEDIA_LMEDIA_ENABLE_STATUS:
			if (wRet == CC_SUCCESS)
				pvmedia_cfg->lmedia_enable = GetVmediaCfgRes.VMediaConfig.lmedia_enable;
			break;
		case VMEDIA_RMEDIA_ENABLE_STATUS:
			if (wRet == CC_SUCCESS)
				pvmedia_cfg->rmedia_enable = GetVmediaCfgRes.VMediaConfig.rmedia_enable;
			break;
		case VMEDIA_SDMEDIA_ENABLE_STATUS:
			if (wRet == CC_SUCCESS)
			pvmedia_cfg->sdmedia_enable = GetVmediaCfgRes.VMediaConfig.sdmedia_enable;
			break;
		case KVM_NUM_FD:
			if (wRet == CC_SUCCESS)
				pvmedia_cfg->kvm_fd = GetVmediaCfgRes.VMediaConfig.kvm_fd;
			break;
		case KVM_NUM_CD:
			if (wRet == CC_SUCCESS)
				pvmedia_cfg->kvm_cd = GetVmediaCfgRes.VMediaConfig.kvm_cd;
			break;
		case KVM_NUM_HD:
			if (wRet == CC_SUCCESS)
				pvmedia_cfg->kvm_hd = GetVmediaCfgRes.VMediaConfig.kvm_hd;
			break;
		}
	}
	if ((wRet & 0xFF) == CC_FEATURE_NOT_ENABLED &&
		((VMEDIA_ENABLE_BOOT_ONCE || VMEDIA_SDMEDIA_ENABLE_STATUS)||
		(KVM_NUM_CD) || (KVM_NUM_FD) || (KVM_NUM_HD))){
		wRet = CC_SUCCESS;
	}
	return wRet;
}

uint16 LIBIPMI_HL_AMIGetVmedia_Encryption(IPMI20_SESSION_T *pSession,INT8U *Media_Encryption, int timeout)
{
	AMIGetVmediaCfgReq_T GetVmediaCfgReq = {0};
	AMIGetVmediaCfgRes_T GetVmediaCfgRes = {0,{0}};
	uint16 wRet;

	GetVmediaCfgReq.Param = VMEDIA_ENCRYPTION;
	wRet = IPMICMD_AMIGetVmediaCfg(pSession, &GetVmediaCfgReq, &GetVmediaCfgRes, timeout);
	if(wRet != CC_SUCCESS)
	{
		TCRIT("Error in Getting the Media Secure Status ::%x\n",wRet);
		return wRet;
	}
	*Media_Encryption = GetVmediaCfgRes.VMediaConfig.secure_status;
	return wRet;
}

uint16 IPMICMD_AMISetVmediaCfg(IPMI20_SESSION_T *pSession, AMISetVmediaCfgReq_T *pAMISetVmediaCfgReq, int timeout)
{
	uint16 wRet;
	AMISetVmediaCfgRes_T SetVmediaCfgRes = {0};
	uint32 wReqLen = sizeof(AMISetVmediaCfgReq_T);
	uint32 wResLen = 0;

	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
					((NETFN_AMI << 2) + 0x00), CMD_AMI_SET_VMEDIA_CONF,
					(uint8 *)pAMISetVmediaCfgReq,wReqLen ,
					(uint8 *)&SetVmediaCfgRes, &wResLen, timeout );
	return wRet;
}

uint16 LIBIPMI_HL_AMISetVmedia_Encryption(IPMI20_SESSION_T *pSession,INT8U Media_Encryption, int timeout)
{
	AMISetVmediaCfgReq_T SetVmediaCfgReq = {0,{0}};
	uint16 wRet;

	SetVmediaCfgReq.Param = VMEDIA_ENCRYPTION;
	SetVmediaCfgReq.VMediaConfig.secure_status = Media_Encryption;
	wRet = IPMICMD_AMISetVmediaCfg(pSession, &SetVmediaCfgReq, timeout);
	if(wRet != CC_SUCCESS)
	{
		TCRIT("Error in Setting the Media Secure Status ::%x\n",wRet);
		return wRet;
	}
	return wRet;
}

uint16 LIBIPMI_HL_AMISetVmediaCfg(IPMI20_SESSION_T *pSession,VMediaCfgParam_T *pvmedia_cfg, int timeout,INT8U Page)
{
	AMISetVmediaCfgReq_T SetVmediaCfgReq;
	uint16 wRet = -1;
	int *param ;

	switch(Page)
	{
	case VMEDIA_REMOTE_ATTACH_MODE:
		param = VmediaAttachMode;
		break;
	case VMEDIA_DEVICE_INSTANCE:
		param = VmediaNumInstance;
		break;
	case VMEDIA_LMEDIA_ENABLE_STATUS:
		param=VmediaLMediaStatus;
		break;
	case VMEDIA_RMEDIA_ENABLE_STATUS:
		param=VmediaRMediaStatus;
		break;
	default:
		TCRIT("\nInvalid page option :%x to set vmedia configuration \n",Page);
		return -1;
	}

	while((*param) != -1)
	{
		memset(&SetVmediaCfgReq,0,sizeof(AMISetVmediaCfgReq_T));

		SetVmediaCfgReq.Param = *param;
		param++;

		switch(SetVmediaCfgReq.Param)
		{
		case VMEDIA_CD_ATTACH_MODE:
			SetVmediaCfgReq.VMediaConfig.attach_cd = pvmedia_cfg->attach_cd;
			break;
		case VMEDIA_FD_ATTACH_MODE:
			SetVmediaCfgReq.VMediaConfig.attach_fd = pvmedia_cfg->attach_fd;
			break;
		case VMEDIA_HD_ATTACH_MODE:
			SetVmediaCfgReq.VMediaConfig.attach_hd = pvmedia_cfg->attach_hd;
			break;
		case VMEDIA_ENABLE_BOOT_ONCE:
			SetVmediaCfgReq.VMediaConfig.enable_boot_once = pvmedia_cfg->enable_boot_once;
			break;
		case VMEDIA_NUM_CD:
			SetVmediaCfgReq.VMediaConfig.num_cd = pvmedia_cfg->num_cd;
			break;
		case VMEDIA_NUM_FD:
			SetVmediaCfgReq.VMediaConfig.num_fd = pvmedia_cfg->num_fd;
			break;
		case VMEDIA_NUM_HD:
			SetVmediaCfgReq.VMediaConfig.num_hd = pvmedia_cfg->num_hd;
			break;
		case VMEDIA_LMEDIA_ENABLE_STATUS:
			SetVmediaCfgReq.VMediaConfig.lmedia_enable = pvmedia_cfg->lmedia_enable;
			break;
		case VMEDIA_RMEDIA_ENABLE_STATUS:
			SetVmediaCfgReq.VMediaConfig.rmedia_enable = pvmedia_cfg->rmedia_enable;
			break;
		case VMEDIA_SDMEDIA_ENABLE_STATUS:
			SetVmediaCfgReq.VMediaConfig.sdmedia_enable = pvmedia_cfg->sdmedia_enable;
			break;
		case VMEDIA_RESTART:
			SetVmediaCfgReq.VMediaConfig.Vmedia_restart = 0x01;
			break;
		case KVM_NUM_CD:
			SetVmediaCfgReq.VMediaConfig.kvm_cd = pvmedia_cfg->kvm_cd;
			break;
		case KVM_NUM_FD:
			SetVmediaCfgReq.VMediaConfig.kvm_fd = pvmedia_cfg->kvm_fd;
			break;
		case KVM_NUM_HD:
			SetVmediaCfgReq.VMediaConfig.kvm_hd = pvmedia_cfg->kvm_hd;
			break;
		default:
			continue;
		}
		wRet = IPMICMD_AMISetVmediaCfg(pSession, &SetVmediaCfgReq, timeout);
		if ((wRet & 0xFF) == CC_FEATURE_NOT_ENABLED &&
			((VMEDIA_ENABLE_BOOT_ONCE || VMEDIA_SDMEDIA_ENABLE_STATUS)||
			(KVM_NUM_CD) || (KVM_NUM_FD) || (KVM_NUM_HD))){
			wRet = CC_SUCCESS;
			continue;
		} else if (wRet != 0) {
			return wRet;
		}
	}
	return wRet;
}
uint16 IPMICMD_AMIGetLDAPConf(IPMI20_SESSION_T *pSession,
				AMIGetLDAPReq_T *pAMIGetDLAPReq,
				AMIGetLDAPRes_T *pAMIGetDLAPRes,
				INT8U ReqLen, int timeout)
{
	uint16 wRet;
	uint32 wResLen = 0;
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
				((NETFN_AMI << 2) + 0x00), CMD_AMI_GET_LDAP_CONF,
				(uint8 *)pAMIGetDLAPReq,ReqLen ,
				(uint8 *)pAMIGetDLAPRes, &wResLen, timeout );
	return wRet;
}

uint16 IPMICMD_AMISetLDAPConf(IPMI20_SESSION_T *pSession,
				AMISetLDAPReq_T *pAMIGetDLAPReq,
				AMISetLDAPRes_T *pAMIGetDLAPRes,
				INT8U ReqLen, int timeout)
{
	uint16 wRet;
	uint32 wResLen = 0;
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
				((NETFN_AMI << 2) + 0x00), CMD_AMI_SET_LDAP_CONF,
				(uint8 *)pAMIGetDLAPReq,ReqLen ,
				(uint8 *)pAMIGetDLAPRes, &wResLen, timeout );
	return wRet;
}
uint16 LIBIPMI_HL_AMIGetLDAPConf(IPMI20_SESSION_T *pSession,IPMILDAPCONFIG * GetldapConfig,int timeout)
{
	AMIGetLDAPReq_T GetLDAPConfReq;
	AMIGetLDAPRes_T GetLDAPConfRes;
	uint16 wRet = -1;
	int param = 0;
	int tempret=0;/*To check return value of snprintf, it is not used any where else*/

	for(param = AMI_LDAP_STATUS; param < AMI_LDAP_GROUPROLE_CONFIG;param++)
	{
		memset(&GetLDAPConfReq,0,sizeof(AMIGetLDAPReq_T));
		memset(&GetLDAPConfRes,0,sizeof(AMIGetLDAPRes_T));
		GetLDAPConfReq.ParameterSelector = param;
		wRet = IPMICMD_AMIGetLDAPConf(pSession, &GetLDAPConfReq, &GetLDAPConfRes,sizeof(INT8U), timeout);
		if (wRet == CC_SUCCESS)
		{
			switch(param)
			{
			case AMI_LDAP_STATUS:
				GetldapConfig->Enable= GetLDAPConfRes.Config.Enable;
				break;
			case AMI_LDAP_ENCRYPTEDENABLE:
				GetldapConfig->EncryptedEnable = GetLDAPConfRes.Config.EncryptedEnable;
				break;
			case AMI_LDAP_PORT_NUM:
				GetldapConfig->PortNum= GetLDAPConfRes.Config.PortNum;
				break;
			case AMI_LDAP_PORT_NUM_SEC:
				GetldapConfig->PortNumSecondary= GetLDAPConfRes.Config.PortNumSecondary;
				break;
			case AMI_LDAP_IP_ADDR:
				tempret=snprintf(GetldapConfig->IPAddr,sizeof(GetldapConfig->IPAddr),"%s", GetLDAPConfRes.Config.IPAddr);
				if((tempret<0)||(tempret>=(signed)sizeof(GetldapConfig->IPAddr)))
                                {
                                    TCRIT("Buffer Overflow");
                                    return -1;
                                }
				break;
			case AMI_LDAP_IP_ADDR_SEC:
				tempret=snprintf(GetldapConfig->IPAddrSecondary, sizeof(GetldapConfig->IPAddrSecondary),"%s",GetLDAPConfRes.Config.IPAddrSecondary);
				if((tempret<0)||(tempret>=(signed)sizeof(GetldapConfig->IPAddrSecondary)))
                                {
                                    TCRIT("Buffer Overflow");
                                    return -1;
                                }
				break;
			case AMI_LDAP_PASSWORD:
				tempret=snprintf(GetldapConfig->Password,sizeof(GetldapConfig->Password),"%s", GetLDAPConfRes.Config.Password);
				if((tempret<0)||(tempret>=(signed)sizeof(GetldapConfig->Password)))
                                {
                                    TCRIT("Buffer Overflow");
                                    return -1;
                                }
				break;
			case AMI_LDAP_BIND_DN:
				tempret=snprintf(GetldapConfig->BindDN,sizeof(GetldapConfig->BindDN),"%s", GetLDAPConfRes.Config.BindDN);
				if((tempret<0)||(tempret>=(signed)sizeof(GetldapConfig->BindDN)))
                                {
                                    TCRIT("Buffer Overflow");
                                    return -1;
                                }
				break;
			case AMI_LDAP_SEARCH_BASE:
				tempret=snprintf(GetldapConfig->SearchBase,sizeof(GetldapConfig->SearchBase),"%s", GetLDAPConfRes.Config.SearchBase);
				if((tempret<0)||(tempret>=(signed)sizeof(GetldapConfig->SearchBase)))
                                {
                                    TCRIT("Buffer Overflow");
                                    return -1;
                                }
				break;
			case AMI_LDAP_ATTRIBUTE_USER_LOGIN:
				tempret=snprintf(GetldapConfig->AttributeOfUserLogin,sizeof(GetldapConfig->AttributeOfUserLogin),"%s", GetLDAPConfRes.Config.AttributeOfUserLogin);
				if((tempret<0)||(tempret>=(signed)sizeof(GetldapConfig->AttributeOfUserLogin)))
                                {
                                    TCRIT("Buffer Overflow");
                                    return -1;
                                }
				break;
			case AMI_LDAP_DEFAULT_ROLE:
				GetldapConfig->DefaultRole= GetLDAPConfRes.Config.DefaultRole;
				break;
			}
		}
	}

	if(wRet == CC_SUCCESS)
	{
		memset(&GetLDAPConfReq,0,sizeof(AMIGetLDAPReq_T));
		memset(&GetLDAPConfRes,0,sizeof(AMIGetLDAPRes_T));
		GetLDAPConfReq.ParameterSelector = AMI_LDAP_FQDN_ENABLE;
		wRet = IPMICMD_AMIGetLDAPConf(pSession, &GetLDAPConfReq, &GetLDAPConfRes,sizeof(INT8U), timeout);
		GetldapConfig->FQDNEnable = GetLDAPConfRes.Config.FQDNEnable;
	}
	return wRet;
}
uint16 LIBIPMI_HL_AMIGetLDAPPrivConf(IPMI20_SESSION_T *pSession,IPMI_LDAP_Config_T  GetldapPrivConfig[] ,int timeout)
{
	AMIGetLDAPReq_T GetLDAPConfReq;
	AMIGetLDAPRes_T GetLDAPConfRes;
	uint16 wRet = -1;
	int GroupRole ;
	INT8U UserID;
	int tempret=0;/*To check return value of snprintf, it is not used any where else*/
	for(UserID=0;UserID<MAX_ROLE_GROUPS;UserID++)
	{
		for(GroupRole = AMI_LDAP_GROUP_NAME; GroupRole <= AMI_LDAP_GROUP_EXTENDED_PRIVILEGE;GroupRole++)
		{
			memset(&GetLDAPConfReq,0,sizeof(AMIGetLDAPReq_T));
			memset(&GetLDAPConfRes,0,sizeof(AMIGetLDAPRes_T));
			GetLDAPConfReq.ParameterSelector = AMI_LDAP_GROUPROLE_CONFIG;
			GetLDAPConfReq.UserID=UserID;
			GetLDAPConfReq.GroupRoleconf=GroupRole;
			wRet = IPMICMD_AMIGetLDAPConf(pSession, &GetLDAPConfReq, &GetLDAPConfRes,sizeof(AMIGetLDAPReq_T), timeout);
			if (wRet == CC_SUCCESS)
			{
				switch(GroupRole)
				{
				case AMI_LDAP_GROUP_NAME:
					tempret=snprintf(GetldapPrivConfig[UserID].LDAPRoleGroupNameStr,MAX_GRP_NAME_DOMAIN_LEN,"%s",GetLDAPConfRes.Config.LDAPRoleGroupNameStr);
					if((tempret<0)||(tempret>=MAX_GRP_NAME_DOMAIN_LEN))
                                        {
                                            TCRIT("Buffer Overflow");
                                            return -1;
                                        }
					break;
				case AMI_LDAP_GROUP_DOMAIN:
					tempret=snprintf(GetldapPrivConfig[UserID].LDAPRoleGroupDomainStr,MAX_GRP_NAME_DOMAIN_LEN,"%s",GetLDAPConfRes.Config.LDAPRoleGroupDomainStr);
					if((tempret<0)||(tempret>=MAX_GRP_NAME_DOMAIN_LEN))
                                        {
                                            TCRIT("Buffer Overflow");
                                            return -1;
                                        }
					break;
				case AMI_LDAP_GROUP_PRIVILEGE:
					GetldapPrivConfig[UserID].LDAPRoleGroupPrivilege= GetLDAPConfRes.Config.LDAPRoleGroupPrivilege;
					break;
				case AMI_LDAP_GROUP_EXTENDED_PRIVILEGE:
					GetldapPrivConfig[UserID].LDAPRoleGroupExtendedPrivilege= GetLDAPConfRes.Config.LDAPRoleGroupExtendedPrivilege;
					break;
				}
			}
		}
	}
	return wRet;
}
uint16 LIBIPMI_HL_AMISetLDAPConf(IPMI20_SESSION_T *pSession,IPMILDAPCONFIG * SetldapConfig,int timeout)
{
	AMISetLDAPReq_T SetLDAPConfReq;
	AMISetLDAPRes_T SetLDAPConfRes;
	uint16 wRet = -1;
	int param = 0,Retval[]={3,3,4,4,3,3,49,65,3,9,3};
	INT8U ReqLen=0;
	unsigned int i=0;
	IPMILDAPCONFIG LDAPCfg;
	int tempret=0;/*To check return value of snprintf, it is not used any where else*/

	wRet = LIBIPMI_HL_AMIGetLDAPConf(pSession, &LDAPCfg, timeout);
	if (wRet != CC_SUCCESS) {
		TCRIT("Error getting LDAP configuration::%x\n", wRet);
		return wRet;
	}

	for(param = AMI_LDAP_STATUS; param < AMI_LDAP_GROUPROLE_CONFIG;param++)
	{
		memset(&SetLDAPConfReq,0,sizeof(AMISetLDAPReq_T));
		memset(&SetLDAPConfRes,0,sizeof(AMISetLDAPRes_T));
		SetLDAPConfReq.ParameterSelector = param;
		SetLDAPConfReq.Blockselector=0;
		ReqLen=Retval[param];
		switch(param)
		{
		case AMI_LDAP_STATUS:
			if(LDAPCfg.Enable != SetldapConfig->Enable)
			{
				SetLDAPConfReq.Config.Enable=SetldapConfig->Enable;
				break;
			}
			else
				continue;

		case AMI_LDAP_ENCRYPTEDENABLE:
			if(LDAPCfg.EncryptedEnable != SetldapConfig->EncryptedEnable)
			{
				SetLDAPConfReq.Config.EncryptedEnable = SetldapConfig->EncryptedEnable;
				break;
			}
			else
				continue;

		case AMI_LDAP_PORT_NUM:
			if(LDAPCfg.PortNum != SetldapConfig->PortNum)
			{
				SetLDAPConfReq.Config.PortNum = SetldapConfig->PortNum;
				break;
			}
			else
				continue;

		case AMI_LDAP_PORT_NUM_SEC:
			if(LDAPCfg.PortNumSecondary != SetldapConfig->PortNumSecondary)
			{
				SetLDAPConfReq.Config.PortNumSecondary = SetldapConfig->PortNumSecondary;
				break;
			}
			else
				continue;

		case AMI_LDAP_IP_ADDR:
			if( LDAPCfg.FQDNEnable != SetldapConfig->FQDNEnable )
			{
				//if using FQDN as server address, need to modified FQDN first
				SetLDAPConfReq.ParameterSelector = AMI_LDAP_FQDN_ENABLE;
				SetLDAPConfReq.Blockselector=0;
				ReqLen=3;
				SetLDAPConfReq.Config.FQDNEnable = SetldapConfig->FQDNEnable;
				wRet = IPMICMD_AMISetLDAPConf(pSession, &SetLDAPConfReq, &SetLDAPConfRes,ReqLen, timeout);
				 if (wRet == CC_SUCCESS)
				 {
					 memset(&SetLDAPConfReq,0,sizeof(AMISetLDAPReq_T));
					 memset(&SetLDAPConfRes,0,sizeof(AMISetLDAPRes_T));
					 SetLDAPConfReq.ParameterSelector = param;
					 SetLDAPConfReq.Blockselector=0;
					 ReqLen=Retval[param];
				 }
				 else if ((wRet & 0xFF) == CC_SERVICE_NOT_ENABLED)
				 {
					 wRet = CC_SUCCESS;
					 return wRet;
				 }
				 else
				 	return wRet;
			}
			SetLDAPConfReq.Config.Enable=1;
			wRet = IPMICMD_AMISetLDAPConf(pSession, &SetLDAPConfReq, &SetLDAPConfRes,ReqLen, timeout);
			if ((wRet & 0xFF) == CC_SERVICE_NOT_ENABLED)
			{
				wRet = CC_SUCCESS;
				return wRet;
			}
			else if(wRet!=CC_SUCCESS)
				return wRet;
			for(i = 0;(i < MAX_MSG_BLOCKS)&&((i*MAX_BLOCK_DATA_SIZE)<strlen((char*)SetldapConfig->IPAddr)); i++)
			{
				SetLDAPConfReq.Blockselector = i+1;
				if(SetLDAPConfReq.Blockselector==MAX_MSG_BLOCKS)
					ReqLen=MAX_BLOCK_DATA_SIZE+1;
				else
					ReqLen = MAX_BLOCK_DATA_SIZE+2;
				memcpy(SetLDAPConfReq.Config.IPAddr,SetldapConfig->IPAddr +(i*MAX_BLOCK_DATA_SIZE),MAX_BLOCK_DATA_SIZE);
				wRet = IPMICMD_AMISetLDAPConf(pSession, &SetLDAPConfReq, &SetLDAPConfRes,ReqLen, timeout);
				if(wRet!=CC_SUCCESS)
				return wRet;
			}
			SetLDAPConfReq.Config.Enable=0;
			SetLDAPConfReq.Blockselector=0;
			ReqLen=Retval[param];
			break;
		case AMI_LDAP_IP_ADDR_SEC:
			if(strlen((char*)SetldapConfig->IPAddrSecondary)!=0)
			{
				SetLDAPConfReq.Config.Enable=1;
				wRet = IPMICMD_AMISetLDAPConf(pSession, &SetLDAPConfReq, &SetLDAPConfRes,ReqLen, timeout);
				if(wRet!=CC_SUCCESS)
					return wRet;
				for(i = 0;(i < MAX_MSG_BLOCKS)&&((i*MAX_BLOCK_DATA_SIZE)<strlen((char*)SetldapConfig->IPAddrSecondary)); i++)
				{
					SetLDAPConfReq.Blockselector = i+1;
					if(SetLDAPConfReq.Blockselector==MAX_MSG_BLOCKS)
						ReqLen=MAX_BLOCK_DATA_SIZE+1;
					else
						ReqLen = MAX_BLOCK_DATA_SIZE+2;
					memcpy(SetLDAPConfReq.Config.IPAddrSecondary,SetldapConfig->IPAddrSecondary +(i*MAX_BLOCK_DATA_SIZE),MAX_BLOCK_DATA_SIZE);
					wRet = IPMICMD_AMISetLDAPConf(pSession, &SetLDAPConfReq, &SetLDAPConfRes,ReqLen, timeout);
					if(wRet!=CC_SUCCESS)
						return wRet;
				}
				SetLDAPConfReq.Config.Enable=0;
				SetLDAPConfReq.Blockselector=0;
				ReqLen=Retval[param];
				break;
			}
			else
			{
				continue;
			}
		case AMI_LDAP_PASSWORD:
			if(strcmp(LDAPCfg.Password, SetldapConfig->Password) != 0)
			{
				tempret=snprintf(SetLDAPConfReq.Config.Password,sizeof(SetLDAPConfReq.Config.Password),"%s",SetldapConfig->Password);
				if((tempret<0)||(tempret>=(signed)sizeof(SetLDAPConfReq.Config.Password)))
                                {
                                    TCRIT("Buffer Overflow");
                                    return -1;
                                }
				break;
			}
			else
				continue;

		case AMI_LDAP_BIND_DN:
			if(strcmp(LDAPCfg.BindDN, SetldapConfig->BindDN) != 0)
			{
				tempret=snprintf(SetLDAPConfReq.Config.BindDN,sizeof(SetLDAPConfReq.Config.BindDN),"%s",SetldapConfig->BindDN);
				if((tempret<0)||(tempret>=(signed)sizeof(SetLDAPConfReq.Config.BindDN)))
                                {
                                    TCRIT("Buffer Overflow");
                                    return -1;
                                }
				break;
			}
			else
				continue;

		case AMI_LDAP_SEARCH_BASE:
			if(strcmp(LDAPCfg.SearchBase, SetldapConfig->SearchBase) != 0)
			{
				SetLDAPConfReq.Config.Enable=1;
				wRet = IPMICMD_AMISetLDAPConf(pSession, &SetLDAPConfReq, &SetLDAPConfRes,ReqLen, timeout);
				if(wRet!=CC_SUCCESS)
					return wRet;
				for(i = 0; i < MAX_MSG_BLOCKS-2; i++)
				{
					SetLDAPConfReq.Blockselector = i+1;
					if(SetLDAPConfReq.Blockselector==MAX_MSG_BLOCKS-2)
						ReqLen=MAX_BLOCK_DATA_SIZE+1;
					else
						ReqLen = MAX_BLOCK_DATA_SIZE+2;
					memcpy(SetLDAPConfReq.Config.SearchBase,SetldapConfig->SearchBase +(i*MAX_BLOCK_DATA_SIZE),MAX_BLOCK_DATA_SIZE);
					wRet = IPMICMD_AMISetLDAPConf(pSession, &SetLDAPConfReq, &SetLDAPConfRes,ReqLen, timeout);
					if(wRet!=CC_SUCCESS)
					return wRet;
				}
				SetLDAPConfReq.Config.Enable=0;
				SetLDAPConfReq.Blockselector=0;
				ReqLen=Retval[param];
				break;
			}
			else
				continue;

		case AMI_LDAP_ATTRIBUTE_USER_LOGIN:
			if(strcmp(LDAPCfg.AttributeOfUserLogin, SetldapConfig->AttributeOfUserLogin) != 0)
			{
				tempret=snprintf( SetLDAPConfReq.Config.AttributeOfUserLogin,sizeof(SetLDAPConfReq.Config.AttributeOfUserLogin),"%s",SetldapConfig->AttributeOfUserLogin);
				if((tempret<0)||(tempret>=(signed)sizeof(SetLDAPConfReq.Config.AttributeOfUserLogin)))
                                {
                                     TCRIT("Buffer Overflow");
                                     return -1;
                                }
				break;
			}
			else
				continue;

		case AMI_LDAP_DEFAULT_ROLE:
			if(LDAPCfg.DefaultRole != SetldapConfig->DefaultRole)
			{
				SetLDAPConfReq.Config.DefaultRole=SetldapConfig->DefaultRole;
				break;
			}
			else
				continue;
		}
		wRet = IPMICMD_AMISetLDAPConf(pSession, &SetLDAPConfReq, &SetLDAPConfRes,ReqLen, timeout);
		if((wRet & 0xFF) == CC_SERVICE_NOT_ENABLED) {
			wRet = CC_SUCCESS;
			return wRet;
		} else if (wRet != CC_SUCCESS) return wRet;
	}
	return wRet;
}
uint16 LIBIPMI_HL_AMISetLDAPPrivConf(IPMI20_SESSION_T *pSession,IPMI_LDAP_Config_T *SetldapPrivConfig,int UserID,int timeout)
{
	AMISetLDAPReq_T SetLDAPConfReq;
	AMISetLDAPRes_T SetLDAPConfRes;
	uint16 wRet = -1;
	int GroupRole,i=0,Retval[]={4,0,0,7,7};
	INT8U  ReqLen=3;
	memset(&SetLDAPConfReq,0,sizeof(AMISetLDAPReq_T));
	memset(&SetLDAPConfRes,0,sizeof(AMISetLDAPRes_T));
	SetLDAPConfReq.ParameterSelector = AMI_LDAP_GROUPROLE_CONFIG;
	SetLDAPConfReq.Blockselector=0;
	SetLDAPConfReq.Config.Enable=1;
	wRet = IPMICMD_AMISetLDAPConf(pSession, &SetLDAPConfReq, &SetLDAPConfRes,ReqLen, timeout);
	if(wRet!=CC_SUCCESS)
		return wRet;
	for(GroupRole = AMI_LDAP_GROUP_USERID; GroupRole <= AMI_LDAP_GROUP_EXTENDED_PRIVILEGE;GroupRole++)
	{
		memset(&SetLDAPConfReq,0,sizeof(AMISetLDAPReq_T));
		memset(&SetLDAPConfRes,0,sizeof(AMISetLDAPRes_T));
		SetLDAPConfReq.ParameterSelector = AMI_LDAP_GROUPROLE_CONFIG;
		SetLDAPConfReq.Blockselector=1;
		SetLDAPConfReq.Config.SetRoleData.GroupRolecfg=GroupRole;
		ReqLen=Retval[GroupRole];
		switch(GroupRole)
		{
		case AMI_LDAP_GROUP_USERID:
			SetLDAPConfReq.Config.SetRoleData.LDAPRole.UserID=UserID;
			break;
		case AMI_LDAP_GROUP_NAME:
			for(i = 0;(i < MAX_MSG_BLOCKS)&&((i*MAX_BLOCK_DATA_SIZE)<SetldapPrivConfig->LDAPRoleGroupNameStrlen); i++)
			{
				SetLDAPConfReq.Blockselector = i+1;
				if(SetLDAPConfReq.Blockselector==MAX_MSG_BLOCKS)
					ReqLen=MAX_BLOCK_DATA_SIZE+2;
				else
					ReqLen = MAX_BLOCK_DATA_SIZE+3;
				memcpy(SetLDAPConfReq.Config.SetRoleData.LDAPRole.LDAPRolestr,SetldapPrivConfig->LDAPRoleGroupNameStr +(i*MAX_BLOCK_DATA_SIZE),MAX_BLOCK_DATA_SIZE);
				wRet = IPMICMD_AMISetLDAPConf(pSession, &SetLDAPConfReq, &SetLDAPConfRes,ReqLen, timeout);
				if(wRet!=CC_SUCCESS)
				return wRet;
			}
			continue;
		case AMI_LDAP_GROUP_DOMAIN:
			for(i = 0;(i < MAX_MSG_BLOCKS)&&((i*MAX_BLOCK_DATA_SIZE)<SetldapPrivConfig->LDAPRoleGroupDomainStrlen); i++)
			{
				SetLDAPConfReq.Blockselector = i+1;
				if(SetLDAPConfReq.Blockselector==MAX_MSG_BLOCKS)
					ReqLen=MAX_BLOCK_DATA_SIZE+2;
				else
					ReqLen = MAX_BLOCK_DATA_SIZE+3;
				memcpy(SetLDAPConfReq.Config.SetRoleData.LDAPRole.LDAPRolestr,SetldapPrivConfig->LDAPRoleGroupDomainStr+(i*MAX_BLOCK_DATA_SIZE),MAX_BLOCK_DATA_SIZE);
				wRet = IPMICMD_AMISetLDAPConf(pSession, &SetLDAPConfReq, &SetLDAPConfRes,ReqLen, timeout);
				if(wRet!=CC_SUCCESS)
				return wRet;
			}
			continue;
		case AMI_LDAP_GROUP_PRIVILEGE:
			SetLDAPConfReq.Config.SetRoleData.LDAPRole.LDAPRolePriv=SetldapPrivConfig->LDAPRoleGroupPrivilege;
			break;
		case AMI_LDAP_GROUP_EXTENDED_PRIVILEGE:
			SetLDAPConfReq.Config.SetRoleData.LDAPRole.LDAPRolePriv=SetldapPrivConfig->LDAPRoleGroupExtendedPrivilege;
			break;
		}
		wRet = IPMICMD_AMISetLDAPConf(pSession, &SetLDAPConfReq, &SetLDAPConfRes,ReqLen, timeout);
		if(wRet!=CC_SUCCESS)
			return wRet;
	}
	SetLDAPConfReq.ParameterSelector = AMI_LDAP_GROUPROLE_CONFIG;
	SetLDAPConfReq.Blockselector=0;
	SetLDAPConfReq.Config.Enable=0;
	ReqLen=3;
	wRet = IPMICMD_AMISetLDAPConf(pSession, &SetLDAPConfReq, &SetLDAPConfRes,ReqLen, timeout);
	return wRet;
}

uint16 IPMICMD_AMIGetRadius(IPMI20_SESSION_T *pSession,
				AMIGetRadiusReq_T *pAMIGetRadiusReq,
				AMIGetRadiusRes_T *pAMIGetRadiusRes,int ReqLen,int timeout)
{
	uint16 wRet = -1;
	uint32 wResLen = 0;
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
				((NETFN_AMI << 2) + 0x00), CMD_AMI_GET_RADIUS_CONF,
				(uint8 *)pAMIGetRadiusReq,ReqLen ,
				(uint8 *)pAMIGetRadiusRes, &wResLen, timeout );
	return wRet;
}
uint16 IPMICMD_AMISetRadius(IPMI20_SESSION_T *pSession,
				AMISetRadiusReq_T *pAMISetRadiusReq,
				AMISetRadiusRes_T *pAMISetRadiusRes,int ReqLen,int timeout)
{
	uint16 wRet;
	uint32 wResLen = 0;
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
				((NETFN_AMI << 2) + 0x00), CMD_AMI_SET_RADIUS_CONF,
				(uint8 *)pAMISetRadiusReq,ReqLen ,
				(uint8 *)pAMISetRadiusRes, &wResLen, timeout );
	return wRet;
}
uint16 LIBIPMI_HL_AMIGetRadiusConf(IPMI20_SESSION_T *pSession,IPMIRADIUSCONFIG *radius_config,int enable_ext_prvi,int timeout)
{
	AMIGetRadiusReq_T GetRadiusConfReq;
	AMIGetRadiusRes_T GetRadiusConfRes;
	int param = 0;
	uint16 wRet = -1;
	int tempret=0;/*To check return value of snprintf, it is not used any where else*/
	for(param = AMI_RADIUS_STATUS; param < AMI_RADIUS_VENDOR_DATA;param++)
	{
		if(!enable_ext_prvi && (param == AMI_RADIUS_EXT_PRIVILEGE))
			continue;

		memset(&GetRadiusConfReq,0,sizeof(AMIGetRadiusReq_T));
		memset(&GetRadiusConfRes,0,sizeof(AMIGetRadiusRes_T));
		GetRadiusConfReq.ParameterSelector = param;
		wRet = IPMICMD_AMIGetRadius(pSession, &GetRadiusConfReq, &GetRadiusConfRes,sizeof(INT8U), timeout);
		if (wRet == CC_SUCCESS)
		{
			switch(param)
			{
			case AMI_RADIUS_STATUS:
				radius_config->Enable=GetRadiusConfRes.Config.Status;
				break;
			case AMI_RADIUS_IP_ADDR:
				tempret=snprintf(radius_config->IPAddr,sizeof(radius_config->IPAddr),"%s", GetRadiusConfRes.Config.IPAddr);
				if((tempret<0)||(tempret>=(signed)sizeof(radius_config->IPAddr)))
                                {
                                    TCRIT("Buffer Overflow");
                                    return -1;
                                }
				break;
			case AMI_RADIUS_PORT_NUM:
				radius_config->PortNum=GetRadiusConfRes.Config.PortNum;
				break;
			case AMI_RADIUS_SECRET:
				tempret=snprintf(radius_config->Secret,sizeof(radius_config->Secret),"%s", GetRadiusConfRes.Config.Secret);
				if((tempret<0)||(tempret>=(signed)sizeof(radius_config->Secret)))
                                {
                                    TCRIT("Buffer Overflow");
                                    return -1;
                                }
				break;
			case AMI_RADIUS_TIMEOUT:
				radius_config->Timeout=GetRadiusConfRes.Config.Timeout;
				break;
			case AMI_RADIUS_PRIVILEGE:
				radius_config->Privilege=GetRadiusConfRes.Config.Privilege;
				break;
			case AMI_RADIUS_EXT_PRIVILEGE:
				radius_config->ExtendedPrivilege=GetRadiusConfRes.Config.ExtendedPrivilege;
				break;
			}
		}
	}
	return wRet;
}
uint16 LIBIPMI_HL_AMIGetRadiusprivConf(IPMI20_SESSION_T *pSession,IPMIRADIUSPRIVCONFIG radius_priv_config[],int timeout)
{
	AMIGetRadiusReq_T GetRadiusConfReq;
	AMIGetRadiusRes_T GetRadiusConfRes;
	int Accesslevel;
	uint16 wRet = -1;
	int tempret=0;/*To check return value of snprintf, it is not used any where else*/

	for(Accesslevel = 0 ; Accesslevel < MAX_ACCESS_LEVEL_USERS; Accesslevel++)
	{
		memset(&GetRadiusConfReq,0,sizeof(AMIGetRadiusReq_T));
		memset(&GetRadiusConfRes,0,sizeof(AMIGetRadiusRes_T));
		GetRadiusConfReq.ParameterSelector = AMI_RADIUS_VENDOR_DATA;
		GetRadiusConfReq.Accesslevel=g_accesslevel[Accesslevel];
		wRet = IPMICMD_AMIGetRadius(pSession, &GetRadiusConfReq, &GetRadiusConfRes,sizeof(INT16U), timeout);
		if(wRet == CC_SUCCESS)
		{
			radius_priv_config[Accesslevel].AccessLevel = g_accesslevel[Accesslevel];
			tempret=snprintf((char*)radius_priv_config[Accesslevel].VendorData,RADIUS_VENDORData_LEN,"%s",GetRadiusConfRes.Config.Get_Vendordata);
			if((tempret<0)||(tempret>=RADIUS_VENDORData_LEN))
                        {
                            TCRIT("Buffer Overflow");
                            return -1;
                        }
		}
	}
	return wRet;
}
uint16 LIBIPMI_HL_AMISetRadiusConf(IPMI20_SESSION_T *pSession,IPMIRADIUSCONFIG *radius_config,int enable_ext_prvi,int timeout)
{
	AMISetRadiusReq_T SetRadiusConfReq;
	AMISetRadiusRes_T SetRadiusConfRes;
	int param = 0,ReqLenght[]={3,3,4,33,6,6,6};
	unsigned int i=0;
	INT8U ReqLen=0;
	uint16 wRet = -1;
	int tempret=0;/*To check return value of snprintf, it is not used any where else*/
	for(param = AMI_RADIUS_STATUS; param < AMI_RADIUS_VENDOR_DATA;param++)
	{
		memset(&SetRadiusConfReq,0,sizeof(AMISetRadiusReq_T));
		memset(&SetRadiusConfRes,0,sizeof(AMISetRadiusRes_T));
		SetRadiusConfReq.ParameterSelector = param;
		SetRadiusConfReq.Blockselector=0;
		ReqLen=ReqLenght[param];
		switch(param)
		{
			case AMI_RADIUS_STATUS:
				SetRadiusConfReq.ConfigData.Status=radius_config->Enable;
				break;
			case AMI_RADIUS_IP_ADDR:
				SetRadiusConfReq.ConfigData.Status=1;
				wRet = IPMICMD_AMISetRadius(pSession, &SetRadiusConfReq, &SetRadiusConfRes,ReqLen, timeout);
				if((wRet & 0xFF) == CC_SERVICE_NOT_ENABLED) {
					wRet = CC_SUCCESS;
					return wRet;
				} else if (wRet != CC_SUCCESS) return wRet;
				for(i = 0;(i < MAX_MSG_BLOCKS)&&((i*MAX_BLOCK_DATA_SIZE)<strlen((char*)radius_config->IPAddr)); i++)
				{
					SetRadiusConfReq.Blockselector = i+1;
					if(SetRadiusConfReq.Blockselector==MAX_MSG_BLOCKS)
						ReqLen=MAX_BLOCK_DATA_SIZE+1;
					else
						ReqLen = MAX_BLOCK_DATA_SIZE+2;
					memcpy(SetRadiusConfReq.ConfigData.IPAddr,radius_config->IPAddr +(i*MAX_BLOCK_DATA_SIZE),MAX_BLOCK_DATA_SIZE);
					wRet = IPMICMD_AMISetRadius(pSession, &SetRadiusConfReq, &SetRadiusConfRes,ReqLen, timeout);
					if(wRet!=CC_SUCCESS)
						return wRet;
				}
				SetRadiusConfReq.ConfigData.Status=0;
				SetRadiusConfReq.Blockselector=0;
				ReqLen=ReqLenght[param];
				break;
			case AMI_RADIUS_PORT_NUM:
				SetRadiusConfReq.ConfigData.PortNum=radius_config->PortNum;
				break;
			case AMI_RADIUS_SECRET:
				tempret=snprintf(SetRadiusConfReq.ConfigData.Secret,sizeof(SetRadiusConfReq.ConfigData.Secret),"%s",radius_config->Secret);
				if((tempret<0)||(tempret>=(signed)sizeof(SetRadiusConfReq.ConfigData.Secret)))
                                {
                                    TCRIT("Buffer Overflow");
                                    return -1;
                                }
				break;
			case AMI_RADIUS_TIMEOUT:
				SetRadiusConfReq.ConfigData.Timeout=radius_config->Timeout;
				break;
			case AMI_RADIUS_PRIVILEGE:
				SetRadiusConfReq.ConfigData.Privilege=radius_config->Privilege;
				break;
			case AMI_RADIUS_EXT_PRIVILEGE:
				if(!enable_ext_prvi)
					continue;
				SetRadiusConfReq.ConfigData.ExtendedPrivilege=radius_config->ExtendedPrivilege;
				break;
			}

			wRet = IPMICMD_AMISetRadius(pSession, &SetRadiusConfReq, &SetRadiusConfRes,ReqLen, timeout);
			if(wRet!=CC_SUCCESS)
				return wRet;
		}
	return wRet;
}
uint16 LIBIPMI_HL_AMISetRadiusprivConf(IPMI20_SESSION_T *pSession,IPMIRADIUSPRIVCONFIG radius_priv_config[],int timeout)
{
	AMISetRadiusReq_T SetRadiusConfReq;
	AMISetRadiusRes_T SetRadiusConfRes;
	int i,Accesslevel;
	INT8U ReqLen=3;
	uint16 wRet;

	memset(&SetRadiusConfReq,0,sizeof(AMISetRadiusReq_T));
	memset(&SetRadiusConfRes,0,sizeof(AMISetRadiusRes_T));
	SetRadiusConfReq.ParameterSelector = AMI_RADIUS_VENDOR_DATA;
	SetRadiusConfReq.ConfigData.Status=1;
	SetRadiusConfReq.Blockselector=0;
	wRet = IPMICMD_AMISetRadius(pSession, &SetRadiusConfReq, &SetRadiusConfRes,ReqLen, timeout);
	if(wRet!=CC_SUCCESS)
		return wRet;
	for(Accesslevel=0;Accesslevel<MAX_ACCESS_LEVEL_USERS;Accesslevel++)
	{
		for(i=0;i<MAX_MSG_BLOCKS-2;i++)
		{
			memset(&SetRadiusConfReq,0,sizeof(AMISetRadiusReq_T));
			memset(&SetRadiusConfRes,0,sizeof(AMISetRadiusRes_T));
			SetRadiusConfReq.ParameterSelector = AMI_RADIUS_VENDOR_DATA;
			SetRadiusConfReq.Blockselector=i+1;
			SetRadiusConfReq.ConfigData.VendorData.AccessLevel=g_accesslevel[Accesslevel];
			strncpy((char*)SetRadiusConfReq.ConfigData.VendorData.VendorData,(char*)radius_priv_config[Accesslevel].VendorData+(i*MAX_BLOCK_DATA_SIZE),(sizeof(SetRadiusConfReq.ConfigData.VendorData.VendorData))-1);
			if(i==0)
				ReqLen=MAX_BLOCK_DATA_SIZE+3;
			else
				ReqLen=MAX_BLOCK_DATA_SIZE+2;
			wRet = IPMICMD_AMISetRadius(pSession, &SetRadiusConfReq, &SetRadiusConfRes,ReqLen, timeout);
			if(wRet!=CC_SUCCESS)
				return wRet;
		}
	}
	memset(&SetRadiusConfReq,0,sizeof(AMISetRadiusReq_T));
	memset(&SetRadiusConfRes,0,sizeof(AMISetRadiusRes_T));
	SetRadiusConfReq.ParameterSelector = AMI_RADIUS_VENDOR_DATA;
	SetRadiusConfReq.ConfigData.Status=0;
	SetRadiusConfReq.Blockselector=0;
	ReqLen=3;
	wRet = IPMICMD_AMISetRadius(pSession, &SetRadiusConfReq, &SetRadiusConfRes,ReqLen, timeout);
	return wRet;
}
uint16 IPMICMD_AMIGetADConf(IPMI20_SESSION_T *pSession,
						AMIGetADReq_T *pAMIGetADReq,
						AMIGetADRes_T *pAMIGetADRes,int ReqLen,int timeout)
{
	uint16 wRet;
	uint32 wResLen = 0;
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
				((NETFN_AMI << 2) + 0x00), CMD_AMI_GET_AD_CONF,
				(uint8 *)pAMIGetADReq,ReqLen ,
				(uint8 *)pAMIGetADRes, &wResLen, timeout );
	return wRet;
}
uint16 IPMICMD_AMISetADConf(IPMI20_SESSION_T *pSession,
				AMISetADReq_T *pAMISetADReq,
				AMISetADRes_T *pAMISetADRes,int ReqLen,int timeout)
{
	uint16 wRet;
	uint32 wResLen = 0;
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
				((NETFN_AMI << 2) + 0x00), CMD_AMI_SET_AD_CONF,
				(uint8 *)pAMISetADReq,ReqLen ,
				(uint8 *)pAMISetADRes, &wResLen, timeout );
	return wRet;
}

uint16 LIBIPMI_HL_AMIGetADConf(IPMI20_SESSION_T *pSession,IPMI_AD_Config_T* ad_config,int timeout)
{
	AMIGetADReq_T GetADConfReq;
	AMIGetADRes_T GetADConfRes;
	int param = 0;
	uint16 wRet = -1;
	int tempret=0;/*To check return value of snprintf, it is not used any where else*/
	for(param = AMI_AD_ENABLE; param < AMI_AD_GROUPROLE_CONFIG;param++)
	{
		memset(&GetADConfReq,0,sizeof(AMIGetADReq_T));
		memset(&GetADConfRes,0,sizeof(AMIGetADRes_T));
		GetADConfReq.ParameterSelector = param;
		wRet = IPMICMD_AMIGetADConf(pSession, &GetADConfReq, &GetADConfRes,sizeof(INT8U), timeout);
		if (wRet == CC_SUCCESS)
		{
			switch(param)
			{
			case AMI_AD_ENABLE:
				ad_config->ADEnable=GetADConfRes.Config.ADEnable;
				break;
			case AMI_AD_SSL_ENABLE:
				ad_config->SSLEnable=GetADConfRes.Config.SSLEnable;
				break;
			case AMI_AD_TIME_OUT:
				ad_config->ADTimeout=GetADConfRes.Config.ADTimeout;
				break;
			case AMI_AD_RAC_DOMAIN:
				tempret=snprintf((char*)ad_config->ADRACDomainStr,sizeof(ad_config->ADRACDomainStr),"%s",(char*)GetADConfRes.Config.ADRACDomainStr);
				if((tempret<0)||(tempret>=(signed)sizeof(ad_config->ADRACDomainStr)))
                                {
                                    TCRIT("Buffer Overflow");
                                    return -1;
                                }
				break;
			case AMI_AD_TYPE:
				ad_config->ADType=GetADConfRes.Config.ADType;
				break;
			case AMI_AD_DC_FILTER1:
				tempret=snprintf((char*)ad_config->ADDCFilter1,sizeof(ad_config->ADDCFilter1),"%s",(char*)GetADConfRes.Config.ADDCFilter1);
				if((tempret<0)||(tempret>=(signed)sizeof(ad_config->ADDCFilter1)))
                                {
                                    TCRIT("Buffer Overflow");
                                    return -1;
                                }
				break;
			case AMI_AD_DC_FILTER2:
				tempret=snprintf((char*)ad_config->ADDCFilter2,sizeof(ad_config->ADDCFilter2),"%s",(char*)GetADConfRes.Config.ADDCFilter2);
				if((tempret<0)||(tempret>=(signed)sizeof(ad_config->ADDCFilter2)))
                                {
                                    TCRIT("Buffer Overflow");
                                    return -1;
                                }
				break;
			case AMI_AD_DC_FILTER3:
				tempret=snprintf((char*)ad_config->ADDCFilter3,sizeof(ad_config->ADDCFilter3),"%s",(char*)GetADConfRes.Config.ADDCFilter3);
				if((tempret<0)||(tempret>=(signed)sizeof(ad_config->ADDCFilter3)))
                                {
                                     TCRIT("Buffer Overflow");
                                     return -1;
                                }
				break;
			case AMI_AD_RAC_USERNAME:
				tempret=snprintf((char*)ad_config->ADRACUserName,sizeof(ad_config->ADRACUserName),"%s",(char*)GetADConfRes.Config.ADRACUserName);
				if((tempret<0)||(tempret>=(signed)sizeof(ad_config->ADRACUserName)))
                                {
                                     TCRIT("Buffer Overflow");
                                     return -1;
                                }
				break;
			case AMI_AD_RAC_PASSWORD:
				tempret=snprintf((char*)ad_config->ADRACPassword,sizeof(ad_config->ADRACPassword),"%s",(char*)GetADConfRes.Config.ADRACPassword);
				if((tempret<0)||(tempret>=(signed)sizeof(ad_config->ADRACPassword)))
                                {
                                    TCRIT("Buffer Overflow");
                                    return -1;
                                }
				break;
			}
		}
	}
	return wRet;
}
uint16 LIBIPMI_HL_AMIGetADprivConf(IPMI20_SESSION_T *pSession,IPMI_SSAD_Config_T ad_priv_config[],int timeout)
{
	AMIGetADReq_T GetADConfReq;
	AMIGetADRes_T GetADConfRes;
	int UserID,Role;
	uint16 wRet = -1;
	int tempret=0;/*To check return value of snprintf, it is not used any where else*/
	for(UserID =0 ; UserID < MAX_ACCESS_LEVEL_USERS;UserID++)
	{
		for(Role=AMI_AD_GROUP_NAME;Role<=AMI_AD_GROUP_EXTENDED_PRIVILEGE;Role++)
		{
			memset(&GetADConfReq,0,sizeof(AMIGetADReq_T));
			memset(&GetADConfRes,0,sizeof(AMIGetADRes_T));
			GetADConfReq.ParameterSelector = AMI_AD_GROUPROLE_CONFIG;
			GetADConfReq.UserID=UserID;
			GetADConfReq.GroupRoleconf=Role;
			wRet = IPMICMD_AMIGetADConf(pSession, &GetADConfReq, &GetADConfRes,sizeof(AMIGetADReq_T), timeout);
			if(wRet == CC_SUCCESS)
			{
				switch(Role)
				{
				case AMI_AD_GROUP_NAME:
					 tempret=snprintf((char*)ad_priv_config[UserID].SSADRoleGroupNameStr,MAX_AD_ROLE_NAME_LEN,"%s",(char*)GetADConfRes.Config.SSADRoleGroupNameStr);
					 if((tempret<0)||(tempret>=MAX_AD_ROLE_NAME_LEN))
                                         {
                                             TCRIT("Buffer Overflow");
                                             return -1;
                                         }
					 break;
				case AMI_AD_GROUP_DOMAIN:
					 tempret=snprintf((char*)ad_priv_config[UserID].SSADRoleGroupDomainStr,MAX_AD_ROLE_DOMAIN_LEN,"%s",(char*)GetADConfRes.Config.SSADRoleGroupDomainStr);
					 if((tempret<0)||(tempret>=MAX_AD_ROLE_DOMAIN_LEN))
                                         {
                                              TCRIT("Buffer Overflow");
                                              return -1;
                                         }
					 break;
				case AMI_AD_GROUP_PRIVILEGE:
					 ad_priv_config[UserID].SSADRoleGroupPrivilege=GetADConfRes.Config.SSADRoleGroupPrivilege;
					 break;
				case AMI_AD_GROUP_EXTENDED_PRIVILEGE:
					 ad_priv_config[UserID].ExtendedPrivilege=GetADConfRes.Config.ExtendedPrivilege;
					 break;
				}
			}
		}
	}
	return wRet;
}
uint16 LIBIPMI_HL_AMISetADConf(IPMI20_SESSION_T *pSession,IPMI_AD_Config_T* ad_config,int timeout)
{
	AMISetADReq_T SetADConfReq;
	AMISetADRes_T SetADConfRes;
	int param = 0,i,Retval[]={3,3,6,3,3,3,3,3,66,3};
	uint16 wRet = -1;
	INT8U ReqLen=0;
	int tempret=0;/*To check return value of snprintf, it is not used any where else*/
	for(param = AMI_AD_ENABLE; param < AMI_AD_GROUPROLE_CONFIG;param++)
	{
		memset(&SetADConfReq,0,sizeof(AMISetADReq_T));
		memset(&SetADConfRes,0,sizeof(AMISetADRes_T));
		SetADConfReq.ParameterSelector = param;
		SetADConfReq.Blockselector=0;
		ReqLen=Retval[param];
		switch(param)
		{
		case AMI_AD_ENABLE:
			SetADConfReq.Configdata.ADEnable=ad_config->ADEnable;
			break;
		case AMI_AD_SSL_ENABLE:
			SetADConfReq.Configdata.SSLEnable=ad_config->SSLEnable;
			break;
		case AMI_AD_TIME_OUT:
			SetADConfReq.Configdata.ADTimeout=ad_config->ADTimeout;
			break;
		case AMI_AD_RAC_DOMAIN:
			SetADConfReq.Configdata.Progressbit=1;
			wRet=IPMICMD_AMISetADConf(pSession, &SetADConfReq, &SetADConfRes,ReqLen, timeout);
			if(wRet!=CC_SUCCESS)
				return wRet;
			for(i = 0; (i < MAX_MSG_BLOCKS)&&((i*MAX_BLOCK_DATA_SIZE)<ad_config->ADRACDomainStrlen); i++)
			{
				SetADConfReq.Blockselector = i+1;
				if(SetADConfReq.Blockselector==MAX_MSG_BLOCKS)
					ReqLen=MAX_BLOCK_DATA_SIZE+1;
				else
					ReqLen = MAX_BLOCK_DATA_SIZE+2;
				memcpy(SetADConfReq.Configdata.ADRACDomainStr,ad_config->ADRACDomainStr +(i*MAX_BLOCK_DATA_SIZE),MAX_BLOCK_DATA_SIZE);
				wRet = IPMICMD_AMISetADConf(pSession, &SetADConfReq, &SetADConfRes,ReqLen, timeout);
				if(wRet!=CC_SUCCESS)
					return wRet;
			}
			SetADConfReq.Configdata.Progressbit=0;
			SetADConfReq.Blockselector=0;
			ReqLen=Retval[param];
			break;
		case AMI_AD_TYPE:
			SetADConfReq.Configdata.ADType=ad_config->ADType;
			break;
		case AMI_AD_DC_FILTER1:
			if(ad_config->ADDCFilter1Len!=0)
			{
				SetADConfReq.Configdata.Progressbit=1;
				wRet = IPMICMD_AMISetADConf(pSession, &SetADConfReq, &SetADConfRes,ReqLen, timeout);
				if(wRet!=CC_SUCCESS)
					  return wRet;
				for(i = 0; (i < MAX_MSG_BLOCKS)&&((i*MAX_BLOCK_DATA_SIZE)<ad_config->ADDCFilter1Len); i++)
				{
					SetADConfReq.Blockselector = i+1;
					if(SetADConfReq.Blockselector==MAX_MSG_BLOCKS)
						ReqLen=MAX_BLOCK_DATA_SIZE+1;
					else
						ReqLen = MAX_BLOCK_DATA_SIZE+2;
					memcpy(SetADConfReq.Configdata.ADDCFilter1,ad_config->ADDCFilter1 +(i*MAX_BLOCK_DATA_SIZE),MAX_BLOCK_DATA_SIZE);
					wRet = IPMICMD_AMISetADConf(pSession, &SetADConfReq, &SetADConfRes,ReqLen, timeout);
					if(wRet!=CC_SUCCESS)
						return wRet;
				}
				SetADConfReq.Configdata.Progressbit=0;
				SetADConfReq.Blockselector=0;
				ReqLen=Retval[param];
				break;
			}
			else
			continue;
		case AMI_AD_DC_FILTER2:
			if(ad_config->ADDCFilter2Len!=0)
			{
				SetADConfReq.Configdata.Progressbit=1;
				wRet = IPMICMD_AMISetADConf(pSession, &SetADConfReq, &SetADConfRes,ReqLen, timeout);
				if(wRet!=CC_SUCCESS)
					return wRet;
				for(i = 0; (i < MAX_MSG_BLOCKS)&&((i*MAX_BLOCK_DATA_SIZE)<ad_config->ADDCFilter2Len); i++)
				{
					SetADConfReq.Blockselector = i+1;
					if(SetADConfReq.Blockselector==MAX_MSG_BLOCKS)
						ReqLen=MAX_BLOCK_DATA_SIZE+1;
					else
						ReqLen = MAX_BLOCK_DATA_SIZE+2;
					memcpy(SetADConfReq.Configdata.ADDCFilter2,ad_config->ADDCFilter2 +(i*MAX_BLOCK_DATA_SIZE),MAX_BLOCK_DATA_SIZE);
					wRet = IPMICMD_AMISetADConf(pSession, &SetADConfReq, &SetADConfRes,ReqLen, timeout);
					if(wRet!=CC_SUCCESS)
						return wRet;
				}
				SetADConfReq.Configdata.Progressbit=0;
				SetADConfReq.Blockselector=0;
				ReqLen=Retval[param];
				break;
			}
			else
				continue;
		case AMI_AD_DC_FILTER3:
			if(ad_config->ADDCFilter3Len!=0)
			{
				SetADConfReq.Configdata.Progressbit=1;
				wRet = IPMICMD_AMISetADConf(pSession, &SetADConfReq, &SetADConfRes,ReqLen, timeout);
				if(wRet!=CC_SUCCESS)
					  return wRet;
				for(i = 0;(i < MAX_MSG_BLOCKS)&&((i*MAX_BLOCK_DATA_SIZE)<ad_config->ADDCFilter3Len); i++)
				{
					SetADConfReq.Blockselector = i+1;
					if(SetADConfReq.Blockselector==MAX_MSG_BLOCKS)
						ReqLen=MAX_BLOCK_DATA_SIZE+1;
					else
						ReqLen = MAX_BLOCK_DATA_SIZE+2;
					memcpy(SetADConfReq.Configdata.ADDCFilter3,ad_config->ADDCFilter3 +(i*MAX_BLOCK_DATA_SIZE),MAX_BLOCK_DATA_SIZE);
					wRet = IPMICMD_AMISetADConf(pSession, &SetADConfReq, &SetADConfRes,ReqLen, timeout);
					if(wRet!=CC_SUCCESS)
						return wRet;
				}
				SetADConfReq.Configdata.Progressbit=0;
				SetADConfReq.Blockselector=0;
				ReqLen=Retval[param];
				break;
			}
			else
				continue;
		case AMI_AD_RAC_USERNAME:
			tempret=snprintf((char*)SetADConfReq.Configdata.ADRACUserName,sizeof(SetADConfReq.Configdata.ADRACUserName),"%s",(char*)ad_config->ADRACUserName);
			if((tempret<0)||(tempret>=(signed)sizeof(SetADConfReq.Configdata.ADRACUserName)))
                        {
                            TCRIT("Buffer Overflow");
                            return -1;
                        }
			break;
		case AMI_AD_RAC_PASSWORD:
			SetADConfReq.Configdata.Progressbit=1;
			wRet = IPMICMD_AMISetADConf(pSession, &SetADConfReq, &SetADConfRes,ReqLen, timeout);
			if(wRet!=CC_SUCCESS)
				return wRet;
			for(i = 0; i < MAX_MSG_BLOCKS-2; i++)
			{
				SetADConfReq.Blockselector = i+1;
				if(SetADConfReq.Blockselector==MAX_MSG_BLOCKS-2)
					ReqLen=MAX_BLOCK_DATA_SIZE+1;
				else
					ReqLen = MAX_BLOCK_DATA_SIZE+2;
				memcpy(SetADConfReq.Configdata.ADRACPassword,ad_config->ADRACPassword +(i*MAX_BLOCK_DATA_SIZE),MAX_BLOCK_DATA_SIZE);
				wRet = IPMICMD_AMISetADConf(pSession, &SetADConfReq, &SetADConfRes,ReqLen, timeout);
				if(wRet!=CC_SUCCESS)
					return wRet;
			}
			SetADConfReq.Configdata.Progressbit=0;
			SetADConfReq.Blockselector=0;
			ReqLen=Retval[param];
			break;
	}
	wRet=IPMICMD_AMISetADConf(pSession, &SetADConfReq, &SetADConfRes,ReqLen, timeout);
	if((wRet & 0xFF) == CC_SERVICE_NOT_ENABLED) {
		wRet = CC_SUCCESS;
		return wRet;
	} else if (wRet != CC_SUCCESS) return wRet;
	}
	return wRet;
}
uint16 LIBIPMI_HL_AMISetADprivConf(IPMI20_SESSION_T *pSession,IPMI_SSAD_Config_T *ad_priv_config,int UserID,int timeout)
{
	AMISetADReq_T SetADConfReq;
	AMISetADRes_T SetADConfRes;
	int Role,i,Retval[]={4,0,0,7,7};
	uint16 wRet = -1;
	INT8U ReqLen;
	memset(&SetADConfReq,0,sizeof(AMISetADReq_T));
	memset(&SetADConfRes,0,sizeof(AMISetADRes_T));
	SetADConfReq.ParameterSelector = AMI_AD_GROUPROLE_CONFIG;
	SetADConfReq.Configdata.Progressbit=1;
	SetADConfReq.Blockselector=0;
	wRet = IPMICMD_AMISetADConf(pSession, &SetADConfReq, &SetADConfRes,3, timeout);
	if(wRet!=CC_SUCCESS)
		return wRet;
	for(Role=AMI_AD_GROUP_USERID;Role<=AMI_AD_GROUP_EXTENDED_PRIVILEGE;Role++)
	{
		memset(&SetADConfReq,0,sizeof(AMISetADReq_T));
		memset(&SetADConfRes,0,sizeof(AMISetADRes_T));
		SetADConfReq.ParameterSelector = AMI_AD_GROUPROLE_CONFIG;
		SetADConfReq.Configdata.SetRoleData.GroupRolecfg=Role;
		SetADConfReq.Blockselector=1;
		ReqLen=Retval[Role];
		switch(Role)
		{
		case AMI_AD_GROUP_USERID:
			SetADConfReq.Configdata.SetRoleData.ADRole.UserID=UserID;
			break;
		case AMI_AD_GROUP_NAME:
			for(i=0;(i < MAX_MSG_BLOCKS)&&((i*MAX_BLOCK_DATA_SIZE)<ad_priv_config->SSADRoleGroupNameStrlen);i++)
			{
				SetADConfReq.Blockselector = i+1;
				if(SetADConfReq.Blockselector==MAX_MSG_BLOCKS)
					ReqLen = MAX_BLOCK_DATA_SIZE+2;
				else
					ReqLen = MAX_BLOCK_DATA_SIZE+3;
				memcpy(SetADConfReq.Configdata.SetRoleData.ADRole.ADRolestr,ad_priv_config->SSADRoleGroupNameStr +(i*MAX_BLOCK_DATA_SIZE),MAX_BLOCK_DATA_SIZE);
				wRet = IPMICMD_AMISetADConf(pSession, &SetADConfReq, &SetADConfRes,ReqLen, timeout);
				if(wRet!=CC_SUCCESS)
					return wRet;
			}
			continue;
		case AMI_AD_GROUP_DOMAIN:
			for(i=0;(i < MAX_MSG_BLOCKS)&&((i*MAX_BLOCK_DATA_SIZE)<ad_priv_config->SSADRoleGroupDomainStrlen);i++)
			{
				SetADConfReq.Blockselector = i+1;
				if(SetADConfReq.Blockselector==MAX_MSG_BLOCKS)
					ReqLen = MAX_BLOCK_DATA_SIZE+2;
				else
					ReqLen = MAX_BLOCK_DATA_SIZE+3;
				memcpy(SetADConfReq.Configdata.SetRoleData.ADRole.ADRolestr,ad_priv_config->SSADRoleGroupDomainStr +(i*MAX_BLOCK_DATA_SIZE),MAX_BLOCK_DATA_SIZE);
				wRet = IPMICMD_AMISetADConf(pSession, &SetADConfReq, &SetADConfRes,ReqLen, timeout);
				if(wRet!=CC_SUCCESS)
					return wRet;
			}
			continue;
		case AMI_AD_GROUP_PRIVILEGE:
			SetADConfReq.Configdata.SetRoleData.ADRole.ADRolePriv=ad_priv_config->SSADRoleGroupPrivilege;
			break;
		case AMI_AD_GROUP_EXTENDED_PRIVILEGE:
			SetADConfReq.Configdata.SetRoleData.ADRole.ADRolePriv=ad_priv_config->ExtendedPrivilege;
			break;
		}
		wRet = IPMICMD_AMISetADConf(pSession, &SetADConfReq, &SetADConfRes,ReqLen, timeout);
		if(wRet!=CC_SUCCESS)
			return wRet;
	}
	SetADConfReq.ParameterSelector = AMI_AD_GROUPROLE_CONFIG;
	SetADConfReq.Configdata.Progressbit=0;
	SetADConfReq.Blockselector=0;
	wRet = IPMICMD_AMISetADConf(pSession, &SetADConfReq, &SetADConfRes,3, timeout);
	return wRet;
}

uint16 IPMICMD_AMIGetVideoRcdConf(IPMI20_SESSION_T *pSession,
				AMIGetVideoRcdReq_T *pAMIGetVideoRcdReq,
				AMIGetVideoRcdRes_T *pAMIGetVideoRcdRes,
				int timeout)
{
	uint16 wRet;
	uint32 wResLen = 0;
	uint32 wReqLen = sizeof(AMIGetVideoRcdReq_T);
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
				((NETFN_AMI << 2) + 0x00), CMD_AMI_GET_VIDEO_RCD_CONF,
				(uint8 *)pAMIGetVideoRcdReq, wReqLen,
				(uint8 *)pAMIGetVideoRcdRes, &wResLen, timeout );
	return wRet;
}
int16 IPMICMD_AMISetVideoRcdConf(IPMI20_SESSION_T *pSession,
				AMISetVideoRcdReq_T *pAMISetVideoRcdReq,
				AMISetVideoRcdRes_T *pAMISetVideoRcdRes,
				int ReqLen,int timeout)
{
	uint16 wRet;
	uint32 wResLen = 0;
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
				((NETFN_AMI << 2) + 0x00), CMD_AMI_SET_VIDEO_RCD_CONF,
				(uint8 *)pAMISetVideoRcdReq, ReqLen,
				(uint8 *)pAMISetVideoRcdRes, &wResLen, timeout );
	return wRet;
}

uint16 LIBIPMI_HL_AMIGetVideoRcdConf(IPMI20_SESSION_T *pSession,IPMI_AutoRecordCfg_T *video_rcd_config,int timeout)
{
	AMIGetVideoRcdReq_T GetVideoRcdConfReq;
	AMIGetVideoRcdRes_T GetVideoRcdConfRes;
	int param = 0;
	uint16 wRet;
	for(param = AMI_VIDEO_RCD_REMOTE_PATH_SUPPORT; param <= AMI_VIDEO_RCD_MNT_SUCCESS;param++)
	{
		memset(&GetVideoRcdConfReq,0,sizeof(AMIGetVideoRcdReq_T));
		memset(&GetVideoRcdConfRes,0,sizeof(AMIGetVideoRcdRes_T));
		GetVideoRcdConfReq.ParameterSelector = param;
		wRet = IPMICMD_AMIGetVideoRcdConf(pSession, &GetVideoRcdConfReq, &GetVideoRcdConfRes, timeout);
		if(wRet==CC_SUCCESS)
		{
			switch(param)
			{
			case AMI_VIDEO_RCD_REMOTE_PATH_SUPPORT:
				video_rcd_config->remote_path_support=GetVideoRcdConfRes.Config.remote_path_support;
				break;
			case AMI_VIDEO_RCD_PATH:
				strncpy(video_rcd_config->path,GetVideoRcdConfRes.Config.path,MAX_IMAGE_PATH_SIZE-1);
				break;
			case AMI_VIDEO_RCD_MAX_TIME:
				video_rcd_config->max_time=GetVideoRcdConfRes.Config.max_time;
				break;
			case AMI_VIDEO_RCD_MAX_SIZE:
				video_rcd_config->max_size=GetVideoRcdConfRes.Config.max_size;
				break;
			case AMI_VIDEO_RCD_MAX_DUMP:
				video_rcd_config->max_dumps=GetVideoRcdConfRes.Config.max_dumps;
				break;
			case AMI_VIDEO_RCD_IP_ADDR:
				strncpy(video_rcd_config->ip_addr,GetVideoRcdConfRes.Config.ip_addr,MAX_IP_ADDR_LEN-1);
				break;
			case AMI_VIDEO_RCD_USERNAME:
				strncpy(video_rcd_config->username,GetVideoRcdConfRes.Config.username,MAX_RMS_USERNAME_LEN-1);
				break;
			case AMI_VIDEO_RCD_PASSWORD:
				strncpy(video_rcd_config->password,GetVideoRcdConfRes.Config.password,MAX_RMS_PASSWORD_LEN-1);
				break;
			case AMI_VIDEO_RCD_SHARE_TYPE:
				strncpy(video_rcd_config->share_type,GetVideoRcdConfRes.Config.share_type,MAX_SHARE_TYPE_LEN-1);
				break;
			case AMI_VIDEO_RCD_DOMAIN:
				strncpy(video_rcd_config->domain,GetVideoRcdConfRes.Config.domain,MAX_DOMAIN_LEN-1);
				break;
			case AMI_VIDEO_RCD_MNT_SUCCESS:
				video_rcd_config->mnt_successful=GetVideoRcdConfRes.Config.mnt_successful;
				break;
			}
		}
	}
	return wRet;
}

uint16 LIBIPMI_HL_AMIGetContVideoRcdConf(IPMI20_SESSION_T *pSession, AMIGetVideoRcdRes_T *GetVideoRcdConfRes,int timeout)
{
		AMIGetVideoRcdReq_T GetVideoRcdConfReq;
		uint16 wRet;

		memset(&GetVideoRcdConfReq, 0, sizeof(AMIGetVideoRcdReq_T));
		GetVideoRcdConfReq.ParameterSelector = AMI_CONT_VIDEO_RCD_CONF;
		wRet = IPMICMD_AMIGetVideoRcdConf(pSession, &GetVideoRcdConfReq, GetVideoRcdConfRes, timeout);
		if (wRet != CC_SUCCESS) {
			TCRIT("There was a problem while getting Pre-Event video record config::%x\n", wRet);
		}
		return wRet;
}

uint16 LIBIPMI_HL_AMISetContVideoRcdConf(IPMI20_SESSION_T *pSession, AMISetVideoRcdReq_T *SetVideoRcdConfReq,int timeout)
{
	AMISetVideoRcdRes_T SetVideoRcdConfRes;
	INT8U ReqLen = 6;
	uint16 wRet;

	memset(&SetVideoRcdConfRes,0,sizeof(AMISetVideoRcdRes_T));
	SetVideoRcdConfReq->ParameterSelector = AMI_CONT_VIDEO_RCD_CONF;
	SetVideoRcdConfReq->Blockselector=0;

	wRet = IPMICMD_AMISetVideoRcdConf(pSession, SetVideoRcdConfReq, &SetVideoRcdConfRes, ReqLen, timeout);
	if (wRet != CC_SUCCESS) {
		TCRIT("There was a problem while Setting Pre-Event video record config::%x\n", wRet);
	}
	return wRet;
}

uint16 LIBIPMI_HL_AMISetVideoRcdConf(IPMI20_SESSION_T *pSession,IPMI_AutoRecordCfg_T *video_rcd_config,int timeout)
{
	AMISetVideoRcdReq_T SetVideoRcdConfReq;
	AMISetVideoRcdRes_T SetVideoRcdConfRes;
	int param;
	unsigned int i=0;
	uint16 wRet = -1;
	INT8U ReqLen=0;
	int DomainLen = 0;
	for(param = AMI_VIDEO_RCD_REMOTE_PATH_SUPPORT; param <= AMI_VIDEO_RCD_RST;param++)
	{
		memset(&SetVideoRcdConfReq,0,sizeof(AMISetVideoRcdReq_T));
		memset(&SetVideoRcdConfRes,0,sizeof(AMISetVideoRcdRes_T));
		SetVideoRcdConfReq.ParameterSelector = param;
		SetVideoRcdConfReq.Blockselector=0;
		switch(param)
		{
		case AMI_VIDEO_RCD_REMOTE_PATH_SUPPORT:
			SetVideoRcdConfReq.Config.remote_path_support=video_rcd_config->remote_path_support;
			ReqLen=(sizeof(AMISetVideoRcdReq_T)-sizeof(VideoRcd_Config_T)+sizeof(SetVideoRcdConfReq.Config.remote_path_support));
			wRet=IPMICMD_AMISetVideoRcdConf(pSession, &SetVideoRcdConfReq, &SetVideoRcdConfRes,ReqLen, timeout);
			if((wRet!=CC_SUCCESS)&&((wRet & 0xFF)!=CC_MOUNT_FAILED_CFG_SAVE))
				return wRet;
			if(video_rcd_config->remote_path_support == 0)
				param = AMI_VIDEO_RCD_MNT_SUCCESS;
			continue;
		case AMI_VIDEO_RCD_PATH:
			SetVideoRcdConfReq.Config.Progressbit=1;
			ReqLen=(sizeof(AMISetVideoRcdReq_T)-sizeof(VideoRcd_Config_T)+sizeof(SetVideoRcdConfReq.Config.Progressbit));
			wRet=IPMICMD_AMISetVideoRcdConf(pSession,&SetVideoRcdConfReq,&SetVideoRcdConfRes,ReqLen, timeout);
			if((wRet!=CC_SUCCESS)&&((wRet & 0xFF)!=CC_MOUNT_FAILED_CFG_SAVE))
				return wRet;
			for(i = 0; (i < MAX_MSG_BLOCKS)&&((i*MAX_BLOCK_DATA_SIZE)<strlen(video_rcd_config->path)); i++)
			{
				SetVideoRcdConfReq.Blockselector = i+1;
				if(SetVideoRcdConfReq.Blockselector==MAX_MSG_BLOCKS)
					ReqLen=MAX_BLOCK_DATA_SIZE+1;
				else
					ReqLen = MAX_BLOCK_DATA_SIZE+2;
				memcpy(SetVideoRcdConfReq.Config.path,video_rcd_config->path +(i*MAX_BLOCK_DATA_SIZE),MAX_BLOCK_DATA_SIZE);
				wRet = IPMICMD_AMISetVideoRcdConf(pSession, &SetVideoRcdConfReq, &SetVideoRcdConfRes,ReqLen, timeout);
				if((wRet!=CC_SUCCESS)&&((wRet & 0xFF)!=CC_MOUNT_FAILED_CFG_SAVE))
					return wRet;
			}
			SetVideoRcdConfReq.Config.Progressbit=0;
			SetVideoRcdConfReq.Blockselector=0;
			ReqLen=(sizeof(AMISetVideoRcdReq_T)-sizeof(VideoRcd_Config_T)+sizeof(SetVideoRcdConfReq.Config.Progressbit));
			break;
		case AMI_VIDEO_RCD_MAX_TIME:
			SetVideoRcdConfReq.Config.max_time=video_rcd_config->max_time;
			ReqLen=(sizeof(AMISetVideoRcdReq_T)-sizeof(VideoRcd_Config_T)+sizeof(SetVideoRcdConfReq.Config.max_time));
			break;
		case AMI_VIDEO_RCD_MAX_SIZE:
			SetVideoRcdConfReq.Config.max_size=video_rcd_config->max_size;
			ReqLen=(sizeof(AMISetVideoRcdReq_T)-sizeof(VideoRcd_Config_T)+sizeof(SetVideoRcdConfReq.Config.max_size));
			break;
		case AMI_VIDEO_RCD_MAX_DUMP:
			SetVideoRcdConfReq.Config.max_dumps=video_rcd_config->max_dumps;
			ReqLen=(sizeof(AMISetVideoRcdReq_T)-sizeof(VideoRcd_Config_T)+sizeof(SetVideoRcdConfReq.Config.max_dumps));
			break;
		case AMI_VIDEO_RCD_IP_ADDR:
			if(strlen(video_rcd_config->ip_addr)!=0)
			{
				strncpy(SetVideoRcdConfReq.Config.ip_addr,video_rcd_config->ip_addr,MAX_IP_ADDR_LEN-1);
				ReqLen=(sizeof(AMISetVideoRcdReq_T)-sizeof(VideoRcd_Config_T)+MAX_IP_ADDR_LEN-sizeof(INT8U));
				break;
			}
			else
				continue;
		case AMI_VIDEO_RCD_USERNAME:
			SetVideoRcdConfReq.Config.Progressbit=1;
			ReqLen=(sizeof(AMISetVideoRcdReq_T)-sizeof(VideoRcd_Config_T)+sizeof(SetVideoRcdConfReq.Config.Progressbit));
			wRet=IPMICMD_AMISetVideoRcdConf(pSession,&SetVideoRcdConfReq,&SetVideoRcdConfRes,ReqLen, timeout);
			if((wRet!=CC_SUCCESS)&&((wRet & 0xFF)!=CC_MOUNT_FAILED_CFG_SAVE))
				return wRet;
			for(i = 0; (i < MAX_MSG_BLOCKS)&&((i*MAX_BLOCK_DATA_SIZE)<strlen(video_rcd_config->username)); i++)
			{
				SetVideoRcdConfReq.Blockselector = i+1;
				if(SetVideoRcdConfReq.Blockselector==MAX_MSG_BLOCKS)
					ReqLen=MAX_BLOCK_DATA_SIZE+1;
				else
					ReqLen = MAX_BLOCK_DATA_SIZE+2;
				memcpy(SetVideoRcdConfReq.Config.username,video_rcd_config->username +(i*MAX_BLOCK_DATA_SIZE),MAX_BLOCK_DATA_SIZE);
				wRet = IPMICMD_AMISetVideoRcdConf(pSession, &SetVideoRcdConfReq, &SetVideoRcdConfRes,ReqLen, timeout);
				if((wRet!=CC_SUCCESS)&&((wRet & 0xFF)!=CC_MOUNT_FAILED_CFG_SAVE))
					return wRet;
			}
			SetVideoRcdConfReq.Config.Progressbit=0;
			SetVideoRcdConfReq.Blockselector=0;
			ReqLen=(sizeof(AMISetVideoRcdReq_T)-sizeof(VideoRcd_Config_T)+sizeof(SetVideoRcdConfReq.Config.Progressbit));
			break;
		case AMI_VIDEO_RCD_PASSWORD:
			strncpy(SetVideoRcdConfReq.Config.password,video_rcd_config->password,MAX_RMS_PASSWORD_LEN-1);
			ReqLen=(sizeof(AMISetVideoRcdReq_T)-sizeof(VideoRcd_Config_T)+MAX_RMS_PASSWORD_LEN-sizeof(INT8U));
			break;
		case AMI_VIDEO_RCD_SHARE_TYPE:
			strncpy(SetVideoRcdConfReq.Config.share_type,video_rcd_config->share_type,MAX_SHARE_TYPE_LEN-1);
			ReqLen=(sizeof(AMISetVideoRcdReq_T)-sizeof(VideoRcd_Config_T)+MAX_SHARE_TYPE_LEN-sizeof(INT8U));
			break;
		case AMI_VIDEO_RCD_DOMAIN:
			DomainLen = strlen(video_rcd_config->domain);
			SetVideoRcdConfReq.Config.Progressbit=1;
			ReqLen=(sizeof(AMISetVideoRcdReq_T)-sizeof(VideoRcd_Config_T)+sizeof(SetVideoRcdConfReq.Config.Progressbit));
			wRet=IPMICMD_AMISetVideoRcdConf(pSession,&SetVideoRcdConfReq,&SetVideoRcdConfRes,ReqLen, timeout);
			if((wRet!=CC_SUCCESS)&&((wRet & 0xFF)!=CC_MOUNT_FAILED_CFG_SAVE))
				return wRet;

			for(i=0;(i < MAX_MSG_BLOCKS)&&(((i*MAX_BLOCK_DATA_SIZE)<(unsigned)DomainLen) || (DomainLen == 0) );i++)
			{
				SetVideoRcdConfReq.Blockselector = i+1;
				if(SetVideoRcdConfReq.Blockselector==MAX_MSG_BLOCKS)
					ReqLen=MAX_BLOCK_DATA_SIZE+1;
				else
					ReqLen = MAX_BLOCK_DATA_SIZE+2;
				memset(SetVideoRcdConfReq.Config.domain,0,MAX_BLOCK_DATA_SIZE);
				if(DomainLen > 0)
				{
					memcpy(SetVideoRcdConfReq.Config.domain,video_rcd_config->domain +(i*MAX_BLOCK_DATA_SIZE),MAX_BLOCK_DATA_SIZE);
				}
				wRet = IPMICMD_AMISetVideoRcdConf(pSession, &SetVideoRcdConfReq, &SetVideoRcdConfRes,ReqLen, timeout);
				if((wRet!=CC_SUCCESS)&&((wRet & 0xFF)!=CC_MOUNT_FAILED_CFG_SAVE))
					return wRet;
			}
				SetVideoRcdConfReq.Config.Progressbit=0;
				SetVideoRcdConfReq.Blockselector=0;
				ReqLen=(sizeof(AMISetVideoRcdReq_T)-sizeof(VideoRcd_Config_T)+sizeof(SetVideoRcdConfReq.Config.Progressbit));
			break;
		case AMI_VIDEO_RCD_MNT_SUCCESS://ReadOnly
			continue;
		case AMI_VIDEO_RCD_RST:
			SetVideoRcdConfReq.Config.Progressbit=0;
			ReqLen=(sizeof(AMISetVideoRcdReq_T)-sizeof(VideoRcd_Config_T)+sizeof(SetVideoRcdConfReq.Config.Progressbit));
			break;
		case AMI_CONT_VIDEO_RCD_CONF:
			//Will be used when RecordBootCrash parameter are configurable from WEB UI.
			continue;
		}
		wRet=IPMICMD_AMISetVideoRcdConf(pSession, &SetVideoRcdConfReq, &SetVideoRcdConfRes,ReqLen, timeout);
		if((wRet!=CC_SUCCESS)&&((wRet & 0xFF)!=CC_MOUNT_FAILED_CFG_SAVE))
			return wRet;
	}
	return 0;
}

uint16 IPMICMD_AMIGetRISConf( IPMI20_SESSION_T *pSession,
				AMIGetRISConfReq_T* pAMIGetRISConfReq,
				AMIGetRISConfRes_T* pAMIGetRISConfRes,
				int timeout )
{
	uint16 wRet;
	uint32 wResLen = 0;
	wResLen=0;
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
				((NETFN_AMI << 2) + 0x00), CMD_AMI_GET_RIS_CONF,
				(uint8 *)pAMIGetRISConfReq,sizeof(AMIGetRISConfReq_T),
				(uint8 *)pAMIGetRISConfRes,&wResLen, timeout );

	return wRet;
}

uint16 LIBIPMI_HL_AMIGetRISCfg(IPMI20_SESSION_T *pSession, INT8U MediaType, MediaConfig_T *rmediacfg,int timeout)
{
	AMIGetRISConfReq_T GetRISConfReqBuff;
	AMIGetRISConfRes_T GetRISConfResBuff;
	uint16 wRet = -1;
	int Param=0;

	for(Param=RIS_IMAGE_NAME;Param <= RIS_ERR_CODE;Param++)
	{
		memset(&GetRISConfReqBuff,0,sizeof(AMIGetRISConfReq_T));
		memset(&GetRISConfResBuff,0,sizeof(AMIGetRISConfRes_T));
		GetRISConfReqBuff.ParameterSelect = Param;
		GetRISConfReqBuff.MediaType = MediaType;
		wRet = IPMICMD_AMIGetRISConf( pSession, &GetRISConfReqBuff, &GetRISConfResBuff,timeout );
		if((wRet & 0xFF) == CC_FEATURE_NOT_ENABLED)
			continue;
		if(wRet != 0)
		{
			TCRIT("Error in Getting the Remote Image Domain ::%x\n", wRet);
			return wRet;
		}
		switch(Param)
		{
		case RIS_IMAGE_NAME:
			strncpy(rmediacfg->Image_Name,(char *)GetRISConfResBuff.Config.ImageName,MAX_IMAGE_NAME_LEN-1);
			break;
		case RIS_REMOTE_PATH:
			strncpy(rmediacfg->Image_Path,(char *)GetRISConfResBuff.Config.MountPath,MAX_IMAGE_PATH_SIZE-1);
			break;
		case RIS_IP_ADDR:
			strncpy(rmediacfg->Ip_Addr,(char *)GetRISConfResBuff.Config.RemoteIP,MAX_IP_ADDR_LEN-1);
			break;
		case RIS_USER_NAME:
			strncpy(rmediacfg->Username,(char *)GetRISConfResBuff.Config.UserName,MAX_RMS_USERNAME_LEN-1);
			break;
		case RIS_PASSWORD:
			strncpy(rmediacfg->Password,(char *)GetRISConfResBuff.Config.Password,MAX_RMS_PASSWORD_LEN-1);
			break;
		case RIS_SHARE_TYPE:
			strncpy(rmediacfg->Share_Type,(char *)GetRISConfResBuff.Config.ShareType,MAX_SHARE_TYPE_LEN-1);
			break;
		case RIS_DOMAIN:
			strncpy(rmediacfg->Domain, (char *)GetRISConfResBuff.Config.Domain,MAX_DOMAIN_LEN-1);
			break;
		case RIS_START_MOUNT:
			rmediacfg->Start_Mount = (unsigned int ) GetRISConfResBuff.Config.Start_Mount;
			break;
		case RIS_MOUNT_STATUS:
			rmediacfg->Mount_Successful = (unsigned int ) GetRISConfResBuff.Config.Mount_Status;
			break;
		case RIS_ERR_CODE:
			rmediacfg->Err_Code = (unsigned int ) GetRISConfResBuff.Config.Err_Code;
			break;
		}
	}
	return wRet;
}
uint16 IPMICMD_AMISetRISConf( IPMI20_SESSION_T *pSession, AMISetRISConfReq_T* pAMISetRISConfReq, int timeout )
{
	AMISetRISConfRes_T SetRISConfResBuff = {0};
	uint16 wRet;
	uint32 wReqLen = sizeof(AMISetRISConfReq_T)-sizeof(RISConfig_T);
	uint32 wResLen = 0;
	switch(pAMISetRISConfReq->ParameterSelect)
	{
	case RIS_IMAGE_NAME:
		wReqLen += MAX_BLOCK_DATA_SIZE;
		break;
	case RIS_REMOTE_PATH:
		wReqLen += MAX_BLOCK_DATA_SIZE;
		break;
	case RIS_IP_ADDR:
		wReqLen += MAX_IP_ADDR_LEN;
		break;
	case RIS_USER_NAME:
		wReqLen += MAX_BLOCK_DATA_SIZE;
		break;
	case RIS_PASSWORD:
		wReqLen += MAX_RMS_PASSWORD_LEN;
		break;
	case RIS_SHARE_TYPE:
		wReqLen += MAX_SHARE_TYPE_LEN;
		break;
	case RIS_DOMAIN:
		wReqLen += MAX_BLOCK_DATA_SIZE;
		break;
	case RIS_STATE:
		wReqLen += 1;
		break;
	case RIS_RESTART:
		// Media type IMAGE_TYPE_ALL represents Rmedia
		if(pAMISetRISConfReq->MediaType == IMAGE_TYPE_ALL)
			wReqLen = 2;
		else
			wReqLen = 3;
		break;
	case RIS_CLR_CONFIGS:
		wReqLen = 2;
		break;
	}
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
				((NETFN_AMI << 2) + 0x00), CMD_AMI_SET_RIS_CONF,
				(uint8 *)pAMISetRISConfReq,wReqLen,
				(uint8 *)&SetRISConfResBuff,&wResLen, timeout );
	return wRet;
}

uint16 IPMICMD_AMISetRISProgressBit(IPMI20_SESSION_T *pSession,INT8U Param,INT8U MediaType, INT8U progressbit,int timeout)
{
	uint16 wRet=0;
	AMISetRISConfReq_T SetRISConfReqBuff = {0,0,0,{"0"}};
	AMISetRISConfRes_T SetRISConfResBuff = {0};
	uint32 wReqLen = sizeof(AMISetRISConfReq_T)-sizeof(RISConfig_T) + 1;
	uint32 wResLen = 0;
	SetRISConfReqBuff.ParameterSelect = Param;
	SetRISConfReqBuff.MediaType = MediaType;
	SetRISConfReqBuff.Blockselector = 0x00;
	SetRISConfReqBuff.config.ProgressBit = progressbit;
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
				((NETFN_AMI << 2) + 0x00), CMD_AMI_SET_RIS_CONF,
				(uint8 *)&SetRISConfReqBuff,wReqLen,
				(uint8 *)&SetRISConfResBuff,&wResLen, timeout);

	if(wRet != CC_SUCCESS)
	{
		TCRIT("Error in setting the Field ::%x Progressbit ::%x\n",Param,wRet);
		return wRet;
	}
	return wRet;
}

uint16 LIBIPMI_HL_AMISetRIS_State(IPMI20_SESSION_T *pSession, INT8U MediaType, INT8U State,int timeout)
{
	AMISetRISConfReq_T SetRISConfReqBuff = {0,0,0,{"0"}};
	uint16 wRet = 0;

	SetRISConfReqBuff.ParameterSelect = RIS_STATE;
	SetRISConfReqBuff.MediaType = MediaType;
	SetRISConfReqBuff.Blockselector = 0x00;
	SetRISConfReqBuff.config.RISstate = State;
	wRet = IPMICMD_AMISetRISConf( pSession, &SetRISConfReqBuff,timeout );
	if(wRet != CC_SUCCESS)
	{
		TCRIT("Error in Setting the RIS_State::%x\n",wRet);
		return wRet;
	}
	return wRet;
}
uint16 LIBIPMI_HL_AMISetRISImageName(IPMI20_SESSION_T *pSession, INT8U MediaType, INT8U *Image_Name,int timeout)
{
	AMISetRISConfReq_T SetRISConfReqBuff={0,0,0,{"0"}};
	AMISetRISConfRes_T SetRISConfResBuff;
	uint16 wRet = 0;
	int i = 0;
	memset(&SetRISConfReqBuff,0,sizeof(AMISetRISConfReq_T));
	memset(&SetRISConfResBuff,0,sizeof(AMISetRISConfRes_T));
	SetRISConfReqBuff.ParameterSelect = RIS_IMAGE_NAME;
	SetRISConfReqBuff.MediaType = MediaType;
	wRet = IPMICMD_AMISetRISProgressBit(pSession,RIS_IMAGE_NAME,MediaType, RIS_PARAM_SET,timeout);
	if (wRet != CC_SUCCESS)
		return wRet;
	for(i=0;(i < MAX_MSG_BLOCKS)&&((i*MAX_BLOCK_DATA_SIZE)<(signed)strlen((char *)Image_Name));i++)
	{
		SetRISConfReqBuff.Blockselector = i+1;
		memset(SetRISConfReqBuff.config.MountPath,0,MAX_BLOCK_DATA_SIZE);
		memcpy(SetRISConfReqBuff.config.ImageName,&Image_Name[i*MAX_BLOCK_DATA_SIZE],MAX_BLOCK_DATA_SIZE);
		wRet = IPMICMD_AMISetRISConf(pSession, &SetRISConfReqBuff,timeout );
		if(wRet != CC_SUCCESS)
		{
			TCRIT("Error in Setting the RIS Image Name :: %x\n",wRet);
			return wRet;
		}
	}
	wRet = IPMICMD_AMISetRISProgressBit(pSession,RIS_IMAGE_NAME,MediaType, RIS_PARAM_RESET,timeout);
	if(wRet != CC_SUCCESS)
	{
		TCRIT("Error in Setting the RIS Image name :: %x\n",wRet);
		return wRet;
	}
	return wRet;
}
uint16 LIBIPMI_HL_AMISetRISClear(IPMI20_SESSION_T *pSession, INT8U MediaType,  int timeout)
{
	AMISetRISConfReq_T SetRISConfReqBuff={0,0,0,{"0"}};
	AMISetRISConfRes_T SetRISConfResBuff;
	uint16 wRet = 0;
	memset(&SetRISConfReqBuff,0,sizeof(AMISetRISConfReq_T));
	memset(&SetRISConfResBuff,0,sizeof(AMISetRISConfRes_T));
	SetRISConfReqBuff.ParameterSelect = RIS_CLR_CONFIGS;
	SetRISConfReqBuff.MediaType = MediaType;
	wRet = IPMICMD_AMISetRISConf(pSession, &SetRISConfReqBuff,timeout );
	if(wRet != CC_SUCCESS)
	{
		TCRIT("Error in Setting the RIS User Name:: %x\n",wRet);
		return wRet;
	}
	return wRet;
}
uint16 LIBIPMI_HL_AMISetRISCfg(IPMI20_SESSION_T *pSession, INT8U MediaType, MediaConfig_T *rmediacfg, INT8U Restart, int timeout)
{
	AMISetRISConfReq_T SetRISConfReqBuff;
	AMISetRISConfRes_T SetRISConfResBuff;
	uint16 wRet = 0;
	int Param = 0;
	unsigned int i=0;
	unsigned DomainLen = 0;

	for(Param = RIS_REMOTE_PATH;Param <= RIS_DOMAIN;Param++)
	{
		if(strcmp(rmediacfg->Share_Type, "nfs") == 0)
		{
			if((Param != RIS_REMOTE_PATH) &&
				(Param != RIS_IP_ADDR) &&
				(Param != RIS_SHARE_TYPE) )
					continue;
		}

		memset(&SetRISConfReqBuff,0,sizeof(AMISetRISConfReq_T));
		memset(&SetRISConfResBuff,0,sizeof(AMISetRISConfRes_T));
		SetRISConfReqBuff.ParameterSelect = Param;
		SetRISConfReqBuff.MediaType = MediaType;
		switch(Param)
		{
		case RIS_REMOTE_PATH:
			wRet = IPMICMD_AMISetRISProgressBit(pSession,RIS_REMOTE_PATH ,MediaType, RIS_PARAM_SET,timeout);
			if(wRet != CC_SUCCESS)
			{
				TCRIT("Error in Setting the Progressbit::%x  in Param = %d\n",wRet,Param);
				return wRet;
			}
			for(i=0;(i < MAX_MSG_BLOCKS)&&((i*MAX_BLOCK_DATA_SIZE)<strlen(rmediacfg->Image_Path));i++)
			{
				SetRISConfReqBuff.Blockselector = i+1;
				memset(SetRISConfReqBuff.config.MountPath,0,MAX_BLOCK_DATA_SIZE);
				memcpy(SetRISConfReqBuff.config.MountPath,&rmediacfg->Image_Path[i*MAX_BLOCK_DATA_SIZE],MAX_BLOCK_DATA_SIZE);
				wRet = IPMICMD_AMISetRISConf(pSession, &SetRISConfReqBuff,timeout );
				if(wRet != CC_SUCCESS)
				{
					TCRIT("Error in Setting the RIS Image PATH :: %x\n",wRet);
					return wRet;
				}
			}
			wRet = IPMICMD_AMISetRISProgressBit(pSession,RIS_REMOTE_PATH ,MediaType, RIS_PARAM_RESET,timeout);
			if(wRet != CC_SUCCESS)
			{
				TCRIT("Error in ReSetting the Progressbit::%x\n",wRet);
				return wRet;
			}
			else
				continue;
		case RIS_IP_ADDR:
			strncpy(SetRISConfReqBuff.config.RemoteIP,rmediacfg->Ip_Addr,MAX_IP_ADDR_LEN);
			break;
		case RIS_USER_NAME:
			if(strcmp(rmediacfg->Share_Type, "nfs") == 0)
				continue;
			wRet = IPMICMD_AMISetRISProgressBit(pSession,RIS_USER_NAME ,MediaType, RIS_PARAM_SET,timeout);
			if(wRet != CC_SUCCESS)
			{
				TCRIT("Error in Setting the Progressbit::%x\n",wRet);
				return wRet;
			}
			for(i=0;(i < MAX_MSG_BLOCKS)&&((i*MAX_BLOCK_DATA_SIZE)<strlen(rmediacfg->Username));i++)
			{
				SetRISConfReqBuff.Blockselector = i+1;
				memset(SetRISConfReqBuff.config.UserName,0,MAX_BLOCK_DATA_SIZE);
				memcpy(SetRISConfReqBuff.config.UserName,&rmediacfg->Username[i*MAX_BLOCK_DATA_SIZE],MAX_BLOCK_DATA_SIZE);
				wRet = IPMICMD_AMISetRISConf(pSession, &SetRISConfReqBuff,timeout );
				if(wRet != CC_SUCCESS)
				{
					TCRIT("Error in Setting the RIS User Name:: %x\n",wRet);
					return wRet;
				}
			}
			wRet = IPMICMD_AMISetRISProgressBit(pSession,RIS_USER_NAME ,MediaType, RIS_PARAM_RESET,timeout);
			if(wRet != CC_SUCCESS)
			{
				TCRIT("Error in ReSetting the Progressbit::%x\n",wRet);
				return wRet;
			}
			else
				continue;
		case RIS_PASSWORD:
			if(strcmp(rmediacfg->Share_Type, "nfs") == 0)
				continue;
			else if(strcmp(rmediacfg->Share_Type, "nfs") != 0 && Restart==0)
				continue;
			strncpy(SetRISConfReqBuff.config.Password,rmediacfg->Password,MAX_RMS_PASSWORD_LEN);
			break;
		case RIS_SHARE_TYPE:
			strncpy(SetRISConfReqBuff.config.ShareType,rmediacfg->Share_Type,MAX_SHARE_TYPE_LEN);
			break;
		case RIS_DOMAIN:
			if(strcmp(rmediacfg->Share_Type, "nfs") == 0)
				continue;
			wRet = IPMICMD_AMISetRISProgressBit(pSession,RIS_DOMAIN ,MediaType, RIS_PARAM_SET,timeout);
			if(wRet != CC_SUCCESS)
			{
				TCRIT("Error in Setting the Progressbit::%x\n",wRet);
				return wRet;
			}

			DomainLen = strlen(rmediacfg->Domain);
			for(i=0;(i < MAX_MSG_BLOCKS)&&(((i*MAX_BLOCK_DATA_SIZE)<DomainLen) || (DomainLen == 0) );i++)
			{
				SetRISConfReqBuff.Blockselector = i+1;
				memset(SetRISConfReqBuff.config.Domain,0,MAX_BLOCK_DATA_SIZE);
				if(DomainLen > 0)
				{
					memcpy(SetRISConfReqBuff.config.Domain,&rmediacfg->Domain[i*MAX_BLOCK_DATA_SIZE],MAX_BLOCK_DATA_SIZE);
				}
				wRet = IPMICMD_AMISetRISConf(pSession, &SetRISConfReqBuff,timeout );
				if(wRet != CC_SUCCESS)
				{
					TCRIT("Error in Setting the RIS User Name:: %x\n",wRet);
					return wRet;
				}
			}
			wRet = IPMICMD_AMISetRISProgressBit(pSession,RIS_DOMAIN ,MediaType, RIS_PARAM_RESET,timeout);
			if(wRet != CC_SUCCESS)
			{
				TCRIT("Error in ReSetting the Progressbit::%x\n",wRet);
				return wRet;
			}
			else
				continue;
		}
		wRet = IPMICMD_AMISetRISConf( pSession, &SetRISConfReqBuff,timeout );
		if(wRet != 0)
		{
			TCRIT("Error in Setting the Remote Image Service Configurations ::%x\n", wRet);
			return wRet;
		}
	}
	// Media type IMAGE_TYPE_ALL represents Rmedia
	if( ( (Restart == 1) && (MediaType == IMAGE_TYPE_ALL)) || (MediaType != IMAGE_TYPE_ALL)){
		memset(&SetRISConfReqBuff,0,sizeof(AMISetRISConfReq_T));
		memset(&SetRISConfResBuff,0,sizeof(AMISetRISConfRes_T));
		SetRISConfReqBuff.MediaType = MediaType;
		SetRISConfReqBuff.ParameterSelect = RIS_RESTART;
		if (MediaType != IMAGE_TYPE_ALL)
			SetRISConfReqBuff.Blockselector = Restart;
		wRet = IPMICMD_AMISetRISConf(pSession, &SetRISConfReqBuff,timeout );
		if(wRet != CC_SUCCESS)
		{
			TCRIT("Error in restarting RMedia Service:: %x\n",wRet);
			return wRet;
		}
	}
	return wRet;
}

uint16 LIBIPMI_HL_AMIRISStartandStop(IPMI20_SESSION_T *pSession,INT8U imagetype, INT8U Status,int timeout)
{
	AMIRISStartStopReq_T RISStartStopReqBuff = {0,0};
	AMIRISStartStopRes_T RISStartStopResBuff = {0};
	uint32 wReqLen = sizeof(AMIRISStartStopReq_T);
	uint32 wResLen = 0;
	uint16 wRet;
	RISStartStopReqBuff.MediaType = imagetype;
	RISStartStopReqBuff.ParameterSelect = Status;
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
						((NETFN_AMI << 2) + 0x00), CMD_AMI_RIS_START_STOP,
						(uint8 *)&RISStartStopReqBuff,wReqLen,
						(uint8 *)&RISStartStopResBuff,&wResLen, timeout );
	if(wRet != CC_SUCCESS)
	{
		TCRIT("Error in Setting the Start and Stop of the Remote Image Service::%x\n",wRet);
		return wRet;
	}
	return wRet;
}

uint16 IPMICMD_AMIGetSinglePortFeatureStatus( IPMI20_SESSION_T *pSession,
		AMIGetRunTimeSinglePortStatusRes_T *pAMIGetRunTimeSinglePortStatusRes, int timeout )
{
	uint16 wRet;
	uint32 wResLen = sizeof(AMIGetRunTimeSinglePortStatusRes_T);
	uint8 *pReq = NULL;

	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
					((NETFN_AMI << 2) + 0x00), CMD_AMI_GET_RUN_TIME_SINGLE_PORT_STATUS,
					(uint8 *)pReq,0,
					(uint8 *)pAMIGetRunTimeSinglePortStatusRes,&wResLen, timeout );
	return( wRet );
}

uint16 LIBIPMI_HL_AMIGetSinglePortFeatureStatus(IPMI20_SESSION_T *pSession,INT8U *Status,int timeout)
{
	uint16 wRet = CC_NORMAL;
	AMIGetRunTimeSinglePortStatusRes_T GetRunTimeSinglePortStatusRes;
	memset(&GetRunTimeSinglePortStatusRes, 0, sizeof (AMIGetRunTimeSinglePortStatusRes_T));
	wRet = IPMICMD_AMIGetSinglePortFeatureStatus(pSession,&GetRunTimeSinglePortStatusRes,timeout);
	*Status = GetRunTimeSinglePortStatusRes.RuntimeSinglePort;
	return wRet;
}

uint16 IPMICMD_AMISetSinglePortFeatureStatus( IPMI20_SESSION_T *pSession,
		AMISetRunTimeSinglePortStatusReq_T* pAMISetRunTimeSinglePortStatusReq, int timeout )
{
	uint16 wRet;
	uint8 Res = 0x00;
	uint32 wResLen = 0;
	uint32 wReqLen = sizeof(AMISetRunTimeSinglePortStatusReq_T);

	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
					((NETFN_AMI << 2) + 0x00), CMD_AMI_SET_RUN_TIME_SINGLE_PORT_STATUS,
					(uint8 *)pAMISetRunTimeSinglePortStatusReq,wReqLen,
					(uint8 *)&Res,&wResLen, timeout );
return( wRet );
}

uint16 LIBIPMI_HL_AMISetSinglePortFeatureStatus(IPMI20_SESSION_T *pSession,INT8U Command,int timeout)
{
	uint16 wRet = CC_NORMAL;
	AMISetRunTimeSinglePortStatusReq_T AMISetRunTimeSinglePortStatusReq;
	memset(&AMISetRunTimeSinglePortStatusReq, 0, sizeof(AMISetRunTimeSinglePortStatusReq_T));

	AMISetRunTimeSinglePortStatusReq.RuntimeSinglePort = Command;
	wRet = IPMICMD_AMISetSinglePortFeatureStatus(pSession,&AMISetRunTimeSinglePortStatusReq,timeout);
	return wRet;
}


uint16  IPMICMD_AMIGetFeatureStatus(IPMI20_SESSION_T *pSession,	INT8U *FeatureName, INT8U *FeatureStatus,int timeout)
{
    uint16 wRet = CC_NORMAL;
    uint32 wResLen = sizeof(AMIGetFeatureStatusRes_T);
    AMIGetFeatureStatusRes_T  GetFeatureStatusRes;
    INT8U ReqBuf[MAX_CORE_FEATURE_NAME_LEN+1]="\0";
    int tempret=0;/*To check return value of snprintf, it is not used any where else*/

    tempret=snprintf((char *)ReqBuf,sizeof(ReqBuf),"%s",(char *)FeatureName);
    if((tempret<0)||(tempret>=(MAX_CORE_FEATURE_NAME_LEN+1)))
    {
        TCRIT("Buffer Overflow");
        return -1;
    }

    memset(&GetFeatureStatusRes, 0x0, sizeof(AMIGetFeatureStatusRes_T));

    if(0xFF == pSession->g_NetFnOEM)
    {
        NetFn_OEM = NETFN_AMI;
    }
    else
    {
        NetFn_OEM = pSession->g_NetFnOEM;
    }

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                             ((NetFn_OEM << 2) + 0x00), CMD_AMI_GET_FEATURE_STATUS,
                                             ReqBuf,strlen((char *)ReqBuf),
                                             (uint8 *)&GetFeatureStatusRes,&wResLen, timeout );
    if(0 == wRet)
    {
        *FeatureStatus = GetFeatureStatusRes.FeatureStatus;
    }

return wRet;
}

uint16 LIBIPMI_HL_AMIGetFeatureStatus(IPMI20_SESSION_T *pSession, INT8U *FeatureName, INT8U *FeatureStatus,int timeout)
{
    uint16 wRet = CC_NORMAL;
    wRet = IPMICMD_AMIGetFeatureStatus(pSession,FeatureName,FeatureStatus,timeout);

return wRet;
}

uint16 IPMICMD_AMIGetExtendedLogConf(IPMI20_SESSION_T *pSession,AMIGetEXTtLogConfRes_T *pGetExtLogConfRes,INT8U ReqLen,int timeout)
{
    uint16 wRet;
    uint32 dwResLen;

    dwResLen = sizeof(AMIGetEXTtLogConfRes_T);

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                                                                                   ( ( NETFN_AMI << 2 ) + 0x00 ),CMD_AMI_GET_EXTLOG_CONF,
                                                                                   (uint8 *)NULL,ReqLen,
                                                                                   (uint8 *)pGetExtLogConfRes, &dwResLen, timeout);
    return( wRet);
}

uint16 IPMICMD_AMIFileUpload(IPMI20_SESSION_T *pSession,INT8U *pReq,uint32 ReqLen,INT8U *TransID,int timeout)
{
    uint16 wRet = CC_NORMAL;
    uint32 wResLen = sizeof(AMIFileUploadRes_T);
    AMIFileUploadRes_T  AMIFileUploadRes;

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession,PAYLOAD_TYPE_IPMI,((NETFN_AMI << 2) + 0x00),
                                                CMD_AMI_FILE_UPLOAD,pReq,ReqLen,(INT8U *)&AMIFileUploadRes,&wResLen,timeout);
    if(0 != wRet)
    {
        printf("Error while uploading the File packet %x\n",wRet);
    }

    *TransID = AMIFileUploadRes.TransID;

    return wRet;
}

uint16 IPMICMD_AMIFileDownload(IPMI20_SESSION_T *pSession,INT8U *pReq,uint32 ReqLen, INT8U *pRes, uint32 *ResLen,int timeout)
{
    uint16 wRet = CC_NORMAL;

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession,PAYLOAD_TYPE_IPMI,((NETFN_AMI << 2) + 0x00),
                                                CMD_AMI_FILE_DOWNLOAD,pReq,ReqLen,pRes,ResLen,timeout);
    if(0 != wRet)
    {
        printf("Error while uploading the File packet %x\n",wRet);
    }

    return wRet;
}


uint16 LIBIPMI_HL_AMIGetSELRecords (IPMI20_SESSION_T *pSession,
			    SELRec_T *pSELRecData,
			    uint32  *nNumSelEntries,
			    int timeout)
{
    AMIGetSELEntriesReq_T pAMIGetSelEntriesReq = {0};
    AMIGetSELEntriesRes_T *pAMIGetSelEntiresRes = NULL;
    uint8 *pSELptr = NULL;
    uint8 *pSELEntries = NULL, *pSELData = NULL;
    uint16 wRet = 0;

    if(!pSELRecData)
    {
        wRet = 0xFF;
        goto EXIT;
    }

    pSELEntries = (uint8 *)calloc(MAX_SEL_RETRIEVAL_SIZE, sizeof(uint8));
    if(pSELEntries == NULL)
    {
        TCRIT("Memory allocation error in LIBIPMI_HL_AMIGetSELEntires \n");
        wRet = 0xFF;
        goto EXIT;
    }


    pSELptr = (uint8*)pSELRecData;
    pAMIGetSelEntiresRes = (AMIGetSELEntriesRes_T *)pSELEntries;
    do
	{
		wRet = IPMICMD_AMIGetSELEntires(pSession,&pAMIGetSelEntriesReq,(AMIGetSELEntriesRes_T *)pSELEntries,timeout);
		pSELData = (uint8*)(pSELEntries + sizeof(AMIGetSELEntriesRes_T));
		if(wRet == LIBIPMI_E_SUCCESS)
		{
			memcpy(pSELptr, pSELData, (pAMIGetSelEntiresRes->Noofentries * sizeof(SELRec_T)));
			pSELptr = pSELptr + (pAMIGetSelEntiresRes->Noofentries * sizeof(SELRec_T));


			if(pAMIGetSelEntiresRes->Status == FULL_SEL_ENTRIES)
			{
				*nNumSelEntries += pAMIGetSelEntiresRes->Noofentries;
				break;
			}
			else
			{
				*nNumSelEntries += pAMIGetSelEntiresRes->Noofentries;
				pAMIGetSelEntriesReq.Noofentretrieved = *nNumSelEntries;
				memset(pSELEntries, 0, MAX_SEL_RETRIEVAL_SIZE);
			}
		}
		else if((wRet & 0xFF) == LIBIPMI_E_SEL_CLEARED)
		{
			memset(pSELEntries, 0, MAX_SEL_RETRIEVAL_SIZE);
			memset(pSELRecData, 0, sizeof(SELRec_T)* *nNumSelEntries);
			*nNumSelEntries =  pAMIGetSelEntriesReq.Noofentretrieved = 0;
			goto EXIT;
		}
		else if((wRet & 0xFF)  == LIBIPMI_E_SEL_EMPTY)
		{
			*nNumSelEntries = 0;
			wRet = LIBIPMI_E_SUCCESS;
			goto EXIT;
		}
		else
		{
			break;
		}
	}while(1);

EXIT:
    if(pSELEntries)
    {
    	 free(pSELEntries);
    	 pSELEntries = NULL;
    }
    return wRet;
}
uint16 LIBIPMI_HL_AMIRestartWebService(IPMI20_SESSION_T *pSession,INT8U *Status, int timeout)
{
	uint16 wRet = CC_NORMAL;
	AMIRestartWebServiceRes_T RestartWebServiceRes;
	memset(&RestartWebServiceRes, 0, sizeof (AMIRestartWebServiceRes_T));

	wRet = IPMICMD_AMIRestartWebService(pSession,&RestartWebServiceRes,timeout);
	*Status = RestartWebServiceRes.CompletionCode;

	return wRet;
}
uint16 IPMICMD_AMIRestartWebService(IPMI20_SESSION_T *pSession,
				AMIRestartWebServiceRes_T *pRestartWebServiceRes, int timeout )
{
	uint16 wRet;
	uint32 wResLen = sizeof(AMIRestartWebServiceRes_T);
	uint8 *pReq = NULL;

	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
				((NETFN_AMI << 2) + 0x00), CMD_AMI_RESTART_WEB_SERVICE,
				(uint8 *)pReq,0 ,
				(uint8 *)pRestartWebServiceRes, &wResLen, timeout );
	return wRet;
}

uint16 IPMICMD_AMIFirmwareUpdate(IPMI20_SESSION_T *pSession,
                        INT8U *pReq, uint32 ReqLen, INT8U *pRes, uint32 *ResLen, int timeout )
{
    uint16 wRet;

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
            ((NETFN_AMI << 2) + 0x00), CMD_AMI_FIRMWAREUPDATE,
            pReq,ReqLen ,
            pRes, ResLen, timeout );
    return wRet;
}

uint16 IPMICMD_AMIEnterFirmwareUpdateMode(IPMI20_SESSION_T *pSession,INT8U FwMode,INT8U *pHandle,
                        int timeout)
{
	uint16 wRet;
	INT8U pReq[2] = {0},pRes[6];
	uint32 ReqLen =2,ResLen=5;
	pReq[0] = IPMI_AMI_SET_UPDATE_MODE;
	pReq[1] = FwMode;

	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
             ((NETFN_AMI << 2) + 0x00), CMD_AMI_FIRMWAREUPDATE,
             pReq,ReqLen ,
             pRes,&ResLen, timeout );
	memcpy(pHandle,&pRes[1],4);
	return wRet;
}

uint16 IPMICMD_AMIExitFwUpdateMode(IPMI20_SESSION_T *pSession,INT8U FwMode,INT8U *pHandle,
                        int timeout)
{
	uint16 wRet;
	INT8U pReq[6] = {0},pRes[10];
	uint32 ReqLen =6,ResLen=sizeof(INT8U);
	pReq[0] = IPMI_AMI_SET_UPDATE_MODE;
	pReq[1] = FwMode;

	memcpy(&pReq[2],pHandle,4);

	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
             ((NETFN_AMI << 2) + 0x00), CMD_AMI_FIRMWAREUPDATE,
             pReq,ReqLen ,
             pRes,&ResLen, timeout );
	return wRet;
}

uint16 LIBIPMI_HL_AMIGetAllImageInfo(IPMI20_SESSION_T *pSession, GetAllImageInfo_T *GetImageInfoReq, uint32 *ResLen,int timeout)
{
    uint16 wRet;
    INT8U Req[2],pRes[MSG_PAYLOAD_SIZE] = {0};
    int nRet=2,i;

	Req[0] = IPMI_AMI_GET_IMAGES_INFO;
    Req[1] = 0;

    wRet = IPMICMD_AMIFirmwareUpdate(pSession, Req, sizeof(Req),pRes, ResLen, timeout);
    if(wRet != 0)
    {
        TCRIT("Error while getting the all images info:%d\n",wRet);
		return wRet;
    }
    GetImageInfoReq->ImageCnt = pRes[1];

    for(i=0;i< GetImageInfoReq->ImageCnt ;i++)
    {
    	GetImageInfoReq->ImageInfo[i].DevType= pRes[nRet];
        nRet++;
        GetImageInfoReq->ImageInfo[i].Slot= pRes[nRet];
        nRet++;
        memcpy(&GetImageInfoReq->ImageInfo[i].Identifier,&pRes[nRet],sizeof(GetImageInfoReq->ImageInfo[i].Identifier));
        nRet +=sizeof(GetImageInfoReq->ImageInfo[i].Identifier);
        GetImageInfoReq->ImageInfo[i].DevStatus= pRes[nRet];
        nRet++;
        GetImageInfoReq->ImageInfo[i].OpStatus= pRes[nRet];
		nRet++;
        memcpy( &GetImageInfoReq->ImageInfo[i].BundleVer,&pRes[nRet],sizeof(GetImageInfoReq->ImageInfo[i].BundleVer));
        nRet += sizeof(GetImageInfoReq->ImageInfo[i].BundleVer);
        memcpy(&GetImageInfoReq->ImageInfo[i].CurrentVer,&pRes[nRet],sizeof(GetImageInfoReq->ImageInfo[i].CurrentVer));
		nRet += sizeof(GetImageInfoReq->ImageInfo[i].CurrentVer);
    }
     return wRet;
}

uint16 LIBIPMI_HL_AMISetUpdateComponent(IPMI20_SESSION_T *pSession, INT8S *ReqData, int ReqDataLen, int timeout)
{
    uint16 wRet;
    AMIFWCommandReq_T UpdateCompReq;
    INT8U Res[2] = {0};
    uint32 ResLen;
    memset(&(UpdateCompReq),0,sizeof(AMIFWCommandReq_T));
    UpdateCompReq.FWSubCommand = IPMI_AMI_SET_UPDATE_COMPONENT;



    memcpy(UpdateCompReq.FWCommandData, (char *)ReqData , 256);

    wRet = IPMICMD_AMIFirmwareUpdate(pSession, (INT8U *)&UpdateCompReq, ReqDataLen, Res, &ResLen, timeout);
    if(wRet != 0)
    {
        TCRIT("Error while Set Update Component ::%d\n",wRet);
        return wRet;
    }
    return wRet;
}


uint16 LIBIPMI_HL_AMIGetComponentUpdateStatus(IPMI20_SESSION_T *pSession,GetAllUpdateStatusInfo_T *UpdateAllCmpStatus, uint32 *ResLen, int timeout)
{
	uint16 wRet;
	INT8U Req[2],pRes[MSG_PAYLOAD_SIZE];
	int i,nRet=2;

	Req[0] = IPMI_AMI_GET_UPDATE_COMP_STATUS;
    Req[1] = 0;

    wRet = IPMICMD_AMIFirmwareUpdate(pSession, Req, sizeof(Req), pRes, ResLen, timeout);
    if(wRet != 0)
    {
        TCRIT("Error while Get Component Update Status::%d\n",wRet);
        return wRet;
    }
    UpdateAllCmpStatus->UpdateCnt = pRes[1];

    for(i=0;i< UpdateAllCmpStatus->UpdateCnt;i++)
    {
    	UpdateAllCmpStatus->UpdateStatus[i].DevType = pRes[nRet];
    	nRet++;
    	UpdateAllCmpStatus->UpdateStatus[i].Slot = pRes[nRet];
    	nRet++;
    	memcpy(&UpdateAllCmpStatus->UpdateStatus[i].Identifier,&pRes[nRet],sizeof(UpdateAllCmpStatus->UpdateStatus[i].Identifier));
    	nRet +=sizeof(UpdateAllCmpStatus->UpdateStatus[i].Identifier);
    	UpdateAllCmpStatus->UpdateStatus[i].Status= pRes[nRet];
    	nRet++;
    	memcpy( &UpdateAllCmpStatus->UpdateStatus[i].Timestamp,&pRes[nRet],sizeof(UpdateAllCmpStatus->UpdateStatus[i].Timestamp));
    	nRet += sizeof(UpdateAllCmpStatus->UpdateStatus[i].Timestamp);
    	memcpy(&UpdateAllCmpStatus->UpdateStatus[i].UTCOffset,&pRes[nRet],sizeof(UpdateAllCmpStatus->UpdateStatus[i].UTCOffset));
    	nRet += sizeof(UpdateAllCmpStatus->UpdateStatus[i].UTCOffset);
    	memcpy(&UpdateAllCmpStatus->UpdateStatus[i].Version,&pRes[nRet],sizeof(UpdateAllCmpStatus->UpdateStatus[i].Version));
    	nRet += sizeof(UpdateAllCmpStatus->UpdateStatus[i].Version) ;
    	memcpy(&UpdateAllCmpStatus->UpdateStatus[i].UpdatePercentage,&pRes[nRet],sizeof(UpdateAllCmpStatus->UpdateStatus[i].UpdatePercentage));
    	nRet++;
    }
    return wRet;
}

uint16 IPMICMD_AMIGetRemoteImageInfo(IPMI20_SESSION_T *pSession,FWRemoteImg_T *FWRemoteImg,int timeout)
{
	uint16 wRet=0;
	INT8U pReq,pRes[MSG_PAYLOAD_SIZE]={0};
	uint32 ReqLen =1,ResLen=0;
	int Shrlength=0,offset=0,FleNameLth=0;
	pReq = IPMI_AMI_GET_NETWORK_SHARE_CONFIG;

	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
		            ((NETFN_AMI << 2) + 0x00), CMD_AMI_FIRMWAREUPDATE,
		            &pReq,ReqLen ,
		            pRes,&ResLen, timeout );

	FWRemoteImg->ShareType = pRes[1];
	strncpy((char *)FWRemoteImg->UserName,(char *)pRes+2,15);
	//strncpy((char *)FWRemoteImg->Pswd,(char *)pRes+18,16);
	Shrlength = pRes[18];
	strncpy((char *)FWRemoteImg->ShareName,(char *)pRes+19,(sizeof(FWRemoteImg->ShareName)-1));
	offset = 18+Shrlength+1;
	FleNameLth = pRes[offset];
	strncpy((char *)FWRemoteImg->FileName,(char *)pRes+offset+1,(sizeof(FWRemoteImg->FileName)-1));
	offset= offset+FleNameLth+1;

	if( sizeof(FWRemoteImg->IpAddr) <= (unsigned) snprintf((char *)FWRemoteImg->IpAddr , sizeof(FWRemoteImg->IpAddr), "%d.%d.%d.%d",pRes[offset],pRes[offset+1],pRes[offset+2],pRes[offset+3]))
		TCRIT("IPMICMD_AMIGetRemoteImageInfo - Source buffer is truncated.");

	return wRet;
}

uint16 LIBIPMI_HL_AMISetRemoteImageInfo(IPMI20_SESSION_T *pSession, FWRemoteImg_T *RemoteImgInfo,int timeout)
{
    uint16 wRet;
    INT8U Req[257] ,Res[2] = {0};
    uint32 ResLen,ReqLen;
    int nRet=0,Shrlen=0,Flen=0;

    Req[nRet] = IPMI_AMI_SET_NETWORK_SHARE_CONFIG;
    nRet++;
    Req[nRet] =  RemoteImgInfo->ShareType;
    nRet++;
    memcpy(&(Req[nRet]),&RemoteImgInfo->UserName,sizeof(RemoteImgInfo->UserName));
    nRet+=sizeof(RemoteImgInfo->UserName);
    memcpy(&Req[nRet],&RemoteImgInfo->Pswd,sizeof(RemoteImgInfo->Pswd));
    nRet+=sizeof(RemoteImgInfo->Pswd);
    Shrlen = Req[nRet]= strlen((char *)RemoteImgInfo->ShareName);
    nRet++;
    memcpy(&Req[nRet],&RemoteImgInfo->ShareName,Shrlen);
    nRet+=Shrlen;
    Flen = Req[nRet]= strlen((char *)RemoteImgInfo->FileName);
    nRet++;
    memcpy(&Req[nRet],&RemoteImgInfo->FileName,Flen);
    nRet+= Flen;
	memcpy(&Req[nRet],&RemoteImgInfo->IpAddr,4);

	ReqLen = 1+1+32+2+Shrlen+Flen+4;

	wRet = IPMICMD_AMIFirmwareUpdate(pSession, Req, ReqLen, Res,&ResLen, timeout);
    if(wRet != 0)
    {
        TCRIT("Error while Set Remote Image Information:: %d\n",wRet);
        return wRet;
    }
    return wRet;
}


uint16 IPMICMD_AMIValidateBundle(IPMI20_SESSION_T *pSession,INT8U *BundleName, INT8U *ValidateStatus,
                        int timeout)
{
	uint16 wRet;
	INT8U pReq[32],pRes[2];
	uint32 ResLen=1 ,ReqLen =17;
	pReq[0] = IPMI_AMI_GET_VALIDATE_BUNDLE;

	memcpy(pReq+1, (char *)BundleName, 16);

	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
	            ((NETFN_AMI << 2) + 0x00), CMD_AMI_FIRMWAREUPDATE,
	            pReq, ReqLen,
	            pRes,&ResLen, timeout );
	ValidateStatus = (pRes+1);
	if(0)
	{
		ValidateStatus=ValidateStatus;/*-Wextra: parameter 'ValidateStatus' set but not used*/
	}

	return wRet;
}


uint16 IPMICMD_AMIGetComponentName(IPMI20_SESSION_T *pSession,INT8U DevID,INT8U *DeviceName,int *Len,
                        int timeout)
{
	uint16 wRet;
	INT8U pReq[2] = {0},pRes[20];
	uint32 ReqLen =2,ResLen=0;
	pReq[0] = IPMI_AMI_GET_COMPONENT_NAME;
	pReq[1] = DevID;

	memset(pRes,0,sizeof(pRes));

	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
             ((NETFN_AMI << 2) + 0x00), CMD_AMI_FIRMWAREUPDATE,
             pReq,ReqLen ,
             pRes,&ResLen, timeout );

	*Len = (int )ResLen -1;
	if(*Len < (signed)sizeof(pRes) && *Len > 0)
	{
	    memcpy(DeviceName,&pRes[1],*Len);
	}
	else
	{
	    TCRIT("Buffer Overflow\n");
	    return -1;
	}

	return wRet;
}

uint16 IPMICMD_AMIDiscardAllPendingUpdate(IPMI20_SESSION_T *pSession,INT8U *CancelStatus,
                        int timeout)
{
	uint16 wRet;
	INT8U pReq,pRes[2];
	uint32 ResLen,ReqLen =1;
	pReq = IPMI_AMI_SET_CANCEL_COMPONENT_UPDATE;

	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
	            ((NETFN_AMI << 2) + 0x00), CMD_AMI_FIRMWAREUPDATE,
	            &pReq,ReqLen ,
	            pRes,&ResLen, timeout );
	CancelStatus = (pRes+1);
	if(0)
	{
		CancelStatus=CancelStatus;  /*-Wextra: parameter 'CancelStatus' set but not used*/
	}
	return wRet;
}

uint16 IPMICMD_AMISetNetworkShareOperation(IPMI20_SESSION_T *pSession,INT8U NetworkOperation,INT8U *Status,int timeout)
{
	uint16 wRet = 0;
	INT8U pReq[2] = {0};
	INT8U pRes_Status[2]={0};
	INT8U pRes[1]={0};
	uint32 ReqLen =2,ResLen=sizeof(INT8U);

	pReq[0] = IPMI_AMI_SET_NETWORK_SHARE_OPERATION;
	pReq[1] = NetworkOperation;

	if(NetworkOperation ==5 || NetworkOperation==6)
	{
		wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
		((NETFN_AMI << 2) + 0x00), CMD_AMI_FIRMWAREUPDATE,
		pReq,ReqLen ,
		pRes_Status, &ResLen, timeout );
		*Status = pRes_Status[1];
	}
	else
	{
		wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
		((NETFN_AMI << 2) + 0x00), CMD_AMI_FIRMWAREUPDATE,
		pReq,ReqLen ,
		pRes, &ResLen, timeout );
		*Status = 0xff;
	}
	return wRet;
}

/*
*@fn IPMICMD_AMIGetBackupFlag
*@brief IPMI Command API to Getting Backup Flag
*@param pSession - IPMI Session Handle
*@param pGetBackupFlagReq - Pointer to the request of Get Backup Flag command
*@param pGetBackupFlagRes - Pointer to the response of Get Backup Flag command
*@param timeout - Timeout Value
*@return Returns LIBIPMI_E_SUCCESS on success
*@return Returns proper completion code on error
**/
uint16 IPMICMD_AMIGetBackupFlag(IPMI20_SESSION_T *pSession, GetBackupConfigReq_T* pGetBackupFlagReq,
                                              uint32 dwReqLen, GetBackupConfigRes_T* pGetBackupFlagRes, int timeout)
{
    uint16 wRet;
    uint32 dwResLen;
    dwResLen = sizeof(GetBackupConfigRes_T);

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                        (NETFN_AMI << 2), CMD_AMI_GET_BACKUP_FLAG,
                        (uint8 *)pGetBackupFlagReq, dwReqLen,
                        (uint8 *)pGetBackupFlagRes, &dwResLen,
                        timeout);

    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TCRIT("Error in Getting Backup Flag in raw IPMI command layer::%d\n", wRet);
    }
    return (wRet);

}

/*
*@fn LIBIPMI_HL_AMIGetBackupFlag
*@brief High Level command to Getting the Backup Flag
*@param pSession - IPMI Session Handle
*@param pCount   - Pointer to the Backup Flag Count
*@param pEnabledFlag - Pointer to the enabled flag that tells the features are enabled/disabled
*@param timeout  - Timeout Value
*@return Returns LIBIPMI_E_SUCCESS on success
*@return Returns proper completion code on error
**/
uint16 LIBIPMI_HL_AMIGetBackupFlag(IPMI20_SESSION_T *pSession, INT8U *pCount, INT8U *pEnabledFlag, int timeout)
{
    uint16 wRet;
    uint32 ReqLen = 1;
    GetBackupConfigReq_T Req;
    GetBackupConfigRes_T Res = {0,{{0,"0"}}};

    memset (&Req, 0, sizeof (GetBackupConfigReq_T));
    Req.Parameter = GET_BACKUP_FLAG;
    wRet =  IPMICMD_AMIGetBackupFlag(pSession, &Req, ReqLen, &Res, timeout);

    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TCRIT("Error in Getting Backup Flag and Count::%d\n", wRet);
        return wRet;
    }
    *pCount = Res.GetBackupConfig.GetBackupFlag.Count;
    memcpy(pEnabledFlag, &Res.GetBackupConfig.GetBackupFlag.Selector, MAX_BACKUP_FEATURE);
    return wRet;
}

/*
*@fn LIBIPMI_HL_AMIGetBackupList
*@brief High Level command to Getting the Backup List
*@param pSession - IPMI Session Handle
*@param Selector - Selector of the backup config feature
*@param pBackupConfig - Pointer to the structure of Backup Config
*@param timeout  - Timeout Value
*@return Returns LIBIPMI_E_SUCCESS on success
*@return Returns proper completion code on error
**/
uint16 LIBIPMI_HL_AMIGetBackupList(IPMI20_SESSION_T *pSession, INT8U Selector, GetBackupList_T *pBackupConfig, int timeout)
{
    uint16 wRet;
    uint32 ReqLen = 2;
    GetBackupConfigReq_T Req;
    GetBackupConfigRes_T Res = {0,{{0,"0"}}};

    memset (&Req, 0, sizeof (GetBackupConfigReq_T));
    Req.Parameter = GET_BACKUP_LIST;
    Req.Selector = Selector;
    wRet =  IPMICMD_AMIGetBackupFlag(pSession, &Req, ReqLen, &Res, timeout);

    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TCRIT("Error While Getting Backup List info::%d\n", wRet);
        return wRet;
    }
    pBackupConfig->Selector = Res.GetBackupConfig.GetBackupList.Selector;
    strncpy(((char*)pBackupConfig->ConfigFile), ((char*)Res.GetBackupConfig.GetBackupList.ConfigFile), MAX_FILENAME_LEN-1);
    pBackupConfig->BackupFlag = Res.GetBackupConfig.GetBackupList.BackupFlag;
    return wRet;
}

/*
*@fn IPMICMD_AMISetBackupFlag
*@brief IPMI Command API to Setting the Backup Flag
*@param pSession - IPMI Session Handle
*@param pSetBackupFlagReq - Pointer to the request of Set Backup Flag command
*@param pSetBackupFlagRes - Pointer to the response of Set Backup Flag command
*@param timeout - Timeout Value
*@return Returns LIBIPMI_E_SUCCESS on success
*@return Returns proper completion code on error
**/
uint16 IPMICMD_AMISetBackupFlag(IPMI20_SESSION_T *pSession, SetBackupFlagReq_T* pSetBackupFlagReq
                                               , SetBackupFlagRes_T* pSetBackupFlagRes, int timeout)
{
    uint16 wRet;
    uint32 dwResLen;
    dwResLen = sizeof(SetBackupFlagRes_T);

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                        (NETFN_AMI << 2), CMD_AMI_SET_BACKUP_FLAG,
                        (uint8 *)pSetBackupFlagReq, sizeof(SetBackupFlagReq_T),
                        (uint8*)pSetBackupFlagRes, &dwResLen,
                        timeout);

    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TCRIT("Error in Setting Backup Flag in raw IPMI command layer::%d\n", wRet);
    }
    return (wRet);

}

/*
*@fn LIBIPMI_HL_AMISetBackupFlag
*@brief High Level command to Setting the Backup Flag
*@param pSession - IPMI Session Handle
*@param backupflag - Backup Flag to be backed up the BMC Configuration
*@param timeout  - Timeout Value
*@return Returns LIBIPMI_E_SUCCESS on success
*@return Returns proper completion code on error
**/
uint16 LIBIPMI_HL_AMISetBackupFlag(IPMI20_SESSION_T *pSession, INT16U backupflag, int timeout)
{
    uint16 wRet;
    SetBackupFlagReq_T Req;
    SetBackupFlagRes_T Res = {0};

    memset (&Req, 0, sizeof (SetBackupFlagReq_T));
    Req.BackupFlag = backupflag;

    wRet =  IPMICMD_AMISetBackupFlag(pSession, &Req, &Res, timeout);

    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TCRIT("Error While Setting Backup Flag%d\n", wRet);
        return wRet;
    }
    return (uint16)Res.CompletionCode;
}

/*
*@fn IPMICMD_AMIManageBMCConfig
*@brief IPMI Command API to Manage the BMC Configuration
*@param pSession - IPMI Session Handle
*@param pAMIManageBMCConfigReq - Pointer to the request of AMI Manage BMC Config command
*@param pAMIManageBMCConfigRes - Pointer to the response of AMI Manage BMC Config command
*@param timeout - Timeout Value
*@return Returns LIBIPMI_E_SUCCESS on success
*@return Returns proper completion code on error
**/
uint16 IPMICMD_AMIManageBMCConfig(IPMI20_SESSION_T *pSession, AMIManageBMCConfigReq_T* pAMIManageBMCConfigReq
                                               , AMIManageBMCConfigRes_T* pAMIManageBMCConfigRes, int timeout)
{
    uint16 wRet;
    uint32 dwResLen;
    dwResLen = sizeof(AMIManageBMCConfigRes_T);

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                        (NETFN_AMI << 2), CMD_AMI_MANAGE_BMC_CONFIG,
                        (uint8 *)pAMIManageBMCConfigReq, 1,
                        (uint8 *)pAMIManageBMCConfigRes, &dwResLen,
                        timeout);

    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TCRIT("Error in Backuping the BMC Configuration in raw IPMI command layer::%d\n", wRet);
    }
    return (wRet);

}

/*
*@fn LIBIPMI_HL_AMIManageBMCConfig_BackupConf
*@brief High Level command to Backup the BMC Configuration
*@param pSession - IPMI Session Handle
*@param timeout  - Timeout Value
*@return Returns LIBIPMI_E_SUCCESS on success
*@return Returns proper completion code on error
**/
uint16 LIBIPMI_HL_AMIManageBMCConfig_BackupConf(IPMI20_SESSION_T *pSession, int timeout)
{
    uint16 wRet;
    AMIManageBMCConfigReq_T Req;
    AMIManageBMCConfigRes_T Res = {0};

    memset (&Req, 0, sizeof (AMIManageBMCConfigReq_T));
    Req.Parameter = BACKUP_CONF;

    wRet =  IPMICMD_AMIManageBMCConfig(pSession, &Req, &Res, timeout);

    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TCRIT("Error in Backuping the BMC Configuration%d\n", wRet);
        return wRet;
    }
    return (uint16)Res.CompletionCode;
}

/*
*@fn LIBIPMI_HL_AMIManageBMCConfig_RestoreConf
*@brief High Level command to restore the BMC Configuration
*@param pSession - IPMI Session Handle
*@param timeout  - Timeout Value
*@return Returns LIBIPMI_E_SUCCESS on success
*@return Returns proper completion code on error
**/
uint16 LIBIPMI_HL_AMIManageBMCConfig_RestoreConf(IPMI20_SESSION_T *pSession, int timeout)
{
    uint16 wRet;
    AMIManageBMCConfigReq_T Req;
    AMIManageBMCConfigRes_T Res = {0};

    memset (&Req, 0, sizeof (AMIManageBMCConfigReq_T));
    Req.Parameter = RESTORE_CONF;

    wRet =  IPMICMD_AMIManageBMCConfig(pSession, &Req, &Res, timeout);

    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TCRIT("Error While restoring BMC Configuration%d\n", wRet);
        return wRet;
    }
    return (uint16)Res.CompletionCode;
}

/*
*@fn IPMICMD_AMIGetPendStatus
*@brief IPMI Command API to get the pend status
*@param pSession - IPMI Session Handle
*@param pAMIGetPendStatusReq - Pointer to the request of AMI Get Pend Status command
*@param pAMIGetPendStatusRes - Pointer to the response of AMI Get Pend Status command
*@param timeout - Timeout Value
*@return Returns LIBIPMI_E_SUCCESS on success
*@return Returns proper completion code on error
**/
uint16 IPMICMD_AMIGetPendStatus(IPMI20_SESSION_T *pSession, AMIGetPendStatusReq_T* pAMIGetPendStatusReq
                                               , AMIGetPendStatusRes_T* pAMIGetPendStatusRes, int timeout)
{
    uint16 wRet;
    uint32 dwResLen;
    dwResLen = sizeof(AMIGetPendStatusRes_T);

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                        (NETFN_AMI << 2), CMD_AMI_GET_PEND_STATUS,
                        (uint8 *)pAMIGetPendStatusReq, sizeof(AMIGetPendStatusReq_T),
                        (uint8 *)pAMIGetPendStatusRes, &dwResLen,
                        timeout);

    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TCRIT("Error in Getting the Pend Status in raw IPMI command layer:%d\n", wRet);
    }
    return (wRet);

}

/*
*@fn LIBIPMI_HL_AMIGetPendStatus
*@brief High Level command to get the Pend status
*@param pSession - IPMI Session Handle
*@param pStatus   - Pointer to the Status
*@param pErrorCode - Pointer to the Error code
*@param timeout  - Timeout Value
*@return Returns LIBIPMI_E_SUCCESS on success
*@return Returns proper completion code on error
**/
uint16 LIBIPMI_HL_AMIGetPendStatus(IPMI20_SESSION_T *pSession, INT8U *pStatus, INT16U *pErrorCode, int timeout)
{
    uint16 wRet;
    AMIGetPendStatusReq_T Req;
    AMIGetPendStatusRes_T Res = {0,0,0};

    memset (&Req, 0, sizeof(AMIGetPendStatusReq_T));
    Req.Operation = PEND_OP_MANAGE_BMC_BKUPCONFIG; //PendTask command no to get Backup/Restore config file status

    wRet =  IPMICMD_AMIGetPendStatus(pSession, &Req, &Res, timeout);

    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TCRIT("Error in Getting the Pend Status%d\n", wRet);
        return wRet;
    }
    *pStatus = Res.PendStatus;
    *pErrorCode = Res.ErrorCode;
    return Res.CompletionCode;
}

uint16 IPMICMD_AMIMediaRedirectStartStop(IPMI20_SESSION_T *pSession,AMIMediaRedirctionStartStopReq_T *pAMIMediaRedirctionStartStopReq,
                                                  int timeout)
{
    AMIMediaRedirctionStartStopRes_T  AMIMediaRedirctionStartStopRes;
    uint16 wRet;
    uint32 dwResLen;
    uint32 dwReqLen = sizeof(AMIMediaRedirctionStartStopReq_T);

    if(pAMIMediaRedirctionStartStopReq->Param == BY_IMAGE_INDEX)
        dwReqLen = dwReqLen - sizeof(pAMIMediaRedirctionStartStopReq->ImageInfo) +sizeof(INT8U);

    memset(&AMIMediaRedirctionStartStopRes,0,sizeof(AMIMediaRedirctionStartStopRes_T));
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                        (NETFN_AMI << 2), CMD_AMI_MEDIA_REDIRECTION_START_STOP,
                        (uint8 *)pAMIMediaRedirctionStartStopReq, dwReqLen,
                        (uint8*)&AMIMediaRedirctionStartStopRes, &dwResLen,
                        timeout);
    return (wRet);
}

uint16 LIBIPMI_HL_AMIMediaRedirectStartStop(IPMI20_SESSION_T *pSession, INT8U *ImageName, INT8U ImageType, INT8U ImageIndex,INT8U State,INT8U Media_Type,int timeout)
{
    AMIMediaRedirctionStartStopReq_T AMIMediaRedirctionStartStopReq = {0,0,0,0,{0}};
    uint16 wRet;
    if (ImageName == NULL)
    {
        AMIMediaRedirctionStartStopReq.Param = BY_IMAGE_INDEX;
        AMIMediaRedirctionStartStopReq.ImageInfo.Index = ImageIndex;
    }
    else
    {
        AMIMediaRedirctionStartStopReq.Param = BY_IMAGE_NAME;
        strncpy((char*)AMIMediaRedirctionStartStopReq.ImageInfo.Name,(char *)ImageName,MAX_IMAGE_NAME_LEN-1);
        AMIMediaRedirctionStartStopReq.ImageInfo.Index = -1;
    }
    AMIMediaRedirctionStartStopReq.AppType = Media_Type;
    AMIMediaRedirctionStartStopReq.ImageType = ImageType;
    AMIMediaRedirctionStartStopReq.RedirectionState = State;

    wRet=IPMICMD_AMIMediaRedirectStartStop(pSession,&AMIMediaRedirctionStartStopReq, timeout);
    if(wRet != CC_SUCCESS)
    {
        TINFO("Error in Redirecting The Image start and stop ..wRet = %x\n",wRet);
        return wRet;
    }
    return wRet;
}

uint16 IPMICMD_AMIGetMediaInfo(IPMI20_SESSION_T *pSession,AMIGetMediaInfoReq_T *pAMIGetMediaInfoReq,
                                                   AMIGetMediaInfoRes_T *pAMIGetMediaInfoRes,int timeout)
{
   uint16 wRet;
   uint32 dwResLen;
   uint32 dwReqLen = sizeof(AMIGetMediaInfoReq_T) -1;

   if(pAMIGetMediaInfoReq->Param < GET_REDIRECT_IMAGE_INFO )
       dwReqLen -= sizeof(INT8U);
   else if(pAMIGetMediaInfoReq->Param == CLEAR_MEDIA_TYPE_INDEX_ERROR)
       dwReqLen = sizeof(AMIGetMediaInfoReq_T);

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                        (NETFN_AMI << 2), CMD_AMI_GET_MEDIA_INFO,
                        (uint8 *)pAMIGetMediaInfoReq, dwReqLen,
                        (uint8*)pAMIGetMediaInfoRes, &dwResLen,
                        timeout);

    return (wRet);
}

uint16 LIBIPMI_HL_AMIGetMediaRedirectedImagesCount(IPMI20_SESSION_T *pSession, INT8U *ImagesCount,INT8U Media_Type,int timeout)
{
   AMIGetMediaInfoReq_T AMIGetMediaInfoReq = {0,0,0,0};
   AMIGetMediaInfoRes_T AMIGetMediaInfoRes = {0,0,0};
   uint16 wRet;
   AMIGetMediaInfoReq.Param = MAX_REDIRECT_IMAGES;
   AMIGetMediaInfoReq.AppType = Media_Type;
   wRet=IPMICMD_AMIGetMediaInfo(pSession,&AMIGetMediaInfoReq,
                                  &AMIGetMediaInfoRes, timeout);
   if(wRet != CC_SUCCESS)
   {
        TINFO("Error in Getting the Total Redirected Images count..wRet = %x\n",wRet);
        return wRet;
   }
   *ImagesCount = AMIGetMediaInfoRes.Num_of_images;
   return wRet;
}

uint16 LIBIPMI_HL_AMIGetMediaRedirectedImagesInfo(IPMI20_SESSION_T *pSession, RedirectedImageInfo_T* RedirectedImageInfo,INT8U ImagesCount,INT8U Media_Type,int timeout)
{
   AMIGetMediaInfoReq_T AMIGetMediaInfoReq = {0,0,0,0};
   RedirectedImageInfo_T* pRedirectedImageBuff = NULL;
   INT8U *ResBuff = NULL;
   int i=0;
   uint16 wRet;

   ResBuff = (INT8U *)malloc((ImagesCount)*sizeof(RedirectedImageInfo_T) + sizeof(INT16U));
   if(ResBuff == NULL)
   {
       TINFO("Error in Allocating the Memory \n");
       return -1;
   }
   AMIGetMediaInfoReq.Param = GET_REDIRECT_IMAGE_INFO;
   AMIGetMediaInfoReq.AppType = Media_Type;
   AMIGetMediaInfoReq.ImageType = IMAGE_TYPE_ALL;
   wRet=IPMICMD_AMIGetMediaInfo(pSession,&AMIGetMediaInfoReq,
                        (AMIGetMediaInfoRes_T *)ResBuff, timeout);
   if(wRet != CC_SUCCESS)
   {
        TINFO("Error in Getting the Total Redirected Images count..wRet = %x\n",wRet);
        free(ResBuff);
        return wRet;
   }
   pRedirectedImageBuff = (RedirectedImageInfo_T*) (ResBuff+2);
   for(i=0;i<ImagesCount;i++)
   {
       memcpy(&RedirectedImageInfo[i], &pRedirectedImageBuff[i],sizeof(RedirectedImageInfo_T));
   }
   free(ResBuff);
   return wRet;
}

uint16 LIBIPMI_HL_AMIGetAllAvaliableImagesCount(IPMI20_SESSION_T *pSession, INT8U *ImagesCount,INT8U Media_Type,int timeout)
{
   AMIGetMediaInfoReq_T AMIGetMediaInfoReq = {0,0,0,0};
   AMIGetMediaInfoRes_T AMIGetMediaInfoRes = {0,0,0};
   uint16 wRet;
   AMIGetMediaInfoReq.Param = MEDIA_NUM_OF_IMAGES;
   AMIGetMediaInfoReq.AppType = Media_Type;
   wRet=IPMICMD_AMIGetMediaInfo(pSession,&AMIGetMediaInfoReq,
                       &AMIGetMediaInfoRes, timeout);
   if(wRet != CC_SUCCESS)
   {
       TINFO("Error in Getting the Total Redirected Images count..wRet = %x\n",wRet);
       return wRet;
   }
   *ImagesCount = AMIGetMediaInfoRes.Num_of_images;
   return wRet;
}

uint16 LIBIPMI_HL_AMIGetAllAvaliableImagesInfo(IPMI20_SESSION_T *pSession, LongImageInfo_T* AvaliableImageInfo,INT8U ImagesCount,INT8U Media_Type,int timeout)
{
   AMIGetMediaInfoReq_T AMIGetMediaInfoReq = {0,0,0,0};
   LongImageInfo_T* pAvaliableImageBuff = NULL;
   INT8U *ResBuff = NULL;
   int i=0;
   uint16 wRet;

   ResBuff = (INT8U *)malloc((ImagesCount)*sizeof(LongImageInfo_T) + sizeof(INT16U));
   if(ResBuff == NULL)
   {
       TINFO("Error in Allocating the Memory \n");
       return -1;
   }
   memset (ResBuff, 0, (ImagesCount * (sizeof(LongImageInfo_T))+sizeof(INT16U)));
   AMIGetMediaInfoReq.Param = GET_ALL_AVILABLE_IMAGE_INFO;
   AMIGetMediaInfoReq.AppType = Media_Type;
   AMIGetMediaInfoReq.ImageType = LONG_IMG_NAME_FORMAT;
   wRet=IPMICMD_AMIGetMediaInfo(pSession,&AMIGetMediaInfoReq,
                     (AMIGetMediaInfoRes_T *)ResBuff, timeout);
   if(wRet != CC_SUCCESS)
   {
        TINFO("Error in Getting the Total Redirected Images count..wRet = %x\n",wRet);
        free(ResBuff);
        return wRet;
   }
   pAvaliableImageBuff = (LongImageInfo_T*) (ResBuff+2);
   for(i=0;i<ImagesCount;i++)
   {
       memcpy(&AvaliableImageInfo[i], &pAvaliableImageBuff[i],sizeof(LongImageInfo_T));
   }
   free(ResBuff);
   return wRet;
}

uint16 LIBIPMI_HL_AMIClearImageInfo(IPMI20_SESSION_T *pSession, INT8U ImageType,INT8U ImageIndex,INT8U Media_Type,int timeout)
{
   AMIGetMediaInfoReq_T AMIGetMediaInfoReq = {0,0,0,0};
   AMIGetMediaInfoRes_T AMIGetMediaInfoRes = {0,0,0};
   uint16 wRet = 0;

   AMIGetMediaInfoReq.Param = CLEAR_MEDIA_TYPE_INDEX_ERROR;
   AMIGetMediaInfoReq.AppType = Media_Type;
   AMIGetMediaInfoReq.ImageType = ImageType;
   AMIGetMediaInfoReq.ImageIndex = ImageIndex;

   wRet=IPMICMD_AMIGetMediaInfo(pSession,&AMIGetMediaInfoReq,
                        (AMIGetMediaInfoRes_T *)&AMIGetMediaInfoRes, timeout);
   if(wRet != CC_SUCCESS)
   {
       TINFO("Error in Clearing the Image Info wRet = %x\n",wRet);
       return wRet;
   }
   return wRet;
}

uint16 IPMICMD_AMISetMediaInfo(IPMI20_SESSION_T *pSession,AMISetMediaInfoReq_T *pAMISetMediaInfoReq,int timeout)
{
    AMISetMediaInfoRes_T AMISetMediaInfoRes={0};
    uint16 wRet;
    uint32 dwResLen;
    uint32 dwReqLen = sizeof(AMISetMediaInfoReq_T) - sizeof(SetOperations_T);

   if(pAMISetMediaInfoReq->Param == DELETE_MEDIA_IMAGE)
        dwReqLen += sizeof(Delete_T);
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                        (NETFN_AMI << 2), CMD_AMI_SET_MEDIA_INFO,
                        (uint8 *)pAMISetMediaInfoReq, dwReqLen,
                        (uint8*)&AMISetMediaInfoRes, &dwResLen,
                        timeout);
    return (wRet);
}

uint16 LIBIPMI_HL_AMIDeleteImage(IPMI20_SESSION_T *pSession, INT8U Media_Type, INT8U ImageIndex, char *ImageName,int timeout)
{
   AMISetMediaInfoReq_T AMISetMediaInfoReq = {0,0,{{0,"0"}}};
   uint16 wRet = 0;

   AMISetMediaInfoReq.Param = DELETE_MEDIA_IMAGE;
   AMISetMediaInfoReq.AppType = Media_Type;
   AMISetMediaInfoReq.Ops.Delete.ImageIndex= ImageIndex;
   strncpy((char *)AMISetMediaInfoReq.Ops.Delete.ImageName, (char *)ImageName,MAX_IMAGE_NAME_LEN-1);
   wRet=IPMICMD_AMISetMediaInfo(pSession,&AMISetMediaInfoReq, timeout);
   if(wRet != CC_SUCCESS)
   {
       TINFO("Error in Deleting the Image  wRet = %x\n",wRet);
       return wRet;
   }
   return wRet;
}

uint16 LIBIPMI_HL_AMIUpdateImagelist(IPMI20_SESSION_T *pSession, INT8U Media_Type,int timeout)
{
   AMISetMediaInfoReq_T AMISetMediaInfoReq = {0,0,{{0,"0"}}};
   uint16 wRet = 0;

   AMISetMediaInfoReq.Param = UPDATE_MEDIA_IMAGE_LIST;
   AMISetMediaInfoReq.AppType = Media_Type;
   wRet=IPMICMD_AMISetMediaInfo(pSession,&AMISetMediaInfoReq, timeout);
   if(wRet != CC_SUCCESS)
   {
       TINFO("Error in Updating the Imagelist wRet = %x\n",wRet);
       return wRet;
   }
   return wRet;
}

uint16 IPMICMD_AMIGetBMCInstanceCount(IPMI20_SESSION_T *pSession,
                AMIGetBMCInstanceCountRes_T *pAMIGetBMCInstanceCountRes,
                int timeout)
{
	uint16 wRet;
    uint8 *pReq = NULL;
    uint32 ResLen=sizeof(AMIGetBMCInstanceCountRes_T);
    memset(pAMIGetBMCInstanceCountRes, 0, sizeof (AMIGetBMCInstanceCountRes_T));

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                  ((NETFN_AMI << 2) + 0x00), CMD_AMI_GET_BMC_INSTANCE_COUNT,
                                   pReq, 0,
                                  (uint8 *)pAMIGetBMCInstanceCountRes, &ResLen, timeout );
    return wRet;
}

uint16 LIBIPMI_HL_AMIGetBMCInstanceCount(IPMI20_SESSION_T *pSession,INT8U *BMCCount, int timeout)
{
       AMIGetBMCInstanceCountRes_T GetBMCInstanceCountRes;
       uint16 wRet;

       wRet = IPMICMD_AMIGetBMCInstanceCount(pSession,&GetBMCInstanceCountRes,timeout);

       if(wRet == CC_NORMAL)
       {
               *BMCCount = GetBMCInstanceCountRes.BMCInstanceCount;
       }

       return wRet;
}

uint16 IPMICMD_AMIGetUSBSwitchSetting(IPMI20_SESSION_T *pSession,
                AMIGetUSBSwitchSettingRes_T *pAMIGetUSBSwitchSettingRes,
                int timeout)
{
	uint16 wRet;
    uint8 *pReq = NULL;
    uint32 ResLen=sizeof(AMIGetUSBSwitchSettingRes_T);
    memset(pAMIGetUSBSwitchSettingRes, 0, sizeof (AMIGetUSBSwitchSettingRes_T));

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                  ((NETFN_AMI << 2) + 0x00), CMD_AMI_GET_USB_SWITCH_SETTING,
                                   pReq, 0,
                                  (uint8 *)pAMIGetUSBSwitchSettingRes, &ResLen, timeout );
    return wRet;
}

uint16 LIBIPMI_HL_AMIGetUSBSwitchSetting(IPMI20_SESSION_T *pSession,INT8U *USBSwitchSetting, int timeout)
{
	AMIGetUSBSwitchSettingRes_T GetUSBSwitchSettingRes={0,0};
	uint16 wRet;

	wRet = IPMICMD_AMIGetUSBSwitchSetting(pSession,&GetUSBSwitchSettingRes,timeout);

	if(wRet != CC_NORMAL)
	{
		TCRIT("Error in Getting the USB Switch Setting::%x\n", wRet);
		return wRet;
	}

	*USBSwitchSetting = GetUSBSwitchSettingRes.USBSwitchSetting;
	return wRet;
}

uint16 IPMICMD_AMISetUSBSwitchSetting(IPMI20_SESSION_T *pSession,
                AMISetUSBSwitchSettingReq_T *pAMISetUSBSwitchSettingReq,
                AMISetUSBSwitchSettingRes_T *pAMISetUSBSwitchSettingRes,
                int timeout)
{
	uint16 wRet;
	uint32 wReqLen = sizeof(AMISetUSBSwitchSettingReq_T);
	uint32 wResLen = sizeof(AMISetUSBSwitchSettingRes_T);

    memset(pAMISetUSBSwitchSettingRes, 0, sizeof (AMISetUSBSwitchSettingRes_T));

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                  ((NETFN_AMI << 2) + 0x00), CMD_AMI_SET_USB_SWITCH_SETTING,
	                               (uint8 *)pAMISetUSBSwitchSettingReq, wReqLen ,
	                               (uint8 *)pAMISetUSBSwitchSettingRes, &wResLen, timeout );
    return wRet;
}


uint16 LIBIPMI_HL_AMISetUSBSwitchSetting(IPMI20_SESSION_T *pSession,INT8U USBSwitchSetting, int timeout)
{
    AMISetUSBSwitchSettingReq_T SetUSBSwitchSettingReq = {0};
    AMISetUSBSwitchSettingRes_T SetUSBSwitchSettingRes = {0};
    uint16 wRet;

	SetUSBSwitchSettingReq.USBSwitchSetting = USBSwitchSetting;
    wRet = IPMICMD_AMISetUSBSwitchSetting(pSession, &SetUSBSwitchSettingReq, &SetUSBSwitchSettingRes, timeout);

	if(wRet != CC_NORMAL)
	{
		TCRIT("Error in Setting the USB Switch Setting::%x\n",wRet);
		return wRet;
	}

	return wRet;
}

uint16 IPMICMD_AMISwitchMUX(IPMI20_SESSION_T *pSession, AMISwitchMUXReq_T* pAMISwitchMUXReq,
                                               uint8* pAMISwitchMUXRes, int timeout)
{
    uint16 wRet;
    uint32 dwResLen;
    dwResLen = sizeof(uint8);

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                        (NETFN_AMI << 2), CMD_AMI_MUX_SWITCHING,
                        (uint8 *)pAMISwitchMUXReq, sizeof(AMISwitchMUXReq_T),
                        (uint8 *)pAMISwitchMUXRes, &dwResLen,
                        timeout);

    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TCRIT("Error in Switching MUX in raw IPMI command layer:%0X\n", wRet);
    }
    return (wRet);

}


uint16 IPMICMD_AMIGetRAIDConfig(IPMI20_SESSION_T *pSession,
                INT8U *pReq,uint32 ReqLen,INT8U *pRes,
                int timeout)
{
    uint16 wRet;
    uint32 wResLen = sizeof(AMIGetRAIDConfigRes_T);


    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                  ((NETFN_AMI << 2) + 0x00), CMD_AMI_GET_RAID_INFO,
                                   (uint8 *)pReq, ReqLen ,
                                   (uint8 *)pRes, &wResLen, timeout );
    return wRet;
}

uint16 LIBIPMI_HL_AMIGetRAIDCtrlCount(IPMI20_SESSION_T *pSession,INT8U *CtrlCount, INT32U *CtrlIDList, int timeout)
{
    uint16 wRet, ReqLen;
    AMIGetRAIDConfigReq_T   AMIGetRAIDConfigReq;
    INT8U pRes[1024] = {0};

    memset(&AMIGetRAIDConfigReq, 0, sizeof(AMIGetRAIDConfigReq));

    AMIGetRAIDConfigReq.Param = GET_RAID_CONTROLLER_COUNT;
    ReqLen = 3;

    wRet = IPMICMD_AMIGetRAIDConfig(pSession,(INT8U*)&AMIGetRAIDConfigReq,ReqLen,pRes,timeout);
    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TINFO("Error while getting the RAID Controller Count %x", wRet);
    }

    *CtrlCount = pRes[1];
    memcpy(CtrlIDList, &pRes[2], (sizeof(INT32U) * pRes[1]));

    return wRet;
}

uint16 LIBIPMI_HL_AMIGetRAIDCtrlInfo(IPMI20_SESSION_T *pSession,INT32U CtrlID, RAIDInfo_T *pRAIDInfo,int timeout)
{
    uint16 wRet, ReqLen;
    AMIGetRAIDConfigReq_T   AMIGetRAIDConfigReq;
    AMIGetRAIDConfigRes_T   AMIGetRAIDConfigRes;


    memset(&AMIGetRAIDConfigReq, 0, sizeof(AMIGetRAIDConfigReq));
    memset(&AMIGetRAIDConfigRes, 0, sizeof(AMIGetRAIDConfigRes));

    AMIGetRAIDConfigReq.Param = GET_RAID_CONTROLLER_INFO;
    AMIGetRAIDConfigReq.ParamData.CtrlID = CtrlID;
    ReqLen = 3 + sizeof(CtrlID);

    wRet = IPMICMD_AMIGetRAIDConfig(pSession,(INT8U*)&AMIGetRAIDConfigReq,ReqLen,(INT8U*)&AMIGetRAIDConfigRes,timeout);
    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TINFO("Error while getting the RAID Controller's ( %d )Info : Error %x", CtrlID, wRet);
    }

    memcpy(pRAIDInfo,&AMIGetRAIDConfigRes.ParamData.CtrlInfo, sizeof(RAIDInfo_T));
    return wRet;
}

uint16 LIBIPMI_HL_AMIGetRAIDPhyDevCount(IPMI20_SESSION_T *pSession,INT32U CtrlID, INT8U *PhyCount, INT16U *PhyDevIDList, int timeout)
{
    uint16 wRet, ReqLen;
    AMIGetRAIDConfigReq_T   AMIGetRAIDConfigReq;
    INT8U pRes[1024] = {0};

    memset(&AMIGetRAIDConfigReq, 0, sizeof(AMIGetRAIDConfigReq));

    AMIGetRAIDConfigReq.Param = GET_CONTROLLER_PHY_DEV_COUNT;
    AMIGetRAIDConfigReq.ParamData.CtrlID = CtrlID;
    ReqLen = 3 + sizeof(CtrlID);

    wRet = IPMICMD_AMIGetRAIDConfig(pSession,(INT8U*)&AMIGetRAIDConfigReq,ReqLen,pRes,timeout);
    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TINFO("Error while getting the RAID Controller's ( %d ) Physical Device Count: Error %x",CtrlID, wRet);
    }

    *PhyCount = pRes[5];
    memcpy(PhyDevIDList, &pRes[6], (sizeof(INT16U) * pRes[5]));

    return wRet;
}

uint16 LIBIPMI_HL_AMIGetRAIDPhyDevInfo(IPMI20_SESSION_T *pSession,INT32U CtrlID, INT16U PhyDevID,PhyDevInfo_T *pPhyDevInfo, int timeout)
{
    uint16 wRet, ReqLen;
    AMIGetRAIDConfigReq_T   AMIGetRAIDConfigReq;
    AMIGetRAIDConfigRes_T   AMIGetRAIDConfigRes;


    memset(&AMIGetRAIDConfigReq, 0, sizeof(AMIGetRAIDConfigReq));
    memset(&AMIGetRAIDConfigRes, 0, sizeof(AMIGetRAIDConfigRes));

    AMIGetRAIDConfigReq.Param = GET_CONTROLLER_PHY_DEV_INFO;
    AMIGetRAIDConfigReq.ParamData.DevInfo.CtrlID = CtrlID;
    AMIGetRAIDConfigReq.ParamData.DevInfo.DevID = PhyDevID;
    ReqLen = 3 + sizeof(Device_T);

    wRet = IPMICMD_AMIGetRAIDConfig(pSession,(INT8U*)&AMIGetRAIDConfigReq,ReqLen,(INT8U*)&AMIGetRAIDConfigRes,timeout);
    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TINFO("Error while getting the RAID Controller's ( %d ) Physical Device ( %d ) Info: Error %x", CtrlID, PhyDevID, wRet);
    }

    memcpy(pPhyDevInfo,&AMIGetRAIDConfigRes.ParamData.Phy, sizeof(PhyDevInfo_T));
    return wRet;
}


uint16 LIBIPMI_HL_AMIGetRAIDLogDevCount(IPMI20_SESSION_T *pSession,INT32U CtrlID, INT8U *LogCount, INT16U *LogDevIDList, int timeout)
{
    uint16 wRet, ReqLen;
    AMIGetRAIDConfigReq_T   AMIGetRAIDConfigReq;
    INT8U pRes[1024] = {0};

    memset(&AMIGetRAIDConfigReq, 0, sizeof(AMIGetRAIDConfigReq));

    AMIGetRAIDConfigReq.Param = GET_CONTROLLER_LOG_DEV_COUNT;
    AMIGetRAIDConfigReq.ParamData.CtrlID = CtrlID;
    ReqLen = 3 + sizeof(CtrlID);

    wRet = IPMICMD_AMIGetRAIDConfig(pSession,(INT8U*)&AMIGetRAIDConfigReq,ReqLen,pRes,timeout);
    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TINFO("Error while getting the RAID Controller's ( %d ) Logical Device Count: Error%x", CtrlID, wRet);
    }

    *LogCount = pRes[5];
    memcpy(LogDevIDList, &pRes[6], (sizeof(INT16U) * pRes[5]));

    return wRet;
}

uint16 LIBIPMI_HL_AMIGetRAIDLogDevInfo(IPMI20_SESSION_T *pSession,INT32U CtrlID,INT16U LogDevID, LogicalDevInfo_T *pLogDevInfo, int timeout)
{
    uint16 wRet, ReqLen;
    AMIGetRAIDConfigReq_T   AMIGetRAIDConfigReq;
    AMIGetRAIDConfigRes_T   AMIGetRAIDConfigRes;
    INT32U ResSize = 0;


    memset(&AMIGetRAIDConfigReq, 0, sizeof(AMIGetRAIDConfigReq));
    memset(&AMIGetRAIDConfigRes, 0, sizeof(AMIGetRAIDConfigRes));

    AMIGetRAIDConfigReq.Param = GET_CONTROLLER_LOG_DEV_INFO;
    AMIGetRAIDConfigReq.ParamData.DevInfo.CtrlID = CtrlID;
    AMIGetRAIDConfigReq.ParamData.DevInfo.DevID = LogDevID;
    ReqLen = 3 + sizeof(Device_T);

    wRet = IPMICMD_AMIGetRAIDConfig(pSession,(INT8U*)&AMIGetRAIDConfigReq,ReqLen,(INT8U*)&AMIGetRAIDConfigRes,timeout);
    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TINFO("Error while getting the RAID Controller's ( %d ) Logical Device ( %d ) Info: Error %x", CtrlID, LogDevID, wRet);
    }

    ResSize = sizeof(LogicalDevInfo_T) - (sizeof(Element) * (MAX_ELEMENT_NUM - AMIGetRAIDConfigRes.ParamData.Logical.ElementsNum));
    memcpy(pLogDevInfo,&AMIGetRAIDConfigRes.ParamData.Logical, ResSize);
    return wRet;

}


uint16 LIBIPMI_HL_AMIGetRAIDBBUInfo(IPMI20_SESSION_T *pSession,INT32U CtrlID, BBUInfo_T *pBBUInfo, int timeout)
{
    uint16 wRet, ReqLen;
    AMIGetRAIDConfigReq_T   AMIGetRAIDConfigReq;
    AMIGetRAIDConfigRes_T   AMIGetRAIDConfigRes;


    memset(&AMIGetRAIDConfigReq, 0, sizeof(AMIGetRAIDConfigReq));
    memset(&AMIGetRAIDConfigRes, 0, sizeof(AMIGetRAIDConfigRes));

    AMIGetRAIDConfigReq.Param = GET_CONTROLLER_BBU_INFO;
    AMIGetRAIDConfigReq.ParamData.CtrlID = CtrlID;
    ReqLen = 3 + sizeof(CtrlID);

    wRet = IPMICMD_AMIGetRAIDConfig(pSession,(INT8U*)&AMIGetRAIDConfigReq,ReqLen,(INT8U*)&AMIGetRAIDConfigRes,timeout);
    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TINFO("Error while getting the RAID Controller's ( %d ) BBU Info: Error %x", CtrlID, wRet);
    }

    memcpy(pBBUInfo,&AMIGetRAIDConfigRes.ParamData.BBUInfo, sizeof(BBUInfo_T));
    return wRet;

}

uint16 LIBIPMI_HL_AMIGetRAIDEventLogEntry(IPMI20_SESSION_T *pSession, INT32U RecID, RAIDEvtRcd_T *pEvtRcd, int timeout)
{
    uint16 wRet, ReqLen;
    AMIGetRAIDConfigReq_T   AMIGetRAIDConfigReq;
    AMIGetRAIDConfigRes_T   AMIGetRAIDConfigRes;


    memset(&AMIGetRAIDConfigReq, 0, sizeof(AMIGetRAIDConfigReq));
    memset(&AMIGetRAIDConfigRes, 0, sizeof(AMIGetRAIDConfigRes));

    AMIGetRAIDConfigReq.Param = GET_RAID_EVENT_RECORD;
    AMIGetRAIDConfigReq.ParamData.GetEvt.RecID = RecID;
    ReqLen = 3 + sizeof(GetEvtRec_T);

    wRet = IPMICMD_AMIGetRAIDConfig(pSession,(INT8U*)&AMIGetRAIDConfigReq,ReqLen,(INT8U*)&AMIGetRAIDConfigRes,timeout);
    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TINFO("Error while getting the RAID Controller's Event Record ID ( %d ): Error %x", RecID, wRet);
    }

    memcpy(pEvtRcd,&AMIGetRAIDConfigRes.ParamData.RAIDEvt, sizeof(RAIDEvtRcd_T));

    return wRet;
}

uint16 LIBIPMI_HL_AMIClearRAIDEventLog(IPMI20_SESSION_T *pSession, int timeout)
{
    uint16 wRet, ReqLen;
    AMIGetRAIDConfigReq_T   AMIGetRAIDConfigReq;
    AMIGetRAIDConfigRes_T   AMIGetRAIDConfigRes;


    memset(&AMIGetRAIDConfigReq, 0, sizeof(AMIGetRAIDConfigReq));
    memset(&AMIGetRAIDConfigRes, 0, sizeof(AMIGetRAIDConfigRes));

    AMIGetRAIDConfigReq.Param = CLEAR_RAID_EVENT_LOG;
    ReqLen = 3 ;

    wRet = IPMICMD_AMIGetRAIDConfig(pSession,(INT8U*)&AMIGetRAIDConfigReq,ReqLen,(INT8U*)&AMIGetRAIDConfigRes,timeout);
    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TINFO("Error while Clearing the RAID Controller's Event Record: Error %x", wRet);
    }

    return wRet;
}

uint16 LIBIPMI_HL_AMIGetRAIDEvtReposInfo(IPMI20_SESSION_T *pSession, RAIDRepos_T *pRAIDReposInfo, int timeout)
{
    uint16 wRet, ReqLen;
    AMIGetRAIDConfigReq_T   AMIGetRAIDConfigReq;
    AMIGetRAIDConfigRes_T   AMIGetRAIDConfigRes;


    memset(&AMIGetRAIDConfigReq, 0, sizeof(AMIGetRAIDConfigReq));
    memset(&AMIGetRAIDConfigRes, 0, sizeof(AMIGetRAIDConfigRes));

    AMIGetRAIDConfigReq.Param = GET_RAID_EVENT_REPO_INFO;
    ReqLen = 3 ;

    wRet = IPMICMD_AMIGetRAIDConfig(pSession,(INT8U*)&AMIGetRAIDConfigReq,ReqLen,(INT8U*)&AMIGetRAIDConfigRes,timeout);
    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TINFO("Error while getting the RAID Controller's Event Repository Info: Error %x",  wRet);
    }

    memcpy(pRAIDReposInfo,&AMIGetRAIDConfigRes.ParamData.RAIDReposInfo, sizeof(RAIDRepos_T));

    return wRet;
}

uint16 LIBIPMI_HL_SetController_Properties(IPMI20_SESSION_T *pSession,INT32U CtrlID,ControllerProps_T config_data,int ConfigSize, int timeout)
{
    uint16 wRet = 1, ReqLen;
    AMIGetRAIDConfigReq_T   AMIGetRAIDConfigReq;
    AMIGetRAIDConfigRes_T   AMIGetRAIDConfigRes;

    memset(&AMIGetRAIDConfigReq, 0, sizeof(AMIGetRAIDConfigReq));
    memset(&AMIGetRAIDConfigRes, 0, sizeof(AMIGetRAIDConfigRes));

    AMIGetRAIDConfigReq.Param = PARAM_MANAGE_CONTROLLER_PROPERTIES;
    AMIGetRAIDConfigReq.ParamData.SetControlerProps.CtrlID = CtrlID;
    AMIGetRAIDConfigReq.ParamData.SetControlerProps.operation = SET_PROP;
    memcpy(&(AMIGetRAIDConfigReq.ParamData.SetControlerProps.controllerProperties),(const void *) &config_data, ConfigSize);
    ReqLen = 3 + sizeof(SetControlerProps_T);

    wRet = IPMICMD_AMIGetRAIDConfig(pSession,(INT8U*)&AMIGetRAIDConfigReq, ReqLen,(INT8U*)&AMIGetRAIDConfigRes, timeout);
    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TINFO("Error while executing create logical device command\n", wRet);
    }

    return wRet;
}

uint16 LIBIPMI_HL_GetController_Properties(IPMI20_SESSION_T *pSession,INT32U CtrlID,GetControllerProps_T *pRes, int timeout)
{
    uint16 wRet = 1, ReqLen;
    AMIGetRAIDConfigReq_T   AMIGetRAIDConfigReq;
    AMIGetRAIDConfigRes_T   AMIGetRAIDConfigRes;

    memset(&AMIGetRAIDConfigReq, 0, sizeof(AMIGetRAIDConfigReq));
    memset(&AMIGetRAIDConfigRes, 0, sizeof(AMIGetRAIDConfigRes));

    AMIGetRAIDConfigReq.Param = PARAM_MANAGE_CONTROLLER_PROPERTIES;
    AMIGetRAIDConfigReq.ParamData.SetControlerProps.CtrlID = CtrlID;
    AMIGetRAIDConfigReq.ParamData.SetControlerProps.operation = GET_PROP;
    ReqLen = 3 + sizeof(SetControlerProps_T)-sizeof(ControllerProps_T);

    wRet = IPMICMD_AMIGetRAIDConfig(pSession,(INT8U*)&AMIGetRAIDConfigReq, ReqLen,(INT8U*)&AMIGetRAIDConfigRes, timeout);
    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TINFO("Error while executing create logical device command\n", wRet);
    }
    memcpy(pRes,&AMIGetRAIDConfigRes.ParamData.GetControllerProps, sizeof(ControllerProps_T));

    return wRet;
}

uint16 LIBIPMI_HL_AMIRAIDStartInitVirDisk(IPMI20_SESSION_T *pSession,INT32U ctrlID,INT16U devID,INT8U Init_Type,int timeout)
{
    uint16 wRet = 1, ReqLen;
    AMIGetRAIDConfigReq_T   AMIGetRAIDConfigReq;
    AMIGetRAIDConfigRes_T   AMIGetRAIDConfigRes;

    memset(&AMIGetRAIDConfigReq, 0, sizeof(AMIGetRAIDConfigReq));
    memset(&AMIGetRAIDConfigRes, 0, sizeof(AMIGetRAIDConfigRes));

    AMIGetRAIDConfigReq.Param = PARAM_INITIALIZE_VIRTUAL_DISK;
    AMIGetRAIDConfigReq.ParamData.InitializationLD.CtrlID= ctrlID;
    AMIGetRAIDConfigReq.ParamData.InitializationLD.DevID= devID;
    AMIGetRAIDConfigReq.ParamData.InitializationLD.operation= START_RAID;
    AMIGetRAIDConfigReq.ParamData.InitializationLD.Init_Type= Init_Type;

    ReqLen = 3 + sizeof(InitializationLD_T);


    wRet = IPMICMD_AMIGetRAIDConfig(pSession,(INT8U*)&AMIGetRAIDConfigReq, ReqLen,(INT8U*)&AMIGetRAIDConfigRes, timeout);
    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TINFO("Error while executing create logical device command\n", wRet);
    }

    return wRet;
}

uint16 LIBIPMI_HL_AMIRAIDCancelInitVirDisk(IPMI20_SESSION_T *pSession,INT32U ctrlID,INT16U devID, int timeout)
{
    uint16 wRet = 1, ReqLen;
    AMIGetRAIDConfigReq_T   AMIGetRAIDConfigReq;
    AMIGetRAIDConfigRes_T   AMIGetRAIDConfigRes;

    memset(&AMIGetRAIDConfigReq, 0, sizeof(AMIGetRAIDConfigReq));
    memset(&AMIGetRAIDConfigRes, 0, sizeof(AMIGetRAIDConfigRes));

    AMIGetRAIDConfigReq.Param = PARAM_INITIALIZE_VIRTUAL_DISK;
    AMIGetRAIDConfigReq.ParamData.InitializationLD.CtrlID= ctrlID;
    AMIGetRAIDConfigReq.ParamData.InitializationLD.DevID= devID;
    AMIGetRAIDConfigReq.ParamData.InitializationLD.operation= STOP_RAID;

    ReqLen = 3 + sizeof(InitializationLD_T) - sizeof(INT8U);


    wRet = IPMICMD_AMIGetRAIDConfig(pSession,(INT8U*)&AMIGetRAIDConfigReq, ReqLen,(INT8U*)&AMIGetRAIDConfigRes, timeout);
    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TINFO("Error while executing create logical device command\n", wRet);
    }

    return wRet;
}

uint16 LIBIPMI_HL_AMIRAIDCancelConsistancyCheck(IPMI20_SESSION_T *pSession,INT32U CtrlID,INT16U DevID, int timeout)
{
    uint16 wRet = 1, ReqLen;
    AMIGetRAIDConfigReq_T   AMIGetRAIDConfigReq;
    AMIGetRAIDConfigRes_T   AMIGetRAIDConfigRes;

    memset(&AMIGetRAIDConfigReq, 0, sizeof(AMIGetRAIDConfigReq));
    memset(&AMIGetRAIDConfigRes, 0, sizeof(AMIGetRAIDConfigRes));

    AMIGetRAIDConfigReq.Param = PARAM_CONSISTENCY_CHECK;
    AMIGetRAIDConfigReq.ParamData.ConsistencyCheck.CtrlID= CtrlID;
    AMIGetRAIDConfigReq.ParamData.ConsistencyCheck.DeviceId= DevID;

    AMIGetRAIDConfigReq.ParamData.ConsistencyCheck.operation= STOP_RAID;
    ReqLen = 3 + sizeof(Patrolopt_T);

    wRet = IPMICMD_AMIGetRAIDConfig(pSession,(INT8U*)&AMIGetRAIDConfigReq, ReqLen,(INT8U*)&AMIGetRAIDConfigRes, timeout);
    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TINFO("Error while executing create logical device command\n", wRet);
    }

    return wRet;
}

uint16 LIBIPMI_HL_AMIRAIDStartConsistancyCheck(IPMI20_SESSION_T *pSession,INT32U CtrlID,INT16U DevID, int timeout)
{
    uint16 wRet = 1, ReqLen;
    AMIGetRAIDConfigReq_T   AMIGetRAIDConfigReq;
    AMIGetRAIDConfigRes_T   AMIGetRAIDConfigRes;

    memset(&AMIGetRAIDConfigReq, 0, sizeof(AMIGetRAIDConfigReq));
    memset(&AMIGetRAIDConfigRes, 0, sizeof(AMIGetRAIDConfigRes));

    AMIGetRAIDConfigReq.Param = PARAM_CONSISTENCY_CHECK;
    AMIGetRAIDConfigReq.ParamData.ConsistencyCheck.CtrlID= CtrlID;
    AMIGetRAIDConfigReq.ParamData.ConsistencyCheck.DeviceId= DevID;

    AMIGetRAIDConfigReq.ParamData.ConsistencyCheck.operation= START_RAID;
    ReqLen = 3 + sizeof(Patrolopt_T);

    wRet = IPMICMD_AMIGetRAIDConfig(pSession,(INT8U*)&AMIGetRAIDConfigReq, ReqLen,(INT8U*)&AMIGetRAIDConfigRes, timeout);
    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TINFO("Error while executing create logical device command\n", wRet);
    }

    return wRet;
}

uint16 LIBIPMI_HL_AMIRAIDPatrolRead(IPMI20_SESSION_T *pSession,INT32U CtrlID,INT8U action, int timeout)
{
    uint16 wRet = 1, ReqLen;
    AMIGetRAIDConfigReq_T   AMIGetRAIDConfigReq;
    AMIGetRAIDConfigRes_T   AMIGetRAIDConfigRes;

    memset(&AMIGetRAIDConfigReq, 0, sizeof(AMIGetRAIDConfigReq));
    memset(&AMIGetRAIDConfigRes, 0, sizeof(AMIGetRAIDConfigRes));

    AMIGetRAIDConfigReq.Param = PARAM_PATROL_READ;
    AMIGetRAIDConfigReq.ParamData.PatrolRead.Patrolopt.CtrlID= CtrlID;
    AMIGetRAIDConfigReq.ParamData.PatrolRead.Patrolopt.Action = action;
    ReqLen = 3 + sizeof(Patrolopt_T);

    wRet = IPMICMD_AMIGetRAIDConfig(pSession,(INT8U*)&AMIGetRAIDConfigReq, ReqLen,(INT8U*)&AMIGetRAIDConfigRes, timeout);
    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TINFO("Error while executing create logical device command\n", wRet);
    }

    return wRet;
}

uint16 LIBIPMI_HL_Set_Patrol_Read_Properties(IPMI20_SESSION_T *pSession,INT32U CtrlID,INT8U mode, PatrolReadPropertiesRes_T *pRes, int timeout)
{
    uint16 wRet = 1, ReqLen;
    AMIGetRAIDConfigReq_T   AMIGetRAIDConfigReq;
    AMIGetRAIDConfigRes_T   AMIGetRAIDConfigRes;

    memset(&AMIGetRAIDConfigReq, 0, sizeof(AMIGetRAIDConfigReq));
    memset(&AMIGetRAIDConfigRes, 0, sizeof(AMIGetRAIDConfigRes));

    AMIGetRAIDConfigReq.Param = PARAM_PATROL_READ_PROPERTIES;
    AMIGetRAIDConfigReq.ParamData.PatrolReadProperties.CtrlID=CtrlID;
    AMIGetRAIDConfigReq.ParamData.PatrolReadProperties.operation=SET_PROP;
    AMIGetRAIDConfigReq.ParamData.PatrolReadProperties.mode=mode;
    ReqLen = 3 + sizeof(PatrolReadPropertiesReq_T);

    wRet = IPMICMD_AMIGetRAIDConfig(pSession,(INT8U*)&AMIGetRAIDConfigReq, ReqLen,(INT8U*)&AMIGetRAIDConfigRes, timeout);
    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TINFO("Error while executing create logical device command\n", wRet);
    }
    if(0)
    {
        pRes=pRes; /*-Wextra flag: Unused parameter.*/
    }
    return wRet;
}

uint16 LIBIPMI_HL_Get_Patrol_Read_Properties(IPMI20_SESSION_T *pSession,INT32U CtrlID, PatrolReadPropertiesRes_T *pRes, int timeout)
{
    uint16 wRet = 1, ReqLen;
    AMIGetRAIDConfigReq_T   AMIGetRAIDConfigReq;
    AMIGetRAIDConfigRes_T   AMIGetRAIDConfigRes;

    memset(&AMIGetRAIDConfigReq, 0, sizeof(AMIGetRAIDConfigReq));
    memset(&AMIGetRAIDConfigRes, 0, sizeof(AMIGetRAIDConfigRes));

    AMIGetRAIDConfigReq.Param = PARAM_PATROL_READ_PROPERTIES;
    AMIGetRAIDConfigReq.ParamData.PatrolReadProperties.CtrlID=CtrlID;
    AMIGetRAIDConfigReq.ParamData.PatrolReadProperties.operation=GET_PROP;
    ReqLen = 3 + sizeof(PatrolReadPropertiesReq_T)-sizeof(INT8U);

    wRet = IPMICMD_AMIGetRAIDConfig(pSession,(INT8U*)&AMIGetRAIDConfigReq, ReqLen,(INT8U*)&AMIGetRAIDConfigRes, timeout);
    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TINFO("Error while executing create logical device command\n", wRet);
    }
    memcpy(pRes,&AMIGetRAIDConfigRes.ParamData.PatrolReadProperties, sizeof(PatrolReadPropertiesRes_T));

    return wRet;
}

uint16 LIBIPMI_HL_AMIRAIDDedicatedHotspare(IPMI20_SESSION_T *pSession, DedicatedHotspare_T ld_config_data, int ConfigSize, int timeout)
{
    uint16 wRet = 1, ReqLen;
    AMIGetRAIDConfigReq_T   AMIGetRAIDConfigReq;
    AMIGetRAIDConfigRes_T   AMIGetRAIDConfigRes;

    memset(&AMIGetRAIDConfigReq, 0, sizeof(AMIGetRAIDConfigReq));
    memset(&AMIGetRAIDConfigRes, 0, sizeof(AMIGetRAIDConfigRes));

    AMIGetRAIDConfigReq.Param = PARAM_MAKE_DEDICATED_HOT_SPARE;
    memcpy(&(AMIGetRAIDConfigReq.ParamData.DedicatedHotspare),(const void *) &ld_config_data, ConfigSize);
    ReqLen = 3 + ConfigSize;

    wRet = IPMICMD_AMIGetRAIDConfig(pSession,(INT8U*)&AMIGetRAIDConfigReq, ReqLen,(INT8U*)&AMIGetRAIDConfigRes, timeout);
    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TINFO("Error while executing create logical device command\n", wRet);
    }

    return wRet;
}

uint16 LIBIPMI_HL_AMIRAIDStartPhyLocateDevice(IPMI20_SESSION_T *pSession,INT32U ctrlID,INT16U devID,INT8U duration,int timeout)
{
    uint16 wRet = 1, ReqLen;
    AMIGetRAIDConfigReq_T   AMIGetRAIDConfigReq;
    AMIGetRAIDConfigRes_T   AMIGetRAIDConfigRes;

    memset(&AMIGetRAIDConfigReq, 0, sizeof(AMIGetRAIDConfigReq));
    memset(&AMIGetRAIDConfigRes, 0, sizeof(AMIGetRAIDConfigRes));

    AMIGetRAIDConfigReq.Param = PARAM_LOCATE_DEVICE;
    AMIGetRAIDConfigReq.ParamData.LocateDevice.CtrlID =ctrlID;
    AMIGetRAIDConfigReq.ParamData.LocateDevice.DeviceId=devID;
    AMIGetRAIDConfigReq.ParamData.LocateDevice.DeviceType=PHY_DRIVE;
    AMIGetRAIDConfigReq.ParamData.LocateDevice.Action = START_LOCATE;
    AMIGetRAIDConfigReq.ParamData.LocateDevice.Duration = duration;
    ReqLen = 3 + sizeof(LocateDevice_T);

    wRet = IPMICMD_AMIGetRAIDConfig(pSession,(INT8U*)&AMIGetRAIDConfigReq, ReqLen,(INT8U*)&AMIGetRAIDConfigRes, timeout);
    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TINFO("Error while executing create logical device command\n", wRet);
    }

    return wRet;
}

uint16 LIBIPMI_HL_AMIRAIDStopPhyLocateDevice(IPMI20_SESSION_T *pSession,INT32U ctrlID,INT16U devID,int timeout)
{
    uint16 wRet = 1, ReqLen;
    AMIGetRAIDConfigReq_T   AMIGetRAIDConfigReq;
    AMIGetRAIDConfigRes_T   AMIGetRAIDConfigRes;

    memset(&AMIGetRAIDConfigReq, 0, sizeof(AMIGetRAIDConfigReq));
    memset(&AMIGetRAIDConfigRes, 0, sizeof(AMIGetRAIDConfigRes));

    AMIGetRAIDConfigReq.Param = PARAM_LOCATE_DEVICE;
    AMIGetRAIDConfigReq.ParamData.LocateDevice.CtrlID =ctrlID;
    AMIGetRAIDConfigReq.ParamData.LocateDevice.DeviceId=devID;
    AMIGetRAIDConfigReq.ParamData.LocateDevice.DeviceType=PHY_DRIVE;
    AMIGetRAIDConfigReq.ParamData.LocateDevice.Action = STOP_LOCATE;
    ReqLen = 3 + sizeof(LocateDevice_T)-sizeof(INT8U);

    wRet = IPMICMD_AMIGetRAIDConfig(pSession,(INT8U*)&AMIGetRAIDConfigReq, ReqLen,(INT8U*)&AMIGetRAIDConfigRes, timeout);
    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TINFO("Error while executing create logical device command\n", wRet);
    }

    return wRet;
}

uint16 LIBIPMI_HL_AMIRAIDStartVirLocateDevice(IPMI20_SESSION_T *pSession,INT32U ctrlID,INT16U devID,INT8U duration,int timeout)
{
    uint16 wRet = 1, ReqLen;
    AMIGetRAIDConfigReq_T   AMIGetRAIDConfigReq;
    AMIGetRAIDConfigRes_T   AMIGetRAIDConfigRes;

    memset(&AMIGetRAIDConfigReq, 0, sizeof(AMIGetRAIDConfigReq));
    memset(&AMIGetRAIDConfigRes, 0, sizeof(AMIGetRAIDConfigRes));

    AMIGetRAIDConfigReq.Param = PARAM_LOCATE_DEVICE;
    AMIGetRAIDConfigReq.ParamData.LocateDevice.CtrlID =ctrlID;
    AMIGetRAIDConfigReq.ParamData.LocateDevice.DeviceId=devID;
    AMIGetRAIDConfigReq.ParamData.LocateDevice.DeviceType=VIR_DRIVE;
    AMIGetRAIDConfigReq.ParamData.LocateDevice.Action = START_LOCATE;
    AMIGetRAIDConfigReq.ParamData.LocateDevice.Duration = duration;
    ReqLen = 3 + sizeof(LocateDevice_T);

    wRet = IPMICMD_AMIGetRAIDConfig(pSession,(INT8U*)&AMIGetRAIDConfigReq, ReqLen,(INT8U*)&AMIGetRAIDConfigRes, timeout);
    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TINFO("Error while executing create logical device command\n", wRet);
    }

    return wRet;
}

uint16 LIBIPMI_HL_AMIRAIDStopVirLocateDevice(IPMI20_SESSION_T *pSession,INT32U ctrlID,INT16U devID,int timeout)
{
    uint16 wRet = 1, ReqLen;
    AMIGetRAIDConfigReq_T   AMIGetRAIDConfigReq;
    AMIGetRAIDConfigRes_T   AMIGetRAIDConfigRes;

    memset(&AMIGetRAIDConfigReq, 0, sizeof(AMIGetRAIDConfigReq));
    memset(&AMIGetRAIDConfigRes, 0, sizeof(AMIGetRAIDConfigRes));

    AMIGetRAIDConfigReq.Param = PARAM_LOCATE_DEVICE;
    AMIGetRAIDConfigReq.ParamData.LocateDevice.CtrlID =ctrlID;
    AMIGetRAIDConfigReq.ParamData.LocateDevice.DeviceId=devID;
    AMIGetRAIDConfigReq.ParamData.LocateDevice.DeviceType=VIR_DRIVE;
    AMIGetRAIDConfigReq.ParamData.LocateDevice.Action = STOP_LOCATE;
    ReqLen = 3 + sizeof(LocateDevice_T)-sizeof(INT8U);

    wRet = IPMICMD_AMIGetRAIDConfig(pSession,(INT8U*)&AMIGetRAIDConfigReq, ReqLen,(INT8U*)&AMIGetRAIDConfigRes, timeout);
    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TINFO("Error while executing create logical device command\n", wRet);
    }

    return wRet;
}

uint16 LIBIPMI_HL_SET_PD_State(IPMI20_SESSION_T *pSession,INT32U CtrlID,INT16U DevID,INT8U state, int timeout)
{
    uint16 wRet = 1, ReqLen;
    AMIGetRAIDConfigReq_T   AMIGetRAIDConfigReq;
    AMIGetRAIDConfigRes_T   AMIGetRAIDConfigRes;

    memset(&AMIGetRAIDConfigReq, 0, sizeof(AMIGetRAIDConfigReq));
    memset(&AMIGetRAIDConfigRes, 0, sizeof(AMIGetRAIDConfigRes));

    AMIGetRAIDConfigReq.Param = PARAM_SET_PD_STATE;
    AMIGetRAIDConfigReq.ParamData.SetPDState.CtrlID=CtrlID;
    AMIGetRAIDConfigReq.ParamData.SetPDState.DeviceId=DevID;
    AMIGetRAIDConfigReq.ParamData.SetPDState.state=state;
    ReqLen = 3 + sizeof(SetPDState_T);

    wRet = IPMICMD_AMIGetRAIDConfig(pSession,(INT8U*)&AMIGetRAIDConfigReq, ReqLen,(INT8U*)&AMIGetRAIDConfigRes, timeout);
    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TINFO("Error while executing create logical device command\n", wRet);
    }

    return wRet;
}

uint16 LIBIPMI_HL_SetLogicalDev_Properties(IPMI20_SESSION_T *pSession,INT32U ctrlID,INT16U devID, LDproperties_T ld_config_data, int ConfigSize, int timeout)
{
    uint16 wRet = 1, ReqLen;
    AMIGetRAIDConfigReq_T   AMIGetRAIDConfigReq;
    AMIGetRAIDConfigRes_T   AMIGetRAIDConfigRes;
    memset(&AMIGetRAIDConfigReq, 0, sizeof(AMIGetRAIDConfigReq));
    memset(&AMIGetRAIDConfigRes, 0, sizeof(AMIGetRAIDConfigRes));

    AMIGetRAIDConfigReq.Param = PARAM_MANAGE_LD_PROPERTIES;
    AMIGetRAIDConfigReq.ParamData.LDpropertiesReq.CtrlID=ctrlID;
    AMIGetRAIDConfigReq.ParamData.LDpropertiesReq.devId=devID;
    memcpy(&(AMIGetRAIDConfigReq.ParamData.LDpropertiesReq.LDproperties),(const void *) &ld_config_data, ConfigSize);
    ReqLen = 3 + ConfigSize;

    wRet = IPMICMD_AMIGetRAIDConfig(pSession,(INT8U*)&AMIGetRAIDConfigReq, ReqLen,(INT8U*)&AMIGetRAIDConfigRes, timeout);
    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TINFO("Error while executing create logical device command\n", wRet);
    }
    return wRet;
}

uint16 LIBIPMI_HL_GetLogicalDev_Properties(IPMI20_SESSION_T *pSession,INT32U ctrlID,INT16U devID,LDpropertiesRes_T *pRes, int timeout)
{
    uint16 wRet = 1, ReqLen;
    AMIGetRAIDConfigReq_T   AMIGetRAIDConfigReq;
    AMIGetRAIDConfigRes_T   AMIGetRAIDConfigRes;
    memset(&AMIGetRAIDConfigReq, 0, sizeof(AMIGetRAIDConfigReq));
    memset(&AMIGetRAIDConfigRes, 0, sizeof(AMIGetRAIDConfigRes));

    AMIGetRAIDConfigReq.Param = PARAM_MANAGE_LD_PROPERTIES;
    AMIGetRAIDConfigReq.ParamData.LDpropertiesReq.CtrlID=ctrlID;
    AMIGetRAIDConfigReq.ParamData.LDpropertiesReq.devId=devID;
    AMIGetRAIDConfigReq.ParamData.LDpropertiesReq.operation = GET_PROP;
    ReqLen = 3 + sizeof(LDpropertiesReq_T)-sizeof(LDproperties_T);

    wRet = IPMICMD_AMIGetRAIDConfig(pSession,(INT8U*)&AMIGetRAIDConfigReq, ReqLen,(INT8U*)&AMIGetRAIDConfigRes, timeout);
    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TINFO("Error while executing create logical device command\n", wRet);
    }
    memcpy(pRes,&AMIGetRAIDConfigRes.ParamData.LDpropertiesRes, sizeof(LDpropertiesRes_T));
    return wRet;
}

uint16 LIBIPMI_HL_GetLogicalDev_Progress(IPMI20_SESSION_T *pSession,INT32U CtrlID,INT16U DevID,LDProgressRes_T *pRes, int timeout)
{
    uint16 wRet = 1, ReqLen;
    AMIGetRAIDConfigReq_T   AMIGetRAIDConfigReq;
    AMIGetRAIDConfigRes_T   AMIGetRAIDConfigRes;

    memset(&AMIGetRAIDConfigReq, 0, sizeof(AMIGetRAIDConfigReq));
    memset(&AMIGetRAIDConfigRes, 0, sizeof(AMIGetRAIDConfigRes));

    AMIGetRAIDConfigReq.Param = PARAM_GET_LD_PROGRESS;
    AMIGetRAIDConfigReq.ParamData.DevInfo.CtrlID = CtrlID;
    AMIGetRAIDConfigReq.ParamData.DevInfo.DevID = DevID;

    ReqLen = 3 + sizeof(Device_T);

    wRet = IPMICMD_AMIGetRAIDConfig(pSession,(INT8U*)&AMIGetRAIDConfigReq, ReqLen,(INT8U*)&AMIGetRAIDConfigRes, timeout);
    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TINFO("Error while executing create logical device command\n", wRet);
    }
    memcpy(pRes,&AMIGetRAIDConfigRes.ParamData.LDProgress, sizeof(LDProgressRes_T));

    return wRet;
}

uint16 LIBIPMI_HL_AMIGetAllRAIDEventLog (IPMI20_SESSION_T *pSession,
                RAIDEvtRcd_T *pEvtRecData,
                uint32  *nNumRAIDEvtEntries,
                int timeout)
{
    uint16 wRet, ReqLen, EntryOffset;
    AMIGetRAIDConfigReq_T   AMIGetRAIDConfigReq;
    AMIGetRAIDConfigRes_T   *pAMIGetRAIDConfigRes = NULL;
    uint8 *pEvtptr = NULL;
    uint8 *pRAIDEvtEntries = NULL, *pRAIDEvtData = NULL;


    memset(&AMIGetRAIDConfigReq, 0, sizeof(AMIGetRAIDConfigReq));

    if(!pEvtRecData)
    {
        wRet = 0xFF;
        goto EXIT;
    }

    pRAIDEvtEntries = (uint8 *)calloc(MAX_RAID_EVENT_SIZE, sizeof(uint8));
    if(pRAIDEvtEntries == NULL)
    {
        TCRIT("Memory allocation error in LIBIPMI_HL_AMIGetSELEntires \n");
        wRet = 0xFF;
        goto EXIT;
    }


    pEvtptr = (uint8*)pEvtRecData;
    pAMIGetRAIDConfigRes = (AMIGetRAIDConfigRes_T *)pRAIDEvtEntries;

    AMIGetRAIDConfigReq.Param = GET_RAID_ALL_EVENT_RECORD;
    ReqLen = 3 + sizeof(AMIGetRAIDConfigReq.ParamData.Noofentryretrieved);
    EntryOffset = sizeof(RAIDGetAllEntry_T) + 1;
    do
    {
        wRet = IPMICMD_AMIGetRAIDConfig(pSession,(INT8U*)&AMIGetRAIDConfigReq,ReqLen,(INT8U*)pAMIGetRAIDConfigRes,timeout);
        pRAIDEvtData = (uint8*)(pRAIDEvtEntries + EntryOffset);
        if(wRet == LIBIPMI_E_SUCCESS)
        {
            memcpy(pEvtptr, pRAIDEvtData, (pAMIGetRAIDConfigRes->ParamData.RAIDGetAllEntry.Noofentries * sizeof(RAIDEvtRcd_T)));
            pEvtptr = pEvtptr + (pAMIGetRAIDConfigRes->ParamData.RAIDGetAllEntry.Noofentries * sizeof(RAIDEvtRcd_T));


            if(pAMIGetRAIDConfigRes->ParamData.RAIDGetAllEntry.Status == FULL_RAID_EVENT)
            {
                *nNumRAIDEvtEntries += pAMIGetRAIDConfigRes->ParamData.RAIDGetAllEntry.Noofentries;
                break;
            }
            else
            {
                *nNumRAIDEvtEntries += pAMIGetRAIDConfigRes->ParamData.RAIDGetAllEntry.Noofentries;
                AMIGetRAIDConfigReq.ParamData.Noofentryretrieved = *nNumRAIDEvtEntries;
                memset(pRAIDEvtEntries, 0, MAX_RAID_EVENT_SIZE);
            }
        }
        else if((wRet & 0xFF) == LIBIPMI_E_RAID_EVENT_INVALID_ENTRY)
        {
            memset(pRAIDEvtEntries, 0, MAX_RAID_EVENT_SIZE);
            memset(pEvtRecData, 0, sizeof(RAIDEvtRcd_T)* *nNumRAIDEvtEntries);
            *nNumRAIDEvtEntries =  AMIGetRAIDConfigReq.ParamData.Noofentryretrieved = 0;
            goto EXIT;
        }
        else if((wRet & 0xFF)  == LIBIPMI_E_RAID_EVENT_EMPTY)
        {
            *nNumRAIDEvtEntries = 0;
            wRet = LIBIPMI_E_SUCCESS;
            goto EXIT;
        }
        else
        {
            break;
        }

    }while(1);

EXIT:
    if(pRAIDEvtEntries)
    {
        free(pRAIDEvtEntries);
        pRAIDEvtEntries = NULL;
    }

    return wRet;
}

uint16 IPMICMD_AMIGetFirmwareRecoveryInfo(IPMI20_SESSION_T *pSession,
                AMIGetRecoveryInfoReq_T *pAMIGetRecoveryInfoReq,
                AMIGetRecoveryInfoRes_T *pAMIGetRecoveryInfoRes,
                INT8U ReqLen, int timeout)
{
    uint16 wRet;
    uint32 wResLen = 0;
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                ((NETFN_AMI << 2) + 0x00), CMD_AMI_GET_FIRMWARE_RECOVERY_INFO,
                (uint8 *)pAMIGetRecoveryInfoReq,ReqLen ,
                (uint8 *)pAMIGetRecoveryInfoRes, &wResLen, timeout );
    return wRet;
}

uint16 IPMICMD_AMISetFirmwareRecoveryInfo(IPMI20_SESSION_T *pSession,
                AMISetRecoveryInfoReq_T *pAMISetRecoveryInfoReq,
                AMISetRecoveryInfoRes_T *pAMISetRecoveryInfoRes,
                INT8U ReqLen, int timeout)
{
    uint16 wRet;
    uint32 wResLen = 0;
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                ((NETFN_AMI << 2) + 0x00), CMD_AMI_SET_FIRMWARE_RECOVERY_INFO,
                (uint8 *)pAMISetRecoveryInfoReq,ReqLen,
                (uint8 *)pAMISetRecoveryInfoRes, &wResLen, timeout );
    return wRet;
}

uint16 LIBIPMI_HL_AMIGetFirmwareRecoveryInfo(IPMI20_SESSION_T *pSession, FIRMWARERECOVERYINFO * FirmwareRecoveryInfo, int timeout)
{
    AMIGetRecoveryInfoReq_T GetRecoveryInfoReq;
    AMIGetRecoveryInfoRes_T GetRecoveryInfoRes;
    uint16 wRet = -1;
    int param = 0;

    for(param = AMI_FORCE_RECOVERY; param <= AMI_BOOT_RETRY; param++)
    {
        memset(&GetRecoveryInfoReq, 0, sizeof(AMIGetRecoveryInfoReq_T));
        memset(&GetRecoveryInfoRes, 0, sizeof(AMIGetRecoveryInfoRes_T));
        GetRecoveryInfoReq.Param = param;
        GetRecoveryInfoReq.BlockSelector = 0;
        wRet = IPMICMD_AMIGetFirmwareRecoveryInfo(pSession, &GetRecoveryInfoReq, &GetRecoveryInfoRes, sizeof(AMIGetRecoveryInfoReq_T), timeout);
        if (wRet == CC_SUCCESS)
        {
            switch(param)
            {
                case AMI_FORCE_RECOVERY:
                    FirmwareRecoveryInfo->ForceRecovery = GetRecoveryInfoRes.Config.ForceRecovery;
                    break;

                case AMI_RECOVERY_SERVER_IP:
                    #if LIBIPMI_IS_OS_LINUX()
                    inet_ntop(AF_INET, &(GetRecoveryInfoRes.Config.RecoveryServer), (char*)FirmwareRecoveryInfo->RecoveryServer, sizeof (FirmwareRecoveryInfo->RecoveryServer));
                    #elif LIBIPMI_IS_OS_WINDOWS()
                    InetNtop(AF_INET, &(GetRecoveryInfoRes.Config.RecoveryServer), (char*)FirmwareRecoveryInfo->RecoveryServer, sizeof (FirmwareRecoveryInfo->RecoveryServer));
                    #endif
                    break;

                case AMI_RECOVERY_IMAGE_NAME:
                    strncpy((char *)FirmwareRecoveryInfo->RecoveryImg, (char *)GetRecoveryInfoRes.Config.RecoveryImg, MAX_RECOVERY_IMG_SIZE-1);
                    break;

                case AMI_RECOVERY_RETRY:
                    FirmwareRecoveryInfo->RecoveryRetry = GetRecoveryInfoRes.Config.RecoveryRetry;
                    break;

                case AMI_BOOT_RETRY:
                    FirmwareRecoveryInfo->BootRetry = GetRecoveryInfoRes.Config.BootRetry;
                    break;
            }
        }
        else
        {
            TCRIT("Error in Getting the Firmware recovery info ::%x param %d\n", wRet,param);
            return wRet;
        }
    }
    return wRet;
}

uint16 LIBIPMI_HL_AMISetFirmwareRecoveryInfo(IPMI20_SESSION_T *pSession, FIRMWARERECOVERYINFO * pFirmwareRecoveryInfo,int timeout)
{
    AMISetRecoveryInfoReq_T SetRecoveryInfoReq;
    AMISetRecoveryInfoRes_T SetRecoveryInfoRes;
    uint16 wRet;
    int param = 0,Retval[]={3,6,12,3,3};
    INT8U ReqLen=0;
    FIRMWARERECOVERYINFO FirmwareRecoveryInfo;

    wRet = LIBIPMI_HL_AMIGetFirmwareRecoveryInfo(pSession, &FirmwareRecoveryInfo, timeout);
    if (wRet != CC_SUCCESS)
    {
        TCRIT("Error Getting Firmware Recovery Info ::%x\n", wRet);
        return wRet;
    }

    for(param = AMI_FORCE_RECOVERY; param <= AMI_BOOT_RETRY;param++)
    {
        memset(&SetRecoveryInfoReq, 0, sizeof(AMISetRecoveryInfoReq_T));
        memset(&SetRecoveryInfoRes, 0, sizeof(AMISetRecoveryInfoRes_T));
        SetRecoveryInfoReq.Param = param;
        SetRecoveryInfoReq.BlockSelector = 0;
        ReqLen = Retval[param];
        switch(param)
        {
            case AMI_FORCE_RECOVERY:
                if(pFirmwareRecoveryInfo->ForceRecovery != FirmwareRecoveryInfo.ForceRecovery)
                {
                    SetRecoveryInfoReq.Config.ForceRecovery = pFirmwareRecoveryInfo->ForceRecovery;
                    break;
                }
                else
                    continue;

            case AMI_RECOVERY_SERVER_IP:
                if(((strlen((const char*) pFirmwareRecoveryInfo->RecoveryServer)) && (strncmp((const char*)FirmwareRecoveryInfo.RecoveryServer, (const char*)pFirmwareRecoveryInfo->RecoveryServer, MAX_IPV4STRADDRS))))
                {
                    #if LIBIPMI_IS_OS_LINUX()
                    if (!inet_pton (AF_INET, (const char *)pFirmwareRecoveryInfo->RecoveryServer, (char*)SetRecoveryInfoReq.Config.RecoveryServer))
                    {
                        TCRIT("Invalid IPv4 Address");
                        continue;
                    }
                    #elif LIBIPMI_IS_OS_WINDOWS()
                    InetPton(AF_INET, (const char *)pFirmwareRecoveryInfo->RecoveryServer, (char*)SetRecoveryInfoReq.Config.RecoveryServer);
                    #endif
                    break;
                }
                else
                    continue;

            case AMI_RECOVERY_IMAGE_NAME:
                if((strlen((const char*) pFirmwareRecoveryInfo->RecoveryImg)) && (strncmp((const char*)FirmwareRecoveryInfo.RecoveryImg, (const char*)pFirmwareRecoveryInfo->RecoveryImg, MAX_RECOVERY_IMG_SIZE) != 0))
                {
                    strncpy((char *)SetRecoveryInfoReq.Config.RecoveryImg,(char *)pFirmwareRecoveryInfo->RecoveryImg,MAX_RECOVERY_IMG_SIZE-1);
                    break;
                }
                else
                    continue;

            case AMI_RECOVERY_RETRY:
                if(pFirmwareRecoveryInfo->RecoveryRetry != FirmwareRecoveryInfo.RecoveryRetry)
                {
                    SetRecoveryInfoReq.Config.RecoveryRetry = pFirmwareRecoveryInfo->RecoveryRetry;
                    break;
                }
                else
                    continue;

            case AMI_BOOT_RETRY:
                if(pFirmwareRecoveryInfo->BootRetry != FirmwareRecoveryInfo.BootRetry)
                {
                    SetRecoveryInfoReq.Config.BootRetry = pFirmwareRecoveryInfo->BootRetry;
                    break;
                }
                else
                    continue;
        }
        wRet = IPMICMD_AMISetFirmwareRecoveryInfo(pSession, &SetRecoveryInfoReq, &SetRecoveryInfoRes, ReqLen, timeout);
        if(wRet != 0)
        {
            TCRIT("Error in Setting the Firmware recovery info ::%x param %d\n", wRet,param);
            return wRet;
        }
    }
    return wRet;
}

uint16 IPMICMD_AMISetSerialLogCfg(IPMI20_SESSION_T *pSession,
                  AMISetSerialLogReq_T *pAMISetSerialLogCfgReq,
                  AMISetSerialLogRes_T *pAMISetSerialLogCfgRes,
                  int ReqLen,int timeout)
{
    uint16 wRet;
    uint32 wResLen = 0;
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                ((NETFN_AMI << 2) + 0x00), CMD_AMI_SET_SERIALLOG_CONF,
                                (uint8 *)pAMISetSerialLogCfgReq,ReqLen ,
                                (uint8 *)pAMISetSerialLogCfgRes, &wResLen, timeout );
    return wRet;

}

uint16 LIBIPMI_HL_AMISetSerialLogCfg(IPMI20_SESSION_T *pSession,IPMI_SerialLogCfg_T* SerialLog_config,int timeout)
{
    AMISetSerialLogReq_T SetSerialLogCfgReq;
    AMISetSerialLogRes_T SetSerialLogCfgRes;
    int param;
    uint16 wRet;
    INT8U ReqLen = 0;


    for(param = AMI_SERIALLOG_LOGSIZE; param <= AMI_SERIALLOG_NUMOFLOG;param++)
    {
        memset(&SetSerialLogCfgReq,0,sizeof(AMISetSerialLogReq_T));
        memset(&SetSerialLogCfgRes,0,sizeof(AMISetSerialLogRes_T));
        SetSerialLogCfgReq.ParameterSelector = param;
        SetSerialLogCfgReq.Blockselector=0;
        switch(param)
        {
            case AMI_SERIALLOG_LOGSIZE:
                SetSerialLogCfgReq.SolLogConfig.log_size = SerialLog_config->LogSize;
                ReqLen=(sizeof(AMISetSerialLogReq_T)-sizeof(SOLLogConfig_T)+sizeof(SetSerialLogCfgReq.SolLogConfig.log_size));
                break;
            case AMI_SERIALLOG_NUMOFLOG:
                SetSerialLogCfgReq.SolLogConfig.numoflog= SerialLog_config->NumofLog;
                ReqLen=(sizeof(AMISetSerialLogReq_T)-sizeof(SOLLogConfig_T)+sizeof(SetSerialLogCfgReq.SolLogConfig.numoflog));
                break;
        }
    wRet=IPMICMD_AMISetSerialLogCfg(pSession, &SetSerialLogCfgReq, &SetSerialLogCfgRes,ReqLen, timeout);
    if((wRet!=CC_SUCCESS)&&((wRet & 0xFF)!=CC_MOUNT_FAILED_CFG_SAVE))
       return wRet;
    }
    return 0;
}

uint16 LIBIPMI_HL_AMISetSerialLogRemoteCfg(IPMI20_SESSION_T *pSession,IPMI_SerialLogCfg_T* SerialLog_config,int timeout)
{
    AMISetSerialLogReq_T SetSerialLogCfgReq;
    AMISetSerialLogRes_T SetSerialLogCfgRes;
    int param;
    unsigned int i=0;
    uint16 wRet;
    INT8U ReqLen = 0;
    unsigned int DomainLen = 0;

    for(param = AMI_SERIALLOG_RM_REMOTE_SUPPORT; param <= AMI_SERIALLOG_SERVICE_RESTART;param++)
    {
        memset(&SetSerialLogCfgReq,0,sizeof(AMISetSerialLogReq_T));
        memset(&SetSerialLogCfgRes,0,sizeof(AMISetSerialLogRes_T));
        SetSerialLogCfgReq.ParameterSelector = param;
        SetSerialLogCfgReq.Blockselector=0;
        switch(param)
        {
            case AMI_SERIALLOG_RM_REMOTE_SUPPORT:
                SetSerialLogCfgReq.SolLogConfig.remote_path_support=SerialLog_config->remote_path_support;
                ReqLen=(sizeof(AMISetSerialLogReq_T)-sizeof(SOLLogConfig_T)+sizeof(SetSerialLogCfgReq.SolLogConfig.remote_path_support));
                wRet=IPMICMD_AMISetSerialLogCfg(pSession, &SetSerialLogCfgReq, &SetSerialLogCfgRes,ReqLen, timeout);
                if((wRet!=CC_SUCCESS)&&((wRet & 0xFF)!=CC_MOUNT_FAILED_CFG_SAVE))
                    return wRet;
                if(SerialLog_config->remote_path_support == 0)
                    param = AMI_SERIALLOG_RM_MNT_SUCCESS;
                continue;
            case AMI_SERIALLOG_RM_PATH:
                SetSerialLogCfgReq.SolLogConfig.Progressbit=1;
                ReqLen=(sizeof(AMISetSerialLogReq_T)-sizeof(SOLLogConfig_T)+sizeof(SetSerialLogCfgReq.SolLogConfig.Progressbit));
                wRet=IPMICMD_AMISetSerialLogCfg(pSession,&SetSerialLogCfgReq,&SetSerialLogCfgRes,ReqLen, timeout);
                if((wRet!=CC_SUCCESS)&&((wRet & 0xFF)!=CC_MOUNT_FAILED_CFG_SAVE))
                    return wRet;
                for(i = 0; (i < MAX_MSG_BLOCKS)&&((i*MAX_BLOCK_DATA_SIZE)<strlen(SerialLog_config->path)); i++)
                {
                    SetSerialLogCfgReq.Blockselector = i+1;
                    if(SetSerialLogCfgReq.Blockselector==MAX_MSG_BLOCKS)
                       ReqLen=MAX_BLOCK_DATA_SIZE+1;
                    else
                       ReqLen = MAX_BLOCK_DATA_SIZE+2;
                    memcpy(SetSerialLogCfgReq.SolLogConfig.path,SerialLog_config->path +(i*MAX_BLOCK_DATA_SIZE),MAX_BLOCK_DATA_SIZE);
                    wRet = IPMICMD_AMISetSerialLogCfg(pSession, &SetSerialLogCfgReq, &SetSerialLogCfgRes,ReqLen, timeout);
                    if((wRet!=CC_SUCCESS)&&((wRet & 0xFF)!=CC_MOUNT_FAILED_CFG_SAVE))
                        return wRet;
                }
                SetSerialLogCfgReq.SolLogConfig.Progressbit=0;
                SetSerialLogCfgReq.Blockselector=0;
                ReqLen=(sizeof(AMISetSerialLogReq_T)-sizeof(SOLLogConfig_T)+sizeof(SetSerialLogCfgReq.SolLogConfig.Progressbit));
                break;
            case AMI_SERIALLOG_RM_IP_ADDR:
                if(strlen(SerialLog_config->ip_addr)!=0)
                {
                    strncpy(SetSerialLogCfgReq.SolLogConfig.ip_addr,SerialLog_config->ip_addr,MAX_IP_ADDR_LEN-1);
                    ReqLen=(sizeof(AMISetSerialLogReq_T)-sizeof(SOLLogConfig_T)+MAX_SOLIP_ADDR_LEN-sizeof(INT8U));
                    break;
                }
                else
                    continue;
            case AMI_SERIALLOG_RM_USERNAME:
                SetSerialLogCfgReq.SolLogConfig.Progressbit=1;
                ReqLen=(sizeof(AMISetSerialLogReq_T)-sizeof(SOLLogConfig_T)+sizeof(SetSerialLogCfgReq.SolLogConfig.Progressbit));
                wRet=IPMICMD_AMISetSerialLogCfg(pSession,&SetSerialLogCfgReq,&SetSerialLogCfgRes,ReqLen, timeout);
                if((wRet!=CC_SUCCESS)&&((wRet & 0xFF)!=CC_MOUNT_FAILED_CFG_SAVE))
                    return wRet;
                for(i = 0; (i < MAX_MSG_BLOCKS)&&((i*MAX_BLOCK_DATA_SIZE)<strlen(SerialLog_config->username)); i++)
                {
                   SetSerialLogCfgReq.Blockselector = i+1;
                   if(SetSerialLogCfgReq.Blockselector==MAX_MSG_BLOCKS)
                       ReqLen=MAX_BLOCK_DATA_SIZE+1;
                   else
                       ReqLen = MAX_BLOCK_DATA_SIZE+2;
                   memcpy(SetSerialLogCfgReq.SolLogConfig.username,SerialLog_config->username +(i*MAX_BLOCK_DATA_SIZE),MAX_BLOCK_DATA_SIZE);
                   wRet = IPMICMD_AMISetSerialLogCfg(pSession, &SetSerialLogCfgReq, &SetSerialLogCfgRes,ReqLen, timeout);
                   if((wRet!=CC_SUCCESS)&&((wRet & 0xFF)!=CC_MOUNT_FAILED_CFG_SAVE))
                       return wRet;
                }

                SetSerialLogCfgReq.SolLogConfig.Progressbit=0;
                SetSerialLogCfgReq.Blockselector=0;
                ReqLen=(sizeof(AMISetSerialLogReq_T)-sizeof(SOLLogConfig_T)+sizeof(SetSerialLogCfgReq.SolLogConfig.Progressbit));
                break;
            case AMI_SERIALLOG_RM_PASSWORD:
                strncpy(SetSerialLogCfgReq.SolLogConfig.password,SerialLog_config->password,MAX_RMS_PASSWORD_LEN-1);
                ReqLen=(sizeof(AMISetSerialLogReq_T)-sizeof(SOLLogConfig_T)+MAX_RMS_PASSWORD_LEN-sizeof(INT8U));
                break;
            case AMI_SERIALLOG_RM_SHARE_TYPE:
                strncpy(SetSerialLogCfgReq.SolLogConfig.share_type,SerialLog_config->share_type,MAX_SHARE_TYPE_LEN-1);
                ReqLen=(sizeof(AMISetSerialLogReq_T)-sizeof(SOLLogConfig_T)+MAX_SHARE_TYPE_LEN-sizeof(INT8U));
                break;
            case AMI_SERIALLOG_RM_DOMAIN:
                DomainLen = strlen(SerialLog_config->domain);
                SetSerialLogCfgReq.SolLogConfig.Progressbit=1;
                ReqLen=(sizeof(AMISetSerialLogReq_T)-sizeof(SOLLogConfig_T)+sizeof(SetSerialLogCfgReq.SolLogConfig.Progressbit));
                wRet=IPMICMD_AMISetSerialLogCfg(pSession,&SetSerialLogCfgReq,&SetSerialLogCfgRes,ReqLen,timeout);
                if((wRet!=CC_SUCCESS)&&((wRet & 0xFF)!=CC_MOUNT_FAILED_CFG_SAVE))
                    return wRet;

                for(i=0;(i < MAX_MSG_BLOCKS)&&(((i*MAX_BLOCK_DATA_SIZE)<DomainLen) || (DomainLen == 0) );i++)
                {
                    SetSerialLogCfgReq.Blockselector = i+1;
                    if(SetSerialLogCfgReq.Blockselector==MAX_MSG_BLOCKS)
                        ReqLen=MAX_BLOCK_DATA_SIZE+1;
                    else
                        ReqLen = MAX_BLOCK_DATA_SIZE+2;
                    memset(SetSerialLogCfgReq.SolLogConfig.domain,0,MAX_BLOCK_DATA_SIZE);
                    if(DomainLen > 0)
                    {
                        memcpy(SetSerialLogCfgReq.SolLogConfig.domain,SerialLog_config->domain +(i*MAX_BLOCK_DATA_SIZE),MAX_BLOCK_DATA_SIZE);
                    }
                    wRet = IPMICMD_AMISetSerialLogCfg(pSession, &SetSerialLogCfgReq, &SetSerialLogCfgRes,ReqLen, timeout);
                    if((wRet!=CC_SUCCESS)&&((wRet & 0xFF)!=CC_MOUNT_FAILED_CFG_SAVE))
                       return wRet;
                }
                SetSerialLogCfgReq.SolLogConfig.Progressbit=0;
                SetSerialLogCfgReq.Blockselector=0;
                ReqLen=(sizeof(AMISetSerialLogReq_T)-sizeof(SOLLogConfig_T)+sizeof(SetSerialLogCfgReq.SolLogConfig.Progressbit));
                break;
            case AMI_SERIALLOG_RM_MNT_SUCCESS://ReadOnly
                continue;
            case AMI_SERIALLOG_SERVICE_RESTART:
                ReqLen =2;
                break;
        }
    wRet=IPMICMD_AMISetSerialLogCfg(pSession, &SetSerialLogCfgReq, &SetSerialLogCfgRes,ReqLen, timeout);
    if((wRet!=CC_SUCCESS)&&((wRet & 0xFF)!=CC_MOUNT_FAILED_CFG_SAVE))
       return wRet;
    }
    return 0;
}


uint16 IPMICMD_AMIGetSerialLogConf(IPMI20_SESSION_T *pSession,
                        AMIGetSerialLogReq_T *pAMIGetSerialLogCfgReq,
                        AMIGetSerialLogRes_T *pAMIGetSerailLogCfgRes,int ReqLen,int timeout)
{
    uint16 wRet;
    uint32 wResLen = 0;
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                ((NETFN_AMI << 2) + 0x00), CMD_AMI_GET_SERIALLOG_CONF,
                (uint8 *)pAMIGetSerialLogCfgReq,ReqLen ,
                (uint8 *)pAMIGetSerailLogCfgRes, &wResLen, timeout );
    return wRet;
}

uint16 LIBIPMI_HL_AMIGetSerialLogCfg(IPMI20_SESSION_T *pSession,IPMI_SerialLogCfg_T* SerialLog_config,int timeout)
{
    AMIGetSerialLogReq_T GetSerialLogCfgReq;
    AMIGetSerialLogRes_T GetSerialLogCfgRes;
    int param = 0;
    uint16 wRet = -1;

	 memset(&GetSerialLogCfgReq,0,sizeof(AMIGetSerialLogReq_T));
     memset(&GetSerialLogCfgRes,0,sizeof(AMIGetSerialLogRes_T));

    for(param = AMI_SERIALLOG_LOGSIZE; param <= AMI_SERIALLOG_NUMOFLOG;param++)
    {
       	GetSerialLogCfgReq.ParameterSelector= param;
        wRet = IPMICMD_AMIGetSerialLogConf(pSession, &GetSerialLogCfgReq, &GetSerialLogCfgRes,sizeof(INT8U), timeout);
        if(wRet==CC_SUCCESS)
        {
            switch(param)
            {
                case AMI_SERIALLOG_LOGSIZE:
                    SerialLog_config->LogSize= GetSerialLogCfgRes.SolLogConfig.log_size;
                    break;
                case AMI_SERIALLOG_NUMOFLOG:
                    SerialLog_config->NumofLog = GetSerialLogCfgRes.SolLogConfig.numoflog;
                    break;
           }
        }
    }
    return wRet;
}

uint16 LIBIPMI_HL_AMIGetSerialLogRemoteCfg(IPMI20_SESSION_T *pSession,IPMI_SerialLogCfg_T* SerialLog_config,int timeout)
{
    AMIGetSerialLogReq_T GetSerialLogCfgReq;
    AMIGetSerialLogRes_T GetSerialLogCfgRes;
    int param = 0;
    uint16 wRet = -1;

    for(param = AMI_SERIALLOG_RM_REMOTE_SUPPORT; param <= AMI_SERIALLOG_RM_MNT_SUCCESS;param++)
    {
        memset(&GetSerialLogCfgReq,0,sizeof(AMIGetSerialLogReq_T));
        memset(&GetSerialLogCfgRes,0,sizeof(AMIGetSerialLogRes_T));
        GetSerialLogCfgReq.ParameterSelector= param;
        wRet = IPMICMD_AMIGetSerialLogConf(pSession, &GetSerialLogCfgReq, &GetSerialLogCfgRes,sizeof(INT8U), timeout);
        if(wRet==CC_SUCCESS)
        {
            switch(param)
            {
                case AMI_SERIALLOG_RM_REMOTE_SUPPORT:
                    SerialLog_config->remote_path_support=GetSerialLogCfgRes.SolLogConfig.remote_path_support;
                    break;
                case AMI_SERIALLOG_RM_PATH:
                    strncpy(SerialLog_config->path,GetSerialLogCfgRes.SolLogConfig.path,MAX_IMAGE_PATH_SIZE-1);
                    break;
                case AMI_SERIALLOG_RM_IP_ADDR:
                    strncpy(SerialLog_config->ip_addr,GetSerialLogCfgRes.SolLogConfig.ip_addr,MAX_IP_ADDR_LEN-1);
                    break;
                case AMI_SERIALLOG_RM_USERNAME:
                    strncpy(SerialLog_config->username,GetSerialLogCfgRes.SolLogConfig.username,MAX_RMS_USERNAME_LEN-1);
                    break;
                case AMI_SERIALLOG_RM_PASSWORD:
                    strncpy(SerialLog_config->password,GetSerialLogCfgRes.SolLogConfig.password,MAX_RMS_PASSWORD_LEN-1);
                    break;
                case AMI_SERIALLOG_RM_SHARE_TYPE:
                    strncpy(SerialLog_config->share_type,GetSerialLogCfgRes.SolLogConfig.share_type,MAX_SHARE_TYPE_LEN-1);
                    break;
                case AMI_SERIALLOG_RM_DOMAIN:
                    strncpy(SerialLog_config->domain,GetSerialLogCfgRes.SolLogConfig.domain,MAX_DOMAIN_LEN-1);
                    break;
                case AMI_SERIALLOG_RM_MNT_SUCCESS:
                    SerialLog_config->mnt_successful=GetSerialLogCfgRes.SolLogConfig.mnt_successful;
                    break;
            }
        }
    }
    return wRet;
}


uint16 LIBIPMI_HL_GetTargetUpgCap(IPMI20_SESSION_T *pSession,GetTargetUpgradeCapablitiesRes_T *TargetUpgCapRes, int timeout){
   GetTargetUpgradeCapablitiesReq_T TargetUpgCapReq = {0};
   uint16 wRet = 0;
   uint32 ResLen;
   uint32 ReqLen = sizeof(GetTargetUpgradeCapablitiesReq_T) ;


   TargetUpgCapReq.Identifier = 0x00;
   wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                     (NETFN_PICMG << 2), CMD_GET_TARGET_UPLD_CAPABLITIES,
                     (uint8 *)&TargetUpgCapReq, ReqLen,
                     (uint8 *)TargetUpgCapRes, &ResLen,
                     timeout);
  return wRet;
}

uint16 LIBIPMI_HL_GetCompProperties_GeneralProp(IPMI20_SESSION_T *pSession, INT8U CompID, INT8U *Res, int timeout){
    GetComponentPropertiesReq_T GetCompPropReq = {0,0,0};
    uint16 wRet = 0;
    GetCompPropGeneralRes_T *CompGeneralRes =(GetCompPropGeneralRes_T *) Res;

    uint32 ResLen;
    uint32 ReqLen = sizeof(GetComponentPropertiesReq_T) ;

    GetCompPropReq.Identifier = 0x00;
    GetCompPropReq.ComponentID = CompID;
    GetCompPropReq.ComponentPropSelector = GENERAL_COMPONENT_PROPERTIES;

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                        (NETFN_PICMG << 2), CMD_GET_COMPONENT_PROPERTIES,
                        (uint8 *)&GetCompPropReq, ReqLen,
                        (uint8*)CompGeneralRes, &ResLen,
                        timeout);

    return wRet;
}

uint16 LIBIPMI_HL_GetCompProperties_DescString(IPMI20_SESSION_T *pSession, INT8U CompID, INT8U *Res, int timeout){
    GetComponentPropertiesReq_T GetCompPropReq = {0,0,0};
    uint16 wRet = 0;
    GetCompPropDescStrRes_T *CompDesStrRes =(GetCompPropDescStrRes_T * ) Res;

    uint32 ResLen;
    uint32 ReqLen = sizeof(GetComponentPropertiesReq_T) ;

    GetCompPropReq.Identifier = 0x00;
    GetCompPropReq.ComponentID = CompID;
    GetCompPropReq.ComponentPropSelector = DESCRIPTION_STRING;

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                        (NETFN_PICMG << 2), CMD_GET_COMPONENT_PROPERTIES,
                        (uint8 *)&GetCompPropReq, ReqLen,
                        (uint8*)CompDesStrRes, &ResLen,
                        timeout);

    return wRet;
}

uint16 LIBIPMI_HL_GetCompProperties_DeferredVersion(IPMI20_SESSION_T *pSession, INT8U CompID, INT8U *Res, int timeout){

    GetComponentPropertiesReq_T GetCompPropReq = {0,0,0};
    uint16 wRet = 0;
    GetCompPropFWRes_T  *CompFWRes = ( GetCompPropFWRes_T *) Res;

    uint32 ResLen;
    uint32 ReqLen = sizeof(GetComponentPropertiesReq_T) ;

    GetCompPropReq.Identifier = 0x00;
    GetCompPropReq.ComponentID = CompID;
    GetCompPropReq.ComponentPropSelector = DEFERRED_UPG_FW_VERSION;

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                        (NETFN_PICMG << 2), CMD_GET_COMPONENT_PROPERTIES,
                        (uint8 *)&GetCompPropReq, ReqLen,
                        (uint8*)CompFWRes, &ResLen,
                        timeout);
    return wRet;
}

uint16 LIBIPMI_HL_GetCompProperties_CurrVersion(IPMI20_SESSION_T *pSession, INT8U CompID, INT8U *Res, int timeout){

    GetComponentPropertiesReq_T GetCompPropReq = {0,0,0};
    uint16 wRet = 0;
    GetCompPropFWRes_T  *CompFWRes = ( GetCompPropFWRes_T *) Res;

    uint32 ResLen;
    uint32 ReqLen = sizeof(GetComponentPropertiesReq_T) ;

    GetCompPropReq.Identifier = 0x00;
    GetCompPropReq.ComponentID = CompID;
    GetCompPropReq.ComponentPropSelector = CURRENT_VERSION;

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                        (NETFN_PICMG << 2), CMD_GET_COMPONENT_PROPERTIES,
                        (uint8 *)&GetCompPropReq, ReqLen,
                        (uint8*)CompFWRes, &ResLen,
                        timeout);
    return wRet;
}

uint16 LIBIPMI_HL_GetCompProperties_RollBackVersion(IPMI20_SESSION_T *pSession, INT8U CompID, INT8U *Res, int timeout){

    GetComponentPropertiesReq_T GetCompPropReq = {0,0,0};
    uint16 wRet = 0;
    GetCompPropFWRes_T  *CompFWRes = ( GetCompPropFWRes_T *) Res;

    uint32 ResLen;
    uint32 ReqLen = sizeof(GetComponentPropertiesReq_T) ;

    GetCompPropReq.Identifier = 0x00;
    GetCompPropReq.ComponentID = CompID;
    GetCompPropReq.ComponentPropSelector = ROLLBACK_FW_VERSION;

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                        (NETFN_PICMG << 2), CMD_GET_COMPONENT_PROPERTIES,
                        (uint8 *)&GetCompPropReq, ReqLen,
                        (uint8*)CompFWRes, &ResLen,
                        timeout);
    return wRet;
}

 uint16 LIBIPMI_HL_InitiateUpgAction(IPMI20_SESSION_T *pSession,INT8U Components, INT8U UpgAction , InitiateUpgActionRes_T *InitialUpgActRes, int timeout){

   InitiateUpgActionReq_T InitiateUpgActReq = {0,0,0};
   uint16 wRet = 0;
   uint32 ResLen;
   uint32 ReqLen = sizeof( InitiateUpgActionReq_T ) ;

   InitiateUpgActReq.Identifier = 0x00;
   InitiateUpgActReq.Components = Components;
   InitiateUpgActReq.UpgradeAction = UpgAction;

   wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                        (NETFN_PICMG << 2), CMD_INITIATE_UPG_ACTION,
                        (uint8 *)&InitiateUpgActReq, ReqLen,
                        (uint8*)InitialUpgActRes, &ResLen,
                        timeout);

   return wRet;
 }

 uint16 LIBIPMI_HL_UploadFWBlock(IPMI20_SESSION_T *pSession,INT8U BlockNumber, INT8U *FirmwareData , INT32U DataLen, UploadFirmwareBlkRes_T *UploadFWBlkRes, int timeout){
    INT8U ReqBuf[MAX_HPM_MSG_LENGTH] = {0};
    UploadFirmwareBlkReq_T *UploadFWBlkReq = (UploadFirmwareBlkReq_T*)ReqBuf;
    uint16 wRet = 0;
    uint32 ResLen;
    uint32 ReqLen ;

    TINFO("LIBIPMI_HL_UploadFWBlock....\n");
    ReqLen = 2 + DataLen;
    UploadFWBlkReq->Identifier = 0x00;
    UploadFWBlkReq->BlkNumber = BlockNumber;

   TINFO("UploadFWBlkReq->BlkNumber %d .DataLen %d\n",UploadFWBlkReq->BlkNumber, DataLen);
   TINFO("before UploadFWBlkReq->BlkNumber %d .ReqLen %d\n",UploadFWBlkReq->BlkNumber, ReqLen);
    memcpy(&UploadFWBlkReq->FWData,FirmwareData,DataLen);
   TINFO("after UploadFWBlkReq->BlkNumber %d .ReqLen %d\n",UploadFWBlkReq->BlkNumber, ReqLen);

    TINFO("%x %x %x %x .....\n", UploadFWBlkReq->FWData[0], UploadFWBlkReq->FWData[1], UploadFWBlkReq->FWData[2], UploadFWBlkReq->FWData[3]);
    printf("%x %x %x %x .....\n", UploadFWBlkReq->FWData[0], UploadFWBlkReq->FWData[1], UploadFWBlkReq->FWData[2], UploadFWBlkReq->FWData[3]);
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                        (NETFN_PICMG << 2), CMD_UPLOAD_FIRMWARE_BLOCK,
                        ReqBuf, ReqLen,
                        (uint8*)UploadFWBlkRes, &ResLen,
                        timeout);

    return wRet;
}

 uint16 LIBIPMI_HL_FinishFWUpload(IPMI20_SESSION_T *pSession,INT8U Comp, INT32U ImgSize , FinishFWUploadRes_T *FinishUploadRes, int timeout){
    FinishFWUploadReq_T FinishUploadReq = {0,0,0};
    uint16 wRet = 0;
    uint32 ResLen;
    uint32 ReqLen = sizeof( FinishFWUploadReq_T) ;

    FinishUploadReq.Identifier = 0x00;
    FinishUploadReq.Component = Comp;
    FinishUploadReq.ImageSize = ImgSize;

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                        (NETFN_PICMG << 2), CMD_FINISH_FIRMWARE_UPLOAD,
                        (uint8 *)&FinishUploadReq, ReqLen,
                        (uint8*)FinishUploadRes, &ResLen,
                        timeout);

    return wRet;
    }


 uint16 LIBIPMI_HL_AbortFWUpgrade(IPMI20_SESSION_T *pSession, AbortFirmwareUpgradeRes_T *AbortFWUpgRes, int timeout){
    AbortFirmwareUpgradeReq_T AbortFWUpgReq = {0};
    uint16 wRet = 0;
    uint32 ResLen;
    uint32 ReqLen = sizeof( AbortFirmwareUpgradeReq_T ) ;


    AbortFWUpgReq.Identifier = 0x00;
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                        (NETFN_PICMG << 2), CMD_ABORT_FIRMWARE_UPGRADE,
                        (uint8 *)&AbortFWUpgReq, ReqLen,
                        (uint8 *)AbortFWUpgRes, &ResLen,
                        timeout);
    return wRet;
    }

 uint16 LIBIPMI_HL_GetUpgStatus(IPMI20_SESSION_T *pSession, GetUpgradeStatusRes_T *GetUpgStatusRes, int timeout){
    GetUpgradeStatusReq_T GetUpgStatusReq = {0};
    uint16 wRet = 0;
    uint32 ResLen;
    uint32 ReqLen = sizeof( GetUpgradeStatusReq_T ) ;


    GetUpgStatusReq.Identifier = 0x00;
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                        (NETFN_PICMG << 2), CMD_GET_UPGRADE_STATUS,
                        (uint8 *)&GetUpgStatusReq, ReqLen,
                        (uint8 *)GetUpgStatusRes, &ResLen,
                        timeout);
    return wRet;
}

uint16 LIBIPMI_HL_ActivateFWUpgrade(IPMI20_SESSION_T *pSession, ActivateFWRes_T *ActFWUpgRes, int timeout){
    ActivateFWReq_T ActFWUpgReq = {0};
    uint16 wRet = 0;
    uint32 ResLen;
    uint32 ReqLen = sizeof( ActivateFWReq_T ) ;


    ActFWUpgReq.Identifier = 0x00;
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                        (NETFN_PICMG << 2), CMD_GET_UPGRADE_STATUS,
                        (uint8 *)&ActFWUpgReq, ReqLen,
                        (uint8 *)ActFWUpgRes, &ResLen,
                        timeout);
    return wRet;
}

uint16 LIBIPMI_HL_QueryRollBackStatus(IPMI20_SESSION_T *pSession, QueryRollbackStatusRes_T *RollBckRes, int timeout){
    QueryRollbackStatusReq_T RollBckReq = {0};
    uint16 wRet = 0;
    uint32 ResLen;
    uint32 ReqLen = sizeof( QueryRollbackStatusReq_T ) ;

    RollBckReq.Identifier = 0x00;
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                        (NETFN_PICMG << 2), CMD_QUERY_ROLLBACK_STATUS,
                        (uint8 *)&RollBckReq, ReqLen,
                        (uint8 *)RollBckRes, &ResLen,
                        timeout);
    return wRet;
}

uint16 LIBIPMI_HL_InitiateManRollBack(IPMI20_SESSION_T *pSession, InitiateManualRollbackRes_T *InitateManRollBckRes, int timeout){
    InitiateManualRollbackReq_T InitateManRollBckReq = {0};
    uint16 wRet = 0;
    uint32 ResLen;
    uint32 ReqLen = sizeof( InitiateManualRollbackReq_T ) ;

    InitateManRollBckReq.Identifier = 0x00;

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                        (NETFN_PICMG << 2), CMD_INITIATE_MANUAL_ROLLBACK,
                        (uint8 *)&InitateManRollBckReq, ReqLen,
                        (uint8 *)InitateManRollBckRes, &ResLen,
                        timeout);
    return wRet;
}

uint16 LIBIPMI_HL_QuerySelfTestResults(IPMI20_SESSION_T *pSession, QuerySelfTestResultsRes_T *QuerySelfTestRes, int timeout){
    QuerySelfTestResultsReq_T QuerySelfTestReq = {0};
    uint16 wRet = 0;
    uint32 ResLen;
    uint32 ReqLen = sizeof( QuerySelfTestResultsReq_T ) ;

    QuerySelfTestReq.Identifier = 0x00;
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                        (NETFN_PICMG << 2), CMD_QUERY_SELF_TEST_RESULTS,
                        (uint8 *)&QuerySelfTestReq, ReqLen,
                        (uint8 *)QuerySelfTestRes, &ResLen,
                        timeout);
return wRet;
}

uint16 LIBIPMI_HL_Make_PHYDRV_OnlineOffline(IPMI20_SESSION_T *pSession, INT32U CtrlID, INT16U deviceId, INT8U cmdparam, int timeout)
{
    uint16 wRet = 1, ReqLen;
    AMIGetRAIDConfigReq_T   AMIGetRAIDConfigReq;
    AMIGetRAIDConfigRes_T   AMIGetRAIDConfigRes;

    memset(&AMIGetRAIDConfigReq, 0, sizeof(AMIGetRAIDConfigReq));
    memset(&AMIGetRAIDConfigRes, 0, sizeof(AMIGetRAIDConfigRes));

    if(cmdparam == MAKE_OFFLINE)
    {
        AMIGetRAIDConfigReq.Param = PARAM_MAKE_PHYDRV_OFFLINE;
    }
    else if(cmdparam == MAKE_ONLINE)
    {
        AMIGetRAIDConfigReq.Param = PARAM_MAKE_PHYDRV_ONLINE;
    }
    else
    {
         TINFO("Unsupported cmdparam value\n");
         return wRet;
    }

    AMIGetRAIDConfigReq.ParamData.DevInfo.CtrlID = CtrlID;
    AMIGetRAIDConfigReq.ParamData.DevInfo.DevID = deviceId;
    ReqLen = 3 + 4 + 2;

    wRet = IPMICMD_AMIGetRAIDConfig(pSession,(INT8U*)&AMIGetRAIDConfigReq, ReqLen,(INT8U*)&AMIGetRAIDConfigRes, timeout);
    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TINFO("Error while executing make phydrv online/offline command\n", wRet);
    }

    return wRet;
}

uint16 LIBIPMI_HL_Manage_HotSpare(IPMI20_SESSION_T *pSession, INT32U CtrlID, INT16U deviceId, INT8U cmdparam, int timeout)
{
    uint16 wRet = 1, ReqLen;
    AMIGetRAIDConfigReq_T   AMIGetRAIDConfigReq;
    AMIGetRAIDConfigRes_T   AMIGetRAIDConfigRes;

    memset(&AMIGetRAIDConfigReq, 0, sizeof(AMIGetRAIDConfigReq));
    memset(&AMIGetRAIDConfigRes, 0, sizeof(AMIGetRAIDConfigRes));

    if(cmdparam == MAKE_GLOBAL_HOTSPARE)
    {
        AMIGetRAIDConfigReq.Param = PARAM_MAKE_GLOBAL_HOTSPARE;
    }
    else if(cmdparam == REMOVE_HOTSPARE)
    {
        AMIGetRAIDConfigReq.Param = PARAM_REMOVE_HOTSPARE;
    }
    else
    {
         TINFO("Unsupported cmdparam value\n");
         return wRet;
    }

    AMIGetRAIDConfigReq.ParamData.DevInfo.CtrlID = CtrlID;
    AMIGetRAIDConfigReq.ParamData.DevInfo.DevID = deviceId;
    ReqLen = 3 + 4 + 2;

    wRet = IPMICMD_AMIGetRAIDConfig(pSession,(INT8U*)&AMIGetRAIDConfigReq, ReqLen,(INT8U*)&AMIGetRAIDConfigRes, timeout);
    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TINFO("Error while executing manage hotspare command\n", wRet);
    }

    return wRet;
}

uint16 LIBIPMI_HL_PHYDRV_RemovalCmd(IPMI20_SESSION_T *pSession, INT32U CtrlID, INT16U deviceId, INT8U cmdparam, int timeout)
{
    uint16 wRet = 1, ReqLen;
    AMIGetRAIDConfigReq_T   AMIGetRAIDConfigReq;
    AMIGetRAIDConfigRes_T   AMIGetRAIDConfigRes;

    memset(&AMIGetRAIDConfigReq, 0, sizeof(AMIGetRAIDConfigReq));
    memset(&AMIGetRAIDConfigRes, 0, sizeof(AMIGetRAIDConfigRes));

    if(cmdparam == PHYDRV_REMOVE)
    {
        AMIGetRAIDConfigReq.Param = PARAM_PREPARE_PHYDRV_FOR_REMOVAL;
    }
    else if(cmdparam == UNDO_PHYDRV_REMOVE)
    {
        AMIGetRAIDConfigReq.Param = PARAM_UNDO_PHYDRV_REMOVAL;
    }
    else
    {
         TINFO("Unsupported cmdparam value\n");
         return wRet;
    }

    AMIGetRAIDConfigReq.ParamData.DevInfo.CtrlID = CtrlID;
    AMIGetRAIDConfigReq.ParamData.DevInfo.DevID = deviceId;
    ReqLen = 3 + 4 + 2;

    wRet = IPMICMD_AMIGetRAIDConfig(pSession,(INT8U*)&AMIGetRAIDConfigReq, ReqLen,(INT8U*)&AMIGetRAIDConfigRes, timeout);
    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TINFO("Error while executing prepare phydrv removal/undo removal command\n", wRet);
    }

    return wRet;
}

#if 0
uint16 LIBIPMI_HL_PHYDRV_FormatCmd(IPMI20_SESSION_T *pSession, INT32U CtrlID, INT16U deviceId, INT8U cmdparam, int timeout)
{
    uint16 wRet = 1, ReqLen;
    AMIGetRAIDConfigReq_T   AMIGetRAIDConfigReq;
    AMIGetRAIDConfigRes_T   AMIGetRAIDConfigRes;

    memset(&AMIGetRAIDConfigReq, 0, sizeof(AMIGetRAIDConfigReq));
    memset(&AMIGetRAIDConfigRes, 0, sizeof(AMIGetRAIDConfigRes));

    if(cmdparam == FORMAT_PHYDRV)
    {
        AMIGetRAIDConfigReq.Param = PARAM_FORMAT_PHYDRV;
    }
    else if(cmdparam == ABORT_FROMAT_PHYDRV)
    {
        AMIGetRAIDConfigReq.Param = PARAM_UNDO_FORMAT_PHYDRV;
    }
    else
    {
         TINFO("Unsupported cmdparam value\n");
         return wRet;
    }

    AMIGetRAIDConfigReq.ParamData.DevInfo.CtrlID = CtrlID;
    AMIGetRAIDConfigReq.ParamData.DevInfo.DevID = deviceId;
    ReqLen = 3 + 4 + 2;

    wRet = IPMICMD_AMIGetRAIDConfig(pSession,(INT8U*)&AMIGetRAIDConfigReq, ReqLen,(INT8U*)&AMIGetRAIDConfigRes, timeout);
    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TINFO("Error while executing phydrv format command\n", wRet);
    }

    return wRet;
}
#endif

uint16 LIBIPMI_HL_Create_LogicalDev(IPMI20_SESSION_T *pSession, INT32U CtrlID, ManageConfig_T ld_config_data, int ConfigSize, int timeout)
{
    uint16 wRet = 1, ReqLen;
    AMIGetRAIDConfigReq_T   AMIGetRAIDConfigReq;
    AMIGetRAIDConfigRes_T   AMIGetRAIDConfigRes;

    memset(&AMIGetRAIDConfigReq, 0, sizeof(AMIGetRAIDConfigReq));
    memset(&AMIGetRAIDConfigRes, 0, sizeof(AMIGetRAIDConfigRes));
    if(0)
    {
        CtrlID=CtrlID;
    }

    AMIGetRAIDConfigReq.Param = PARAM_ADD_CONFIGURATION;
    memcpy(&(AMIGetRAIDConfigReq.ParamData.ManageConfig),(const void *) &ld_config_data, ConfigSize);
    ReqLen = 3 + ConfigSize;

    wRet = IPMICMD_AMIGetRAIDConfig(pSession,(INT8U*)&AMIGetRAIDConfigReq, ReqLen,(INT8U*)&AMIGetRAIDConfigRes, timeout);
    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TINFO("Error while executing create logical device command\n", wRet);
    }

    return wRet;
}

uint16 LIBIPMI_HL_Delete_LogicalDev(IPMI20_SESSION_T *pSession, INT32U CtrlID, INT16U DeviceID, int timeout)
{
    uint16 wRet = 1, ReqLen;
    AMIGetRAIDConfigReq_T   AMIGetRAIDConfigReq;
    AMIGetRAIDConfigRes_T   AMIGetRAIDConfigRes;

    memset(&AMIGetRAIDConfigReq, 0, sizeof(AMIGetRAIDConfigReq));
    memset(&AMIGetRAIDConfigRes, 0, sizeof(AMIGetRAIDConfigRes));

    AMIGetRAIDConfigReq.Param = PARAM_DELETE_VIRTUAL_DRIVE;
    AMIGetRAIDConfigReq.ParamData.SetPDParam.CtrlID = CtrlID;
    AMIGetRAIDConfigReq.ParamData.SetPDParam.DeviceId = DeviceID;
    ReqLen = 3 + 4 + 2;

    wRet = IPMICMD_AMIGetRAIDConfig(pSession,(INT8U*)&AMIGetRAIDConfigReq, ReqLen,(INT8U*)&AMIGetRAIDConfigRes, timeout);
    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TINFO("Error while executing create logical device command\n", wRet);
    }

    return wRet;
}

uint16 LIBIPMI_HL_Get_RAID_Level_Strip_Size(IPMI20_SESSION_T *pSession, INT32U CtrlID, RL_Strip_Info_T *pRLStripInfo, int timeout)
{
    uint16 wRet = 1, ReqLen;
    AMIGetRAIDConfigReq_T   AMIGetRAIDConfigReq;
    AMIGetRAIDConfigRes_T   AMIGetRAIDConfigRes;

    memset(&AMIGetRAIDConfigReq, 0, sizeof(AMIGetRAIDConfigReq));
    memset(&AMIGetRAIDConfigRes, 0, sizeof(AMIGetRAIDConfigRes));

    AMIGetRAIDConfigReq.Param = PARAM_GET_RL_STRIP_SIZE_INFO;
    AMIGetRAIDConfigReq.ParamData.SetPDParam.CtrlID = CtrlID;
    ReqLen = 3 + 4 ;

    wRet = IPMICMD_AMIGetRAIDConfig(pSession,(INT8U*)&AMIGetRAIDConfigReq, ReqLen,(INT8U*)&AMIGetRAIDConfigRes, timeout);
    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TINFO("Error while executing Get RL Strip Size command\n", wRet);
    }

    memcpy(pRLStripInfo,&AMIGetRAIDConfigRes.ParamData.RLStripInfo,sizeof(RL_Strip_Info_T));

    return wRet;
}

uint16 LIBIPMI_HL_Get_RAID_Level_PD_Count(IPMI20_SESSION_T *pSession, INT32U CtrlID, RL_PD_Count_T *pRLPDCount, int timeout)
{
    uint16 wRet = 1, ReqLen;
    AMIGetRAIDConfigReq_T   AMIGetRAIDConfigReq;
    AMIGetRAIDConfigRes_T   AMIGetRAIDConfigRes;

    memset(&AMIGetRAIDConfigReq, 0, sizeof(AMIGetRAIDConfigReq));
    memset(&AMIGetRAIDConfigRes, 0, sizeof(AMIGetRAIDConfigRes));

    AMIGetRAIDConfigReq.Param = PARAM_GET_RL_MAXMIN_PD_COUNT;
    AMIGetRAIDConfigReq.ParamData.SetPDParam.CtrlID = CtrlID;
    ReqLen = 3 + 4;

    wRet = IPMICMD_AMIGetRAIDConfig(pSession,(INT8U*)&AMIGetRAIDConfigReq, ReqLen,(INT8U*)&AMIGetRAIDConfigRes, timeout);
    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TINFO("Error while executing RL PD Count command\n", wRet);
    }

    memcpy(pRLPDCount,&AMIGetRAIDConfigRes.ParamData.RLPDCount,sizeof(RL_PD_Count_T));
    return wRet;
}

uint16 LIBIPMI_HL_AMIGetSASITCtrlCount(IPMI20_SESSION_T *pSession,INT8U *CtrlCount, INT32U *CtrlIDList, int timeout)
{
    uint16 wRet, ReqLen;
    AMIGetRAIDConfigReq_T   AMIGetRAIDConfigReq;
    INT8U pRes[1024] = {0};

    memset(&AMIGetRAIDConfigReq, 0, sizeof(AMIGetRAIDConfigReq));

    AMIGetRAIDConfigReq.Param = GET_SASIT_CTRL_COUNT;
    ReqLen = 3;

    wRet = IPMICMD_AMIGetRAIDConfig(pSession,(INT8U*)&AMIGetRAIDConfigReq,ReqLen,pRes,timeout);
    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TINFO("Error while getting the SAS IT Controller Count %x", wRet);
    }

    *CtrlCount = pRes[1];
    memcpy(CtrlIDList, &pRes[2], (sizeof(INT32U) * pRes[1]));

    return wRet;
}

uint16 LIBIPMI_HL_AMIGetSASITCtrlInfo(IPMI20_SESSION_T *pSession,INT32U CtrlID, HBAInfo_T *pHBAInfo,int timeout)
{
    uint16 wRet, ReqLen;
    AMIGetRAIDConfigReq_T   AMIGetRAIDConfigReq;
    AMIGetRAIDConfigRes_T   AMIGetRAIDConfigRes;


    memset(&AMIGetRAIDConfigReq, 0, sizeof(AMIGetRAIDConfigReq));
    memset(&AMIGetRAIDConfigRes, 0, sizeof(AMIGetRAIDConfigRes));

    AMIGetRAIDConfigReq.Param = GET_SASIT_CTRL_INFO;
    AMIGetRAIDConfigReq.ParamData.CtrlID = CtrlID;
    ReqLen = 3 + sizeof(CtrlID);

    wRet = IPMICMD_AMIGetRAIDConfig(pSession,(INT8U*)&AMIGetRAIDConfigReq,ReqLen,(INT8U*)&AMIGetRAIDConfigRes,timeout);
    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TINFO("Error while getting the SAS IT Controller's ( %d )Info : Error %x", CtrlID, wRet);
    }

    memcpy(pHBAInfo,&AMIGetRAIDConfigRes.ParamData.HBAInfo, sizeof(HBAInfo_T));
    return wRet;
}

uint16 LIBIPMI_HL_AMIGetSASITPhyDevCount(IPMI20_SESSION_T *pSession,INT32U CtrlID, INT8U *PhyCount, INT16U *PhyDevIDList, int timeout)
{
    uint16 wRet, ReqLen;
    AMIGetRAIDConfigReq_T   AMIGetRAIDConfigReq;
    INT8U pRes[1024] = {0};

    memset(&AMIGetRAIDConfigReq, 0, sizeof(AMIGetRAIDConfigReq));

    AMIGetRAIDConfigReq.Param = GET_SASIT_PHY_DEV_COUNT;
    AMIGetRAIDConfigReq.ParamData.CtrlID = CtrlID;
    ReqLen = 3 + sizeof(CtrlID);

    wRet = IPMICMD_AMIGetRAIDConfig(pSession,(INT8U*)&AMIGetRAIDConfigReq,ReqLen,pRes,timeout);
    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TINFO("Error while getting the SAS IT Controller's ( %d ) Physical Device Count: Error %x",CtrlID, wRet);
    }

    *PhyCount = pRes[5];
    memcpy(PhyDevIDList, &pRes[6], (sizeof(INT16U) * pRes[5]));

    return wRet;
}

uint16 LIBIPMI_HL_AMIGetSASITPhyDevInfo(IPMI20_SESSION_T *pSession,INT32U CtrlID, INT16U PhyDevID,HBAPhyDevInfo_T *pPhyDevInfo, int timeout)
{
    uint16 wRet, ReqLen;
    AMIGetRAIDConfigReq_T   AMIGetRAIDConfigReq;
    AMIGetRAIDConfigRes_T   AMIGetRAIDConfigRes;


    memset(&AMIGetRAIDConfigReq, 0, sizeof(AMIGetRAIDConfigReq));
    memset(&AMIGetRAIDConfigRes, 0, sizeof(AMIGetRAIDConfigRes));

    AMIGetRAIDConfigReq.Param = GET_SASIT_PHY_DEV_INFO;
    AMIGetRAIDConfigReq.ParamData.DevInfo.CtrlID = CtrlID;
    AMIGetRAIDConfigReq.ParamData.DevInfo.DevID = PhyDevID;
    ReqLen = 3 + sizeof(Device_T);

    wRet = IPMICMD_AMIGetRAIDConfig(pSession,(INT8U*)&AMIGetRAIDConfigReq,ReqLen,(INT8U*)&AMIGetRAIDConfigRes,timeout);
    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TINFO("Error while getting the SAS IT Controller's ( %d ) Physical Device ( %d ) Info: Error %x", CtrlID, PhyDevID, wRet);
    }

    memcpy(pPhyDevInfo,&AMIGetRAIDConfigRes.ParamData.HBAPhyDevInfo, sizeof(HBAPhyDevInfo_T));
    return wRet;
}

uint16 LIBIPMI_HL_AMIGetSASITToplogy(IPMI20_SESSION_T *pSession,INT32U CtrlID, Topology_T *pTopology, int timeout)
{
    uint16 wRet, ReqLen;
    AMIGetRAIDConfigReq_T   AMIGetRAIDConfigReq;
    AMIGetRAIDConfigRes_T   AMIGetRAIDConfigRes;


    memset(&AMIGetRAIDConfigReq, 0, sizeof(AMIGetRAIDConfigReq));
    memset(&AMIGetRAIDConfigRes, 0, sizeof(AMIGetRAIDConfigRes));

    AMIGetRAIDConfigReq.Param = GET_SASIT_TOPOLOGY_INFO;
    AMIGetRAIDConfigReq.ParamData.EnclosureConfig.CtrlID = CtrlID;
    ReqLen = 3 + sizeof(INT32U);

    wRet = IPMICMD_AMIGetRAIDConfig(pSession,(INT8U*)&AMIGetRAIDConfigReq,ReqLen,(INT8U*)&AMIGetRAIDConfigRes,timeout);
    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TINFO("Error while getting the SAS IT Controller's ( %d ) Topology Info: Error %x", CtrlID, wRet);
    }

    memcpy(pTopology,&AMIGetRAIDConfigRes.ParamData.TopologyInfo, (sizeof(TopologyNodeInfo_T) * AMIGetRAIDConfigRes.ParamData.TopologyInfo.NodeCount));
    return wRet;
}

uint16 LIBIPMI_HL_AMIGetSASITElementStatus(IPMI20_SESSION_T *pSession,INT32U CtrlID, INT16U EnclID, INT8U *pStatus, int timeout)
{
    uint16 wRet, ReqLen;
    AMIGetRAIDConfigReq_T   AMIGetRAIDConfigReq;

    memset(&AMIGetRAIDConfigReq, 0, sizeof(AMIGetRAIDConfigReq));

    AMIGetRAIDConfigReq.Param = GET_SASIT_ELEMENT_STATUS;
    AMIGetRAIDConfigReq.ParamData.EnclosureConfig.CtrlID = CtrlID;
    AMIGetRAIDConfigReq.ParamData.EnclosureConfig.EnclID = EnclID;
    ReqLen = 3 + sizeof(CtrlID) + sizeof(EnclID);

    wRet = IPMICMD_AMIGetRAIDConfig(pSession,(INT8U*)&AMIGetRAIDConfigReq,ReqLen,(INT8U*)pStatus,timeout);
    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TINFO("Error while getting the SAS IT Controller's ( %d ) Enclosure Element Status Info: Error %x", CtrlID, wRet);
    }

    return wRet;
}

uint16 LIBIPMI_HL_AMIGetSASITTempSensorInfo(IPMI20_SESSION_T *pSession,INT32U CtrlID, INT16U EnclID, INT8U Index, TempSensorInfo_T *pTempSensorInfo, int timeout)
{
    uint16 wRet, ReqLen;
    AMIGetRAIDConfigReq_T   AMIGetRAIDConfigReq;
    AMIGetRAIDConfigRes_T   AMIGetRAIDConfigRes;


    memset(&AMIGetRAIDConfigReq, 0, sizeof(AMIGetRAIDConfigReq));
    memset(&AMIGetRAIDConfigRes, 0, sizeof(AMIGetRAIDConfigRes));

    AMIGetRAIDConfigReq.Param = GET_SASIT_TEMP_SEN_THRESHOLD;
    AMIGetRAIDConfigReq.ParamData.EnclosureConfig.CtrlID = CtrlID;
    AMIGetRAIDConfigReq.ParamData.EnclosureConfig.EnclID = EnclID;
    AMIGetRAIDConfigReq.ParamData.EnclosureConfig.Index = Index;
    ReqLen = 3 + sizeof(EnclosureConfig_T);

    wRet = IPMICMD_AMIGetRAIDConfig(pSession,(INT8U*)&AMIGetRAIDConfigReq,ReqLen,(INT8U*)&AMIGetRAIDConfigRes,timeout);
    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TINFO("Error while getting the SAS IT Controller's ( %d ) Temp Sensor ( Encl : %d ) Info: Error %x", CtrlID, EnclID, wRet);
    }

    memcpy(pTempSensorInfo,&AMIGetRAIDConfigRes.ParamData.TempSensorInfo, sizeof(TempSensorInfo_T));
    return wRet;
}

uint16 LIBIPMI_HL_AMIGetSASITEnclosureList(IPMI20_SESSION_T *pSession,INT32U CtrlID, EnclosureInfo_T *pEnclListInfo, int timeout)
{
    uint16 wRet, ReqLen;
    AMIGetRAIDConfigReq_T   AMIGetRAIDConfigReq;
    AMIGetRAIDConfigRes_T   AMIGetRAIDConfigRes;


    memset(&AMIGetRAIDConfigReq, 0, sizeof(AMIGetRAIDConfigReq));
    memset(&AMIGetRAIDConfigRes, 0, sizeof(AMIGetRAIDConfigRes));

    AMIGetRAIDConfigReq.Param = GET_SASIT_ELEMENT_LIST;
    AMIGetRAIDConfigReq.ParamData.DevInfo.CtrlID = CtrlID;
    ReqLen = 3 + sizeof(CtrlID);

    wRet = IPMICMD_AMIGetRAIDConfig(pSession,(INT8U*)&AMIGetRAIDConfigReq,ReqLen,(INT8U*)&AMIGetRAIDConfigRes,timeout);
    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TINFO("Error while getting the SAS IT Controller's ( %d ) Enclosure Element List Info: Error %x", CtrlID, wRet);
    }

    memcpy(pEnclListInfo,&AMIGetRAIDConfigRes.ParamData.EnclosureInfo, (sizeof(EnclDetail_T) * AMIGetRAIDConfigRes.ParamData.EnclosureInfo.EnclCount));
    return wRet;
}

uint16 LIBIPMI_HL_AMISetSASITPSElementInfo(IPMI20_SESSION_T *pSession,PSConfig_T PSConfig, int timeout)
{
    uint16 wRet, ReqLen;
    AMIGetRAIDConfigReq_T   AMIGetRAIDConfigReq;
    AMIGetRAIDConfigRes_T   AMIGetRAIDConfigRes;


    memset(&AMIGetRAIDConfigReq, 0, sizeof(AMIGetRAIDConfigReq));
    memset(&AMIGetRAIDConfigRes, 0, sizeof(AMIGetRAIDConfigRes));

    AMIGetRAIDConfigReq.Param = SET_SASIT_PS_ELEMENT_INFO;
    memcpy(&AMIGetRAIDConfigReq.ParamData.PSConfig, &PSConfig, sizeof(PSConfig_T));
    ReqLen = 3 + sizeof(PSConfig_T);

    wRet = IPMICMD_AMIGetRAIDConfig(pSession,(INT8U*)&AMIGetRAIDConfigReq,ReqLen,(INT8U*)&AMIGetRAIDConfigRes,timeout);
    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TINFO("Error while Setting the SAS IT Controller's ( %d ) Power Supply Element ( Encl : %d ) Info: Error %x", PSConfig.CtrlID, PSConfig.EnclID, wRet);
    }

    return wRet;
}

uint16 LIBIPMI_HL_AMISetSASITTempSenElementInfo(IPMI20_SESSION_T *pSession, SetTempSenConfig_T SetTempSenConfig, int timeout)
{
    uint16 wRet, ReqLen;
    AMIGetRAIDConfigReq_T   AMIGetRAIDConfigReq;
    AMIGetRAIDConfigRes_T   AMIGetRAIDConfigRes;


    memset(&AMIGetRAIDConfigReq, 0, sizeof(AMIGetRAIDConfigReq));
    memset(&AMIGetRAIDConfigRes, 0, sizeof(AMIGetRAIDConfigRes));

    AMIGetRAIDConfigReq.Param = SET_SASIT_TEMP_SEN_ELEMENT_INFO;
    memcpy(&AMIGetRAIDConfigReq.ParamData.SetTempSenConfig, &SetTempSenConfig, sizeof(SetTempSenConfig_T));
    ReqLen = 3 + sizeof(SetTempSenConfig_T);

    wRet = IPMICMD_AMIGetRAIDConfig(pSession,(INT8U*)&AMIGetRAIDConfigReq,ReqLen,(INT8U*)&AMIGetRAIDConfigRes,timeout);
    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TINFO("Error while getting the SAS IT Controller's ( %d ) Temp Sensor Element ( Encl : %d ) Info: Error %x", SetTempSenConfig.CtrlID, SetTempSenConfig.EnclID, wRet);
    }

    return wRet;
}

uint16 LIBIPMI_HL_AMISetSASITCoolingInfo(IPMI20_SESSION_T *pSession,CoolingConfig_T CoolingConfig, int timeout)
{
    uint16 wRet, ReqLen;
    AMIGetRAIDConfigReq_T   AMIGetRAIDConfigReq;
    AMIGetRAIDConfigRes_T   AMIGetRAIDConfigRes;


    memset(&AMIGetRAIDConfigReq, 0, sizeof(AMIGetRAIDConfigReq));
    memset(&AMIGetRAIDConfigRes, 0, sizeof(AMIGetRAIDConfigRes));

    AMIGetRAIDConfigReq.Param = SET_SASIT_COOLING_ELEMENT_INFO;
    memcpy(&AMIGetRAIDConfigReq.ParamData.CoolingConfig, &CoolingConfig, sizeof(CoolingConfig_T));
    ReqLen = 3 + sizeof(CoolingConfig_T);

    wRet = IPMICMD_AMIGetRAIDConfig(pSession,(INT8U*)&AMIGetRAIDConfigReq,ReqLen,(INT8U*)&AMIGetRAIDConfigRes,timeout);
    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TINFO("Error while getting the SAS IT Controller's ( %d ) Cooling Element ( Encl : %d ) Info: Error %x", CoolingConfig.CtrlID, CoolingConfig.EnclID, wRet);
    }

    return wRet;
}

uint16 LIBIPMI_HL_AMISetSASITAlarmInfo(IPMI20_SESSION_T *pSession,AlarmConfig_T AlarmConfig, int timeout)
{
    uint16 wRet, ReqLen;
    AMIGetRAIDConfigReq_T   AMIGetRAIDConfigReq;
    AMIGetRAIDConfigRes_T   AMIGetRAIDConfigRes;


    memset(&AMIGetRAIDConfigReq, 0, sizeof(AMIGetRAIDConfigReq));
    memset(&AMIGetRAIDConfigRes, 0, sizeof(AMIGetRAIDConfigRes));

    AMIGetRAIDConfigReq.Param = SET_SASIT_ALARM_ELEMENT_INFO;
    memcpy(&AMIGetRAIDConfigReq.ParamData.AlarmConfig, &AlarmConfig, sizeof(AlarmConfig_T));
    ReqLen = 3 + sizeof(AlarmConfig_T);

    wRet = IPMICMD_AMIGetRAIDConfig(pSession,(INT8U*)&AMIGetRAIDConfigReq,ReqLen,(INT8U*)&AMIGetRAIDConfigRes,timeout);
    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TINFO("Error while getting the SAS IT Controller's ( %d ) Alarm Element ( %d ) Info: Error %x", AlarmConfig.CtrlID, AlarmConfig.EnclID, wRet);
    }

    return wRet;
}

uint16 IPMICMD_AMIGetBIOSTable(IPMI20_SESSION_T *pSession,
                INT8U *pReq,uint32 ReqLen,INT8U *pRes, uint32 *wResLen,
                int timeout)
{
    uint16 wRet;

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                  ((NETFN_AMI << 2) + 0x00), CMD_AMI_PLDM_BIOS_MSG,
                                   (uint8 *)pReq, ReqLen ,
                                   (uint8 *)pRes, wResLen, timeout );

    return wRet;
}


uint16 LIBIPMI_HL_AMIGetBIOSTable(IPMI20_SESSION_T *pSession, BIOSResInfo_T *biosInfo, INT8U *payLoad, INT8U tableType, int timeout)
{
	uint16 wRet,ReqLen;
	uint32 ResLen = 0;
	INT8U *pRes = NULL;
	SetGetPLDMBIOSTableReq_T pSetGetPLDMBIOSTableReq;
	SetGetPLDMBIOSTableRes_T *pSetGetPLDMBIOSTableRes = NULL;

	pRes = calloc(1, BIOS_TABLE_MAX_READ_SIZE+ sizeof(SetGetPLDMBIOSTableRes_T));

	if(pRes == NULL)
	{
		TCRIT("Memory allocation error in LIBIPMI_HL_AMIGetBIOSTable \n");
		wRet = 0xFF;
		goto EXIT;
	}

	memset(&pSetGetPLDMBIOSTableReq,0x0,sizeof(SetGetPLDMBIOSTableReq_T));
	ReqLen = sizeof(pSetGetPLDMBIOSTableReq);

	pSetGetPLDMBIOSTableReq.PLDMMsgFields.PLDM_CommandCode = GET_BIOS_TABLE;
	pSetGetPLDMBIOSTableReq.PLDMMsgFields.InstanceID = 0x80;
	pSetGetPLDMBIOSTableReq.PLDMMsgFields.PLDMType = 0x03;
	pSetGetPLDMBIOSTableReq.PLDMTransferInfo.TableType = tableType;
	pSetGetPLDMBIOSTableReq.PLDMTransferInfo.DataTransferHandle =  biosInfo->DataTransferHandle;
	pSetGetPLDMBIOSTableReq.PLDMTransferInfo.TransferFlag = biosInfo->PLDMTransferFlag;

	wRet = IPMICMD_AMIGetBIOSTable(pSession,(INT8U*)&pSetGetPLDMBIOSTableReq,ReqLen,pRes,&ResLen,timeout);

	pSetGetPLDMBIOSTableRes = (SetGetPLDMBIOSTableRes_T *) pRes;

	if(wRet != LIBIPMI_E_SUCCESS )
	{
		TINFO("Error while getting the BIOS Table Info: Error %x",  wRet);
		goto EXIT;
	}

	if( (pSetGetPLDMBIOSTableRes->PLDMBIOSRes.PLDM_CompletionCode) == TABLE_NOT_FOUND )
	{
		TINFO(" BIOS Tables are not Found ");
		goto EXIT;
	}

	biosInfo->TableSize = ResLen - sizeof(SetGetPLDMBIOSTableRes_T);

	memcpy(payLoad,pRes+sizeof(SetGetPLDMBIOSTableRes_T),biosInfo->TableSize);
	biosInfo->DataTransferHandle =  pSetGetPLDMBIOSTableRes->PLDMBIOSRes.NextDataTransferHandle;
	biosInfo->CommonPLDMTransferFlag =  pSetGetPLDMBIOSTableRes->CommonPLDM.TransferFlag;

EXIT:

	if(pRes)
	{
		free(pRes);
		pRes = NULL;
	}
	return wRet;
}
