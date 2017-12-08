/*****************************************************************
*	File		:	WinNTMediaReader.cpp						 *
*	Author		:	Rajasekhar J(rajasekharj@amiindia.co.in)	 *
*	Created		:	18-June-2003								 *
*	Description	:	Implements CDROM redirection routines		 *
*					for WinNT/2K/XP								 *
******************************************************************/

/**
* @file WinNTMediaReader.cpp
* @author Raja Sekhar J(rajasekharj@amiindia.co.in)
* @date 18-Jun-2003
*
* @brief Implements routines for CDROM Redirection in WinNT/2K/XP.
*/
#include "WinNTMediaReader.h"
#include <stddef.h>
#include <stdio.h>
#include "dbgout.h"
#include "Usbdisk.h"

#include "ntddkincludes.h" 

#include "crc32.h"

#if defined (WIN64)
unsigned int gMyCRC;
#else
unsigned long gMyCRC;
#endif

#if defined (WIN64)
unsigned int
CalculateCRC32(unsigned char *Buffer, unsigned int Size)
#else
unsigned long
CalculateCRC32(unsigned char *Buffer, unsigned long Size)
#endif
{
#if defined (WIN64)
    unsigned int i,crc32 = 0xFFFFFFFF;
#else
    unsigned long i,crc32 = 0xFFFFFFFF;
#endif

	/* Read the data and calculate crc32 */	
    for(i = 0; i < Size; i++)
		crc32 = ((crc32) >> 8) ^ CrcLookUpTable[(Buffer[i]) 
								^ ((crc32) & 0x000000FF)];
	return ~crc32;
}




/*****************************************************************
	Function	:	Constructor.
	Description	:	Intializes the object.
******************************************************************/
CWinNTMediaReader::CWinNTMediaReader()
{
	m_bIsDeviceOpen		=	false;
	m_dwSectorSize		=	0;
	m_dwSectorCount		=	0;
	m_nFirstTime		=	1;
}

/*****************************************************************
	Function	:	Constructor.
	Description	:	Deallocate any buffer allocated.
******************************************************************/
CWinNTMediaReader::~CWinNTMediaReader()
{
	CloseDevice();
}

/*****************************************************************
	Function	:	OpenDevice
	
	Parameters	:	pszDevicePath - first character must be the
						selected CDROM drive letter.
					nByteOrder	- byte order used by the MegaRAC
						card.
				
	Returns		:	SUCCESS				- on success.
					DEVICE_ALREADY_OPEN - if in open state.
					ERROR_CODE			- error value if any other error

	Description	:	Validates the drive letter.
					Checks for existence of the drive.
******************************************************************/
int CWinNTMediaReader::OpenDevice(char *pszDevicePath, enum byteOrder nByteOrder)
{
	int				nRetval;

	/* check the flag whether last drive is closed */
	if( m_bIsDeviceOpen )
	{
		TCRIT("Device is already in open state\n");
		return DEVICE_ALREADY_OPEN;
	}
	
	/* first character contains the drive letter.*/
	m_nDriveLetter	= pszDevicePath[0];
	m_nByteOrder	= nByteOrder;
	
	/* Validate the drive, if fails returns media error.*/
	nRetval = ValidateDrive();
	
	if(nRetval != SUCCESS)
	{
		TCRIT("Invalid CDROM Device\n");
		CloseDevice();
		return nRetval;
	}
	
	TINFO("Start Redirecting the CDROM device %c\n", m_nDriveLetter );
	
	/*Set the OPEN flag*/
	m_bIsDeviceOpen = true;

	return SUCCESS;
}

/*****************************************************************
	Function	:	CloseDevice
				
	Returns		:	SUCCESS	- on success.
	
	Description	:	close the device and resets the OPEN flag.
******************************************************************/
int CWinNTMediaReader::CloseDevice()
{
	/*reset the OPEN flag*/
	m_bIsDeviceOpen	= false;

	/* Next time when he uses OpenDevice, it will be first
	time for the new CDROM device. */
	m_nFirstTime	= 1;

	return SUCCESS;
}

