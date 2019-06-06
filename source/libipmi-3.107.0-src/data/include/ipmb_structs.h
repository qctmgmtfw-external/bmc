/* Copied from G3 libipmi/libhhm-ipmi.h
   Rajasekhar 12-Dec-2005
*/
#ifndef __IPMB_STRUCTS_H__
#define __IPMB_STRUCTS_H__

#include "coreTypes.h"

/* IPMB related definitions */
#define		IPMB_MSG_HDR		(5+1+1)/*ipmb_msg_hdr + checksum + start/stop byte */
#define		MAX_IPMB_BUFFER		(255) 
#define		MAX_IPMB_DATA_SIZE	(MAX_IPMB_BUFFER - IPMB_MSG_HDR)




/* Macros for converting seq/lun combos */
#define SEQ( seqlun )           ( ( seqlun & (u8)0xFC ) >> 2 )
#define SLUN( seqlun )          ( ( seqlun & (u8)0x03 ) )
#define SEQLUN( seq, lun )      ( ( seq << 2 ) | ( lun ) )

/* The number of times we will try to send a command and get a response */
/* before we give up, and allow the error to filter up to the caller    */
#define IPMI_COMMAND_RETRIES    ( 3 )

/*****          IPMI Command and Response Structures                     *****/
/* Adapted/stolen from MegaNASFreeBSD/Development/ipmid/ipmb.h originally    */
/* written by Subash Kalbarga                                                */

/** \brief The header for every IPMI message delivered over the IPMB */
typedef struct _IPMBHeader
{
    /* The target slave address is supplied by the driver here */
    u8 netFnTargetLUN;
    u8 Checksum1;
    u8 SourceSA;
    u8 SourceSeqSourceLUN;
} IPMBHeader;


/** \brief Basic IPMB command - usually just one byte, but can be more */
typedef struct _IPMBCmd
{
    u8 ipmbcmd[ 1 ];
    /* data bytes will follow this */
} IPMBCmd;


/** \brief The basic format of an I2C message on the IPMB */
typedef struct _IPMBMsg
{
    IPMBHeader header;
    IPMBCmd command;
} IPMBMsg;


/** \brief The basic format of an I2C message response on the IPMB */
typedef struct _GenericMsgResponse
{
    IPMBMsg ipmbmsg;
    u8 CompletionCode;
} GenericMsgResponse;


#endif


