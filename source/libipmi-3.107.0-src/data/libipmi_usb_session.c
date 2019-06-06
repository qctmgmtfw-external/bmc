/*****************************************************************
******************************************************************
***                                                            ***
***        (C)Copyright 2008, American Megatrends Inc.         ***
***                                                            ***
***                    All Rights Reserved                     ***
***                                                            ***
***       5555 Oakbrook Parkway, Norcross, GA 30093, USA       ***
***                                                            ***
***                     Phone 770.246.8600                     ***
***                                                            ***
******************************************************************
******************************************************************
******************************************************************
* 
* Filename: libipmi_usb_session.c
*
* Description: Contains implementation of session establishment
*   functions to BMC thru USB medium
*
* Author: Anurag Bhatia
*
******************************************************************/
#include "platform.h"
#include "libipmi_session.h"
#include "libipmi_usb.h"
#include <stddef.h>
#include "dbgout.h"

#if LIBIPMI_IS_OS_WINDOWS()
#include "g2cdhostwindows.h"
#elif LIBIPMI_IS_OS_LINUX()
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/file.h>
#include <fcntl.h>
#include <scsi/sg.h>
#include <netinet/in.h>

#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <dirent.h>

#include <time.h>
#include "unix.h"

#include <sys/ioctl.h>
#include <scsi/scsi_ioctl.h>
#include <scsi/scsi.h>

#include "hostconf.h"
#include "libipmi_scsi.h"
#endif


#define  mac2short(x) 	((uint16) (((uint16)(x) >> 8) | ((uint16)(x) << 8)))
#define  mac2long(x)	( ((x) >> 24) | ((x) << 24) | (((x) & 0x00ff0000) >> 8) | (((x) & 0x0000ff00) << 8))

int SendDataToUSBDriver(IPMI20_SESSION_T *pSession, char *ReqBuffer, unsigned int ReqBuffLen, unsigned char *ResBuffer, unsigned int *ResBuffLen, unsigned int timeout);
int WaitForCommandCompletion(IPMI20_SESSION_T *pSession,CONFIG_CMD* pG2CDCmdHeader,uint32 timeout,uint32 DataLen);
void InitCmdHeader(CONFIG_CMD* pG2CDCmdHeader);
int WriteSplitData(IPMI20_SESSION_T *pSession,char* Buffer,char Sector, uint32 NumBytes,uint32 timeout);
int ReadSplitData(IPMI20_SESSION_T *pSession,char* Buffer, char Sector, uint32 NumBytes);
int FindG2CDROM(IPMI20_SESSION_T *pSession);

//extern void select_sleep(time_t sec,suseconds_t usec);

#if LIBIPMI_IS_OS_WINDOWS()
uint32 SendScsiCmd(HANDLE hCD,
						  void* cdb_buf,unsigned char cdb_len,
						  void* data_buf,uint32* data_len,
						  int direction, long timeout);
HANDLE OpenDrive(char DriveLetter);
int IsG2Drive(HANDLE hCD);
int WriteCD(HANDLE hCD, char Sector, char *Buffer,unsigned int timeout,uint32 DataLen); 
int ReadCD(HANDLE hCD, char Sector, char *Buffer,uint32 DataLen); 
#else
int WriteCD(int cd_desc,char CmdData, char *Buffer,unsigned int timeout,uint32 DataLen); 
int ReadCD(int cd_desc,char CmdData, char *Buffer,uint32 DataLen);
//extern int scsiProbe(int probeFlags,char *dev);
extern int scsiProbeNew(int* numdev,int *devarray);
int OpenCD(IPMI20_SESSION_T *pSession,char *CDName);
int IsG2Drive(int cd_desc);
int AMI_SPT_CMD_RecvCmd(int cd_desc, char *Buffer, char type, uint16 buflen);
int AMI_SPT_CMD_SendCmd(int cd_desc, char *Buffer, char type, uint16 buflen,unsigned int timeout);
#endif

#if LIBIPMI_IS_OS_LINUX()
void select_sleep(time_t sec,suseconds_t usec)
{
    struct timeval tv;

    tv.tv_sec = sec;
    tv.tv_usec = usec;

    while(select(0, NULL, NULL, NULL, &tv) < 0 && errno == EINTR);
}
#endif

