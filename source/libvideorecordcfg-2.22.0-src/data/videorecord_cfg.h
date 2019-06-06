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

#define VDO_RCD_SUCCESS               0
#define VDO_RCD_FAILURE               -1
#define MAX_FILE_NAME_LEN             256
#define MAX_PATH_NAME_LEN             (4 * MAX_FILE_NAME_LEN)
#define MAX_CMD_NAME_LEN              (MAX_PATH_NAME_LEN + 256)

#define AUTORECORD_STATUS_FILE        "/tmp/auto_video_record_status"
#define AUTORECORD_CFG_FILE           "/conf/autorecord.conf"                 /** Location of the auto-record configuration file */
#define AUTORECORD_DEF_CFG_FILE       "/etc/defconfig/autorecord.conf"        /** Location of the auto-record configuration file */
#define AUTORECORD_WWW_PATH           "/usr/local/www/video/"

#define DEF_VDO_DUMP_PATH	      "/var/video/"
#define VDO_DUMP_FILE		      "video_dump_"
#define VDO_DUMP_FILE_EXT	      ".dat"
#define DEF_MAX_VDO_DUMP	      2
#define DEF_MAX_VDO_DUMP_SIZE	      (5767168L) //(5.5 MB)
#define DEF_MAX_VDO_DUMP_TIME	      20
#define DEF_MAX_REMOTE_VDO_DUMP_TIME  300

#define ACCESS_GRANTED		      0
#define ACCESS_DENIED		      -1
#define FILE_NOT_AVAILABLE	      -2
#define FILE_LOCKED		      -3
#define MOUNT_FAILED		      -4

#define VIDEO_FILE_SUCCESS	       0
#define FAILED			      -1

#define MAX_RETRIES		       3

#define MAX_RMS_USERNAME_LEN           256
#define MAX_RMS_PASSWORD_LEN           32
#define MAX_SHARE_TYPE_LEN             6
#define MAX_IP_LEN                     63
#define MAX_DOMAIN_LEN                 256
/* This section is for the Auto Recording Feature Configurable Parameters */
#define AUTORECORD_SECTION_NAME	"autorecord"

#define RECORD_FILE_PATH	"dump_path"	/** Key string that represents path to store the dump files */
#define MAX_TIME		"max_time"	/** Key string that represents the max time to record dumps */
#define MAX_SIZE		"max_size"	/** Key string that represents the max size to record dumps */
#define MAX_DUMP_FILES		"max_dumps"	/** Key string that represents the max number of dump files */
#define REMOTE_PATH_SUPPORT	"remote_support"	/** Key string that represents remote file support*/
#define REMOTE_FILE_PATH	"remote_path"	/** Key string that represents path to store the dump files in remote*/
#define REMOTE_IP	"ipaddr"	/** Key string that represents remote share IP address*/
#define REMOTE_USER_NAME	"username"	/** Key string that represents remote machine user name*/
#define REMOTE_USER_PASS "password"	/** Key string that represents remote machine password*/
#define REMOTE_SHARE_TYPE	"sharetype"	/** Key string that represents remote machine share type*/
#define REMOTE_DOMAIN	"domain"	/** Key string that represents remote machine domain*/
#define MOUNT_STATUS	"mnt_status"	/** Key string that represents mount status*/

#define SHARE_TYPE_NFS			"nfs"
#define SHARE_TYPE_CIFS			"cifs"
#define SHARE_TYPE_SMB			"smb"
#define SHARE_TYPE_UNKNOWN		""
//#define KVMSERVER_RESTART "/etc/init.d/kvm_servers restart"
#define ADVISER_RESTART		"/etc/init.d/adviserd.sh restart &"
/**************************RecordBootCrash******************************/
//Structure holding /conf/autorecord.conf [record_boot_crash] field data
typedef struct {
    //enable/disable of this depends of event selection from Web UI
    //unsigned char current_state;
    unsigned char vdo_duration;
    unsigned char vdo_quality;
    unsigned char vdo_compression_mode;
    unsigned char frame_count;
}PACK RecordBootCrashCfg_T;

#define MAX_TEMP_ARRAY_SIZE 64
#define DEF_REC_DUMP_PATH       "/var/rec/"
#define REC_BOOT_CRASH_SECTION_NAME "record_boot_crash" //Section in config file for RecordBootCrash Feature
#define REC_BOOT_CRASH_CURRENT_STATE "current_state"
#define REC_BOOT_CRASH_VDO_DURATION "vdo_duration"
#define REC_BOOT_CRASH_VDO_QUALITY "vdo_quality"
#define REC_BOOT_CRASH_VDO_COMPRESSION_MODE "vdo_compression_mode"
#define REC_BOOT_CRASH_FRAME_COUNT "frame_count"
/*Default configuration for RecorBootCrash feature, used in case of config file corruption*/
#define DEF_REC_BOOT_CRASH_VDO_DURATION 10 //time in seconds 
#define DEF_REC_BOOT_CRASH_VDO_QUALITY  0 //DCT_Quantization table selector 
#define DEF_REC_BOOT_CRASH_VDO_COMPRESSION_MODE 0 //Compression mode type 
#define DEF_REC_BOOT_CRASH_FRAME_COUNT 1 //Will store 1 frame/sec to avoid CPU overusage

