#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <asm/uaccess.h>
#include <linux/ioport.h>
#include <asm/io.h>

#include "access_reg.h"


static unsigned long IOBase;
#ifdef ACCESS_REG_MAJOR
	int access_reg_major = ACCESS_REG_MAJOR;
#else
	int access_reg_major = 0;
#endif 	//ACCESS_REG_MAJOR

int access_reg_minor= ACCESS_REG_MINOR;

struct access_reg_dev {
	struct semaphore sem;
	struct cdev cdev;
} ;
struct access_reg_dev *access_reg_dev_ptr=NULL;

//struct semaphore    		access_sem;

#if 1
static struct access_reg_client access_reg_client_template = {
	.name		= "access_reg /dev entry",
};
#endif

/* +++++++ function declared +++++++ */
static int access_reg_open(struct inode *inode, struct file *filp);
static int access_reg_release(struct inode *inode, struct file *filp);
static ssize_t access_reg_write (struct file *file, const char __user *buf, size_t count, loff_t *offset);
static ssize_t access_reg_read (struct file *file, char __user *buf, size_t count, loff_t *offset);
static long access_reg_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);
/* ------- function declared ------- */

/* export_symbol */
int access_reg_get_value(unsigned long addr, unsigned long *value);
int access_reg_set_value(unsigned long addr, unsigned long value);

int access_reg_get_value(unsigned long addr, unsigned long *value)
{
	void * virt_addr= NULL;

	//down(&access_sem);
	virt_addr = ioremap(addr, sizeof(unsigned long));
	if (!virt_addr){
		printk("phy_addr 0x%lx ioremap fail\n", addr);
		//up(&access_sem);
		return -EFAULT;
	}
	*value = ioread32(virt_addr);	
	iounmap(virt_addr);
	//up(&access_sem);

	return 0;	
}

int access_reg_set_value(unsigned long addr, unsigned long value)
{
	void * virt_addr = NULL;
	u32 val;

	//down(&access_sem);
	virt_addr = ioremap(addr, sizeof(unsigned long));
	if (!virt_addr){
		printk("phy_addr 0x%lx ioremap fail\n", addr);
		//up(&access_sem);
		return -EFAULT;
	}

	iowrite32(value, virt_addr);
	val = ioread32(virt_addr);
	if (val != value){
		printk("write value 0x%lx to phy_addr 0x%lx fail\n", value, addr);
		//up(&access_sem);
		return -EFAULT;
	}
	iounmap(virt_addr);
	//up(&access_sem);

	return 0;	
}

static int access_reg_open(struct inode *inode, struct file *filp)
{
	struct access_reg_client *client=NULL;

	//down(&access_sem);
	
	if( iminor(inode) >= ACCESS_REG_DEVICE_NUM ){
		//up(&access_sem);
		return -EINVAL;
	}
	client = kmalloc(sizeof(struct access_reg_client), GFP_KERNEL);
	if (!client) {
		printk("alloc memory for client fail.\n");
		//up(&access_sem);
		return -ENOMEM;
	}
	memcpy(client, &access_reg_client_template, sizeof(*client));
	filp->private_data = client;

    return 0;
}
static int access_reg_release(struct inode *inode, struct file *filp)
{
	struct access_reg_client *client = filp->private_data;

	kfree(client);
	filp->private_data = NULL;
	//up(&access_sem);
    return 0;
}
static ssize_t access_reg_read (struct file *file, char __user *buf, size_t count, loff_t *offset)
{
	printk("umimplement access_reg_read.\n");
	return 0;
}
static ssize_t access_reg_write (struct file *file, const char __user *buf, size_t count, loff_t *offset)
{
	printk("umimplement access_reg_write.\n");
	return 0;
}

