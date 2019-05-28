/*
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2006-2009, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 */
 
#ifndef __MCTP_PCIE_H__
#define __MCTP_PCIE_H__

#include "mctppcieifc.h"

typedef struct 
{
	int (*downstream_mctp_pkt)(mctp_iodata *io);
	int (*upstream_mctp_pkt)(mctp_iodata * io);
	int (*enable_mctp_pkt)(void);
} mctppcie_hal_operations_t;

typedef struct
{
	int (*process_pcie_intr) (int dev_id, unsigned char ch_num);
	void (*wakeup_pcie_intr_queue)(void);
} pcie_core_funcs_t;


#endif
