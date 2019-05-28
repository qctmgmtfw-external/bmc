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
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/poll.h>
#include <linux/pci.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/cacheflush.h>
#include <linux/unistd.h>
#include <linux/cdev.h>

#if (LINUX_VERSION_CODE <  KERNEL_VERSION(3,4,11))
#ifndef AUTOCONF_INCLUDED
#include <linux/config.h>
#endif
#endif

#include <linux/errno.h>
#include <linux/sched.h>
#include <asm/delay.h>

#include "driver_hal.h"
#include "helper.h"
#include "cipher.h"

#ifdef HAVE_UNLOCKED_IOCTL  
  #if HAVE_UNLOCKED_IOCTL  
	#define USE_UNLOCKED_IOCTL  
  #endif  
#endif 
MODULE_AUTHOR( "American Megatrends Inc." );
MODULE_DESCRIPTION( "Cipher Common Driver " );
MODULE_LICENSE( "GPL" );

#ifdef DEBUG
#define dbgprint(fmt, args...)       printk (KERN_INFO fmt, ##args) 
#else
#define dbgprint(fmt, args...)       
#endif

#define CIPHER_MAJOR           126
#define CIPHER_MINOR           0
#define CIPHER_MAX_DEVICES     1
#define CIPHER_DEV_NAME        "cipher_dev"

/* ------------------------------------------------------------------------- *
 *  Function prototypes
 * ------------------------------------------------------------------------- */
static int  __init cipher_init ( void );
static void __exit cipher_exit ( void );

static int  cipher_open     ( struct inode *inode, struct file *filp );
static int  cipher_release  ( struct inode *inode, struct file *filp );
#ifdef USE_UNLOCKED_IOCTL
static long  cipher_ioctlUnlocked    ( struct file *file, uint cmd, ulong arg );
#else
static int  cipher_ioctl    ( struct inode *inode, struct file *file, uint cmd, ulong arg );
#endif

static int process_cipher (uint cmd, struct cipher_dev *pdev, cipher_data_t  *cipher_data);

static int process_get_info (struct cipher_dev *pdev, cipher_data_t *cipher_data);
static int process_data_padding (struct cipher_dev *pdev, cipher_data_t *cipher_data);
static int process_remove_data_padding (struct cipher_dev *pdev, cipher_data_t  *cipher_data);
static int process_aes (struct cipher_dev *pdev, cipher_data_t *cipher_data);
static int process_rc4 (struct cipher_dev *pdev, cipher_data_t *cipher_data);
static int process_des (struct cipher_dev *pdev, cipher_data_t *cipher_data);
static int process_3des (struct cipher_dev *pdev, cipher_data_t *cipher_data);
static int process_rsa (struct cipher_dev *pdev, cipher_data_t *cipher_data);
static int process_rng (struct cipher_dev *pdev, cipher_data_t *cipher_data);
static int process_sha1 (struct cipher_dev *pdev, cipher_data_t *cipher_data);
static int process_sha224 (struct cipher_dev *pdev, cipher_data_t *cipher_data);
static int process_sha256 (struct cipher_dev *pdev, cipher_data_t *cipher_data);
static int process_md5 (struct cipher_dev *pdev, cipher_data_t *cipher_data);
static int process_hmac_sha1 (struct cipher_dev *pdev, cipher_data_t *cipher_data);
static int process_hmac_sha224 (struct cipher_dev *pdev, cipher_data_t *cipher_data);
static int process_hmac_sha256 (struct cipher_dev *pdev, cipher_data_t *cipher_data);
static int process_hmac_md5 (struct cipher_dev *pdev, cipher_data_t *cipher_data);

/* cipher file operations */
static struct file_operations cipher_ops =
{
    open    : cipher_open,
    release : cipher_release,
#ifdef USE_UNLOCKED_IOCTL 
    unlocked_ioctl:	cipher_ioctlUnlocked, 
#else 
    ioctl	: cipher_ioctl,
#endif
    owner   : THIS_MODULE,
};

static struct cdev *cipher_cdev;
static dev_t cipher_devno = MKDEV( CIPHER_MAJOR, CIPHER_MINOR );

static unsigned int data_bufflen = 0;
static unsigned char *data_inbuff;
static unsigned char *data_outbuff;

