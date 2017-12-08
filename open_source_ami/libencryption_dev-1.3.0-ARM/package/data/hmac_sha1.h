/****************************************************************
 **                                                            **
 **    (C)Copyright 2006-2009, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
****************************************************************/
#define SHA_DIGESTSIZE	20
#define SHA_BLOCKSIZE	64


void datatruncate
  (
   char*   d1,	 /* data to be truncated */
   char*   d2,	 /* truncated data */
   int	   len	 /* length in bytes to keep */
  );




void
hmac_sha1
(
 char*	  k,	 /* secret key */
 int	  lk,	 /* length of the key in bytes */
 char*	  d,	 /* data */
 int	  ld,	 /* length of data in bytes */
 char*	  out,	 /* output buffer, at least "t" bytes */
 int	  t
);

