/***************************************************************
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
* Filename: nwcfg.h
*
* Description: Contains code for the basic network library APIs.
*
****************************************************************/

#ifndef _NWCFG_H
#define _NWCFG_H

#ifdef ICC_OS_WINDOWS
#ifndef INET6_ADDRSTRLEN
#define INET6_ADDRSTRLEN 46
#endif
#ifndef INET_ADDRSTRLEN
#define INET_ADDRSTRLEN 16
#endif
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#include <Types.h>
#include "hostname.h"

#ifdef __GNUC__
#define PACKED __attribute__ ((packed))
#else
#define PACKED
#pragma pack( 1 )
#endif

/* External dependencies */
#define	PROC_NET_ROUTE_FILE	"/proc/net/route"
/* ipv6 */
#define	PROC_NET_IPV6ROUTE_FILE "/proc/net/ipv6_route"
#define DHCPD_PID_FILE		"/var/run/udhcpc.eth0.pid"
#define DHCP6C_PID_FILE		"/var/run/dhcp6c.pid"

#define DHCPD_CONFIG_FILE   "/conf/dhcpc-config"
#define DHCPD_CONFIG_FILE_TEMP      "/var/dhcp-config.tmp"
#define RESOLV_CONF_FILE    "/conf/resolv.conf"
#define RESOLV_CONF_FILE_TEMP    "/var/resolv.conf.tmp"
#define DNS_CONFIG_FILE   "/conf/dns.conf"
#define DNS_CONFIG_FILE_TEMP      "/var/dns.conf.tmp"
#define IFUP_BIN_PATH       "/sbin/ifup"
#define IFDOWN_BIN_PATH     "/sbin/ifdown"
#define ZERO_IP         "0.0.0.0"
#define DEFAULT_GW_STR      "0.0.0.0"
/* /etc/network/interfaces file defines */
#define NETWORK_IF_FILE		"/etc/network/interfaces"
#define NETWORK_IF_FILE_TMP         "/conf/interfaces.tmp"
#define NETWORK_DNS_TMP             "/conf/dns.tmp"
#define NETWORK_IP_SRC_FILE	"/conf/ipaddrsource"
#define AUTO_LOCAL_STR		"auto lo\n"
#define AUTO_LOCAL_LOOPBACK_STR	"iface lo inet loopback\n"
#define IF_STATIC_IP_STR	"address"
#define IF_STATIC_MASK_STR	"netmask"
#define IF_STATIC_BCAST_STR	"broadcast"
#define IF_STATIC_GW_STR	"gateway"
#define IF_STATIC_MTU_STRING "mtu"
#define DEFAULT_GW_FLAGS	0x0003
#define DEV_FILE	"/proc/net/dev"

#define VLAN_ID_SETTING_STR "vlanid"
#define VLAN_INTERFACES_FILE    "/conf/vlaninterfaces"
#define VLAN_PRIORITY_SETTING_STR     "vlanpriority"
#define VLAN_INDEX_STR       "index"
#define VLANSETTING_CONF_FILE "/conf/vlansetting.conf"

#define VLAN_NETWORK_DECONFIG_FILE    "/etc/init.d/vlannetworking stop"
#define VLAN_NETWORK_CONFIG_FILE    "/etc/init.d/vlannetworking start"
#define VLAN_ONLY_IFDOWN    "/etc/init.d/vlannetworking downvlan"
#define VLAN_PROC_SYS_RAC_NCSI_ENABLE_LAN    "echo 1 > /proc/sys/ractrends/ncsi/Enable"

#define VLAN_NETWORK_IP_CONFIGFILE    "/etc/init.d/vlannetworking enableip"
#define IF_STATIC_ADDR_STR " pre-up ip addr add"

/*Bond*/
#define BONDING_CONF_FILE   "/conf/bond.conf"
#define BONDING_SYSTEM_FILE "/sys/class/net"
#define IFACE_ENABLED     0x01
#define BOND_ROUND_ROBIN    0x00
#define BOND_ACTIVE_BACKUP  0x01
#define MAX_BOND_MODE           0x06
#define MAX_BOND                0x03
#define ETH_IFACE_TYPE      0x01
#define BOND_IFACE_TYPE     0x02
#define BOND_MAX_SLAVE      8