static unsigned char key_buff[MAX_KEY_BUF];
static unsigned char iv_buff[MAX_IV_BUF];
static unsigned int g_total_data_len=0;
static unsigned int g_blocksize = 0;
int
register_cipher_hal_module (unsigned char num_instances, void *phal_ops, void **phw_data)
{
    struct cipher_hal* pcipher_hal;

    pcipher_hal = (struct cipher_hal*) kmalloc (sizeof(struct cipher_hal), GFP_KERNEL);
    if (!pcipher_hal)
    {
        return -ENOMEM;
    }

    pcipher_hal->pcipher_hal_ops = ( cipher_hal_operations_t *) phal_ops;
    *phw_data = (void *) pcipher_hal;
    //Dircectly inovke get_max_datalen to allocate the cipher data buffer
    data_bufflen = pcipher_hal->pcipher_hal_ops->cipher_get_max_datalen ();
    if (data_bufflen > MAX_DATA_BUF)
        data_bufflen = MAX_DATA_BUF;
        
    data_inbuff = (unsigned char *) kmalloc (data_bufflen, GFP_KERNEL);
    data_outbuff = (unsigned char *) kmalloc (data_bufflen, GFP_KERNEL);
    memset(key_buff, '\0', MAX_KEY_BUF);
    memset(iv_buff, '\0', MAX_IV_BUF);
    
    return 0;
}

int
unregister_cipher_hal_module (void *phw_data)
{

    struct cipher_hal *pcipher_hal = (struct cipher_hal*) phw_data;

    dbgprint ("unregister hal addr : %p\n", cipher_hal);
                                          
    kfree (pcipher_hal);
    
    kfree (data_inbuff);
    kfree (data_outbuff);
    
    return 0;
}


/*
 * Cipher driver exit function
 */
static void __exit cipher_exit( void )
{
    if ( NULL != cipher_cdev )
    {
        dbgprint( "Cipher char device deleted\n" );
        cdev_del ( cipher_cdev );
    }

    unregister_core_hal_module (EDEV_TYPE_CIPHER);

    /* Unregister the device */
    unregister_chrdev_region( cipher_devno,  CIPHER_MAX_DEVICES );


    printk( "The Cipher coprocessor Driver Unloaded sucessfully.\n" );
    return;
}

/*
 * Cipher driver file open
 */
static int cipher_open ( struct inode *inode, struct file *file )
{ 
    unsigned int minor = iminor(inode);
    struct cipher_hal *pcipher_hal;
    struct cipher_dev *pdev;
    hw_info_t cipher_hw_info;
    unsigned char open_count;
    int ret=0;
    
    ret = hw_open (EDEV_TYPE_CIPHER, minor,&open_count, &cipher_hw_info);
    if (ret)
        return -ENXIO;

    pcipher_hal = cipher_hw_info.pdrv_data;

    pdev = (struct cipher_dev*)kmalloc(sizeof(struct cipher_dev), GFP_KERNEL);
    
    if (!pdev)
    {
        hw_close (EDEV_TYPE_CIPHER, minor, &open_count);
        printk (KERN_ERR "%s: failed to allocate cipher private dev structure for cipher iminor: %d\n", CIPHER_DEV_NAME, minor);
        return -ENOMEM;
    }

    pdev->pcipher_hal = pcipher_hal;
    file->private_data = pdev;

    return nonseekable_open (inode, file);	
}


/*
 * Cipher driver file release
 */
static int cipher_release ( struct inode *inode, struct file *file )
{
    int ret;
    unsigned char open_count;
    struct cipher_dev *pdev = (struct cipher_dev*)file->private_data;
    
    pdev->pcipher_hal = NULL;
    file->private_data = NULL;
    
    ret = hw_close (EDEV_TYPE_CIPHER, iminor(inode), &open_count);
    if (ret)
    {
        return -1;
    }
    kfree (pdev);

    return 0;
}

