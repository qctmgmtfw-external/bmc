#ifndef __LIBIPMI_API_H__
#define __LIBIPMI_API_H__

#include "libipmiifc.h"

#ifdef  __cplusplus
extern "C" {
#endif

/* Device API */
LIBIPMI_API uint16 LIBIPMI_GetDeviceID( IPMI20_SESSION_T *pSession, char *pszDeviceID, int timeout );

/* User Management functions */
LIBIPMI_API uint16 LIBIPMI_SetUser( IPMI20_SESSION_T *pSession, char *pszUsername, uint8 byID, int timeout );
LIBIPMI_API uint16 LIBIPMI_GetUser( IPMI20_SESSION_T *pSession, char *pszUsername, uint8 byID, int timeout );

#ifdef  __cplusplus
}
#endif

#endif


