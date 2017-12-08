#ifndef __ADVISER_CFG_H__
#define __ADVISER_CFG_H__


#define ADVISER_CFG_FILE      "/conf/adviserd.conf"  /** Location of the configuration file */
#define STUNNEL_CFG_FILE      "/conf/stunnel.conf"  /** Location of the configuration file */
#define TMP_STUNNEL_FILE	"/conf/stunnel_tmp.conf"

#define ADVISER_SECTION_NAME	"adviserd"

#define KVM_ENABLED             "enabled"       /** Key string that represents enabled */
#define MAX_SESSIONS            "max_sessions"   /** Key string that represents max_sessions */
#define ACTIVE_SESSIONS         "active_sessions"   /** Key string that represents active_sessions */
#define STR_PORT                "port"              /** Key string that represents non-secure port */
#define STR_SECURE_PORT         "secure_port"       /** Key string that represents secure port */
#define STR_HID_PORT      		"hid_port"    /** Key string that represents secure hid port */
#define STR_SECURE_CHANNEL      "secure_channel"    /** Key string that represents secure channel */
#define STR_MOUSE_MODE      	"mouse_mode"    /** Key string that represents mouse mode */
#define STR_DISABLE_LOCAL      	"disable_local"    /** Key string that represents local console */
#define STR_KEYBOARD_LAYOUT     "keyboard_layout"  

//#define KVMSERVER_RESTART "/etc/init.d/kvm_servers restart"
#define ADVISER_RESTART		"/etc/init.d/adviserd.sh restart"
#define ADVISER_RESTART_CONF	"/etc/init.d/adviserd.sh restart_conf"
#define KEYBOARD_LANG_SIZE 3

typedef struct {
	unsigned int mouse_mode;   /** Says whether to use Absolute mouse or relative mouse */
    char   keyboard_layout[KEYBOARD_LANG_SIZE];
} AdviserCfg_T;


int GetAdviserCfg(AdviserCfg_T* pAdviserCfg);
int SetAdviserCfg(AdviserCfg_T* pAdviserCfg);
int GetAdviserSecureStatus(void);
int SetAdviserSecureStatus(int secure);
int SetMouseModeCfg(unsigned int mouse_mode);
int SetKeyLayoutCfg(char *keyboard_layout);

#endif


