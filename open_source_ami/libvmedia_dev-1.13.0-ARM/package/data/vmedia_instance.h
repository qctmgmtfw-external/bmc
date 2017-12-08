/****************************************************************
 **                                                            **
 **    (C)Copyright 2006-2009, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
****************************************************************/

#ifndef __VMEDIA_INSTANCE_H__
#define __VMEDIA_INSTANCE_H__

#include "coreTypes.h"
#include "token.h"

// error codes
#define VMEDIA_E_SUCCESS	(0x00) // success
#define VMEDIA_E_IN_USE		(0x02) // vMedia is already in use by someother user

int set_cdrom_redir(char*, char*, uint8*);
int	reset_cdrom_redir(char *, unsigned long *racsession_id, unsigned char *is_other_vmedia_in_use);
int set_floppy_redir(char *, char*, uint8 *);
int	reset_floppy_redir(char *, unsigned long *racsession_id, unsigned char *is_other_vmedia_in_use);
int set_hdisk_redir(char *, char*, uint8 *);
int	reset_hdisk_redir(char *, unsigned long *racsession_id, unsigned char *is_other_vmedia_in_use);
int set_vmedia_racsession_id(unsigned long racsession_id);
int Checkanyinstancerunning(int gWasInstanceUsed[],int NumInstances);

#endif /* __VMEDIA_INSTANCE_H__ */



