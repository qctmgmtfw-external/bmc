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

#include <linux/version.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include "proc.h"

unsigned int m_kcs_hw_test_enable = 0;

#if (LINUX_VERSION_CODE > KERNEL_VERSION(3,4,11))
int kcs_hw_write_test_setting (struct file *file,  const char __user *buff, size_t len, loff_t *offset)
#else
ssize_t kcs_hw_write_test_setting ( struct file *filp, const char __user *buff, unsigned long len, void *data )
#endif
{
	char buf_val[16];
	if (copy_from_user( buf_val, buff, len )) 
		return -EFAULT;
	buf_val[len] = '\0';
	m_kcs_hw_test_enable = (unsigned int) simple_strtol(buf_val, NULL, 10);
	return len;
}


#if (LINUX_VERSION_CODE > KERNEL_VERSION(3,4,11))
int kcs_hw_read_test_setting (struct file *file,  char __user *buf, size_t count, loff_t *offset)
#else
int kcs_hw_read_test_setting ( char *buf, char **start, off_t offset, int count, int *eof, void *data )
#endif
{
	if (m_kcs_hw_test_enable == ENABLE_DRIVER_TESTING)
		printk("KCS HW tests are enabled\n");
	else
		printk("KCS HW tests are disabled\n");

	return 0;
}

