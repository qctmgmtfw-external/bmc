/****************************************************************
 **                                                            **
 **    (C)Copyright 2009-2015, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross              **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************/

/****************************************************************
 *
 * ast_kcs.c
 * ASPEED AST2100/2050/2200/2150/2300/2400 KCS controller driver
 *
*****************************************************************/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/errno.h>
#include <linux/ioport.h>
#include <linux/delay.h>
#include <mach/platform.h>

#include <asm/irq.h>
#include <asm/io.h>
#include <asm/uaccess.h>

#include "driver_hal.h"
#include "kcs.h"
#include "ast_kcs.h"
#include "coreTypes.h"

#define AST_KCS_DRIVER_NAME        "ast_kcs"

#define AST_KCS_REG_VA_BASE         IO_ADDRESS(AST_KCS_REG_BASE)

/** Default values in these arrays are for the as **/
static u32 kcs_as_io_base[ AST_KCS_CHANNEL_NUM ] =
{
#if defined(GROUP_AST1070_COMPANION)
    AST_BMCCC_LPC0_VA_BASE(0),
    AST_BMCCC_LPC0_VA_BASE(0),
    AST_BMCCC_LPC0_VA_BASE(0),
    AST_BMCCC_LPC1_VA_BASE(0),
    AST_BMCCC_LPC1_VA_BASE(0),
    AST_BMCCC_LPC1_VA_BASE(0),
    AST_BMCCC_LPC2_VA_BASE(0),
    AST_BMCCC_LPC2_VA_BASE(0),
    AST_BMCCC_LPC2_VA_BASE(0),
    AST_BMCCC_LPC3_VA_BASE(0),
    AST_BMCCC_LPC3_VA_BASE(0),
    AST_BMCCC_LPC3_VA_BASE(0),
#if (CONFIG_SPX_FEATURE_NUM_BMC_COMPANION_DEVICES >= 2)
    AST_BMCCC_LPC0_VA_BASE(1),
    AST_BMCCC_LPC0_VA_BASE(1),
    AST_BMCCC_LPC0_VA_BASE(1),
    AST_BMCCC_LPC1_VA_BASE(1),
    AST_BMCCC_LPC1_VA_BASE(1),
    AST_BMCCC_LPC1_VA_BASE(1),
    AST_BMCCC_LPC2_VA_BASE(1),
    AST_BMCCC_LPC2_VA_BASE(1),
    AST_BMCCC_LPC2_VA_BASE(1),
    AST_BMCCC_LPC3_VA_BASE(1),
    AST_BMCCC_LPC3_VA_BASE(1),
    AST_BMCCC_LPC3_VA_BASE(1),
#endif /* CONFIG_SPX_FEATURE_NUM_BMC_COMPANION_DEVICES >= 2 */
#else /* !defined(GROUP_AST1070_COMPANION) */

    AST_KCS_REG_VA_BASE, 
    AST_KCS_REG_VA_BASE,
    AST_KCS_REG_VA_BASE,
#if (AST_KCS_CHANNEL_NUM>=4)
    AST_KCS_REG_VA_BASE, 
#endif

#endif /* !defined(GROUP_AST1070_COMPANION) */
};

static kcs_core_funcs_t *kcs_core_ops=NULL;
static int ast_kcs_hal_id;

#if defined(GROUP_AST2300) || defined(GROUP_AST2300_PLUS)
bool kcs_enabled_by_user[AST_KCS_CHANNEL_NUM] = {FALSE,FALSE,FALSE,FALSE};
#elif defined(GROUP_AST1070_COMPANION)
bool kcs_enabled_by_user[AST_KCS_CHANNEL_NUM] = {
                                                    FALSE,FALSE,FALSE,
                                                    FALSE,FALSE,FALSE,
                                                    FALSE,FALSE,FALSE,
                                                    FALSE,FALSE,FALSE,
#if (CONFIG_SPX_FEATURE_NUM_BMC_COMPANION_DEVICES >= 2)
                                                    FALSE,FALSE,FALSE,
                                                    FALSE,FALSE,FALSE,
                                                    FALSE,FALSE,FALSE,
                                                    FALSE,FALSE,FALSE,
#endif /* CONFIG_SPX_FEATURE_NUM_BMC_COMPANION_DEVICES >= 2 */
                                                };
