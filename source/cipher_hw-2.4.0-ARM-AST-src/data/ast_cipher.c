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
/****************************************************************
 *
 * ast_cipher.c
 * ASPEED AST1100/2050/2100/2150/2300/2400 cipher hw driver
 *
 ****************************************************************/
#include <linux/version.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/errno.h>
#include <linux/ioport.h>
#include <linux/delay.h>
#include <linux/jiffies.h>
#include <linux/semaphore.h>
#include <linux/dma-mapping.h>

//#include <linux/wait.h>


#include <asm/irq.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/cacheflush.h>

/* ASPEED Sample */
#include "aes.h"
#include "rc4.h"
#include "rsa.h"

/* AMI */
#include "driver_hal.h"
#include "cipher.h"
#include "ast_cipher.h"

#ifdef DEBUG
#define dbgprint(fmt, args...)       printk (KERN_INFO fmt, ##args) 
#else
#define dbgprint(fmt, args...)       
#endif

#define AST_CIPHER_DRIVER_NAME		"cipher_hw"

#if defined(GROUP_AST2300) || defined(GROUP_AST2300_PLUS)
    #define SUPPORT_NUM       35
#else 
    #define SUPPORT_NUM       24
#endif
#define SUPPORT_FIXED_COL 3
#define ALGO_INDEX        0
#define MODE_INDEX        1
#define KEYLEN_INDEX      2

static DECLARE_WAIT_QUEUE_HEAD(crypto_event);
static DECLARE_WAIT_QUEUE_HEAD(hash_event);


unsigned char ast_cipher_support_tbl[] __attribute__((unused)) = 
{   /* algo */        /* mode */       /* key bit length */
    CIPHER_AES,       CIPHERMODE_ECB,  CIPHER_KEYBIT_128,
    CIPHER_AES,       CIPHERMODE_CBC,  CIPHER_KEYBIT_128,
    CIPHER_AES,       CIPHERMODE_CFB,  CIPHER_KEYBIT_128,
    CIPHER_AES,       CIPHERMODE_OFB,  CIPHER_KEYBIT_128,
    CIPHER_AES,       CIPHERMODE_CTR,  CIPHER_KEYBIT_128,
    
    CIPHER_AES,       CIPHERMODE_ECB,  CIPHER_KEYBIT_192,
    CIPHER_AES,       CIPHERMODE_CBC,  CIPHER_KEYBIT_192,
    CIPHER_AES,       CIPHERMODE_CFB,  CIPHER_KEYBIT_192,
    CIPHER_AES,       CIPHERMODE_OFB,  CIPHER_KEYBIT_192,
    CIPHER_AES,       CIPHERMODE_CTR,  CIPHER_KEYBIT_192,
    
    CIPHER_AES,       CIPHERMODE_ECB,  CIPHER_KEYBIT_256,
    CIPHER_AES,       CIPHERMODE_CBC,  CIPHER_KEYBIT_256,
    CIPHER_AES,       CIPHERMODE_CFB,  CIPHER_KEYBIT_256,
    CIPHER_AES,       CIPHERMODE_OFB,  CIPHER_KEYBIT_256,
    CIPHER_AES,       CIPHERMODE_CTR,  CIPHER_KEYBIT_256,

    CIPHER_RC4,       CIPHERMODE_NONE, CIPHER_KEYBIT_NONE,

#if defined(GROUP_AST2300) || defined(GROUP_AST2300_PLUS)
    CIPHER_DES,       CIPHERMODE_ECB,  CIPHER_KEYBIT_NONE,
    CIPHER_DES,       CIPHERMODE_CBC,  CIPHER_KEYBIT_NONE,
    CIPHER_DES,       CIPHERMODE_CFB,  CIPHER_KEYBIT_NONE,
    CIPHER_DES,       CIPHERMODE_OFB,  CIPHER_KEYBIT_NONE,
    CIPHER_DES,       CIPHERMODE_CTR,  CIPHER_KEYBIT_NONE,
        
    CIPHER_3DES,      CIPHERMODE_ECB,  CIPHER_KEYBIT_NONE,
    CIPHER_3DES,      CIPHERMODE_CBC,  CIPHER_KEYBIT_NONE,
    CIPHER_3DES,      CIPHERMODE_CFB,  CIPHER_KEYBIT_NONE,
    CIPHER_3DES,      CIPHERMODE_OFB,  CIPHER_KEYBIT_NONE,
    CIPHER_3DES,      CIPHERMODE_CTR,  CIPHER_KEYBIT_NONE,
    
    CIPHER_RSA,       CIPHERMODE_NONE,  CIPHER_KEYBIT_NONE,
#endif
        
    /* Hash */
    CIPHER_SHA1,      CIPHERMODE_NONE, CIPHER_KEYBIT_NONE,
    CIPHER_SHA224,    CIPHERMODE_NONE, CIPHER_KEYBIT_NONE,
    CIPHER_SHA256,    CIPHERMODE_NONE, CIPHER_KEYBIT_NONE,
    CIPHER_MD5,       CIPHERMODE_NONE, CIPHER_KEYBIT_NONE,
    
    /* HMAC + Hash */
    CIPHER_HMAC_SHA1,   CIPHERMODE_NONE, CIPHER_KEYBIT_NONE,
    CIPHER_HMAC_SHA224, CIPHERMODE_NONE, CIPHER_KEYBIT_NONE,
    CIPHER_HMAC_SHA256, CIPHERMODE_NONE, CIPHER_KEYBIT_NONE,
    CIPHER_HMAC_MD5,    CIPHERMODE_NONE, CIPHER_KEYBIT_NONE,
};
#if (LINUX_VERSION_CODE >=  KERNEL_VERSION(3,4,11))
DEFINE_SEMAPHORE (mutex_key);
#else
DECLARE_MUTEX (mutex_key);
#endif
static void *ast_cipher_virt_base;
#if defined(GROUP_AST2300) || defined(GROUP_AST2300_PLUS)
static void *ast_sram_virt_base;
#endif
#if defined(GROUP_AST2300)
static void *ast_sram2_virt_base;
#endif
static cipher_core_funcs_t *cipher_core_ops;
static int ast_cipher_hal_id;

static unsigned char *cipher_data_buf;
static unsigned int  cipher_data_buf_len;

static crypto_hw_info_t    crypto_hwinfo;
static hash_hw_info_t      hash_hwinfo;

static AES_KEY  key_ctx;
static int hash_wakeup = HASH_SLEEP;
static int crypto_wakeup = CRYPTO_SLEEP;

const static uint32_t CRYPTO_CMD = (CRYPTO_ENABLE_RW | CRYPTO_ENABLE_CONTEXT_LOAD | CRYPTO_ENABLE_CONTEXT_SAVE | CRYPTO_SYNC_MODE_ASYNC); //CRYPTO_ENABLE_CONTEXT_LOAD | CRYPTO_ENABLE_CONTEXT_SAVE |

inline uint32_t ast_cipher_read_reg(uint32_t reg)
{
    return ioread32((void * __iomem)ast_cipher_virt_base + reg);
}

inline void ast_cipher_write_reg(uint32_t data, uint32_t reg)
{
    iowrite32(data, (void * __iomem)ast_cipher_virt_base + reg);
}

#if defined(GROUP_AST2300) || defined(GROUP_AST2300_PLUS)
inline uint32_t ast_sram_read_reg(uint32_t reg)
{
    return ioread32((void * __iomem)ast_sram_virt_base + reg);
}

inline void ast_sram_write_reg(uint32_t data, uint32_t reg)
{
    iowrite32(data, (void * __iomem)ast_sram_virt_base + reg);
}
#endif

#if defined(GROUP_AST2300)
inline uint32_t ast_sram2_read_reg(uint32_t reg)
{
    return ioread32((void * __iomem)ast_sram2_virt_base + reg);
}

inline void ast_sram2_write_reg(uint32_t data, uint32_t reg)
{
    iowrite32(data, (void * __iomem)ast_sram2_virt_base + reg);
}
#endif

/******************************* HW cipher functions ********************************************/ 

static unsigned int ast_cipher_get_max_datalen(void)
{
    return (HW_MAX_DATA_LEN);
}

static int ast_cipher_get_info(cipher_data_t  *cipher_data)
{
	int i = 0;

    int len = (SUPPORT_NUM * SUPPORT_FIXED_COL);

    if((cipher_data->algo != CIPHER_GETINFO) && (cipher_data->algo != INVALID_ALGO))
    {

		if( (ast_cipher_support_tbl[i*SUPPORT_FIXED_COL + ALGO_INDEX] == cipher_data->algo) && 
            (ast_cipher_support_tbl[i*SUPPORT_FIXED_COL + MODE_INDEX] == cipher_data->mode) &&
            (ast_cipher_support_tbl[i*SUPPORT_FIXED_COL + KEYLEN_INDEX] == cipher_data->key_bitlen) )
		{
            return 1;
		}
        else
            return -1;
	}
    
    cipher_data->data_len = len;
    
	if(cipher_data->data_len > HW_MAX_DATA_LEN)
        return -1;

    for(i=0; i<len; i++)
    {
        cipher_data->data_out[i] = ast_cipher_support_tbl[i];
    }

    return 0;
}



