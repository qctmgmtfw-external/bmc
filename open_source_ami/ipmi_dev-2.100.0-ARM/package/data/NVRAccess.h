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
 * nvram.h
 * NVRAM Functions
 *
 *  Author: Govind Kothandapani <govindk@ami.com>
 *			Basavaraj Astekar	<basavaraja@ami.com>
 *			Ravinder Reddy		<bakkar@ami.com>
 ******************************************************************/
#ifndef NVRAM_H
#define NVRAM_H



#include "Types.h"
#include "IPMI_Main.h"
#include "NVRData.h"
#include "PDKPARAccess.h"
#include "SharedMem.h"
#include "IPMI_OPMA.h"
#include "IPMI_AMISmtp.h"

#define IPMI_CONFIGS_FILE(Instance,filename)    \
sprintf(filename,"%s%d/%s",NV_DIR_PATH,Instance,"IPMIConfig.dat")

#define SENSORTHRESH_FILE(Instance,filename)   \
sprintf(filename,"%s%d/%s",NV_DIR_PATH,Instance,"SensorThresholds.dat");

#define WRITE_NVR               1
#define READ_NVR                2

#define FLUSH_PMC               1
#define FLUSH_SDR               2
#define FLUSH_SEL               3
#define FLUSH_FRU               4

extern int FlushStarted;

/**
*@fn GetSDRSELNVRAddr
*@brief Get SDR & SEL address from RAM
*@param NVRHanle Handler for NVRAM
*@param BMCInst - BMC Instance
*/
extern INT8U*  GetSDRSELNVRAddr(INT32U NVRHandle, int BMCInst);

/**
 * @macro  GetNVRUsrCfgAddr
 * @brief Gets NVR UsrCfg address from RAM
 * @param NVRHandle   -  Handle for NVRAM
 **/
extern INT8U* GetNVRUsrCfgAddr(INT32U NVRHandle, int BMCInst) ;


 /**
 * @fn VerifyChksum
 * @brief Verify checksum of a block
 * @param Addr	- Start address of block
 * @param Size	- Size of block
**/
extern int	VerifyChksum ( _FAR_ INT32U* Addr , INT16U Size );

 /**
 * @fn VerifyChksum
 * @brief Calculates checksum of a block
 * @param Addr	- Start address of block
 * @param Size	- Size of block
**/
extern void CalChksum( _FAR_ INT32U* Offset, INT16U Size);


 /**
 * @fn ReadWriteNVR
 * @brief Reads/Writes the Non volatile informations to/from file
 * @param FileName - File to Write or Read from.
 * @param pData    - Pointer to data.
 * @param Offset   - Offset in the file to Write of Read from.
 * @param Size     - Size of data to read/write.
 * @param Flag     - Flag to perform write or read operation.
**/
extern int ReadWriteNVR (char *FileName, INT8U* pData, INT32U Offset, INT16U Size, INT8U Falg);

/*
*@fn FlushIPMI
*@brief Writes IPMI configurations to NVRAM
*@param RAMAddr - Address in RAM to be flushed to NVRAM
*@param Offset- 
*@param Size - 
*@param BMCInst - BMC Instance
*/
extern int FlushIPMI(INT8U *RAMStartAddr,INT8U* RAMAddr,INT32U Offset,INT16U Size,int BMCInst);

/*
*@fn FlushChConfigs
*@brief Flushes Channel Configurations to IPMIConfig.dat file
*@param RAMAddr - Address in RAM to be flushed to NVRAM
*@param ChNum - Channel Number
*@param BMCInst - BMC Instance
*/
extern int FlushChConfigs(INT8U* RAMAddr,int ChNum,int BMCInst);

/**
*@fn GetChType
*@brief Retrieves the Corresponding Channel Type
*@param BMCInst - BMC Instance
*@param ChNum - Channel Number
*/
extern int GetChType(int BMCInst,int ChNum);

/**
 *@macro  FlushSDR
 *@brief Wirtes the SDR entries to NVRAM
 **/
#define FlushSDR(RAMAddr, Size, Ret, BMCInst)         \
do {                                                         \
    if(g_PDKHandle[PDKWRITESDR] != NULL)     \
    {                                                                           \
         Ret = ((int(*)(INT8U*,INT32U,INT16U,int))g_PDKHandle[PDKWRITESDR]) ((INT8U*)RAMAddr,(INT8U*)RAMAddr - (INT8U*)GetSDRSELNVRAddr(NVRH_SDR, BMCInst),Size, BMCInst);     \
    }                                                                            \
    else                                                                       \
    {                                                                           \
        IPMI_WARNING("Unable to FlushSDR to NVRAM \n");    \
        Ret = 0;												\
    }                                                                          \
}while(0);

/**
 *@macro  FlushSEL
 *@brief Wirtes the SEL entries to NVRAM
 **/
#define FlushSEL(RAMAddr, Size, Ret, BMCInst)         \
do {                                                         \
    if(g_PDKHandle[PDKWRITESEL] != NULL)     \
    {                                                                           \
         Ret = ((int(*)(INT8U*,INT32U,INT16U,int))g_PDKHandle[PDKWRITESEL]) ((INT8U*)RAMAddr, (INT8U*)RAMAddr - (INT8U*)GetSDRSELNVRAddr((pBMCInfo->IpmiConfig.SDRAllocationSize * 1024), BMCInst),Size, BMCInst);     \
    }                                                                            \
    else                                                                       \
    {                                                                           \
        IPMI_WARNING("Unable to FlushSEL to NVRAM \n");   \
        Ret = 0;											\
    }                                                                           \
}while(0);



#endif /* NVRAM_H */