#else
bool kcs_enabled_by_user[AST_KCS_CHANNEL_NUM] = {FALSE,FALSE,FALSE};
#endif

inline uint32_t ast_kcs_read_reg(u8 ch_num, uint32_t reg)
{
    return ioread32((void * __iomem)kcs_as_io_base[ch_num] + reg);
}

inline void ast_kcs_write_reg(u8 ch_num, uint32_t data, uint32_t reg)
{
    iowrite32(data, (void * __iomem)kcs_as_io_base[ch_num] + reg);
}

static void ast_kcs_enable_channel(void)
{
    uint32_t reg;
    int i=0;

#if defined(GROUP_AST1070_COMPANION)
    for (i=0;i<AST_KCS_CHANNEL_NUM; i+=AST_KCS_CHAN_NUM_EACH_NODE) 
#endif
    {
       reg = ast_kcs_read_reg(i, AST_LPC_HICR0);
       reg |= (AST_LPC_HICR0_LPC3E | AST_LPC_HICR0_LPC2E | AST_LPC_HICR0_LPC1E);
       ast_kcs_write_reg(i, reg, AST_LPC_HICR0);
    }

    #if defined(GROUP_AST2300) || defined(GROUP_AST2300_PLUS)
    reg = ast_kcs_read_reg(i, AST_LPC_HICRB);
    reg |= AST_LPC_HICRB0_KCS4E;
    ast_kcs_write_reg(i, reg, AST_LPC_HICRB);
    #endif
}

static void ast_kcs_disable_channel(void)
{
    uint32_t reg;
    int i=0;

#if defined(GROUP_AST1070_COMPANION)
    for (i=0;i<AST_KCS_CHANNEL_NUM; i+=AST_KCS_CHAN_NUM_EACH_NODE) 
#endif
    {
        reg = ast_kcs_read_reg(i, AST_LPC_HICR0);
        reg &= ~(AST_LPC_HICR0_LPC3E | AST_LPC_HICR0_LPC2E | AST_LPC_HICR0_LPC1E);
        ast_kcs_write_reg(i, reg, AST_LPC_HICR0);
    }
    
    #if defined(GROUP_AST2300) || defined(GROUP_AST2300_PLUS)
    reg = ast_kcs_read_reg(i, AST_LPC_HICRB);
    reg &= ~(AST_LPC_HICRB0_KCS4E );
    ast_kcs_write_reg(i, reg, AST_LPC_HICRB);
    #endif
}

static void ast_kcs_configure_io_port_addr(void)
{
    uint32_t reg;
    int i=0;

#if defined(GROUP_AST1070_COMPANION)
    for (i=0;i<AST_KCS_CHANNEL_NUM; i+=AST_KCS_CHAN_NUM_EACH_NODE) 
#endif
    {
        /* switch the access of LADR12 to LADR1 */
        reg = ast_kcs_read_reg(i, AST_LPC_HICR4);
        reg &= ~AST_LPC_HICR4_LADR12SEL;
        ast_kcs_write_reg(i, reg, AST_LPC_HICR4);

        mb();

        /* set I/O port address of channel 1*/
        ast_kcs_write_reg(i, AST_KCS_ADR1_HI, AST_LPC_LADR12H);
        ast_kcs_write_reg(i, AST_KCS_ADR1_LO, AST_LPC_LADR12L);

        mb();

        /* switch the access of LADR12 to LADR2 */
        reg = ast_kcs_read_reg(i, AST_LPC_HICR4);
        reg |= AST_LPC_HICR4_LADR12SEL;
        ast_kcs_write_reg(i, reg, AST_LPC_HICR4);

        mb();

        /* set I/O port address 2 */
        ast_kcs_write_reg(i, AST_KCS_ADR2_HI, AST_LPC_LADR12H);
        ast_kcs_write_reg(i, AST_KCS_ADR2_LO, AST_LPC_LADR12L);

        mb();

        /* enable KCS in channel 3 */
        reg = ast_kcs_read_reg(i, AST_LPC_HICR4);
        reg |= AST_LPC_HICR4_KCSENBL;
        ast_kcs_write_reg(i, reg, AST_LPC_HICR4);

        mb();

        /* set I/O port address of channel 3 */
        ast_kcs_write_reg(i, AST_KCS_ADR3_HI, AST_LPC_LADR3H);
        ast_kcs_write_reg(i, AST_KCS_ADR3_LO, AST_LPC_LADR3L);

        mb();
    }

    #if defined(GROUP_AST2300) || defined(GROUP_AST2300_PLUS)
    /* enable KCS in channel 4 */
    reg = ast_kcs_read_reg(i, AST_LPC_HICRB);
    reg |= AST_LPC_HICRB0_KCS4E;
    ast_kcs_write_reg(i, reg, AST_LPC_HICRB);

    mb();
    
    /* set I/O port address of channel 4 */
    ast_kcs_write_reg(i, AST_KCS_ADR4, AST_LPC_LADR4);
    #endif
}

