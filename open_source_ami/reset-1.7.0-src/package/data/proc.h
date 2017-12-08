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

#ifndef __LPCRESET_PROC_H__
#define __LPCRESET_PROC_H__

#define ENABLE_RESET_DRIVER_TESTING           7800

int reset_write_test_setting ( struct file *filp, const char __user *buff, unsigned long len, void *data );
int reset_read_test_setting ( char *buf, char **start, off_t offset, int count, int *eof, void *data );

#endif

