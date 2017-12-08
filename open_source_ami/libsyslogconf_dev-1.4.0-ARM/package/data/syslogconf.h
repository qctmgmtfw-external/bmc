/****************************************************************
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2005-2006, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        6145-F, Northbelt Parkway, Norcross,                **
 **                                                            **
 **        Georgia - 30071, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 ****************************************************************
 ****************************************************************
 *
 * syslogconf.h
 * AMI Syslog Configuration related API Calls
 *
 * Author: Gokula Kannan. S <gokulakannans@amiindia.co.in>
 *
 *****************************************************************/

#ifndef SYSLOGCONF_H_
#define SYSLOGCONF_H_

#include "Types.h"

#define DISABLE	0x0
#define ENABLE_LOCAL	0x1
#define ENABLE_REMOTE	0x2
#define HOSTNAME_LEN	64
#define LOGFILENAME_LEN	32
#define MAXFILESIZE		10000

/**
 * @fn SetSyslogConf
 * @brief Set the syslog configuration file with the given system status
 * 		  and audit status.
 * @param[in] nSysStatus - Status of system log to be set.
 * @param[in] nAuditStatus - Status of the audit log.
 * @param[in] pHostName - Name of the host, if the system log is remote
 * @retval return 0 on success and -1 on failure.
 */
extern int SetSyslogConf(INT8U nSysStatus, INT8U sysStatus, char *pHostName);
/**
 * @fn AuditlogDisable
 * @brief Disable the audit log in the system.
 * @retval return 0 on success and -1 on failure.
 */
extern int AuditlogDisable();
/**
 * @fn AuditlogEnable
 * @brief Enable the audit log in the system.
 * @retval return 0 on success and -1 on failure.
 */
extern int AuditlogEnable();
/**
 * @fn SyslogDisable
 * @brief Disable the system log for the system.
 * @retval return 0 on success and -1 on failure.
 */
extern int SyslogDisable();
/**
 * @fn SyslogEnableRemote
 * @brief Enable the system log for remote logging.
 * @param[in] pHost - Name of the remote syslog server
 * @retval return 0 on success and -1 on failure.
 */
extern int SyslogEnableRemote(char *pHost);
/**
 * @fn SyslogEnableLocal
 * @brief Enable the system log for local logging.
 * @retval return 0 on success and -1 on failure.
 */
extern int SyslogEnableLocal();
/**
 * @fn GetSyslogConf
 * @brief Get the syslog configuration file with the given system status
 * 		  and audit status.
 * @param[out] pSysStatus - Status of system log.
 * @param[out] pAuditLog - Status of the audit log.
 * @param[out] pAuditLog - Name of the host, if the system log status is remote
 * @retval return 0 on success and -1 on failure.
 */
extern int GetSyslogConf(INT8U *pSysStatus, INT8U *pAuditLog, char *pHostName);
/**
 * @fn GetLogRotate
 * @brief Get the configuration of log rotate.
 * @param[out] pRotate - Number of back up files after logrotate.
 * @param[out] pSize - Size of each file to rotate.
 * @retval return 0 on success and -1 on failure.
 */
extern int GetLogRotate(INT8U *pRotate, INT32U *pSize);
/**
 * @fn SetLogRotate
 * @brief Set the configuration of log rotate.
 * @param[in] nRotate - Number of back up files after logrotate.
 * @param[in] nSize - Size of each file to rotate.
 * @retval return 0 on success and -1 on failure.
 */
extern int SetLogRotate(INT8U nRotate, INT32U nSize);

#define LOG_VAR_LOCATION	"/var/log/"
#define CONF_LOCATION	"/conf/"
#define ALERT_FILE			LOG_VAR_LOCATION "alert.log"
#define CRITICAL_FILE		LOG_VAR_LOCATION "crit.log"
#define ERROR_FILE			LOG_VAR_LOCATION "err.log"
#define NOTIFICATION_FILE	LOG_VAR_LOCATION "notice.log"
#define WARNING_FILE		LOG_VAR_LOCATION "warning.log"
#define DEBUG_FILE			LOG_VAR_LOCATION "debug.log"
#define EMERGENCY_FILE		LOG_VAR_LOCATION "emerg.log"
#define INFORMATION_FILE	LOG_VAR_LOCATION "info.log"
#define CONF_AUDIT_FILE	CONF_LOCATION "audit.log"
#define LOG_AUDIT_FILE		LOG_VAR_LOCATION "audit.log"

#define ALERT_LOG		1
#define CRIT_LOG		2
#define ERROR_LOG		3
#define NOTICE_LOG		4
#define WARN_LOG		5
#define DEBUG_LOG		6
#define EMER_LOG		7
#define INFO_LOG		8

#endif