static unsigned char ast_kcs_num_ch(void)
{
    return AST_KCS_CHANNEL_NUM;
}
    
static void ast_kcs_enable_interrupt(u8 ch_num)
{
    uint32_t reg;

#if defined(GROUP_AST1070_COMPANION)
    if((ch_num%AST_KCS_CHAN_NUM_EACH_NODE) == 0)
#else
    if(ch_num == 0)
#endif
    {
        reg = ast_kcs_read_reg(ch_num, AST_LPC_HICR2);
        reg |= AST_LPC_HICR2_IBFIE1; 
        ast_kcs_write_reg(ch_num, reg, AST_LPC_HICR2);
    }
#if defined(GROUP_AST1070_COMPANION)
    else if((ch_num%AST_KCS_CHAN_NUM_EACH_NODE) == 1)
#else
    else if(ch_num == 1)
#endif
    {
        reg = ast_kcs_read_reg(ch_num, AST_LPC_HICR2);
        reg |= AST_LPC_HICR2_IBFIE2;
        ast_kcs_write_reg(ch_num, reg, AST_LPC_HICR2);
    }
#if defined(GROUP_AST1070_COMPANION)
    else if((ch_num%AST_KCS_CHAN_NUM_EACH_NODE) == 2)
#else
    else if(ch_num == 2)
#endif
    {
        reg = ast_kcs_read_reg(ch_num, AST_LPC_HICR2);
        reg |= AST_LPC_HICR2_IBFIE3;
        ast_kcs_write_reg(ch_num, reg, AST_LPC_HICR2);
    }
    #if defined(GROUP_AST2300) || defined(GROUP_AST2300_PLUS)
    else if(ch_num == 3)
    {
        reg = ast_kcs_read_reg(ch_num, AST_LPC_HICRB);
        reg |= (AST_LPC_HICRB0_KCS4INTE);
        ast_kcs_write_reg(ch_num, reg, AST_LPC_HICRB);
    }
    #endif
}

static void ast_kcs_disable_interrupt(u8 ch_num)
{
    uint32_t reg;

#if defined(GROUP_AST1070_COMPANION)
    if((ch_num%AST_KCS_CHAN_NUM_EACH_NODE) == 0)
#else
    if(ch_num == 0)
#endif
    {
        reg = ast_kcs_read_reg(ch_num, AST_LPC_HICR2);
        reg &= ~AST_LPC_HICR2_IBFIE1;
        ast_kcs_write_reg(ch_num, reg, AST_LPC_HICR2);
    }
#if defined(GROUP_AST1070_COMPANION)
    else if((ch_num%AST_KCS_CHAN_NUM_EACH_NODE) == 1)
#else
    else if(ch_num == 1)
#endif
    {
        reg = ast_kcs_read_reg(ch_num, AST_LPC_HICR2);
        reg &= ~AST_LPC_HICR2_IBFIE2;
        ast_kcs_write_reg(ch_num, reg, AST_LPC_HICR2);
    }
#if defined(GROUP_AST1070_COMPANION)
    else if((ch_num%AST_KCS_CHAN_NUM_EACH_NODE) == 2)
#else
    else if(ch_num == 2)
#endif
    {
        reg = ast_kcs_read_reg(ch_num, AST_LPC_HICR2);
        reg &= ~AST_LPC_HICR2_IBFIE3;
        ast_kcs_write_reg(ch_num, reg, AST_LPC_HICR2);
    }
    #if defined(GROUP_AST2300) || defined(GROUP_AST2300_PLUS)
    else if(ch_num == 3)
    {
        reg = ast_kcs_read_reg(ch_num, AST_LPC_HICRB);
        reg &= ~(AST_LPC_HICRB0_KCS4INTE);
        ast_kcs_write_reg(ch_num, reg, AST_LPC_HICRB);
    }
    #endif
}