static int cipher_data_padding_len_big_endian(unsigned char *data_array, int start_index, 
                                                unsigned int length_low, unsigned int length_high)
{
    data_array[start_index + 56] = (length_high >> 24) & 0xFF;
    data_array[start_index + 57] = (length_high >> 16) & 0xFF;
    data_array[start_index + 58] = (length_high >> 8) & 0xFF;
    data_array[start_index + 59] = (length_high) & 0xFF;
    data_array[start_index + 60] = (length_low >> 24) & 0xFF;
    data_array[start_index + 61] = (length_low >> 16) & 0xFF;
    data_array[start_index + 62] = (length_low >> 8) & 0xFF;
    data_array[start_index + 63] = (length_low) & 0xFF;
    
    return 0;    
}

static int cipher_data_padding_len_little_endian(unsigned char *data_array, int start_index,
                                                    unsigned int length_low, unsigned int length_high)
{
    data_array[start_index + 56] = (length_low) & 0xFF;
    data_array[start_index + 57] = (length_low >> 8) & 0xFF;
    data_array[start_index + 58] = (length_low >> 16) & 0xFF;
    data_array[start_index + 59] = (length_low >> 24)& 0xFF;
    data_array[start_index + 60] = (length_high) & 0xFF;  
    data_array[start_index + 61] = (length_high >> 8) & 0xFF;
    data_array[start_index + 62] = (length_high >> 16) & 0xFF;
    data_array[start_index + 63] = (length_high >> 24) & 0xFF;
    
    return 0; 
}

static int cipher_data_padding_len(int algo, unsigned char *data_array, int start_index,
                                    unsigned int length_low, unsigned int length_high)
{
    switch (algo)
    {
        case CIPHER_SHA1:
        case CIPHER_SHA224:
        case CIPHER_SHA256:
        case CIPHER_HMAC_SHA1:
        case CIPHER_HMAC_SHA224:
        case CIPHER_HMAC_SHA256:
            return cipher_data_padding_len_big_endian(data_array, start_index, length_low, length_high);
        break;
        
        case CIPHER_MD5:
        case CIPHER_HMAC_MD5:
            return cipher_data_padding_len_little_endian(data_array, start_index, length_low, length_high);
        break;
        
        default :
            return 0;    
    }
    
    return 0;
}

static int cipher_sha1_data_padding(struct cipher_dev *pdev, cipher_data_t  *cipher_data)
{
    unsigned int length = 0;
    unsigned int length_low = 0, length_high = 0;
    
    unsigned int blocknum = 0, need_data_padding_start = 0, need_data_padding_len = 0, extend_max_len = 0;
    unsigned int len_index = 0;

    unsigned char final_msg_block[SHA1_BLOCK_SIZE];
    /* hmac hash will put 512 bits key in front of inner data */
    if( (cipher_data->algo == CIPHER_HMAC_SHA1)   || 
        (cipher_data->algo == CIPHER_HMAC_SHA224) ||
        (cipher_data->algo == CIPHER_HMAC_SHA256) ||
        (cipher_data->algo == CIPHER_HMAC_MD5) )
        length_low = 0x200;

    memset(final_msg_block, 0, SHA1_BLOCK_SIZE);
    
    /* Calculate Length_Low and Length_High from total length first */
    length = g_total_data_len;
    while(length--) 
    {
        length_low += 8;
        length_low &= 0xFFFFFFFF;
        if (length_low == 0)
        {
            length_high++;
            /* Force it to 32 bits */
            length_high &= 0xFFFFFFFF;
            if (length_high == 0)
            {
                /* Message is too long */
                return -EFBIG;
            }
        }
    }

    /*
     *  Check to see if the current message block is too small to hold
     *  the initial padding bits and length.  If so, we will pad the
     *  block, process it, and then continue padding into a second
     *  block.
     */
    blocknum = cipher_data->data_len / SHA1_BLOCK_SIZE;
    need_data_padding_len = cipher_data->data_len % SHA1_BLOCK_SIZE;
    
    need_data_padding_start = blocknum * SHA1_BLOCK_SIZE; 
    extend_max_len = need_data_padding_start + SHA1_BLOCK_SIZE + SHA1_BLOCK_SIZE; 
    
    if ((need_data_padding_len > 55) && (extend_max_len > data_bufflen))
    {  
        /* 
         * Need a extra pad data, but total size will greater than max buffer size
         * process block that no need padding first
         */
        cipher_data->data_len = need_data_padding_start;     
        /* Store the rest of data that are not enough for a block */
        memcpy(final_msg_block, &cipher_data->data_in[need_data_padding_start], need_data_padding_len);
        
        /* 
         * Just process_cipher first, and there is one more block left, 
         * there is no need to copy the data_out into userspace right away.
         * And hardware driver needs to handle this kind of situration. 
         */
        cipher_data->progress = CIPHER_PROGRESS_COMPLETE; /* There is still an extra block */

        process_cipher(cipher_data->algo, pdev, cipher_data);

        cipher_data->progress = CIPHER_KS_PROGRESS_COMPLETE;
        /* After processing, copy final data back to the beginning position of cipher data */
        cipher_data->data_len = need_data_padding_len; 
        memcpy(cipher_data->data_in, final_msg_block, need_data_padding_len);
               
        need_data_padding_start = 0;
        /* need to assign extend_max_len again for new value */
        extend_max_len = need_data_padding_start + SHA1_BLOCK_SIZE + SHA1_BLOCK_SIZE; 
    }
    
    len_index = cipher_data->data_len; 
       
    if (need_data_padding_len > 55)
    {   
               
        cipher_data->data_in[len_index++] = 0x80;
        
        while(len_index < extend_max_len)
        {
            cipher_data->data_in[len_index++] = 0;
        }
        /* move need_data_padding_start to extended block start */
        need_data_padding_start += SHA1_BLOCK_SIZE; 
    }
    else 
    {
        /* Best situation, no need to add an extra padding block */
        cipher_data->data_in[len_index++] = 0x80;
        while(len_index < (need_data_padding_start + SHA1_BLOCK_SIZE))
        {
            cipher_data->data_in[len_index++] = 0;
        }
    }
    
    /*
     *  Store the message length as the last 8 octets
     */
    cipher_data_padding_len(cipher_data->algo, cipher_data->data_in, need_data_padding_start, length_low, length_high);   
    
    cipher_data->data_len = len_index;
       
    return 0;        
}

