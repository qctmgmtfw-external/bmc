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

#ifndef KCS_H
#define KCS_H
#include "icc_what.h"
#include "kcs_timeout.h"

#define MSG_PAYLOAD_SIZE	(33*1024)
#define MAX_KCS_PKT_LEN		(33*1024)
#define INT_ENABLE  0x1E
#define POLL_MSEC HZ

/* Different Phases of the KCS Module */
#define KCS_PHASE_IDLE			    0x00
#define KCS_PHASE_WRITE			    0x01
#define KCS_PHASE_WRITE_END		    0x02
#define KCS_PHASE_READ			    0x03
#define KCS_PHASE_ABORT			    0x04
#define KCS_PHASE_ERROR			    0x05
#define ABORT_PHASE_ERROR1		    0x02
#define ABORT_PHASE_ERROR2		    0x03

/* KCS Error Codes */
#define KCS_NO_ERROR			0x00
#define KCS_ABORTED_BY_COMMAND		0x01
#define KCS_ILLEGAL_CONTROL_CODE	0x02
#define KCS_LENGTH_ERROR		0x06
#define KCS_UNSPECIFIED_ERROR		0xff

/**** Command Completion Codes ****/
#define	CC_NORMAL			0x00

/* Different KCS States */
#define KCS_IDLE_STATE	            0x00
#define KCS_READ_STATE	            0x40
#define KCS_WRITE_STATE             0x80
#define KCS_ERROR_STATE	            0xC0

/* KCS Command Control Codes */
#define KCS_GET_STATUS				0x60
#define KCS_ABORT				0x60
#define KCS_WRITE_START				0x61
#define KCS_WRITE_END				0x62
#define KCS_READ				0x68

#define KCS_CHANNEL_NOT_IN_USE		0x00
#define KCS_CHANNEL_IN_USE		0x01

#define TRANSMIT_TIMEOUT        10000   /* Transmission Timeout in msecs */

/* IOCTL related */
#define	ARBITRARY_NUM		100
#define MEL_IOCTL_TEST		_IOWR(ARBITRARY_NUM, 0, int)
#define MEL_IOCTL_READ		_IOR(ARBITRARY_NUM, 1, struct ioctl_t *)
#define MEL_IOCTL_WRITE		_IOW(ARBITRARY_NUM, 4, struct ioctl_t *)


typedef struct
{
       u32		    		KCSRcvPktIx;
	u8*				pKCSRcvPkt;
	u32		    		KCSSendPktIx;          
	u32		    		KCSSendPktLen;       
	u8*				pKCSSendPkt;
	u8		    		KCSPhase;	
	u8		    		AbortPhase;
	u8		    		KCSError;
	u8 				FirstTime;
	Kcs_OsSleepStruct 		KcsReqWait;
	u8				KcsIFActive;
	u8				KcsWakeup;
	u8				KcsResFirstTime;
	u8				KcsINuse;
	Kcs_OsSleepStruct 		KcsResWait;
	u8 				KcsWtIFActive;
	u8 				TxReady;
	u8				PrevCmdAborted;
	spinlock_t 			kcs_lock;
//	Event_T		    		KCSRcvPktReady;
	u8				KCSSeqNo;
}KCSBuf_T;



typedef struct
{
	u8			Channel;		/**< Originator's channel number*/
	//HQueue_T		hSrcQ;			/**< Originator Queue*/
	u8			Param;			/**< Parameter*/
	u8			Cmd;			/**< Command that needs to be processed*/
	u8			NetFnLUN;		/**< Net function and LUN of command*/
	u8			Privilege;		/**< Current privilege level*/
	u32			SessionID;		/**< Session ID if any*/
	u8			RequestSize;		/**< Size of the Request*/
	u8			Request	 [MSG_PAYLOAD_SIZE];	/**< Request Data buffer*/
	u32			ResponseSize;			/**< Size of the response*/  //Modified by winston for YAFU
	u8			Response [MSG_PAYLOAD_SIZE];	/**< Response Data buffer*/
	u8* 	    		pRequest;               /**< Pointer to Request data*/ 
	u8*    			pResponse;              /**< Pointer to Response data*/

} __attribute__((packed)) IPMICmdMsg_T;


/* KCS Request Structure */
typedef struct
{
	u8			NetFnLUN;
	u8			Cmd;

} __attribute__((packed)) KCSReq_T;


typedef struct
{
	unsigned char (*num_kcs_ch) (void);
	void (*enable_kcs_interrupt) (u8 ch_num); 
	void (*disable_kcs_interrupt) (u8 ch_num);
	void (*read_kcs_status) (u8 ch_num, u8* status_value);
	void (*write_kcs_status) (u8 ch_num, u8 status_value);
	void (*read_kcs_command) (u8 ch_num, u8* command);
	void (*read_kcs_data_in) (u8 ch_num, u8* data_value);	
	void (*write_kcs_data_out) (u8 ch_num, u8 data_value);
	void (*kcs_interrupt_enable_user) (u8 ch_num); 
	void (*kcs_interrupt_disable_user) (u8 ch_num);
	void (*kcs_set_obf_status)(u8 ch_num);
} kcs_hal_operations_t;

typedef struct
{
	int (*process_kcs_intr) (int dev_id, unsigned char ch_num);
	int (*is_hw_test_mode) (void);
} kcs_core_funcs_t;

#endif /* KCS_H */
