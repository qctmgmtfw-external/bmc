/**
 * @file   libipmi_nework.h
 * @author Rajasekhar (rajasekharj@amiindia.co.in)
 * @date   02-Sep-2004
 *
 * @brief  Contains definition of network(UDP) layer API
 *			required by RMCP layer for connecting to BMC thru network medium
 *			
 */

#ifndef __LIBIPMI_NETWORK_H__
#define __LIBIPMI_NETWORK_H__

#include "libipmi_session.h"

/**
\breif	Creates the socket, initializes it and stores it for further user.
 @param	LanInfo			[in]Lan Info handle
 @param szIPAddress		[in]IP Address of BMC
 @param	wPort			[in]socke port to be connected.

 @retval Returns LIBIPMI_STATUS_SUCCESS on success and error codes on failure
*/
uint8 UDP_Connect(LAN_CHANNEL_T *LanInfo, char *szIPAddress, uint16 wPort );

/**
\breif	Closes the socket
 @param	LanInfo			[in]Lan Info handle
*/
void UDP_Close( LAN_CHANNEL_T *LanInfo );

/**
\breif	Sends data to BMC. If write pipe is full, it waits for specified timeout
		period and sends the data. If the write pipe still stays full even after
		waiting for specified timeout, it returns error
 @param	LanInfo			[in]Lan Info handle
 @param	szBuffer		[in]Data to be sent
 @param	dwSize			[in]Size of the data to be sent.
 @param	timeout			[in]timeout value in seconds.

 @retval Returns LIBIPMI_STATUS_SUCCESS on success and error codes on failure
*/
uint8 UDP_SendData( LAN_CHANNEL_T *LanInfo, char *szBuffer, uint32 dwSize, int timeout );

/**
\breif	Receives data from BMC. If read pipe is not free, it waits for specified timeout
		period and sends the data. If the read pipe still doesnt become free even after
		waiting for specified timeout, it returns error
 @param	LanInfo			[in]Lan Info handle
 @param	szBuffer		[out]Data pointer where the received buffer is saved.
 @param	dwSize			[out]Size of the data to be receievd
 @param	timeout			[in]timeout value in seconds.

 @retval Returns LIBIPMI_STATUS_SUCCESS on success and error codes on failure
*/
uint8 UDP_ReceiveData( LAN_CHANNEL_T *LanInfo, char *szBuffer, uint32* pdwSize, int timeout );


/**
\breif	Creates the socket, initializes it and stores it for further user.
 @param	LanInfo			[in]Lan Info handle
 @param szIPAddress		[in]IP Address of BMC
 @param	wPort			[in]socke port to be connected.

 @retval Returns LIBIPMI_STATUS_SUCCESS on success and error codes on failure
*/
uint8 TCP_Connect(LAN_CHANNEL_T *LanInfo, char *szIPAddress, uint16 wPort );

/**
\breif	Closes the socket
 @param	LanInfo			[in]Lan Info handle
*/
void TCP_Close( LAN_CHANNEL_T *LanInfo );

/**
\breif	Sends data to BMC. If write pipe is full, it waits for specified timeout
		period and sends the data. If the write pipe still stays full even after
		waiting for specified timeout, it returns error
 @param	LanInfo			[in]Lan Info handle
 @param	szBuffer		[in]Data to be sent
 @param	dwSize			[in]Size of the data to be sent.
 @param	timeout			[in]timeout value in seconds.

 @retval Returns LIBIPMI_STATUS_SUCCESS on success and error codes on failure
*/
uint8 TCP_SendData( LAN_CHANNEL_T *LanInfo, char *szBuffer, uint32 dwSize, int timeout );

/**
\breif	Receives data from BMC. If read pipe is not free, it waits for specified timeout
		period and sends the data. If the read pipe still doesnt become free even after
		waiting for specified timeout, it returns error
 @param	LanInfo			[in]Lan Info handle
 @param	szBuffer		[out]Data pointer where the received buffer is saved.
 @param	dwSize			[out]Size of the data to be receievd
 @param	timeout			[in]timeout value in seconds.

 @retval Returns LIBIPMI_STATUS_SUCCESS on success and error codes on failure
*/
uint8 TCP_ReceiveData( LAN_CHANNEL_T *LanInfo, char *szBuffer, uint32* pdwSize, int timeout );


#endif /* __LIBIPMI_NETWORK_H__ */


