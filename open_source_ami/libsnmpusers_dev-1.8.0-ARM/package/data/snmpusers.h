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
 * snmpusers.h
 *
 *  Created on: May 5, 2011
 *      Author: Michael Wozniak
 */

#ifndef SNMPUSERS_H_
#define SNMPUSERS_H_
int SNMPAddUser(char* UserName,char* ClearPassword, INT8U access, INT8U enc_type_1, INT8U enc_type_2);
int SNMPDelUser(char* UserName);
int SNMPGetUser(char* UserName, INT8U *SNMPStatus, INT8U *SNMPAccess, INT8U *AUTHProtocol, INT8U *PrivProtocol);

#endif /* SNMPUSERS_H_ */