static void ast_kcs_read_status(u8 channel, u8 *status)
{
    #if defined(GROUP_AST2300) || defined(GROUP_AST2300_PLUS)
    if (channel == 3)
    {
        *status = (uint8_t) ast_kcs_read_reg(channel, AST_LPC_STR4);
    }
    else
    #endif
        *status = (uint8_t) ast_kcs_read_reg(channel, AST_LPC_STR_CH(channel));
}

static void ast_kcs_write_status(u8 channel, u8 status)
{
    #if defined(GROUP_AST2300) || defined(GROUP_AST2300_PLUS)
    if (channel == 3)
    {
        ast_kcs_write_reg(channel, status, AST_LPC_STR4);
    }
    else
    #endif
        ast_kcs_write_reg(channel, status, AST_LPC_STR_CH(channel));

}

static void ast_kcs_read_command(u8 channel, u8 *command)
{
    #if defined(GROUP_AST2300) || defined(GROUP_AST2300_PLUS)
    if (channel == 3)
    {
        *command = (uint8_t) ast_kcs_read_reg(channel, AST_LPC_IDR4);
    }
    else
    #endif
        *command = (uint8_t) ast_kcs_read_reg(channel, AST_LPC_IDR_CH(channel));
}

static void ast_kcs_read_data(u8 channel, u8 *data)
{
    #if defined(GROUP_AST2300) || defined(GROUP_AST2300_PLUS)
    if(channel == 3)
    {
        *data = (uint8_t) ast_kcs_read_reg(channel, AST_LPC_IDR4);
    }
    else
    #endif
        *data = (uint8_t) ast_kcs_read_reg(channel, AST_LPC_IDR_CH(channel));
    
}

static void ast_kcs_write_data(u8 channel, u8 data)
{
    #if defined(GROUP_AST2300) || defined(GROUP_AST2300_PLUS)
    if (channel == 3)
    {
        ast_kcs_write_reg(channel, data, AST_LPC_ODR4);
    }
    else
    #endif
        ast_kcs_write_reg(channel, data, AST_LPC_ODR_CH(channel));    
}

void 
ast_kcs_interrupt_enable_user (u8 ch_num)
{
  if(ch_num < AST_KCS_CHANNEL_NUM)
  {
        kcs_enabled_by_user[ch_num] = TRUE;
        ast_kcs_enable_interrupt(ch_num);
  }
}

void
ast_kcs_interrupt_disable_user (u8 ch_num)
{
  if(ch_num < AST_KCS_CHANNEL_NUM)
  {
        kcs_enabled_by_user[ch_num] = FALSE;
        ast_kcs_disable_interrupt(ch_num);
  }
}

static void ast_kcs_set_obf_status(u8 channel)
{
    /*Setting obf bit based on the platform*/
#if 0
    u8 status=0;

    ast_kcs_read_status (channel, &status);
    status=status | 0x01;
    ast_kcs_write_status (channel, status);
#endif
}


static kcs_hal_operations_t ast_kcs_hw_ops = {
    .num_kcs_ch = ast_kcs_num_ch,
    .enable_kcs_interrupt = ast_kcs_enable_interrupt,
    .disable_kcs_interrupt = ast_kcs_disable_interrupt,
    .read_kcs_status = ast_kcs_read_status,
    .write_kcs_status = ast_kcs_write_status,
    .read_kcs_command = ast_kcs_read_command,
    .read_kcs_data_in = ast_kcs_read_data,
    .write_kcs_data_out = ast_kcs_write_data,
    .kcs_interrupt_enable_user = ast_kcs_interrupt_enable_user,
    .kcs_interrupt_disable_user = ast_kcs_interrupt_disable_user,
    .kcs_set_obf_status = ast_kcs_set_obf_status
};

static hw_hal_t ast_kcs_hw_hal = {
    .dev_type = EDEV_TYPE_KCS,
    .owner = THIS_MODULE,
    .devname = AST_KCS_DRIVER_NAME,
    .num_instances = AST_KCS_CHANNEL_NUM,
    .phal_ops = (void *) &ast_kcs_hw_ops
};

