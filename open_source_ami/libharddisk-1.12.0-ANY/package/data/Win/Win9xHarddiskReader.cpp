/*****************************************************************
*	File		:	Win9xHarddiskReader						 *
*	Author		:	Rajasekhar J(rajasekharj@amiindia.co.in)	 *
*	Created		:	18-July-2003								 *
*	Description	:	Implements Harddisk redirection routines		 *
*					for Win95/98/ME								 *
******************************************************************/

/**
* @file Win9xHarddiskReader
* @author Raja Sekhar J(rajasekharj@amiindia.co.in)
* @date 18-July-2003
*
* @brief Implements the routines for Harddisk redirection in Win95/98/ME
*
*/

#include "Win9xHarddiskReader.h"
#include "harddiskerrors.h"
#include "libharddisk.h"

#include <stdio.h>
#include <dbgout.h>

/****************************** public member functions**********************/
/*****************************************************************
	Function	:	Constructor.
	Description	:	Intializes the object.
******************************************************************/
CWin9xHarddiskReader::CWin9xHarddiskReader()
{
	m_bIsDeviceOpen		=	false;
	m_dwSectorSize		=	0;
	m_dwSectorCount		=	0;
	m_ProgressPercentage=	0;
	m_bFlagStop			=	false;
	m_bFirstTime		=	false;
}

/*****************************************************************
	Function	:	Destructor.
	Description	:	Deallocate any buffer allocated.
******************************************************************/
CWin9xHarddiskReader::~CWin9xHarddiskReader()
{
	CloseDevice( );
}

/*****************************************************************
	Function	:	OpenDevice

	Parameters	:	pszDevicePath - first character must be the
						selected Harddisk drive letter.
					nByteOrder	- byte order used by the MegaRAC
						card.

	Returns		:	SUCCESS				- on success.
					DEVICE_ALREADY_OPEN - if in open state.
					ERROR_CODE			- error value if any other error

	Description	:	Validates the drive letter.
					Checks for existence of the drive.
******************************************************************/
int CWin9xHarddiskReader::OpenDevice(char *pszDevicePath, enum byteOrder nByteOrder)
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
		TCRIT("Invalid Harddisk Device\n");
		return nRetval;
	}
	
	nRetval = OpenDrive();
	if(nRetval != SUCCESS)
	{
		TCRIT("Unable to open Harddisk Device\n");
		return nRetval;
	}
			
	TINFO("Start Redirecting the Harddisk device %c\n", m_nDriveLetter );
	
	/*Set the OPEN flag*/
	m_bIsDeviceOpen = true;
	m_bFirstTime = true;

	return nRetval;
}