int OpenUSB(IPMI20_SESSION_T *pSession, char *pszDevice)
{
	
#if LIBIPMI_IS_OS_WINDOWS()
	SECURITY_DESCRIPTOR    SD;
	SECURITY_ATTRIBUTES     sa;
#endif

	//DEBUG_PRINTF("OpenUSB\n");
	if(FindG2CDROM(pSession) == 0)
	{
#if LIBIPMI_IS_OS_LINUX()
		if(0)
	    {
		    pszDevice=pszDevice; /* -Wextra: Flag added for strict compilation */
	    }
//		DEBUG_PRINTF("\nproblem finding Virtual CDROM DRIVE. \nMake sure the firmware is up,sg driver is loaded\n");
#elif LIBIPMI_IS_OS_WINDOWS()
//		DEBUG_PRINTF("\nproblem finding Virtual CDROM DRIVE. \nMake sure the firmware is up\n");
#endif


		return 1;
	}
	else
	{
		//DEBUG_PRINTF("Found G2 CDROM DRIVE\n");
	}
#if LIBIPMI_IS_OS_WINDOWS()
	/*Though FindG2CDROM uses AMICMD_ID we do not need the mutex on that cmd since
	it is a stateless cmd.so the mutx can remain here lik ein the hard disk*/

	/*************Create proper security attributes for this object******/
	//create a null security descriptor so that
	//even normal client programs can acces this object
	//if the object is created ina service there are problems
	//accessing it from a client. Putting a null DACL while
	//creating the object makes this object accessible even
	//from normal user mode programs

	if (!InitializeSecurityDescriptor(&SD, SECURITY_DESCRIPTOR_REVISION))
	{
		DEBUG_PRINTF("error in initializing security desc");
	}
		// Add a NULL DACL to the security descriptor..
	if (!SetSecurityDescriptorDacl(&SD, TRUE, (PACL) NULL, FALSE))
	{
		DEBUG_PRINTF("Error while setting null dacl");
	}
	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = &SD;
	sa.bInheritHandle = TRUE;

	/*************Create proper security attributes for this object******/
/*	//create the mutex object
    CMutex *m_pg2Mutex;
	m_pg2Mutex = new CMutex(FALSE,"G2_HD_ACCESS_MUTEX",&sa);
    hpg2Mutex = (HANDLE)m_pg2Mutex; //  save in object

	if(m_pg2Mutex == NULL)
	{
		DEBUG_PRINTF("Mutex allocation failure!!\n");
		return 3;
	}
*/
	//DEBUG_PRINTF("OpenUSB Success\n");
#endif
	return 0;

}

uint16 Create_IPMI20_USB_Session(IPMI20_SESSION_T *pSession, char *pszDevice,
								 uint8 byResAddr)
{
	uint8			byRet;

	//DEBUG_PRINTF("Creating USB Session...\n");
	/* initialize to zero */
	memset(pSession, 0, sizeof(IPMI20_SESSION_T) );

	/* allocate memory for USB session */
	pSession->hUSBSession = (IPMI20_USB_SESSION_T*)malloc( sizeof(IPMI20_USB_SESSION_T) );
	if(pSession->hUSBSession == NULL)
	{
		TCRIT("Error in allocating memory \n");
		return -1;
	}

	/* Open USB */
	byRet = OpenUSB(pSession, pszDevice);
	/* return if error */
	if(byRet != 0)
	{
//		DEBUG_PRINTF("OpenUSB failed\n");
		return STATUS_CODE(MEDIUM_ERROR_FLAG, byRet);
	}
	
	pSession->bySessionStarted				= SESSION_STARTED;
	pSession->byMediumType					= USB_MEDIUM;
	pSession->hUSBSession->byResAddr		= byResAddr;

	
	return LIBIPMI_E_SUCCESS;
}


int CloseCD(IPMI20_SESSION_T *pSession)
{
#if LIBIPMI_IS_OS_WINDOWS()
	if(pSession->hUSBSession->hUSBDevice != INVALID_HANDLE_VALUE)
	{
		//DEBUG_PRINTF("Closing CD...\n");
		CloseHandle(pSession->hUSBSession->hUSBDevice);
	}
	pSession->hUSBSession->hUSBDevice = INVALID_HANDLE_VALUE;
#else
	if(pSession->hUSBSession->hUSBDesc != -1) 
	{
		close(pSession->hUSBSession->hUSBDesc);
	}
	pSession->hUSBSession->hUSBDesc = -1; 
#endif
	return 0;
}

uint16 Close_IPMI20_USB_Session( IPMI20_SESSION_T *pSession )
{
	//DEBUG_PRINTF("Closing USB Session...\n");
	CloseCD(pSession);
	return LIBIPMI_E_SUCCESS;
}

