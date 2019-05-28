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

#ifndef KCSIFC_H

#define KCSIFC_H
    #define KCS_IOC_MAGIC 				'K'
      
      /* kcs ioctl data  */
      
      typedef struct
      {
      	unsigned char 	kcsifcnum;	/* KCS Interface on which the ioctl should act upon */
      
      }  __attribute__((packed)) kcs_data_t;
      
#endif	/* KCSIFC_H */