static irqreturn_t ast_kcs_irq_handler(int irq, void *dev_id)
{
    uint32_t reg;
    int ch=0;
    int ret;
    uint8_t status;
    int handled=0;
#if defined(GROUP_AST2300) || defined(GROUP_AST2300_PLUS)
    uint32_t lpc_hicrb_reg;
#endif

#if defined(GROUP_AST1070_COMPANION)
    switch(irq)
    {
        case IRQ_BMCCC0_N1_KCS:
            ch=AST_KCS_CHAN_NUM_EACH_NODE*0;
            break;

        case IRQ_BMCCC0_N2_KCS:
            ch=AST_KCS_CHAN_NUM_EACH_NODE*1;
            break;

        case IRQ_BMCCC0_N3_KCS:
            ch=AST_KCS_CHAN_NUM_EACH_NODE*2;
            break;

        case IRQ_BMCCC0_N4_KCS:
            ch=AST_KCS_CHAN_NUM_EACH_NODE*3;
            break;

#if (CONFIG_SPX_FEATURE_NUM_BMC_COMPANION_DEVICES >= 2)
        case IRQ_BMCCC1_N1_KCS:
            ch=AST_KCS_CHAN_NUM_EACH_NODE*4;
            break;

        case IRQ_BMCCC1_N2_KCS:
            ch=AST_KCS_CHAN_NUM_EACH_NODE*5;
            break;

        case IRQ_BMCCC1_N3_KCS:
            ch=AST_KCS_CHAN_NUM_EACH_NODE*6;
            break;

        case IRQ_BMCCC1_N4_KCS:
            ch=AST_KCS_CHAN_NUM_EACH_NODE*7;
            break;
#endif /* CONFIG_SPX_FEATURE_NUM_BMC_COMPANION_DEVICES >= 2 */

        default:
            printk(KERN_WARNING "%s: Unknow Interrupt Number %d\n", AST_KCS_DRIVER_NAME, irq);
    }
#endif

    reg = ast_kcs_read_reg(ch, AST_LPC_HICR6);

    if (reg & (AST_LPC_HICR6_SNP0_STR | AST_LPC_HICR6_SNP1_STR)) { /* snoop interrupt is occured */
        return IRQ_NONE; /* handled by snoop driver */
    }

#if defined(GROUP_AST2300) || defined(GROUP_AST2300_PLUS)
    reg = ast_kcs_read_reg(ch, LPC_iBTCR2);
    if (reg & LPC_iBTCR2_H2BI)
    {
        return IRQ_NONE;
    }
#endif
    reg = ast_kcs_read_reg(ch, AST_LPC_HICR2);

    if (reg & (AST_LPC_HICR2_LRST | AST_LPC_HICR2_SDWN | AST_LPC_HICR2_ABRT)) { /* LRESET | SDWN | ABRT interrupt is occured */
        return IRQ_NONE; /* handled by LPC-reset driver */
    }
	
    #if defined(GROUP_AST2300) || defined(GROUP_AST2300_PLUS)
    lpc_hicrb_reg = ast_kcs_read_reg(ch,AST_LPC_HICRB);

    if ( (reg & (AST_LPC_HICR2_IBFIE2 | AST_LPC_HICR2_IBFIE1 | AST_LPC_HICR2_IBFIE3)) || (lpc_hicrb_reg & AST_LPC_HICRB0_KCS4INTE) )
    #else
    if (reg & (AST_LPC_HICR2_IBFIE2 | AST_LPC_HICR2_IBFIE1 | AST_LPC_HICR2_IBFIE3))
    #endif
    {
    	for (ch = 0; ch < AST_KCS_CHANNEL_NUM; ch ++) {
        	ast_kcs_read_status(ch, &status);
	        if (status & 0x02) { /* Command or Data_In register has been written by system-side software */
			if( kcs_core_ops ) {
        		    	handled = 1;
	        		ret = kcs_core_ops->process_kcs_intr(ast_kcs_hal_id, ch);
			    	if (ret != 0) {
        		        	printk(KERN_WARNING "KCS core IRQ handler failed\n");
            			}
        		}	
		}
        	/* when the KCS core IRQ handler reads IDR, IDF is cleared automatically */
	}
    }
    else{
	handled = 0;
    }

    return (handled == 1) ? IRQ_HANDLED : IRQ_NONE;
}