static int ast_cipher_process_data_padding(cipher_data_t  *cipher_data)
{
    return RET_NONE; /* No dictated method is required for data padding */
}

static int ast_cipher_process_remove_data_padding(cipher_data_t  *cipher_data)
{
    return RET_NONE; /* No dictated method is required for removing data padding */
}

/*
 * @fn ast_cipher_enable_hmac
 * @brief Enable AST1100/2050/2100 Engines
 */
static void ast_cipher_enable_hmac(void)
{
    uint32_t data;
    uint32_t srcbase, dstbase, contextbase;
    uint32_t hash_srcbase, hash_dstbase, hash_contextbase;
    
     /* Crypto */	
    srcbase = ((uint32_t)crypto_hwinfo.crypto_src_dma_addr + 7) & 0xFFFFFFF8;
    dstbase = ((uint32_t)crypto_hwinfo.crypto_dst_dma_addr + 7) & 0xFFFFFFF8;
    contextbase = ((uint32_t)crypto_hwinfo.crypto_context_dma_addr + 7) & 0xFFFFFFF8;

    dbgprint("Crypto Data Source Base Address: %x \n", (u32) srcbase);
    dbgprint("Crypto Data Destination Base Address: %x \n", (u32) dstbase);
    dbgprint("Crypto Data Context Base Address: %x \n", (u32) contextbase);
    
    /* Hash */
    hash_srcbase = ((uint32_t)hash_hwinfo.hash_src_dma_addr + 7) & 0xFFFFFFF8;
    hash_dstbase = ((uint32_t)hash_hwinfo.hash_dst_dma_addr + 7) & 0xFFFFFFF8;
    hash_contextbase = ((uint32_t)hash_hwinfo.hash_dst_dma_addr + 7) & 0xFFFFFFF8;

    
    dbgprint("Hash Data Source Base Address: %x \n", (u32) hash_srcbase);
    dbgprint("Hash Data Dstination Base Address: %x \n", (u32) hash_dstbase);
    dbgprint("Hash Data Context Base Address: %x \n", (u32) hash_contextbase);
    
    /* Init SCU */
     
    /* unlock SCU */
    iowrite32(AST_SCU_UNLOCK_MAGIC, (void * __iomem)SCU_KEY_CONTROL_REG); 

    /* Reset HAC Engine */
    data = ioread32((void * __iomem)SCU_SYS_RESET_REG);
    data |= 0x00000010;   /* BIT4=1, Reset HAC engine */      
    iowrite32(data, (void * __iomem)SCU_SYS_RESET_REG);
    
    udelay(100);
        
    /* Stop YCLK(For HAC) */ 
    data = ioread32((void * __iomem)SCU_CLK_STOP_REG);	     
#if defined(GROUP_AST2300_PLUS)
    data &= 0xfeffdfff;   /* BIT13=0, enable clock running, BIT24=0, enable RSACLK (For RSA Engine) Clock*/
#else
    data &= 0xffffdfff;   /* BIT13=0, enable clock running */    
#endif
    iowrite32(data, (void * __iomem)SCU_CLK_STOP_REG);

    /* Reset HAC Engine */
    data = ioread32((void * __iomem)SCU_SYS_RESET_REG);
    data &= 0xffffffef;   /* BIT4=0, No operation */      
    iowrite32(data, (void * __iomem)SCU_SYS_RESET_REG);
    
    iowrite32(0, (void * __iomem)SCU_KEY_CONTROL_REG); /* lock SCU */
    
    /* Init Crypto Register */
    ast_cipher_write_reg(srcbase, REG_CRYPTO_SRC_BASE_OFFSET);
    ast_cipher_write_reg(dstbase, REG_CRYPTO_DST_BASE_OFFSET);
    ast_cipher_write_reg(contextbase, REG_CRYPTO_CONTEXT_BASE_OFFSET);
    
    /* Hash */
    //ast_cipher_write_reg(0xA8, 0x0100);
    ast_cipher_write_reg(hash_srcbase, REG_HASH_SRC_BASE_OFFSET);
    ast_cipher_write_reg(hash_dstbase, REG_HASH_DST_BASE_OFFSET);
    ast_cipher_write_reg(hash_contextbase, REG_HASH_KEY_BASE_OFFSET);
}


static int ast_crypto(cipher_data_t  *cipher_data, uint32_t command)
{
    int i;
    uint32_t srcbase, dstbase;

    srcbase = ((uint32_t) crypto_hwinfo.src_virt_base + 7) & 0xFFFFFFF8;
    dstbase = ((uint32_t)crypto_hwinfo.dst_virt_base + 7) & 0xFFFFFFF8;

    /* Init HW */
    ast_cipher_write_reg(cipher_data->data_len, REG_CRYPTO_LEN_OFFSET);

    /* Set source */
    down(&mutex_key); 	
    for (i = 0; i < cipher_data->data_len; i++)
    	*(unsigned char *) (srcbase + i) = cipher_data->data_in[i];   	
    up(&mutex_key);

    /* Send cmd */
    ast_cipher_write_reg(CRYPTO_INT_ENABLE,REG_CRYPTO_STATUS_OFFSET);
    ast_cipher_write_reg(command, REG_CRYPTO_CMD_BASE_OFFSET);
    wait_event_interruptible(crypto_event,(crypto_wakeup == CRYPTO_WAKEUP));
    crypto_wakeup = CRYPTO_SLEEP;

    /* Output */
    down(&mutex_key);
    for (i = 0; i < cipher_data->data_len; i++)
    	cipher_data->data_out[i] = *(unsigned char *) (dstbase + i);      
    up(&mutex_key);

    return 0;
}


/*
 * AES functions
 */
void ast_aes_set_expansion_key(AES_KEY *key_ctx)
{    
    uint32_t i;
    uint32_t contextbase;

    contextbase = ((uint32_t) crypto_hwinfo.context_virt_base + 7) & 0xFFFFFFF8;
    
    down(&mutex_key);
    /* Set Expansion Key */   
    for (i = 0; i < (4 * (key_ctx->rounds+1)); i++)
    {
        *(uint32_t *) (contextbase + i*4 + 16) = 
            ((key_ctx->rd_key[i] & 0xFF) << 24) + 
            ((key_ctx->rd_key[i] & 0xFF00) << 8) + 
            ((key_ctx->rd_key[i] & 0xFF0000) >> 8) + 
            ((key_ctx->rd_key[i] & 0xFF000000) >> 24);
    }
    up(&mutex_key);	
}

void ast_aes_set_key(AES_KEY *key_ctx, aes_key_info_t *aes_key_info)
{      
    memset(key_ctx, 0, sizeof(AES_KEY));
    
    AES_set_encrypt_key(aes_key_info->aes_key, aes_key_info->key_bitlen, key_ctx);

    ast_aes_set_expansion_key(key_ctx); 
}

void ast_aes_iv(aes_iv_info_t *aes_iv_info)
{
    uint32_t i;
    uint32_t contextbase;

    contextbase = ((uint32_t) crypto_hwinfo.context_virt_base + 7) & 0xFFFFFFF8;
    
    down(&mutex_key);
    for (i = 0; i < 16; i++)
	 	*(unsigned char *) (contextbase + i) = aes_iv_info->aes_iv[i];	
    up(&mutex_key);	
}

static int ast_aes_crypto(cipher_data_t  *cipher_data, uint32_t command)
{
    switch (key_ctx.rounds)
    {
    case 10:
        command |= CRYPTO_AES128;
        break;
    case 12:
	    command |= CRYPTO_AES192;
        break;    
    case 14:
        command |= CRYPTO_AES256;
        break;    
    }

    switch (cipher_data->mode)
    {
    case CIPHERMODE_ECB:
        command |= CRYPTO_AES_ECB;
        break;
    case CIPHERMODE_CBC:
        command |= CRYPTO_AES_CBC;
        break;
    case CIPHERMODE_CFB:
        command |= CRYPTO_AES_CFB;
        break;
    case CIPHERMODE_OFB:
        command |= CRYPTO_AES_OFB;
        break;
    case CIPHERMODE_CTR:
        command |= CRYPTO_AES_CTR;
        break;                
    }

    return ast_crypto(cipher_data, command);
} 

static int ast_aes_enc(cipher_data_t  *cipher_data)
{
    uint32_t command = 0;
    command = (CRYPTO_CMD | CRYPTO_AES | CRYPTO_INT_ENABLE | CRYPTO_ENCRYPTO);
    
    return ast_aes_crypto(cipher_data, command);
}

