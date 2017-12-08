/*****************************************************************
 *****************************************************************
 ***                                                            **
 ***    (C)Copyright 2005-2006, American Megatrends Inc.        **
 ***                                                            **
 ***            All Rights Reserved.                            **
 ***                                                            **
 ***        6145-F, Northbelt Parkway, Norcross,                **
 ***                                                            **
 ***        Georgia - 30071, USA. Phone-(770)-246-8600.         **
 ***                                                            **
 *****************************************************************
 *****************************************************************
 ******************************************************************
 *
 * Iptables.h
 * .
 *
 *  Author: AMI MegaRAC PM Team
 ******************************************************************/


#define IP_IPV4						       0x00 
#define IPS_IPV4					       0x01
#define PORT		 				       0x02
#define PORT_RANGE					       0x03	
#define IP_IPV4_Unlock					   0x04
#define IPS_IPV4_Unlock					   0x05
#define PORT_Release					   0x06
#define PORT_RANGE_Release				   0x07
#define FLUSH 						       0x08
#define DISABLE_ALL					       0x09
#define REMOVE_DISABLE_ALL				   0x0a

#define GET_IPTABLE_COUNT				   0x00
#define GET_ENTRY_INFO					   0x01
#define IS_BLOCK_ALL                                      0x02

typedef struct
{
    INT8U protocol;
    INT16U Port_NO;

} PACKED Port;

typedef struct
{
    INT8U protocol;
    INT16U starting_port;
    INT16U closing_port;

} PACKED RANGE_Prt;

typedef struct
{
    INT8U starting_ip[4];
    INT8U closing_ip[4];

} PACKED RANGE_Ipv4;

typedef union 
{
    INT8U IPAddr_ipv4[4];
    RANGE_Ipv4 IPRange_ipv4;
    Port  Port_Data;
    RANGE_Prt Port_Range;
	
} FirewallConfUn_T; 

typedef struct
{
    INT8U TYPE;
    INT8U State;
    FirewallConfUn_T Entry_info;
    
} PACKED GetFirewallConf_T;



/* Prototype Declarations */

void *block_ip_ipv4 ( void *, void *);
void *block_ip_range_ipv4 ( void *, void * );
void *block_port ( void *, void * );
void *block_range_ports ( void *, void *);
void *release_range_port ( void *port_range, void * );
void *release_port ( void *port_in, void *);
void *release_range_ip_ipv4 ( void *range_in, void * );
void *release_ip_ipv4 ( void *ip_in, void * );
void convert_IPv4_string (INT8U *IP_array, INT8U *IPv4 );
void *flush_iptables ( void *in );
void *enable_all ( void *in);
void *block_all ( void *in);

int get_iptable_count ();
int get_iptable_entry ( INT8U *pos, GetFirewallConf_T *pFWc);
int IsBlockAllEnabled();
/***********************************/