static void ast_kcs_init_hw(void)
{
    uint32_t reg;
    int ch;
    u8 ch_num, status;
    
#ifndef CONFIG_SPX_FEATURE_GLOBAL_ENABLE_LPC_TO_AHB_BRIDGE
    reg = ast_kcs_read_reg(0, AST_LPC_HICR5);
    reg &= ~(0x00000500); /* clear bit 10 & 8 to disable LPC flash cycle */
    ast_kcs_write_reg(0, reg, AST_LPC_HICR5);
#endif

    for(ch = 0;ch < AST_KCS_CHANNEL_NUM; ch ++)
    {
        ast_kcs_disable_interrupt(ch);
    }
    ast_kcs_disable_channel();

    ast_kcs_configure_io_port_addr();

    /* clear OBF */
    for (ch = 0; ch < AST_KCS_CHANNEL_NUM; ch ++) {
        #if defined(GROUP_AST2300) || defined(GROUP_AST2300_PLUS)
        if(ch == 3)
        {
            reg = ast_kcs_read_reg(ch, AST_LPC_STR4);
            reg &= ~AST_LPC_STR_OBFA;
            ast_kcs_write_reg(ch , reg, AST_LPC_STR4);
        }
        else
        #endif
        {        
            reg = ast_kcs_read_reg(ch, AST_LPC_STR_CH(ch));
            reg &= ~AST_LPC_STR_OBFA;
            ast_kcs_write_reg(ch, reg, AST_LPC_STR_CH(ch));
        }
    }
    for (ch_num = 0; ch_num < AST_KCS_CHANNEL_NUM; ++ch_num)
    {
        ast_kcs_read_status (ch_num, &status);
        status = status | ERROR_STATE;
        ast_kcs_write_status (ch_num, status);
    }

    ast_kcs_enable_channel();
    //ast_kcs_enable_interrupt();
}

