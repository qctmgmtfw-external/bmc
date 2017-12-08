/**
 * @file   libipmi_ATCA.c
 * @author Balakumar (balakumart@amiindia.co.in)
 * @date   23-jun-2008
 *
 * @brief  Contains exported Handle Switch and FRU LED API by LIBIPMI for
 *			communicating with the BMC.
 *
 */

#ifndef __LIBIPMI_ATCA_H__
#define __LIBIPMI_ATCA_H__

/* LIIPMI core header files */
#include "libipmi_session.h"
#include "libipmi_errorcodes.h"
#include "libipmiifc.h"

/* command specific header files */
#include "IPMI_AMIDevice.h"
#include "IPMC_PICMG.h"

LIBIPMI_API uint16	IPMCCMD_GetFRULedProperties( IPMI20_SESSION_T *pSession,
							GetFRULedPropReq_T	*pGetFRULedPropReq,
							GetFRULedPropRes_T *pGetFRULedPropRes,
							int timeout);
LIBIPMI_API uint16	IPMCCMD_GetLedColorCaps( IPMI20_SESSION_T *pSession,
							GetLedColorCapsReq_T	*pGetLedColorCapsReq,
							GetLedColorCapsRes_T *pGetLedColorCapsRes,
							int timeout);
LIBIPMI_API uint16	IPMCCMD_SetFRULedState( IPMI20_SESSION_T *pSession,
							SetFRULedStateReq_T *pSetFRULedStateReq,
							SetFRULedStateRes_T *pSetFRULedStateRes,
							int timeout);
LIBIPMI_API uint16	IPMCCMD_GetFRULedState( IPMI20_SESSION_T *pSession,
							GetFRULedStateReq_T *pGetFRULedStateReq,
							GetFRULedStateRes_T *pGetFRULedStateRes,
							int timeout);


#endif
