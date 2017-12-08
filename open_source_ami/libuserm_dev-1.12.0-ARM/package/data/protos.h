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
* protos.h
*
******************************************************************/
#ifndef __PROTOS_H__
#define __PROTOS_H__

#include <sys/types.h>
#include <stdio.h>
#include <string.h>

#include <pwd.h>
#include <shadow.h>
#include <grp.h>
#include <utmpx.h> 

#include "pwio.h"
#include "shadowio.h"
#include "groupio.h"
#include "webshio.h"
//#include "clpwio.h"

#include <coreTypes.h>
#include "config.h"
#include "defines.h"

/*lock.c*/
int InitUsermOps();
int FiniUsermOps();

/*validate.c*/
BOOL IsGoodNameChars(char* name);
int IsGoodUserNameLen(char* username);
int IsGoodGroupNameLen(char* grpname);
int IsGoodGecosLen(char* gecos);
int ValidateUserName(char* username);
int ValidateGroupName(char* grpname);
int ValidatePasswdCrypted(char* cryptedpwd);
int ValidatePasswdClear(char* clearpwd);
int ValidateGecos(char* gecos);



/*helpers.c*/
int FindFreeUid(uid_t* free_uid);
int FindFreeGid(gid_t* free_gid);
void FillPwEntry(struct passwd* newent,char* username,char* passwd,char* comments,
                 uid_t uidgot,gid_t gidgot,char* homedir,char* shellname);
void FillSpwEntry(struct spwd* newspent,char* username,char* password,
                         long int lastchg,long int min,long int max,
                  long int warn,long int inactive,long int expire);
/*void FillClpwEntry( struct clpwd *new_clear_entry,
                    char *username, char *password );	*/
void FillGroupEntry(struct group* newgrpent,char* gr_name,gid_t gr_gid);
BOOL IsOnList(char* const* gr_mem,char* username);
char**  AddToList(char** list,const char* username);
char ** DeleteFromList(char **list, const char *username);
int ListCount(char* const* list);
char* ListToAsciiz(char** list,int nCount);
void FreeFullList(char** list);


int ChangeGecos(char* username,char* gecos);




/*pam-helpers.c*/
int DoPAMPasswd(const char *user, char* passwd);

/*group free helper function*/
void group_free(void *ent);
void shadow_free(void* ent);
void passwd_free(void* ent);
void webshadow_free(void* ent);
//void clpasswd_free(void* ent);

void webcrypt(char* username,char* passwd,char* realm,char* webcryptedpwd);

int WebgoShouldRefreshCache();
int ShouldWebgoRefreshCache();


#endif