static int ast_aes_dec(cipher_data_t  *cipher_data)
{
    uint32_t command = 0;

    command = (CRYPTO_CMD | CRYPTO_AES | CRYPTO_INT_ENABLE | CRYPTO_DECRYPTO);
    
    return ast_aes_crypto(cipher_data, command);
}


/*
 * RC4 functions
 */
static void ast_rc4_set_key(rc4_key_info_t *rc4_key_info)
{
    struct rc4_key_st s;
    uint32_t i;
	uint32_t contextbase;

    contextbase = ((uint32_t) crypto_hwinfo.context_virt_base + 7) & 0xFFFFFFF8;
	
    rc4_set_key(&s, rc4_key_info->rc4_key, rc4_key_info->key_bitlen);
    
    down(&mutex_key);
    /* Set Context */
    /* Set i, j */
    *(uint32_t *) (contextbase + 8) = 0x0001;
       
    /* Set Expansion Key */   
    for (i = 0; i < 64; i++) /* 64 is (MAX_RC4_KEY_LEN / 4) */
    {
        *(uint32_t *) (contextbase + i*4 + 16) = 
            (s.state[i * 4] & 0xFF) + ((s.state[i * 4 + 1] & 0xFF) << 8) + 
                ((s.state[i * 4 + 2] & 0xFF) << 16) + ((s.state[i * 4+ 3] & 0xFF) << 24);
    }
    up(&mutex_key);
}

	
static int ast_rc4_crypto(cipher_data_t  *cipher_data)
{   
    uint32_t command = (CRYPTO_RC4 | CRYPTO_CMD | CRYPTO_INT_ENABLE);
    
    return ast_crypto(cipher_data, command);  
} 


/*
 * Fill cipher info functions
 */
static void ast_aes_fill_key_info (aes_key_info_t *aes_key_info, cipher_data_t *cipher_data)
{
    memset(aes_key_info, 0, sizeof(aes_key_info_t));
	aes_key_info->key_bitlen = cipher_data->key_bitlen;
                           
	memcpy(aes_key_info->aes_key, cipher_data->key, cipher_data->key_len);
}

static void ast_aes_fill_iv_info (aes_iv_info_t *aes_iv_info, cipher_data_t *cipher_data)
{                          
	memset(aes_iv_info, 0, sizeof(aes_iv_info_t)); 
	memcpy(aes_iv_info->aes_iv, cipher_data->iv, sizeof(aes_iv_info->aes_iv));
}

static void ast_rc4_fill_key_info (rc4_key_info_t *rc4_key_info, cipher_data_t *cipher_data)
{
	rc4_key_info->key_bitlen = cipher_data->key_len;
	memcpy(rc4_key_info->rc4_key, cipher_data->key, cipher_data->key_len);
}


/*
 * cipher process functions
 */
static int reset_data_buf(unsigned char *buf, unsigned int *buf_len)
{
    memset(buf, 0, sizeof(buf));
    *buf_len = 0;
    
    return 0;
}

static int copy_to_cipher_data_buf(unsigned char *cipher_data_buf, cipher_data_t  *cipher_data)
{
    if (cipher_data->progress == CIPHER_KS_PROGRESS_START)
        reset_data_buf(cipher_data_buf, &cipher_data_buf_len);
    
    memcpy(&cipher_data_buf[cipher_data_buf_len], cipher_data->data_in, cipher_data->data_len);    
    cipher_data_buf_len += cipher_data->data_len;
    
    if ((cipher_data->progress == CIPHER_KS_PROGRESS_COMPLETE) ||
        ((cipher_data_buf_len + cipher_data->data_len) > HW_MAX_DATA_LEN))
        return RET_PROCESS_CIPHER_DATA;
    else
        cipher_data->data_len = 0;
    
    return RET_SKIP_PROCESS_CIPHER_DATA;    
}

int ast_cipher_process_aes(cipher_data_t  *cipher_data)
{
    int ret = 0;  
	void *temp_addr;
    aes_key_info_t      aes_key_info;
    aes_iv_info_t       aes_iv_info;
	
    switch (cipher_data->method) 
    {
        case AES_SET_KEY: /* fall-through */ 
        case AES_SET_DEC_KEY:
            ast_aes_fill_key_info(&aes_key_info, cipher_data);
            ast_aes_set_key(&key_ctx, &aes_key_info);
        break;

        case AES_SET_IV:
            ast_aes_fill_iv_info(&aes_iv_info, cipher_data);
            ast_aes_iv(&aes_iv_info);
        break;
        
        case AES_ENCRYPTO:  
            if (RET_PROCESS_CIPHER_DATA == copy_to_cipher_data_buf(cipher_data_buf, cipher_data))
            {
                temp_addr = cipher_data->data_in;
                cipher_data->data_in = cipher_data_buf;
                cipher_data->data_len = cipher_data_buf_len;

                ast_aes_enc(cipher_data);

                cipher_data->data_in = temp_addr;    
                reset_data_buf(cipher_data_buf, &cipher_data_buf_len);
            }
        break;
        
        case AES_DECRYPTO:
            if (RET_PROCESS_CIPHER_DATA == copy_to_cipher_data_buf(cipher_data_buf, cipher_data))
            {
                temp_addr = cipher_data->data_in;
                cipher_data->data_in = cipher_data_buf;
                cipher_data->data_len = cipher_data_buf_len;

                ast_aes_dec(cipher_data);

                cipher_data->data_in = temp_addr;
                reset_data_buf(cipher_data_buf, &cipher_data_buf_len);
            }
        break;
             
        default:
            ret = -1;
        break;
    }
                 
	return ret;
}

int ast_cipher_process_rc4(cipher_data_t  *cipher_data)
{
    int ret = 0;
    rc4_key_info_t      rc4_key_info;    
	     
    switch (cipher_data->method) 
    {       
        case RC4_SET_KEY:
            ast_rc4_fill_key_info(&rc4_key_info, cipher_data);  
            ast_rc4_set_key(&rc4_key_info);        
        break;
        
        case RC4_CRYPTO:
            ast_rc4_crypto(cipher_data);
        break;
            
        default:
            ret = -1;
        break;
    }
                    
	return ret;
}

#if defined(GROUP_AST2300) || defined(GROUP_AST2300_PLUS)

void ast_des_set_key(cipher_data_t  *cipher_data)
{    
    uint32_t i;
    uint32_t contextbase;
    int max_key_len = 0;
    
    contextbase = ((uint32_t) crypto_hwinfo.context_virt_base + 7) & 0xFFFFFFF8;
    
    if(cipher_data->algo == CIPHER_DES) 
        max_key_len = 8;
    else if(cipher_data->algo == CIPHER_3DES) 
        max_key_len = 24;
    
    /*Clear key*/
    down(&mutex_key);
    for (i = 0; i < max_key_len; i++)
    {
        *(uint32_t *) (contextbase + i + 16) = 00;
    }
    up(&mutex_key);
    
    /*fill key*/
    max_key_len = cipher_data->key_len;
    
    down(&mutex_key);
    for (i = 0; i < max_key_len; i++)
    {
        *(uint32_t *) (contextbase + i + 16) = cipher_data->key[i];
    }
    up(&mutex_key);	
}

void ast_des_iv(cipher_data_t  *cipher_data)
{
    uint32_t i;
    uint32_t contextbase;
    int max_iv_len = 8;
    contextbase = ((uint32_t) crypto_hwinfo.context_virt_base + 7) & 0xFFFFFFF8;
    
    max_iv_len = cipher_data->iv_len;
    
    down(&mutex_key);
    for (i = 0; i < max_iv_len; i++)
    {
	 	*(unsigned char *) (contextbase + i + 8) = cipher_data->iv[i];	
    }
    up(&mutex_key);	
}

static int ast_des_crypto(cipher_data_t  *cipher_data, uint32_t command)
{
    switch (cipher_data->mode)
    {
        case CIPHERMODE_ECB:
            command |= CRYPTO_DES_ECB;
            break;
        case CIPHERMODE_CBC:
            command |= CRYPTO_DES_CBC;
            break;
        case CIPHERMODE_CFB:
            command |= CRYPTO_DES_CFB;
            break;
        case CIPHERMODE_OFB:
            command |= CRYPTO_DES_OFB;
            break;
        case CIPHERMODE_CTR:
            command |= CRYPTO_DES_CTR;
            break;                
    }

    return ast_crypto(cipher_data, command);
} 

/*
 *  Single DES
 */ 
static int ast_des_enc(cipher_data_t  *cipher_data)
{
    uint32_t command = (CRYPTO_CMD | CRYPTO_AES | CRYPTO_DES1 | CRYPTO_INT_ENABLE | CRYPTO_ENCRYPTO);
    return ast_des_crypto(cipher_data, command);
}

