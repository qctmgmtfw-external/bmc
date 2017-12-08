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
* Filename: parse-ex.h
*
* Description: Extended version of Ini Parser. Provides simpler and
*              more fuctions to handle INI files
*
******************************************************************/
typedef void (*INI_HANDLE);
#define MAX_STRSIZE 2048

INI_HANDLE IniLoadFile(char *filename);
int   IniSaveFile(INI_HANDLE handle,char *filename);
void  IniCloseFile(INI_HANDLE handle);
void  IniDump(INI_HANDLE handle);

void  IniAddSection(INI_HANDLE handle, char *Section);
void  IniDelSection(INI_HANDLE handle, char *Section);

void  IniAddEntry(INI_HANDLE handle,char *Section,  char * Key, char *Val);
void  IniDelEntry(INI_HANDLE handle, char *Section, char *Key);
char* IniGetEntry(INI_HANDLE handle, char *Section, char *Key);

void  IniSetStr (INI_HANDLE handle, char *Section, char *Key, char *Val);
void  IniSetUInt(INI_HANDLE handle, char *Section, char *Key, unsigned long Val);
void  IniSetSInt(INI_HANDLE handle, char *Section, char *Key, long Val);
void  IniSetBool(INI_HANDLE handle, char *Section, char *Key, int Bool);
void  IniSetChar(INI_HANDLE handle, char *Section, char *Key, char Val);

char* IniGetStr(INI_HANDLE handle, char *Section, char *Key, char *Def);
long  IniGetSInt(INI_HANDLE handle, char *Section, char *Key, long  Def);
unsigned long IniGetUInt(INI_HANDLE handle, char *Section, char *Key, unsigned long  Def);
int IniGetBool(INI_HANDLE handle, char *Section, char *Key, int Def);
char IniGetChar(INI_HANDLE handle, char *Section, char *Key, char Def);

/*-------------------------------------------------------------------------------------------*/
