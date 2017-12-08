/****************************************************************
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2002-2003, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        6145-F, Northbelt Parkway, Norcross,                **
 **                                                            **
 **        Georgia - 30071, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 ****************************************************************/

#ifndef __USERMAPIS_H__
#define __USERMAPIS_H__

#include "icc_what.h"

#include <exterrno.h>
#include "coreTypes.h"
#include "usermdefs.h"
#include "stdgrps.h"

#pragma pack(1)



/*****Structures*********/


/**a brief idea**
1) An admin can change anyones password. It is not reasonable to expect that he knows
the old password in order to change the password for a user
2) Any user can change his own password when he is logged in. This has to be
3) If he is logged in he logged in by typing the existing password and therefore
the can change to a new password. 
4) This is just like Windows..An administrator can change anybodys password
but a user can change his own password only when he is logged in

1)if the password is hashed we need to give all hashed passwords
2) if the passwd is clear we wil convert it here..suggested method for now
*/


#define UI_FMASK_CLEAR_PWDS            (0x00000000)
#define UI_FMASK_HASHED_PWDS           (0x00000001)

#define UI_FMASK_PWD_NO_CHANGE         (0x00000100) //set if password is not changed
   




typedef struct tag_userinfo
{
    uint32 fieldmask;

    CHAR username[MAX_USER_NAME_LEN+1];    
    uint32 uid;
    
    union
    {
        struct
        {
            CHAR passwd[MAX_PASSWORD_LEN_CLEAR+1]; //we allocate the max reqd for a salted password
        } clearpwds; //set this if password is clear
        struct
        {
            CHAR unixhash[MAX_PASSWORD_LEN_HASHED+1];
            CHAR webhash[MAX_PASSWORD_LEN_HASHED+1];
            CHAR snmphash[MAX_PASSWORD_LEN_HASHED+1];
        }hashedpwds; //set these if passwd is hashed already
    }pwds;
    
    CHAR gecos[MAX_GECOS_LEN+1];
    CHAR reserved[32]; //do not use

    //permission structure for std groups..value 0 indicates no membership
    //value 1 indicates Read Only membership
    //value 2 indicates Read and Write Mebership
    #define USERM_PERMS_NONMEMBER 0
    #define USERM_PERMS_RDONLY    1
    #define USERM_PERMS_RDWR      2
    uint8 StdGrpPerms[RAC_GRP_NUM_END];
    long passwdExpiry;
}
USER_INFO;





/*******Function prototypes**********/

/**User existence APIS**/
BOOL DoesUserExist(char* username);
BOOL DoesGroupExist(char* grpname);
BOOL DoesUserExistInShadow(char* username);
BOOL DoesUserExistInGroup(char* username,char* grpname);
uint8 GetUserPermsInGroup(char* username,char* grpname);



/**No of users, groups**/
int GetNumUsers();
int GetNumGroups();
int GetNumUsersInGroup(char* grpname,int* nCount);


/**Get people**/
int GetAllMembersOfGroup(char* grpname,char** asciiz,int* nNumMembers);
int GetAllUserNames(char** asciiz,int* nNumUsers);
int GetAllGroupNames(char** asciiz,int* nNumGroups);
int GetUserInfo(USER_INFO* ui);


/**operations like add, del and membership**/
int AddNewUser(USER_INFO* ui);
int DeleteUser(char* username);
int AddNewGroup(char* grpname);
int DeleteGroup(char* grpname);
int AddUserToGroup(char* username,char* grpname);
int DeleteUserFromGroup(char* username,char* groupname);
int ChangeUserInfo(USER_INFO *ui);
BOOL IsAdmin(char* username);
int pppDialInAddNewUser( char *usrName, char *clearPassword );
int pppDialInDelUser( char *usrName );
int pppDialInEditUserPasswd( char *userName, char *password );


int AuthenticateWebUser(char* username,char* webpass);

/**********General helpers********/
char** GetStdGroupNames();
int ModifyBasedOnPerms(char* username,char* grpname,uint8 Perms);
int GetUsersUnixCryptPasswd(char* username,char* unix_hashed_pwd);
void PreInitUsermOps();
void CloseUsermOps();


#pragma pack()


#endif
