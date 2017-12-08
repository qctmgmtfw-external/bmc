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

#define NTP_CFG_FILE "/conf/ntp.conf"
#define NTP_STAT_FILE "/conf/ntp.stat"
#define UTC_OFFSET_FILE "/conf/utc.offset"
#define NTP_START "/etc/init.d/ntpdate start"
#define MAX_NTP_STR_LENGTH 2048
#define NTP_STATUS_STR_LENGTH 10
#define SET_NTPCFG_SUCCESS 0
#define SET_DATETIME_ERR 1
#define SET_NTPSTATUS_ERR 2
#define SET_UTCOFFSET_ERR 3
#define SET_NTPSERVER_ERR 4
#define RESTART_NTP_ERR 5
#define SESSION_FILE "/conf/time.txt"

extern int libami_getntpServer(char *);
extern int libami_setntpServer(char *);
extern int libami_getntpStatus(char *ntpstatus);
extern int libami_setntpStatus(char *ntpstatus);
extern int libami_getutcOffset(char *offset);
extern int libami_setutcOffset(char *offset);
extern int libami_getSessionTime(char *);
extern int libami_setSessionTime(char *);
extern int restart_ntpdate(void);
extern char *strrtrim_LCL(char *, const char *);
extern char *strltrim_LCL(char *, const char *);
extern char *strtrim_LCL(char *, const char *);