/*****************************************************************
	Function	:	CloseDevice
				
	Returns		:	SUCCESS	- on success.
	
	Description	:	close the device and resets the OPEN flag.
******************************************************************/
int CWin9xHarddiskReader::CloseDevice()
{
	/*reset the OPEN flag*/
	if( m_bIsDeviceOpen )
	{
		CloseDrive();
		m_bIsDeviceOpen	= false;
		m_bFirstTime = false;
	}
	
	return SUCCESS;
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

	Description	:	Execute the command on local harddisk drive.
					Get the status and fill the response packet.
					Add the data to reponse packet as a result if any.
					fill data-length field.
******************************************************************/
#if defined (WIN64)
int CWin9xHarddiskReader::ExecuteSCSICmd(IUSB_SCSI_PACKET *pRequestPkt, 
										IUSB_SCSI_PACKET *pResponsePkt,
										unsigned int *pdwDataLen)
#else
int CWin9xHarddiskReader::ExecuteSCSICmd(IUSB_SCSI_PACKET *pRequestPkt, 
										IUSB_SCSI_PACKET *pResponsePkt,
										unsigned long *pdwDataLen)
#endif										
{
	int						nRetval=SUCCESS;
	SCSI_COMMAND_PACKET		*ScsiPkt;
	READ_CAPACITY			*ScsiReadCap;
	HD_READ_FORMAT_CAPACITIES *ReadFmtCaps;
#if defined (WIN64)
	unsigned int			dwDataLen=0;
	unsigned int			dwSectorNo, dwSectors;
#else
	unsigned long			dwDataLen=0;
	unsigned long			dwSectorNo, dwSectors;
#endif
	static bool				bMediaChanged=false;

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
	else if( m_bFirstTime )
	{
		nRetval = MEDIUM_CHANGE;
		m_bFirstTime = false;
	}
	

	if(bMediaChanged && 
		ScsiPkt->OpCode != SCSI_READ_FORMAT_CAPACITIES )
	{
		nRetval = MEDIUM_CHANGE;
		bMediaChanged = false;
	}

	if(nRetval == SUCCESS )
	{
		/* process each command */
		switch (ScsiPkt->OpCode)
		{
			case SCSI_FORMAT_UNIT:
				nRetval = SUCCESS;
				break;
			case SCSI_START_STOP_UNIT:
				nRetval = SUCCESS;
				break;
			case SCSI_TEST_UNIT_READY:
				nRetval = TestUnitReady();
				break;
			case SCSI_MEDIUM_REMOVAL:
				nRetval = SUCCESS;
				break;
			case SCSI_READ_FORMAT_CAPACITIES:
				ReadFmtCaps = (HD_READ_FORMAT_CAPACITIES *)(&(pResponsePkt->Data));
				ReadFmtCaps->ListLength  = 8;
				nRetval = ReadCapacity(&ReadFmtCaps->BlockSize,&ReadFmtCaps->TotalBlocks);

				if(nRetval == MEDIUM_CHANGE)
					bMediaChanged = true;

				if(nRetval == SUCCESS )
				{
					ReadFmtCaps->BlockSize   = ReadFmtCaps->BlockSize| 0x02000000;
					ReadFmtCaps->TotalBlocks = ReadFmtCaps->TotalBlocks;
				}
				else
				{
					ReadFmtCaps->BlockSize   = 512| 0x03000000;
					ReadFmtCaps->TotalBlocks = 2880;
					nRetval = SUCCESS;
				}

				if( m_nByteOrder == bigendian )
				{
					ReadFmtCaps->BlockSize   = mac2blong(ReadFmtCaps->BlockSize);
					ReadFmtCaps->TotalBlocks = mac2blong(ReadFmtCaps->TotalBlocks);
				}

				dwDataLen = sizeof(HD_READ_FORMAT_CAPACITIES);

				break;
			case SCSI_READ_CAPACITY:
				ScsiReadCap = (READ_CAPACITY *)(&(pResponsePkt->Data));

				nRetval = ReadCapacity(&ScsiReadCap->BlockSize,&ScsiReadCap->TotalBlocks);

				if(nRetval == SUCCESS )
				{
					if( m_nByteOrder == bigendian )
					{
						ScsiReadCap->BlockSize   = mac2blong(ScsiReadCap->BlockSize);
						ScsiReadCap->TotalBlocks = mac2blong(ScsiReadCap->TotalBlocks-1);
					}
					else
					{
						/* ScsiReadCap->BlockSize   = ScsiReadCap->BlockSize; */
						ScsiReadCap->TotalBlocks = ScsiReadCap->TotalBlocks-1;
					}

					dwDataLen = 8;
				}
				else
					dwDataLen = 0;
				break;
			case SCSI_READ_10:
			case SCSI_READ_12:
				dwSectorNo = ScsiPkt->Lba;
				dwSectors = (ScsiPkt->OpCode == SCSI_READ_10) ? ScsiPkt->CmdLen.Cmd10.Length : ScsiPkt->CmdLen.Cmd12.Length32;

				/* convert the from bigendian*/
				if( m_nByteOrder == bigendian )
				{
					dwSectorNo = mac2blong(dwSectorNo);
					dwSectors  = mac2bshort(dwSectors);
				}

				if ( dwSectors <= MAX_HARDDISK_READ_SECTORS)
				{
					/* Read till successfull */
					nRetval = ReadHarddisk(dwSectorNo,
								dwSectors,
								&dwDataLen, (char *)(&(pResponsePkt->Data)));
					// To support Virtual harddisk boot, the following
					// values must be modified everytime we supply 
					// the zeroth sector
					//  bsHiddenSectors must be 0
					//  bsHeads must be 0
					//  bsSecPerTrack must be 0x3f for devices larger than 8mb
					//  bsHeads must be 0xff for devices  larger than 8mb
					if( nRetval == SUCCESS && dwSectorNo == 0)
					{
#if defined (WIN64)
						unsigned int SectorSize=0;
						unsigned int SectorCount=0;
#else
						unsigned long SectorSize=0;
						unsigned long SectorCount=0;
#endif
						if( ReadCapacity(&SectorSize,&SectorCount) == SUCCESS )
						{
							if( SectorCount >= (NUM_OF_TRACKS*NUM_OF_HEADS) )
							{
								if( IsHDFSFAT32((unsigned char *)(&(pResponsePkt->Data)) ) )
								{
									BOOTSECTOR_32 *pBootSector = (BOOTSECTOR_32 *)(&(pResponsePkt->Data));

									pBootSector->bsParamBlock.bsHiddenSectors = 0;
									pBootSector->bsDriveNumber = 0;
									pBootSector->bsParamBlock.bsSecPerTrack = NUM_OF_TRACKS;
									pBootSector->bsParamBlock.bsHeads = NUM_OF_HEADS;
								}
								else if( IsHDFSFAT16((unsigned char *)(&(pResponsePkt->Data)) ) )
								{
									BOOTSECTOR_12_16 *pBootSector = (BOOTSECTOR_12_16 *)(&(pResponsePkt->Data));

									pBootSector->bsParamBlock.bsHiddenSectors = 0;
									pBootSector->bsDriveNumber = 0;
									pBootSector->bsParamBlock.bsSecPerTrack = NUM_OF_TRACKS;
									pBootSector->bsParamBlock.bsHeads = NUM_OF_HEADS;
								}
							}
						}
					}
				}
				else
				{
					nRetval = INVALID_PARAMS;
				}
				
				break;
			
			case SCSI_WRITE_10:
				dwSectorNo = ScsiPkt->Lba;
				dwSectors  = ScsiPkt->CmdLen.Cmd10.Length;

				/* convert the from bigendian*/
				if( m_nByteOrder == bigendian )
				{
					dwSectorNo = mac2blong(dwSectorNo);
					dwSectors  = mac2bshort(dwSectors);
				}

				if ( dwSectors <= MAX_HARDDISK_READ_SECTORS)
				{
					// ATTENTION ATTENTION
					// We are doing a stunt while reading sector zero
					// by masking HiddenSectors and drive number.
					// In the same way, we should not change this values
					// while writing sector zero.
					// So while writing sector zero, we have to replace
					// the above values with the actual values.
					if( dwSectorNo == 0 )
					{
						BYTE SectZero[512];
	#if defined (WIN64)
						unsigned int dwTemp=0;
	#else
						DWORD dwTemp=0;
	#endif
						int nRet;

						nRet = ReadHarddisk(0, 1, &dwTemp, (char*)SectZero);

						if( nRet == SUCCESS )
						{
#if defined (WIN64)
						unsigned int SectorSize=0;
						unsigned int SectorCount=0;
#else
						unsigned long SectorSize=0;
						unsigned long SectorCount=0;
#endif

							if( ReadCapacity(&SectorSize,&SectorCount) == SUCCESS )
							{
								if( SectorCount >= (NUM_OF_TRACKS*NUM_OF_HEADS) )
								{
									if( IsHDFSFAT32((unsigned char *)(&(pRequestPkt->Data)) ) )
									{
										BOOTSECTOR_32 *pBootSector = (BOOTSECTOR_32 *)(&(pRequestPkt->Data));
										BOOTSECTOR_32 *pOrigBootSector = (BOOTSECTOR_32 *)(SectZero);

										pBootSector->bsParamBlock.bsHiddenSectors = pOrigBootSector->bsParamBlock.bsHiddenSectors;
										pBootSector->bsDriveNumber = pOrigBootSector->bsDriveNumber;
										pBootSector->bsParamBlock.bsSecPerTrack = pOrigBootSector->bsParamBlock.bsSecPerTrack;
										pBootSector->bsParamBlock.bsHeads = pOrigBootSector->bsParamBlock.bsHeads;
									}
									else if( IsHDFSFAT16((unsigned char *)(&(pRequestPkt->Data)) ) )
									{
										BOOTSECTOR_12_16 *pBootSector = (BOOTSECTOR_12_16 *)(&(pRequestPkt->Data));
										BOOTSECTOR_12_16 *pOrigBootSector = (BOOTSECTOR_12_16 *)(SectZero);

										pBootSector->bsParamBlock.bsHiddenSectors = pOrigBootSector->bsParamBlock.bsHiddenSectors;
										pBootSector->bsDriveNumber = pOrigBootSector->bsDriveNumber;
										pBootSector->bsParamBlock.bsSecPerTrack = pOrigBootSector->bsParamBlock.bsSecPerTrack;
										pBootSector->bsParamBlock.bsHeads = pOrigBootSector->bsParamBlock.bsHeads;
									}
								}
							}
						}
					}
					/* Read till successfull */
					nRetval = WriteHarddisk(dwSectorNo,
								dwSectors,
								(char *)(&(pRequestPkt->Data)));
				}
				else
				{
					nRetval = INVALID_PARAMS;
				}
				break;
			/* Mode sense */
			case SCSI_MODE_SENSE:
				{
					uint8	nPageCode = (ScsiPkt->Lba)&0x3f;

					dwDataLen = 0;
					// we support only Flexible Disk page (05h)
					if( nPageCode == FLEXIBLE_DISK_PAGE_CODE )
					{
						MODE_SENSE_RESPONSE	*ModeSenseResp = 
									(MODE_SENSE_RESPONSE *) (&(pResponsePkt->Data));
						uint32 SectorSize=0;
						uint32 SectorCount=0;
						unsigned char NumofHeads=1;
						unsigned char SectorsPerTrack=1;


						memset(ModeSenseResp, 0, sizeof(MODE_SENSE_RESPONSE) );
						
						nRetval = ReadCapacity(&SectorSize, &SectorCount);
						if( nRetval == SUCCESS )
						{
							unsigned char FirstSector[2048];
#if defined (WIN64)
							unsigned int	dwTemp=0;
#else
							unsigned long	dwTemp=0;
#endif
							nRetval = ReadHarddisk(0,1,&dwTemp, (char *)FirstSector);

							if( nRetval == SUCCESS )
							{
								BPB *pBootSector = (BPB *)FirstSector;

								if( SectorCount >= (NUM_OF_TRACKS*NUM_OF_HEADS) )
								{
									SectorsPerTrack = NUM_OF_TRACKS;
									NumofHeads = NUM_OF_HEADS;
								}
								else
								{
									NumofHeads = (unsigned char) pBootSector->bsHeads;
									SectorsPerTrack = (unsigned char)pBootSector->bsSecPerTrack;
								}
								
								// to avoid divide by zero
								if( SectorsPerTrack == 0 ) SectorsPerTrack = 1;
								if( NumofHeads == 0 ) NumofHeads = 1;
							}
						}
						if( nRetval == SUCCESS )
						{
							memset(ModeSenseResp, 0, sizeof(MODE_SENSE_RESPONSE));
							//Mode sense header
							ModeSenseResp->ModeSenseRespHdr.ModeDataLen = 
								mac2bshort((FLEXIBLE_DISK_PAGE_LEN+sizeof(MODE_SENSE_RESP_HDR)-2));
							
							if( SectorSize == 512 && SectorCount == 2880 )
							{ //floppy
								ModeSenseResp->ModeSenseRespHdr.MediumTypeCode = MEDIUM_TYPE_144_MB;
							}
							else
							{//usbdisk
								ModeSenseResp->ModeSenseRespHdr.MediumTypeCode = MEDIUM_TYPE_DEFAULT;
							}

							// Mode sense data
							FLEXIBLE_DISK_PAGE	*DiskPage = 
											(FLEXIBLE_DISK_PAGE*)(&ModeSenseResp->PageData[0]);

							DiskPage->PageCode = FLEXIBLE_DISK_PAGE_CODE;
							DiskPage->PageLength = 0x1E;
							DiskPage->NumberofHeads	= NumofHeads;
							DiskPage->SectorsPerTrack = SectorsPerTrack;
							if( SectorSize == 512 && SectorCount == 2880 )
							{ //floppy
								DiskPage->TransferRate = mac2bshort(0x01f4); //500kbit/s
								DiskPage->NumberofHeads	= 0x02;
								DiskPage->SectorsPerTrack = 0x12;
								DiskPage->MotorONDelay = 0x05;
								DiskPage->MotorOFFDelay = 0x1e;
								DiskPage->MediumRotationRate = mac2bshort(0x12c);
							}
							else
							{//usbdisk
								DiskPage->TransferRate = mac2bshort(0xf000); //500kbit/s
								//DiskPage->TransferRate = mac2bshort(0x01f4); //500kbit/s
								DiskPage->NumberofHeads	= 0x10;
								DiskPage->SectorsPerTrack = 0x20;
							}
							
							uint16 NumberofCylinders = 
									SectorCount/(DiskPage->NumberofHeads*DiskPage->SectorsPerTrack);
							DiskPage->NumberofCylinders = mac2bshort(NumberofCylinders);
							DiskPage->DataBytesPerSector = mac2bshort(SectorSize);

						
						dwDataLen = FLEXIBLE_DISK_PAGE_LEN + sizeof(MODE_SENSE_RESP_HDR);
						}
					}
					else
					{
						nRetval = UNSUPPORTED_COMMAND;
					}
				}
				break;
			default:
				nRetval = UNSUPPORTED_COMMAND;
				break;
		}/* end switch case*/

	}/* nRetval; == SUCCESS */

	/* Set the error flags for response packet based on the error */
	SetErrorStatus(nRetval, pResponsePkt);

	/* Set the datalength of response data */
	pResponsePkt->DataLen	=	dwDataLen;
	*pdwDataLen				=	dwDataLen;

	return nRetval;
}

/*****************************************************************
	Function	:	ListHarddiskDrives
	
	Parameters	:	DriveList - It is an OUT parameter returned with
						a string of harddisk drive letters connected
						to the system

	Returns		:	Number of harddisk drives connected to the system.

	Description	:	Search for the fddrives connected to the system
					and form a string of fd driveletters and return
******************************************************************/
int	CWin9xHarddiskReader::ListHarddiskDrives(char *DriveList)
{
	TCHAR Buffer[HD_BUFFER_SIZE];
#if defined (WIN64)
	unsigned int Size;
#else
	unsigned long Size;
#endif
	TCHAR *pBuff = &Buffer[0];
	size_t Len;
	int nIndex;

	Size = GetLogicalDriveStrings(HD_BUFFER_SIZE, pBuff );

	nIndex = 0;
	while(*pBuff)
	{
		if ( GetDriveType(pBuff) == DRIVE_REMOVABLE )
		{
			strupr(pBuff);
			DriveList[nIndex++] = pBuff[0];
		}

		Len = strlen(pBuff);
		pBuff += (Len+1);
	}
	
	return nIndex;
}

/**********************************************************************************/

/***************************protected member functions*****************************/
/*****************************************************************
	Fucntion		:	ReadCapacity
	
	Returns			:	SUCCESS - on success
						ERROR_VALUES - on any other error.
	
	Description		:	Gets the sectorsize and sectorcount 
						parameters of a harddisk media
******************************************************************/
#if defined (WIN64)
int	CWin9xHarddiskReader::ReadCapacity(unsigned int* SectorSize,unsigned int* SectorCount)
#else
int	CWin9xHarddiskReader::ReadCapacity(unsigned long* SectorSize,unsigned long* SectorCount)
#endif
{
	int	nRetval = SUCCESS;	

	nRetval = CheckForMediaChange();
	
	if(nRetval == SUCCESS )
	{
		nRetval = GetDiskInfo(SectorSize, SectorCount);
	}
	
	if ( nRetval != SUCCESS )
	{
		*SectorSize = 0;
		*SectorCount= 0;
	}
	
	return nRetval;
}

/*****************************************************************
	Fucntion		:	TestUnitReady
	
	Returns			:	SUCCESS - on Success
						ERROR_VALUES - on other errors
	
	Description		:	Checks the Harddisk drive for 
						medium change, ready state,
						and informs the state of harddisk drive.
******************************************************************/
int CWin9xHarddiskReader::TestUnitReady()
{
	int nRetval;

	nRetval = ReadCapacity(&m_dwSectorSize, &m_dwSectorCount);

	return nRetval;
}

/*****************************************************************
	Fucntion		:	ReadHarddisk
	
	Returns			:	SUCCESS - on success
						ERROR_VALUES - on any other error.
	
	Description		:	Read the desired sectors starting from
						SectorNo to SectorNo+Sectors.
******************************************************************/
#if defined (WIN64)
int	CWin9xHarddiskReader::ReadHarddisk(unsigned int SectorNo,unsigned int Sectors,
							unsigned int *dwDataLen,
							char *Buffer)
#else
int	CWin9xHarddiskReader::ReadHarddisk(unsigned long SectorNo,unsigned long Sectors,
							unsigned long *dwDataLen,
							char *Buffer)
#endif							
{
	
#if defined (WIN64)
	unsigned int	dwReadSize;
#else
	unsigned long	dwReadSize;
#endif
	LPBYTE			lpSector;
	int				nRetval;
	CONTROLBLOCK	ControlBlock;
	DIOC_REGISTERS	reg ;
	BOOL			fResult ;
	DWORD			cb;

	/* Let's say the data read length as zero initially. 
	   if no error occurs while reading, we can assign the correct
	   data length later.
	*/
	*dwDataLen = 0;

	if (m_dwSectorSize == 0)
	{
		nRetval = TestUnitReady();
		
		if(nRetval != SUCCESS)
			return nRetval;
	}

	if (Sectors ==  0)
	{
		nRetval = SUCCESS;
		return nRetval;
	}


	/*Calculate Buffer Requirements*/
	dwReadSize = Sectors * m_dwSectorSize;

    /* Allocate buffer */
	lpSector = (unsigned char *)VirtualAlloc (NULL, dwReadSize,
									MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
	if (lpSector == NULL)
	{
		nRetval = INVALID_PARAMS;
		return nRetval;		
	}

	ControlBlock.NumberOfSectors = (WORD)Sectors;
#if defined (WIN64)
	ControlBlock.StartingSector = (unsigned int)SectorNo;
	ControlBlock.pBuffer = (unsigned int)lpSector;
#else
	ControlBlock.StartingSector = (DWORD)SectorNo;
	ControlBlock.pBuffer = (DWORD)lpSector;
#endif

	reg.reg_ESI = 0x00;  /* 0x00 = Read */
	reg.reg_ECX = -1;
#if defined (WIN64)
	reg.reg_EBX = (unsigned int)(&ControlBlock);
#else
	reg.reg_EBX = (DWORD)(&ControlBlock);
#endif
	reg.reg_EDX = m_nDriveLetter - 'A' + 1;
	reg.reg_EAX = 0x7305;

	//  6 == VWIN32_DIOC_DOS_DRIVEINFO
	fResult = DeviceIoControl ( m_hHarddiskDevice,
	VWIN32_DIOC_DOS_DRIVEINFO,
	&(reg),
	sizeof (reg),
	&(reg),
	sizeof (reg),
	&cb,
	0);

	if ( !fResult )
	{
		nRetval = MEDIA_ERROR;
	}
	else if ( reg.reg_Flags & 0x0001 )
	{
		switch ( reg.reg_EAX & 0xff )
		{
			case 0x02:
				nRetval = NO_MEDIA;
				break;
			case 0x04:
			case 0x06:
			case 0x0A:
			case 0x0B:
			case 0x0C:
				nRetval = MEDIA_ERROR;
				break;
			case 0x07:
				nRetval = UNREADABLE_MEDIA;
				break;
			case 0x08:
				nRetval = SECTOR_RANGE_ERROR;
				break;
			default:
				nRetval = MEDIA_ERROR;
				break;
		}
	}
	else
	{
		nRetval = SUCCESS;
		*dwDataLen = dwReadSize;
	}

	/* Copy to local Buffer and Free it */
	if (nRetval == SUCCESS)
	{
		*dwDataLen = dwReadSize;
		memcpy(Buffer,lpSector,dwReadSize);
	}
	
    VirtualFree (lpSector, 0, MEM_RELEASE);

	return nRetval;
}

/*****************************************************************
	Fucntion		:	WriteHarddisk
	
	Returns			:	SUCCESS - on success
						ERROR_VALUES - on any other error.
	
	Description		:	Write to desired sectors starting from
						SectorNo to SectorNo+Sectors.
******************************************************************/
#if defined (WIN64)
int CWin9xHarddiskReader::WriteHarddisk(unsigned int SectorNo,unsigned int Sectors,
							char *Buffer)
#else
int CWin9xHarddiskReader::WriteHarddisk(unsigned long SectorNo,unsigned long Sectors,
							char *Buffer)
#endif							
{
#if defined (WIN64)
	unsigned int	dwReadSize;
#else
	unsigned long	dwReadSize;
#endif
	LPBYTE			lpSector;
	CONTROLBLOCK	ControlBlock;
	DIOC_REGISTERS	reg ;
	BOOL			fResult ;
	DWORD			cb;
	int				nRetval;

	if (m_dwSectorSize == 0)
	{
		nRetval = TestUnitReady();
		
		if(nRetval != SUCCESS)
			return nRetval;
	}

	if (Sectors ==  0)
	{
		nRetval = SUCCESS;
		return nRetval;
	}


	/*Calculate Buffer Requirements*/
	dwReadSize = Sectors * m_dwSectorSize;

    /* Allocate buffer */
	lpSector = (unsigned char *)VirtualAlloc (NULL, dwReadSize,
									MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
	if (lpSector == NULL)
	{
		nRetval = INVALID_PARAMS;
		return nRetval;		
	}

	/* copy from buffer to Allocated Memory */
	memcpy(lpSector,Buffer,dwReadSize);

	ControlBlock.NumberOfSectors = (WORD)Sectors;
#if defined (WIN64)
	ControlBlock.StartingSector = (unsigned int)SectorNo;	
	ControlBlock.pBuffer = (unsigned int)lpSector;
#else
	ControlBlock.StartingSector = (DWORD)SectorNo;	
	ControlBlock.pBuffer = (DWORD)lpSector;
#endif

	reg.reg_ESI = 0x01; /* 0x01 = Write */
	reg.reg_ECX = -1 ;
#if defined (WIN64)
	reg.reg_EBX = (unsigned int)(&ControlBlock);
#else
	reg.reg_EBX = (DWORD)(&ControlBlock);
#endif
	reg.reg_EDX = m_nDriveLetter - 'A' + 1;
	reg.reg_EAX = 0x7305 ;

	//  6 == VWIN32_DIOC_DOS_DRIVEINFO
	fResult = DeviceIoControl ( m_hHarddiskDevice,
	VWIN32_DIOC_DOS_DRIVEINFO,
	&(reg),
	sizeof (reg),
	&(reg),
	sizeof (reg),
	&cb,
	0);

	/*
			Error coee is set in AX register.
			Error code if a sector read/write error.
		   ;  00h = Disk is write-protected.
		   ;  01h = Invalid drive number.
		   ;  02h = Drive not ready.
		   ;  04h = Bad CRC.
		   ;  06h = Couldn't access track.
		   ;  07h = Unrecognizable disk format.
		   ;  08h = Sector not found.
		   ;  0Ah = Write error.
		   ;  0Bh = Read error.
		   ;  0Ch = General unknown error.
	*/

	if ( !fResult )
	{
		nRetval = MEDIA_ERROR;
	}
	else if ( reg.reg_Flags & 0x0001 )
	{
		switch ( reg.reg_EAX & 0xff )
		{
			case 0x00:
				nRetval = WRITE_PROTECT_ERROR;
				break;
			case 0x02:
				nRetval = NO_MEDIA;
				break;
			case 0x04:
			case 0x06:
			case 0x0A:
			case 0x0B:
			case 0x0C:
				nRetval = MEDIA_ERROR;
				break;
			case 0x07:
				nRetval = UNREADABLE_MEDIA;
				break;
			case 0x08:
				nRetval = SECTOR_RANGE_ERROR;
				break;
			default:
				nRetval = MEDIA_ERROR;
				break;
		}
	}
	else
	{
		nRetval = SUCCESS;
	}

	/* Copy to local Buffer and Free it */
	VirtualFree (lpSector, 0, MEM_RELEASE);

	return nRetval;
}

/*****************************************************************
	Fucntion		:	OpenDrive
	
	Returns			:	SUCCESS - on success
						ERROR_VALUES - on any other error.
	
	Description		:	Open the harddisk drive for reading.
******************************************************************/
int CWin9xHarddiskReader::OpenDrive()
{
	int	nRetval;

	m_hHarddiskDevice = CreateFile ( "\\\\.\\vwin32",
						0,
						0,
						NULL,
						0,
						FILE_FLAG_DELETE_ON_CLOSE,
						NULL );

	if (m_hHarddiskDevice == INVALID_HANDLE_VALUE)
		nRetval = NO_MEDIA;
	else
		nRetval = SUCCESS;
	
	return nRetval;
}

/*****************************************************************
	Fucntion		:	CloseDrive
	
	Returns			:	SUCCESS - on success
	
	Description		:	Close the drive
******************************************************************/
int	CWin9xHarddiskReader::CloseDrive()
{
	CloseHandle(m_hHarddiskDevice);

	return SUCCESS;
}

/*****************************************************************
	Function	:	ValidateDrive
				
	Returns		:	SUCCESS	- on success.
					MEDIA_ERROR - on failure
	
	Description	:	Validate the harddisk drive letter for
					existance of FLOPPY drive.
******************************************************************/
int CWin9xHarddiskReader::ValidateDrive()
{
	char	DriveList[1024];
	int		nTotalDrives, nIndex;
	
	nTotalDrives = ListHarddiskDrives(DriveList);

	for(nIndex=0; nIndex<nTotalDrives; nIndex++)
	{
		if( DriveList[nIndex] == m_nDriveLetter )
			return SUCCESS;
	}
	
	return NO_MEDIA;
}

/*****************************************************************
	Function	:	GetDiskInfo
				
	Returns		:	SUCCESS	- on success.
					error code - on any error
	
	Description	:	Reads the disk information from boot sector.
******************************************************************/
#if defined (WIN64)
int CWin9xHarddiskReader::GetDiskInfo(unsigned int *SectorSize, unsigned int *SectorCount)
#else
int CWin9xHarddiskReader::GetDiskInfo(unsigned long *SectorSize, unsigned long *SectorCount)
#endif
{
	BPB		FirstSector;
	CONTROLBLOCK	ControlBlock;
	DIOC_REGISTERS	reg ;
	BOOL			fResult ;
	unsigned long	cb;
	LPBYTE			lpSector;
#if defined (WIN64)
	static unsigned int	PrevMediaID	 = 0;
#else
	static unsigned long	PrevMediaID	 = 0;
#endif

    lpSector = (unsigned char *)VirtualAlloc (NULL, 2048,
									MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
	if (lpSector == NULL)
	{
		return INVALID_PARAMS;
	}

#if defined (WIN64)
	ControlBlock.StartingSector = (unsigned int)0;	/* Read Boot Sector */
	ControlBlock.pBuffer = (unsigned int)lpSector;
#else
	ControlBlock.StartingSector = (unsigned long)0;	/* Read Boot Sector */
	ControlBlock.pBuffer = (unsigned long)lpSector;
#endif
	ControlBlock.NumberOfSectors = (WORD)1;

	reg.reg_ESI = 0x00;
	reg.reg_ECX = -1 ;
#if defined (WIN64)
	reg.reg_EBX = (unsigned int)(&ControlBlock);
#else
	reg.reg_EBX = (unsigned long)(&ControlBlock);
#endif
	reg.reg_EDX = m_nDriveLetter - 'A' + 1;
	reg.reg_EAX = 0x7305 ;


	fResult = DeviceIoControl ( m_hHarddiskDevice,
	VWIN32_DIOC_DOS_DRIVEINFO,
	&(reg),
	sizeof (reg),
	&(reg),
	sizeof (reg),
	&cb,
	0);

	if (!fResult )
	{
		VirtualFree (lpSector, 0, MEM_RELEASE);
		return NO_MEDIA;	/* Set No Media*/
	}
	if ( reg.reg_Flags & 0x0001 )
	{
		VirtualFree (lpSector, 0, MEM_RELEASE);
		return NO_MEDIA; /* Set No Media*/
	}

	memcpy( &FirstSector, lpSector, sizeof(BPB) );

	if( FirstSector.bsMedia != 0xF0 && FirstSector.bsMedia != 0xF9 )
	{
		/* The media is not 3 1/2 harddisk disk */
		VirtualFree (lpSector, 0, MEM_RELEASE);
		return WRONG_MEDIA;
	}

	if( IsHDFSFAT32((unsigned char *)&FirstSector) )
	{
		BOOTSECTOR_32 *pBootSector = (BOOTSECTOR_32*)&FirstSector;

		if ( PrevMediaID != pBootSector->bsVolumeID )
		{
			/* The media is changed */
			VirtualFree (lpSector, 0, MEM_RELEASE);
			PrevMediaID = pBootSector->bsVolumeID;
			return MEDIUM_CHANGE;
		}
	}
	else if( IsHDFSFAT16((unsigned char *)&FirstSector) )
	{
		BOOTSECTOR_12_16 *pBootSector = (BOOTSECTOR_12_16*)&FirstSector;
		if ( PrevMediaID != pBootSector->bsVolumeID )
		{
			/* The media is changed */
			VirtualFree (lpSector, 0, MEM_RELEASE);
			PrevMediaID = pBootSector->bsVolumeID;
			return MEDIUM_CHANGE;
		}
	}
	
	if ( FirstSector.bsSectors == 0 )
	{
		*SectorCount = FirstSector.bsHugeSectors;
	}
	else
	{
		*SectorCount = FirstSector.bsSectors;
	}

	*SectorSize	= FirstSector.bsBytesPerSec;

	/* Copy to local Buffer and Free it */
	 VirtualFree (lpSector, 0, MEM_RELEASE);

	return SUCCESS;
}

int CWin9xHarddiskReader::CheckForMediaChange()
{
	DIOC_REGISTERS	reg ;
	BOOL			fResult;
	int				nRetval = SUCCESS;
	unsigned long	dwNotUsed;

	memset(&reg,0,sizeof(reg));

	DIOC_DL(&reg) =  0;
	DIOC_AH(&reg)  = 0x16;

	//  6 == VWIN32_DIOC_DOS_DRIVEINFO
	fResult = DeviceIoControl ( m_hHarddiskDevice,
	VWIN32_DIOC_DOS_INT13,
	&(reg),
	sizeof (reg),
	&(reg),
	sizeof (reg),
	&dwNotUsed,
	0);

	if (fResult)
	{
		if( DIOC_AH(&reg) != 0 )
			nRetval = MEDIUM_CHANGE;
		else
			nRetval = SUCCESS;
	}
	else
	{
		nRetval = NO_MEDIA;
	}

	return nRetval;
}
