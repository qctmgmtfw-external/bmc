/*****************************************************************
 *****************************************************************
 ***                                                            **
 ***    (C)Copyright 2005-2006, American Megatrends Inc.        **
 ***                                                            **
 ***            All Rights Reserved.                            **
 ***                                                            **
 ***        6145-F, Northbelt Parkway, Norcross,                **
 ***                                                            **
 ***        Georgia - 30071, USA. Phone-(770)-246-8600.         **
 ***                                                            **
 *****************************************************************
 *****************************************************************
 ******************************************************************
 *
 * Filename : WS_Filter.h
 * 
 * File description : Various Filter parsing. 
 *
 *  Author: Akbar Ali. A <akbaralil@ami.com> 
 ******************************************************************/

#ifndef _WS_FILTER_H_
#define _WS_FILTER_H_

/*@
Define
@*/

#define SUPPORTED_DIALECT 3
#define FILTER_DIALECT_LEN 128

#define SUPPORTED_DIALECT_START "<wsen:SupportedDialect>"
#define SUPPORTED_DIALECT_END "</wsen:SupportedDialect>"


/*@
Externs
@*/

/*@
Function Declarations
@*/

/* */
WS_BOOLEAN WSIsSupportedFilterDialect(xmlNode *FilterNode,struct SOAPStructure *soap);

/* */
WS_INT16 WSGetFilter(xmlNode *FilterNode, struct SOAPStructure *soap);

/* Parsing body for associated filter requets */
WS_Filter *WSAssociationFilterParser(char * filterDialect,xmlNode *node);

/* Parsing body for execute query filter requets */
WS_Filter *WSExecuteQueryFilterParser(char * filterDialect,xmlNode *node);

/* Parsing body for associated filter requets */
WS_Filter *WSSelectorFilterParser(char * pstrFilterDialect,xmlNode *pFilterNode);

/* Creates a copy of the filter*/
WS_INT16 WS_CopyFilter(WS_Filter *pSrcFilter , WS_Filter **pDestFilter);

/* Frees the filter Structure*/
WS_INT16 WS_FreeFilter(WS_Filter *pFilter);


/*@
Typedef
@*/

//Function pointer for filter parser
typedef WS_Filter *(*ParserFilter) (char * filterDialect,xmlNode *node);

/*@
 Global structure	
@*/

// Global structure for storing filter information
typedef struct
{
	char *filterDialect;
	ParserFilter fpFilterParser;

}WS_RegisterFilter;

/*@
Gloval Variables
@*/

extern WS_RegisterFilter g_Filter[10];

#endif