static int ast_des_dec(cipher_data_t  *cipher_data)
{
    uint32_t command = (CRYPTO_CMD | CRYPTO_AES | CRYPTO_DES1 | CRYPTO_INT_ENABLE | CRYPTO_DECRYPTO);
    return ast_des_crypto(cipher_data, command);
}
  
int ast_cipher_process_des(cipher_data_t  *cipher_data)
{
    int ret = 0;  

	
    switch (cipher_data->method) 
    {
        case DES_SET_KEY:
            ast_des_set_key(cipher_data);
        break;
        
        case DES_SET_IV:
            ast_des_iv(cipher_data);
        break;
               
        case DES_ENCRYPTO:
            ast_des_enc(cipher_data);
        break;
        
        case DES_DECRYPTO:
            ast_des_dec(cipher_data);
        break;
             
        default:
            ret = -1;
        break;
    }
                 
	return ret;
}

/*
 *  Triple DES
 */ 
static int ast_des3_enc(cipher_data_t  *cipher_data)
{
    uint32_t command = (CRYPTO_CMD | CRYPTO_AES | CRYPTO_DES3 | CRYPTO_INT_ENABLE | CRYPTO_ENCRYPTO);

    return ast_des_crypto(cipher_data, command);
}

static int ast_des3_dec(cipher_data_t  *cipher_data)
{
    uint32_t command = (CRYPTO_CMD | CRYPTO_AES | CRYPTO_DES3 | CRYPTO_INT_ENABLE | CRYPTO_DECRYPTO);
    
    return ast_des_crypto(cipher_data, command);
}

int ast_cipher_process_des3(cipher_data_t  *cipher_data)
{
    int ret = 0;  
	    
    switch (cipher_data->method) 
    {
        case DES3_SET_KEY:
            ast_des_set_key(cipher_data);
        break;
        
        case DES3_SET_IV:
            ast_des_iv(cipher_data);
        break;
        
        case DES3_ENCRYPTO:
            ast_des3_enc(cipher_data);
        break;
        
        case DES3_DECRYPTO:
            ast_des3_dec(cipher_data);
        break;
             
        default:
            ret = -1;
        break;
    }
                 
	return ret;
}

/*RSA*/
int ast_cipher_process_rsa(cipher_data_t  *cipher_data)
{
	int ret = 0;
	rsa_mp_info_t      *rsa_mp_info;
	uint32_t i;
	int len;
	volatile uint32_t temp;
	unsigned char tempDWORD[4];
	unsigned int timeout_count = 0;
#if !defined(GROUP_AST2300_PLUS)
	unsigned int ulCommand = 0;
	unsigned int NonZeroExpCount = 0;
	unsigned char *p1 = NULL;
#endif
	rsa_mp_info = NULL;
	

    switch (cipher_data->method) 
    {       
        case RSA_CRYPTO:
        	  if (cipher_data->iv_len < MIN_RSA_MOD_LEN || cipher_data->iv_len > MAX_RSA_KEY_LEN)
        	  {
        	      printk("RSA Modulus length %d are must from byte %d~%d\n", cipher_data->iv_len, MIN_RSA_MOD_LEN, MAX_RSA_KEY_LEN);
        	      ret = -1;
        	      break;
        	  }
        	  if (cipher_data->key_len < MIN_RSA_EXP_LEN || cipher_data->key_len > MAX_RSA_KEY_LEN)
        	  {
        	      printk("RSA Exponent length %d are must from byte %d~%d\n", cipher_data->key_len, MIN_RSA_EXP_LEN, MAX_RSA_KEY_LEN);
        	      ret = -1;
        	      break;
        	  }
					
				    //Clear rsa sdram
				    len = AST_RSA_SRAM_REG_SIZE / sizeof(uint32_t);
				    for (i=0; i< len; i++)
				    {
				    	  ast_sram_write_reg(0x00000000, i*4);
				    }
#if defined(GROUP_AST2300)
				    len = AST_RSA_SRAM2_REG_SIZE / sizeof(uint32_t);
				    for (i=0; i< len; i++)
				    {
				    	  ast_sram2_write_reg(0x00000000, i*4);
				    }
#endif
					
				    //Fill message at 0x1e727200
				    len = (cipher_data->data_len/sizeof(uint32_t)) + ((cipher_data->data_len%sizeof(uint32_t) == 0)?0:1);
				    for (i=0; i<len; i++)
				    {
				    	  if (i==len-1)
                {
                	  memset(tempDWORD, 0, sizeof(uint32_t));
                	  memcpy(tempDWORD, &cipher_data->data_in[i*4], cipher_data->data_len - (i*4));
                	  temp = (tempDWORD[0] << 24) + (tempDWORD[1] << 16) + (tempDWORD[2] << 8) + tempDWORD[3];
                }
                else
				    	      temp = (cipher_data->data_in[i*4] << 24) + (cipher_data->data_in[i*4 + 1] << 16) + (cipher_data->data_in[i*4 + 2] << 8) + cipher_data->data_in[i*4 + 3];
				    	  
				    	  ast_sram_write_reg(temp, AST_RSA_MSG_OFFSET + ((len-i-1)*4));
				    }
					
				    //Fill modular at 0x1e727800
				    len = (cipher_data->iv_len/sizeof(uint32_t)) + ((cipher_data->iv_len%sizeof(uint32_t) == 0)?0:1);
				    for (i=0; i< len; i++)
				    {
				    	  if (i==len-1)
                {
                	  memset(tempDWORD, 0, sizeof(uint32_t));
                	  memcpy(tempDWORD, &cipher_data->iv[i*4], cipher_data->iv_len - (i*4));
                	  temp = tempDWORD[0] + (tempDWORD[1] << 8) + (tempDWORD[2] << 16) + (tempDWORD[3] << 24);
                }
                else
				    	      temp = *((uint32_t *) &cipher_data->iv[i*4]);
				    	  ast_sram_write_reg(temp, AST_RSA_MOD_OFFSET + i*4);
				    }
				    
#if defined(GROUP_AST2300_PLUS)
            rsa_mp_info = (rsa_mp_info_t *) kmalloc(sizeof(rsa_mp_info_t), GFP_KERNEL); 
            memset(rsa_mp_info, 0, sizeof(rsa_mp_info_t));
					  
					  //Find n'(Mp) and mod bitnum and exp bitnum
				    if (FillMp(rsa_mp_info, cipher_data))
				    {
				    	  kfree(rsa_mp_info);
				        return -1;
				    }
				    
				    //Fill exp at 0x1e727000
				    len = (cipher_data->key_len/sizeof(uint32_t)) + ((cipher_data->key_len%sizeof(uint32_t) == 0)?0:1);
				    for (i=0; i< len; i++)
				    {
				    	  if (i==len-1)
                {
                	  memset(tempDWORD, 0, sizeof(uint32_t));
                	  memcpy(tempDWORD, &cipher_data->key[i*4], cipher_data->key_len - (i*4));
                	  temp = tempDWORD[0] + (tempDWORD[1] << 8) + (tempDWORD[2] << 16) + (tempDWORD[3] << 24);
                }
                else
				    	      temp = *((uint32_t *) &cipher_data->key[i*4]);
				    	  ast_sram_write_reg(temp, AST_RSA_EXP_OFFSET + i*4);
				    }
				     
				    //Fill n'(Mp) at 0x1e727400
				    len = (rsa_mp_info->mp_len/sizeof(uint32_t)) + ((rsa_mp_info->mp_len%sizeof(uint32_t) == 0)?0:1);
				    for (i=0; i< len; i++)
				    {
				    	if (i==len-1)
				    	{
				    		memset(tempDWORD, 0, sizeof(uint32_t));
				    		memcpy(tempDWORD, &rsa_mp_info->rsa_mp[i*4], rsa_mp_info->mp_len - (i*4));
				    		temp = tempDWORD[0] + (tempDWORD[1] << 8) + (tempDWORD[2] << 16) + (tempDWORD[3] << 24);
				    	}
				    	else
				    		temp = *((uint32_t *) &rsa_mp_info->rsa_mp[i*4]);
				    	  ast_sram_write_reg(temp, AST_RSA_NP_OFFSET + i*4);
				    }
				    
				    //Fill mod_exp_bitnum at 0x1e6e3040
				    temp = rsa_mp_info->mod_exp_bitnum;
				    ast_cipher_write_reg(temp, REG_RSA_MODEXP_BITNUM_OFFSET);
            kfree(rsa_mp_info);
            
				    /* Send cmd */
				    ast_cipher_write_reg(RSA_ENABLE_SRAMACCESS | RSA_FIRE_ENGINE, REG_RSA_CTRL_CMD_OFFSET);
				    msleep(1);
            
            
				    /* busy waiting for status register */	   
				    timeout_count = 0;
				    barrier();
				    do {
					    temp = ast_cipher_read_reg(REG_CRYPTO_STATUS_OFFSET);
						  if ( ((temp & RSA_INT_ENABLE) == RSA_INT_ENABLE) &&  ((temp & RSA_BUSY) == 0) )
							    break;
				      timeout_count++;
				      msleep(1);  //RSA needs more time to process when data size is large
					  } while (timeout_count < AST_CIPHER_READ_TIMEOUT);  
				    barrier();
				    if (timeout_count == AST_CIPHER_READ_TIMEOUT)
				    {
				    	  printk("RSA engine is busy\n");
				    	  /* clear interrupt */
				    	  ast_cipher_write_reg(temp, REG_CRYPTO_STATUS_OFFSET);
				    	  /* disable SRAM */
				    	  ast_cipher_write_reg(0x0, REG_RSA_CTRL_CMD_OFFSET);
				        return -EFAULT;
				    }
				    /* clear interrupt */
				    ast_cipher_write_reg(temp, REG_CRYPTO_STATUS_OFFSET);
				    
#else //SOC_AST2300
				    /* init command */
				    len = (cipher_data->iv_len/sizeof(uint32_t)) + ((cipher_data->iv_len%sizeof(uint32_t) == 0)?0:1); 
				    if (len == 0) len += 1;
				    ulCommand = RSA_ENABLE_SRAMACCESS | RSA_FIRE_ENGINE | RSA_MODE_EXP | len;
            
				    /* init exp */
				    len = (cipher_data->key_len/sizeof(uint32_t)) + ((cipher_data->key_len%sizeof(uint32_t) == 0)?0:1);  
				    p1  = &cipher_data->key[(len-1) * 4];
            
				    /* start mod */
				    for (i=0; i<len; i++)
				    {
				    	  /* write handled exp */
				    	  if (i==0)
                {
                    memset(tempDWORD, 0, sizeof(uint32_t));
                    memcpy(tempDWORD, p1, cipher_data->key_len - ((len-1) * 4));
                    temp = tempDWORD[0] + (tempDWORD[1] << 8) + (tempDWORD[2] << 16) + (tempDWORD[3] << 24);
                }
				        else
				            temp = *(uint32_t *)(p1);
				        ast_cipher_write_reg(temp, REG_RSA_EXP_OFFSET);
				        p1 -= 4;
				        
				        if (temp) NonZeroExpCount++;
				        
				        if (NonZeroExpCount)
				        {
				            if ( (NonZeroExpCount !=1) || (temp != 1) )
				            {	
				                /* Send cmd */
				                ast_cipher_write_reg(ulCommand, REG_RSA_CTRL_CMD_OFFSET);
				                
										    /* busy waiting for status register */	   
										    timeout_count = 0;
										    barrier();
										    do {
											    temp = ast_cipher_read_reg(REG_CRYPTO_STATUS_OFFSET);
												  if ( ((temp & RSA_INT_ENABLE) == RSA_INT_ENABLE) && ((temp & RSA_BUSY) == 0) )
													    break;
										      timeout_count++;
										      msleep(1);  //RSA needs more time to process when data size is large
											  } while (timeout_count < AST_CIPHER_READ_TIMEOUT);  
										    barrier();
										    if (timeout_count == AST_CIPHER_READ_TIMEOUT)
										    {
										    	  printk("RSA engine is busy\n");
										    	  /* clear interrupt */
										    	  ast_cipher_write_reg(temp, REG_CRYPTO_STATUS_OFFSET);
										    	  /* disable SRAM */
										    	  ast_cipher_write_reg(0x0, REG_RSA_CTRL_CMD_OFFSET);
										        return -EFAULT;
										    }
										    /* clear interrupt */
										    ast_cipher_write_reg(temp, REG_CRYPTO_STATUS_OFFSET);
				            }
				            ulCommand |= RSA_MODE_EXP3;		//use mode3 for successive exp
				        }    
				    }
#endif //SOC_AST2300
            
		    /* disable SRAM */
		    ast_cipher_write_reg(0x0, REG_RSA_CTRL_CMD_OFFSET);
		    
		    
        flush_cache_all(); 
		    
		    //Read Output from 0x1e727200, put back to cipher_data->data_out
		    //temp = (ast_cipher_read_reg(REG_RSA_MODEXP_BITNUM_OFFSET) >> 16) & 0xFFF;
		    //len = (temp / 32) + (((temp%32) == 0)?0:1);
		    //cipher_data->data_len = len * 4;
		    len = (cipher_data->iv_len / 4) + (((cipher_data->iv_len%4) == 0)?0:1);
		    cipher_data->data_len = len * 4;  //cipher_data->data_len = (modulus bit length/32)*4 = modulus bit length/8
		    for (i = 0; i < len; i++)
		    {
#if defined(GROUP_AST2300_PLUS)
		    	temp=ast_sram_read_reg(AST_RSA_OUT_OFFSET + i*4);
#else
		    	temp=ast_sram2_read_reg(i*4);
#endif
		    	cipher_data->data_out[(len-i-1)*4] = (temp >> 24) & 0xFF;
		    	cipher_data->data_out[(len-i-1)*4 + 1] = (temp >> 16) & 0xFF;
		    	cipher_data->data_out[(len-i-1)*4 + 2] = (temp >> 8) & 0xFF;
		    	cipher_data->data_out[(len-i-1)*4 + 3] = temp & 0xFF;
		    }
        break;
        
        default:
            ret = -1;
        break;
    }
                    
	return ret;
}
#endif

