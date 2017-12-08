/** \file smtpclient.h
 *  \brief  SMTP Client library to send E-Mail
 *  \author Venkatesan B <venkatesanb@amiindia.co.in>
 *  \date   Thu, 07 Aug 2003 14:30:51 IST
 */
#ifndef _SMTP_CLIENT_H_
#define _SMTP_CLIENT_H_

/*! \def    ADDR_SIZE       The buffer size to hold the Email address */
#define ADDR_SIZE           256
/*! \def MESSAGE_BODY_SIZE  The buffer size to hold the EMail message body */
#define MESSAGE_BODY_SIZE   1024
/*! \def FILE_ATTACHMENT_SIZE  The buffer size to hold the EMail attachment Data */
#define FILE_ATTACHMENT_SIZE  1500
/*! \def    SMTP_MAIL_PORT  Define to default SMTP port 25 */
#define SMTP_MAIL_PORT      25

#define AUTH_TYPE_NONE	    0
#define AUTH_TYPE_PLAIN	    1
#define AUTH_TYPE_LOGIN	    2
#define AUTH_TYPE_CRAM_MD5  4

#define AUTH_PLAIN	    "PLAIN"
#define AUTH_CRAM_MD5	    "CRAM-MD5"
#define AUTH_LOGIN	    "LOGIN"

#define SEND_NO_RESP	    0
#define CHECK_NORMAL	    1
#define CHECK_BANNER	    2
#define GET_AUTH_CAP	    3
#define GET_LOGIN_USER	    4
#define GET_LOGIN_PASS	    5
#define GET_AUTH_RESPONSE   6
#define GET_SERVER_RESP	    7
#define CHECK_MD5           8
#define CHECK_DIGEST_MD5    9

#define B64_BUFF_LEN	    1024
#define ATTACHMENT_FILENAME "RMS.RMS"
#define ERR_SUCCESS							1
#define ERR_GENERIC_FAILURE		 			0
#define ERR_UNABLE_TO_CONNECT_SMTPSERVER 	2
#define ERR_AUTHENTICATION_FAILURE 			3
#define ERR_UNSUPPORTED_AUTHTYPE			4
extern int EnableAuth;

#pragma pack(1)
/** \struct SMTP_STRUCT
 *  \brief  Structure to hold the SMTP Infromation and EMAIL data.
 *  The username/password pair right now not used and may be left unfilled.
 *  The Local host name used to say Hi with SMTP server.
 *  The \c to_addr, \c from_addr \c Subject and \c message_body are must for email
 *  The \c smtp_server should hold vaild mail server address in dotted notation.
 */
typedef struct {
    char username[65];          /*! Username to authenticate with mail server */
    char password[65];          /*! Password to authenticate with mail server */
    char local_host[32];        /*! The local host name to say hi with mail server */
#ifdef CFG_PROJ_ENABLE_IPV6_SUPPORT
    char smtp_server[INET6_ADDRSTRLEN];       /*! Vaild IP address of Mail server */
#else
    char smtp_server[32];       /*! Vaild IP address of Mail server */
#endif
    char to_addr[ADDR_SIZE];    /*! Email address */
    char cc_addr[ADDR_SIZE];    /*! Email adress */
    char err_addr[ADDR_SIZE];   /*! Email adress */
    char from_addr[ADDR_SIZE];  /*! Email adress */
    char reply_addr[ADDR_SIZE]; /*! Email adress */
    char subject[ADDR_SIZE];    /*! Email subject */
    char message_body[MESSAGE_BODY_SIZE];   /*! The message to be sent */
    unsigned char Attach_File;    /*Checks for File attachment */
    char LogValues[FILE_ATTACHMENT_SIZE];   /*Holds the Attachment data*/
    unsigned char UserID; /* User ID*/
} SMTP_STRUCT;
#pragma pack()

/** \fn smtp_mail
 *  \brief  Sends the email through the given SMTP server
 *  The \c to_addr, \c from_addr, \c subject, \c message_body and \c smtp_server
 *  vaild to send a mail. All other fields are optional.
 *  Currently \c username and \c password are not used and ment for smtp server
 *  authentication in future.
 *  \param pointer to \c SMTP_STRUCT
 *  \return on successful send rtuens 0, otherwise -1
 */
extern int smtp_mail (SMTP_STRUCT *pmail);

/** \fn smtp_test
 *  \brief  Test the given SMTP server
 *  The \c from_addr, and \c smtp_server
 *  vaild to test a mail server. All other fields are optional.
 *  \param pointer to \c SMTP_STRUCT
 *  \return on successful send rtuens 0, otherwise -1
 */
extern int smtp_test( SMTP_STRUCT *pmail );

#endif /* _SMTP_CLIENT_H_   */
