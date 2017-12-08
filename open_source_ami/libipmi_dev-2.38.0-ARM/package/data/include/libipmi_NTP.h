

#ifndef __LIBIPMI_NTP_H__
#define __LIBIPMI_NTP_H__

/* LIIPMI core header files */
#include "libipmi_session.h"
#include "libipmi_errorcodes.h"
#include "libipmiifc.h"


#ifdef  __cplusplus
extern "C" {
#endif

/*---------- IPMI Command direct routines ----------*/
LIBIPMI_API uint16	IPMICMD_SetTimeZone( IPMI20_SESSION_T *pSession/*in*/,
										uint8* pTimeZone/*out*/,
										int timeout/*in*/);

#ifdef  __cplusplus
}
#endif


#endif


