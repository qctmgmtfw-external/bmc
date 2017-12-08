/*
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2006-2009, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 */
/*****************************************************************
*
* Filename: parse-ex.c
*
* Description: Extended version of Ini Parser. Provides simpler and
*              more fuctions to handle INI files
*
******************************************************************/
#include <stdio.h>
#include "iniparser.h"
#include "parse-ex.h"


INI_HANDLE 
IniLoadFile(char *filename)
{
	dictionary *d = NULL;
	FILE *fp;


	/* Check if file Exists and if does not exist create a new one */
	fp = fopen(filename,"rb");
	if (fp != NULL)
	{
		fclose(fp);
	}
	else
	{
//		printf("[INFO] LoadIniFile: %s Does not exist.Creating New file ...\n",filename);
		printf("Creating new %s ...\n",filename);
		fp = fopen(filename,"wb");
		if (fp == NULL)
		{
			printf("[ERROR] LoadIniFile: %s creation Failed\n",filename);
			return NULL;
		}
		fclose(fp);

	}

	/* Open the Ini File */
   	d = iniparser_load(filename);
	if (d == NULL)
	{
		printf("[ERROR] LoadIniFile: %s open  Failed\n",filename);
		return NULL;
				
	}
	return (INI_HANDLE)d;
}

void
IniCloseFile(INI_HANDLE handle)
{
	dictionary_del((dictionary*)(handle));
	return;
}

void
IniAddSection(INI_HANDLE handle, char *Section)
{
	dictionary_set((dictionary*)handle, Section, NULL);
   	return;
}

void
IniDelSection(INI_HANDLE handle, char *Section)
{
	dictionary *d = (dictionary *)handle;
    	char FullSec[MAX_STRSIZE];	
    	int i;

    	sprintf(FullSec,"%s:",Section);
    	/* Scan all keys belong to section and delete them */
    	for (i=0 ; i<d->size ; i++) 
    	{
        	if (d->key[i]==NULL)
            	continue ;
	
		if (strncmp(d->key[i],FullSec,strlen(FullSec)) == 0)
			dictionary_unset(d,d->key[i]);
   	}

   	dictionary_unset(d,Section);
    	return;
}



void 
IniAddEntry(INI_HANDLE handle,char *Section,  char * Key, char *Val)
{

	char FullKey[MAX_STRSIZE];	

    	if (handle ==NULL || Key==NULL || Section == NULL)
        	return;

    	sprintf(FullKey,"%s:%s",Section,Key);

	/* Will add the Section if not exists */
    	dictionary_set((dictionary *)handle,Section,NULL);
	
	/* Remove the old key if exists and  add new */
    	dictionary_unset((dictionary *)handle,FullKey);
    	dictionary_set((dictionary *)handle,FullKey,Val);
    	return;
	
}

void
IniDelEntry(INI_HANDLE handle, char *Section, char *Key)
{

    	char FullKey[MAX_STRSIZE];	

    	if (handle ==NULL || Key==NULL || Section == NULL)
        	return;

    	sprintf(FullKey,"%s:%s",Section,Key);

    	dictionary_unset((dictionary *)handle,FullKey);
	return;
}

char *
IniGetEntry(INI_HANDLE handle, char *Section, char *Key)
{

    	char FullKey[MAX_STRSIZE];	

    	if (handle ==NULL || Key==NULL || Section == NULL)
        	return NULL;

    	sprintf(FullKey,"%s:%s",Section,Key);

    	return dictionary_get((dictionary *)handle,FullKey,NULL);
}

void
IniSetStr(INI_HANDLE handle, char *Section, char *Key, char *Val)
{

	IniAddEntry(handle,Section,Key,Val);
	return;
}

void
IniSetUInt(INI_HANDLE handle, char *Section, char *Key, unsigned long Val)
{
    	char Str[MAX_STRSIZE];
	sprintf(Str,"%lu",Val);	
	IniAddEntry(handle,Section,Key,Str);
	return;
}

void
IniSetSInt(INI_HANDLE handle, char *Section, char *Key, long Val)
{
    	char Str[MAX_STRSIZE];
	sprintf(Str,"%ld",Val);	
	IniAddEntry(handle,Section,Key,Str);
	return;
}

void
IniSetBool(INI_HANDLE handle, char *Section, char *Key, int Bool)
{
	if (Bool)
		IniAddEntry(handle,Section,Key,"TRUE");
	else
		IniAddEntry(handle,Section,Key,"FALSE");
	return;
}

void
IniSetChar(INI_HANDLE handle, char *Section, char *Key, char Val)
{
	char Str[2];
	Str[0]=Val;
	Str[1]=0;
	IniAddEntry(handle,Section,Key,Str);
	return;
}




char*
IniGetStr(INI_HANDLE handle, char *Section, char *Key, char *Def)
{

	char *s = IniGetEntry(handle,Section,Key);
	if (s == NULL)
		return Def;
	return s;
}

long
IniGetSInt(INI_HANDLE handle, char *Section, char *Key, long  Def)
{

	char *s = IniGetEntry(handle,Section,Key);
	if (s == NULL)
		return Def;
	return strtol(s,NULL,0);
}

unsigned long
IniGetUInt(INI_HANDLE handle, char *Section, char *Key, unsigned long  Def)
{

	char *s = IniGetEntry(handle,Section,Key);
	if (s == NULL)
		return Def;
	return strtoul(s,NULL,0);
}


int
IniGetBool(INI_HANDLE handle, char *Section, char *Key, int Def)
{

	char *s = IniGetEntry(handle,Section,Key);
	if (s == NULL)
		return Def;
	if (strlen(s) == 1)
	{
   	 	if (s[0]=='y' || s[0]=='Y' || s[0]=='1' || s[0]=='t' || s[0]=='T') 
			return 1;
   	 	if (s[0]=='n' || s[0]=='N' || s[0]=='0' || s[0]=='f' || s[0]=='F') 
			return 0;
	}
	if (strcasecmp(s,"YES") == 0 || strcasecmp(s,"TRUE"))
			return 1;
	if (strcasecmp(s,"NO") == 0 || strcasecmp(s,"FALSE"))
			return 0;

    	return Def;
}

char
IniGetChar(INI_HANDLE handle, char *Section, char *Key, char Def)
{

	char *s = IniGetEntry(handle,Section,Key);
	if (s == NULL)
		return Def;
	return s[0];
}

int
IniSaveFile(INI_HANDLE handle,char *filename)
{
    	FILE *fp;
	char bkp_filename[MAX_STRSIZE];

	/* Save the INI in a backup file. So during failure (poweroff/reset) 
	   during write will not corrupt original file */
	sprintf(bkp_filename,"%s%s",filename,".tmp");
    	fp = fopen(bkp_filename,"w");
    	if(fp == NULL)
    	{
		printf("[ERROR] SaveIniFile: %s open  Failed\n",filename);
        	return -1;
    	}
    	iniparser_dump_ini((dictionary *)handle,fp);
    	fclose(fp);

	/* Now rename backup file to original file - This is a atmoic
           operation of changing the inode link. So depending upon the 
	   time of failure the file will be intact either with old or new
	   contents */
	rename(bkp_filename,filename);

    	return 0;
}

void
IniDump(INI_HANDLE handle)
{
   	printf("\n#-----------------------------------------------------------------------\n");
    	iniparser_dump_ini((dictionary *)handle,stdout);
   	printf("\n#-----------------------------------------------------------------------\n");
}

/**************************************************************************************************/

		