uint16 Send_RAW_IPMI2_0_USB_Command(IPMI20_SESSION_T *pSession,
										uint8 byNetFnLUN, uint8 byCommand,
										uint8 *pbyReqData, unsigned int dwReqDataLen,
										uint8 *pbyResData, unsigned int *pdwResDataLen,
										int	timeout)
{
	uint8 byRet = 0;
	char ReqBuff[MAX_REQUEST_SIZE] = {0};
	IPMIUSBRequest_T* pReqPkt = (IPMIUSBRequest_T*)ReqBuff;

    int retries = 0;

    /* Timeout for Activate Flash mode command for YAFU */
    if( byNetFnLUN == 0xC8 && byCommand == 0x10)
    {
        timeout = 60000;
    }
	
	/********** FORM IPMI PACKET *****************/
	pReqPkt->byNetFnLUN = byNetFnLUN;
	pReqPkt->byCmd = byCommand;
	if(dwReqDataLen)
	{
		if (dwReqDataLen <= sizeof(pReqPkt->byData))
		{
			memcpy(pReqPkt->byData, pbyReqData, dwReqDataLen);
		}
		else
		{
			TCRIT("Buffer overflow in %s\n",__FUNCTION__);
			return 0;
		}
	}
	/********** SEND DATA TO USB ******************/
     while (retries < 3)
     {
        retries++;
        byRet = SendDataToUSBDriver(pSession,
								    ReqBuff, 2 + dwReqDataLen,
								    pbyResData, pdwResDataLen,timeout);

        if (byRet == 0) break;

     }
     if (retries == 3)
     {
	    DEBUG_PRINTF("Error while sending command using SendDataToUSBDriver\n");
	    return STATUS_CODE(MEDIUM_ERROR_FLAG, byRet);
     }
     if(*pbyResData != CC_NORMAL)
     {
         return STATUS_CODE(IPMI_ERROR_FLAG, *pbyResData);
     }
	return LIBIPMI_E_SUCCESS;
}

int SendDataToUSBDriver(IPMI20_SESSION_T *pSession, char *ReqBuffer, unsigned int ReqBuffLen, unsigned char *ResBuffer, unsigned int *ResBuffLen, unsigned int timeout)
{
	char CmdHeaderBuffer[sizeof(CONFIG_CMD)];
	int retVal;
	int waitretval = 0;
	unsigned int to = 0;
	uint32 DataLen = 0; 
	
	CONFIG_CMD* pG2CDCmdHeader = (CONFIG_CMD*)CmdHeaderBuffer;

		
	/*FillHeader*/
	InitCmdHeader(pG2CDCmdHeader);

	/*Set command number*/
	pG2CDCmdHeader->Command = CMD_RESERVED; /* HARD CODED */

	/*Fill Lengths*/
	pG2CDCmdHeader->DataOutLen = *ResBuffLen;
	pG2CDCmdHeader->DataInLen = ReqBuffLen;

	if(!timeout)
		to = 3000;
	DataLen = sizeof(CONFIG_CMD); 
#if LIBIPMI_IS_OS_WINDOWS()
	/***Write the command header to hard disk****/
	if(WriteCD(pSession->hUSBSession->hUSBDevice, SCSI_AMIDEF_CMD_SECTOR, (char*)(pG2CDCmdHeader),to,DataLen) == -1)  
		return -1;
#else
	/* Replace WriteCD for Linux here */
       if(WriteCD(pSession->hUSBSession->hUSBDesc, SCSI_AMIDEF_CMD_SECTOR, (char*)(pG2CDCmdHeader),to,DataLen) == -1)
		return -1;
#endif

	/***Write the data to hard disk***/
	if((retVal = WriteSplitData(pSession,ReqBuffer, SCSI_AMIDEF_DATA_SECTOR, ReqBuffLen,timeout)) != 0) 
    {
		
        if(retVal == -1) 
			return retVal;
        return -1;
    }
#if LIBIPMI_IS_OS_LINUX()
	if(!timeout)
		return 0;
#endif
		
	
	
	/*Read Status now*/
	waitretval = WaitForCommandCompletion(pSession,pG2CDCmdHeader,timeout,DataLen);  
	if(waitretval != 0)
	{
		DEBUG_PRINTF("waitforcmdcomplete failed\n");
		return (0-waitretval);
	}
	else
	{
		//DEBUG_PRINTF("WaitForCommandCompletion SUCCESS\n");
	}

	switch(pG2CDCmdHeader->Status)
	{
		case ERR_SUCCESS:
			{
			*ResBuffLen = pG2CDCmdHeader->DataOutLen;
			
			if(ReadSplitData(pSession,(char *)ResBuffer, SCSI_AMIDEF_DATA_SECTOR, pG2CDCmdHeader->DataOutLen) != 0) 
			{
				return -1;
			}

			/*a read at the end to see if read of data itself had any problems*/
			/*this read verifies that the previous read above was ok*/
			DataLen = sizeof(CONFIG_CMD); 
#if LIBIPMI_IS_OS_WINDOWS()
			ReadCD(pSession->hUSBSession->hUSBDevice, SCSI_AMIDEF_CMD_SECTOR,(char*)(pG2CDCmdHeader),DataLen);  
#else
			ReadCD(pSession->hUSBSession->hUSBDesc, SCSI_AMIDEF_CMD_SECTOR,(char*)(pG2CDCmdHeader),DataLen); 
#endif

			break;
			}
		case ERR_BIG_DATA:
			DEBUG_PRINTF("Too much data\n");
			break;
		case ERR_NO_DATA:
			DEBUG_PRINTF("Too little data\n");
			break;
		case ERR_UNSUPPORTED:
			DEBUG_PRINTF("Unsupported command\n");
			break;
		default:
			DEBUG_PRINTF("Unknown status\n");
			break;
	}
	
	return pG2CDCmdHeader->Status;
}

