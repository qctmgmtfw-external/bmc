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
#ifndef __UNIXCRYPT__
#define __UNIXCRYPT__

//send a pointer to buffer to hold unix hashed password into this function
#ifdef __cplusplus
extern "C" {
#endif


#define DEFAULT_SALT            "$1$A17c6z5w"
#define STD_UNIX_HASH_LEN       34
#define MAX_CHALLENGE_LEN       32
#define STD_DIGEST_LEN          16


void unixcrypt(const char *cleartextpw, const char *salt,char* unix_hashed_result);
void CreateChallengeDigestFromClearText(unsigned char* Challenge,char* cleartext,unsigned char* ResultDigest);
void CreateChallengeDigestFromUnixHash(unsigned char* Challenge,char* unix_hashed_result,unsigned char* ResultDigest);
    


#ifdef __cplusplus
}
#endif

#endif

