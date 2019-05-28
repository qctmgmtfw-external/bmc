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
 * ast_cipher.h
 * ASPEED AST2100/2050/2200/2150/2300/2400 Crypto controller
 * definitions, macros, prototypes
 *
*****************************************************************/

#ifndef __AST_CIPHER_H__
#define __AST_CIPHER_H__

#if defined(SOC_AST2300) || defined(SOC_AST1050_1070)
#define GROUP_AST2300 1
#endif
#if defined(SOC_AST2400) || defined(SOC_AST1250) || defined(SOC_AST2500) || defined(SOC_AST2520)
#define GROUP_AST2300_PLUS 1
#endif

#define AST_CIPHER_REG_BASE		0x1e6e3000
#define AST_CIPHER_REG_SIZE     SZ_4K
#define AST_MAX_CIPHER_HWDEV    1

#define AST_SCU_UNLOCK_MAGIC    0x1688A8A8
#define AST_CIPHER_READ_TIMEOUT 2000

#define SCU_REG_BASE 			0x1e6e2000

#define AST_IRQ_HAC 4

/* Crypto controller registers */
#define REG_CRYPTO_SRC_BASE_OFFSET	0x00
#define REG_CRYPTO_DST_BASE_OFFSET	0x04
#define REG_CRYPTO_CONTEXT_BASE_OFFSET	0x08
#define REG_CRYPTO_LEN_OFFSET		0x0C
#define REG_CRYPTO_CMD_BASE_OFFSET	0x10
//#define REG_CRYPTO_ENABLE_OFFSET	0x14
#define REG_CRYPTO_STATUS_OFFSET	0x1C

#define REG_HASH_SRC_BASE_OFFSET	0x20
#define REG_HASH_DST_BASE_OFFSET	0x24
#define REG_HASH_KEY_BASE_OFFSET	0x28
#define REG_HASH_LEN_OFFSET		0x2C
#define REG_HASH_CMD_OFFSET		0x30
//#define REG_HASH_ENABLE_OFFSET		0x14
#define REG_HASH_STATUS_OFFSET		0x1C
#if defined(GROUP_AST2300)
#define REG_RSA_EXP_OFFSET	          0x40
#elif defined(GROUP_AST2300_PLUS)
#define REG_RSA_MODEXP_BITNUM_OFFSET	0x40
#endif
#define REG_RSA_CTRL_CMD_OFFSET	0x4C

#define HASH_BUSY			0x01
#define CRYPTO_BUSY			0x02
#define RSA_BUSY      0x04
#define HMAC_ENABLE                 0x80
#define HMAC_DIGEST_CAL_ENABLE      0x180
#define HASH_INT_ENABLE	            0x200
#define CRYPTO_INT_ENABLE	0x1000
#define RSA_INT_ENABLE	0x2000

//#define CRYPTO_SYNC_MODE_MASK		0x03
#define CRYPTO_SYNC_MODE_ASYNC		0x00

#define CRYPTO_AES128			0x00
#define CRYPTO_AES192			0x04
#define CRYPTO_AES256			0x08

#define CRYPTO_AES_ECB			0x00
#define CRYPTO_AES_CBC			0x10
#define CRYPTO_AES_CFB			0x20
#define CRYPTO_AES_OFB			0x30
#define CRYPTO_AES_CTR			0x40

#define CRYPTO_DES_ECB			CRYPTO_AES_ECB
#define CRYPTO_DES_CBC			CRYPTO_AES_CBC
#define CRYPTO_DES_CFB			CRYPTO_AES_CFB
#define CRYPTO_DES_OFB			CRYPTO_AES_OFB
#define CRYPTO_DES_CTR			CRYPTO_AES_CTR

#define CRYPTO_ENCRYPTO			0x80
#define CRYPTO_DECRYPTO			0x00

#define CRYPTO_AES			0x000
#define CRYPTO_RC4			0x100

#define CRYPTO_DES1			0x00010000
#define CRYPTO_DES3			0x00030000

#define CRYPTO_ENABLE_RW		0x000
#define CRYPTO_ENABLE_CONTEXT_LOAD	0x000
#define CRYPTO_ENABLE_CONTEXT_SAVE	0x000

