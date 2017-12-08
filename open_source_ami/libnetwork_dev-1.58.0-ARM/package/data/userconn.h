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
 * filename userconn.h
 *  Author: venkateshr@ami.com
 *
 ****************************************************************/
#include <sys/types.h>

#include <icc_what.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#if defined (ICC_OS_LINUX)
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <pwd.h>
#include <syslog.h>
#include <arpa/inet.h>
#endif

#include <errno.h>

/*
 * @define	MAX_IP_STR_LEN
 * @brief	Maximum length of an IPV4/IPV6 string 
 */
#define MAX_IP_STR_LEN	64

/*
 * @define	MAX_USERNAME
 * @brief	Maximum length of an  user name string
 */
#define MAX_USERNAME	256

/*
 * @define	MAX_PROGNAME
 * @brief	Maximum length of an application name 
 */
#define MAX_PROGNAME	256

/*
 * @define	MAX_FNAME
 * @brief	Maximum length of a filename/path string 
 */
#define MAX_FNAME		256

/*
 * @define	MAX_PROC_NET_TCP_LINE
 * @brief	Maximum length of a an entry in kernel tcp proc entry 
 */
#define MAX_PROC_NET_TCP_LINE	256


/*
 * @define	PROC_NET_TCPV4
 * @brief	Path name for kernel TCPv4 connection list
 */
#define PROC_NET_TCPV4 "/proc/net/tcp"

/*
 * @define	PROC_NET_TCPV6
 * @brief	Path name for kernel TCPv6 connection list 
 */
#define PROC_NET_TCPV6 "/proc/net/tcp6"

/*
 * @define	PROC_NET_TCPV4_FORMAT_STR
 * @brief	Format of each line in the TCPv4 proc entry 
 */
#define PROC_NET_TCPV4_FORMAT_STR "%u: %8s:%4x %8s:%4x %02x %s %s %u %u %u %u"

/*
 * @define	PROC_NET_TCPV6_FORMAT_STR
 * @brief	Format of each line in the TCPv6 proc entry 
 */
#define PROC_NET_TCPV6_FORMAT_STR "%u: %32s:%4x %32s:%4x %02x %s %s %u %u %u %u"



/*
 * @define	BYTEN(x)
 * @brief	Macro to split 32 bit integer 
 */
#ifdef CFG_PROJ_ENDIAN_BIG
#define BYTE1(x) (((x) >> 24) & 0xFF)
#define BYTE2(x) (((x) >> 16) & 0xFF)
#define BYTE3(x) (((x) >> 8) & 0xFF)
#define BYTE4(x) ((x) & 0xFF)
#else
#define BYTE1(x) ((x) & 0xFF)
#define BYTE2(x) (((x) >> 8) & 0xFF)
#define BYTE3(x) (((x) >> 16) & 0xFF)
#define BYTE4(x) (((x) >> 24) & 0xFF)
#endif

/*
 * @define	BYTEN_V6(x)
 * @brief	Macro to shift 128 bit integer (ipv6 address)
 */
#ifdef CFG_PROJ_ENDIAN_BIG
#define BYTE1_V6(x) (((x) >> 48) & 0xFFFF)
#define BYTE2_V6(x) (((x) >> 32) & 0xFFFF)
#define BYTE3_V6(x) (((x) >> 16) & 0xFFFF)
#define BYTE4_V6(x) ((x) & 0xFFFF)
#else
#define BYTE1_V6(x) ((x) & 0xFFFF)
#define BYTE2_V6(x) (((x) >> 16) & 0xFFFF)
#define BYTE3_V6(x) (((x) >> 32) & 0xFFFF)
#define BYTE4_V6(x) (((x) >> 48) & 0xFFFF)
#endif

/*
 * @define	IPV4NOTN
 * @brief	Format of an IPv4 string notated in IPv6 format 
 */
#ifdef CFG_PROJ_ENDIAN_BIG
#define IPV4NOTN "00000000000000000000FFFF"
#else
#define IPV4NOTN "0000000000000000FFFF0000"
#endif

#define LEN_IPV4NOTN (sizeof(IPV4NOTN) - 1)

#define IPV6ZERONOTN "00000000000000000000000000000000"
#define IPV6LOOPBACKNOTN "00000000000000000000000000000001"

#define LEN_IPV6_64BIT_CHAR 	16

/*
 * @define	TCP_STATE_ESTB
 * @brief	Connection state of TCP as maintained by kernel 
 */
#define TCP_STATE_ESTB	1


/*
 * @define	tcp_conn_list_t
 * @brief	Entry for each kernel TCP connection 
 */
typedef struct tcp_conn_list tcp_conn_list_t;

struct tcp_conn_list 
{
	char tcl_local_ip[ MAX_IP_STR_LEN ];
	char tcl_remote_ip[ MAX_IP_STR_LEN ];
	int tcl_local_port;
	int	tcl_remote_port;
	int	tcl_state;
	int	tcl_conn_uid;
	int	tcl_inode;
	tcp_conn_list_t *next;
};


/*
 * @define	usr_conn_list_t
 * @brief	Entry for username to connection mapping for each connection 
 */
typedef struct user_conn_list usr_conn_list_t;

struct user_conn_list
{
	char	ucl_username[ MAX_USERNAME ];
	int		ucl_uid;
	char	ucl_progname[ MAX_USERNAME ];
	char	ucl_local_ip[ MAX_IP_STR_LEN ];
	char	ucl_remote_ip[ MAX_IP_STR_LEN ];
	int		ucl_local_port;
	int		ucl_remote_port;
	usr_conn_list_t *next;
};

/*
 * @fn          build_tcp_list
 * @brief       Builds a list of established TCP connections in the system
 * @return      tcp_conn_list_t or NULL
 *
 */
tcp_conn_list_t *build_tcp_list(void);


/*
 * @fn          get_user_conn_list
 * @brief       To get a list of open TCP connections for all users
 * @return      An linked list of users-connection information
 *                      or NULL for error or no connections
 */
usr_conn_list_t *get_user_conn_list();


/*
 * @fn          free_user_conn_list
 * @brief       Frees all user-connection linked list
 * @param       [in] usr_conn_list_t structure to be freed
 * @return      None
 */
void free_user_conn_list(usr_conn_list_t *userlist);

/*
 * @fn		free_tcp_conn_list
 * @brief	Frees all tcp-connection linked list 
 * @param	[in] tcp_conn_list_t structure to be freed
 * @return	None 	
 */
void free_tcp_conn_list(tcp_conn_list_t *tcplist);