static int cipher_common_data_padding (struct cipher_dev *pdev, cipher_data_t  *cipher_data)
{
    int i = 0, j = 0;
    int shift_bits = 0;
    unsigned int msglen = 0;
               
    /* continue to do common method below */
    switch (cipher_data->algo)
    {
        case CIPHER_SHA1:
        case CIPHER_SHA224:
        case CIPHER_SHA256:
        case CIPHER_MD5:
        case CIPHER_HMAC_SHA1:
        case CIPHER_HMAC_SHA224:
        case CIPHER_HMAC_SHA256:
        case CIPHER_HMAC_MD5:
            return cipher_sha1_data_padding(pdev, cipher_data);
        break;
        
        case CIPHER_AES:
        case CIPHER_DES:
        case CIPHER_3DES:
            if((cipher_data->data_len % g_blocksize) == 0)
                break;    
            i=0;
            while(g_blocksize>>(++i))
                shift_bits++;  
                
            msglen = cipher_data->data_len;
            j = ((msglen + (g_blocksize-1)) >> shift_bits) << shift_bits;
            for (i = msglen; i < j; i++)
                cipher_data->data_in[i] = 0;
            cipher_data->data_len = j;
        break;
        
        default :
            return 0;    
    }
    
    return 0;
}

static int cipher_common_remove_data_padding (struct cipher_dev *pdev, cipher_data_t  *cipher_data)
{
    unsigned int tmplen = 0;

    /* continue to do common method below */
    switch (cipher_data->algo)
    {      
        case CIPHER_AES:
        case CIPHER_DES:
        case CIPHER_3DES:
            /* Remove padding bytes */
            tmplen = cipher_data->data_len;
            while(tmplen--)
            {
                if(cipher_data->data_out[tmplen] == '\0')
                    cipher_data->data_len = tmplen;
                else
                    break;    
            }
        break;
        
        default :
            return 0;    
    }
    
    return 0;
}

