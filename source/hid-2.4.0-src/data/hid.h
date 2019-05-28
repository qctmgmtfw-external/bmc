/*
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2009-2015, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 */
#ifndef HID_H
#define HID_H

#include "dbgout.h"
#include "usb_core.h"

#define DEBUG_HID				0x01
#define DEBUG_MOUSE 			0x02
#define DEBUG_KEYBD 			0x04
#define DEBUG_IUSB_HID			0x08
#define DEBUG_PIPE 				0x10
#define MAX_MOUSE_DATA_SIZE		6
#define MAX_KEYBD_DATA_SIZE		8
#define MAX_PIPE_DATA_SIZE		1024


#pragma pack (1)
/**************************HID Related Defines ********************************/

/* USB HID Class Request Types */
#define GET_REPORT				0x01
#define GET_IDLE				0x02
#define GET_PROTOCOL			0x03
#define SET_REPORT				0x09
#define SET_IDLE				0x0A
#define SET_PROTOCOL			0x0B

/* HID Interface Subclasses */
#define NO_SUBCLASS				0x00
#define BOOT_INTERFACE			0x01

/* HID Interface Protocol Codes */
#define NONE					0x00
#define KEYBOARD_INTERFACE		0x01
#define MOUSE_INTERFACE			0x02


/********************* Report Descriptor Definitions **************************/

/* Main Items */
#define INPUT					0x80
#define OUTPUT					0x90
#define FEATURE					0xB0
#define COLLECTION				0xA0
#define END_COLLECTION			0xC0

/* Input Item Arguments */
#define DATA					0x00
#define CONSTANT				0x01
#define ARRAY					0x00
#define VARIABLE				0x02
#define ABSOLUTE				0x00
#define RELATIVE				0x04
#define NO_WRAP					0x00
#define WRAP					0x08
#define LINEAR					0x00
#define NON_LINEAR				0x10
#define PREFERRED_STATE			0x00
#define NO_PREFERRED			0x20
#define NO_NULL_POSITION		0x00
#define NULL_STATE				0x40
#define NON_VOLATILE			0x00
#define VOLATILE				0x80
#define BIT_FIELD				0x00
#define BUFFERED_BYTES			0x100

/* Collection Item Arguments */
#define PHYSICAL				0x00
#define APPLICATION				0x01
#define LOGICAL					0x02

/* Global Items */
#define USAGE_PAGE				0x04
#define LOGICAL_MINIMUM			0x14
#define LOGICAL_MAXIMUM			0x24
#define PHYSICAL_MINIMUM		0x34
#define PHYSICAL_MAXIMUM		0x44
#define UNIT_EXPONENT			0x54
#define UNIT					0x64
#define REPORT_SIZE				0x74
#define REPORT_ID				0x84
#define REPORT_COUNT			0x94
#define PUSH					0xA4
#define POP						0xB4

/* Usage Page Item Arguments */
#define GENERIC_DESKTOP			0x01
#define SIMULATION				0x02
#define VR						0x03
#define SPORT					0x04
#define GAME					0x05
#define KEYBOARD_KEYPAD			0x07
#define LEDS					0x08
#define BUTTON					0x09
#define ORDINAL					0x0A
#define TELEPHONY				0x0B
#define CONSUMER				0x0C
#define DIGITIZER				0x0D
#define PID_PAGE				0x0F
#define UNICODE					0x10
#define ALPHANUMERIC_DISPLAY	0x14
#define BAR_CODE_SCANNER		0x8C
#define SCALE					0x8D
#define CAMERA					0x90
#define ARCADE					0x91

/* Local Items */
#define USAGE					0x08
#define USAGE_MINIMUM			0x18
#define USAGE_MAXIMUM			0x28
#define DESIGNATOR_INDEX		0x38
#define DESIGNATOR_MINIMUM		0x48
#define DESIGNATOR_MAXIMUM		0x58
#define STRING_INDEX			0x78
#define STRING_MINIMUM			0x88
#define STRING_MAXIMUM			0x98
#define DELIMITER				0xA8

