/**
 * @file   libipmi_IPM.h
 * @author Rajasekhar (rajasekharj@amiindia.co.in)
 * @date   02-Sep-2004
 *
 * @brief  Contains exported IPM API by LIBIPMI for
 *			communicating with the BMC.Corresponds
 			directly to IPM device commands
 *
 */

#ifndef __LIBIPMI_IPM_H__
#define __LIBIPMI_IPM_H__

/* LIIPMI core header files */
#include "libipmi_session.h"
#include "libipmi_errorcodes.h"
#include "libipmiifc.h"

/* command specific header files */
#include "IPMI_IPM.h"

#ifdef  __cplusplus
extern "C" {
#endif

/*---------- IPMI Command direct routines ----------*/
/* Device ID */
LIBIPMI_API uint16	IPMICMD_GetDeviceID( IPMI20_SESSION_T *pSession/*in*/,
										GetDevIDRes_T *pGetDeviceID/*out*/,
										int timeout/*in*/);
/* Device GUID */
LIBIPMI_API uint16	IPMICMD_GetDeviceGUID( IPMI20_SESSION_T *pSession/*in*/, 
                                        GetDevGUIDRes_T *pGetDeviceGUID/*out*/,
                                        int timeout/*in*/ );

/*---------- LIBIPMI Higher level routines -----------*/
/* Device ID */
LIBIPMI_API uint16 LIBIPMI_HL_GetDeviceID( IPMI20_SESSION_T *pSession/*in*/,
											char *pszDeviceID/*out*/, int timeout/*in*/ );

/* Device GUID */
LIBIPMI_API uint16 LIBIPMI_HL_GetDeviceGUID( IPMI20_SESSION_T *pSession /*in*/, 
                                            char *pszDeviceGUID /*out*/, int timeout /*in*/);

#ifdef  __cplusplus
}
#endif


#endif


