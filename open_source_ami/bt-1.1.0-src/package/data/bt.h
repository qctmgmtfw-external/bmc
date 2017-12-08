/*
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2006-2009, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 */

#ifndef BT_H
#define BT_H

#include "bt_timeout.h"

#define MSG_PAYLOAD_SIZE	(33*1024)

typedef struct
{
	u32		    		BTRcvPktLen;
	u8*					pBTRcvPkt;
	u32		    		BTSendPktLen;       
	u8*					pBTSendPkt;
	u8 					FirstTime;
	u8					BtWakeup;
	u8					BtReqWaiting;
	Bt_OsSleepStruct 	BtReqWait;

} __attribute__((packed)) BTBuf_T;

typedef struct
{
	u8			Channel;						/**< Originator's channel number*/
	u8			Param;							/**< Parameter*/
	u8			Cmd;							/**< Command that needs to be processed*/
	u8			NetFnLUN;						/**< Net function and LUN of command*/
	u8			Privilege;						/**< Current privilege level*/
	u32			SessionID;						/**< Session ID if any*/
	u8			RequestSize;					/**< Size of the Request*/
	u8			Request	 [MSG_PAYLOAD_SIZE];	/**< Request Data buffer*/
	u32			ResponseSize;					/**< Size of the response*/ 
	u8			Response [MSG_PAYLOAD_SIZE];	/**< Response Data buffer*/
	u8* 	    pRequest;               		/**< Pointer to Request data*/ 
	u8*    		pResponse;              		/**< Pointer to Response data*/

} __attribute__((packed)) IPMICmdMsg_T;

/* BT Request Structure */
typedef struct
{
	u8 			Length;
	u8			NetFnLUN;
	u8			seq;
	u8			Cmd;

} __attribute__((packed)) BTReq_T;

typedef struct
{
	unsigned char (*num_bt_ch) (void);
	void (*bt_interrupt_enable_user) (void); 
	void (*bt_interrupt_disable_user) (void);
	void (*set_sms_bit) (u8 ch_num);
	void (*clr_sms_bit) (u8 ch_num);
	void (*write_bt_data_buf) (u8 ch_num, u8* Buf, u32 Len);
	void (*read_bt_data_buf) (u8 ch_num, u8* Buf);
} bt_hal_operations_t;

typedef struct
{
	int (*process_bt_intr) (int dev_id, unsigned char ch_num, int Error);
	
} bt_core_funcs_t;

#endif /* BT_H */