#define MAX_CHANNEL    0x03

/* IPv6 */
#define KERNEL_IPV6_FILE "/proc/net/if_inet6"

#define MAX_MAC_LEN 	64
#define MAC_ADDR_LEN	6
#define IP_ADDR_LEN	4
#define IP6_ADDR_LEN	16
#define MAX_STR_LENGTH 	128
#define ROUTE_GW_LENGTH 100
#define STR1_LEN 1 

#define MAX_IPV6ADDRS  16

/* DNS */
#define MAX_HOST_NAME_STRING_SIZE       64
#define MAX_DOMAIN_NAME_STRING_SIZE     256
#define HOSTNAME_SETTING_MANUAL			0
#define HOSTNAME_SETTING_AUTO			1
#define ONELINE_LEN                     300
#define DNS_SERVERIP_LEN                26

/* Type of network configuration */
#define CFGMETHOD_STATIC    1
#define CFGMETHOD_DHCP      2
#define CFGMETHOD_BIOS      3
#define CFGMETHOD_OTHER     4

#define NWCFGTYPE_STATIC        0x1
#define NWCFGTYPE_DHCP          0x2
#define NWCFGTYPE_DHCPFIRST     0x4

/*Interface enable state*/
#define NW_INTERFACE_ENABLE     0x1
#define NW_INTERFACE_DISABLE    0x2
#define NW_INTERFACE_UNKNOWN    0x3

#define NW_AUTO_NEG_ON  0x1
#define NW_AUTO_NEG_OFF 0x2

#define NW_DUPLEX_FULL 0x1
#define NW_DUPLEX_HALF 0x2

/*
 * ETHSET FLAG: See nwSetNWExtEthCfg API
 */
#define NWEXT_ETHCFG_LAMAC      0x1
#define NWEXT_ETHCFG_BURNEDMAC  0x2
#define NWEXT_ETHCFG_SPEED      0x4
#define NWEXT_ETHCFG_DUPLEX     0x8
#define NWEXT_ETHCFG_AUTONEG    0x10
#define NWEXT_ETHCFG_MTU        0x20
#define NWEXT_ETHCFG_WOL        0x40
#define NWEXT_ETHCFG_ALL (NWEXT_ETHCFG_LAMAC \
                            | NWEXT_ETHCFG_BURNEDMAC \
                            | NWEXT_ETHCFG_SPEED \
                            | NWEXT_ETHCFG_DUPLEX \
                            | NWEXT_ETHCFG_AUTONEG \
                            | NWEXT_ETHCFG_MTU \
                            | NWEXT_ETHCFG_WOL)

/*
 * NWSET FLAG: See nwSetNWExtIPCfg API
 */
#define NWEXT_IPCFG_INTFSTATE   0x1
#define NWEXT_IPCFG_CFGMETHOD   0x2
#define NWEXT_IPCFG_IP          0x4
#define NWEXT_IPCFG_MASK        0x8
#define NWEXT_IPCFG_GW          0x10
#define NWEXT_IPCFG_FBIP        0x20
#define NWEXT_IPCFG_FBMASK      0x40
#define NWEXT_IPCFG_FBGW        0x80
#define NWEXT_IPCFG_ALL (NWEXT_IPCFG_INTFSTATE \
                         | NWEXT_IPCFG_CFGMETHOD \
                         | NWEXT_IPCFG_FBIP \
                         | NWEXT_IPCFG_FBMASK \
                         | NWEXT_IPCFG_FBGW \
                         | NWEXT_IPCFG_IP \
                         | NWEXT_IPCFG_MASK \
                         | NWEXT_IPCFG_GW)

#define MAX_RESTART_SERVICE     5

#define MAX_SERVICE    5

/*Used to check the flag status*/
#define CHECK_FLAG(in,level) ((in&level) == level)

