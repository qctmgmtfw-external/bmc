/*****************************************************************
 **                                                             **
 **     (C) Copyright 2009-2015, American Megatrends Inc.       **
 **                                                             **
 **             All Rights Reserved.                            **
 **                                                             **
 **         5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                             **
 **         Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                             **
 ****************************************************************/

//#include <linux/ide.h>
#include <linux/version.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include "proc.h"
#include "reset.h"

unsigned int m_reset_test_enable = ENABLE_RESET_DRIVER_TESTING;
extern int TestResetCount(void);

#if (LINUX_VERSION_CODE > KERNEL_VERSION(3,4,11))
int reset_write_test_setting (struct file *file,  const char __user *buff, size_t len, loff_t *offset)
#else
int reset_write_test_setting ( struct file *filp, const char __user *buff, unsigned long len, void *data )
#endif
{

        char buf_val[16];
	
	if (len > 16)
                return -EFAULT;

	if (copy_from_user( buf_val, buff, len ))
                return -EFAULT;
	
        buf_val[len] = '\0';
        m_reset_test_enable = (unsigned int) simple_strtol(buf_val, NULL, 10);

	if (m_reset_test_enable == ENABLE_RESET_DRIVER_TESTING)
	{	
		printk("installing the reset handler for RESET\n");
		install_reset_handler(TestResetCount);
	}
	else
	{
		printk("uninstalling the reset handler for RESET\n");
		uninstall_reset_handler(TestResetCount);
	}

        return len;
}


#if (LINUX_VERSION_CODE > KERNEL_VERSION(3,4,11))
int reset_read_test_setting (struct file *file,  char __user *buf, size_t count, loff_t *offset)
#else
int reset_read_test_setting ( char *buf, char **start, off_t offset, int count, int *eof, void *data )
#endif
{

        if (m_reset_test_enable == ENABLE_RESET_DRIVER_TESTING)
                printk("LPCRESET tests are enabled\n");
        else
                printk("LPCRESET tests are disabled\n");

        return 0;
}