static int ast_kcs_module_init(void)
{
    int ret;

	extern int kcs_core_loaded;
	if (!kcs_core_loaded)
			return -1;

    ast_kcs_hal_id = register_hw_hal_module(&ast_kcs_hw_hal, (void **) &kcs_core_ops);
    if (ast_kcs_hal_id < 0) {
        printk(KERN_WARNING "%s: register KCS HAL module failed\n", AST_KCS_DRIVER_NAME);
        return ast_kcs_hal_id;
    }

#if defined(GROUP_AST1070_COMPANION)
    ret = request_irq(IRQ_BMCCC0_N1_KCS, ast_kcs_irq_handler, IRQF_SHARED, AST_KCS_DRIVER_NAME, (void *)AST_BMCCC_LPC0_VA_BASE(0));

    if (ret) {
        printk(KERN_WARNING "%s: BMCCC0_N1 request irq failed\n", AST_KCS_DRIVER_NAME);
        goto out_register_hal;
    }
	else
	{
		BMCCC0_IRQ_SET_HIGH_LEVEL (IRQ_BMCCC0_N1_KCS);
        BMCCC0_IRQ_SET_LEVEL_TRIGGER (IRQ_BMCCC0_N1_KCS);
	}

    ret = request_irq(IRQ_BMCCC0_N2_KCS, ast_kcs_irq_handler, IRQF_SHARED, AST_KCS_DRIVER_NAME, (void *)AST_BMCCC_LPC1_VA_BASE(0));

    if (ret) {
        printk(KERN_WARNING "%s: BMCCC0_N2 request irq failed\n", AST_KCS_DRIVER_NAME);
        goto out_request_BMCCC0_N1_KCS;
    }
	else
	{
		BMCCC0_IRQ_SET_HIGH_LEVEL (IRQ_BMCCC0_N2_KCS);
        BMCCC0_IRQ_SET_LEVEL_TRIGGER (IRQ_BMCCC0_N2_KCS);
	}

    ret = request_irq(IRQ_BMCCC0_N3_KCS, ast_kcs_irq_handler, IRQF_SHARED, AST_KCS_DRIVER_NAME, (void *)AST_BMCCC_LPC2_VA_BASE(0));

    if (ret) {
        printk(KERN_WARNING "%s: BMCCC0_N3 request irq failed\n", AST_KCS_DRIVER_NAME);
        goto out_request_BMCCC0_N2_KCS;
    }
	else
	{
		BMCCC0_IRQ_SET_HIGH_LEVEL (IRQ_BMCCC0_N3_KCS);
        BMCCC0_IRQ_SET_LEVEL_TRIGGER (IRQ_BMCCC0_N3_KCS);
	}

    ret = request_irq(IRQ_BMCCC0_N4_KCS, ast_kcs_irq_handler, IRQF_SHARED, AST_KCS_DRIVER_NAME, (void *)AST_BMCCC_LPC3_VA_BASE(0));

    if (ret) {
        printk(KERN_WARNING "%s: BMCCC0_N4 request irq failed\n", AST_KCS_DRIVER_NAME);
        goto out_request_BMCCC0_N3_KCS;
    }
	else
	{
		BMCCC0_IRQ_SET_HIGH_LEVEL (IRQ_BMCCC0_N4_KCS);
        BMCCC0_IRQ_SET_LEVEL_TRIGGER (IRQ_BMCCC0_N4_KCS);
	}

#if (CONFIG_SPX_FEATURE_NUM_BMC_COMPANION_DEVICES >= 2)
    ret = request_irq(IRQ_BMCCC1_N1_KCS, ast_kcs_irq_handler, IRQF_SHARED, AST_KCS_DRIVER_NAME, (void *)AST_BMCCC_LPC0_VA_BASE(1));

    if (ret) {
        printk(KERN_WARNING "%s: BMCCC1_N1 request irq failed\n", AST_KCS_DRIVER_NAME);
        goto out_request_BMCCC0_N4_KCS;
    }
	else
	{
		BMCCC1_IRQ_SET_HIGH_LEVEL (IRQ_BMCCC1_N1_KCS);
        BMCCC1_IRQ_SET_LEVEL_TRIGGER (IRQ_BMCCC1_N1_KCS);
	}

    ret = request_irq(IRQ_BMCCC1_N2_KCS, ast_kcs_irq_handler, IRQF_SHARED, AST_KCS_DRIVER_NAME, (void *)AST_BMCCC_LPC1_VA_BASE(1));

    if (ret) {
        printk(KERN_WARNING "%s: BMCCC1_N2 request irq failed\n", AST_KCS_DRIVER_NAME);
        goto out_request_BMCCC1_N1_KCS;
    }
	else
	{
		BMCCC1_IRQ_SET_HIGH_LEVEL (IRQ_BMCCC1_N2_KCS);
        BMCCC1_IRQ_SET_LEVEL_TRIGGER (IRQ_BMCCC1_N2_KCS);
	}

    ret = request_irq(IRQ_BMCCC1_N3_KCS, ast_kcs_irq_handler, IRQF_SHARED, AST_KCS_DRIVER_NAME, (void *)AST_BMCCC_LPC2_VA_BASE(1));

    if (ret) {
        printk(KERN_WARNING "%s: BMCCC1_N3 request irq failed\n", AST_KCS_DRIVER_NAME);
        goto out_request_BMCCC1_N2_KCS;
    }
	else
	{
		BMCCC1_IRQ_SET_HIGH_LEVEL (IRQ_BMCCC1_N3_KCS);
        BMCCC1_IRQ_SET_LEVEL_TRIGGER (IRQ_BMCCC1_N3_KCS);
	}

    ret = request_irq(IRQ_BMCCC1_N4_KCS, ast_kcs_irq_handler, IRQF_SHARED, AST_KCS_DRIVER_NAME, (void *)AST_BMCCC_LPC3_VA_BASE(1));

    if (ret) {
        printk(KERN_WARNING "%s: BMCCC1_N4 request irq failed\n", AST_KCS_DRIVER_NAME);
        goto out_request_BMCCC1_N3_KCS;
    }
	else
	{
		BMCCC1_IRQ_SET_HIGH_LEVEL (IRQ_BMCCC1_N4_KCS);
        BMCCC1_IRQ_SET_LEVEL_TRIGGER (IRQ_BMCCC1_N4_KCS);
	}
#endif /* CONFIG_SPX_FEATURE_NUM_BMC_COMPANION_DEVICES >= 2 */

#else
    IRQ_SET_LEVEL_TRIGGER(AST_KCS_IRQ);
    IRQ_SET_HIGH_LEVEL(AST_KCS_IRQ);

    ret = request_irq(AST_KCS_IRQ, ast_kcs_irq_handler, IRQF_SHARED, AST_KCS_DRIVER_NAME, (void *)AST_KCS_REG_VA_BASE);

    if (ret) {
        printk(KERN_WARNING "%s: request irq failed\n", AST_KCS_DRIVER_NAME);
        goto out_register_hal;
    }
#endif

    ast_kcs_init_hw();

    return 0;

#if defined(GROUP_AST1070_COMPANION)
#if (CONFIG_SPX_FEATURE_NUM_BMC_COMPANION_DEVICES >= 2)
out_request_BMCCC1_N3_KCS:
    free_irq(IRQ_BMCCC1_N3_KCS, (void *)AST_BMCCC_LPC2_VA_BASE(1));
out_request_BMCCC1_N2_KCS:
    free_irq(IRQ_BMCCC1_N2_KCS, (void *)AST_BMCCC_LPC1_VA_BASE(1));
out_request_BMCCC1_N1_KCS:
    free_irq(IRQ_BMCCC1_N1_KCS, (void *)AST_BMCCC_LPC0_VA_BASE(1));
out_request_BMCCC0_N4_KCS:
    free_irq(IRQ_BMCCC0_N4_KCS, (void *)AST_BMCCC_LPC3_VA_BASE(0));
#endif /* CONFIG_SPX_FEATURE_NUM_BMC_COMPANION_DEVICES >= 2 */
out_request_BMCCC0_N3_KCS:
    free_irq(IRQ_BMCCC0_N3_KCS, (void *)AST_BMCCC_LPC2_VA_BASE(0));
out_request_BMCCC0_N2_KCS:
    free_irq(IRQ_BMCCC0_N2_KCS, (void *)AST_BMCCC_LPC1_VA_BASE(0));
out_request_BMCCC0_N1_KCS:
    free_irq(IRQ_BMCCC0_N1_KCS, (void *)AST_BMCCC_LPC0_VA_BASE(0));
#endif
out_register_hal:
    unregister_hw_hal_module(EDEV_TYPE_KCS, ast_kcs_hal_id);

    return ret;
}

