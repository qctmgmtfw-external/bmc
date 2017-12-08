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
 * Filename : WSDLParser.h
 * 
 * File description : WSDL file parse 
 *
 *  Author: Manish Tomar <manisht@amiindia.co.in> 
 ******************************************************************/
#ifndef WSDLParser_H_
#define WSDLParser_H_

#include "WS_Include.h"
#include "tree.h"

#define MAX_OPERATIONS 70
#define MAX_PARAMETERS 50
#define MAX_ELEMENTS   50
#define MAX_LEN	       75

enum TYPE_IMPL { PRIMITIVE, SIMPLE, COMPLEX };

typedef struct Type
{
	enum TYPE_IMPL typeImpl;
	char name[NAME_LEN];
}Type;

typedef struct Parameter
{
	char name[NAME_LEN];
	Type *type;
	int size; 
}Parameter;

typedef struct Element
{
	char name[NAME_LEN];
	Type *type;
	unsigned int maxOccurs;
	unsigned int minOccurs;
}Element;
 
typedef struct ComplexType
{
	enum TYPE_IMPL typeImpl;
	char name[NAME_LEN];
	Element *elements[MAX_ELEMENTS];
	int elementsLength;
}ComplexType;
 

typedef struct Operation
{
	char operName[NAME_LEN];
	char actionURI[URI_LEN];
	Parameter *inputParameters[MAX_PARAMETERS];
	int ipSize;
	Parameter *outputParameters[MAX_PARAMETERS];
	int opSize;	
}Operation;

typedef struct UnresolvedElement {
	Element *element;
	char typeName[STR_LEN];
}UnresolvedElement;

typedef struct Schema
{
	Type *types[MAX_LEN];
	int typesLength;

	Element *elements[MAX_LEN];
	int elementsLength;

	// unresolved elements which are defined later
	UnresolvedElement *unresolvedElements;
	int ueLength;
}Schema;

typedef struct WSDL
{
	Operation *operations[MAX_OPERATIONS];
	Schema *schema;
}WSDL;


WS_INT16 freeSchema(Schema *schema);
extern Schema *parseSchema(xmlNode *schemaNode);
WS_INT16 parseWSDL(xmlNode *wsdlDefNode, WSDL *wsdl);
WS_INT16 printWSDL(WSDL *wsdl);
WS_INT16 printParameters(Parameter **parameters);
WS_INT16 printOperations(Operation **operations);
WS_INT16 freeOperation(Operation *operation);

#endif /*WSDLParser_H_*/
