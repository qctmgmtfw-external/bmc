/****************************************************************
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2005-2007, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        6145-F, Northbelt Parkway, Norcross,                **
 **                                                            **
 **        Georgia - 30071, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 ****************************************************************
 ****************************************************************
 ****************************************************************
 *
 *  Filename : netports.h
 *  Author   : Aruna. V  (arunav@amiindia.co.in)
 *
 ****************************************************************/

#ifndef _NETPORTS_H
#define _NETPORS_H

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "nwcfg.h"

/*
 * @define      NW_MAX_LINE_LEN
 * @brief       Maximum length of a line
 */

#define NW_MAX_LINE_LEN   1024

#define SUCCESS		0
#define FAILURE		1

#define LISTEN			'l'
#define LOCAL			's'
#define REMOTE			'r'
#define CONFIGURE		'c'

/*
 * @define      NETSTAT_BIN_PATH
 * @brief       Path of the netstat in the system
 */
#define NETSTAT_BIN_PATH "/bin/netstat"

/*
 * @define      GREP_BIN_PATH
 * @brief       Path of the grep in the system
 */
#define GREP_BIN_PATH "/bin/grep"

/*
 * @define      SED_BIN_PATH
 * @brief      Path of the sed in the system
 */
#define SED_BIN_PATH "/bin/sed"

/*
 * @define      SED_OPTIONS_COLON_TO_SPACE
 * @brief       options to be used for sed command in order to 
		replace ':' to space
 */
#define SED_OPTIONS_COLON_TO_SPACE "-r s/':'/' '/g"

/*
 * @define      SED_OPTIONS_DCOLON_TO_SPACE
 * @brief       options to be used for sed command in order to 
		replace '::' to star
 */
#define SED_OPTIONS_DCOLON_TO_SPACE "-r s/'::'/'*'/g"

/*
 * @define      SED_OPTIONS_MULTISPACE_TO_SPACE
 * @brief       options to be used for sed command in order to 
		replace Multi-space with space
 */
#define SED_OPTIONS_MULTISPACE_TO_SPACE "-r s/'  *'/' '/g"

/*
 * @define      SED_OPTIONS_HOSTNAME_TO_SPACE
 * @brief       options to be used for sed command in order to 
		replace '::ffff' to space
 */
#define SED_OPTIONS_HOSTNAME_TO_SPACE "-r s/'::ffff:'/''/g"

/*
 * Structure Name : netstat_est_data
 * Description    : This structure has members for holding the output 
		    values of Netstat Established command.
*/
#ifndef _NETSTATEST_DATA
#define _NETSTATEST_DATA

typedef struct netstat_est_data {
	char localAddress[16];
	char remoteAddress[16];
	unsigned long ulLocalPort;
	unsigned long ulRemotePort;
	struct netstat_est_data * next;
} NetstatEst_data;

#endif /*_NETSTATEST_DATA*/

/*
 * Structure Name : netstat_listen_data
 * Description    : This structure has members for holding the 
		    output values of Netstat Listen command.
*/
#ifndef _NETSTATLISTEN_DATA
#define _NETSTATLISTEN_DATA

typedef struct netstat_listen_data {
	char localAddress[16];
	unsigned long ulLocalPort;
	struct netstat_listen_data * next;
} NetstatListen_data;

#endif /*_NETSTATLISTEN_DATA*/


/*
 * Structure Name : portsList
 * Description    : This structure stores all the ports 
		    (listening, self, remote, configured) 
		    with its services and type in order to form
 		    the instances for TCPProtocolEndpoint.
*/
#ifndef _PORTS_LIST
#define _PORTS_LIST

typedef struct portsList {
	char service[64];
	unsigned long port;
	char type[64];
	struct portsList * next;
} Ports_list;

#endif /*_PORTS_LIST*/

/** Free the Netstat Listen structure **/
void free_NetstatListen_list(NetstatListen_data * pNetLData);

/** Frees the netstat established structure **/
void free_NetstatEst_list(NetstatEst_data * pNetEData);

/** Frees the Ports List **/
void free_Ports_list(Ports_list * pPortsList);

/** Checks whether the IP:Port combination already exist **/
int CheckIfIPPortExist(NetstatListen_data * pNetListenData,
		       NetstatEst_data * pNetEstData,
		       char * pStrAddress,
		       unsigned long ulPort);

/** Adds the given port, along with its service to Ports List **/
int AddToPortsList(Ports_list ** ppPortsList,
		   unsigned long ulPort, 
		   char * pStrServiceName,
		   char cType, char * pstrIP);

/** Collect IPs through IOCTL calls **/
int CollectIPs(char *** pppStrIP, int * pNIPCount);

void ConvertIPnumToStr(unsigned char *var, unsigned int len,
                       char *string);

#endif /* _NETPORTS_H */
