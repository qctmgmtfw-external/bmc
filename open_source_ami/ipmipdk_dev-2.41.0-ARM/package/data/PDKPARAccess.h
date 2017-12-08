/******************************************************************
 ******************************************************************
 ***                                                                                                           **
 ***    (C)Copyright 2011, American Megatrends Inc.                            **
 ***                                                                                                           **
 ***    All Rights Reserved.                                                                          **
 ***                                                                                                           **
 ***    5555 , Oakbrook Pkwy, Norcross,                                                       **
 ***                                                                                                           **
 ***    Georgia - 30093, USA. Phone-(770)-246-8600.                                  **
 ***                                                                                                           **
 ******************************************************************
 ******************************************************************
 ******************************************************************
 *
 * PDKPARAccess.h
 * PDK PAR Hooks Access methods
 *
 *  Author: Winston <winstonv@amiindia.co.in>
 
 ******************************************************************/
#ifndef _PDK_PAR_ACCESS_
#define _PDK_PAR_ACCESS_

#include "PDKAccess.h"
//#define PDKCMDS_LIB "/usr/local/lib/libipmipdkcmds.so"


typedef enum
{
    PDKPAR_USERINFOLOAD =0,
    PDKPAR_USERINFOSAVE,
    PDKPAR_DCMIINFOLOAD,
    PDKPAR_DCMIINFOSAVE,
    PDKPAR_CHINFOLOAD,
    PDKPAR_CHINFOSAVE,
    PDKPAR_WDTINFOLOAD,
    PDKPAR_WDTINFOSAVE,
    PDKPAR_PEFINFOLOAD,
    PDKPAR_PEFINFOSAVE,
    PDKPAR_CHASSISINFOLOAD,
    PDKPAR_CHASSISINFOSAVE,
    PDKPAR_SERIALINFOLOAD,
    PDKPAR_SERIALINFOSAVE,
    PDKPAR_BRIDGEINFOLOAD,
    PDKPAR_BRIDGEINFOSAVE,
    PDKPAR_SYSTEMINFOLOAD,
    PDKPAR_SYSTEMINFOSAVE,
    PDKPAR_LAN1INFOLOAD,
    PDKPAR_LAN1INFOSAVE,
    PDKPAR_LAN2INFOLOAD,
    PDKPAR_LAN2INFOSAVE,
    PDKPAR_LAN3INFOLOAD,
    PDKPAR_LAN3INFOSAVE,
    PDKPAR_RMCP1INFOLOAD,
    PDKPAR_RMCP1INFOSAVE,
    PDKPAR_RMCP2INFOLOAD,
    PDKPAR_RMCP2INFOSAVE,
    PDKPAR_RMCP3INFOLOAD,
    PDKPAR_RMCP3INFOSAVE,
    PDKPAR_SOL1INFOLOAD,
    PDKPAR_SOL1INFOSAVE,
    PDKPAR_SOL2INFOLOAD,
    PDKPAR_SOL2INFOSAVE,
    PDKPAR_SOL3INFOLOAD,
    PDKPAR_SOL3INFOSAVE,
    PDKPAR_FIREWALLINFOLOAD,
    PDKPAR_FIREWALLINFOSAVE,
    PDKPAR_OPMAINFOLOAD,
    PDKPAR_OPMAINFOSAVE,
    PDKPAR_SMTP1INFOLOAD,
    PDKPAR_SMTP1INFOSAVE,
    PDKPAR_SMTP2INFOLOAD,
    PDKPAR_SMTP2INFOSAVE,
    PDKPAR_SMTP3INFOLOAD,
    PDKPAR_SMTP3INFOSAVE,
    PDKPAR_GENERICINFOLOAD,
    PDKPAR_GENERICINFOSAVE,
    PDKPAR_IPMBCHINFOLOAD,
    PDKPAR_IPMBCHINFOSAVE,
    PDKPAR_LANCH1INFOLOAD,
    PDKPAR_LANCH1INFOSAVE,
    PDKPAR_LANCH2INFOLOAD,
    PDKPAR_LANCH2INFOSAVE,
    PDKPAR_LANCH3INFOLOAD,
    PDKPAR_LANCH3INFOSAVE,
    PDKPAR_SMLINKCHINFOLOAD,
    PDKPAR_SMLINKCHINFOSAVE,
    PDKPAR_ICMBCHINFOLOAD,
    PDKPAR_ICMBCHINFOSAVE,
    PDKPAR_SMBUSCHINFOLOAD,
    PDKPAR_SMBUSCHINFOSAVE,
    PDKPAR_SERIALCHINFOLOAD,
    PDKPAR_SERIALCHINFOSAVE,
    PDKPAR_SMMCHINFOLOAD,
    PDKPAR_SMMCHINFOSAVE,
    PDKPAR_SYSCHINFOLOAD,
    PDKPAR_SYSCHINFOSAVE,
    PDKPAR_TRIGGEREVTINFOLOAD,
    PDKPAR_TRIGGEREVTINFOSAVE,
    PDKPAR_LOGINAUDITINFOLOAD,
    PDKPAR_LOGINAUDITINFOSAVE,
    PDKPAR_AMIINFOLOAD,
    PDKPAR_AMIINFOSAVE,
#ifdef CONFIG_SPX_FEATURE_SSICOMPUTEBLADE_SUPPORT
    PDKPAR_SSIINFOLOAD,
    PDKPAR_SSIINFOSAVE,
#endif
#ifdef CONFIG_SPX_FEATURE_IPMI_CONFIG_VER_CHECK
    PDKPAR_VERSIONINFOLOAD,
    PDKPAR_VERSIONINFOSAVE,
#endif
    MAX_PDKPAR_HANDLE
}PDKPAR_HOOK;

extern pfunc g_PDKPARHandle[MAX_PDKPAR_HANDLE];
extern int InitPDKPARHooks(int BMCInst);


#endif //_PDK_PAR_ACCESS_