/*****************************************************************
	Function	:	ListCDROMDrives
	
	Parameters	:	DriveList - It is an OUT parameter returned with
						a string of cdrom drive letters connected
						to the system

	Returns		:	Number of cd drives connected to the system.

	Description	:	Search for the cddrives connected to the system
					and form a string of cd driveletters and return
******************************************************************/
int	CWinNTMediaReader::ListCDROMDrives(char *DriveList)
{
	int	  nIndex=0;
#if defined (WIN64)
	unsigned int Size;
	unsigned int DriveBitMap = 0;
#else
	unsigned long Size;
	unsigned long DriveBitMap = 0;
#endif
	
	nIndex  = ReInitUSB_Disk_Letter( &DriveList[nIndex]);

	return nIndex;
}

/*****************************************************************
	Function	:	ExecuteSCSICmd
	
	Parameters	:	pRequestPkt - command request packet sent by
						the MegaRAC Card.
					pResponsePkt - response packet sent to the
						MegaRAC Card.
					pdwDataLen	- length of data if any data 
						added to the response packet. This will
						be useful while sending the reponse packet
						to the card.
				
	Returns		:	SUCCESS				- on success.
					ERROR_CODE			- error value if any other error

	Description	:	Execute the command on local cdrom drive.
					Get the status and fill the response packet.
					Add the data to reponse packet as a result if any.
					fill data-length field.
******************************************************************/
#if defined (WIN64)
int CWinNTMediaReader::ExecuteSCSICmd(IUSB_SCSI_PACKET *pRequestPkt, 
										IUSB_SCSI_PACKET *pResponsePkt,
										unsigned int *pdwDataLen)
#else
int CWinNTMediaReader::ExecuteSCSICmd(IUSB_SCSI_PACKET *pRequestPkt, 
										IUSB_SCSI_PACKET *pResponsePkt,
										unsigned long *pdwDataLen)
