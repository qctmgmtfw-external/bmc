/****************************************************************
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2007, American Megatrends Inc.             **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        6145-F, Northbelt Parkway, Norcross,                **
 **                                                            **
 **        Georgia - 30071, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 *****************************************************************/

#ifndef LIBIPMI_AMIEXT_H_
#define LIBIPMI_AMIEXT_H_

/* LIBIPMI core header files */
#include "libipmi_session.h"
#include "libipmi_errorcodes.h"
#include "libipmiifc.h"

#include <Types.h>

/* command specific header files */
#include "IPMI_AMIDevice.h"
#include "IPMI_AMISmtp.h"
#include "IPMI_AMIConf.h"

#ifdef __cplusplus
extern "C" {
#endif

LIBIPMI_API uint16 IPMICMD_AMIGetSMTPServerInfo( IPMI20_SESSION_T *pSession,
								char	*smtpServerCfgInfo, unsigned char Channel,
                             int timeout );

LIBIPMI_API uint16 IPMICMD_AMIGetSMTPServerInfo_IPv6( IPMI20_SESSION_T *pSession,
								char	*smtpServerIP, unsigned char Channel,
                             int timeout );

LIBIPMI_API uint16 IPMICMD_AMISetSMTPServerInfo( IPMI20_SESSION_T *pSession,
								char	*smtpServerIP,unsigned char Channel,
                             int timeout );

LIBIPMI_API uint16 IPMICMD_AMISetSMTPServerInfo_IPv6( IPMI20_SESSION_T *pSession,
								char	*smtpServerIP,unsigned char Channel,
                             int timeout );
LIBIPMI_API uint16 IPMICMD_AMIGetSMTPServer2Info( IPMI20_SESSION_T *pSession,
								char	*smtpServerCfgInfo, unsigned char Channel,
                             int timeout );

LIBIPMI_API uint16 IPMICMD_AMIGetSMTPServer2Info_IPv6( IPMI20_SESSION_T *pSession,
								char	*smtpServerIP, unsigned char Channel,
                             int timeout );

LIBIPMI_API uint16 IPMICMD_AMISetSMTPServer2Info( IPMI20_SESSION_T *pSession,
								char	*smtpServerIP,unsigned char Channel,
                             int timeout );

LIBIPMI_API uint16 IPMICMD_AMISetSMTPServer2Info_IPv6( IPMI20_SESSION_T *pSession,
								char	*smtpServerIP,unsigned char Channel,
                             int timeout );

LIBIPMI_API uint16 IPMICMD_AMIGetEmailDest( IPMI20_SESSION_T *pSession,
								uint8	index,
								uint8 *Userid,
								char *Subject,
								char *Msg,unsigned char Channel,
                             int timeout );

LIBIPMI_API uint16 IPMICMD_AMISetEmailDest(IPMI20_SESSION_T *pSession,
					uint8 DestSel,
					uint8 UserID,
					uint8* pSubject,
					uint8* pMsg,unsigned char Channel,
					int timeout);


/*LIBIPMI_API uint16 IPMICMD_AMISetSMTPInfo(IPMI20_SESSION_T *pSession,
					uint8 *Username,
					uint8 *Passwd,
					uint8* SenderAddr,
					uint8* Machinename,
					uint8 Channel,
					int timeout);

LIBIPMI_API uint16 IPMICMD_AMIGetSMTPInfo(IPMI20_SESSION_T *pSession,
						uint8 *Username,
						uint8 *passwd,
						uint8* SenderAddr,
						uint8* Machinename,
						uint8 Channel,
						int timeout);

*/
LIBIPMI_API uint16 IPMICMD_AMISetSMTPInfo(IPMI20_SESSION_T *pSession,
                            SmtpInfoParam_T *SmtpInfo,
                            uint8 Channel,
                            int timeout);

LIBIPMI_API uint16 IPMICMD_AMIGetSMTPInfo(IPMI20_SESSION_T *pSession,
                            SmtpInfoParam_T *SmtpInfo,
                            uint8 Channel,
                            int timeout);

LIBIPMI_API uint16 IPMICMD_AMISetSMTP2Info(IPMI20_SESSION_T *pSession,
                            SmtpInfoParam_T *SmtpInfo,
                            uint8 Channel,
                            int timeout);

LIBIPMI_API uint16 IPMICMD_AMIGetSMTP2Info(IPMI20_SESSION_T *pSession,
                            SmtpInfoParam_T *SmtpInfo,
                            uint8 Channel,
                            int timeout);

LIBIPMI_API uint16 IPMICMD_AMIGetUserEmail( IPMI20_SESSION_T *pSession,
                                uint8 uid,
                                char *pEmailAddr,
                                int timeout );


LIBIPMI_API uint16 IPMICMD_AMISetUserEmail( IPMI20_SESSION_T *pSession,
                                uint8 uid,
                                char *pEmailAddr,
                                int timeout );

LIBIPMI_API uint16 IPMICMD_AMIGetUserEmailFormat( IPMI20_SESSION_T *pSession,
                                uint8 uid,
                                char *pEmailFormat,
                                int timeout );


LIBIPMI_API uint16 IPMICMD_AMISetUserEmailFormat( IPMI20_SESSION_T *pSession,
                                uint8 uid,
                                char *pEmailFormat,
                                int timeout );

LIBIPMI_API uint16 IPMICMD_AMIResetUserPassword (IPMI20_SESSION_T *pSession,
                                uint8 uid,
                                uint8 channel,
                                int timeout );

LIBIPMI_API uint16 IPMICMD_AMIRestoreFactoryDefaults (IPMI20_SESSION_T *pSession,
				int timeout );

LIBIPMI_API uint16  IPMICMD_AMISetRootPassword(IPMI20_SESSION_T *pSession,
				AMISetRootPasswordReq_T    *pReqSetRootPswd,
				AMISetRootPasswordRes_T    *pResSetRootPswd, 
				int timeout);
LIBIPMI_API uint16  IPMICMD_AMIGetRootUserAccess(IPMI20_SESSION_T *pSession,
				AMIGetRootUserAccessRes_T    *pResGetRootAccess, 
				int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetRootUserAccess( IPMI20_SESSION_T *pSession,
				INT8U UserID,INT8U *UserAccessState,
				int timeout );
LIBIPMI_API uint16 LIBIPMI_HL_AMISetRootPassword( IPMI20_SESSION_T *pSession,
				INT8U UserID,INT8U IsPwdChange,
				char* ClearPswd,INT8U EnableUser,
				int timeout );

LIBIPMI_API uint16 IPMICMD_AMIGetAllDNSConf( IPMI20_SESSION_T *pSession,
                             AMIGetDNSConfRes_T *pGetLANEnableRes,
                             int timeout );

LIBIPMI_API uint16 IPMICMD_Set_DNS_Conf( IPMI20_SESSION_T *pSession,
                             AMISetDNSConfReq_T* pSetDNSConfReq,
                             unsigned int ReqDataLen,
                             AMISetDNSConfRes_T *pSetDNSConfRes,
                             int timeout );

LIBIPMI_API uint16 IPMICMD_Get_DNS_Conf( IPMI20_SESSION_T *pSession,
                             AMIGetDNSConfReq_T *pGetDNSConfReq,
                             AMIGetDNSConfRes_T *pGetDNSConfRes,
                             uint32* ResLen,
                             int timeout );

LIBIPMI_API uint16 LIBIPMI_HL_GetAllSeviceConf (IPMI20_SESSION_T *pSession, 
                           GetServiceInfo_T *GetServiceInfo, int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_AMIGetServiceConf(IPMI20_SESSION_T *pSession,
                           INT32U ServiceID, AMIServiceConfig_T* pAMIGetServiceCfgRes,int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_AMISetServiceConf(IPMI20_SESSION_T *pSession, 
                             AMIServiceConfig_T *pSetServiceCfgReq,int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_AMIGetEthIndex(IPMI20_SESSION_T *pSession,
                            INT8U Channel,INT8U *EthIndex,int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_AMISetPamOrder (IPMI20_SESSION_T *pSession, 
                             AMISetPamReq_T *pAMISetPamReq, int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_AMIGetPamOrder (IPMI20_SESSION_T *pSession, 
                             INT8U *PamOrder, int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_AMISetDNSHostSetting(IPMI20_SESSION_T *pSession,INT8U* HostSetting,int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_AMIGetDNSHostSetting(IPMI20_SESSION_T *pSession,INT8U* HostSetting,int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_AMISetDNSRegister(IPMI20_SESSION_T *pSession,INT8U* RegisterBMC,int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_AMIGetDNSRegister(IPMI20_SESSION_T *pSession,INT8U* RegisterBMC,int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_AMISetDNSDomainSetting(IPMI20_SESSION_T *pSession,INT8U *DomainConf,int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_AMIGetDNSDomainSetting(IPMI20_SESSION_T *pSession,INT8U *DomainConf,int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_AMISetDNSDomainName(IPMI20_SESSION_T *pSession,INT8U Block,INT8U *DomainName,int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_AMIGetDNSDomainName(IPMI20_SESSION_T *pSession,INT8U Block,INT8U *DomainName,int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_AMISetDNSSetting(IPMI20_SESSION_T *pSession,INT8U* DNSConf,int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_AMIGetDNSSetting(IPMI20_SESSION_T *pSession,INT8U *DNSConf,int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_AMISetDNSIPAddress(IPMI20_SESSION_T *pSession,INT8U Block,INT8U *IPAddr,INT8U AddrLen,int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_AMIGetDNSIPAddress(IPMI20_SESSION_T *pSession,INT8U Block,INT8U *IPAddr,INT8U* AddrLen,int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_AMISetDNSRestart(IPMI20_SESSION_T *pSession,int timeout);

#ifdef CONFIG_SPX_FEATURE_SNMP_SUPPORT
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetSNMPConf(IPMI20_SESSION_T *pSession,
							INT8U pGetSNMPConf, char * Username, INT8U *SNMPStatus, 
							INT8U *SNMPAccess, INT8U *AUTHProtocol, 
							INT8U *PrivProtocol, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMISetSNMPConf(IPMI20_SESSION_T *pSession,
							INT8U pSetSNMPConf, char * Username, char * Password, 
							INT8U SNMPStatus, INT8U SNMPAccess, INT8U AUTHProtocol, 
							INT8U PrivProtocol, int timeout);
#endif//SNMP_SUPPORT


#ifdef __cplusplus
}
#endif

#endif
