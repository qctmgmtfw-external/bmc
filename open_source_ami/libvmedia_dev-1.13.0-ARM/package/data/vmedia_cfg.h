/****************************************************************
 **                                                            **
 **    (C)Copyright 2006-2009, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
****************************************************************/

#ifndef __VMEDIA_CFG_H__
#define __VMEDIA_CFG_H__

#define	MODE_VMEDIA_ATTACH		(0)
#define MODE_VMEDIA_AUTO_ATTACH		(1)
#define MODE_VMEDIA_DETACH		(2)

#define VMEDIA_CFG_FILE			"/conf/vmedia.conf"  /** Location of the configuration file */
#define VMEDIA_STUNNEL_CFG_FILE		"/conf/stunnel_vmedia.conf"  /** Location of the configuration file */

#define VMEDIA_SECTION_NAME		"vmedia"
#define LMEDIA_SECTION_NAME		"lmedia"

#define STR_VMEDIA_MAX_SESSIONS		"max_sessions"   /** Key string that represents max_sessions */
#define STR_VMEDIA_ACTIVE_SESSIONS	"active_sessions"   /** Key string that represents active_sessions */
#define STR_VMEDIA_SECURE_CHANNEL	"secure_channel"    /** Key string that represents secure channel */
#define STR_VMEDIA_PORT			"port"              /** Key string that represents non-secure port */
#define STR_VMEDIA_SECURE_PORT		"secure_port"       /** Key string that represents secure port */
#define STR_VMEDIA_MODE_CDROM		"cdrom_mode"   /** Attach mode for CDROM */
#define STR_VMEDIA_MODE_FLOPPY		"floppy_mode"   /** Attach mode for floppy */
#define STR_VMEDIA_MODE_HDISK		"hdisk_mode"	/** Attach mode for hard disk */
#define STR_ENABLE_BOOT_ONCE		"boot_once"    /** Key string that represents secure hid port */
#define STR_VMEDIA_NUM_CD		"num_cd"
#define STR_VMEDIA_NUM_FD		"num_fd"
#define STR_VMEDIA_NUM_HD		"num_hd"
#define STR_VMEDIA_NUM_VF_CD		"num_vf_cd"
#define STR_VMEDIA_NUM_VF_FD		"num_vf_fd"
#define STR_VMEDIA_NUM_VF_HD		"num_vf_hd"
#define STR_VMEDIA_LMEDIA_ENABLE	"lmedia_enable"

#define STR_LMEDIA_CD_IMAGE		"cdimage"
#define STR_LMEDIA_HD_IMAGE		"hdimage"
#define STR_LMEDIA_FD_IMAGE		"fdimage"

#define FDSERVER_RESTART 		"/etc/init.d/fdserver restart"
#define CDSERVER_RESTART 		"/etc/init.d/cdserver restart"
#define HDSERVER_RESTART 		"/etc/init.d/hdserver restart"
#define VMAPP_RESTART	 		"/etc/init.d/vmscript restart"
#define ADVISER_RESTART			"/etc/init.d/adviserd.sh restart"

#if 0
typedef struct {
	unsigned int max_sessions;		/** Max number of sessions allowed at a time */
	unsigned int active_sessions;           /** Number of sessions active at a time */
	unsigned int secure_channel;   		/** Says whether to open secure channel or non-secure channel */
	unsigned int vmedia_port;		/** Port number on which vmedia will be listening for normal connections*/
	unsigned int vmedia_secure_port;	/** Port number on which vmedia will be listening for SSL connections*/
	unsigned int attach_cd;			/** Attach mode for CD */
	unsigned int attach_fd;			/** Attach mode for FD */
	unsigned int attach_hd;			/** Attach mode for HD */
	unsigned int enable_boot_once;		/** Enable boot once or not */
	unsigned int num_cd;			/** Number of CD Instances */
	unsigned int num_fd;			/** Number of FD Instances */
	unsigned int num_hd;			/** Number of HD Instances */
	unsigned int lmedia_enable;		/** Enable the LMedia Feature */
} VMediaCfg_T;
#endif

typedef struct {
	unsigned int attach_cd;			/** Attach mode for CD */
	unsigned int attach_fd;			/** Attach mode for FD */
	unsigned int attach_hd;			/** Attach mode for HD */
	unsigned int enable_boot_once;		/** Enable boot once or not */
	unsigned int num_cd;			/** Number of CD Instances */
	unsigned int num_fd;			/** Number of FD Instances */
	unsigned int num_hd;			/** Number of HD Instances */
	unsigned int lmedia_enable;		/** Enable the LMedia Feature */
} VMediaCfg_T;

int GetVMediaCfg(VMediaCfg_T* pVMediaCfg);
int SetVMediaCfg(VMediaCfg_T* pVMediaCfg);

int GetVmediaSecureStatus(void);
int SetVmediaSecureStatus(int secure);

int GetVmediaCdromMode(void);
int SetVmediaCdromMode(int mode);

int GetVmediaFloppyMode(void);
int SetVmediaFloppyMode(int mode);

int GetVmediaHdiskMode(void);
int SetVmediaHdiskMode(int mode);

int GetVmediaAttachMode(void);
int SetVmediaAttachMode(int mode);


/* LMEDIA -----------> */

#define MAX_IMG_PATH			256
#define LMEDIA_CFG_FILE 		"/usr/local/lmedia/lmedia.toc"
#define LMEDIA_RESTART 			"/etc/init.d/lmedia.sh restart"
typedef struct 
{
	char Image_cd[MAX_IMG_PATH];
	char Image_fd[MAX_IMG_PATH];
	char Image_hd[MAX_IMG_PATH];
	char Image_cd_valid;
	char Image_fd_valid;
	char Image_hd_valid;
} LMediaCfg_T;
		
int GetLMediaCfg(LMediaCfg_T* pLMediaCfg);
int AddLMediaImage(int type,char *Image);
int ModifyLMediaImage(int type,char *Image);
int DeleteLMediaImage(int type);

/* Values for type in the above functions */
#define IMAGE_TYPE_CD			0x1
#define IMAGE_TYPE_FD			0x2
#define IMAGE_TYPE_HD			0x4

/* <------------ LMEDIA */

#endif /* __VMEDIA_CFG_H__ */



