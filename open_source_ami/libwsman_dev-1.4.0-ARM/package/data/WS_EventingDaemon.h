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
 * Filename : WS_EventingDaemon.h
 * 
 * File description : The Eventing Daemon  
 *
 *  Author: Yogeswaran T <yogeswarant@amiindia.co.in> 
 ******************************************************************/
#ifndef _WS_EVENTINGDAEMON_H
#define _WS_EVENTINGDAEMON_H


#include "WS_Fault.h"
#include "WS_LinkedList.h"
#include "WS_Parser.h"

#define LISTENING_PORT 555
#define SHORT_STR_LEN	12
#define MILLI_SECS 1000
#define BLOCK_LEN 1024
#define ADDITIONAL_BLOCK 512

// _SC - String constant MACROs
#define _SC_SYSTEM_CREATION_CLASSNAME 		"SystemCreationClassName"
#define _SC_SYSTEM_NAME			   			"SystemName"
#define _SC_CREATION_CLASSNAME			   	"CreationClassName"
#define _SC_NAME						   	"Name"
#define _SC_STRING					   		"string"	
#define _SC_QUERY							"Query"
#define _SC_QUERY_LANGUAGE					"QueryLanguage"
#define _SC_CIM_INDICATIONFILTER			"CIM_IndicationFilter"
#define _SC_CIM_COMPUTERSYSTEM				"CIM_ComputerSystem"
#define _SC_CIM_INDICATION_HANDLER_CIMXML	"CIM_IndicationHandlerCIMXML"
#define _SC_LOCALHOST_LOCALDOMAIN			"localhost.localdomain"

#define _PUSH_DELV_MODE_URI		"http://schemas.xmlsoap.org/ws/2004/08/eventing/DeliveryModes/Push"
#define _PULL_DELV_MODE_URI		"http://schemas.dmtf.org/wbem/wsman/1/wsman/Pull"

/* These are subscription related structures,  there should not be any pointers used in these structures bcoz these will
   have to passed from one process to the another */

#define MAX_EVENT_LEN 1024*5
typedef struct Auth_Info {

	char	SecProfile[URI_LEN];	/* security profile URI */
	char 	Username[STR_LEN];	/* username */
	char 	Password[STR_LEN];	/* password */

}AuthInfo;

typedef struct _subs_pull_req {
   int contextNumber;
   int identifier;
}SubsPull_Req;

typedef struct _unsubscribe_req{
   int identifier;
}Unsubscribe_Req;

typedef struct _subs_resp {
   int identifier;
}Subcription_Resp;

typedef struct _renew_resp {
   int status;            // SUCCESS or FAILURE 
}Renew_Resp;

typedef struct _unsubscribe_resp {
   int status;            // SUCCESS or FAILURE 
}Unsubscribe_Resp;

typedef struct _subs_pull_resp{
   char itemsXML[MAX_EVENT_LEN];   
}SubsPull_Resp;


typedef struct _subsEPR {
   char address[URI_LEN];    /* address URI */
   char refProps[BUFF_SIZE];    /* This will have reference properties - along with prefix and namespace declaration */
   char refParams[BUFF_SIZE];    /* This will have prefix with namespace declaration */

/*	xmlNode *refPropsNode;
	xmlNode *refParamsNode;*/
}SubsEPR;

typedef struct _subsSelectorSet {
   char selector_name[10][128];
   char selector_value[10][256];
   int no_Of_selector;
}SubsSelectorSet; 

enum DeliveryMode {_PUSH_DELV_MODE  = 100, _PULL_DELV_MODE};

typedef struct _subscribe_req 
{
	
  char username[256];
  char password[256];
   SubsEPR        NotifyTo;        /* Where to send notifications EPR (end point reference), 
									we dont use the normal epr here bcoz we cannot have pointers */
   SubsEPR        EndTo;            /* Where to send subscription end EPR, we dont use the normal epr here
                                  bcoz we cannot have pointers */
   long    expiryTime;    /* time at which subscription will expire */
   char strExpiryTime[NAME_LEN];  /* Expiry time as string */   
   int        retryCount;    /* number of times connection to the client has be tried while
                          sending notification */
   int        retryDelay;    /* number of seconds specifying how long to wait between retries
                          while trying to connect */
   enum DeliveryMode mode; /* Delivery mode, either PUSH or PULL */
   char    EventActionURI[URI_LEN];/* event action URI to be used while delivering that kind of event */
   SubsSelectorSet     select; /* This selector select will have a copy of the selector select given in the
                                SOAP header.  Here the Selector set is special type specially meant for
                                eventing type bcoz we cannot have pointers */
   AuthInfo    Auth_Info;        /* Authentication information used while initiating a secure
                          connection to the client during notification */

   char resourceURI[URI_LEN];
   char serverURI[URI_LEN]; /* This should the address given in the SubscriptionManager EPR */
   
   char strFiltQuery[STR_LEN];
}Subscribe_Req;


typedef struct _subscribe_resp {
	int identifier;	
	enum DeliveryMode mode;
	int contextNumber;
}Subscribe_Resp;

typedef struct _subscription_info{
	Subscribe_Req subscription;
	int identifier;	
	LinkedList *pEvents;
	int contextNumber;
	CIMC_InstanceName *pFiltInstName;
	CIMC_InstanceName *pHandlerInstName;
	CIMC_InstanceName *pSubsInstName;
}SubscriptionInfo;

typedef struct _renew_req {
   int identifier;
   long expiryTime;
   char strExpiryTime[NAME_LEN];  /* Expiry time as string */
}Renew_Req;

enum message_type { _SUBSCRIBE_MSG = 5, _RENEW_MSG, _UNSUBSCRIBE_MSG, _PULL_MSG, _FAILURE_MSG , _DESTROY_MSG};
typedef enum message_type message_t;
 
typedef struct _eventing_req {
	message_t m_type;
	union {
		Subscribe_Req subsreq;
		Renew_Req renewreq;
		Unsubscribe_Req unsubsreq;
		SubsPull_Req pullreq;
	}eventing_req;

}Eventing_Req;

typedef struct _eventing_resp {
	message_t m_type;
	FaultSubcode subCode;
	union {
		Subscribe_Resp subsresp;
		Renew_Resp renewresp;
		Unsubscribe_Resp unsubsresp;
		SubsPull_Resp pullresp;
	}eventing_resp;

}Eventing_Resp;

extern LinkedList *g_SubsList;
WS_INT16 StartEventingDaemon(void);
WS_INT16 InitSubscriptionList(void);
WS_INT16 InitEventingDaemon(int readpipefd, int writepipefd);
WS_INT16 DestroyEventingDaemon(void);
void WSFreeSubscribe_Req(Subscribe_Req *pSubsReq);
CIMC_ObjectWithPath *ExecuteAssociators(void *handle, char *pStrAssocClass, 
							char *pResultClass, 
							CIMC_InstanceName *pInstanceName);
#if defined(CFG_PROJ_QOM_DAEMON_YES) || defined(CFG_PROJ_QOM_LIBRARY_YES) 
void *EventingThreadFunc(void *thread_input);
#endif
#endif //_WS_EVENTINGDAEMON_H
