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

#ifndef _USB_PROC_H_
#define _USB_PROC_H_


#if (LINUX_VERSION_CODE > KERNEL_VERSION(3,4,11))
extern int ReadUsbeStatus(struct file *file,  char __user *buf, size_t count, loff_t *offset);
#else
extern int ReadUsbeStatus(char *buf, char **start, off_t offset, int count, int *eof, void *data);
#endif

#endif /* _USB_PROC_H_ */