#endif										
{
	SCSI_COMMAND_PACKET *ScsiPkt;
	READ_CAPACITY		*ScsiReadCap;
	int					nRetval=SUCCESS;
	BOOL				Status;
#if defined (WIN64)
	unsigned int		dwDataLen=0;
	unsigned long		dwNotUsed;
	unsigned int		dwSectorNo, dwSectors;
	unsigned int		dwMaxTOCSize;
#else
	unsigned long		dwDataLen=0;
	unsigned long		dwNotUsed;
	unsigned long		dwSectorNo, dwSectors;
	unsigned long		dwMaxTOCSize;
#endif

	/* Some Fields has to be sent back. So copy all as of now */
	memcpy(pResponsePkt,pRequestPkt,sizeof(IUSB_SCSI_PACKET));

	ScsiPkt=(SCSI_COMMAND_PACKET *)(&(pRequestPkt->CommandPkt));

	dwDataLen	=	0;

	/* It fails here if OpenDevice function is not used before using this
	function. So instead of returning error, just say NO_MEDIA.
	*/
	if( !m_bIsDeviceOpen )
	{
		TWARN("ExecuteSCSICmd called before using OpenDevice\n");
		nRetval = NO_MEDIA;
	}
	else
	{
		nRetval = OpenDrive();
	}

	if(nRetval == SUCCESS )
	{
		/* process each command */
		switch (ScsiPkt->OpCode)
		{
			/* Test Unit Ready */
			case SCSI_TEST_UNIT_READY:
				nRetval = TestUnitReady();
				break;

			/* Read Capacity */
			case SCSI_READ_CAPACITY:
				/* some times the first command will be read-capacity
				so in that case, inform medium change before sending
				read capacity information.
				*/
				if( m_nFirstTime == 1 )
				{
					nRetval = MEDIUM_CHANGE;
					m_nFirstTime = 0;
				}
				else
				{
					/* This is not first time.*/
					ScsiReadCap = (READ_CAPACITY *)(&(pResponsePkt->Data));

					nRetval = ReadActualCapacity(&ScsiReadCap->BlockSize,&ScsiReadCap->TotalBlocks);
					if(nRetval == SUCCESS )
					{
						//the following are scsi vars ..so always make them BE	readable by the card.
						
							ScsiReadCap->BlockSize   = mac2blong(ScsiReadCap->BlockSize);
							ScsiReadCap->TotalBlocks = mac2blong(ScsiReadCap->TotalBlocks);
						

						dwDataLen = 8;
					}
				}

				break;
			/* Read the sectors */
			case SCSI_READ_10:
			case SCSI_READ_12:
			
				dwSectorNo = ScsiPkt->Lba;
				dwSectors = (ScsiPkt->OpCode == SCSI_READ_10) ? ScsiPkt->CmdLen.Cmd10.Length : ScsiPkt->CmdLen.Cmd12.Length32;

			
				/* convert the from bigendian*/
				//if( m_nByteOrder == bigendian )
				//{
				//always flip..the abov are scsi packet values so BE
				//and therefore we have to flip it to this machines values
				//for the ioctl so we have to make it little.
				//this is true whatever the endianness of the card.
					dwSectorNo = mac2blong(dwSectorNo);
					dwSectors  = mac2bshort(dwSectors);
				//}

				if ( dwSectors <= MAX_READ_SECTORS) 
				{
					/* Read till successfull */
					for(int NumRetries = 0;NumRetries < MAX_READ_RETRIES;NumRetries++)
					{
						nRetval = ReadCDROM(dwSectorNo,
									dwSectors,
									&dwDataLen, (char *)(&(pResponsePkt->Data)));

						if(nRetval == SUCCESS) 
						{
							//TCRIT("S");
							break;
						}
						else
						{
							TCRIT("**************Read CDROm actually failed after retries***********\n");
						}
					}
				}
				else
				{
					TCRIT("******************Recd read request for enormously huge no of sectors..nuts!! %x********************\n",dwSectors);
					nRetval = INVALID_PARAMS;
					dwDataLen = 0;
				}
				break;
			/* Read Table of Contents */
			case SCSI_READ_TOC:
				TCRIT("in read toc\n");
				TCRIT("Scsi pkt LUN is %x\n",ScsiPkt->Lun);
				nRetval = ReadTOC(ScsiPkt,&dwDataLen, (char *)(&(pResponsePkt->Data)));

				dwMaxTOCSize = ScsiPkt->CmdLen.Cmd10.Length;
				TCRIT("dwMaxTOCSize before conversion is %x\n",dwMaxTOCSize);
				
				//if( m_nByteOrder == bigendian )
				//always flip
				//scsi to window le
					dwMaxTOCSize = mac2bshort(dwMaxTOCSize);

				TCRIT("dwMaxTOCSize after conversion is %x\n",dwMaxTOCSize);
				TCRIT("Dw Data len before comp is %d\n",dwDataLen);
				if (dwDataLen > dwMaxTOCSize)
					dwDataLen = dwMaxTOCSize;

				TCRIT("dwDataLen  is %x\n",dwDataLen);
				break;

			/* Prevention of medium removal is not supported in this version */
			case SCSI_MEDIUM_REMOVAL:
				nRetval = UNSUPPORTED_COMMAND;
				break;
			/* Load and Eject the CD Drives. */
			case SCSI_START_STOP_UNIT:
				/* Eject the Media*/
				if(ScsiPkt->Lba == 0x00020000)
				{
					Status = DeviceIoControl (m_hCDROMDevice, IOCTL_STORAGE_EJECT_MEDIA, NULL,
										0, NULL,0, &dwNotUsed, NULL);
				}
				/*Load Media*/
				else if(ScsiPkt->Lba == 0x00030000)
				{
					Status =DeviceIoControl (m_hCDROMDevice, IOCTL_STORAGE_LOAD_MEDIA ,NULL,
										0, NULL, 0, &dwNotUsed, NULL);
				}
				else
				{
					Status = TRUE;
				}

				if(Status)
				{
					nRetval = SUCCESS;
				}
				else
				{
					if( GetLastError() == ERROR_NOT_READY)
					{
						nRetval = REMOVAL_PREVENTED;
					}
					else
					{
						nRetval = NO_MEDIA;
					}
				}
				break;
			default:
				nRetval = UNSUPPORTED_COMMAND;
				break;
		}/* switch */
		
		CloseDrive();
	}/* if nRetval == SUCCESS*/
	else
	{
			TCRIT("**********The strange failure case in OpenDrive***************\n");
	}

	
	/* Set the error flags for response packet based on the error */
	SetErrorStatus(nRetval, pResponsePkt);

	/* Set the datalength of response data */
	pResponsePkt->DataLen	=	dwDataLen;
	*pdwDataLen				=	dwDataLen;

	return nRetval;
}

