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
 * Filename : WS_Structures.h
 * 
 * File description : WSMAN structures 
 *
 *  Author: 	Arun Venkatachalam <arunv@ami.com>
 *  			Akbar Ali A <akbaralil@ami.com> 
 *  			Yogeswaran T <yogeswarant@amiindia.co.in>   
 ******************************************************************/
#ifndef _WS_STRUCTURES_H_
#define _WS_STRUCTURES_H_

#include "WS_Include.h"
#include "WS_WSDLParser.h"
#include "WS_LinkedList.h"

#include "tree.h"
#include "CIMC_Include.h"

/*Storing SOAP Response Body information*/

typedef struct SOAP_Body SOAP_Body_Response;

/*Storing SelectorSet information including selector name & value*/
typedef struct Selector_set
{
	WS_CHAR **selector_name;
	WS_CHAR **selector_value;
	int no_Of_selector;
	struct WSMAN_EPR **eprs; /* this will be array of WSMAN_EPR*, size being no_Of_selector,
								will be NULL if selector value is simple text else,
								will point to a valid address of WSMAN_EPR structure */
}SelectorSet;

/* Following structure represents a end point reference */

typedef struct epr {

	char address[URI_LEN];	/* address URI */
	xmlNode *refPropsNode; /* reference properties */
	xmlNode *refParamsNode; /* reference parameters */
}EPR;

typedef struct WSMAN_EPR
{
	char address[STR_LEN];
	char resourceURI[URI_LEN];
	SelectorSet selectorSet;

}WSMAN_EPR;


/*Storing SOAP Request Header Information*/
typedef struct SOAP_Header
{
	WS_CHAR To[URI_LEN];			// Where to send the SOAP message
	WS_CHAR ResourceURI[URI_LEN];		// To identify the resource
	WS_CHAR Action[URI_LEN];		// Type of action like Get,Put,Create or Delete
	SelectorSet select;			// Identify the operation from set of operation
	WS_CHAR MessageID[URI_LEN];		// Unique identifier for each SOAP message
	
	EPR ReplyTo;		// Where to send the response
	
	WS_CHAR ReplyToMustUnderstand;
	
	EPR From;			// Address of the client machine
	
	WS_CHAR FromMustUnderstand;
	
	EPR FaultTo;		// Where to send the fault message
	
	WS_CHAR FaultToMustUnderstand;
	WS_CHAR OperationTimeOut[NAME_LEN];	// Maximum time limit for each sending response
	int maxEnvelopeSize;	
	WS_CHAR wseIdentifier[LEN];
		// Unique identifier for each event subscribe message
}SOAPHeader;

/*Storing SOAP Response Header Information*/
typedef struct SOAP_Header_response
{
	EPR To;
	WS_CHAR RelatesTo[URI_LEN];
	EPR From;

}ResponseSOAPHeader;


enum BodyType {TRANSFER_GET_REQ=1, TRANSFER_PUT_REQ, TRANSFER_CREATE_REQ, TRANSFER_DELETE_REQ, 
			   ENUMERATE_REQ, ENUMERATE_PULL_REQ, ENUMERATE_RELEASE_REQ,
			   SUBSCRIBE_REQ,RENEW_REQ,UNSUBSCRIBE_REQ,
			   INVOKEMETHOD_REQ,CUSTOM_OPERATION,
			   IDENTIFY_REQ};

typedef struct SOAP_Body_Element
{
	void *body; // this will be pointing to Enumerate_Req if bodyType is ENUMERATE_REQ
				// this will be pointing to Subscription_Req if the bodyType is SUBSCRIBE_REQ

}SOAP_Body_Element;


/* Filter structure */
typedef struct CIM_ExecuteQueryFilter
{
// 	char *filterDialect;
	
	// CIM Query
	char CIM_Query[ENUM_MODE_LEN];
	
}CIM_ExecuteQueryFilter;

enum filterMode {ASSOCIATED_INSTS, ASSOCIATION_INSTS};
typedef enum filterMode FilterMode;

typedef struct 
{	
	FilterMode assocFilterMode;
	WSMAN_EPR object;
	char associatedClassName[CIM_NAME_LEN];
	char role[CIM_NAME_LEN];
	char resultClassName[CIM_NAME_LEN];
	char resultRole[CIM_NAME_LEN];
	int nIncludeResultPropertyCount;
	char **includeResultProperty;

}AssociationFilter;


typedef struct 
{
	SelectorSet *selectorSet;	
}WS_SelectorFilter;


typedef struct
{
	char filterDialect[DIALECT_LEN];
	enum { F_ASSOC, F_SELECTOR, F_EXEC_QUERY } e_FilterType;
	union {
		AssociationFilter *assocFilter;
		WS_SelectorFilter *selectorFilter;
		CIM_ExecuteQueryFilter *execQueryFilter;
	}filter;
}WS_Filter;


/* Following structure which encapsulates all the information about a enumeration PULL request*/
typedef struct enum_pull
{
	WS_CHAR contextNumber[STR_LEN];
	WSMAN_EPR  EPR;
	int identifier;
}EnumPullReq;

/* Following structure which encapsulates all the information about a enumeration RELEASE request*/
typedef struct enum_release
{
	WS_CHAR contextNumber[STR_LEN];
	WSMAN_EPR  EPR;
}EnumReleaseReq;

/* Main Structure for SOAP Message */
struct SOAPStructure
{	
	SOAPHeader header;		// Request SOAP header information
	SOAP_Body_Element bodyElement;
	enum BodyType bodyType;
	
	// Header & Body xmlNode	
	ResponseSOAPHeader respHeader;	// Response SOAP header information	
	EnumPullReq enumPull;		// Enumeration PULL Request information	
	EnumReleaseReq enumRelease; // Enumeration Release Request information
	WS_Filter *pFilter;			// Filter structure

	xmlNode *operationNode;			// Instance node (i.e) first child of body	
};



enum enumMode { ENUMERATE_NONE, ENUMERATE_EPR, ENUMERATE_OBJECT_AND_EPR };
typedef enum enumMode EnumMode;

enum PolymorphismMode { NONE, INCLUDESUBCLASSPROPERTY, EXCLUDESUBCLASSPROPERTY };

typedef struct Enumerate_Req
{
	WSMAN_EPR *epr;
	EnumMode enumMode;
	WS_BOOLEAN optimizedEnumeration;
	int maxElements;
	WS_Filter *filter ;
	xmlNode *itemsNode;
	int eos;
	enum PolymorphismMode e_PolymorphismMode;
		
}Enumerate_Req;





/* Structure to store the resource information */
typedef struct _Resource_
{
	char resourceURI[URI_LEN];	/* ResourceURI */  
	char libName[NAME_LEN]; 	/* Library Name */	
	void *lib_handle;
	WSDL *wsdl;	
	char wsdlRef[NAME_LEN];
	char WSDLFileNameWithPath[NAME_LEN];
		
}WS_Resource;


enum Property_Type {SIMPLE_TYPE  = 1,DATETIME_TYPE,EPR_TYPE };

typedef struct Property
{
	char propertyName[PROPNAME_LEN];
	union{
		WSMAN_EPR *EPR;
		char simpleValue[STR_LEN];
	}propertyValue;
	enum Property_Type e_PropertyType;
}Property;


#endif
