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
 * HelperFuncs.h
 *
 *  Created on: May 5, 2011
 *      Author: Michael Wozniak
 */

#ifndef HELPERFUNCS_H_
#define HELPERFUNCS_H_

#ifndef SNMPD_PWD_CONF
#define SNMPD_PWD_CONF      "/conf/snmp/snmpd.conf"
#endif

#ifndef SNMPD_CONF
#define SNMPD_CONF "/conf/snmp_users.conf"
#endif

unsigned char str2int(unsigned char *st);
void charstr2intstr(unsigned char *string,unsigned int *length);
void ConvertUserNameToHex(char *username, char *hexusername);

void GetEngineID(FILE *fptr, char *idStr);
/* Returns: EngineID string stored inside snmpd.conf */

int stop_snmp(void);
int start_snmp(void);


#endif /* HELPERFUNCS_H_ */