static long access_reg_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{		
	struct access_reg_client *client = (struct access_reg_client *)filp->private_data;
	long  ret = 0;
	unsigned int value =0;

	/* client->pin = iminor(inode); */

	//printk("access_reg_drv:ioctl,cmd=0x%x\n",cmd);

	switch(cmd){
	case ACCESS_REG_SEND_CTRL_DATA:
		if(	copy_from_user(&client->ctrl_data, (void *)arg, sizeof(struct access_reg_ctrl_data))){
			printk("copy_from_user fail.\n");
			//up(&access_sem);
			return -EFAULT;
		}
		IOBase = (unsigned long)ioremap( client->ctrl_data.addr, client->ctrl_data.size);
		if (!IOBase){
			printk("ioremap %lx fail\n", client->ctrl_data.addr);
			//up(&access_sem);
			return -1;
		}
		return ret;

	case ACCESS_REG_SET_VALUE:
		iowrite32( client->ctrl_data.value, (void *)IOBase);
		value = ioread32( (void *)IOBase );
		iounmap( (void *)IOBase);
		if (value != client->ctrl_data.value){
			printk( "write data 0x%08lx to addr 0x%lx fail.\n", client->ctrl_data.value, client->ctrl_data.addr);
			//up(&access_sem);
			return -1;
		}
		return 0;

	case ACCESS_REG_GET_VALUE:
		value = ioread32( (void *)IOBase );
		if( copy_to_user( (void __user *)arg, &value, sizeof(unsigned int))){
			printk("copy_to_user fail.\n");
			//up(&access_sem);
			return -EFAULT;
		}
		iounmap( (void *)IOBase);

		return 0;

	case ACCESS_REG_DUMP:
		if( copy_to_user( (void __user *)&client->ctrl_data.buf, (void *)IOBase, client->ctrl_data.size)){
			printk("copy_to_user fail.\n");
			//up(&access_sem);
			return -EFAULT;
		}
		iounmap( (void *)IOBase);

		return 0;
	default:
		printk("unknown cmd !\n");
		return -1;
	}
	
}

struct file_operations access_reg_fops = {
	.owner	= THIS_MODULE,
	.open	= access_reg_open,
	.release= access_reg_release,
	.write	= access_reg_write,
	.read	= access_reg_read,
	.unlocked_ioctl	= access_reg_ioctl,
};

static int  access_reg_setup_cdev(struct access_reg_dev *dev, int index)
{	
	int err;
	
	dev_t devno = MKDEV(access_reg_major, access_reg_minor + index);

	cdev_init(&dev->cdev, &access_reg_fops);
	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops = &access_reg_fops;
	err = cdev_add (&dev->cdev, devno, 1);
	if (err)
		printk( "%d adding cdev access_reg-%d \n", err, index);

	return err;
}

static void __exit access_reg_exit(void){
	//int i;
	//char node_name[32];
	dev_t devno = MKDEV(access_reg_major, access_reg_minor);

	/*for(i=0;i < ACCESS_REG_DEVICE_NUM ;i++){
		sprintf(node_name, "IOreg-%d", i);
		devfs_remove(node_name);
	}*/

	cdev_del(&access_reg_dev_ptr->cdev);
	kfree(access_reg_dev_ptr);
	unregister_chrdev_region( devno, ACCESS_REG_DEVICE_NUM);
}

static int  __init access_reg_init(void)
{	
	int i;
	int ret; 
	dev_t dev;
	//char node_name[32];
	
	// register major/minor NO.
	if (access_reg_major){ // not 0
		dev = MKDEV(access_reg_major, access_reg_minor);
		ret =register_chrdev_region(dev, ACCESS_REG_DEVICE_NUM, ACCESS_REG_NAME);
	}else {	// equal 0
		ret = alloc_chrdev_region(&dev, access_reg_minor, ACCESS_REG_DEVICE_NUM, ACCESS_REG_NAME);
		access_reg_major = MAJOR(dev);
	}
	if (ret < 0){
		printk( "Can't get major %d\n", access_reg_major);
		return ret;
	}
	printk("AccessReg_drv: register major/minor ok,is %d/%d.\n" ,access_reg_major, access_reg_minor);
	
	// register cdev device
	access_reg_dev_ptr = kmalloc ( sizeof(struct access_reg_dev)* ACCESS_REG_DEVICE_NUM, GFP_KERNEL);
	if (!access_reg_dev_ptr){
		printk("alloc mem fail\n");
		ret = -ENOMEM;
		goto fail_char_register;
	}

	for (i = 0; i < ACCESS_REG_DEVICE_NUM; i ++){
		ret = access_reg_setup_cdev(access_reg_dev_ptr, ACCESS_REG_INDEX + i);
		if (ret){
			goto fail_dev_ptr_free;
		}
	}
	/*for (i = 0; i < ACCESS_REG_DEVICE_NUM; i++){
		sprintf(node_name, "IOreg-%d",  i );
		ret = devfs_mk_cdev( MKDEV(access_reg_major, i),S_IFCHR|S_IRUSR|S_IWUSR, node_name);
		if (ret){
			goto fail_cdev_add;
		}
	}*/

	//init_MUTEX(&access_sem);

	return 0;


fail_dev_ptr_free:
	kfree(access_reg_dev_ptr);
fail_char_register:
	unregister_chrdev_region(dev, ACCESS_REG_DEVICE_NUM);

	return ret;
}

module_init(access_reg_init);
module_exit(access_reg_exit);
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Jerry Yang");
MODULE_VERSION("1.0");
MODULE_DESCRIPTION("develop access_reg module");
//MODULE_ALIAS("develop program.");

EXPORT_SYMBOL(access_reg_get_value);
EXPORT_SYMBOL(access_reg_set_value);