/*Used to set flag value*/
#define SET_FLAG(out,level) (out|=level)

/* Enable or disable Auto-Negotiation.  If this is set to enable,
 * the forced link modes are completely ignored.
 */
#define AUTONEG_DISABLE	0x00
#define AUTONEG_ENABLE		0x01

/* MII register for PHY Identifier */
#define MII_PHY_ID1		0x02
#define MII_PHY_ID2		0x03

#pragma pack (1)
typedef struct{
	unsigned char CfgMethod; /* This field can either be NWCFGTYPE_DHCP or NWCFGTYPE_STATIC */
	unsigned char MAC[6];
	unsigned char Local_MAC[6];
	unsigned char IPAddr[4];
	unsigned char Broadcast[4];
	unsigned char Mask[4];
	unsigned char Gateway[4];
	unsigned char  BackupGateway[4];
	  /* If interface is enabled, value is
     * NW_INTERFACE_ENABLE otherwise NW_INTERFACE_DISABLE
     */
    unsigned char enable;
	unsigned char IFName[10];
	unsigned short VLANID;
        unsigned char Master;
        unsigned char Slave;
        unsigned char BondIndex;
} PACKED NWCFG_STRUCT;
/*ipv6*/
typedef struct{
	unsigned char enable;
       unsigned char MAC[6];
	unsigned char CfgMethod; /* This field can either be NWCFGTYPE_DHCP or NWCFGTYPE_STATIC */
	unsigned char LinkIPAddr[16];
	unsigned char LinkPrefix;
	unsigned char SiteIPAddr[16];
	unsigned char SitePrefix;
	unsigned char GlobalIPAddr[MAX_IPV6ADDRS][16];   //anyCast Globel address    //UniCast Globel address
	unsigned char GlobalPrefix[MAX_IPV6ADDRS];
	unsigned char Gateway[16];
       unsigned char IFName[10];
} PACKED NWCFG6_STRUCT;

typedef struct {
        unsigned char Enable;
        unsigned short MiiInterval;
        unsigned char BondMode;
        unsigned char Slave;
}PACKED BondConf;

typedef struct {
    unsigned char Upslave;
}PACKED ActiveConf;

typedef struct
{
    INT8S Ifcname[16];
    INT8U Index;
    INT8U Enabled;
}PACKED IfcName_T;

typedef struct{
	unsigned short Family;
		NWCFG_STRUCT 	IPv4;
		NWCFG6_STRUCT	IPv6;
} PACKED ALL_NWCFG_STRUCT;
#pragma pack ()

typedef struct{
	NWCFG_STRUCT NwInfo[10];
	int IFCount;
} PACKED NWCFGS;

typedef struct
{
    /* This field can either be NW_DUPLEX_FULL or NW_DUPLEX_HALF */
    unsigned long speed;

    /* This field can either be NW_DUPLEX_FULL or NW_DUPLEX_HALF */
    unsigned int duplex;

    /* This field can either be  NW_AUTO_NEG_ON or NW_AUTO_NEG_OFF,
     * Auto negotiation is applicable for both speed & duplex.
     */
    unsigned int autoneg;

    /* This field features the link modes that are supported by the interface */
    unsigned char supported; 

    /*Max transmission unit*/
    unsigned long maxtxpkt;

    unsigned long wolsupported;
    unsigned long wolopts;

} PACKED ETHCFG_STRUCT;

typedef struct{
        ETHCFG_STRUCT EthInfo[10];
} PACKED ETHCFGS;


