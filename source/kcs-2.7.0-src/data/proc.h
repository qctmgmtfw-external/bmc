/*
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2009-2015, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 */

#ifndef __KCS_PROC_H__
#define __KCS_PROC_H__

#define ENABLE_DRIVER_TESTING		7835

#include <linux/version.h>

#if (LINUX_VERSION_CODE > KERNEL_VERSION(3,4,11))
int kcs_hw_write_test_setting (struct file *file,  const char __user *buff, size_t len, loff_t *offset);
int kcs_hw_read_test_setting (struct file *file,  char __user *buf, size_t count, loff_t *offset);
#else
ssize_t kcs_hw_write_test_setting ( struct file *filp, const char __user *buff, unsigned long len, void *data );
int kcs_hw_read_test_setting ( char *buf, char **start, off_t offset, int count, int *eof, void *data );
#endif


#endif
