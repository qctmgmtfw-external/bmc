#ifndef __VIDEORECORD_CFG_H__
#define __VIDEORECORD_CFG_H__

#ifdef ICC_OS_WINDOWS
#pragma pack(1)
#endif

#ifdef ICC_OS_LINUX
#define PACK __attribute__((packed))
#else
#define PACK
#endif

#define MAX_PATH_SIZE		256
#define AUTORECORD_CFG_FILE	"/conf/autorecord.conf"		/** Location of the auto-record configuration file */
#define AUTORECORD_WWW_PATH	"/usr/local/www/video/"

#define DEF_VDO_DUMP_PATH		"/var/video/"
#define DEF_MAX_VDO_DUMP		2
#define DEF_MAX_VDO_DUMP_SIZE		(5767168L) //(5.5 MB)
#define DEF_MAX_VDO_DUMP_TIME		20
#define VDO_DUMP_FILE			"video_dump_"

#define ACCESS_GRANTED			0
#define ACCESS_DENIED			-1
#define FILE_NOT_AVAILABLE		-2
#define FILE_LOCKED			-3

#define VIDEO_FILE_SUCCESS				0
#define FAILED				-1

#define MAX_RETRIES			3

/* This section is for the Auto Recording Feature Configurable Parameters */
#define AUTORECORD_SECTION_NAME	"autorecord"

#define RECORD_FILE_PATH	"dump_path"	/** Key string that represents path to store the dump files */
#define MAX_TIME		"max_time"	/** Key string that represents the max time to record dumps */
#define MAX_SIZE		"max_size"	/** Key string that represents the max size to record dumps */
#define MAX_DUMP_FILES		"max_dumps"	/** Key string that represents the max number of dump files */

//#define KVMSERVER_RESTART "/etc/init.d/kvm_servers restart"
#define ADVISER_RESTART		"/etc/init.d/adviserd.sh restart"
#define ADVISER_RESTART_CONF    "/etc/init.d/adviserd.sh restart_conf"

typedef struct {
    unsigned char path[MAX_PATH_SIZE];
    unsigned int max_time;
    unsigned int max_size;
    unsigned int max_dumps;
} PACK AutoRecordCfg_T; 

typedef struct {
	char FileName[64];
	char TimeStamp[64];
} PACK AutoRecordFile_T;

int GetAutoRecordCfg(AutoRecordCfg_T* pAutoRecordCfg);
int SetAutoRecordCfg(AutoRecordCfg_T* pAutoRecordCfg);
int SetFileAccess(char* FileName);
int ResetFileAccess(char* FileName);
int DeleteDataFile(char* FileName);
int GetFileCount(void);
int GetFileInfo(void* pFileStructure);

#ifdef ICC_OS_WINDOWS
#pragma pack()
#endif

#endif