//////////////////////////////////////////////////////////////////////////////
// FindG2CDROM()
//	Return value:
//		On success - Returns 1 
//		On error   - Returns 0
//////////////////////////////////////////////////////////////////////////////
int FindG2CDROM(IPMI20_SESSION_T *pSession)
{
#if LIBIPMI_IS_OS_WINDOWS()
	static TCHAR Buffer[1024*64];
	TCHAR Mapping[1024];
	DWORD Size;
	TCHAR *pBuff = &Buffer[0];
	size_t Len;

	
	Size = QueryDosDevice(NULL,pBuff,1024*64);

	while (*pBuff)
	{
		Len = strlen(pBuff);
		if (pBuff[1] == ':')
		{
			Size = QueryDosDevice(pBuff,&Mapping[0],1024);
			if(strstr(Mapping,"\\Device\\CdRom"))
			{
				pSession->hUSBSession->hUSBDevice = OpenDrive(pBuff[0]);
				if(pSession->hUSBSession->hUSBDevice != NULL)
				{
					if(!IsG2Drive(pSession->hUSBSession->hUSBDevice))
					{
						//DEBUG_PRINTF("FindG2 CDROM DRIVE - Success\n");
						return 1;
					}
					CloseHandle(pSession->hUSBSession->hUSBDevice);
				}
			}
		}
		pBuff+=(Len+1);
	}/* while (*pBuff)*/
#else // LIBIPMI_IS_OS_LINUX()
	int err = 0;
	char device[256];
	int devarray[8];
	int numdev = 8;
	int iter;
	//err = scsiProbe(0,device);
	err = scsiProbeNew(&numdev,devarray);
	if(err == 0 && numdev > 0)
	{
		for(iter =0;iter < numdev;iter++)
		{
			sprintf(device,"/dev/sg%d",devarray[iter]);
		/* Success */
		if(!OpenCD(pSession,device))
		{
			/* Success */
			if (!IsG2Drive(pSession->hUSBSession->hUSBDesc))
			{
				/* Success */
				return 1;
			}
			close(pSession->hUSBSession->hUSBDesc);
			}
		}
	}
	else
	{
		//DEBUG_PRINTF("\nUnable to find Virtual CDROM Device\n");
	}
#endif

	return 0;
}

int WaitForCommandCompletion(IPMI20_SESSION_T *pSession,CONFIG_CMD* pG2CDCmdHeader,uint32 timeout,uint32 DataLen) 
{
	uint32 TimeCounter = 0;
	
	do
	{
		
#if LIBIPMI_IS_OS_WINDOWS()
		if(ReadCD(pSession->hUSBSession->hUSBDevice, SCSI_AMIDEF_CMD_SECTOR,(char*)(pG2CDCmdHeader),DataLen) == -1) 
#else
		if(ReadCD(pSession->hUSBSession->hUSBDesc, SCSI_AMIDEF_CMD_SECTOR,(char*)(pG2CDCmdHeader),DataLen) == -1)
#endif
		{
			DEBUG_PRINTF("ReadCD returned ERROR\n");
			return 1;
		}

		if(pG2CDCmdHeader->Status & IN_PROCESS)
		{
			
#if LIBIPMI_IS_OS_WINDOWS()
			Sleep(1);
#else
			select_sleep(0,1000);
#endif
			if((signed)timeout >= 0)
			{

				TimeCounter++;
				if(TimeCounter == (timeout+1)) /*timeout+1 because we have already checked..so we honor atcual timeout by comparing to +1*/
				{
					return 2; /*different condition for timeout*/
				}
			}
		}
		else
		{
			//DEBUG_PRINTF("Command completed\n");
			break;
		}
	}
	while(1);

	return 0;
}

