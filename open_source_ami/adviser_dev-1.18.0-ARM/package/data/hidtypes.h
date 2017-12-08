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

/** @file   hidtypes.h
 *  @author <ramkumars@ami.com>
 ****************************************************************/

#ifndef HIDTYPES_H
#define HIDTYPES_H

#include "coreTypes.h"
#include "iusb.h"

#include "icc_what.h"
#include "authenticate.h"

#ifdef ICC_OS_WINDOWS
#pragma pack(1)
#endif

#ifdef ICC_OS_LINUX
#define PACK __attribute__((packed))
#else
#define PACK
#endif

//#define AUTHENTICATE		//	Comments this to disable authentication
#define MOUSE_PORT           (5121)

#define HIDCMD_SIG           "HIDCMD  "

#define MAX_HID_PKT_SIZE     (256)	//	This should accomodate any command packets
									//	from HID_CMD, IPS2 or IUSB. So, if there is
									//	any new command addition that needs a bigger
									//	buffer size, then increase this. Or if we
									//	decide to pad multiple data packets in one
									//	command, then this size should be increased.

#define HID_PROTO_PS2			(0x50)
#define HID_PROTO_USB			(0x51)

#define HID_ALIVE_INTERVAL_IN_SECS              (40)
/* command numbers */
#define HID_CMD_START_REDR      (100)
#define HID_CMD_STOP_REDR       (101)
#define HID_CMD_GET_DEVCAPS     (102)
#define HID_CMD_GET_PORTCAPS    (103)
#define HID_CMD_GET_CHALLENGE	(104)
#define HID_CMD_LOGIN		(105)
#define HID_CMD_CHANGE_PROTOCOL  (106)
#define HID_CMD_ENCRYPT_CHALLENGE (107)
#define HID_CMD_GET_LOCAL_VIDEO_STATUS (108)
#define HID_CMD_SET_LOCAL_VIDEO_STATUS (109)
#define HID_CMD_I_AM_ALIVE         (111)
#define HID_CMD_USB_RECONNECT   ( 110 )
#define HID_RESCMD_MOUSE_MODE   ( 112 )
#define HID_REQCMD_MOUSE_MODE   ( 113 )
#define HID_CMD_MOUSE_MODE   ( 114 )

/* New protocol */
#define EXCLUSIVE_MODE          (0x25)
#define SHARED_MODE             (0x26)
#define DEVNUM_KBD_MOUSE        (2)

#ifdef ICC_OS_LINUX
#define LEFT_BUTTON             (0x01)
#define RIGHT_BUTTON            (0x02)
#define MIDDLE_BUTTON           (0x04)
#endif

//instead of redefining IUSB_HEADER as HID_HEADER
//and including it everywhere, we just make HID_HEADER
//equivalent to IUSB_HEADER

//typedef IUSB_HEADER HID_HEADER;
//typedef IUSB_HID_PACKET HID_PACKET;

#define HID_MOUSE_MODE_ABSOLUTE		0
#define HID_MOUSE_MODE_RELATIVE		1
#define HID_MOUSE_MODE_DEFAULT		HID_MOUSE_MODE_RELATIVE //HID_MOUSE_MODE_ABSOLUTE

/**
 * @struct HID_CMD_PACKET
 **/
typedef struct HID_CMD_PACKET_tag
{

  u8  Signature [8];          /**< signature "HIDCMD  "  */
  u16 wCmdNo;                 /**< Command Number */
  u16 wStatus;                /**< Status, ErrorCodes */
  u32 dwDataLen;              /**< Length of the data Packet */

}PACK HID_CMD_HEADER;


typedef struct
{
	HID_CMD_HEADER	Hdr;
	u8				Data [1];

} PACK HID_CMD_PACKET;
/**
 * HID_CHALLENGE_DATA packet
**/
typedef struct HID_CHALLENGE_DATA_tag
{
    unsigned char challenge [MAX_CHALLENGE_LEN];

} PACK HID_CHALLENGE_DATA;

/**
 * HID_GET_CHALLENGE_PACKET
**/
typedef struct HID_GET_CHALLENGE_tag
{
    HID_CMD_HEADER		Cmd;
    HID_CHALLENGE_DATA	Data;

} PACK HID_GET_CHALLENGE_PACKET;


typedef struct HID_LOGIN_DATA_tag
{
    unsigned char UserName [MAX_USER_NAME_LEN];
    unsigned char Digest [STD_DIGEST_LEN];

} PACK HID_LOGIN_DATA;


typedef struct HID_LOGIN_DATA_PACKET_tag
{
    HID_CMD_HEADER	Cmd;
    HID_LOGIN_DATA  Data;

}PACK HID_LOGIN_DATA_PACKET;


/**
 * @struct START_REDIRECTION_DATA packet
 *
**/
typedef struct START_REDIRECTION_DATA_tag
{
  int16				wSrvrPort;
  u8				bReserved [2];

} PACK HID_START_REDIRECTION_DATA;

