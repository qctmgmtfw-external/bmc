/*
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2006-2011, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 */

#ifndef LIBPHYCONF_H
#define LIBPHYCONF_H

#define PHYCFG_CONFIG_FILE "/conf/phycfg.conf"

#define SECTION_NAME_INTERFACE_PREFIX	"interface#"
#define	INTERFACE						"interface"
#define AUTONEGOTIATION        			"autonegotiation"
#define SPEED        					"speed"
#define DUPLEX        					"duplex"

#define PACKED __attribute__ ((packed))

typedef struct
{
	char interfaceName[8];
	int autoNegotiationEnable;
	int speed;
	int duplex;
} PACKED PHYConfigFile_T;


int getTotalEthInterfaces(int* nInterfaces);
int getAllPHYConfigs(PHYConfigFile_T configs[], int interface);
int getPHYConfig(int interface, PHYConfigFile_T* retrievedConfig);
int setPHYConfig(char* interfaceName, int autoNegotiation, int speed, int duplex);
int getPHYConfigByName(char* interfaceName, PHYConfigFile_T* retrievedConfig);
int setPHYConfigByName(char* interfaceName, PHYConfigFile_T config);
int createDefaultPHYConfFile(void);

#endif