void InitCmdHeader(CONFIG_CMD* pG2CDCmdHeader)
{
	memset(pG2CDCmdHeader,0,sizeof(CONFIG_CMD));
	strncpy((char*)pG2CDCmdHeader->BeginSig,BEGIN_SIG,BEGIN_SIG_LEN);
	
}

int WriteSplitData(IPMI20_SESSION_T *pSession, char* Buffer,char Sector, uint32 NumBytes, uint32 timeout)
{
	uint32 BytesWritten = 0;
    int retVal;
	
	if(NumBytes == 0) return 0;
       
	while(BytesWritten < NumBytes)
	{
#if LIBIPMI_IS_OS_WINDOWS()
        if((retVal = WriteCD(pSession->hUSBSession->hUSBDevice, Sector, (Buffer + BytesWritten),timeout,NumBytes)) != 0)  
			return retVal;
#else
        if((retVal = WriteCD(pSession->hUSBSession->hUSBDesc, Sector, (Buffer + BytesWritten),timeout,NumBytes)) != 0)
			return retVal;
#endif
		BytesWritten += NumBytes; 
	}

	return 0;
}

int ReadSplitData(IPMI20_SESSION_T *pSession, char* Buffer, char Sector, uint32 NumBytes)
{
	uint32 BytesRead = 0;

	if(NumBytes == 0) return 0;

	while(BytesRead < NumBytes)
	{
#if LIBIPMI_IS_OS_WINDOWS()
		if(ReadCD(pSession->hUSBSession->hUSBDevice, Sector,(Buffer+BytesRead),NumBytes) == -1) 
			return 1;
#else
		if(ReadCD(pSession->hUSBSession->hUSBDesc, Sector,(Buffer+BytesRead),NumBytes) == -1)
			return 1;
#endif
		BytesRead += NumBytes; 
	}
   
	return 0;
}

/********************************************************/
/********************************************************/
/***************** WINDOWS SPECIFIC *********************/
/********************************************************/
/********************************************************/
#if LIBIPMI_IS_OS_WINDOWS()
PUCHAR AllocateAlignedBuffer(unsigned int size, unsigned int Align)
{
    PUCHAR ptr;

    UINT_PTR    Align64 = (UINT_PTR)Align;

    if (!Align) 
	{
       ptr = (PUCHAR)malloc(size);
	}
    else 
	{
       ptr = (PUCHAR)malloc(size + Align);
       ptr = (PUCHAR)(((UINT_PTR)ptr + Align64) & ~Align64);
    }
    if (ptr == NULL) 
	{
       DEBUG_PRINTF("Memory allocation error.  Terminating program\n");
       exit(1);
    }
    else 
	{
       return ptr;
    }
}

int AMI_SPT_CMD_Identify(HANDLE hCD,char* Signature)
{
	char* SigBuffer;
	DWORD ReqdDataLen = 10;
	uint32 RetVal;

	SCSI_COMMAND_PACKET IdPkt = {0};
	IdPkt.OpCode = SCSI_AMICMD_ID;

	SigBuffer = (char*)AllocateAlignedBuffer(10,1); //10 bytes sig size
	RetVal = SendScsiCmd(hCD,&IdPkt,10,SigBuffer,&ReqdDataLen,SCSI_IOCTL_DATA_IN,1000 );
	if(RetVal != 0)
	{
		//DEBUG_PRINTF("Error while sending ID command in send scsi cmd\n");
	}

	strncpy(Signature,SigBuffer,10);
	free(SigBuffer);

	return RetVal;
}

int IsG2Drive(HANDLE hCD)
{

	char Signature[10];

	//DEBUG_PRINTF("IsG2Drive\n");

	if(AMI_SPT_CMD_Identify(hCD,Signature) == 0)
	{
		if(strcmp(Signature,"$$$AMI$$$") == 0)
			return 0;
		else
			return 1;
	}
	else
		return 1;
}

HANDLE OpenDrive(char DriveLetter)
{
	char *DriveString = "\\\\.\\%c:";
	char DeviceName[16];

	//DEBUG_PRINTF("DriveLetter: %c\n",DriveLetter);
	sprintf(DeviceName,DriveString,DriveLetter);
	return CreateFile (DeviceName, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE,
								NULL, OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL, NULL);
}

