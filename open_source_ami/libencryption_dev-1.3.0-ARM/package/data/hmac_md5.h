
/****************************************************************
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2005-2006, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        6145-F, Northbelt Parkway, Norcross,                **
 **                                                            **
 **        Georgia - 30071, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 *****************************************************************
 *
 *   hmac_md5 .h   
 *   This Code is taken from RFC 2104 (RFC2104)
 *
 *
 *****************************************************************/

void hmac_md5 ( unsigned char *key, int key_len,unsigned char *text, int text_len, unsigned char *digest, int length);

