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
#ifndef _I2C_LOG_H
#define _I2C_LOG_H

#define LOG_I2C_MW  0
#define LOG_I2C_MR  1

#ifdef CONFIG_SPX_FEATURE_ENABLE_I2C_LOG
#include <linux/i2c.h>

void log_i2c_proc_open(struct i2c_adapter *pdev);
void log_i2c_proc_release(struct i2c_adapter *pdev);
void log_i2c(struct i2c_adapter *pdev, struct i2c_msg *msg, unsigned char rw);
void log_i2c_plain(struct i2c_adapter *pdev, char *buf, int len);
int log_i2c_enabled(struct i2c_adapter *pdev);
int print_bus(struct i2c_adapter *pdev);
#define  log_i2c_err(pdev,format,arg...)\
	if(log_i2c_enabled(pdev))	\
	{				\
		printk(KERN_DEBUG "I2C%d:" format, print_bus(pdev),## arg);\
	}				
#else
#define log_i2c_proc_open(pdev)		do { } while (0)
#define log_i2c_proc_release(pdev)	do { } while (0)
#define log_i2c(pdev, msg, rw)		do { } while (0)
#define log_i2c_plain(pdev, buf, len)	do { } while (0)
#define log_i2c_err(pdev,format,arg...) do { } while (0)			
#endif // CONFIG_SPX_FEATURE_ENABLE_I2C_LOG

#endif // _I2C_LOG_H