#define REC_BOOT_CRASH_MIN_FILE_DURATION 10 //Using symmetric split to store large vdos, each file will store 10 sec vdo
//Max Limits for parameters
#define REC_BOOT_CRASH_VDO_DURATION_MAX_LIMIT           60 //time in seconds
#define REC_BOOT_CRASH_VDO_QUALITY_MAX_LIMIT            4
#define REC_BOOT_CRASH_VDO_COMPRESSION_MODE_MAX_LIMIT   3
#define REC_BOOT_CRASH_FRAME_COUNT_MAX_LIMIT            4
#define REC_BOOT_CRASH_FRAME_COUNT_INVALID_VAL          0
//Sum of compression(C) and FPS(F) to validate video duration
#define REC_BOOT_CRASH_C_F_LOWER_LIMIT 3
#define REC_BOOT_CRASH_C_F_UPPR_LIMIT 5
#define REC_BOOT_CRASH_VDO_DURATION_AVERAGE_LIMIT 30

#define CONT_REC_FILE_PREFIX                    "rec@"
#define STOP_CONT_RECORDING                     -2
//Pre Crash/Boot File Count at remote path
#define MAX_REC_FILE_COUNT                       3
//Pre Crash/Boot File Count at BMC 
//(if no remote path available)
#define DEF_REC_FILE_COUNT                       1
#define PRE_CRASH_VIDEO_DUMP_PREFIX              "pre_crash_video_"
#define PRE_RESET_VIDEO_DUMP_PREFIX              "pre_reset_video_"
#define CONT_REC_TIME_STR_SIZE                   10
#define CONT_REC_TMP_DUMP_SIZE                   20
#define REC_FILE_PATH_SIZE                       256
#define CMD_STR_SIZE                             100
/*Currently same as triggered recording
*TBD:Once Front-end web part will design will move
 Crash recording to other Non-volatile memory area
*/
#define DEF_CRASH_VIDEO_FILE_PATH                "/var/video/"

#define GET_FOLDER_NAME	1
#define GET_FILE_NAME	2
#define GET_FULL_PATH	3

/*Function Prototype*/
int GetRecordBootCrashCfg(RecordBootCrashCfg_T *);
int SetRecordBootCrashCfg(RecordBootCrashCfg_T *);
int ValidateRecordBootCrashCfg(RecordBootCrashCfg_T *);
/***********************************************************************/

typedef struct {
    char path[MAX_FILE_NAME_LEN];
    unsigned int max_time;
    unsigned int max_size;
    unsigned int max_dumps;
    unsigned int remote_path_support;	
    char ip_addr[MAX_IP_LEN];
    char username[MAX_RMS_USERNAME_LEN];
    char password[MAX_RMS_PASSWORD_LEN];
    char share_type[MAX_SHARE_TYPE_LEN];
    char domain[MAX_DOMAIN_LEN];	
    unsigned int mnt_successful;
} PACK AutoRecordCfg_T; 

typedef struct {
	char FileName[256];
	char TimeStamp[64];
} PACK AutoRecordFile_T;

int GetAutoRecordCfg(AutoRecordCfg_T* pAutoRecordCfg);
int SetAutoRecordCfg(AutoRecordCfg_T* pAutoRecordCfg);
int SaveAutoRecordCfg(AutoRecordCfg_T* pAutoRecordCfg);
int SetFileAccess(char* FileName);
int ResetFileAccess(char* FileName);
int DeleteDataFile(char* FileName);
int GetFileCount(void);
int GetPreEventFileCount(void);
int GetFileInfo(void* pFileStructure);
int MountRemotePath(AutoRecordCfg_T	*AutoRecordCfg,const char *mountPath);
int MountShare (AutoRecordCfg_T *pAutoRecordCfg, const char *RemotePath,const char *mountPath);
int unMount(const char *mountPath);
int unMountPath(const char *mountPath);
int updateMntStatus(int status);
unsigned char GetVideoRecordStatus ();
int getvideolist( unsigned int *filecnt,AutoRecordFile_T **fileinfo);
int GetVideoFolder(char *folder,int option);
int IsMaxVideoFolderSizeReached();

#ifdef ICC_OS_WINDOWS
#pragma pack()
#endif

#endif