/*
 * Hash functions
 */
 
/* Hash initial functions */
static void ast_sha1_init(void)
{
    int i;
    unsigned char msg_digest[MAX_SHA1_DIGEST] = { 0x67, 0x45, 0x23, 0x01, 
                                                  0xef, 0xcd, 0xab, 0x89, 
                                                  0x98, 0xba, 0xdc, 0xfe, 
                                                  0x10, 0x32, 0x54, 0x76, 
                                                  0xc3, 0xd2, 0xe1, 0xf0 };
    uint32_t ctxbase;
    ctxbase = ((uint32_t) hash_hwinfo.context_virt_base + 7) & 0xFFFFFFF8;                                         
                                       
     for (i = 0; i < MAX_SHA1_DIGEST; i++)
        *(unsigned char *) (ctxbase + i) = msg_digest[i];   
  
}

static void ast_sha224_init(void)
{
    int i;
    unsigned char msg_digest[MAX_SHA224_DIGEST] = { 0xC1, 0x05, 0x9E, 0xD8, 
                                                    0x36, 0x7C, 0xD5, 0x07, 
                                                    0x30, 0x70, 0xDD, 0x17, 
                                                    0xF7, 0x0E, 0x59, 0x39, 
                                                    0xFF, 0xC0, 0x0B, 0x31,
                                                    0x68, 0x58, 0x15, 0x11,
                                                    0x64, 0xF9, 0x8F, 0xA7,
                                                    0xBE, 0xFA, 0x4F, 0xA4 };
                                                   
    uint32_t ctxbase;
    ctxbase = ((uint32_t) hash_hwinfo.context_virt_base + 7) & 0xFFFFFFF8;                                         
                                       
     for (i = 0; i < MAX_SHA224_DIGEST; i++)
        *(unsigned char *) (ctxbase + i) = msg_digest[i];  
    
}

static void ast_sha256_init(void)
{
    int i;
    unsigned char msg_digest[MAX_SHA256_DIGEST] = { 0x6A, 0x09, 0xE6, 0x67, 
                                                    0xBB, 0x67, 0xAE, 0x85, 
                                                    0x3C, 0x6E, 0xF3, 0x72, 
                                                    0xA5, 0x4F, 0xF5, 0x3A, 
                                                    0x51, 0x0E, 0x52, 0x7F,
                                                    0x9B, 0x05, 0x68, 0x8C,
                                                    0x1F, 0x83, 0xD9, 0xAB,
                                                    0x5B, 0xE0, 0xCD, 0x19 };
                                                   
    uint32_t ctxbase;
    ctxbase = ((uint32_t) hash_hwinfo.context_virt_base + 7) & 0xFFFFFFF8;                                         
                                       
     for (i = 0; i < MAX_SHA256_DIGEST; i++)
        *(unsigned char *) (ctxbase + i) = msg_digest[i];   
        
}

static void ast_md5_init(void)
{
    int i;
    unsigned char msg_digest[MAX_MD5_DIGEST] = { 0x01, 0x23, 0x45, 0x67, 
                                                 0x89, 0xAB, 0xCD, 0xEF, 
                                                 0xFE, 0xDC, 0xBA, 0x98, 
                                                 0x76, 0x54, 0x32, 0x10 };
    uint32_t ctxbase;
    ctxbase = ((uint32_t) hash_hwinfo.context_virt_base + 7) & 0xFFFFFFF8;                                         
                                       
     for (i = 0; i < MAX_MD5_DIGEST; i++)
        *(unsigned char *) (ctxbase + i) = msg_digest[i];

}

