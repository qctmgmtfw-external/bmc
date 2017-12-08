//;*****************************************************************;
//;*****************************************************************;
//;**                                                             **;
//;**     (C) COPYRIGHT American Megatrends Inc. 2008-2009        **;
//;**                     ALL RIGHTS RESERVED                     **;
//;**                                                             **;
//;**  This computer software, including display screens and      **;
//;**  all related materials, are confidential and the            **;
//;**  exclusive property of American Megatrends, Inc.  They      **;
//;**  are available for limited use, but only pursuant to        **;
//;**  a written license agreement distributed with this          **;
//;**  computer software.  This computer software, including      **;
//;**  display screens and all related materials, shall not be    **;
//;**  copied, reproduced, published or distributed, in whole     **;
//;**  or in part, in any medium, by any means, for any           **;
//;**  purpose without the express written consent of American    **;
//;**  Megatrends, Inc.                                           **;
//;**                                                             **;
//;**                                                             **;
//;**                American Megatrends, Inc.                    **;
//;**           5555 Oakbook Parkway, Building 200                **;
//;**     Norcross,  Georgia - 30071, USA. Phone-(770)-246-8600.  **;
//;**                                                             **;
//;*****************************************************************;
//;*****************************************************************;

// JavaScript Document
var bitset = {};
//some utility functions

//satrtbit and endbit as used in IPMI specs..for e.g: [6:0]
getbits = function(orig,startbit,endbit)
{
	var temp = orig;
	var mask =0x00;

	for(var i=startbit;i>=endbit;i--)
	{
		mask = mask | (1 << i);
	}

	return (temp & mask);
}

setbits = function(orig,startbit,endbit)
{
	var temp = orig;
	var mask = 0x0;

	for(var i=startbit;i>=endbit;i--)
	{
			mask = mask | (1 << i);

	}
	return (temp | mask);
}

getbitsval = function(orig,startbit,endbit)
{
	var temp;
	temp = getbits(orig,startbit,endbit);
	temp = temp >> endbit;
	return temp;
}

var MEDIUM_ERROR_FLAG = 0;
var IPMI_ERROR_FLAG = 1;

IS_MEDIUM_ERROR = function(x)
{
	return (((x)>>8) ==  MEDIUM_ERROR_FLAG );
}

IS_IPMI_ERROR = function(x)
{
	return (((x)>>8) ==  IPMI_ERROR_FLAG );
}

GET_ERROR_CODE = function(x)
{
	return (x & 0xff);
}

/**array of error strings...will need to move it later to lang specific files***/
/* Medium related errors */
MediumErrorStr = new Array();
MediumErrorStr[0x01]="Could not establish a session";//"LIBIPMI_MEDIUM_E_CONNECT_FAILURE";
MediumErrorStr[0x02]="Could not send data";//"LIBIPMI_MEDIUM_E_SEND_DATA_FAILURE";
MediumErrorStr[0x03]="Could not receive data";//"LIBIPMI_MEDIUM_E_RECV_DATA_FAILURE";
MediumErrorStr[0x04]="Could not initialize communication";//"LIBIPMI_MEDIUM_E_WSA_INIT_FAILURE";
MediumErrorStr[0x05]="Could not initialize communication";//"LIBIPMI_MEDIUM_E_INVALID_SOCKET";
MediumErrorStr[0x06]="Session timed out";//"LIBIPMI_MEDIUM_E_TIMED_OUT";
MediumErrorStr[0x07]="Unsupported medium";//"LIBIPMI_MEDIUM_E_UNSUPPORTED";
MediumErrorStr[0x08]="Unsupported OS";//"LIBIPMI_MEDIUM_E_OS_UNSUPPORTED";
MediumErrorStr[0x09]="Invalid parameters";//"LIBIPMI_MEDIUM_E_INVALID_PARAMS";
MediumErrorStr[0x0A]="Invalid data";//"LIBIPMI_MEDIUM_E_INVALID_DATA";
MediumErrorStr[0x0B]="Timed out while sending data";//"LIBIPMI_MEDIUM_E_TIMED_OUT_ON_SEND";

/* Session related errors */
MediumErrorStr[0x10]="Session expired";//"LIBIPMI_SESSION_E_EXPIRED";
MediumErrorStr[0x11]="Session reconnect failure";//"LIBIPMI_SESSION_E_RECONNECT_FAILURE";
MediumErrorStr[0x12]="Session handshake failure";//"LIBIPMI_SESSION_E_HANDSHAKE_NOT_RECVD";

/* RMCP reated errors*/
MediumErrorStr[0x20]="Invalid RMCP Packet";//"LIBIPMI_RMCP_E_INVALID_PACKET";
MediumErrorStr[0x21]="Invalid Pong";//"LIBIPMI_RMCP_E_INVALID_PONG";

MediumErrorStr[0x30]="BMC does not support IPMI2.0";//"LIBIPMI_BMC_E_IPMI2_NOT_SUPPORTED"

/* AES Encryption Errors */
MediumErrorStr[0x40]="Insufficient memory to perform operation";//"LIBIPMI_AES_CBC_E_NO_ENOUGH_MEMORY";
MediumErrorStr[0x41]="Encryption is not supported";//"LIBIPMI_ENCRYPTION_UNSUPPORTED";