/* Usage Item Arguments - Generic Desktop Page */
#define POINTER					0x01
#define MOUSE					0x02
#define JOYSTICK				0x04
#define GAME_PAD				0x05
#define KEYBOARD				0x06
#define KEYPAD					0x07
#define MULTI_AXIS_CONTROLLER	0x08
#define X						0x30
#define Y						0x31
#define Z						0x32
#define Rx						0x33
#define Ry						0x34
#define Rz						0x35
#define SLIDER					0x36
#define DIAL					0x37
#define WHEEL					0x38
#define HAT_SWITCH				0x39
#define COUNTED_BUFFER			0x3A
#define BYTE_COUNT				0x3B
#define MOTION_WAKEUP			0x3C
#define START					0x3D
#define SELECT					0x3E
#define Vx						0x40
#define Vy						0x41
#define Vz						0x42
#define Vbrx					0x43
#define Vbry					0x44
#define Vbrz					0x45
#define Vno						0x46
#define SYSTEM_CONTROL			0x80
#define SYSTEM_POWER_DOWN		0x81
#define SYSTEM_SLEEP			0x82
#define SYSTEM_WAKE_UP			0x83
#define SYSTEM_CONTEXT_MENU		0x84
#define SYSTEM_MAIN_MENU		0x85
#define SYSTEM_APP_MENU			0x86
#define SYSTEM_MENU_HELP		0x87
#define SYSTEM_MENU_EXIT		0x88
#define SYSTEM_MENU_SELECT		0x89
#define SYSTEM_MENU_RIGHT		0x8A
#define SYSTEM_MENU_LEFT		0x8B
#define SYSTEM_MENU_UP			0x8C
#define SYSTEM_MENU_DOWN		0x8D
#define D_PAD_UP				0x90
#define D_PAD_DOWN				0x91
#define D_PAD_RIGHT				0x92
#define D_PAD_LEFT				0x93


/************************ Report Descriptor Macros ***************************/

#define SHORT_ITEM_1(ITEM, ARG)	(uint16)(((ITEM | 1) << 8) | (ARG & 0xFF))
#define SHORT_ITEM_2(ITEM, ARG)	(uint32)(((ITEM | 2) << 16) | ((ARG & 0xFF) << 8) |((ARG & 0xFF00)>>8))

/* Main Items */
#define INPUT3(x, y, z)		SHORT_ITEM_1(INPUT, (x | y | z))
#define INPUT2(x, y)		SHORT_ITEM_1(INPUT, (x | y ))
#define INPUT1(x)			SHORT_ITEM_1(INPUT, x)
#define OUTPUT3(x, y, z)	SHORT_ITEM_1(OUTPUT, (x | y | z))
#define OUTPUT1(x)			SHORT_ITEM_1(OUTPUT, x)
#define FEATURE3(x, y, z)	SHORT_ITEM_1(FEATURE, (x | y | z))
#define FEATURE1(x)			SHORT_ITEM_1(FEATURE, x)
#define COLLECTION1(x)		SHORT_ITEM_1(COLLECTION, x)

/* Global Items */
#define USAGE_PAGE1(x)		SHORT_ITEM_1(USAGE_PAGE, x)
#define USAGE_PAGE2(x)		SHORT_ITEM_2(USAGE_PAGE, x)
#define LOGICAL_MINIMUM1(x)	SHORT_ITEM_1(LOGICAL_MINIMUM, x)
#define LOGICAL_MAXIMUM1(x)	SHORT_ITEM_1(LOGICAL_MAXIMUM, x)
#define LOGICAL_MAXIMUM2(x)	SHORT_ITEM_2(LOGICAL_MAXIMUM, x)
#define PHYSICAL_MINIMUM1(x)	SHORT_ITEM_1(PHYSICAL_MINIMUM, x)
#define PHYSICAL_MAXIMUM1(x)	SHORT_ITEM_1(PHYSICAL_MAXIMUM, x)
#define REPORT_SIZE1(x)		SHORT_ITEM_1(REPORT_SIZE, x)
#define REPORT_COUNT1(x)	SHORT_ITEM_1(REPORT_COUNT, x)	

/* Local Items */
#define USAGE1(x)			SHORT_ITEM_1(USAGE, x)
#define USAGE2(x)			SHORT_ITEM_2(USAGE, x)
#define USAGE_MINIMUM1(x)	SHORT_ITEM_1(USAGE_MINIMUM, x)
#define USAGE_MAXIMUM1(x)	SHORT_ITEM_1(USAGE_MAXIMUM, x)
#define USAGE_MAXIMUM2(x)	SHORT_ITEM_2(USAGE_MAXIMUM, x)


/* Hid Interface Data */
typedef struct
{
	uint8	Idle;
	uint8	Protocol;
	uint8 	SetReportCalled;
	uint8   ReportReq;
} HID_IF_DATA;

TDBG_DECLARE_DBGVAR_EXTERN(hid);
extern uint8 MouseMode;
extern USB_CORE UsbCore;
extern USB_IUSB iUSB;

#pragma pack()


#endif /* HID_H */

/************************************************************************/