static int ast_cipher_algo_init(cipher_data_t  *cipher_data)
{
    switch (cipher_data->algo)
    {
        case CIPHER_SHA1:
        case CIPHER_HMAC_SHA1:
            ast_sha1_init();
            break;
        
        case CIPHER_SHA224:
        case CIPHER_HMAC_SHA224:
            ast_sha224_init();
            break;        

        case CIPHER_SHA256:
        case CIPHER_HMAC_SHA256:
            ast_sha256_init();
            break;
            
        case CIPHER_MD5:
        case CIPHER_HMAC_MD5:
            ast_md5_init();                        
            break;                
            
        default:
        break;        
    }
    
    return 0;
}
 
static int ast_get_hash_digest_len (int cipher_algo)
{
    switch (cipher_algo)
    {        
        case CIPHER_SHA1:
        case CIPHER_HMAC_SHA1:
            return MAX_SHA1_DIGEST;
            break;
            
        case CIPHER_SHA224:
        case CIPHER_HMAC_SHA224:        
            return SHA224_DIGEST;
            break;
            
        case CIPHER_SHA256:
        case CIPHER_HMAC_SHA256:        
            return MAX_SHA256_DIGEST;
            break;
            
        case CIPHER_MD5:
        case CIPHER_HMAC_MD5:        
            return MAX_MD5_DIGEST;
            break;
        
        default :
            return 0;
            break;   
    }
    
    return 0;
}
  
static int ast_hash(cipher_data_t  *cipher_data, uint32_t command)
{
    int i;
    uint32_t srcbase, dstbase;
    unsigned int digest_len = 0;

    srcbase = ((uint32_t) hash_hwinfo.src_virt_base + 7) & 0xFFFFFFF8;
    dstbase = ((uint32_t) hash_hwinfo.dst_virt_base + 7) & 0xFFFFFFF8;
    
    /* Init HW */
    ast_cipher_write_reg(cipher_data->data_len, REG_HASH_LEN_OFFSET);

    /* Set source */
    down(&mutex_key); 	
    for (i = 0; i < cipher_data->data_len; i++)
    {   	
    	*(unsigned char *) (srcbase + i) = cipher_data->data_in[i];   	
    }
    up(&mutex_key);

    /* Send cmd */
    ast_cipher_write_reg(command, REG_HASH_CMD_OFFSET);
    wait_event_interruptible(hash_event,(hash_wakeup == HASH_WAKEUP));
    hash_wakeup = HASH_SLEEP;

    if(cipher_data->progress == CIPHER_KS_PROGRESS_COMPLETE )
    {
        digest_len = ast_get_hash_digest_len(cipher_data->algo);
 
        /* Output */
        down(&mutex_key);
        for (i = 0; i < digest_len; i++)
        {    
            cipher_data->data_out[i] = *(unsigned char *) (dstbase + i);      
        }
        up(&mutex_key);
        cipher_data->data_len = digest_len;
    }
    else
        cipher_data->data_len = 0;
                    
    return 0;
}

static int ast_sha1_hash(cipher_data_t *cipher_data)
{
    uint32_t command = (HASH_INT_ENABLE | HASH_ACCUMULATIVE_MODE | HASH_ALG_SELECT_SHA1 | HASH_BYTE_SWAP_CTL_SHA);
    
    return ast_hash(cipher_data, command);
}


int ast_cipher_process_sha1(cipher_data_t  *cipher_data)
{
    int ret = 0;
    
    if(cipher_data->progress == CIPHER_PROGRESS_INIT)
    {
        ast_sha1_init();
        return 0;
    }
        
        
    ret = ast_sha1_hash(cipher_data);
    
    if(cipher_data->progress == CIPHER_KS_PROGRESS_COMPLETE)
    {       
        cipher_data->data_len = MAX_SHA1_DIGEST;
        
    }
    else
        cipher_data->data_len = 0;
           
	return ret;
}

static int ast_sha224_hash(cipher_data_t *cipher_data)
{
    uint32_t command = (HASH_INT_ENABLE | HASH_ACCUMULATIVE_MODE | HASH_ALG_SELECT_SHA224 | HASH_BYTE_SWAP_CTL_SHA);

    return ast_hash(cipher_data, command);
}

int ast_cipher_process_sha224(cipher_data_t  *cipher_data)
{    
    int ret = 0;

    if(cipher_data->progress == CIPHER_PROGRESS_INIT)
    {
        ast_sha224_init();
        return 0;
    }
        

    ret = ast_sha224_hash(cipher_data);

    if(cipher_data->progress == CIPHER_KS_PROGRESS_COMPLETE)
        cipher_data->data_len = SHA224_DIGEST;
    else
        cipher_data->data_len = 0;

                   
	return ret;
}

static int ast_sha256_hash(cipher_data_t *cipher_data)
{
    uint32_t command = (HASH_INT_ENABLE | HASH_ACCUMULATIVE_MODE | HASH_ALG_SELECT_SHA256 | HASH_BYTE_SWAP_CTL_SHA);

    return ast_hash(cipher_data, command);
}

int ast_cipher_process_sha256(cipher_data_t  *cipher_data)
{    
    int ret = 0;

    if(cipher_data->progress == CIPHER_PROGRESS_INIT)
    {
        ast_sha256_init();
        return 0;
    }
        

    ret = ast_sha256_hash(cipher_data);

    
    if(cipher_data->progress == CIPHER_KS_PROGRESS_COMPLETE)
        cipher_data->data_len = MAX_SHA256_DIGEST;
    else
        cipher_data->data_len = 0;

                   
	return ret;
}

static int ast_md5_hash(cipher_data_t *cipher_data)
{
    uint32_t command = (HASH_INT_ENABLE | HASH_ACCUMULATIVE_MODE | HASH_ALG_SELECT_MD5 | HASH_BYTE_SWAP_CTL_MD5);
    
    return ast_hash(cipher_data, command);
}

int ast_cipher_process_md5(cipher_data_t  *cipher_data)
{    
    int ret = 0;

    if(cipher_data->progress == CIPHER_PROGRESS_INIT)
    {
        ast_md5_init();
        return 0;
    }
    

    ret = ast_md5_hash(cipher_data);

    if(cipher_data->progress == CIPHER_KS_PROGRESS_COMPLETE)
        cipher_data->data_len = MAX_MD5_DIGEST;
    else
        cipher_data->data_len = 0;

                   
	return ret;
}


/*
 * HMAC + HASH functions
 */
static unsigned char in_digest[SHA_BLOCK_SIZE];
static unsigned char out_digest[SHA_BLOCK_SIZE];
 
static void hmac_sha_pad_inner_msg(unsigned char *message_block, unsigned char digest_len)
{
    int i=0;
    int length_high = 0;
    int length_low = 0x200 + (digest_len * 8);

    message_block[digest_len] = 0x80;
    for(i = (digest_len + 1); i < 56; i++)
    {
        message_block[i] = 0;
    }
    
    /*
     *  Store the message length as the last 8 octets
     */
    message_block[56] = (length_high >> 24) & 0xFF;
    message_block[57] = (length_high >> 16) & 0xFF;
    message_block[58] = (length_high >> 8) & 0xFF;
    message_block[59] = (length_high) & 0xFF;
    message_block[60] = (length_low >> 24) & 0xFF;
    message_block[61] = (length_low >> 16) & 0xFF;
    message_block[62] = (length_low >> 8) & 0xFF;
    message_block[63] = (length_low) & 0xFF;
}

static void hmac_md5_pad_inner_msg(unsigned char *message_block, unsigned char digest_len)
{
    int i=0;
    int length_high = 0;
    int length_low = 0x200 + (digest_len * 8);
    
    message_block[digest_len] = 0x80;
    for(i = (digest_len + 1); i < 56; i++)
    {
        message_block[i] = 0;
    }
    
    /*
     *  Store the message length as the last 8 octets
     */
    message_block[56] = (length_low) & 0xFF;;
    message_block[57] = (length_low >> 8) & 0xFF;;
    message_block[58] = (length_low >> 16) & 0xFF;
    message_block[59] = (length_low >> 24)& 0xFF;
    message_block[60] = (length_high) & 0xFF;  
    message_block[61] = (length_high >> 8) & 0xFF;
    message_block[62] = (length_high >> 16) & 0xFF;
    message_block[63] = (length_high >> 24) & 0xFF;
}

static int hmac_hash_pad_inner_msg(int algo, unsigned char *message_block, unsigned int digest_len)
{    
    switch (algo)
    {
        case CIPHER_HMAC_SHA1:
        case CIPHER_HMAC_SHA224:
        case CIPHER_HMAC_SHA256:
            hmac_sha_pad_inner_msg(message_block, digest_len);
            break;
        
        case CIPHER_HMAC_MD5:
            hmac_md5_pad_inner_msg(message_block, digest_len);
            break;
        
        default :
            break;   
    }
    
    return 0;
}
 