typedef struct{

    /* Name of the interface, example: eth0, eth1, ethernet,...*/
    char IFName[10];

    /*
     * This field can either be NWCFGTYPE_DHCP or
     * NWCFGTYPE_STATIC or NWCFGTYPE_DHCPFIRST
     */
    unsigned char CfgMethod;

    /*
     *  If interface is enabled, value is NW_INTERFACE_ENABLE
     *  otherwise NW_INTERFACE_DISABLE
     */
    unsigned char Enable;

    /*
     * Current IP Origin NWCFGTYPE_DHCP or NWCFGTYPE_STATIC
     */
    unsigned char IPOrigin;

    /*
     * IP assigned: If IPOrgin is DHCP, then this is DHCP IP,
     * if the IPOrigin is Static, then this is Static IP address
     */
    unsigned char IPAddr[4];
    unsigned char Mask[4];
    unsigned char Gateway[4];

    /*
     *  Manually configured Fall back (FB) IP
     */
    unsigned char FB_IPAddr[4];
    unsigned char FB_Mask[4];
    unsigned char FB_Gateway[4];

} PACKED NWEXT_IPCFG;


typedef struct
{
    /*Burned-in MAC address*/
    unsigned char BurnedMAC[6];

    /* Locally admin-MAC: Setting Local MAC to other than 00:00:00:00:00:00
     * makes it as current MAC. If a platform does not support. If this is
     * non-zero then this is current MAC while getting
     */
    unsigned char Local_MAC[6];

}PACKED NWEXT_MACCFG;


typedef struct
{

    /* MAC Configuration */
    NWEXT_MACCFG mac_cfg;

    /* Ethernet Configuration */
    ETHCFG_STRUCT eth_cfg;

} PACKED NWEXT_ETHCFG;

typedef struct
{
    unsigned char DNSDhcpEnable;                             /*Specifies that the DNS server IP addresses should be assigned from the DHCP server*/
    unsigned char DNSServerEthIndex;                         /*Specifies that from which DHCP server the IP addresses should be assigned */
    unsigned char DNSServer1[IP_ADDR_LEN];                   /*Specifies the IP address for DNS server 1*/
    unsigned char DNSServer2[IP_ADDR_LEN];                   /*Specifies the IP address for DNS server 2*/
    unsigned char v4v6;
    unsigned char RegisterBMCFQDN[MAX_CHANNEL];  	     /*Register the BMC host name using FQDN method */
    unsigned char DNSRegisterBMC[MAX_CHANNEL];               /*Enable registering the BMC host name on the DNS server*/
    unsigned char DNSBMCHostSetting;                         /*Specifies host name is Automatic or Manual */
    unsigned char DNSBMCHostNameLen;
    unsigned char DNSBMCHostName[MAX_HOST_NAME_STRING_SIZE];          /*Specifies the DNS BMC host name*/
    unsigned char DNSDomainNameDhcpEnable;                   /*Specifies that the DNS domain name should be assigned from the DHCP server*/
    unsigned char DNSDomainNameEthIndex;                     /*Specifies that from which DHCP server the DNS domain name should be assigned */    
    unsigned char DNSDomainNameLen;
    unsigned char DNSDomainName[MAX_DOMAIN_NAME_STRING_SIZE];/*The DNS domain name string*/

}  PACKED DNS_CONFIG;

/* IPv6 DNS structure */
#if 0
typedef struct
{
    unsigned char v6DNSDhcpEnable;                             /*Specifies that the DNS server IPv6 addresses should be assigned from the DHCP server*/
    unsigned char v6DNSServerEthIndex;                         /*Specifies that from which DHCP server the IPv6 addresses should be assigned */
    unsigned char v6DNSServer1[IP6_ADDR_LEN];                   /*Specifies the IPv6 address for DNS server 1*/
    unsigned char v6DNSServer2[IP6_ADDR_LEN];                   /*Specifies the IPv6 address for DNS server 2*/
   
}  PACKED V6DNS_CONFIG;
#endif

#define DNSCFG_MAX_DOMAIN_NAME_LEN 256 //with null
#define DNSCFG_MAX_RAC_NAME_LEN    64

/*DNS-Manual enable state*/
#define DNS_MANUAL_ENABLE   0x1
#define DNS_MANUAL_DISABLE  0x2

typedef struct
{
    unsigned char dnsEnable;
    unsigned char manualDNSDomain1Length;
    char manualDNSDomainName1[DNSCFG_MAX_DOMAIN_NAME_LEN];
    unsigned char manualDNSDomain2Length;
    char manualDNSDomainName2[DNSCFG_MAX_DOMAIN_NAME_LEN];
    struct in_addr manualDNSServer1;
    struct in_addr manualDNSServer2;
    struct in_addr manualDNSServer3;

} PACKED MANUAL_DNS_CONFIG;


