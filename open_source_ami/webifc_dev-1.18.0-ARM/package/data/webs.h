/*
 *  webs.h -- GoAhead Web public header
 *
 * Copyright (c) GoAhead Software Inc., 1992-2000. All Rights Reserved.
 *
 *  See the file "license.txt" for information on usage and redistribution
 *
 * $Id: webs.h,v 1.2 2004/10/29 21:11:16 andrewm Exp $
 */

#ifndef _h_WEBS
#define _h_WEBS 1

/******************************** Description *********************************/

/*
 *  GoAhead Web Server header. This defines the Web public APIs.
 *  Include this header for files that contain ASP or Form procedures.
 *  Include wsIntrn.h when creating URL handlers.
 */

/********************************* Includes ***********************************/

#include    "ej.h"
#ifdef WEBS_SSL_SUPPORT
    #include    "websSSL.h"
#endif

#ifdef MEGARAC_G3_ENABLE_FILEUPLOAD
#include "multipart_handler.h"
#endif  /*  MEGARAC_G3_ENABLE_FILEUPLOAD    */

#include "libipmi_session.h"
#include "libipmi_errorcodes.h"
#include <authenticate.h>
#include <security/pam_appl.h>

/********************************** Defines ***********************************/
/*
 *  By license terms the server software name defined in the following line of
 *  code must not be modified.
 */
#define WEBS_NAME               T("GoAhead-Webs")
#define WEBS_VERSION            T("2.1.5")

#define WEBS_HEADER_BUFINC      512         /* Header buffer size */
#define WEBS_ASP_BUFINC         512         /* Asp expansion increment */
#define WEBS_MAX_PASS           32          /* Size of password */
#define WEBS_BUFSIZE            (3 * 4096)  /* websWrite max output string  Previously it was 4096 (wsman)*/
#define WEBS_MAX_HEADER         (5 * 1024)  /* Sanity check header */
#define WEBS_MAX_URL            4096        /* Maximum URL size for sanity */
#define WEBS_SOCKET_BUFSIZ      256         /* Bytes read from socket */

#define WEBS_HTTP_PORT          T("httpPort")
#define CGI_BIN                 T("cgi-bin")

/*
 *  Request flags. Also returned by websGetRequestFlags().
 */
#define WEBS_LOCAL_PAGE         0x1         /* Request for local webs page */
#define WEBS_KEEP_ALIVE         0x2         /* HTTP/1.1 keep alive */
#define WEBS_DONT_USE_CACHE     0x4         /* Not implemented cache support */
#define WEBS_COOKIE             0x8         /* Cookie supplied in request */
#define WEBS_IF_MODIFIED        0x10        /* If-modified-since in request */
#define WEBS_POST_REQUEST       0x20        /* Post request operation */
#define WEBS_LOCAL_REQUEST      0x40        /* Request from this system */
#define WEBS_HOME_PAGE          0x80        /* Request for the home page */
#define WEBS_ASP                0x100       /* ASP request */
#define WEBS_HEAD_REQUEST       0x200       /* Head request */
#define WEBS_CLEN               0x400       /* Request had a content length */
#define WEBS_FORM               0x800       /* Request is a form */
#define WEBS_REQUEST_DONE       0x1000      /* Request complete */
#define WEBS_POST_DATA          0x2000      /* Already appended post data */
#define WEBS_CGI_REQUEST        0x4000      /* cgi-bin request */
#define WEBS_SECURE             0x8000      /* connection uses SSL */
#define WEBS_AUTH_BASIC         0x10000     /* Basic authentication request */
#define WEBS_AUTH_DIGEST        0x20000     /* Digest authentication request */
#define WEBS_HEADER_DONE        0x40000     /* Already output the HTTP header */

/*
 *  URL handler flags
 */
#define WEBS_HANDLER_FIRST  0x1         /* Process this handler first */
#define WEBS_HANDLER_LAST   0x2         /* Process this handler last */


/*****************Stuff for maintaing IPMI sessions via the web*****************/
//for now max no of IPMI sessions possible on the web
#define WEBIPMI_MAX_SESSIONS 64
typedef struct _tagWEB_IPMI_SessionEntry
{
	struct timeval tv;
	IPMI20_SESSION_T IPMISession;
	pam_handle_t *pamh;
    UINT32 ExtraPrivileges; //,eant to store any extra privileges that fall out of the pruview of the IPMI stack
}
WEBIPMI_SESSION_ENTRY_T;


/******************************** WC Prototypes **********************************/
// returns the client ip of current connection in a NULL terminated string.
// that MUST NOT be freed by caller
char *wc_get_client_ip(char *wp);
WEBIPMI_SESSION_ENTRY_T *wc_get_ipmi_session(char *wp);
char *wc_get_username(char *wp);
int wc_get_port(char *wp);

/******************************** Prototypes **********************************/
extern int       websAccept(int sid, char *ipaddr, int port, int listenSid);
extern int       websAspDefine(char_t *name,
                    int (*fn)(int ejid, char* wp, int argc, char_t **argv));
extern int       websAspRequest(char* wp, char_t *lpath);
extern void      websCloseListen();
extern int       websDecode64(char_t *outbuf, char_t *string, int buflen);
extern void      websDecodeUrl(char_t *token, char_t *decoded, int len);
extern void      websDone(char* wp, int code);
extern void      websEncode64(char_t *outbuf, char_t *string, int buflen);
extern void      websError(char* wp, int code, char_t *msg, ...);
/* function websErrorMsg() made extern 03 Jun 02 BgP */
extern char_t   *websErrorMsg(int code);
extern void      websFooter(char* wp);
extern int       websFormDefine(char_t *name, void (*fn)(char* wp,
                    char_t *path, char_t *query));