/**
 * @struct START_REDIRECTION packet *
**/
typedef struct START_REDIRECTION_tag
{
  HID_CMD_HEADER				Cmd;
  HID_START_REDIRECTION_DATA	Data;

} PACK HID_START_REDIRECTION_PACKET;



/**
 * @struct HID_CMD_GET_DEVCAPS packet
**/
typedef struct HID_DEVCAPS_tag
{
  u8   bHidProtocol;    /**< HID_PROTO_USB or HID_PROTO_PS2 */
  int  nPorts;          /**< 1 ==> K1 or G3,  4 ==>K16 */
  u8   bReserved [3];

} PACK HID_DEVCAPS;


/**
 * @struct HID_CMD_GET_DEVCAPS packet
**/
typedef struct HID_CMD_GET_DEVCAPS_tag
{
  HID_CMD_HEADER	Cmd;
  HID_DEVCAPS		DevCaps;

} PACK HID_CMD_GET_DEVCAPS_PACKET;


/**
 * @struct HID_CMD_GET_PORTCAPS packet
**/
typedef struct HID_PORTCAPS_tag
{
	int16  wPortno;      /** <Port Number */
	u8     bAccessMode;  /**< HID_EXCLUSIVE_MODE or HID_SHARED_MODE */
	int    nClients;     /**< number of clients connected to "wPortNo" */
	u8     bReserved [1];

} PACK HID_PORTCAPS;

/**
 * @struct	HID_CMD_GET_PORTCAPS packet
 * @brief	When issueing the command, wPortno field should be filled with
 *				the "port" for which this information is needed. A value of
 *				-1 in wPortno will be returned with information regarding
 *				all the ports in the target device.
**/
typedef struct HID_CMD_GET_PORTCAPS_tag
{
	HID_CMD_HEADER	Cmd;
	HID_PORTCAPS	PortCaps [1];

} PACK HID_CMD_GET_PORTCAPS_PACKET;


/********************************************************************************/
// NEW FEATURE : SUPPORTING BOTH USB AND PS2
// Note: On receiving this packet, hidserver performs the following
//       1. check for number of clients
//
//       2. issue ioclt to driver
//       3. chenge the hid.conf file
//       4. hidserver returns the packet with the new protcol and status = success/failure

/**
 * @struct HID_CHANGE_PROTCOL packet
 * @brief  HID_PROTO_USB or HID_PROTO_IPS2
**/
typedef struct HID_CHANGE_PROTOCOL_tag
{
    u8  bHidProtocol;

} PACK HID_CHANGE_PROTOCOL;

/**
 * @struct	HID_CHANGE_PROTCOL_PACKET packet
 * @brief
**/
typedef struct HID_CHANGE_PROTCOL_PACKET_tag
{
    HID_CMD_HEADER	Cmd;
    HID_CHANGE_PROTOCOL Data;

} PACK HID_CMD_CHANGE_PROTOCOL_PACKET;


/**
 * @struct HID_CHANGE_PROTCOL packet
 * @brief  HID_PROTO_USB or HID_PROTO_IPS2
**/
typedef struct HID_CHANGE_MOUSE_MODE_tag
{
    u8  bMouseMode;

} PACK HID_MOUSE_MODE;

/**
 * @struct	HID_CHANGE_PROTCOL_PACKET packet
 * @brief
**/
typedef struct HID_CHANGE_MOUSE_MODE_PACKET_tag
{
    HID_CMD_HEADER	Cmd;
    HID_MOUSE_MODE Data;

} PACK HID_CMD_CHANGE_MOUSE_MODE_PACKET;


/************************************************************************************/
typedef struct HID_LOCAL_VIDEO_CTRL_tag
{
    unsigned char bLocalVideoState;
}PACK HID_LOCAL_VIDEO_CTRL_DATA;

typedef struct HID_LOCAL_VIDEO_CTRL_DATA_PACKET_tag
{
    HID_CMD_HEADER Cmd;
    HID_LOCAL_VIDEO_CTRL_DATA Data;

}PACK HID_LOCAL_VIDEO_CTRL_DATA_PACKET;



/**
 * @struct
 * @brief
**/
typedef struct
{
  u8      Event;
  u8      dx;
  u8      dy;
  u16     AbsX;
  u16     AbsY;
  u16     ResX;
  u16     ResY;
  u8      IsValidData;
} PACK HOST_MOUSE_PACKET;


typedef enum
{

  DEV_ACCESS_EXCLUSIVE_MODE = 0,
  DEV_ACCESS_SHARED_MODE

} ACCESSMODE_E;


typedef enum
{
  PS2TYPE = 0,
  USBTYPE

} DEVICE_TYPE_E;

typedef enum
{
	PKTTYPE_INVALID = 0,
	PKTTYPE_HID_CMD,
	PKTTYPE_IPS2,
	PKTTYPE_IUSB,

} PKT_TYPES_E;

typedef struct AddDevice
{
	uint8 DeviceNo;
	uint8 DeviceType;
} AddDev_T;


#ifdef ICC_OS_WINDOWS
#pragma pack()
#endif


#endif // HIDTYPES_H
