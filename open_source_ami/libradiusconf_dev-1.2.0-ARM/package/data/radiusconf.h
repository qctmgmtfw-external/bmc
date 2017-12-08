#ifndef _RADIUS_H
#define _RADIUS_H

#define		CFG_RADIUS_ENABLE	1
#define		CFG_RADIUS_DISABLE	0
#define		MAX_LINE_LENGTH		128
#define		MAX_VALUE_LENGTH	64

#define		DEFAULT_RADIUS_PORT		1812
#define		DEFAULT_TIME_OUT		3

/* RADIUS configuration file */
#define		RADIUS_CONFIG_FILE				"/conf/radius.conf"
#define		RADIUS_CONFIG_FILE_DISABLED		"/conf/radius.conf.disabled"

#pragma pack (1)
typedef struct {
	unsigned char Enable;
	unsigned char IPAddr[32];
	unsigned short PortNum;
	unsigned char Secret[32];
	unsigned int Timeout;
} RADIUSCONFIG;
#pragma pack ()

int setradiusconfig(RADIUSCONFIG *radiusconfig,int servercount);
int getradiusconfig(RADIUSCONFIG **radius_config_data,int *servercount);

int read_radiusconf_file(char *file,RADIUSCONFIG **rad_config,int *server_count);
int write_radiusconf_file(char *file, RADIUSCONFIG *radiusconfig,int servercount);


#endif /* _RADIUS_H */