static int cipher_get_blocksize (int cipher_algo)
{
    switch (cipher_algo)
    {        
        case CIPHER_AES:
            return AES_BLOCK_SIZE;
            break;
            
        case CIPHER_DES:
        case CIPHER_3DES:
            return DES_BLOCK_SIZE;
            break;
        
        case CIPHER_SHA1:
        case CIPHER_SHA224:
        case CIPHER_SHA256:
        case CIPHER_MD5:
        case CIPHER_HMAC_SHA1:
        case CIPHER_HMAC_SHA224:
        case CIPHER_HMAC_SHA256:
        case CIPHER_HMAC_MD5:
            return SHA1_BLOCK_SIZE;
            break;
        
        default :
            return 0;
            break;   
    }
    
    return 0;
}
static int
process_cipher (uint cmd, struct cipher_dev *pdev, cipher_data_t  *cipher_data)
{
    int ret = 0;		
    
    switch ( cmd )
    {
        case CIPHER_GETINFO:
            ret = process_get_info (pdev, cipher_data);
            break;
        case CIPHER_AES:				
            ret = process_aes (pdev, cipher_data);
            break;
    
        case CIPHER_RC4:
            ret = process_rc4 (pdev, cipher_data);
            break;
            
        case CIPHER_DES:
            ret = process_des (pdev, cipher_data);
            break;
        
        case CIPHER_3DES:
            ret = process_3des (pdev, cipher_data);
            break;    
        
        case CIPHER_RSA:
            ret = process_rsa (pdev, cipher_data);
            break;

        case CIPHER_RNG:
            ret = process_rng (pdev, cipher_data);
            break;
        
        case CIPHER_SHA1:
            ret = process_sha1 (pdev, cipher_data);
            break;
        
        case CIPHER_SHA224:
            ret = process_sha224 (pdev, cipher_data);
            break;            
        
        case CIPHER_SHA256:
            ret = process_sha256 (pdev, cipher_data);
            break;
            
        case CIPHER_MD5:
            ret = process_md5 (pdev, cipher_data);
            break;            
            
        case CIPHER_HMAC_SHA1:
            ret = process_hmac_sha1 (pdev, cipher_data);
            break;
            
        case CIPHER_HMAC_SHA224:
            ret = process_hmac_sha224 (pdev, cipher_data);
            break;                   

        case CIPHER_HMAC_SHA256:
            ret = process_hmac_sha256 (pdev, cipher_data);
            break;
            
        case CIPHER_HMAC_MD5:
            ret = process_hmac_md5 (pdev, cipher_data);
            break;               
                
        default :
            printk( KERN_ERR "Invalid Cipher Function\n"  );
            return -EINVAL;
    }
    
    return ret;   
}


