/****************************************************************
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2007, American Megatrends Inc.             **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        6145-F, Northbelt Parkway, Norcross,                **
 **                                                            **
 **        Georgia - 30071, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 *****************************************************************/

#ifndef LIBIPMI_NCML_H_
#define LIBIPMI_NCML_H_

//#include "Types.h"
//#include "libipmi_struct.h"

int FindServiceID(char *service_name);
int Get_Service_Conf_UDS (char *service_name, void *Res);
int Set_Service_Conf_UDS (char *service_name, void *Req);

#endif
