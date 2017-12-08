/****************************************************************
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2005-2007, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        6145-F, Northbelt Parkway, Norcross,                **
 **                                                            **
 **        Georgia - 30071, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 ****************************************************************
 ****************************************************************
 ****************************************************************
 *
 * libipmi_fru.h
 ******************************************************************/

#ifndef _LIBIPMI_FRU_

#define _LIBIPMI_FRU_


#include "libipmi_session.h"
#include "libipmi_errorcodes.h"
#include "libipmi_AppDevice.h"
#include "libipmi_StorDevice.h"
#include "libipmi_sensor.h"
#include "libipmi_XportDevice.h"
#include "IPMI_SDRRecord.h"
#include "IPMI_FRU.h"


typedef struct
{
	int present; //! 1 means yes..
	char MfrName[60];
	char ProductName[60];
	char ProductVersion[60];
	char ProductPartNum[60];
	char ProductSerialNum[60];
	char AssetTag[60];
	char FRUFileID[60];

	//! Custom fileds.......

}ProductInfo;

typedef struct
{
	int present; //! 1 means yes..
	char BoardMfr[60];
	char Language[60];
	char BoardProductName[60];
	char BoardSerialNum[60];
	char BoardPartNum[60];
	char FRUFileID[60];

	//! Custom fileds.......

}BoardInfo;

typedef struct
{
	int present; //! 1 means yes..
	uint8 ChassisType;
	char ChassisPartNum[60];
	char ChassisSerialNum[60];

	//! Custom fileds.......
	char GUID[60]; //! chassis info area offset 36
}ChassisInfo;

//! FRU Data
typedef struct fru_t
{
	uint8 fruDeviceID;
	ProductInfo pInfo;
	BoardInfo bInfo;
	ChassisInfo cInfo;

}FRU_T;


LIBIPMI_API uint16 LIBIPMI_HL_GetFRUData(IPMI20_SESSION_T *pSession,
					u8 fru_device_id,
				  	u32* p_fru_size,
				  	FRUData_T* fru_data,
					int timeout);

void LIBIPMI_HL_FreeFRUData(FRUData_T* fru_data);


#endif
