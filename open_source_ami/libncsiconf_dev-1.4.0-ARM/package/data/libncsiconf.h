#ifndef LIBNCSICONF_H
#define LIBNCSICONF_H

#define NCSICFG_CONFIG_FILE "/conf/ncsicfg.conf"

#define SESSION_NAME_PORT_PREFIX	"port#"
#define PACKAGE_NAME        		"package"
#define CHANNEL_NAME        		"channel"
#define INTERFACE_NAME        		"interface"
#define PORT_NAME					"port"

typedef struct
{
	char InterfaceName[8]; 
	int PackageId;
	int ChannelId;
} NCSIConfig_T;


int NCSIGetTotalPorts(int* ports);
int NCSIGetAllPortConfigs(NCSIConfig_T configs[], int ports);
int NCSIGetPortConfig(int portNumber, NCSIConfig_T* retrievedConfig);
int NCSISetPortConfig(int portNumber, NCSIConfig_T config);
int NCSIGetPortConfigByName(char* interfaceName, NCSIConfig_T* retrievedConfig);
int NCSISetPortConfigByName(char* interfaceName, NCSIConfig_T config);
int NCSICreateDefaultConfFile(void);

#endif
