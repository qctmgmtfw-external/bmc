/*****************************************************************
 *****************************************************************
 ***                                                            **
 ***    (C)Copyright 2005-2006, American Megatrends Inc.        **
 ***                                                            **
 ***            All Rights Reserved.                            **
 ***                                                            **
 ***        6145-F, Northbelt Parkway, Norcross,                **
 ***                                                            **
 ***        Georgia - 30071, USA. Phone-(770)-246-8600.         **
 ***                                                            **
 *****************************************************************
 *****************************************************************
 ******************************************************************
 *
 * support.h
 * supported commands Macros
 *
 * Author: shivaranjanic <shivaranjanic@amiindia.co.in>
 *
 ******************************************************************/
#ifndef __OEMCOMMANDLIST_H__ 
#define __OEMCOMMANDLIST_H__


#define OEM_COMMANDS_ENABLE     1


/*----------------- Oem  Commands ---------------------------*/
#define  EXAMPLE_1    OemExample1
#define EXAMPLE_2  	 OemExample2
#define EXAMPLE_3     UNIMPLEMENTED



#if OEM_COMMANDS_ENABLE != 1
#undef EXAMPLE_1
#undef EXAMPLE_2
#undef EXAMPLE_3
#define EXAMPLE_1     UNIMPLEMENTED
#define EXAMPLE_2    UNIMPLEMENTED
#define EXAMPLE_3     UNIMPLEMENTED
#endif // OEM_COMMANDS_ENABLE != 1




#endif // __OEMCOMMANDLIST_H__ 
