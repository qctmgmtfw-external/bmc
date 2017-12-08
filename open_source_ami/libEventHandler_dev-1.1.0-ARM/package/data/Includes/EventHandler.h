/****************************************************************
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2009-2010, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        6145-F, Northbelt Parkway, Norcross,                **
 **                                                            **
 **        Georgia - 30071, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 *****************************************************************
 *
 * FileName    : EventHandler.h
 * 
 * Description : Functions defined for EventHandler
 *
 * Author      : Ram Krishna Poricha <ramkrishnap@amiindia.co.in>
 *****************************************************************/

#ifndef __EVENT_HANDLER_H__
#define __EVENT_HANDLER_H__

#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include "IPMI_SEL.h"

#define IPMI_SEL_EVENT_FIFO "/tmp/SELEvent_fifo"
#define SUCCESS 0
#define FAILURE -1

typedef struct
{
	SELEventRecord_T SelEvt;
	int DestSel;
	int ChannelNum;
}Event_T;

int SetSELEvent(SELEventRecord_T * sel_evt, int DestSel, int ChannelNum);

#endif