uint32 SendScsiCmd(HANDLE hCD,
						  void* cdb_buf,unsigned char cdb_len,
						  void* data_buf,uint32* data_len,
						  int direction,long timeout)
{
//	int ind = 0;
	SCSI_PASSTHROUGH_DIRECT_WITH_SENSEBUFFER sptdwb;
	unsigned int returned;

	//DEBUG_PRINTF("SendScsiCmd, handle = %x\n",hCD);
	memset(&sptdwb,0,sizeof(sptdwb));
	sptdwb.sptd.Length = sizeof(SCSI_PASS_THROUGH_DIRECT);

	/*ignore path id, target id and Lun since someone down below will do it
	because we have the drive handle*/
	sptdwb.sptd.CdbLength = cdb_len;
	sptdwb.sptd.DataIn = direction;
	sptdwb.sptd.DataBuffer = data_buf;
	sptdwb.sptd.DataTransferLength = *data_len;


	/*
  	 *	Woraround for Windows Bug (KB 239573)  
  	 *	http://support.microsoft.com/kb/259573
  	 *	BUG: SCSI Pass Through Fails with Invalid User Buffer Error
	 */
	if (sptdwb.sptd.DataTransferLength == 1)
		sptdwb.sptd.DataTransferLength = 2;

	sptdwb.sptd.SenseInfoLength = 0;
	sptdwb.sptd.SenseInfoOffset = offsetof(SCSI_PASSTHROUGH_DIRECT_WITH_SENSEBUFFER,ucSenseBuf);


	sptdwb.sptd.TimeOutValue = timeout;

	if(!timeout)
		sptdwb.sptd.TimeOutValue = 3000;

	memcpy(sptdwb.sptd.Cdb,cdb_buf,cdb_len);

	if(!DeviceIoControl(hCD,IOCTL_SCSI_PASS_THROUGH_DIRECT,
						&sptdwb,
						sizeof(sptdwb),
						&sptdwb,
						sizeof(sptdwb),
						&returned,
						NULL))
	{
		if(!timeout)
			return 0;

		//DEBUG_PRINTF("DevIOCtrl sptdwb.sptd.Cdb[0] is 0x%x\n",sptdwb.sptd.Cdb[0]);
		if(sptdwb.sptd.Cdb[0] != 0xee)
		{
			int x = GetLastError();
			//TRACE("last error is as follows %ld\n",GetLastError());
/*
			DEBUG_PRINTF("Error In DevieIoControl for sending SCSI Command..dump of CDB Follows!!!\n");
			for(ind = 0;ind < cdb_len;ind++)
			{
				DEBUG_PRINTF("0x%x ",sptdwb.sptd.Cdb[ind]);
			}

			DEBUG_PRINTF("\n");
*/
		}
		return 0xFF;
	}
	*data_len = sptdwb.sptd.DataTransferLength;
	return sptdwb.sptd.ScsiStatus;
}

int ReadCD(HANDLE hCD, char Sector, char *Buffer,uint32 DataLen) 
{
	DWORD ReqdDataLen = DataLen; 
	uint32 RetVal;
	unsigned int RetryCt = 1;
	SCSI_COMMAND_PACKET scsicmd= {0};
	unsigned char *pReadCDPkt = (unsigned char *) AllocateAlignedBuffer(DataLen,1);  

	if(pReadCDPkt == NULL)
	{
		DEBUG_PRINTF("Memory allocation error for READ CD packet\n");
		return -1;
	}

	while(RetryCt <= 3)
	{
		/*Setup SCSI header and other stuff**/
		ReqdDataLen = DataLen; 
		scsicmd.OpCode = SCSI_AMICMD_CURI_READ;
		/***Note : Setting Length inside CDB to 1 is ok for now as we dont interpret it
		and neither does the OS. As per SCSI specs this is upto the particular cmd.
		We may want to take a look at this particular field behavior across other 
		Operating Systems in case of problems**/
		scsicmd.CmdLen.Cmd10.Length = mac2short(1);
		scsicmd.Lba = mac2long(Sector);

		RetVal = SendScsiCmd(hCD,&scsicmd,10,pReadCDPkt,&ReqdDataLen,SCSI_IOCTL_DATA_IN,1000);

		if(RetVal == 0)
		{
			memcpy(Buffer,pReadCDPkt,DataLen); 
			break;
		}
		else
		{
			RetryCt++;
			Sleep(2000);
			if(RetryCt == 3)
				DEBUG_PRINTF("SendScsiCommand failure even after 3 retries in ReadCD!!\n");
			continue;
		}
	}

	free(pReadCDPkt);
	return RetVal;
}

