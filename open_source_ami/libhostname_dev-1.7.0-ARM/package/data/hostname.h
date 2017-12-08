/*
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2006-2009, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 */

#ifndef _HOSTNAME_H
#define _HOSTNAME_H

#define HOSTNAME_CFG_FILE	"/etc/hostname"
#define HOSTS_CFG_FILE		"/etc/hosts"
#define HOSTNAME_SETTINGS_FILE	"/conf/hostname.conf"

#define MAX_HOSTNAME_LEN	64
#define MAX_MAC_LEN	64


/* One MUST pass a pointer to a string of length MAX_HOSTNAME_LEN to the 
   following function */
int GetHostName(char *name);

int SetHostName(char *name);

int SetDefaultHostName(void);

int GetRequestedHostName(char *name);

int GetHostName(char *name);

int GetMACAddr(char *);

int GetHostNameConf(char *name, int* hostsetting);

int SetHostNameConf(char *name, int hostsetting);

#endif
