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
 * Filename : WS_Parser.h
 * 
 * File description : SOAP Parsing functions. 
 *
 *  Author: 	Akbar Ali. A <akbaralil@ami.com>
 * 				Yogeswaran T <yogeswarant@amiindia.co.in> 
 ******************************************************************/

#ifndef _WS_PARSER_H_
#define _WS_PARSER_H_

#include "WS_Structures.h"
#include "WS_Include.h"
#include "WS_LinkedList.h"

#include "tree.h"



/*@
Define
@*/
#define PARAM_COUNT		10
#define MIN_ENVELOPE_SIZE 8192	


#define XSI "http://www.w3.org/2001/XMLSchema-instance"


// WSMAN-Spec: ResourceURI and SelectorSet & Selector tags namespace URI
#define WSMAN	 		"http://schemas.dmtf.org/wbem/wsman/1/wsman.xsd"
#define WSMAN1	 		"http://schemas.xmlsoap.org/ws/2005/06/management"

// SOAP1.2: Envelope, Header & Body tags namespace URI
#define SOAP			"http://www.w3.org/2003/05/soap-envelope"

// WS-Addressing: To, ReplyTo, FaultTo, Action, MessageID & RelatesTo tags namespace URI
#define WSA		 	"http://schemas.xmlsoap.org/ws/2004/08/addressing"

//WS-Eventing: Subscribe, Renew, Unsubscribe, SubscribeManager, SubscribeEnd, Delivery, NotifyTo, EndTo, Filter, Expires
#define WSE			"http://schemas.xmlsoap.org/ws/2004/08/eventing"

//WS-Enumeration:
#define WSEN			"http://schemas.xmlsoap.org/ws/2004/09/enumeration"

/* WS-Transfer */
#define WXF 			"http://schemas.xmlsoap.org/ws/2004/09/transfer"

//WS-Security: UserNameToken, UserName, Password
#define WSSE			"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd"

//WS-Trust: RequestedSecurityToken, IssuedTokens
#define WST			"http://schemas.xmlsoap.org/ws/2005/02/trust"

//WS-Policy: AppliesTo
#define WSP			"http://schemas.xmlsoap.org/ws/2004/09/policy"

/* WS-CIM Binding */
#define WSMB		"http://schemas.dmtf.org/wbem/wsman/1/cimbinding.xsd"

// CIM Common Namespace 
#define WSCIM_COMMON "http://schemas.dmtf.org/wbem/wscim/1/common"

// WS-Identity
#define WSMID		"http://schemas.dmtf.org/wbem/wsman/identity/1/wsmanidentity.xsd"

// WS-Capability - Integral part of WS-Identity
#define WSC			"http://schemas.dmtf.org/wbem/wsman/1/capability.xsd"

// WS-Trasfer
#define ACTION_GET	 		"http://schemas.xmlsoap.org/ws/2004/09/transfer/Get"
#define ACTION_PUT	 		"http://schemas.xmlsoap.org/ws/2004/09/transfer/Put"
#define ACTION_CREATE	 	"http://schemas.xmlsoap.org/ws/2004/09/transfer/Create"
#define ACTION_DELETE	 	"http://schemas.xmlsoap.org/ws/2004/09/transfer/Delete"

// Eventing
#define SUBSCRIBE_EVENT 	"http://schemas.xmlsoap.org/ws/2004/08/eventing/Subscribe"
#define RENEW_EVENT			"http://schemas.xmlsoap.org/ws/2004/08/eventing/Renew"
#define UNSUBSCRIBE_EVENT 	"http://schemas.xmlsoap.org/ws/2004/08/eventing/Unsubscribe"

// Enumeration
#define ENUMERATE			"http://schemas.xmlsoap.org/ws/2004/09/enumeration/Enumerate"
#define PULL				"http://schemas.xmlsoap.org/ws/2004/09/enumeration/Pull"
#define RELEASE				"http://schemas.xmlsoap.org/ws/2004/09/enumeration/Release"


#define LOG_FILE_PATH		CFG_PROJ_TEMP_CONFIG_PATH "/ParserLog.log"

#define MUST_UNDERSTAND	"mustUnderstand"


#define OPERATION_TIMEOUT 	-2

long g_Operation_Timeout;

typedef struct URInPrefix{
	char nsURI[URI_LEN];
	char prefix[PREFIX_LEN];
}URInPrefix;


typedef struct header_tags
{
	char *Tag;
	int uMustunderstandstatus;	
}HeaderTags;

/*@
Externs
@*/


/*@
Function Declarations
@*/

/*Main module for SOAP Parser*/
//WS_INT16 WSSoapParser(char* soapRequest,struct SOAPStructure *soap, xmlNode **rootNode);
WS_INT16 WSSoapParser(char* soapRequest,struct SOAPStructure *soap/*, xmlNode **rootNode*/);
long WS_ParseExpiryTime( char *ExpiryTime );

/*Display all the elemets in the SOAP Message*/
#ifdef DEBUG
void WSDisplay_structure(struct SOAPStructure *);
#endif

/*Parsing Selector set*/
SelectorSet * parseSelectorSetNode(xmlNode *selectorSetNode);

/*Parsing the WSMAN_EPR*/
WSMAN_EPR * parseWSMAN_EPRNode(xmlNode *eprNode, WS_BOOLEAN checkEPR);

/*Return the bodt type*/
WS_INT16 WSReturnBodyType(char *string);

#ifdef DEBUG
WS_INT16 printSelectorSet(SelectorSet ss);
#endif //DEBUG

WS_INT16 WSFreeEPR(EPR *epr);

WS_BOOLEAN WSIsWsmanURI(char *pURI);

/*@
Global Variables
@*/

FILE *ParserLog;


#define setFaultReturnFailure(ptr, faultCode, str) {  if(!ptr) { SetFault(faultCode);SetFaultTextDetail(str); return FAILURE; } } 
#define setFaultReturnNULL(ptr, faultCode, str) {  if(!ptr) { SetFault(faultCode;SetFaultTextDetail(str); return NULL;} }

#endif
