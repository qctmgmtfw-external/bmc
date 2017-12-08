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
 * Filename : tcp.h
 * Author   : Aruna. V (arunav@amiindia.co.in)
 *	    : Dipankar (dipankarpatra@amiindia.co.in)
 ****************************************************************/


#ifndef _TCP_H
#define _TCP_H

#include <ctype.h>
#include "netports.h" 

#define SSH_CLP_INDEX		0
#define SSH_CLI_INDEX		2
#define TNET_CLP_INDEX		4
#define TNET_CLI_INDEX		6
#define CIM_HTTP_INDEX		8
#define CIM_HTTPS_INDEX		10
#define WEB_HTTP_INDEX		12
#define WEB_HTTPS_INDEX		14
#define STR_LEN_128		128
#define STR_LEN_1024		1024
#define NCML_CONF_FILE		"/conf/ncml.conf"
#define SSHD_CONF_FILE		"/etc/ssh/sshd_config"
#define NONSECURE_PORT		"nonscecure_port"

unsigned long g_telnet_pulport;
unsigned long g_ssh_pulport;

/*
 * Structure Name : portsList
 * Description    : This structure stores all the ports 
		    (listening, self, remote, configured) 
		    with its services and type in order to form
 		    the instances for TCPProtocolEndpoint.
*/

/*
 * @define      NETSTAT_OPTIONS_LISTEN_TCP_NUM_PROC
 * @brief       options to be used for netstat listen command
 */
#define NETSTAT_OPTIONS_LISTEN_TCP_NUM_PROC "-tln"

/*
 * @define      NETSTAT_OPTIONS_ESTABLISHED_TCP_NUM_PROC
 * @brief       options to be used for netstat established command
 */
#define NETSTAT_OPTIONS_ESTABLISHED_TCP_NUM_PROC "-tn"

/** Forms the ports list using netstat command **/
int TCP_GetPortsList(Ports_list ** ppPortsList, unsigned long * pulPort);

/** Gets a single particular instance **/
int TCP_GetPortByName(char * pstrName, unsigned long * pUlPort);

/** Fills the ports from netstat command **/
int TCP_GetPorts(unsigned long *pTelnetPort, unsigned long *pSSHPort);

/** read the /etc/ncml.conf file store the port value **/
int get_telnet_port(unsigned long *pPort, char *name);

/**Searches for the given key in the given line. If found,
 	reads the rest of the line as value and extracts the port **/
int get_port_from_line(char *pStrLine, char *pStrKey,
                                        unsigned long *pulPort);
/** Reads the SSHCLP Port **/
int get_ssh_port(char * pStrFileName, unsigned long * pUlPort);

/** Returns the service name of the given port **/
void TCP_GetServiceName(unsigned long * pUlPort,
                    unsigned long ulLocalPort,
                    char * pstrServiceName, char cType);

 /** Returns the service name of the given port. **/
void TCP_GetConfiguredServiceName(unsigned long pUlPort,
                    char * pstrServiceName, char cType);

#endif /* _NETPORTS_H */