//////////////////////////////////////////////////////////////////////////////
// WriteCD()
// Parameter :
//		Sector	- Indicates Data to be Send to card for Data/Command
//		Buffer  - Pointer to char Buffer
// Return value:
//		On Success 	- Returns Number of bytes send
//		On Error	- Return -1
//////////////////////////////////////////////////////////////////////////////
int WriteCD(HANDLE hCD, char Sector, char *Buffer,unsigned int timeout,uint32 DataLen) 
{
	DWORD SendDataLen = DataLen;  
	uint32 RetVal;
	unsigned int RetryCt = 1;
	SCSI_COMMAND_PACKET scsicmd = {0};
	/*it is safer to have the data buffer aligned even for writes*/
	unsigned char *pWriteCDPkt = (unsigned char *) AllocateAlignedBuffer(DataLen,1); 
	
	
	if(pWriteCDPkt == NULL)
	{
		DEBUG_PRINTF("Memory allocation error for WRITE CD packet\n");
		return -1;
	}

	/*copy to aligned buffer ..non aligned buffers may cause problems as per DDK*/
	memcpy(pWriteCDPkt,Buffer,DataLen);

	while(RetryCt <= 3)
	{
		//DEBUG_PRINTF("RetryCt = %d\n",RetryCt);
		SendDataLen = DataLen; 
		/***setup headesr and stuff***/
		scsicmd.OpCode = SCSI_AMICMD_CURI_WRITE;
		/***Note : Setting Length inside CDB to 1 is ok for now as we dont interpret it
		and neither does the OS. As per SCSI specs this is upto the particular cmd.
		We may want to take a look at this particular field behavior across other 
		Operating Systems in case of problems**/
		scsicmd.CmdLen.Cmd10.Length = mac2short(1);
		scsicmd.Lba = mac2long(Sector);

		RetVal = SendScsiCmd(hCD,&scsicmd,10,pWriteCDPkt,&SendDataLen,SCSI_IOCTL_DATA_OUT,timeout);
		if(RetVal == 0)
		{
			break;
		}
		else
		{
			RetryCt++;
			Sleep(2000);
            if(RetryCt == 3) {
                DEBUG_PRINTF("SendScsiCommand failure even after 3 retries in WriteCD!!\n");
                return -1;
            }

			continue;
		}
	}


	free(pWriteCDPkt);
	return RetVal;
}
#endif

/********************************************************/
/********************************************************/
/****************	LINUX SPECIFIC	 ********************/
/********************************************************/
/********************************************************/
#if LIBIPMI_IS_OS_LINUX()
int sendscsicmd_SGIO(int cd_desc, unsigned char* cdb_buf,unsigned char cdb_len,
			void* data_buf,unsigned int* data_len,
			int direction,
			void* sense_buf,unsigned char slen,
			unsigned int timeout)
{
	sg_io_hdr_t io_hdr;

    /* Prepare command */
	memset(&io_hdr, 0, sizeof(sg_io_hdr_t));
        io_hdr.interface_id = 'S';

	//io_hdr.cmd_len = cdb_len;
	io_hdr.cmd_len = cdb_len;

	/* io_hdr.iovec_count = 0; */  /* memset takes care of this */

	/*transfer direection and length*/
	io_hdr.dxfer_direction = direction;
   	io_hdr.dxfer_len = *data_len;

	/*data buffer to hold data*/
	io_hdr.dxferp = data_buf;

	/*where is the command*/
	io_hdr.cmdp = cdb_buf;

	/*sense buffer*/
	io_hdr.sbp = (unsigned char *)sense_buf;
        io_hdr.mx_sb_len = slen; /*we dont want sense info*/

	/*timeout*/
	io_hdr.timeout = timeout;
	
	if(!timeout)
		io_hdr.timeout = 20000;

	if (ioctl(cd_desc,SG_IO,&io_hdr) < 0)
	{
	        DEBUG_PRINTF("sendscsicmd_SGIO:  SG_IO ioctl error\n");
        	return 1;
   	}
	else
	{
	       if(io_hdr.status != 0) 
	       {
		   	return 1;
	       }
//		DEBUG_PRINTF("ioctl came back ok\n");
	}

	/* RESET THE CARD CASE */
	if(!timeout)
		return 0;

	if ((io_hdr.info & SG_INFO_OK_MASK) != SG_INFO_OK)
	{
//       	DEBUG_PRINTF("sendscsicmd_SGIO: No OK\n");
	}
	else
	{
//		DEBUG_PRINTF("sendscsicmd_SGIO : OK GOT!!\n");
		return 0;
	}

	return 1;
}

