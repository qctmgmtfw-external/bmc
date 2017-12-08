/******************************************************************
 ******************************************************************
 ***                                                             **
 ***    (C)Copyright 2005-2006, American Megatrends Inc.         **
 ***                                                             **
 ***    All Rights Reserved.                                     **
 ***                                                             **
 ***    5555 , Oakbrook Pkwy, Norcross,                          **
 ***                                                             **
 ***    Georgia - 30093, USA. Phone-(770)-246-8600.              **
 ***                                                             **
 ******************************************************************
 ******************************************************************
 ******************************************************************
 *
 * IfcSupport.h
 * Loading of IPMI Interfaces dynamically
 *
 *  Author: Winston <winstonv@amiindia.co.in>
 ******************************************************************/
#ifndef _H_IFCSUPPORT_H
#define _H_IFCSUPPORT_H

#define MAX_LIB_PATH 256
#define MAX_SYM_SIZE 50

#define TERMINAL_LIB_PATH "/usr/local/lib/libipmiserialterminal.so"
#define APML_LIB_PATH "/usr/local/lib/libipmiapml.so"
#define IPMI_LIBS_PATH "/usr/local/lib/ipmi/"
#define TERMINAL_SYM_NAME "TerminalIfcTask"
#define CREATE_TASK 0x01
#define ADDTO_MSGHNDLR_TBL 0x02
#define ADDTO_GRPEXTN_TBL 0x03
#define ADD_TO_TIMERTSK_TBL 0x04

typedef struct
{
    CHAR SectionName[MAX_SYM_SIZE];
    INT8U Enabled;
    INT8U Type;
    CHAR SymbolName[MAX_SYM_SIZE];
    CHAR LibPath[MAX_LIB_PATH];
    INT8U Arg_NetFn;
    INT8U GrpExtnCode; //DCMI
}DynamicLoader_T;


/**
* @fn StartIfcTasks
* @brief Loads the IPMI Interfaces dynamically
*/
extern int StartIfcTasks(int BMCInst);
//extern DynamicLoader_T g_DynamicInfoTable[100];
//extern INT32U g_DynamicInfoTableCount;

/**
* @fn GetLibrarySymbol
* @brief Gets the symbol from the library
* @param symbolname - Name of the symbol
* @param librarypath - Library name from where the symbol has to be get
* @param funcHandle - Function handle
*/
int GetLibrarySymbol(CHAR *symbolName, CHAR *libraryPath, void **funcHandle);




#endif //_H_IFCSUPPORT_H


