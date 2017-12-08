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
 * Filename : fileio.h
 * 
 * File description : File read - write functions 
 *
 *  Author: Manish Tomar <manisht@amiindia.co.in> 
 ******************************************************************/
#ifndef _WS_FILE_IO
#define _WS_FILE_IO

#include "WS_Include.h"

extern char *readTextFromFile(char *fileName);
extern int writeTextToFile(char *filename, const char *text);

#endif // _WS_FILE_IO