static int ast_handle_hmac_hash(cipher_data_t  *cipher_data)
{
    switch (cipher_data->algo)
    {
        case CIPHER_SHA1:
        case CIPHER_HMAC_SHA1:
            return ast_cipher_process_sha1(cipher_data);
            break;
        
        case CIPHER_SHA224:
        case CIPHER_HMAC_SHA224:
            return ast_cipher_process_sha224(cipher_data);
            break;

        case CIPHER_SHA256:
        case CIPHER_HMAC_SHA256:
            return ast_cipher_process_sha256(cipher_data);
            break;
            
        case CIPHER_MD5:
        case CIPHER_HMAC_MD5:
            return ast_cipher_process_md5(cipher_data);                        
            break;
                            
        default:
        break;        
    }
    
    return -1;
}

static int ast_cipher_hmac_hash_set_key(cipher_data_t  *cipher_data)
{
    int i = 0;
    int ret = 0;
    
    ast_cipher_algo_init(cipher_data);  
     
    /* use cipher_data_buf to be the buffer of data_in */
    reset_data_buf(cipher_data_buf, &cipher_data_buf_len);
    cipher_data->data_in = cipher_data_buf;
    cipher_data->data_len = SHA_BLOCK_SIZE;
    
    memset(in_digest, HMAC_SHA1_IPAD, SHA_BLOCK_SIZE);
    memset(out_digest, HMAC_SHA1_OPAD, SHA_BLOCK_SIZE);
    for (i = 0; i < cipher_data->key_len; i++)
    {
        in_digest[i] = cipher_data->key[i] ^ HMAC_SHA1_IPAD;
        out_digest[i] = cipher_data->key[i] ^ HMAC_SHA1_OPAD;  
    }

    memcpy(cipher_data->data_in, in_digest, cipher_data->data_len);    
  
    ret = ast_handle_hmac_hash(cipher_data);
    cipher_data->data_len = 0;
    
    reset_data_buf(cipher_data_buf, &cipher_data_buf_len);
    
    return ret;       
}
 
static int ast_cipher_hmac_hash_update(cipher_data_t  *cipher_data)
{
    int ret = 0;
    unsigned int digest_len = 0;
    unsigned char inner_buffer[SHA_BLOCK_SIZE];    
    cipher_data_t  cdata;
    
    memcpy(&cdata, cipher_data, sizeof(cipher_data_t));
        
    ret = ast_handle_hmac_hash(&cdata);
    cipher_data->data_len = 0;
 
    if(cipher_data->progress == CIPHER_KS_PROGRESS_COMPLETE)
    {  
        ast_cipher_algo_init(&cdata);
   
        digest_len = ast_get_hash_digest_len(cipher_data->algo);
        memset(inner_buffer, 0, SHA_BLOCK_SIZE);       
        memcpy(inner_buffer, cdata.data_out, digest_len);
        hmac_hash_pad_inner_msg(cdata.algo, inner_buffer, digest_len);
        
        cdata.data_in = cipher_data_buf;
        cdata.data_len = (SHA_BLOCK_SIZE + SHA_BLOCK_SIZE);
        memset(cdata.data_in, 0, cdata.data_len);
    
        memcpy(cdata.data_in, out_digest, SHA_BLOCK_SIZE);
        memcpy(&(cdata.data_in[SHA_BLOCK_SIZE]), inner_buffer, SHA_BLOCK_SIZE);

        
        ret = ast_handle_hmac_hash(&cdata);
        cipher_data->data_len = digest_len;  
                     
        reset_data_buf(cipher_data_buf, &cipher_data_buf_len);
        memset(in_digest, HMAC_SHA1_IPAD, SHA_BLOCK_SIZE); 
        memset(out_digest, HMAC_SHA1_OPAD, SHA_BLOCK_SIZE); 
    }
    
    return ret;          
}

static int ast_cipher_process_hmac_hash(cipher_data_t  *cipher_data)
{
    if(cipher_data->progress == CIPHER_PROGRESS_INIT)
    {
        ast_cipher_algo_init(cipher_data);
        return 0;
    }
    
    if (cipher_data->method == HMAC_SET_KEY)
        return (ast_cipher_hmac_hash_set_key(cipher_data));
    
    else if (cipher_data->method == HMAC_HASH)
        return (ast_cipher_hmac_hash_update(cipher_data));
                   
    else
        return -1;
}

int ast_cipher_process_hmac_sha1(cipher_data_t  *cipher_data)
{
    return ast_cipher_process_hmac_hash(cipher_data);
}

int ast_cipher_process_hmac_sha224(cipher_data_t  *cipher_data)
{
    return ast_cipher_process_hmac_hash(cipher_data);
}

int ast_cipher_process_hmac_sha256(cipher_data_t  *cipher_data)
{
    return ast_cipher_process_hmac_hash(cipher_data);
}

int ast_cipher_process_hmac_md5(cipher_data_t  *cipher_data)
{
    return ast_cipher_process_hmac_hash(cipher_data);
}


/*
 * init, exit, ops functions
 */
static cipher_hal_operations_t ast_cipher_hw_ops = {
    .cipher_get_max_datalen = ast_cipher_get_max_datalen,
    .cipher_get_info              = ast_cipher_get_info,
    .cipher_process_data_padding  = ast_cipher_process_data_padding,
    .cipher_process_remove_data_padding  = ast_cipher_process_remove_data_padding,
	.cipher_process_aes = ast_cipher_process_aes,
	.cipher_process_rc4 = ast_cipher_process_rc4,
#if defined(GROUP_AST2300) || defined(GROUP_AST2300_PLUS)
    .cipher_process_des = ast_cipher_process_des,
    .cipher_process_3des = ast_cipher_process_des3,
    .cipher_process_rsa = ast_cipher_process_rsa,
#endif    
    .cipher_process_sha1 = ast_cipher_process_sha1,
    .cipher_process_sha224 = ast_cipher_process_sha224,
    .cipher_process_sha256 = ast_cipher_process_sha256,
    .cipher_process_md5 = ast_cipher_process_md5,
    .cipher_process_hmac_sha1 = ast_cipher_process_hmac_sha1,
    .cipher_process_hmac_sha224 = ast_cipher_process_hmac_sha224,
    .cipher_process_hmac_sha256 = ast_cipher_process_hmac_sha256,
    .cipher_process_hmac_md5 = ast_cipher_process_hmac_md5    
};

static hw_hal_t ast_cipher_hw_hal = {
	.dev_type = EDEV_TYPE_CIPHER,
	.owner = THIS_MODULE,
	.devname = AST_CIPHER_DRIVER_NAME,
	.num_instances = AST_MAX_CIPHER_HWDEV,
	.phal_ops = (void *) &ast_cipher_hw_ops
};



static int ast_cipher_init_hw(void)
{

    /* allocate resource */
    crypto_hwinfo.src_virt_base = dma_alloc_coherent(NULL,HW_MAX_DATA_LEN,&crypto_hwinfo.crypto_src_dma_addr,GFP_KERNEL);
    if(crypto_hwinfo.src_virt_base == NULL)
    {
        printk(KERN_WARNING "%s: memory alloc failed\n", AST_CIPHER_DRIVER_NAME);
        return -1;
    }

    crypto_hwinfo.dst_virt_base = dma_alloc_coherent(NULL,HW_MAX_DATA_LEN, &crypto_hwinfo.crypto_dst_dma_addr,GFP_KERNEL);
    if(crypto_hwinfo.dst_virt_base == NULL)
    {
        printk(KERN_WARNING "%s: memory alloc failed\n", AST_CIPHER_DRIVER_NAME);
        return -1;
    }

    crypto_hwinfo.context_virt_base = dma_alloc_coherent(NULL,HW_MAX_DATA_LEN, &crypto_hwinfo.crypto_context_dma_addr,GFP_KERNEL);
    if(crypto_hwinfo.context_virt_base == NULL)
    {
        printk(KERN_WARNING "%s: memory alloc failed\n", AST_CIPHER_DRIVER_NAME);
        return -1;
    }

    dbgprint("Crypto Data Src Virtual Address: %x \n", (u32) crypto_hwinfo.src_virt_base);
    dbgprint("Crypto Data Dst Virtual Address: %x \n", (u32) crypto_hwinfo.dst_virt_base);
    dbgprint("Crypto Data Context Virtual Address: %x \n", (u32) crypto_hwinfo.context_virt_base);
    
    hash_hwinfo.src_virt_base = dma_alloc_coherent(NULL,HW_MAX_DATA_LEN,&hash_hwinfo.hash_src_dma_addr,GFP_KERNEL);
     if(hash_hwinfo.src_virt_base == NULL)
    {
         printk(KERN_WARNING "%s: memory alloc failed\n", AST_CIPHER_DRIVER_NAME);
         return -1;
    }
    
     hash_hwinfo.dst_virt_base = dma_alloc_coherent(NULL,HW_MAX_DATA_LEN,&hash_hwinfo.hash_dst_dma_addr,GFP_KERNEL);
     if(hash_hwinfo.dst_virt_base == NULL)
    {
         printk(KERN_WARNING "%s: memory alloc failed\n", AST_CIPHER_DRIVER_NAME);
         return -1;
    }

    /* Key buffer and digest buffer must use the same space */
    hash_hwinfo.context_virt_base = hash_hwinfo.dst_virt_base;

    dbgprint("Hash Data Src Virtual Address: %x \n", (u32) hash_hwinfo.src_virt_base);
    dbgprint("Hash Data Dst Virtual Address: %x \n", (u32) hash_hwinfo.dst_virt_base);
    dbgprint("Hash Data Context Virtual Address: %x \n", (u32) hash_hwinfo.context_virt_base);
    
    /* Init H/W Engines */
    ast_cipher_enable_hmac();
    
	/* Init data buffer in H/W layer */
    cipher_data_buf = (unsigned char *) kmalloc (HW_MAX_DATA_LEN, GFP_KERNEL);
    cipher_data_buf_len = 0;
    return 0;
}