int ReadCD(int cd_desc,char CmdData, char *Buffer,uint32 DataLen)
{
	int ret;

	ret = AMI_SPT_CMD_RecvCmd(cd_desc, Buffer, CmdData,DataLen );  

	if ( ret != 0 ) {
		DEBUG_PRINTF("Error while reading CD-Drive\n");
		return -1;
	}

	return 0;
}

int WriteCD(int cd_desc,char CmdData, char *Buffer, unsigned int timeout,uint32 DataLen)
{
	int ret;

	ret = AMI_SPT_CMD_SendCmd(cd_desc, Buffer, CmdData,DataLen ,timeout);  
	if ( ret != 0 ) 
	{
		DEBUG_PRINTF("Error while writing to CD-Drive\n");
		return -1;
	}

	return 0;
}

int OpenCD(IPMI20_SESSION_T *pSession,char *CDName)
{
//	strcpy(CDName,"/dev/sg1");
	//DEBUG_PRINTF("OpenCD: Opening AMI Virtual CD at %s\n", CDName);
	pSession->hUSBSession->hUSBDesc = open(CDName, O_RDWR);
	if(pSession->hUSBSession->hUSBDesc == -1) 
	{
		DEBUG_PRINTF("OpenCD:Unable to open the device, %s\n",strerror(errno));
		return 1;
	}
	return 0;
}

int AMI_SPT_CMD_Identify(int cd_desc, char* szSignature)
{
	SCSI_COMMAND_PACKET IdPkt= {0,0,0,{{0,0,"0"}}};
	int ret;
	unsigned int siglen = 10;

	IdPkt.OpCode 	   = SCSI_AMICMD_ID;
	ret = sendscsicmd_SGIO(cd_desc, (unsigned char*)&IdPkt,
				10,szSignature,&siglen,SG_DXFER_FROM_DEV,
				NULL,0,5000);

	return ret;
}

int AMI_SPT_CMD_SendCmd(int cd_desc, char *Buffer, char type, uint16 buflen, unsigned int timeout)
{
	SCSI_COMMAND_PACKET	Cmdpkt;
	char 			sensebuff[32];
	int 			ret;
	unsigned int	pktLen;
    int             count = 3;
    
	memset(&Cmdpkt, 0, sizeof(SCSI_COMMAND_PACKET) );

	Cmdpkt.OpCode	= SCSI_AMICMD_CURI_WRITE;
	Cmdpkt.Lba	= htonl( type );
	Cmdpkt.CmdLen.Cmd10.Length = htons(1);

	pktLen = buflen;
    while(count > 0)
    {
    	ret = sendscsicmd_SGIO(cd_desc,(unsigned char*)&Cmdpkt,
				10,Buffer,&pktLen,SG_DXFER_TO_DEV,
				sensebuff,32,timeout);
        count--;
        if(ret == 0) break;
        else ret = -1;
    }

	return ret;
}

int AMI_SPT_CMD_RecvCmd(int cd_desc, char *Buffer, char type, uint16 buflen)
{
	SCSI_COMMAND_PACKET	Cmdpkt;
	char 			sensebuff[32];
	int 			ret;
	unsigned int	pktLen;
    int             count = 3;

	memset(&Cmdpkt, 0, sizeof(SCSI_COMMAND_PACKET) );
	
	Cmdpkt.OpCode	= SCSI_AMICMD_CURI_READ;
	Cmdpkt.Lba	= htonl( type );
	Cmdpkt.CmdLen.Cmd10.Length = htons(1);
	
	pktLen = buflen;
	while (count > 0)
	{
		ret = sendscsicmd_SGIO(cd_desc, (unsigned char*)&Cmdpkt,
					10,Buffer,&pktLen,SG_DXFER_FROM_DEV,
					sensebuff,32,5000);
		count--;
		if (0 == ret) break;
		else ret = -1;
	}
	
	return ret;
}

int IsG2Drive(int cd_desc)
{
	char szSignature[15];
	int ret;

	memset(szSignature, 0, 15);

	flock(cd_desc,LOCK_EX);
	ret = AMI_SPT_CMD_Identify(cd_desc, szSignature);
	flock(cd_desc,LOCK_UN);
	if ( ret != 0 ) 
	{
		DEBUG_PRINTF("IsG2Drive:Unable to send ID command to the device\n");
		return 1;
	}

	if ( strcmp( szSignature, "$$$AMI$$$") != 0 ) {
		DEBUG_PRINTF("IsG2Drive:Signature mismatch when ID command is sent\n");
		return 1;
	}

	return 0;
}

#endif