/*********************************/
/* DNS Configuration structures */
/*********************************/
#define DOMAIN_V4 1
#define DOMAIN_V6 2

#define CONFDNSCONF "/conf/dns.conf"
#define CONFDNSCONF_TMP "/conf/dns.conf.tmp"

typedef struct
{
    INT8U HostSetting;                         /*Specifies host name is Automatic or Manual */
    INT8U HostNameLen;
    INT8U HostName[MAX_HOSTNAME_LEN];          /*Specifies the DNS BMC host name*/
    
}PACKED HOSTNAMECONF;

typedef struct
{
    INT8U    dhcpEnable;
    INT8U    EthIndex;
    INT8U    v4v6;
    INT8U    domainname[DNSCFG_MAX_DOMAIN_NAME_LEN];
    INT8U    domainnamelen;
    
}PACKED DOMAINCONF;

typedef struct
{
    INT8U   DNSIP1[IP6_ADDR_LEN];
    INT8U   DNSIP2[IP6_ADDR_LEN];
    INT8U   DNSIP3[IP6_ADDR_LEN];
    INT8U   DNSDHCP;
    INT8U   DNSIndex;
    INT8U   IPPriority;
}PACKED DNSCONF;

/*
typedef struct
{
    INT8U    dhcpEnable;
    INT8U    EthIndex;
    INT8U    v4DNSIP1[INET_ADDRSTRLEN];
    INT8U    v4DNSIP2[INET_ADDRSTRLEN];
    
}PACKED DNSIPV4CONF;

typedef struct
{
    INT8U    dhcpEnable;
    INT8U    EthIndex;
    INT8U    v6DNSIP1[INET6_ADDRSTRLEN];
    INT8U    v6DNSIP2[INET6_ADDRSTRLEN];
    
}PACKED DNSIPV6CONF;
*/

typedef struct
{
    INT8U   Enable;                                                             /*Enable/Disable*/
    INT8U   BondIndex;                                                      /*Index value of Bond Interface*/
    INT8U   BondMode;                                                       /*Bond Mode*/
    INT16U   MiiInterval;                                                     /*MII Interval*/
    INT8U   Slaves;                                                           /*Each bit represents the interface Index value i.e 0-7*/
    INT8U   AutoConf;
}PACKED BondIface;


/* Define the network interface */
typedef enum
{
    PENDING=1, // Interface having configured/pending settings
    ACTIVE     // Interface having current/active settings
}NW_INTERFACE;

//#pragma pack ()

/* Functions to read/write current network status */
int nwReadNWCfgs(NWCFGS *cfg, ETHCFGS *ethcfg);
int nwGetEthInformation(ETHCFG_STRUCT *ethcfg, char * IFName);
int nwSetEthInformation(unsigned long speed, unsigned int duplex, char * IFName);
int nwWriteNWCfgNoUpDown(NWCFG_STRUCT *cfg);

int nwSetBkupGWyAddr(unsigned char *ip,INT8U EthIndex);
int nwGetBkupGWyAddr(unsigned char *ip,INT8U EthIndex);
/* One should pass the buffer of size MAX_MAC_LEN to the following function */
int nwGetMACAddr(char *MAC);
int nwSetMACAddr(char *MAC);
int nwSetGateway(INT8U* GwIP,INT8U EthIndex);
int nwDelExistingGateway(INT8U EthIndex);



int nwGetNWInformations(NWCFG_STRUCT *cfg,char *IFName);

int nwMakeIFDown(INT8U EthIndex);
int nwMakeIFUp(INT8U  EthIndex);
extern void GetNwCfgInfo(void);
extern int GetNoofInterface(void);