/*****************************************************************
	Function	:	ValidateDrive
				
	Returns		:	SUCCESS	- on success.
					MEDIA_ERROR - on failure
	
	Description	:	Validate the cdrom drive letter for
					existance of the CDROM Drive.
******************************************************************/
int CWinNTMediaReader::ValidateDrive()
{
	TCHAR Buffer[16*1024];
	TCHAR Mapping[1024];
	TCHAR *pBuff = &Buffer[0];
	size_t Len;
#if defined (WIN64)
	unsigned int Size;
	unsigned int DriveBitMap = 0;
#else
	unsigned long Size;
	unsigned long DriveBitMap = 0;
#endif

	Size = QueryDosDevice(NULL,pBuff,1024*16);

	while (*pBuff)
	{
		Len = strlen(pBuff);
		if (pBuff[1] == ':')
		{
			Size = QueryDosDevice(pBuff,&Mapping[0],1024);
			if (strstr(Mapping,"\\Device\\CdRom"))
			{
				if(m_nDriveLetter == pBuff[0] )
					return SUCCESS;
			}
		}
		pBuff+=(Len+1);
	}

	return MEDIA_ERROR;
}

/*****************************************************************
	Fucntion		:	TestUnitReady
	
	Returns			:	SUCCESS - on Success
						ERROR_VALUES - on other errors
	
	Description		:	Checks the CDROM drive for 
						medium change, ready state,
						and informs the state of cdrom drive.
******************************************************************/
int CWinNTMediaReader::TestUnitReady()
{
#if defined (WIN64)
	unsigned int			dwSectorSize;
	unsigned int			dwSectorCount;
	unsigned int 			actlen;
#else
	unsigned long			dwSectorSize;
	unsigned long			dwSectorCount;
	unsigned long actlen;
#endif
	int						nRetval = SUCCESS;
	SCSI_COMMAND_PACKET		tur_cdb;
	static unsigned char	MediumToNoMedium=0;
	
	/* For first time send medium change,
	to alert the host machine that there is
	a change in the status of cd drive.
	*/
	if(m_nFirstTime == 1)
	{
		m_nFirstTime = 0;
		nRetval	= MEDIUM_CHANGE;
		return nRetval;
	}

	memset(&tur_cdb,0,sizeof(SCSI_COMMAND_PACKET));
	tur_cdb.OpCode = SCSI_TEST_UNIT_READY;

	/* Send SCSI command*/
	if(SendScsiCmd(&tur_cdb,6,NULL,0,SCSI_IOCTL_DATA_IN,NULL,24,5,&actlen) == 0)
	{
		nRetval = SUCCESS;
		MediumToNoMedium = 1; 

		nRetval = ReadActualCapacity(&dwSectorSize,&dwSectorCount);

		/* If success, there is a cd-media in the cddrive.*/
		if( nRetval == SUCCESS)
		{
			/* sector-count differs from previous sector-count
				that means there is a change in the cdrom drive.
			*/
			if(m_dwSectorCount != dwSectorCount)
			{
				nRetval = MEDIUM_CHANGE;
			}

			m_dwSectorCount	= dwSectorCount;
			m_dwSectorSize	= dwSectorSize;
		}
		/* if failure, there is no cd media.*/
		else
		{
			m_dwSectorCount	= 0;
			m_dwSectorSize	= 0;
			
			nRetval = NO_MEDIA;
		}
	}
	/* if failure, there is no cd media.*/
	else
	{
		/* if this is the first failure after a success,
		then inform the medium change to the host machine.
		*/
		if(MediumToNoMedium == 1)
		{
			MediumToNoMedium = 0;
			nRetval = MEDIUM_CHANGE;
		}
		else
		{
			nRetval = NO_MEDIA;
		}
		
		m_dwSectorCount	= 0;
		m_dwSectorSize	= 0;
	}

	return nRetval;
}

