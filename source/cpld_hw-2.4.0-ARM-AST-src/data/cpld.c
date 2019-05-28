/****************************************************************
 **                                                            **
 **    (C)Copyright 2006-2009, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross              **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
****************************************************************/

#include <linux/init.h>
#include <linux/module.h>
#include "jtag.h"
#include "jtag_ioctl.h"
#include "ast_jtag.h"
#include "lattice.h"

extern uint32_t get_device_idcode (unsigned long *);
extern uint32_t hw_device_ctl(unsigned int, unsigned long *, unsigned long); 
extern int jbcmain(int argc, char **argv);


static jtag_hw_device_operations_t cpld_hw_ops = {
	.get_hw_device_idcode = get_device_idcode,
	.set_hw_device_ctl = hw_device_ctl,
	.jtag_io_ctl=jtag_io,
#ifdef CONFIG_SPX_FEATURE_ALTERA_SUPPORT
	.jbcmain_proc=jbcmain,
#else
	.jbcmain_proc=NULL,
#endif //CONFIG_SPX_FEATURE_ALTERA_SUPPORT
};


int cpld_hw_init(void)
{
	
	// Register JTAG Hardware Device Operation
	if (0 > register_jtag_hw_device_ops(&cpld_hw_ops))
		printk ("Failed to register CPLD Hardware Driver\n");
	else
		printk ("The CPLD Hardware Driver is loaded successfully\n");
	
	return 0;
}


void cpld_hw_exit(void)
{
	unregister_jtag_hw_device_ops();
	printk ( "Exit the CPLD Hardware Driver sucessfully\n");
	return;
}

module_init(cpld_hw_init);
module_exit(cpld_hw_exit);

MODULE_AUTHOR("American Megatrends Inc.");
MODULE_DESCRIPTION("CPLD Hardware Driver");
MODULE_LICENSE ("GPL");