static void ast_kcs_module_exit(void)
{
#if defined(GROUP_AST1070_COMPANION)
    free_irq(IRQ_BMCCC0_N4_KCS, (void *)AST_BMCCC_LPC3_VA_BASE(0));
    free_irq(IRQ_BMCCC0_N3_KCS, (void *)AST_BMCCC_LPC2_VA_BASE(0));
    free_irq(IRQ_BMCCC0_N2_KCS, (void *)AST_BMCCC_LPC1_VA_BASE(0));
    free_irq(IRQ_BMCCC0_N1_KCS, (void *)AST_BMCCC_LPC0_VA_BASE(0));
#if (CONFIG_SPX_FEATURE_NUM_BMC_COMPANION_DEVICES >= 2)
    free_irq(IRQ_BMCCC1_N4_KCS, (void *)AST_BMCCC_LPC3_VA_BASE(1));
    free_irq(IRQ_BMCCC1_N3_KCS, (void *)AST_BMCCC_LPC2_VA_BASE(1));
    free_irq(IRQ_BMCCC1_N2_KCS, (void *)AST_BMCCC_LPC1_VA_BASE(1));
    free_irq(IRQ_BMCCC1_N1_KCS, (void *)AST_BMCCC_LPC0_VA_BASE(1));
#endif /* CONFIG_SPX_FEATURE_NUM_BMC_COMPANION_DEVICES >= 2 */
#else
    free_irq(AST_KCS_IRQ, (void *)AST_KCS_REG_VA_BASE);
#endif
    unregister_hw_hal_module(EDEV_TYPE_KCS, ast_kcs_hal_id);
}

module_init(ast_kcs_module_init);
module_exit(ast_kcs_module_exit);

MODULE_AUTHOR("American Megatrends Inc.");
MODULE_DESCRIPTION("AST2100/2050/2200/2150/2300/2400/1050/1250 KCS controller driver");
MODULE_LICENSE("GPL");
