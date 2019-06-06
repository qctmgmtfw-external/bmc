/*****************************************************************
 **                                                             **
 **     (C) Copyright 2006-2009, American Megatrends Inc.       **
 **                                                             **
 **             All Rights Reserved.                            **
 **                                                             **
 **         5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                             **
 **         Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                             **
 ****************************************************************/

/***************************************************************
* @file   	Reader.c
* @author 	Govind Kothandapani		<govindk@ami.com>
*		Baskar Parthiban		<bparthiban@ami.com>
*		Varadachari Sudan Ayanam 	<varadacharia@ami.com>
* @brief  	contains functions that reads and processes the
*			data from client
****************************************************************/

#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>

#include "sync.h"
#include "reader.h"
#include "advisererrno.h"
#include "dbgout.h"
#include "errno.h"

CoreFeatures_T g_corefeatures;

/**
 * @brief recv_bytes () - receives the data from the pipe
 * @return SUCCESS on success, ERROR on failure
**/
int
recv_bytes (int sock, u8* buf, u32 len, session_info_t* p_si)
{
	int     dwIndex = 0;
	int     nBytes = 0;
	fd_set         	rfds;           /* Read file descriptors */
	int            	retval;         /* Select return value */
	struct timeval tv = { 0,0 };

	do
	{
		FD_ZERO(&rfds);
		FD_SET(sock, &rfds);
		tv.tv_sec = 60;
		tv.tv_usec = 0;

		retval = select (sock + 1, &rfds, NULL, NULL, &tv);
		if (retval > 0)
		{
			if (FD_ISSET(sock, &rfds))
			{
				nBytes = recv(sock, &buf[dwIndex], len, 0);
				if (nBytes == 0)
				{
					if(p_si != NULL)
					{
						//TINFO_4("No data available for reading\n");
						if (p_si->is_used)
						{
							acquire_mutex (p_si->p_access_guard);
							p_si->clean = TRUE;
							if(g_corefeatures.kvm_reconnect_support == ENABLED)
							{
								if(p_si->failed_time == 0)
								{
									time(&p_si->failed_time);
								}
							}
							release_mutex (p_si->p_access_guard);
						}
					}
					return -1;
				}
				else if (nBytes == -1)
				{
					if(p_si != NULL)
					{
						//TINFO_4("Unable to read the socket for data\n");
						if (p_si->is_used)
						{
							acquire_mutex (p_si->p_access_guard);
							p_si->clean = TRUE;
							if(g_corefeatures.kvm_reconnect_support == ENABLED)
							{
								if(p_si->failed_time == 0)
								{
									time(&p_si->failed_time);
								}
							}
							release_mutex (p_si->p_access_guard);
						}
					}
					return -1;
				}
			}
		}
		else if (retval == 0)
		{
			TWARN("select timed out while waiting for data\n");
			if(p_si != NULL)
			{
				if (p_si->is_used)
				{
					acquire_mutex (p_si->p_access_guard);
					p_si->clean = TRUE;
					if(g_corefeatures.kvm_reconnect_support == ENABLED)
					{
						if(p_si->failed_time == 0)
						{
							time(&p_si->failed_time);
						}
					}
					release_mutex (p_si->p_access_guard);
				}
			}
			return -1;
		}
		else if (retval == EINTR)
		{
			TWARN("select interrupted by signal\n");
			continue;
		}
		else
		{
			TWARN("some othr error in select\n");
			if(p_si != NULL)
			{
				if (p_si->is_used)
				{
					acquire_mutex (p_si->p_access_guard);
					p_si->clean = TRUE;
					if(g_corefeatures.kvm_reconnect_support == ENABLED)
					{
						if(p_si->failed_time == 0)
						{
							time(&p_si->failed_time);
						}
					}
					release_mutex (p_si->p_access_guard);
				}
			}
			return -1;
		}
		len -= nBytes;
		dwIndex += nBytes; 
	}while(len);
    return SUCCESS;
}


