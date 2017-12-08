/*****************************************************************
******************************************************************
***                                                            ***
***        (C)Copyright 2008, American Megatrends Inc.         ***
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
* Filename: ipmi_userifc_private.h
*
* Description: Private defines and structures used by IPMI/linux user
*
* Author: Andrew McCallum
*
******************************************************************/
#ifndef IPMI_USERIFC_PRIVATE_H_
#define IPMI_USERIFC_PRIVATE_H_

#include <stdint.h>

/* Handling for packing structs */
#ifdef __GNUC__
#define PACKED __attribute__ ((packed))
#else
#define PACKED
#pragma pack( 1 )
#endif


/* User ID constants */
#define IPMI_ANON_USER      ( 1 )
#define IPMI_ROOT_USER      ( 2 )
#define LINUX_ANON_USER     ( (uid_t)500 )
#define LINUX_ROOT_USER     ( (uid_t)0 )
#define LINUX_USER_BASE     ( (uid_t)501 )
#define LINUX_USER_MAX_PASSWORD_LEN  (64)

#define PASSWD_BUF_SIZE     ( 2048 )

/* IPMI command info */
#define IPMI_USERID_MASK    ( 0x3f )
#define IPMI_USEROP_MASK    ( 0x03 )
#define OP_DISABLE_USER     ( 0x00 )
#define OP_ENABLE_USER      ( 0x01 )
#define OP_SET_PASSWORD     ( 0x02 )
#define OP_TEST_PASSWORD    ( 0x03 )

struct IPMI_SetUserName_Command
{
    /* [7:6] - Reserved, [5:0] - UserID */
    uint8_t UserID;

    /* ASCII user name string.  Unused bytes should be set to NULL */
    char UserName[ 16 ];
} PACKED;


struct IPMI_GetUserName_Command
{
    /* [7:6] - Reserved, [5:0] - UserID */
    uint8_t UserID;
} PACKED;


struct IPMI_SetUserPassword_Command
{
    /* [7] - Password size, [6] - Reserved, [5:0] - UserID */
    uint8_t UserID;
    
    /* [7:2] - Reserved, [1:0] - Operation */
    uint8_t Operation;
    
    /* Max 64 bytes long - unused bytes set to NULL */
    char Password[ LINUX_USER_MAX_PASSWORD_LEN + 1 ];
} PACKED;



#ifndef __GNUC__
#pragma pack( )
#endif

#endif