#define RSA_FIRE_ENGINE 	      0x00000800
#define RSA_ENABLE_SRAMACCESS 	0x00001000
#define RSA_MODE_EXP	          0x00000000
#define RSA_MODE_EXP1	          0x00000100
#define RSA_MODE_EXP2	          0x00000200
#define RSA_MODE_EXP3	          0x00000300

/* General */
#define MAX_AES_KEY_LEN     32
#define MAX_AES_IV_LEN      32
#define MAX_RC4_KEY_LEN		256 

#define HW_MAX_DATA_LEN     (1024)


#define MAX_CONTEXT_BUF_LEN 1024
#define MAX_HASH_KEY_BUF_LEN  64
#define HASH_ALG_SELECT_MD5		    0x00
#define HASH_ALG_SELECT_SHA1		0x20
#define HASH_ALG_SELECT_SHA224		0x40
#define HASH_ALG_SELECT_SHA256		0x50
#define HASH_BYTE_SWAP_CTL_MD5      0x04
#define HASH_BYTE_SWAP_CTL_SHA      0x08
#define HASH_ACCUMULATIVE_MODE      0x100            

#define HASHMODE_MD5		0x00
#define HASHMODE_SHA1		0x01
#define HASHMODE_SHA256		0x02
#define HASHMODE_SHA224		0x03

#define MIXMODE_DISABLE     0x00
#define MIXMODE_CRYPTO      0x02
#define MIXMODE_HASH        0x03

#define RET_PROCESS_CIPHER_DATA      1
#define RET_SKIP_PROCESS_CIPHER_DATA 0

#define MAX_SHA1_DIGEST     20
#define SHA224_DIGEST       28
#define MAX_SHA224_DIGEST   32
#define MAX_SHA256_DIGEST   32
#define MAX_MD5_DIGEST      16

#define SHA_BLOCK_SIZE      64
#define MD5_BLOCK_SIZE      64

#define HASH_SLEEP 0
#define CRYPTO_SLEEP 0
#define HASH_WAKEUP 1
#define CRYPTO_WAKEUP 1

/* aes */
typedef struct {

    unsigned int key_bitlen;	
    unsigned char aes_key[MAX_AES_KEY_LEN];

} aes_key_info_t;

typedef struct {
	
    unsigned char aes_iv[MAX_AES_IV_LEN];

} aes_iv_info_t;
	
/* rc4 */
typedef struct {

    unsigned int key_bitlen;	
    unsigned char rc4_key[MAX_RC4_KEY_LEN];

} rc4_key_info_t;


typedef struct {
  unsigned char *src_virt_base;
  unsigned char *dst_virt_base;
  unsigned char *context_virt_base;
  dma_addr_t crypto_src_dma_addr;
  dma_addr_t crypto_dst_dma_addr;
  dma_addr_t crypto_context_dma_addr;
} crypto_hw_info_t;
	
typedef struct {
  unsigned char *src_virt_base;
  unsigned char *dst_virt_base;
  dma_addr_t hash_src_dma_addr;
  dma_addr_t hash_dst_dma_addr;
  unsigned char *context_virt_base;
  	
} hash_hw_info_t;

/*RSA*/
#if defined(GROUP_AST2300)
/* AST2300_RSA */
#define AST_RSA_SRAM_BASE		  0x1e722000
#define AST_RSA_SRAM_REG_SIZE SZ_4K
#define AST_RSA_MSG_OFFSET	  0x000   //X Buffer
#define AST_RSA_MOD_OFFSET	  0x800   //n Buffer
#define AST_RSA_SRAMOUT_BASE	0x1e723000  //A Buffer
#define AST_RSA_SRAM2_REG_SIZE (SZ_4K / 8)
#elif defined(GROUP_AST2300_PLUS)
/* AST2400_RSA */
#define AST_RSA_SRAM_BASE		  0x1e727000
#define AST_RSA_SRAM_REG_SIZE SZ_4K
#define AST_RSA_EXP_OFFSET    0x000    //e Buffer
#define AST_RSA_MSG_OFFSET    0x200    //X/A Buffer
#define AST_RSA_OUT_OFFSET    0x200    //X/A Buffer
#define AST_RSA_NP_OFFSET     0x400    //n�� Buffer
#define AST_RSA_MOD_OFFSET    0x800    //n Buffer
#endif  /*RSA*/

#endif /* __AST_CIPHER_H__ */