/* Extended API for network*/
int GetHostEthbyIPAddr(char *IPAddr);
int GetHostEthByIPv6Addr(char *IPAddr);
int GetIfcNameByIndex(int Index, char * IfcName);
int nwGetSrcMacAddr(INT8U* IpAddr,INT8U EthIndex,INT8U *MacAddr);
int nwGetSrcMacAddr_IPV6(INT8U* IpAddr,INT8U *MacAddr);
int nwGetNWExtEthCfg(NWEXT_ETHCFG *cfg);
int nwGetNWInterfaceStatus(void);
int nwSetNWExtEthCfg(NWEXT_ETHCFG *cfg, int nwSetFlag);
int nwSetNWExtIPCfg (NWEXT_IPCFG *cfg, int nwSetFlag);
int GetNwLinkStatus(int fd,char *ifname);
int GetNwLinkType_mii(char *ifname);

/**
*@fn nwUpdateVLANInterfacesFile
*@brief This function is invoked to update vlan interfaces file
*/
int nwUpdateVLANInterfacesFile(void);

/**
*@fn ReadVLANFile
*@brief This function is invoked to read the vlan configuration files
*@param SettingStr - Pointer to setting name that we want to read from vlan configurations file
*@param desArr - pointer to an array where the reading has to be stored
*/
int ReadVLANFile(char *SettingStr, INT16U *desArr);

/**
*@fn WriteVLANFile
*@brief This function is invoked to write all the vlan configuration files
*@param SettingStr - Pointer to setting name that we want to write into vlan configurations file
*@param desArr - pointer to an array where the reading has to be stored
*@param EthIndex - char value to Ethernet index
*@param val - short int to the value that has to be written
*/
int WriteVLANFile(char *SettingStr, INT16U *desArr, INT8U EthIndex, INT16U val);

/*ipv6*/
void ConvertIP6numToStr(unsigned char *var, unsigned int len,unsigned char *string) ;
int nwReadNWCfg_IPv6(NWCFG6_STRUCT *cfg,INT8U EthIndex);
int GetDefaultGateway_ipv6(unsigned char *gw,INT8U *Interface);
int nwGetResolvConf_v4_v6(char* DNS1,char*DNS2,char *DNS3, INT8U DNSIPPriority,char* domain,unsigned char* domainnamelen);

/**
*@fn IsValidGlobalIPv6Addr
*@brief This function will check the given IPv6 address as global IP address or not
*@return Return 0 on success and -1 on failure
*/
int IsValidGlobalIPv6Addr(struct in6_addr * IPv6Addr);

/**
*@fn nwReadNWCfg_v4_v6
*@brief This function is invoked to Get the current network status of both IPv4 and IPv6 networks.
*@		If there is no need of IPv6 data means, then just pass NULL to IPv6 pointer.
*@param cfg - Pointer to Structure used to get IPv4 network configurations.
*@param cfg6 - Pointer to Structure used to get IPv6 network configurations.
*@param EthIndex - pointer to char used to store Interface Index value.
*@return Returns 0 on success and -1 on fails
*/
int nwReadNWCfg_v4_v6(NWCFG_STRUCT *cfg, NWCFG6_STRUCT *cfg6, INT8U EthIndex);

/**
*@fn nwWriteNWCfg_ipv4_v6
*@brief This function is invoked to set both IPv4 and IPv6 network configurations.
*@		If there is no need to write IPv6 data means, then just pass NULL to IPv6 pointer.
*@param cfg - Pointer to Structure used to set IPv4 network configurations
*@param cfg6 - Pointer to Structure used to set IPv6 network configurations
*@param EthIndex - pointer to char used to store Interface Index value.
*@return Returns 0 on success and -1 on fails
*/
int nwWriteNWCfg_ipv4_v6(NWCFG_STRUCT *cfg, NWCFG6_STRUCT *cfg6, INT8U EthIndex);

/*
 * @fn nwConfigureBonding
 * @brief This function will Enable/Disable the bonding support
 * @param BondCfg [in] Bonding configuration table
 * @param BondIndex [in] Index value of Bond interface to be configured
 * @return 0 on success, -1 on failure
 */
