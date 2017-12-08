/*****************************************************************
 **                                                             **
 **     (C) Copyright 2006-2009, American Megatrends Inc.       **
 **                                                             **
 **             All Rights Reserved.                            **
 **                                                             **
 **         5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                             **
 **         Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                             **
 ****************************************************************/

/**
 * cmdhndlr.h
 * Command Handler
**/

#ifndef CMDHNDLR_H
#define CMDHNDLR_H
#include "coreTypes.h"
#include "videopkt.h"
#include "session_data.h"


typedef int (*p_cmd_hndlr_t) (session_info_t* p_si, ivtp_hdr_t* p_hdr);

typedef struct
{
        u16                     cmd;
        p_cmd_hndlr_t           hndlr;

} hndlr_map_t;

/**
*Method to send the KVM wait packet to the second User
*
**/

int send_req_kvm_wait(session_info_t* p_si);

/**
*Method to approve the session based on the permission given by the First user
*
**/

void  approve_session(session_info_t* p_si,  auth_t *auth_pkt, char *user_name,char *client_ip, unsigned char kvm_priv);

/**
* 
Methos to send the request KVM previlege packet to the second User
*
**/
int send_req_kvm_priv(session_info_t * first_session);

#endif	/* CMDHNDLR_H */

