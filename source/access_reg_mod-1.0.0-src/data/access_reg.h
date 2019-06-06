
#ifndef _ACCESS_REG_H_
#define _ACCESS_REG_H_

#define ACCESS_REG_DEVICE_INDEX	0
#define ACCESS_REG_DEVICE_NUM		1
#define ACCESS_REG_INDEX 			0
/* #define ACCESS_REG_IRQ 			20 */
#define ACCESS_REG_MAJOR   			241
#define ACCESS_REG_MINOR			0

#define ACCESS_REG_NAME 	"ACCESS_REG"
/* #define REG_PHY_BASE 		0x14000000 */
#define ACCESS_REG_IO_SIZE 		0x04

/* on/off */
#define ENABLE 		0x01
#define DISABLE 	0X00

/* read/Write Register marco */
#define	ReadByte(addr)			ioread8( (void *)(addr) )
#define	WriteByte(addr, mask, value)	(iowrite8( ((ioread8((void *)(addr)) & ~(mask)) | ((value) & (mask))), (void *)(addr)))

/*  user space and kernel space */

struct access_reg_ctrl_data {
	unsigned long value;
	/* unsigned char offset; */
	/* unsigned char mask; */
	unsigned long  addr;
	unsigned long  size;
	unsigned long  *buf;
};

/* data struct just for kernel space  */
struct access_reg_client {
	/* unsigned char pin; */
	struct access_reg_ctrl_data ctrl_data;
	char name[32];
};

/* data struct for kernel api */
struct kfile {
	void *private_data;
};

struct kinode {
	dev_t i_rdev;
};

struct access_reg_kprocess{
	struct kfile file;
	struct kinode inode;
};

/* Use 'x' as magic number */
#define REG_IOC_MAGIC 'x'

#define ACCESS_REG_SEND_CTRL_DATA		_IOW(REG_IOC_MAGIC, 0, struct access_reg_ctrl_data)
#define ACCESS_REG_GET_VALUE			_IOR(REG_IOC_MAGIC, 1, unsigned long)
#define ACCESS_REG_SET_VALUE			_IO(REG_IOC_MAGIC, 2)
#define ACCESS_REG_DUMP					_IO(REG_IOC_MAGIC, 3)

#define REG_IOC_MAXNR	    4

#endif /* _REG_H_ */