static int cipher_handle_data(uint cmd, struct cipher_dev *pdev, cipher_data_t  *cipher_data, cipher_data_t  *us_arg)
{
    void *us_data_in_addr;  /* user space data_in address */
    void *us_data_out_addr; /* user space data_ot address */
    unsigned int total_len = 0, len = 0, out_len = 0;
    unsigned int offset = 0, out_offset = 0;
    int ret = 0;
    /* 
     * Stages of cipher progress: 
     * CIPHER_PROGRESS_START - first block from user space
     * CIPHER_KS_PROGRESS_START - first block of both user space and kernal space
     * CIPHER_PROGRESS_IN - in the middle of transferring data
     * CIPHER_PROGRESS_COMPLETE - final block from user space
     * CIPHER_KS_PROGRESS_COMPLETE - final block of both user space and kernal space
     */
    if (cipher_data->progress == CIPHER_PROGRESS_START)
    {
        g_total_data_len = cipher_data->data_len;
        cipher_data->progress = CIPHER_KS_PROGRESS_START;
    }
    else if (cipher_data->progress == CIPHER_PROGRESS_IN || cipher_data->progress == CIPHER_PROGRESS_COMPLETE)
    {   
        g_total_data_len += cipher_data->data_len;
    }
        
    total_len = cipher_data->data_len;
    /* To decide the data block size to send into hardware driver */
#if 0
    if (cipher_data->data_len > 0)
    {
        g_blocksize = cipher_get_blocksize(cipher_data->algo);

        if(g_blocksize > 0) /* need padding */
        {
            if (g_blocksize > data_bufflen )
                return -EFAULT;
            
            /* send data bytes as much as possible into hardware layer */    
            /* len = (data_bufflen / g_blocksize) * g_blocksize; */
            
            /* Just send a algorithmic block according to current algorithm */   
            len = g_blocksize;
        }
        else
        {
            if(total_len > data_bufflen)
                len = data_bufflen;
            else
                len = total_len;
        }    
    }		
#endif

    if(cipher_data->data_len > 0)
    {
        g_blocksize = cipher_get_blocksize(cipher_data->algo);

        if(total_len > data_bufflen)
        {
            len = data_bufflen;
        }
        else
        {
            len = total_len;
        }
    }

    /* Record user space address */
    us_data_in_addr = cipher_data->data_in;
    us_data_out_addr = cipher_data->data_out;
   
    /* Block by block to process cipher data */
    offset = 0;
    out_offset = 0;
    
    while(total_len > 0)
    {                   
        memset (data_inbuff, '\0', data_bufflen);
               
        /* the last block */
        if(total_len <= len)
        {
            len = total_len;
            
            if(cipher_data->progress == CIPHER_PROGRESS_COMPLETE)
                cipher_data->progress = CIPHER_KS_PROGRESS_COMPLETE;
        }

        if (copy_from_user(data_inbuff, (us_data_in_addr + offset), len))
        {
            printk ("cipher_ioctl(): copy_from_user failed for cipher data in\n");
            return -EFAULT;
        }
        
        cipher_data->data_len = len;
        cipher_data->data_in = data_inbuff;

        memset (data_outbuff, '\0', data_bufflen); 
        cipher_data->data_out = data_outbuff;
        
        /* check if this request is to handle cipher data */
        if (cipher_data->progress == CIPHER_KS_PROGRESS_COMPLETE)
        {
            /* check data if it realy needs padding */
            if(g_blocksize > 0)
            {
                /* 
                 * Check if needs specific hardware padding first, 
                 * then invoke common padding function if no special hw padding fuction. 
                 */
                if(RET_HANDLED != process_data_padding(pdev, cipher_data))
                {
                    ret = cipher_common_data_padding(pdev, cipher_data);
                    if(ret != 0)    
                        return ret;
                }
            }
            
        }
        	
        process_cipher(cmd, pdev, cipher_data);
        
        if (cipher_data->progress == CIPHER_KS_PROGRESS_START)
            cipher_data->progress = CIPHER_PROGRESS_START;
        
        if (cipher_data->progress == CIPHER_KS_PROGRESS_COMPLETE)
        {
            /* check data if it realy needs to remove padding */
            if (g_blocksize > 0)
            {
                /* 
                 * Check if needs specific hardware remove padding method first, 
                 * then invoke common remove padding function if there is no special hw remove padding function. 
                 */
                if (RET_HANDLED != process_remove_data_padding(pdev, cipher_data))
                    cipher_common_remove_data_padding(pdev, cipher_data);
            } 
            
            g_total_data_len = 0;  /* clear value */            
        }
        
        out_len = cipher_data->data_len;
        if ((cipher_data->data_out != NULL) && (out_len != 0))
        {    
            if (copy_to_user((us_data_out_addr + out_offset), cipher_data->data_out, out_len))
                 return -EFAULT;
        }
        
        total_len -= len;
        offset += len;
        out_offset += out_len;
    }
    
    cipher_data->data_in = us_data_in_addr;
    cipher_data->data_out = us_data_out_addr;
    cipher_data->data_len = out_offset;
    
    /* Copy the new length to user space */
    if ( copy_to_user( &(us_arg->data_len), &(cipher_data->data_len), sizeof(cipher_data->data_len) ) )
         return -EFAULT;
    
    return 0;
}

/*
 * Cipher driver file ioctl
 */