/*****************************************************************
	Fucntion		:	SendScsiCmd
	
	Returns			:	status of the scsi command
	
	Description		:	ioctl the SCSI command.
******************************************************************/
#if defined (WIN64)
unsigned int CWinNTMediaReader::SendScsiCmd(void* cdb_buf,unsigned char cdb_len,
						  void* data_buf,unsigned int data_len,
						  int direction,
						  void* sense_buf,unsigned slen,
						  int timeout,
						  unsigned int* ActualDataLen
						  )
#else
unsigned long CWinNTMediaReader::SendScsiCmd(void* cdb_buf,unsigned char cdb_len,
						  void* data_buf,unsigned long data_len,
						  int direction,
						  void* sense_buf,unsigned slen,
						  long timeout,
						  unsigned long* ActualDataLen
						  )
#endif						  
{

	SCSI_PASSTHROUGH_DIRECT_WITH_SENSEBUFFER sptdwb;
	ULONG returned;

	memset(&sptdwb,0,sizeof(sptdwb));
	sptdwb.sptd.Length = sizeof(SCSI_PASS_THROUGH_DIRECT);

	/*ignore path id, target id and Lun since someone down below will do it
	because we have the drive handle*/

	sptdwb.sptd.CdbLength = cdb_len; /*CDB10 generic*/
	sptdwb.sptd.DataIn = direction;
	sptdwb.sptd.DataBuffer = data_buf;
	sptdwb.sptd.DataTransferLength = data_len;


	sptdwb.sptd.SenseInfoLength = 0;
	sptdwb.sptd.SenseInfoOffset = offsetof(SCSI_PASSTHROUGH_DIRECT_WITH_SENSEBUFFER,ucSenseBuf);

	sptdwb.sptd.TimeOutValue = timeout;

	memcpy(sptdwb.sptd.Cdb,cdb_buf,cdb_len);

	if(!DeviceIoControl(m_hCDROMDevice,IOCTL_SCSI_PASS_THROUGH_DIRECT,
						&sptdwb,
						sizeof(sptdwb),
						&sptdwb,
						sizeof(sptdwb),
						&returned,
						NULL)
   	  )
	{
		int x = GetLastError();
		return 0xFF;
	}

	//TCRIT("Cmd is %x and Data length from spt is %x\n",((UCHAR*)cdb_buf)[0],sptdwb.sptd.DataTransferLength);
	*ActualDataLen = sptdwb.sptd.DataTransferLength;
	return sptdwb.sptd.ScsiStatus;

}

/*****************************************************************
	Fucntion		:	ReadCapacity
	
	Returns			:	SUCCESS - on success
						ERROR_VALUES - on any other error.
	
	Description		:	Gets the sectorsize and sectorcount 
						parameters of a cdmedia
******************************************************************/
#if defined (WIN64)
int CWinNTMediaReader::ReadCapacity(unsigned int* SectorSize,unsigned int* SectorCount)
#else
int CWinNTMediaReader::ReadCapacity(unsigned long* SectorSize,unsigned long* SectorCount)
#endif
{
	unsigned long	SectorsPerCluster;     // sectors per cluster
	unsigned long	BytesPerSector;        // bytes per sector
	unsigned long	NumberOfFreeClusters;  // free clusters
	unsigned long	TotalNumberOfClusters;  // total clusters
	char	CDDrvStr[5];
	
	sprintf(CDDrvStr,"%c:\\",m_nDriveLetter);
	if(GetDiskFreeSpace(CDDrvStr,&SectorsPerCluster,&BytesPerSector,&NumberOfFreeClusters,&TotalNumberOfClusters) == 0)
	{
			*SectorSize = 0;
			*SectorCount = 0;
			return NO_MEDIA;
	}
	*SectorSize = BytesPerSector;
	*SectorCount = SectorsPerCluster * TotalNumberOfClusters;

	return SUCCESS;
}

