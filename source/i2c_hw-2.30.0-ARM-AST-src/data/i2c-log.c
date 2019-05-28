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
#ifdef CONFIG_SPX_FEATURE_ENABLE_I2C_LOG

#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <asm/uaccess.h>
#if (LINUX_VERSION_CODE >=  KERNEL_VERSION(2,6,24)) 
#include <linux/slab.h>
#include "../fs/proc/internal.h"
#endif

#include "i2c-data.h"
#include "i2c-log.h"

// /proc/log-i2cN contains configuration for I2C bus N logging:
// log: 0 | 1
// f1: XX 	Filter not used.
// f1: 4F *	Enable filter for I2C address 4F. Only the frames with I2C address 4F are traced.
// f1: 4F 02 CA	Enable filter for I2C address 4F with data at offset 02 value CA. Only the frames with I2C address 4F and data byte CA on offset 02 are traced. The offset is 0 based.
// Max number of filters is I2C_LOG_FILTER_COUNT_MAX (it's 6 now).
#if (LINUX_VERSION_CODE >=  KERNEL_VERSION(2,6,24)) 
static ssize_t log_i2c_proc_read(struct file *file, char *buffer, size_t size, loff_t *ppos)
{
	void *data = PDE_DATA(file_inode(file));
#else
static int log_i2c_proc_read(char *buffer, char **buffer_location,
	off_t offset, int buffer_length, int *eof, void *data)
{
#endif
	struct log_i2c_conf *log_conf = data;
	int len;
	u8 i;
	struct log_i2c_filter *filter;

	if (!log_conf)
	{
		printk(KERN_ERR "%s() empty data\n", __FUNCTION__);
		return -EFAULT;
	}

	#if (LINUX_VERSION_CODE <  KERNEL_VERSION(2,6,24)) 
	*eof = 1;

	if (offset > 0)
		return 0;
	#endif

	len = sprintf(buffer, "log: %d\n", log_conf->log_enabled);

	for (i = 0; i < I2C_LOG_FILTER_COUNT_MAX; ++i)
	{
		filter = &log_conf->filters[i];
		if (filter->mode == I2C_LOG_ALL) {
			len += sprintf(buffer + len, "f%d: %02X *\n",
				i + 1, filter->addr);
		} else if (filter->mode == I2C_LOG_FILTER) {
			len += sprintf(buffer + len, "f%d: %02X %02X %02X\n",
				i + 1, filter->addr, filter->offset, filter->val);
		} else {
			len += sprintf(buffer + len, "f%d: XX\n", i + 1);
		}
	}


	if (len > PAGE_SIZE)
	{
		printk(KERN_ERR
			"%s() trying to pass data larger than PAGE_SIZE\n",
			__FUNCTION__);
	}

	return len;
}

static void log_i2c_read_filter(struct log_i2c_conf *log_conf, char *line)
{
	char *p;
	struct log_i2c_filter filter;
	unsigned val, filter_id;

	if (!line || !*line)
		return;

	p = strchr(line, ':');
	if (!p)
		return;
	*p = '\0';
	if (sscanf(line, "f%d", &val) != 1)
		return;

	filter_id = val - 1; // "f1" assumed to be the first filter
	if (filter_id >= I2C_LOG_FILTER_COUNT_MAX)
	{
		printk(KERN_ERR "%s() wrong filter_id %d\n",
			__FUNCTION__, filter_id);
		return;
	}

	line = p + 1; // skip "fN:" part

	if (sscanf(line, "%02X %02X %02X",
		&filter.addr, &filter.offset, &filter.val) == 3)
	{
		filter.mode = I2C_LOG_FILTER;
	} else if (sscanf(line, "%02X", &filter.addr) == 1) {
		filter.mode = I2C_LOG_ALL;
	} else {
		filter.mode = I2C_LOG_NONE;
	}

	log_conf->filters[filter_id] = filter;
}

static void log_i2c_read_line(struct log_i2c_conf *log_conf, char *line)
{
	char *p;

	if (!line || !*line)
		return;

	p = strstr(line, "log:");
	if (p)
	{
		if (strchr(p, '1')) {
			log_conf->log_enabled = 1;
		} else if (strchr(p, '0')) {
			log_conf->log_enabled = 0;
		}
		return;
	}
	log_i2c_read_filter(log_conf, line);
}

#if (LINUX_VERSION_CODE >=  KERNEL_VERSION(2,6,24)) 
static ssize_t log_i2c_proc_write(struct file *file, const char __user *buffer, size_t buffer_length, loff_t *ppos)
{
	void *data = PDE_DATA(file_inode(file));
#else
static int log_i2c_proc_write(struct file *file, const char __user *buffer,
	unsigned long buffer_length, void *data)
{
#endif
	struct log_i2c_conf *log_conf = data;
	char *str, *p, *line;

	if (!log_conf)
	{
		printk(KERN_ERR "%s() empty data\n", __FUNCTION__);
		return -EFAULT;
	}

	if (buffer_length == 0)
		return 0;

	str = kmalloc(buffer_length, GFP_KERNEL);
	if (!str)
		return -ENOMEM;
	if (copy_from_user(str, buffer, buffer_length))
	{
		kfree(str);
		return -EFAULT;
	}
	str[buffer_length - 1] = '\0';

	p = str;
	while (p && *p)
	{
		line = strsep(&p, "\n");
		log_i2c_read_line(log_conf, line);
	}

	kfree(str);
	return buffer_length;
}

static void get_proc_name(int bus, char *buf, size_t size)
{
	snprintf(buf, size, "log-i2c%d", bus);
}

static void log_conf_init(struct log_i2c_conf *log_conf)
{
	unsigned i;

	log_conf->log_enabled = 0;
	for (i = 0; i < I2C_LOG_FILTER_COUNT_MAX; ++i)
		log_conf->filters[i].mode = I2C_LOG_NONE;
}

#if (LINUX_VERSION_CODE >=  KERNEL_VERSION(2,6,24)) 
static const struct file_operations log_i2c_proc_fops = {
	.owner = THIS_MODULE,
	.read  = log_i2c_proc_read,
	.write = log_i2c_proc_write,
};
#endif

void log_i2c_proc_open(struct i2c_adapter *pdev)
{
	struct i2c_as_data *algo_data;
	struct log_i2c_conf *log_conf;
	struct proc_dir_entry *pde;
	char name[32];

	algo_data = pdev->algo_data;
	if (!algo_data)
	{
		printk(KERN_ERR "%s() empty algo_data\n", __FUNCTION__);
		return;
	}

	log_conf = &(algo_data->log_conf);
	log_conf_init(log_conf);

	get_proc_name(pdev->nr, (char*)name, sizeof(name));

	// create entry in /proc root
	#if (LINUX_VERSION_CODE >=  KERNEL_VERSION(2,6,24)) 
	pde = proc_create(name, 0644, NULL, &log_i2c_proc_fops);
	#else
	pde = create_proc_entry(name, 0644, NULL);
	#endif
	if (pde == NULL)
	{
		remove_proc_entry(name, NULL);
		printk(KERN_ERR "Could not initialize /proc/%s\n", name);
		return;
	}

	#if (LINUX_VERSION_CODE <  KERNEL_VERSION(2,6,24)) 
	// specify read/write callbacks
	pde->read_proc	= log_i2c_proc_read;
	pde->write_proc	= log_i2c_proc_write;

	// specify file attributes for file
	pde->owner	= THIS_MODULE;
	pde->uid 	= 0;
	pde->gid 	= 0;	
	#endif	
	pde->mode 	= S_IFREG | S_IRUGO;

	pde->data = log_conf;
}

void log_i2c_proc_release(struct i2c_adapter *pdev)
{
	char name[32];
	get_proc_name(pdev->nr, (char*)name, sizeof(name));
	remove_proc_entry(name, NULL);
}

static void log_msg(int bus, struct i2c_msg *msg, unsigned char rw)
{
	u16 i;
	//printk(KERN_DEBUG "I2C%d: %02X", bus, ((msg->addr<<1) | rw));
	printk("I2C%d: %02X", bus, ((msg->addr<<1) | rw));
	for (i = 0; i < msg->len; ++i) {
		printk(" %02X", msg->buf[i]);
		if( i >= 0xFF) {
			printk(" ....");
			i = msg->len - 1;
			break;
		}
	}
	printk("\n");
}

static int log_i2c_match_filter(struct log_i2c_filter *filter,
	struct i2c_msg *msg)
{
	// use address bits from message, not r/w bit
	if (filter->addr != ((msg->addr << 1 ) & 0xFE))
		return 0;

	if (filter->mode == I2C_LOG_ALL)
		return 1;

	if (filter->mode == I2C_LOG_FILTER &&
		filter->offset < msg->len &&
		filter->val == msg->buf[filter->offset])
	{
		return 1;
	}
	return 0;
}

void log_i2c(struct i2c_adapter *pdev, struct i2c_msg *msg, unsigned char rw)
{
	struct i2c_as_data *data;
	struct log_i2c_conf *log_conf;
	u8 i, match, filter_specified;
	struct log_i2c_filter *filter;

	data = pdev->algo_data;
	if (!data)
	{
		printk(KERN_DEBUG "%s() empty algo_data\n", __FUNCTION__);
		return;
	}

	log_conf = &(data->log_conf);

	if (!log_conf->log_enabled)
	{
		return;
	}
	filter_specified = 0;
	match = 0;
	for (i = 0; i < I2C_LOG_FILTER_COUNT_MAX; ++i)
	{
		filter = &log_conf->filters[i];
		if (filter->mode != I2C_LOG_NONE)
			filter_specified = 1;
		match += log_i2c_match_filter(filter, msg);
	}

	if (!filter_specified || match)
		log_msg(pdev->nr, msg, (rw & 0x01));
}
// Assume buf[0] is I2C address
void log_i2c_plain(struct i2c_adapter *pdev, char *buf, int len)
{
	struct i2c_msg msg;

	msg.addr = buf[0] >>1;
	msg.flags = 0;
	msg.buf = &buf[1];
	msg.len = len - 1;

	log_i2c(pdev, &msg, LOG_I2C_MW);
}

int log_i2c_enabled(struct i2c_adapter *pdev)
{
        struct log_i2c_conf *log_conf;
        struct i2c_as_data *data;

        data = pdev->algo_data;

        if (!data)
        {
                printk(KERN_DEBUG "%s() empty algo_data\n", __FUNCTION__);
                return 0;
        }

        log_conf = &(data->log_conf);

        if (!log_conf->log_enabled)
                return 0;

        return 1;
}

int print_bus(struct i2c_adapter *pdev)
{
        return pdev->nr;
}

#endif // CONFIG_SPX_FEATURE_ENABLE_I2C_LOG