irqreturn_t ast_cipher_irq_handler( int this_irq, void *dev_id)
{
    volatile uint32_t temp=0;

    temp = ast_cipher_read_reg(REG_CRYPTO_STATUS_OFFSET);
    if((temp & HASH_INT_ENABLE) == HASH_INT_ENABLE)
    {
        hash_wakeup = HASH_WAKEUP;
        wake_up_interruptible_all(&hash_event);
        ast_cipher_write_reg(HASH_INT_ENABLE,REG_CRYPTO_STATUS_OFFSET);
    }

    if((temp & CRYPTO_INT_ENABLE) == CRYPTO_INT_ENABLE)
    {
        crypto_wakeup = CRYPTO_WAKEUP;
        wake_up_interruptible_all(&crypto_event);
        ast_cipher_write_reg(CRYPTO_INT_ENABLE,REG_CRYPTO_STATUS_OFFSET);
    }

    return IRQ_HANDLED;
}

static void free_dma_alloc_mem(void)
{
    if(crypto_hwinfo.src_virt_base)
        dma_free_coherent(NULL,HW_MAX_DATA_LEN,crypto_hwinfo.src_virt_base, crypto_hwinfo.crypto_src_dma_addr);
    if(crypto_hwinfo.dst_virt_base)
        dma_free_coherent(NULL,HW_MAX_DATA_LEN,crypto_hwinfo.dst_virt_base, crypto_hwinfo.crypto_dst_dma_addr);
    if(crypto_hwinfo.context_virt_base)
        dma_free_coherent(NULL,HW_MAX_DATA_LEN,crypto_hwinfo.context_virt_base, crypto_hwinfo.crypto_context_dma_addr);
    if(hash_hwinfo.src_virt_base)
        dma_free_coherent(NULL,HW_MAX_DATA_LEN,hash_hwinfo.src_virt_base, hash_hwinfo.hash_src_dma_addr);
    if(hash_hwinfo.dst_virt_base)
        dma_free_coherent(NULL,HW_MAX_DATA_LEN,hash_hwinfo.dst_virt_base, hash_hwinfo.hash_dst_dma_addr);
}

static int ast_cipher_module_init(void)
{
	int ret;

	extern int cipher_core_loaded;
	if (!cipher_core_loaded)
			return -1;

	if (!request_mem_region(AST_CIPHER_REG_BASE, AST_CIPHER_REG_SIZE, AST_CIPHER_DRIVER_NAME)) {
		printk(KERN_WARNING "%s: request memory region failed\n", AST_CIPHER_DRIVER_NAME);
		return -EBUSY;
	}

	ast_cipher_virt_base = ioremap(AST_CIPHER_REG_BASE, AST_CIPHER_REG_SIZE);
	if (!ast_cipher_virt_base) {
		printk(KERN_WARNING "%s: ioremap failed\n", AST_CIPHER_DRIVER_NAME);
		ret = -ENOMEM;
		goto out_mem_region;
	}

#if defined(GROUP_AST2300) || defined(GROUP_AST2300_PLUS)
	if (!request_mem_region(AST_RSA_SRAM_BASE, AST_RSA_SRAM_REG_SIZE, AST_CIPHER_DRIVER_NAME)) {
		printk(KERN_WARNING "%s: request memory region (rsa sram) failed\n", AST_CIPHER_DRIVER_NAME);
		ret = -EBUSY;
		goto out_mem_region;
	}
	
	ast_sram_virt_base = ioremap(AST_RSA_SRAM_BASE, AST_RSA_SRAM_REG_SIZE);
	if (!ast_sram_virt_base) {
		printk(KERN_WARNING "%s: ioremap failed (rsa sram)\n", AST_CIPHER_DRIVER_NAME);
		ret = -ENOMEM;
		goto out_sram_mem_region;
	}
#endif

#if defined(GROUP_AST2300)
	if (!request_mem_region(AST_RSA_SRAMOUT_BASE, AST_RSA_SRAM2_REG_SIZE, AST_CIPHER_DRIVER_NAME)) {
		printk(KERN_WARNING "%s: request memory region (rsa sram2) failed\n", AST_CIPHER_DRIVER_NAME);
		ret = -EBUSY;
		goto out_sram2_mem_region;
	}
	
	ast_sram2_virt_base = ioremap(AST_RSA_SRAMOUT_BASE, AST_RSA_SRAM2_REG_SIZE);
	if (!ast_sram2_virt_base) {
		printk(KERN_WARNING "%s: ioremap failed (rsa sram2)\n", AST_CIPHER_DRIVER_NAME);
		ret = -ENOMEM;
		goto out_sram2_mem_region;
	}
#endif

	if(ast_cipher_init_hw() != 0)
	{
		free_dma_alloc_mem();
	}

	IRQ_SET_HIGH_LEVEL(AST_IRQ_HAC);
	IRQ_SET_LEVEL_TRIGGER(AST_IRQ_HAC);

	ret = request_irq(AST_IRQ_HAC, ast_cipher_irq_handler, IRQF_DISABLED, AST_CIPHER_DRIVER_NAME, NULL);
	if (ret != 0) {
		printk(KERN_WARNING "%s: request IRQ failed\n", AST_CIPHER_DRIVER_NAME);
		goto out_mem_region;
	}

	ast_cipher_hal_id = register_hw_hal_module(&ast_cipher_hw_hal, (void **) &cipher_core_ops);
	if (ast_cipher_hal_id < 0) {
		printk(KERN_WARNING "%s: register CIPHER HAL HW module failed\n", AST_CIPHER_DRIVER_NAME);
	}

	return 0;

#if defined(GROUP_AST2300)
out_sram2_mem_region:
	release_mem_region(AST_RSA_SRAMOUT_BASE, AST_RSA_SRAM2_REG_SIZE);
#endif

#if defined(GROUP_AST2300) || defined(GROUP_AST2300_PLUS)
out_sram_mem_region:
	release_mem_region(AST_RSA_SRAM_BASE, AST_RSA_SRAM_REG_SIZE);
#endif

out_mem_region:
	release_mem_region(AST_CIPHER_REG_BASE, AST_CIPHER_REG_SIZE);

	return ret;
}

static void ast_cipher_module_exit(void)
{
    disable_irq( AST_IRQ_HAC );
    free_irq( AST_IRQ_HAC, NULL );
    free_dma_alloc_mem();
    kfree(cipher_data_buf);
    
#if defined(GROUP_AST2300)
	iounmap(ast_sram2_virt_base);
	release_mem_region(AST_RSA_SRAMOUT_BASE, AST_RSA_SRAM2_REG_SIZE);
#endif
#if defined(GROUP_AST2300) || defined(GROUP_AST2300_PLUS)
	iounmap(ast_sram_virt_base);
	release_mem_region(AST_RSA_SRAM_BASE, AST_RSA_SRAM_REG_SIZE);
#endif
	iounmap(ast_cipher_virt_base);
	release_mem_region(AST_CIPHER_REG_BASE, AST_CIPHER_REG_SIZE);
	unregister_hw_hal_module(EDEV_TYPE_CIPHER, ast_cipher_hal_id);
}

                                                                      
/*--------------------------- Module Information Follows --------------------------*/

module_init(ast_cipher_module_init);
module_exit(ast_cipher_module_exit);

MODULE_AUTHOR("American Megatrends Inc.");

#if defined(GROUP_AST2300) || defined(GROUP_AST2300_PLUS)
MODULE_DESCRIPTION("AST2300/2400/1250/1050 Cipher driver");
#else
MODULE_DESCRIPTION("AST2100/2050/2200/2150 Cipher driver");
#endif

MODULE_LICENSE("GPL");