extern char_t   *websGetDefaultDir();
extern char_t   *websGetDefaultPage();
extern char_t   *websGetHost();
extern char_t   *websGetHostUrl();
extern char_t   *websGetIpaddrUrl();
extern char_t   *websGetPassword();
extern int       websGetPort();
extern char_t   *websGetPublishDir(char_t *path, char_t **urlPrefix);
extern char_t   *websGetRealm();
extern int       websGetRequestBytes(char* wp);
extern char_t   *websGetRequestDir(char* wp);
extern int       websGetRequestFlags(char* wp);
extern char_t   *websGetRequestIpaddr(char* wp);
extern char_t   *websGetRequestLpath(char* wp);
extern char_t   *websGetRequestPath(char* wp);
extern char_t   *websGetRequestPassword(char* wp);
extern char_t   *websGetRequestType(char* wp);
extern int       websGetRequestWritten(char* wp);
extern char_t   *websGetVar(char* wp, char_t *var, char_t *def);
extern int       websCompareVar(char* wp, char_t *var, char_t *value);
extern void      websHeader(char* wp);
extern int       websOpenListen(int port, int retries);
extern int       websPageOpen(char* wp, char_t *lpath, char_t *path,
                    int mode, int perm);
extern void      websPageClose(char* wp);
extern int       websPublish(char_t *urlPrefix, char_t *path);
extern void      websRedirect(char* wp, char_t *url);
void 		 websRedirectAlwaysSecure(char* wp, char_t *url);
extern void      websSecurityDelete();
extern int       websSecurityHandler(char* wp, char_t *urlPrefix,
                    char_t *webDir, int arg, char_t *url, char_t *path,
                    char_t *query);
extern int       websSecurityHandlerRACTRENDS(char* wp, char_t *urlPrefix,
                    char_t *webDir, int arg, char_t *url, char_t *path,
                    char_t *query);
extern int       websSecurityHandler_SessionCookie(char* wp, char_t *urlPrefix,
                    char_t *webDir, int arg, char_t *url, char_t *path,
                    char_t *query);
extern void      websSetDefaultDir(char_t *dir);
extern void      websSetDefaultPage(char_t *page);
extern void      websSetEnv(char* wp);
extern void      websSetHost(char_t *host);
extern void      websSetIpaddr(char_t *ipaddr);
extern void      websSetPassword(char_t *password);
extern void      websSetRealm(char_t *realmName);
extern void      websSetRequestBytes(char* wp, int bytes);
extern void      websSetRequestFlags(char* wp, int flags);
extern void      websSetRequestLpath(char* wp, char_t *lpath);
extern void      websSetRequestPath(char* wp, char_t *dir, char_t *path);
extern char_t   *websGetRequestUserName(char* wp);
extern void      websSetRequestWritten(char* wp, int written);
extern void      websSetVar(char* wp, char_t *var, char_t *value);
extern int       websTestVar(char* wp, char_t *var);
extern void      websTimeoutCancel(char* wp);
extern int       websUrlHandlerDefine(char_t *urlPrefix, char_t *webDir,
                    int arg, int (*fn)(char* wp, char_t *urlPrefix,
                    char_t *webDir, int arg, char_t *url, char_t *path,
                    char_t *query), int flags);
extern int       websUrlHandlerDelete(int (*fn)(char* wp, char_t *urlPrefix,
                    char_t *webDir, int arg, char_t *url, char_t *path,
                    char_t *query));
extern int       websUrlHandlerRequest(char* wp);
extern int       websUrlParse(char_t *url, char_t **buf, char_t **host,
                    char_t **path, char_t **port, char_t **query,
                    char_t **proto, char_t **tag, char_t **ext);
extern char_t   *websUrlType(char_t *webs, char_t *buf, int charCnt);
extern int       websWrite(char* wp, char_t* fmt, ...);
extern int       websWriteBlock(char* wp, char_t *buf, int nChars);
extern int       websWriteDataNonBlock(char* wp, char *buf, int nChars);
extern int       websValid(char* wp);
extern int       websValidateUrl(char* wp, char_t *path);
extern void     websSetTimeMark(char* wp);

/*
 *  The following prototypes are used by the SSL patch found in websSSL.c
 */
extern int      websAlloc(int sid);
extern void     websFree(char* wp);
extern void     websTimeout(void *arg, int id);
extern void     websReadEvent(char* wp);

/*
 *  Prototypes for functions available when running as part of the
 *  GoAhead Embedded Management Framework (EMF)
 */
#ifdef EMF
extern void      websFormExplain(char* wp, char_t *path, char_t *query);
#endif



/*********AMI additions********/
void internal_fn_webpage_write_JSON_record(char* wp,unsigned char** FieldDesc,int nNumFields,...);

void internal_fn_webpage_rewrite_JSON_record(char* wp,char* jsonvarname,char* structvarname,int index,
                                             unsigned char** FieldDesc,int nNumFields,...);

extern int       websWriteVARARGS(char* wp,char_t* fmt,va_list vargs);

extern char_t   *websGetVarWrap(char* wp, char_t *var, char_t *def);
int websGetVarInt(char* wp, char_t *var, int* value);
int websGetVarInt_Error(char* wp, char_t *var, int* value);


#endif /* _h_WEBS */

/******************************************************************************/