/*****************************************************************
	Fucntion		:	ReadActualCapacity
	
	Returns			:	SUCCESS - on success
						ERROR_VALUES - on any other error.
	
	Description		:	Gets the sectorsize and sectorcount 
						parameters of a cdmedia
******************************************************************/
#if defined (WIN64)
int CWinNTMediaReader::ReadActualCapacity(unsigned int* SectorSize,unsigned int* SectorCount)
#else
int CWinNTMediaReader::ReadActualCapacity(unsigned long* SectorSize,unsigned long* SectorCount)
#endif
{
	SCSI_COMMAND_PACKET readcap_pkt;
	READ_CAPACITY		*pReadCapData;
	int					nRetval=SUCCESS;
#if defined (WIN64)
	unsigned int actlen;
#else
	unsigned long actlen;
#endif

	nRetval = ReadCapacity(SectorSize,SectorCount);

	pReadCapData = (READ_CAPACITY*)AllocateAlignedBuffer(sizeof(READ_CAPACITY));
	
	if(pReadCapData == NULL)
	{
		*SectorSize	 = 0;
		*SectorCount = 0;
		nRetval = NO_MEDIA;
		return nRetval;
	}

	memset(&readcap_pkt,0,sizeof(SCSI_COMMAND_PACKET));
	readcap_pkt.OpCode = SCSI_READ_CAPACITY;

	if(SendScsiCmd(&readcap_pkt,10,pReadCapData,sizeof(READ_CAPACITY),SCSI_IOCTL_DATA_IN,NULL,24,5,&actlen) != 0)
	{
		*SectorSize	 = 0;
		*SectorCount = 0;
		free(pReadCapData);
		nRetval = NO_MEDIA;
		return nRetval;
	}

	*SectorSize  = mac2blong(pReadCapData->BlockSize);
	*SectorCount = mac2blong(pReadCapData->TotalBlocks);

	free(pReadCapData);
	
	return SUCCESS;
}

/*****************************************************************
	Fucntion		:	AllocateAlignedBuffer
	
	Returns			:	Pointer to buffer on Success.
						and NULL on failure, 
	
	Description		:	allocates a alligned buffer.
******************************************************************/
int CWinNTMediaReader::FindAlignment()
{
	IO_SCSI_CAPABILITIES capabilities;
	BOOL status = 0;
	ULONG returned=0,errorCode=0;

	
	status = DeviceIoControl(m_hCDROMDevice,
                             IOCTL_SCSI_GET_CAPABILITIES,
                             NULL,
                             0,
                             &capabilities,
                             sizeof(IO_SCSI_CAPABILITIES),
                             &returned,
                             FALSE);

    if (!status ) 
	{
       TCRIT( "No fatal Error in io control; error was %d\n",
          errorCode = GetLastError() );
       return 0; //we dont need any alignment
       
    }
	else
	{
		TCRIT("alignment requirements are %x\n",capabilities.AlignmentMask);
		return capabilities.AlignmentMask;
	}

}

#if defined (WIN64)
PUCHAR CWinNTMediaReader::AllocateAlignedBuffer(unsigned int size)
#else
PUCHAR CWinNTMediaReader::AllocateAlignedBuffer(ULONG size)
#endif
{
	static int AlignmentFound = 0;
	static int AlignReq = 0;

    PUCHAR ptr;

	
	if(AlignmentFound == 0)
	{
		//alignment has not been found..find it
		TCRIT("going to call alignment found\n");
		AlignReq = FindAlignment();
		AlignmentFound = 1;
	}


    UINT_PTR    Align64 = (UINT_PTR)AlignReq;

	if (!AlignReq)
	{
		ptr = (PUCHAR)malloc(size);
	}
	else
	{
		ptr = (PUCHAR)malloc(size + AlignReq);
		ptr = (PUCHAR)(((UINT_PTR)ptr + Align64) & ~Align64);
	}

	return ptr;
}

