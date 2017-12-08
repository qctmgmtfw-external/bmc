/*****************************************************************
 *****************************************************************
 ***                                                            **
 ***    (C)Copyright 2005-2006, American Megatrends Inc.        **
 ***                                                            **
 ***            All Rights Reserved.                            **
 ***                                                            **
 ***        6145-F, Northbelt Parkway, Norcross,                **
 ***                                                            **
 ***        Georgia - 30071, USA. Phone-(770)-246-8600.         **
 ***                                                            **
 *****************************************************************
 *****************************************************************
 ******************************************************************
 *
 * fru.c
 * fru functions.
 *
 *  Author: Rama Bisa <ramab@ami.com>
 *
 ******************************************************************/
#ifndef PDK_FRU_H
#define PDK_FRU_H
#include "Types.h"
#include "IPMI_SDRRecord.h"

#define MAX_ID_STR_FILE_NAME 128

typedef struct
{
    INT8U   DeviceID;       /* FRU Device ID                                */
    INT8U   IsInternalFRU;  /* Flag to hide FRU					*/
    INT8U   Type;           /* FRU type NVR/EEPROM/....                     */
    INT16U  Size;           /* FRU Device size                              */
    INT8U   AccessType;     /* FRU device access type BYTE/WORD             */
    INT8S   NVRFile[MAX_ID_STR_FILE_NAME];        /* FRU file name if FRU is NVR                  */
    INT8U   BusNumber;      /* Bus number if FRU is EEPROM                  */
    INT8U   SlaveAddr;      /* FRU device slave address if FRU is EEPROM    */
    INT8U   DeviceType;     /* FRU device type if FRU is EEPROM             */
    INT16U  Offset;         /* FRU start offset in a physical FRU           */
    INT8S   FRUName[MAX_ID_STR_LEN]; /* FRU Name                 */
} FRUInfo_T;

typedef struct
{
    UINT8  BusNo;          /* FRU Device Bus ID */
    UINT8  SlaveAddr ;     /* FRU Slave address */
    UINT8  FRU_ID;         /* FRU Device ID */
    UINT16 FRU_Size;       /* Size of the EEPROM */
    UINT16 FRU_Offset;     /* Starting offset in the EEPROM */
    UINT8  PageSize;       /* Page size */
} FRUConfig;

typedef struct
{
    INT8U FRUID;
    INT8U SlaveAddr;
    INT8U BMCInst;
} FRUAccess_T;

#define MAX_PDK_FRU_SUPPORTED 10

#define FRU_TYPE_NVR                    1
#define FRU_TYPE_EEPROM                 2
#define FRU_TYPE_RAM  					3
#define FRU_FILE_SIZE                   256
#define FRU_RAM_SIZE                    0xA000 //40K
#define DEV_ACCESS_MODE_IN_BYTES        0X00

#define READ_RAM 		1
#define WRITE_RAM 		2
#define FRU_DT_OEM_ZS 	0xC0



extern int PDK_GetFRUAreaInfo (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes, int BMCInst);
extern int PDK_ReadFRUData (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes, int BMCInst);
extern int PDK_WriteFRUData (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes, int BMCInst);
extern int PDK_InitFRU (int BMCInst);
#if 0
extern int PDK_RegisterFRU (FRUInfo_T *pFRUInfo, int BMCInst);
#endif
int PDK_RegisterAllFRUs(int BMCInst);
extern int PDK_DCMIGetFRUDeviceID (INT8U* pDCMIFRUId, int BMCInst);
extern int PDK_DCMIReadFRU (INT8U DCMIFRUId, INT8U Offset, INT8U Len, INT8U* pReadBuf, int BMCInst);
extern int PDK_DCMIWriteFRU (INT8U DCMIFRUId, INT8U Offset, INT8U Len, INT8U* pReadBuf, int BMCInst);
#endif /**PDK_FRU_H*/



