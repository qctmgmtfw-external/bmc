//;*****************************************************************;
//;*****************************************************************;
//;**                                                             **;
//;**     (C) COPYRIGHT American Megatrends Inc. 2011             **;
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
/*
 * SNMPApi.h
 *
 *  Created on: May 5, 2011
 *      Author: Michael Wozniak
 */

#ifndef SNMPAPI_H_
#define SNMPAPI_H_

#include "Types.h"

#define SNMPCFG_CONFIG_FILE "/conf/snmpcfg.conf"

#define SNMP_STATUS "SNMPStatus"
#define SNMP_ACCESS "SNMPAccess"
#define SNMP_AUTH_PROTOCOL "AUTHProtocol"
#define SNMP_PRIV_PROTOCOL "PrivProtocol"

#define MD5_CONF 2
#define SHA_CONF 1
#define DES_CONF 1
#define AES_CONF 2
#define RO_CONF 1
#define RW_CONF 2

typedef struct
{
	INT8U SNMPStatus;
	INT8U SNMPAccess;
	INT8U AUTHProtocol;
	INT8U PrivProtocol;
} SNMPConfigFile_T;

#define        READ_ONLY 0
#define        READ_WRITE 1
#define        MD5 0
#define        SHA 1
#define        DES 0
#define        AES 1

int AddSNMPUserPwd(unsigned char *userName,unsigned char *passwd, INT8U enctype1, INT8U enctype2, INT8U accessType);
void DelSNMPUser(unsigned char *userName);
int GetSNMPUser(char *Username,  SNMPConfigFile_T* retrievedConfigData);
int SetSNMPConfig(char * Username, SNMPConfigFile_T configData);
int DelFromSNMPConfig(char * Username);
int InitBackupConf(void);
int DelUserFromSnmpdConf(char *user);
int DelUserFromSnmpUserConf(char *user);
int ModUserSnmpdConf(char *user, INT8U enctype1, INT8U enctype2);
void CloseBackupConf(void);

#endif /* SNMPAPI_H_ */