/*****************************************************************
	Fucntion		:	OpenDrive
	
	Returns			:	SUCCESS - on success
						ERROR_VALUES - on any other error.
	
	Description		:	Open the cddrive for reading.
******************************************************************/
int	CWinNTMediaReader::OpenDrive()
{
	char	*DriveString = "\\\\.\\%c:";
	char	DeviceName[16];
	int		nRetval=SUCCESS;
	int retries = 0;
	int MaxOpenRetries = 6;
	
	//sometimes some background thread in Windows may cause open to fail by saying "it is already used by another process"
	//therefore we have to retry a bit here

	for(retries = 0; retries < MaxOpenRetries;retries++)
	{
		sprintf(DeviceName,DriveString,m_nDriveLetter);
		m_hCDROMDevice = CreateFile (DeviceName, GENERIC_READ| GENERIC_WRITE, FILE_SHARE_READ,
									NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		
		if (m_hCDROMDevice == INVALID_HANDLE_VALUE)
		{
			LPVOID lpMsgBuf;
			DWORD lasterr = GetLastError();


			if (!FormatMessage( 
				FORMAT_MESSAGE_ALLOCATE_BUFFER | 
				FORMAT_MESSAGE_FROM_SYSTEM | 
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				lasterr,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
				(LPTSTR) &lpMsgBuf,
				0,
				NULL ))
			{
			   // Handle the error.
				TCRIT("Error in format message %x\n",lasterr);
			   
			}

			//MessageBox( NULL, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );
			TCRIT("Windows error in open drive is %s %x",lpMsgBuf,lasterr);

			// Free the buffer.
			LocalFree( lpMsgBuf );
			
			//we just continue for the max retries
			Sleep(1000);
			continue;

			//nRetval	= NO_MEDIA;
		}
		else
		{
			return SUCCESS;
		}
	}

	if(retries == MaxOpenRetries)
	{
		TCRIT("Could not open device even after retrying for a while..estupido\n");
		nRetval = NO_MEDIA;
	}
	
	return nRetval;
}

/*****************************************************************
	Fucntion		:	CloseDrive
	
	Returns			:	SUCCESS - on success
	
	Description		:	Close the drive
******************************************************************/
int	CWinNTMediaReader::CloseDrive()
{
	CloseHandle(m_hCDROMDevice);

	return SUCCESS;
}

/*****************************************************************
	Fucntion		:	ReadCDROM
	
	Returns			:	SUCCESS - on success
						ERROR_VALUES - on any other error.
	
	Description		:	Read the desired sectors starting from
						SectorNo to SectorNo+Sectors.
******************************************************************/
#if defined (WIN64)
int CWinNTMediaReader::ReadCDROM(unsigned int SectorNo,unsigned int Sectors, 
								 unsigned int *dwDataLen, 
								 char *Buffer)
#else
int CWinNTMediaReader::ReadCDROM(unsigned long SectorNo,unsigned long Sectors, 
								 unsigned long *dwDataLen, 
								 char *Buffer)
#endif								 
{
	PUCHAR	lpSector;
	int		nRetval;
#if defined (WIN64)
	unsigned int	dwReadSizeBytes;
	unsigned int actlen;
#else
	unsigned long	dwReadSizeBytes;
	unsigned long actlen;
#endif
	
	if(Sectors == 0x3f)
	{
		Sectors = 0x3f;
	}

	//TCRIT("ReadCDROM sector request for %x for %x sectors\n",SectorNo,Sectors);

	dwReadSizeBytes  = Sectors * 2048;

	lpSector = AllocateAlignedBuffer(dwReadSizeBytes);

	if(lpSector == NULL)
	{
		TCRIT("***********Could not allocate buffer for Sector %x, readsize %x*************************\n",SectorNo,dwReadSizeBytes);
		nRetval = INVALID_PARAMS;
		*dwDataLen = 0;
		return nRetval;
	}

	SCSI_COMMAND_PACKET ReadCmd = {0};

#if defined (WIN64)
	unsigned int SectorsLeft = Sectors;
#else
	unsigned long SectorsLeft = Sectors;
#endif
	PUCHAR BufferOffset = lpSector;

	int max_chunk=MAX_CHUNK;
	/*split and read*/
	while(SectorsLeft != 0)
	{
		int ReadLen = 0;

		memset(&ReadCmd,0,sizeof(SCSI_COMMAND_PACKET));

		/*how many sectors do we have left to read?
		if it is more than the 0x20 windows likeablity factor just do that
		much otherwise do the whole thing*/
		if(SectorsLeft > max_chunk)
			ReadLen = max_chunk;
		else
			ReadLen = SectorsLeft;


		ReadCmd.OpCode = SCSI_READ_10;
		ReadCmd.Lba = mac2blong(SectorNo + (Sectors - SectorsLeft));
		ReadCmd.CmdLen.Cmd10.Length = mac2bshort(ReadLen);


		if(SendScsiCmd(&ReadCmd,10,BufferOffset,(ReadLen*2048),SCSI_IOCTL_DATA_IN,NULL,0,25,&actlen) != 0)
		{
			TCRIT("*************************Encounetered error when reading Sector:%x numsectors:%x!!******************************\n",SectorNo,Sectors);
			
			/*some drives cannot read 0x20 sectors in a single stretch. So, In those cases,
				we reduce the max_chunk value to half i.e. 0x10*/

			if( max_chunk == MAX_CHUNK && SectorsLeft == Sectors ) // this means that this is the first parse in the loop.
			{
				max_chunk = max_chunk/2;
				continue;
			}

			nRetval = MEDIA_ERROR;
			free(lpSector);
			*dwDataLen = 0;
			return nRetval;
		}


		SectorsLeft -= ReadLen;
		BufferOffset += (ReadLen * 2048);
	}


	memcpy(Buffer,lpSector,dwReadSizeBytes);
	free(lpSector);
	
	*dwDataLen = dwReadSizeBytes;
	return SUCCESS;
}

/*****************************************************************
	Fucntion		:	ReadTOC
	
	Returns			:	SUCCESS - on success
						ERROR_VALUES - on any other error.
	
	Description		:	Read TOC of CDMEDIA.
******************************************************************/
#if defined (WIN64)
int	CWinNTMediaReader::ReadTOC(SCSI_COMMAND_PACKET* cdb,
							   unsigned int *dwDataLen, char *Buffer)
#else
int	CWinNTMediaReader::ReadTOC(SCSI_COMMAND_PACKET* cdb,
							   unsigned long *dwDataLen, char *Buffer)
#endif							   
{
	CDROM_TOC*	lpTOC;
	int			nRetval=SUCCESS;
#if defined (WIN64)
	unsigned int actlen;
#else
	unsigned long actlen;
#endif

	int RandomBuffer[255];

	int i;
	for(i=0;i<254;i++)
	{ 
			RandomBuffer[i] = rand();
	}


	/*allocate 810 bytes to cover table of contents*/
	/*even if someone asks us for 12 we should read the whole thing..send them only what is needed
	and give them what length is left*/
	lpTOC = (CDROM_TOC*) AllocateAlignedBuffer(mac2bshort(cdb->CmdLen.Cmd10.Length));
	
	if(lpTOC == NULL)
	{
		*dwDataLen = 0;
		nRetval = INVALID_PARAMS;
		return nRetval;
	}

	if(SendScsiCmd(cdb,10,lpTOC,mac2bshort(cdb->CmdLen.Cmd10.Length),SCSI_IOCTL_DATA_IN,NULL,24,25,&actlen) == 0)
	{
		TCRIT("Actual length is %d\n",actlen);
		memcpy(Buffer,lpTOC,actlen);
		
		*dwDataLen = actlen;
		nRetval = SUCCESS;
	}
	else
	{
		*dwDataLen = 0;
		nRetval = NO_MEDIA;
	}
	
	free(lpTOC);
	return nRetval;
}


