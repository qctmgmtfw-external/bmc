/* @(#)libipmi_ChassisDevice.h
 */

#ifndef _LIBIPMI_CHASSISDEVICE_H
#define _LIBIPMI_CHASSISDEVICE_H 1


/* LIIPMI core header files */
#include "libipmi_session.h"
#include "libipmi_errorcodes.h"
#include "libipmiifc.h"

/* command specific header files */
#include "IPMI_ChassisDevice.h"
#include "IPMI_Chassis.h"

#ifdef  __cplusplus
extern "C" {
#endif




/*---------- LIBIPMI Higher level routines -----------*/


#define CHASSIS_POWEROFF	 0x00
#define CHASSIS_POWERUP		 0x01
#define CHASSIS_POWERCYCLE	 0x02
#define CHASSIS_HARDRESET	 0x03
#define CHASSIS_DIAGINT          0x04
#define CHASSIS_SOFTOFF	         0x05





/**
   \breif	Higher level function for PowerOff Host.
   @param	pSession		[in]Session handle
   @param	timeout			[in]timeout value in seconds.

   @retval Returns LIBIPMI_STATUS_SUCCESS on success and error codes on failure
*/
LIBIPMI_API uint16 LIBIPMI_HL_PowerOff( IPMI20_SESSION_T *pSession/*in*/, int timeout/*in*/);



/**
   \breif	Higher level function for PowerUP Host
   @param	pSession		[in]Session handle
   @param	timeout			[in]timeout value in seconds.

   @retval Returns LIBIPMI_STATUS_SUCCESS on success and error codes on failure
*/
LIBIPMI_API uint16 LIBIPMI_HL_PowerUp( IPMI20_SESSION_T *pSession/*in*/, int timeout/*in*/);



/**
   \breif	Higher level function for PowerCycle host.
   @param	pSession		[in]Session handle
   @param	timeout			[in]timeout value in seconds.

   @retval Returns LIBIPMI_STATUS_SUCCESS on success and error codes on failure
*/
LIBIPMI_API uint16 LIBIPMI_HL_PowerCycle( IPMI20_SESSION_T *pSession, int timeout );



/**
   \breif	Higher level function for HardReset
   @param	pSession		[in]Session handle
   @param	timeout			[in]timeout value in seconds.

   @retval Returns LIBIPMI_STATUS_SUCCESS on success and error codes on failure
*/
LIBIPMI_API uint16 LIBIPMI_HL_HardReset( IPMI20_SESSION_T *pSession, int timeout );
    

/**
   \breif	Higher level function for DiagInt
   @param	pSession		[in]Session handle
   @param	timeout			[in]timeout value in seconds.

   @retval Returns LIBIPMI_STATUS_SUCCESS on success and error codes on failure
*/
LIBIPMI_API uint16 LIBIPMI_HL_DiagInt( IPMI20_SESSION_T *pSession, int timeout );
    

/**
   \breif	Higher level function for SoftOff
   @param	pSession		[in]Session handle
   @param	timeout			[in]timeout value in seconds.

   @retval Returns LIBIPMI_STATUS_SUCCESS on success and error codes on failure
*/
LIBIPMI_API uint16 LIBIPMI_HL_SoftOff( IPMI20_SESSION_T *pSession, int timeout );
    


typedef enum tag_CHASSIS_ID_STATE
{
	CHASSIS_ID_OFF=0,
	CHASSIS_ID_TIMED_ON=1,
	CHASSIS_ID_INDEFINITE_ON=2
} CHASSIS_ID_STATE;

typedef struct tag_chassis_status_T
{
	unsigned char power_state;
	unsigned char last_power_event;
	unsigned char chassis_indetify_supported;
	CHASSIS_ID_STATE chassis_identify_state;
} chassis_status_T;

/** 
 * 
 * 
 *  @param	pSession		[in]Session handle
 *  @param	timeout			[in]timeout value in seconds.
 *  @param      status			[out]status of the host is returned.
 * 
 * @return  Returns LIBIPMI_STATUS_SUCCESS on success and error codes on failure
 */
LIBIPMI_API uint16	LIBIPMI_HL_GetChassisStatus( IPMI20_SESSION_T *pSession, int timeout,  chassis_status_T *status);

/*---------------------Chassis Idnetify defines ---------------------*/
#define CHASSIS_ID_INTERVAL_TURN_OFF		0
#define CHASSIS_ID_INTERVAL_SPEC_DEFAULT   	15

typedef enum tag_FORCE_CHASSIS_ID
{
	USE_INTERVAL=0,
	FORCE=1
} FORCE_CHASSIS_ID;

/** 
 * 
 * 
 *  @param	pSession		[in]Session handle
 *  @param	timeout			[in]timeout value in seconds.
 *  @param      BlinkTimeSecs		[in]BlinkTimeInSecs.0 for Turn off (use #defs above)
 *  @param 	ForceOn			[in] Whether Interval is indefinite (Use enum above)
 * 
 * @return  Returns LIBIPMI_STATUS_SUCCESS on success and error codes on failure
 */
LIBIPMI_API uint16 LIBIPMI_HL_ChassisIdentify(IPMI20_SESSION_T *pSession, int timeout,INT8U BlinkTimeSecs,FORCE_CHASSIS_ID ForceOn);

uint16	LIBIPMI_HL_GetSystemBootOptions_BootFlags( IPMI20_SESSION_T *pSession, BootFlags_T* pBootFlags,int timeout);
uint16	LIBIPMI_HL_SetSystemBootOptions_BootFlags( IPMI20_SESSION_T *pSession, BootFlags_T* pBootFlags,int timeout);

    

#ifdef  __cplusplus
}
#endif


#endif /* _LIBIPMI_CHASSISDEVICE_H */

