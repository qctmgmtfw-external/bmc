#ifndef __AUTHENTICATE_H__
#define __AUTHENTICATE_H__

#include "coreTypes.h"
#include "usermdefs.h"
#include "hashers.h" //for common defines and hasher stuff
#include "stdgrps.h"

/******************************************Limits*******************************************/

#define MAX_SERVICE_NAME_LEN 20 //terminating 0 not included


/*******************************************************************************************/






/**********************************Service Type Enumerations*******************************/
/*There could be deviatiosn between service types and standard groups
Service types may be more detailed for e.g: within CURI (which has only one grp)
there may be curi local, curi nw etc*/
#if 0
typedef enum _ServiceType
{
    RAC_SERVICE_TYPE_RESERVED = 0,
    RAC_SERVICE_TYPE_WEB,
    RAC_SERVICE_TYPE_VIDEO,
    RAC_SERVICE_TYPE_CURINW,
    RAC_SERVICE_TYPE_CURILOC,
    RAC_SERVICE_TYPE_CURIUSB,
    RAC_SERVICE_TYPE_SSH
}
RAC_SERVICE_TYPE;
#endif
/* RAC_SERVICE_TYPE moved to rac_numbers.h  */
#include "rac_numbers.h"

/*******************************************************************************************/




/*************************************Session info structure********************************/

typedef struct _session_info
{
    /****Generic information for all services*****/
    UCHAR ServiceName[MAX_SERVICE_NAME_LEN+1];
    UCHAR UserName[MAX_USER_NAME_LEN+1];
    RAC_SERVICE_TYPE  ServiceType; //a numeric service indicator


    /***states****/
    BOOL IsValidHandle; //is this structure valid?
    BOOL IsAuth; //has this session already passed authentication
    BOOL IsUserNameValid;
    BOOL IsClientSuppliedDigestValid;



    /***User Permissions****/
    BOOL HasWrite; //user for this session has write permission
    BOOL HasRead;  //user for this session has read permission
    BOOL IsAdmin;  //is the user for this session root


    /***Challenge****/
    //make it a 16 byte challenge
    UCHAR  Challenge[MAX_CHALLENGE_LEN];
    UCHAR  ClientSuppliedDigest[STD_DIGEST_LEN];



    /***Service specific storage ..can be extended by various services***/
    union
    {
        struct _websession
        {
            char WebPasswd[MAX_PASSWORD_LEN_HASHED+1];
        }WebSvc;
        struct _snmpsession
        {
        }SnmpSvc;
        struct curisession
        {
        }CuriSvc;
        struct OSDsession
        {
			char OSDPasswd[MAX_PASSWORD_LEN_HASHED+1];
		}OSDSvc;
        struct _vidsession
        {
            char VidPasswd[MAX_PASSWORD_LEN_HASHED+1];
        }VidSvc;
    }Service;
}
SESSION_INFO;


//Session handle is just a pointer to session info
typedef SESSION_INFO* SESSION_HANDLE;


/*Convenience macros fro this structure..enables easy access to service specific structures*/
#define SESSION_INFO_WEB(x)   ((x)->Service.WebSvc)
#define SESSION_INFO_SNMP(x)  ((x)->Service.SnmpSvc)
#define SESSION_INFO_CURI(x)  ((x)->Service.CuriSvc)
#define SESSION_INFO_OSD(x)  ((x)->Service.OSDSvc)


/*Macros for validity checking*/
#define SESSION_ASSERT_VALID(x) TCRIT_ASSERT( ((x != NULL) && ((x)->IsValidHandle == 1)),"Invalid Session Handle\n" )


/*******************************************************************************************/




/********************************Externally used macros and functions************************/



/**************************************Permission check macros*******************************/

//pass a session handle here.
#define HAS_PERMS_WRITE(x)((x)->HasWrite)
#define HAS_PERMS_READ(x) ((x)->HasRead)
#define IS_ADMIN(x)       ((x)->IsAdmin)

/**These macros can be used where hSession is a parameter
   to the function and is called hSession**/
/**This is useful because people can then use the same macros
whether in web interface or CURI interface.Otherwise if each
one passes hSession differenyl a different macro has to be used
everywhere..So any interface should pass hSession as preferably
the first parameter..even if it is part of some other structure
there is no harm in passing the session handle inside it as the
value for hSession..see web for such an example**/

#define PERMCHK_HAS_PERMS_WRITE() HAS_PERMS_WRITE((hSession))
#define PERMCHK_HAS_PERMS_READ()  HAS_PERMS_READ((hSession))
#define PERMCHK_IS_ADMIN()        IS_ADMIN((hSession))

/*******************************************************************************************/




/**************************************Function Prototypes***********************************/

SESSION_HANDLE SESSION_Init(RAC_SERVICE_TYPE stype);
SESSION_HANDLE SESSION_Duplicate(SESSION_HANDLE destination, SESSION_HANDLE source);
void SESSION_SetUserName(SESSION_HANDLE hSession,char* username);
void SESSION_Close(SESSION_HANDLE hSession);
void SESSION_GenerateChallenge(SESSION_HANDLE hSession);
void SESSION_SetClientSuppliedDigest(SESSION_HANDLE hSession,UCHAR* ClientSuppliedDigest);

int AuthenticateUser(SESSION_HANDLE hSession);


//used only by WEB ..meaningless for other services
int WebAuthenticateCacheRefresh();

/*******************************************************************************************/



/*******************************************************************************************/


#endif //__AUTHENTICATE_H__