static long cipher_ioctlUnlocked( struct file *file, uint cmd, ulong arg )
{
    cipher_data_t  cipher_data;
	cipher_data_t *us_arg = (cipher_data_t *)(arg);

    void *us_data_out_addr;

    struct cipher_dev *pdev = (struct cipher_dev*) file->private_data;    

    memset(&cipher_data, '\0', sizeof(cipher_data_t));
    if (0 != copy_from_user(&cipher_data, (void *) arg, sizeof(cipher_data_t)))
    {
        printk( KERN_ERR "Error copying the data from user to kernel\n"  );
        return -EFAULT;
    }

    if ((cipher_data.key_len != 0) && (cipher_data.key != NULL))   
	{
		if (copy_from_user(key_buff, cipher_data.key, cipher_data.key_len))
		{
			printk ("cipher_ioctl(): copy_from_user failed for cipher key\n");
			return -EFAULT;
		}
		cipher_data.key = key_buff;
	}

	else if ((cipher_data.iv_len != 0) && (cipher_data.iv != NULL))
	{
		if (copy_from_user(iv_buff, cipher_data.iv, cipher_data.iv_len))
		{
			printk ("cipher_ioctl(): copy_from_user failed for cipher iv\n");
			return -EFAULT;
		}
		cipher_data.iv = iv_buff;
	}

	else if ((cipher_data.data_len != 0) && (cipher_data.data_in != NULL))
	{
  	    return (cipher_handle_data(cmd, pdev, &cipher_data, us_arg));
	}
    
    /* Handle key, iv, and get_info */
    us_data_out_addr = cipher_data.data_out;
    memset (data_outbuff, '\0', data_bufflen); 
    cipher_data.data_out = data_outbuff;
    
    process_cipher(cmd, pdev, &cipher_data);

    /* get_info needs data_out buffer and length */
    if ((cipher_data.data_out != NULL) && (cipher_data.data_len != 0))
    {   
        if(copy_to_user(us_data_out_addr, cipher_data.data_out, cipher_data.data_len))
        {
			printk ("cipher_ioctl(): copy_to_user failed during cipher data copy\n");
			return -EFAULT;
        }
               
        /* Copy the new length to user space */
        if ( copy_to_user( &(us_arg->data_len), &(cipher_data.data_len), sizeof(cipher_data.data_len) ) )
        {
			printk ("cipher_ioctl(): copy_to_user failed during cipher data length copy\n");
			return -EFAULT;
        }
    }
    return 0;
}

#ifndef USE_UNLOCKED_IOCTL
static int cipher_ioctl( struct inode *inode, struct file *file, uint cmd, ulong arg )
{
	return (cipher_ioctlUnlocked( file, cmd, arg ));
}
#endif

static int 
process_get_info (struct cipher_dev *pdev, cipher_data_t  *cipher_data)
{
    return (pdev->pcipher_hal->pcipher_hal_ops->cipher_get_info (cipher_data));   
}

/* 
 *@fn process_data_padding 
 *@brief Check if data padding is handled by hardware-layer driver
 *@return RET_NONE - Need to pad data by common driver
 *        RET_HANDLED - Handled by hw driver 
 *        RET_FAILED - Failed to pad data by hw driver, this should never be happened
 */
static int 
process_data_padding (struct cipher_dev *pdev, cipher_data_t  *cipher_data)
{
    return (pdev->pcipher_hal->pcipher_hal_ops->cipher_process_data_padding (cipher_data));   
}

static int 
process_remove_data_padding (struct cipher_dev *pdev, cipher_data_t  *cipher_data)
{
    return (pdev->pcipher_hal->pcipher_hal_ops->cipher_process_remove_data_padding (cipher_data));   
}

static int 
process_aes (struct cipher_dev *pdev, cipher_data_t  *cipher_data)
{
	pdev->pcipher_hal->pcipher_hal_ops->cipher_process_aes (cipher_data); 
    return 0;   
}

static int 
process_rc4 (struct cipher_dev *pdev, cipher_data_t  *cipher_data)
{
    pdev->pcipher_hal->pcipher_hal_ops->cipher_process_rc4 (cipher_data);
    return 0;   
}

static int 
process_des (struct cipher_dev *pdev, cipher_data_t  *cipher_data)
{
    pdev->pcipher_hal->pcipher_hal_ops->cipher_process_des (cipher_data);	 
    return 0;   
}

static int 
process_3des (struct cipher_dev *pdev, cipher_data_t  *cipher_data)
{
    pdev->pcipher_hal->pcipher_hal_ops->cipher_process_3des (cipher_data);	 	 
    return 0;   
}

static int 
process_rsa (struct cipher_dev *pdev, cipher_data_t  *cipher_data)
{
    pdev->pcipher_hal->pcipher_hal_ops->cipher_process_rsa (cipher_data); 
    return 0;   
}

static int 
process_rng (struct cipher_dev *pdev, cipher_data_t  *cipher_data)
{
	pdev->pcipher_hal->pcipher_hal_ops->cipher_process_rng (cipher_data); 
    return 0;   
}