int nwConfigureBonding(BondIface *BondCfg,INT8U EthIndex);

/*
 * @fn nwGetBondConf
 * @brief This function will Get the bonding Configuration of Specified index
 * @param BondCfg [out] Bonding configuration table
 * @param BondIndex [in] Index value of Bond interface 
 * @return 0 on success, -1 on failure
 */
int nwGetBondConf(BondIface *BondCfg,INT8U BondIndex);

/*
 * @fn CheckIfcEntry
 * @brief This function will check the interface presence in ifcname table
 * @param Index [in] index value 
 * @param IfcType [in] interface type
 * @return 0 in success, -1 on failure
 */
int CheckIfcEntry(INT8U Index,INT8U IfcType);

/*
 * @fn CheckBondSlave
 * @brief This function will check the given interfaces slave status
 * @param EthIndex[in] interface's Ethindex value
 * @returns 1 if the interface is a slave of any bond interface, otherwise 0
 */
int CheckBondSlave(INT8U EthIndex);

/*
 * @fn CheckIfcLinkStatus
 * @brief This function will check the interface's Link health
 * @param Index [in] index value 
 * @return -1 on failure
 */
int CheckIfcLinkStatus(INT8U Index);

/**
*@fn IsKernelIPv6Enabled
*@brief This function is used to check for IPv6 support in the kernel.
*@return Returns 1 on success and 0 on fails
*/
int IsKernelIPv6Enabled();

/*
 * @fn nwActiveSlave
 * @brief This function will active the single slave for the bonding interface
 * @param SlaveIndex to be activated
 * @return 0 on success, -1 on failure
*/
int nwActiveSlave(INT8U BondIndex,INT8U SlaveIndex);

/*
 * @fn nwGetActiveSlave
 * @brief This function will gets the active interface of specified bondindex
 * @param Bondindex [in] bonding index, Activeindex[out] active slaves
 * @return 0 on success, -1 on failure
 */
int nwGetActiveSlave(INT8U BondIndex,INT8U *ActiveIndex);

/**
*@fn Write_dhcp6c_conf
*@brief This function is used to write interface wise entries for dhcp6c.conf file.
*@return Returns 0 on success and -1 on fails
*/
int Write_dhcp6c_conf();


/* DNS */
int nwSetResolvConf_v4_v6(char* dns1,char* dns2,char* dnsv3,char* domain);

int ReadDNSConfFile ( DOMAINCONF *DomainConfig, DNSCONF *DnsIPConfig,INT8U *regBMC_FQDN);
int WriteDNSConfFile ( DOMAINCONF *DomainConfig, DNSCONF *DnsIPConfig, INT8U *regBMC_FQDN);
int nwGetAllDNSConf( HOSTNAMECONF *HostnameConfig, DOMAINCONF *DomainConfig, DNSCONF *DnsIPConfig,INT8U *regBMC_FQDN );
int nwSetAllDNSConf( HOSTNAMECONF *HostnameConfig, DOMAINCONF *DomainConfig, DNSCONF *DnsIPConfig,INT8U *regBMC_FQDN );

/* Library for AMI Extended DNS command  */
int nwSetHostName(char *name);
int nwGetDNSConf(unsigned char* UseDHCPForDNS,unsigned char* UseDHCPForDomain,unsigned char* DoDDNS);
int nwGetAllDNSCfg(DNS_CONFIG* dnscfg);
int nwSetDNSConf(unsigned char UseDHCPForDNS,unsigned char UseDHCPForDomain,unsigned char DoDDNS);
int nwSetAllDNSCfg_NotRestart(DNS_CONFIG* dnscfg);

int nwGetDHCPServerIP(char *dhcpServerIP);
/*!
* @brief Reading current/active network configuration using netman script
* #param[out] cfg - IP, Netmask, Gateway, Conf method(dhcp/statis)
* @returns 0 on succee , -1 on failure
*/
int nwGetNwActIPCfg( NWEXT_IPCFG *cfg );
int GetIPAdrressType(INT8U* IPAddr);
#endif /* _NW_CFG_H */


