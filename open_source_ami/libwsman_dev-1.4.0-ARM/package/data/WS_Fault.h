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
 * Filename : WS_Fault.h
 * 
 * File description : Generating WS-Man fault  
 *
 *  Author: Yogeswaran T <yogeswarant@amiindia.co.in> 
 ******************************************************************/
#ifndef _WS_FAULT_H_
#define _WS_FAULT_H_

#include "WS_Include.h"
#include "WS_Structures.h"

typedef enum Fault_Subcode{
							WSMAN_ACCESS_DENIED, 
							WSA_ACTION_NOT_SUPPORTED, 
							WSMAN_ALREADY_EXISTS,
							WSEN_CANNOT_PROCESS_FILTER,
							WSMAN_CONCURRENCY,
							WSE_DELIVERY_MODE_REQUESTED_UNAVAILABLE,
							WSMAN_DELIVERY_REFUSED,
							WSA_DESTINATION_UNREACHABLE,
							WSMAN_ENCODING_LIMIT,
							WSA_ENDPOINT_UNAVAILABLE,
							WSE_EVENT_SOURCE_UNABLE_TO_PROCESS,
							WSEN_FILTER_DIALECT_REQUESTED_UNAVAILABLE,
							WSE_FILTERING_NOT_SUPPORTED,
							WSEN_FILTERING_NOT_SUPPORTED,
							WSE_FILTERING_REQUESTED_UNAVAILABLE,
							WSMAN_INTERNAL_ERROR,
							WSMAN_FRAGMENT_DIALECT_NOT_SUPPORTED,
							WSMAN_INVALID_BOOKMARK,
							WSEN_INVALID_ENUMERATION_CONTEXT,
							WSE_INVALID_EXPIRATION_TIME,
							WSEN_INVALID_EXPIRATION_TIME,
							WSE_INVALID_MESSAGE,
							WSA_INVALID_MESSAGE_INFORMATION_HEADER,
							WSMAN_INVALID_OPTIONS,
							WSMAN_INVALID_PARAMETER,
							WXF_INVALID_REPRESENTATION,
							WSMAN_INVALID_SELECTORS,
							WSA_MESSAGE_INFORMATION_HEADER_REQUIRED,
							WSMAN_NO_ACK,
							WSMAN_QUOTA_LIMIT,
							WSMAN_SCHEMA_VALIDATION_ERROR,
							WSEN_TIMED_OUT,
							WSMAN_TIMED_OUT,
							WSE_UNABLE_TO_RENEW,
							WSE_UNSUPPORTED_EXPIRATION_TYPE,
							WSEN_UNSUPPORTED_EXPIRATION_TYPE,
							WSMAN_UNSUPPORTED_FEATURE,
							WSMAN_NOTUNDERSTOOD_FAULT,
							WSMAN_INVALID_HEADER,
							WSMB_POLYMORPHISM_MODE_NOT_SUPPORTED,
							WSMB_CIM_EXCEPTION,
							WSE_EVENTSOURCE_UNABLE_TO_PROCESS
						  }FaultSubcode;

typedef struct _Fault
{
	char actionURI[URI_LEN];
    char code[NAME_LEN];
    FaultSubcode subcode;
    char subcodeText[NAME_LEN];
    char reason[STR_LEN];
    char detail[FAULT_DETAIL_LEN];

	char Qname[NAME_LEN];
    char Prefix[NAME_LEN];
    char NamespaceURI[URI_LEN];
	
	xmlDoc *xmlDocument;
	xmlNode *headerNode;
	xmlNode *actionNode;
	xmlNode *faultNode;
	xmlNode *faultCodeNode;
	xmlNode *faultSubCodeNode;
	xmlNode *faultReasonNode;
	xmlNode *faultDetailNode;
}Fault;

#define WSMAN_FAULT_ACTION_URI 			"http://schemas.dmtf.org/wbem/wsman/1/wsman/fault"
#define WSA_FAULT_ACTION_URI			"http://schemas.xmlsoap.org/ws/2004/08/addressing/fault"
#define WSEN_FAULT_ACTION_URI			"http://schemas.xmlsoap.org/ws/2004/09/enumeration/fault"
#define WSE_FAULT_ACTION_URI			"http://schemas.xmlsoap.org/ws/2004/08/eventing/fault"
#define WXF_FAULT_ACTION_URI			"http://schemas.xmlsoap.org/ws/2004/09/transfer/fault"
#define CIM_BINDING_FAULT_ACTION_URI		"http://schemas.dmtf.org/wbem/wsman/1/cimbinding/fault"