static int 
process_sha1 (struct cipher_dev *pdev, cipher_data_t  *cipher_data)
{
    pdev->pcipher_hal->pcipher_hal_ops->cipher_process_sha1 (cipher_data);
    return 0;   
}

static int 
process_sha224 (struct cipher_dev *pdev, cipher_data_t  *cipher_data)
{
    pdev->pcipher_hal->pcipher_hal_ops->cipher_process_sha224 (cipher_data);	 
    return 0;   
}

static int 
process_sha256 (struct cipher_dev *pdev, cipher_data_t  *cipher_data)
{
    pdev->pcipher_hal->pcipher_hal_ops->cipher_process_sha256 (cipher_data);	 
    return 0;   
}

static int 
process_md5 (struct cipher_dev *pdev, cipher_data_t  *cipher_data)
{
    pdev->pcipher_hal->pcipher_hal_ops->cipher_process_md5 (cipher_data);	 
    return 0;   
}

static int 
process_hmac_sha1 (struct cipher_dev *pdev, cipher_data_t  *cipher_data)
{
    pdev->pcipher_hal->pcipher_hal_ops->cipher_process_hmac_sha1 (cipher_data);
    return 0;   
}

static int 
process_hmac_sha224 (struct cipher_dev *pdev, cipher_data_t  *cipher_data)
{
    pdev->pcipher_hal->pcipher_hal_ops->cipher_process_hmac_sha224 (cipher_data);
    return 0;   
}

static int 
process_hmac_sha256 (struct cipher_dev *pdev, cipher_data_t  *cipher_data)
{
    pdev->pcipher_hal->pcipher_hal_ops->cipher_process_hmac_sha256 (cipher_data);
    return 0;   
}

static int 
process_hmac_md5 (struct cipher_dev *pdev, cipher_data_t  *cipher_data)
{
    pdev->pcipher_hal->pcipher_hal_ops->cipher_process_hmac_md5 (cipher_data);	 
    return 0;   
}



/* ----- Driver registration ---------------------------------------------- */

static core_hal_t cipher_core_hal =
{
    .owner             = THIS_MODULE,
    .name             = "CIPHER CORE",
    .dev_type              = EDEV_TYPE_CIPHER,
    .register_hal_module   = register_cipher_hal_module,
    .unregister_hal_module = unregister_cipher_hal_module,
    .pcore_funcs           = (void *)NULL
};


/*
 * Cipher driver init function
 */
static int __init cipher_init( void )
{
    int            ret = 0;

    cipher_cdev = NULL;

    /* Register the character device */
    if ( ( ret = register_chrdev_region( cipher_devno, CIPHER_MAX_DEVICES, CIPHER_DEV_NAME ) ) < 0 )
    {
        printk( KERN_ERR "Error: Cipher Driver : Failed to register cipher char device (err %d)\n", ret );
        return -1;
    }

    cipher_cdev = cdev_alloc();
    if (!cipher_cdev)
    {
        unregister_chrdev_region (cipher_devno, CIPHER_MAX_DEVICES);
        printk (KERN_ERR "%s: failed to allocate cipher cdev structure\n", CIPHER_DEV_NAME);
        return -1;
    }

    cdev_init (cipher_cdev, &cipher_ops);
    cipher_cdev->owner = THIS_MODULE;

    /* Add char device to the system */
    if ( ( ret = cdev_add( cipher_cdev, cipher_devno, CIPHER_MAX_DEVICES ) < 0 ) )
    {
        cdev_del (cipher_cdev);
        unregister_chrdev_region (cipher_devno, CIPHER_MAX_DEVICES);

        printk( KERN_ERR "Error: Cipher Driver : Failed to add cipher char device\n" );
        return -1;
    }

    if ((ret = register_core_hal_module (&cipher_core_hal)) < 0)
    {
        printk(KERN_ERR "failed to register the Core Cipher module\n");
        cdev_del (cipher_cdev);
        unregister_chrdev_region (cipher_devno, CIPHER_MAX_DEVICES);
        ret = -EINVAL;
        return ret;
    }

    printk( "The Cipher coprocessor driver loaded successfully\n" );

    return ret;
}


module_init ( cipher_init );
module_exit ( cipher_exit );

int cipher_core_loaded =1;
EXPORT_SYMBOL(cipher_core_loaded);
