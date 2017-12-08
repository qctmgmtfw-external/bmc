/****************************************************************
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2005-2006, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        6145-F, Northbelt Parkway, Norcross,                **
 **                                                            **
 **        Georgia - 30071, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 *****************************************************************
 *
 * SOL.h
 * SOL Rountines & Definitions
 *
 * Author: Govind Kothandapani <govindk@ami.com>
 *       : Gowtham Shanmukham<gowthams@ami.com>
 *
 *****************************************************************/
#ifndef SOL_H
#define SOL_H
#include "Types.h"

#define PAYLOAD_SOL         0x01
#define MAX_SOL_DATA_SIZE   248

/**
 * SOL Payload Data Format
**/
typedef struct
{
    INT8U	seq_num;
    INT8U	ack_seq_num;
    INT8U	accepted_char_count;
    INT8U	status;
    INT8U	data [MAX_SOL_DATA_SIZE];

} SOLPayLoad_T;

/**
 * @def START_SOL
 * @brief Message Parameter to start SOL
 **/
#define     START_SOL                1


/**
 * @def ACTIVATE_SOL
 * @brief Message Parameter to activate SOL
 **/
#define     ACTIVATE_SOL                2

/**
 * @def DEACTIVATE_SOL
 * @brief Message Parameter to deactivate SOL
 **/
#define		DEACTIVATE_SOL				3

/**
 * @def WAKEUP_SOL
 * @brief Message Parameter to wake up SOL
 **/
#define     WAKEUP_SOL                  4



/**
 * Status Definitions
**/
#define	BMC_2_RC_NACK							0x40
#define BMC_2_RC_CHAR_TRANSFER_UNAVAILABLE		0x20
#define BMC_2_RC_SOL_DEACTIVATED				0x10
#define BMC_2_RC_TRANSMIT_OVERRUN				0x08
#define BMC_2_RC_BREAK							0x04

#define RC_2_BMC_NACK							0x40
#define RC_2_BMC_RING							0x20
#define RC_2_BMC_BREAK							0x10
#define RC_2_BMC_CTS_PAUSE						0x08
#define RC_2_BMC_DROP_DCD						0x04
#define RC_2_BMC_FLUSH_INBOUND					0x02
#define RC_2_BMC_FLUSH_OUTBOUND					0x01

/**
 * SOL Buffer Sizes & watermark
**/
#define SOL_SEND_BUF_SIZE					(64*1024)
#define SOL_RECV_BUF_SIZE					(1024)

#define SOL_CTS_WATERMARK					(48*1024)

/**
 * SOL milliseconds per tick
**/
#define SOL_MSECS_PER_TICK                  20

/**
 * SOL msecs wait for rechecking whether CTS has been asserted from deasserted state
**/
#define SOL_DEASSERT_WAIT_MSECS         10

#define PREVIOUS_SEQ_NUM(n)		(((n) == 0x01) ? 0x0f : ((n) - 1))

/*------------------ Extern Definitions --------------------------*/
extern void     WakeupSOLTask (int BMCInst);
extern INT32U   tick_diff (INT32U t1, INT32U t2);
extern int      SOLTask (void);
extern int	OpenSOLPort (int BMCInst);
extern int	CloseSOLPort (int BMCInst);
extern int	ConfigureSOLPort (int BMCInst);
extern void     AssertSOLCTS  (int BMCInst);
extern void     DeAssertSOLCTS(int BMCInst);

#endif	/* SOL_H */