#define SOAP_SENDER				"s:Sender"
#define SOAP_RECEIVER				"s:Receiver"
#define SOAP_MUSTUNDERSTAND			"s:MustUnderstand"

#define NO_OF_RECORDS					42

#define FAULT_INVALID_NAMESPACE "http://schemas.dmtf.org/wbem/wsman/1/wsman/faultDetail/InvalidNamespace"
#define FAULT_EXPIRATION_TIME "http://schemas.dmtf.org/wbem/wsman/1/wsman/faultDetail/ExpirationTime"
#define FAULT_ADDRESSING_MODE "http://schemas.dmtf.org/wbem/wsman/1/wsman/faultDetail/AddressingMode"
#define FAULT_ACTION_MISMATCH "http://schemas.dmtf.org/wbem/wsman/1/wsman/faultDetail/ActionMismatch"
#define FAULT_OPERATION_TIMEOUT "http://schemas.dmtf.org/wbem/wsman/1/wsman/faultDetail/OperationTimeout"
#define FAULT_INVALID_TIMEOUT "http://schemas.dmtf.org/wbem/wsman/1/wsman/faultDetail/InvalidTimeout"
#define FAULT_MAXIMUM_ENVELOPE_SIZE "http://schemas.dmtf.org/wbem/wsman/1/wsman/faultDetail/MaxEnvelopeSize"
#define FAULT_SERVICE_ENVELOPE_LIMIT "http://schemas.dmtf.org/wbem/wsman/1/wsman/faultDetail/ServiceEnvelopeLimit"
#define FAULT_MINIMUM_ENVELOPE_LIMIT "http://schemas.dmtf.org/wbem/wsman/1/wsman/faultDetail/MinimumEnvelopeLimit"
#define FAULT_MAXIMUM_ENVELOPE_SIZE_UNREPORTABLE_SUCCESS "http://schemas.dmtf.org/wbem/wsman/1/wsman/faultDetail/UnreportableSuccess"
#define FAULT_INVALID_PARAMETER  "http://schemas.dmtf.org/wbem/wsman/1/wsman/faultDetail/InvalidName" 
#define FAULT_INVALID_RESOURCE_URI "http://schemas.dmtf.org/wbem/wsman/1/wsman/faultDetail/InvalidResourceURI"
#define FAULT_AMBIGOUS_SELECTORS "http://schemas.dmtf.org/wbem/wsman/1/wsman/faultDetail/AmbiguousSelectors"
#define FAULT_UNEXPECTED_SELECTORS "http://schemas.dmtf.org/wbem/wsman/1/wsman/faultDetail/UnexpectedSelectors"
#define FAULT_INVALID_VALUE "http://schemas.dmtf.org/wbem/wsman/1/wsman/faultDetail/InvalidValue"
#define FAULT_INSUFFICIENT_SELECTORS "http://schemas.dmtf.org/wbem/wsman/1/wsman/faultDetail/InsufficientSelectors"
#define FAULT_TYPE_MISMATCH "http://schemas.dmtf.org/wbem/wsman/1/wsman/faultDetail/TypeMismatch"
#define FAULT_DUPLICATE_SELECTORS "http://schemas.dmtf.org/wbem/wsman/1/wsman/faultDetail/DuplicateSelectors"
#define FAULT_ENUMERATION_MODE "http://schemas.dmtf.org/wbem/wsman/1/wsman/faultDetail/EnumerationMode"
#define FAULT_FRAGMENT_UNSUPPORTED "http://schemas.dmtf.org/wbem/wsman/1/wsman/faultDetail/FragmentLevelAccess"
#define FAULT_MISSING_VALUES "http://schemas.dmtf.org/wbem/wsman/1/wsman/faultDetail/MissingValues"

WS_INT16 SetFault2Default(void);
WS_INT16 SetFault(FaultSubcode faultSubcode);
void SetFaultWSMANTextDetail(char *pstrText);
void SetFaultTextDetail(char *pstrText);
WS_INT16 WS_CreateEmptyFault();
WS_INT16 GenerateFaultResponse(struct SOAPStructure *soapStruct,char **soapResponse);
WS_INT16 WSFreeFault(void);

extern Fault *g_Fault;

#endif
