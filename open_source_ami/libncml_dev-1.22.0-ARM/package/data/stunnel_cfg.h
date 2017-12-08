/*****************************************************************
******************************************************************
***                                                            ***
***        (C)Copyright 2010, American Megatrends Inc.         ***
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
* Filename: stunnel_cfg.h
*
* Description: Contains code for the Stunnel Config file manipulation APIs
*
***************************************************************/

#ifndef _STUNNEL_CFG_H
#define _STUNNEL_CFG_H

#include <Types.h>

#define 	STUNNEL_CFG_FILE	"/conf/stunnel.conf"
#define 	TMP_STUNNEL_FILE	"/conf/stunnel_tmp.conf"
#define 	STUNNEL4_RESTART	"/etc/init.d/stunnel4 restart"

#define		VKVM_SERVICE_NAME	"kvm"

#define		TOTAL_STUNNEL_SECTIONS	5
#define		STUNNEL_SECTION_SIZE	3
#define		MEDIA_STUNNEL_MODULES	3
#define 	KVM_STUNNEL_MODULES	1

#define 	ADVISER_STUNNEL_NAME	"adviserd"
#define		CDSERVER_STUNNEL_NAME	"cdserver"
#define		FDSERVER_STUNNEL_NAME	"fdserver"
#define		HDSERVER_STUNNEL_NAME	"hdserver"
#define		VFSERVER_STUNNEL_NAME	"vfserver"

int FindEntryInStunnel	(char *Name);
int AddStunnelEntry	(char *Name, int RestartStunnel);
int RemoveStunnelEntry	(char *Name, int RestartStunnel);
int UpdateStunnelEntry	(char *Name);

#endif