/* Validation Errors */
MediumErrorStr[0x50]="Invalid Authentication type";//"LIBIPMI_E_INVALID_AUTHTYPE";
MediumErrorStr[0x51]="Invlais Session Id";//"LIBIPMI_E_INVALID_SESSIONID";
MediumErrorStr[0x52]="LIBIPMI_E_PADBYTES_MISMATCH";
MediumErrorStr[0x53]="LIBIPMI_E_AUTHCODE_MISMATCH";
MediumErrorStr[0x54]="Checksum mismatch";//"LIBIPMI_E_CHKSUM_MISMATCH";
MediumErrorStr[0x55]="Unsupported authentication type";//"LIBIPMI_E_AUTHTYPE_NOT_SUPPORTED";

/* Session Establishment Errors */
MediumErrorStr[0x60]="Invalid response to Open Session";//"LIBIPMI_E_INVALID_OPEN_SESSION_RESPONSE";
MediumErrorStr[0x61]="Invalid message received";//"LIBIPMI_E_INVALID_RAKP_MESSAGE_2";
MediumErrorStr[0x62]="Authentication algorithm not supported";//"LIBIPMI_E_AUTH_ALG_UNSUPPORTED";
MediumErrorStr[0x63]="Integrity algorithm not supported";//"LIBIPMI_E_INTEGRITY_ALG_UNSUPPORTED";
MediumErrorStr[0x64]="Confidentiality algorithm not supported";//"LIBIPMI_E_CONFIDENTIALITY_ALG_UNSUPPORTED";
MediumErrorStr[0x65]="Auth code is Invalid";//"LIBIPMI_E_AUTH_CODE_INVALID";
MediumErrorStr[0x66]="Invalid RAKP_MSG_4";//"LIBIPMI_E_INVALID_RAKP_MESSAGE_4";
MediumErrorStr[0x67]="Invalid HMAC_SIK";//"LIBIPMI_E_INVALID_HMAC_SIK";

/* Highlevel function errors */
MediumErrorStr[0x70]="Invalid User ID";//"LIBIPMI_E_INVALID_USER_ID";
MediumErrorStr[0x71]="Invalid User Name";//"LIBIPMI_E_INVALID_USER_NAME";
MediumErrorStr[0x72]="Invalid Index";//LIBIPMI_E_INVALID_INDEX;
MediumErrorStr[0x80]="Not enough Memory";//LIBIPMI_E_NO_ENOUGH_MEMORY;
MediumErrorStr[0x81]="Invalid host address";//LIBIPMI_E_INVALID_HOST_ADDR;
MediumErrorStr[0x82]="I2C write failure";//LIBIPMI_E_I2C_WRITE_FAILURE;
MediumErrorStr[0x83]="I2C read failure";//LIBIPMI_E_I2C_READ_FAILURE;
MediumErrorStr[0x84]="I2C Bus suspend";//LIBIPMI_E_I2C_BUS_SUSPEND;
MediumErrorStr[0x85]="Sequence number mismatch";//LIBIPMI_E_SEQ_NUM_MISMATCH;
MediumErrorStr[0x86]="Insufficient buffer size";//LIBIPMI_E_INSUFFICIENT_BUFFER_SIZE;
MediumErrorStr[0x87]="IPMB Lock Access failed";//LIBIPMI_E_IPMB_LOCK_ACCESS_FAILED;
MediumErrorStr[0x88]="IPMB Communication failure";//LIBIPMI_E_IPMB_COMM_FAILURE;
MediumErrorStr[0x89]="IPMB Unknown error";//LIBIPMI_E_IPMB_UNKNOWN_ERROR;
MediumErrorStr[0x8A]="IPMI Request Buffer too big";//LIBIPMI_E_IPMB_REQ_BUFF_TOO_BIG;
MediumErrorStr[0x8B]="IPMB Response Buffer too big";//LIBIPMI_E_IPMB_RES_BUFF_TOO_BIG;

IPMIErrorStr = new Array();
IPMIErrorStr[0xD4]="You do not have sufficient privileges to perform this operation";

IPMIPrivileges = new Array();
IPMIPrivileges[0] = "Reserved";
IPMIPrivileges[1] = "Callback";
IPMIPrivileges[2] = "User";
IPMIPrivileges[3] = "Operator";
IPMIPrivileges[4] = "Administrator";
IPMIPrivileges[5] = "OEM Proprietary";
IPMIPrivileges[0xf] = "No Access";


GET_ERROR_CODE_STR = function(x)
{
	acterr = GET_ERROR_CODE(x);

	if(IS_MEDIUM_ERROR(x))
	{

		//errdesc = domapi.rVal(MediumErrorStr[acterr],"undefined");
		return "Communication error " + acterr + " : " + MediumErrorStr[acterr];
	}
	else
	{
		//errdesc = domapi.rVal(IPMIErrorStr[acterr],"undefined");
		return "IPMI Command Error " + acterr + " : " + IPMIErrorStr[acterr];
	}
}

bitset.loaded = true;