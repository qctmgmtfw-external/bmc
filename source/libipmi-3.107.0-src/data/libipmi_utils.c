/*****************************************************************
******************************************************************
***                                                            ***
***  (C)Copyright 2009-2015, American Megatrends Inc.          ***
***                                                            ***
***                    All Rights Reserved                     ***
***                                                            ***
***       5555 Oakbrook Parkway, Norcross, GA 30093, USA       ***
***                                                            ***
***                     Phone 770.246.8600                     ***
***                                                            ***
******************************************************************
******************************************************************
******************************************************************
*
* Filename: libipmi_utils.c
*
* Descriptions: contains utility functions
*
* Author: Rajamanickam T 
*
******************************************************************/
#include <stdio.h>
#include <openssl/rand.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "libipmi_session.h"


int Generate_random_number(unsigned char *random_number,int len)
{
	unsigned char buf[10240]={0};
	int fd=0;
	memset ((void *)buf, 0, sizeof(buf));
	/*Sample seed for RAND_seed*/
	fd = open ("/dev/urandom", O_RDONLY);
	if (fd < 0)
	{
		printf ("Security.c 132: Unable to open random device\n");
		return -1;
	}
	read (fd, buf, sizeof(buf));
	close(fd);
	((void (*)(const void *, int ))g_CryptoHandle[CRYPTO_SEED])(buf, sizeof(buf));
	if( ((int (*)(unsigned char *, int ))g_CryptoHandle[CRYPTO_BYTE])(random_number, len) < 1 )
	{
		printf ("Security.c 140: Generating Random Number Failed\n");
		return -1;
	}
	return 0;
}
