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

#ifndef BTIFC_H
#define BTIFC_H
     
/* bt ioctl data  */
typedef struct
{
	unsigned char 	btifcnum;	/* BT Interface on which the ioctl should act upon */

}  __attribute__((packed)) bt_data_t;
      
#endif	/* BTIFC_H */
