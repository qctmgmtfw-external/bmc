/*****************************************************************
*	File		:	WinNTHarddiskReader.cpp						 *
*	Author		:	Rajasekhar J(rajasekharj@amiindia.co.in)	 *
*	Created		:	18-July-2003								 *
*	Description	:	Implements Harddisk redirection routines		 *
*					for WinNT/2K/XP								 *
******************************************************************/

/**
* @file WinNTHarddiskReader.cpp
* @author Raja Sekhar J(rajasekharj@amiindia.co.in)
* @date 18-July-2003
*
* @brief Implements routines for Harddisk Redirection in WinNT/2K/XP.
*
*/

#include "WinNTHarddiskReader.h"
#include <winioctl.h>
#include <windows.h>
#include <stdio.h>
#include "libharddisk.h"
#include "harddiskerrors.h"
#include "Usbdisk.h"
#include "ntddscsi.h"
#define IOCTL_DISK_UPDATE_PROPERTIES    CTL_CODE(IOCTL_DISK_BASE, 0x0050, METHOD_BUFFERED, FILE_ANY_ACCESS)
/*#define SPT_SENSE_LENGTH 32
#define SPTWB_DATA_LENGTH 512
#define CDB10GENERIC_LENGTH                  10
typedef struct _SCSI_PASS_THROUGH_WITH_BUFFERS {
    SCSI_PASS_THROUGH spt;
    ULONG             Filler;      // realign buffers to double word boundary
    UCHAR             ucSenseBuf[SPT_SENSE_LENGTH];
    UCHAR             ucDataBuf[SPTWB_DATA_LENGTH];
    } SCSI_PASS_THROUGH_WITH_BUFFERS, *PSCSI_PASS_THROUGH_WITH_BUFFERS;

typedef struct _SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER {
    SCSI_PASS_THROUGH_DIRECT sptd;
    ULONG             Filler;      // realign buffer to double word boundary
    UCHAR             ucSenseBuf[SPT_SENSE_LENGTH];
    } SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER, *PSCSI_PASS_THROUGH_DIRECT_WITH_BUFFER;

*/


#ifdef ICC_PLATFORM_BIG_ENDIAN
#define GET_SHORT_VALUE(x) (uint16)((((uint16)(x) >> 8) | ((uint16)(x) << 8)))
#define GET_LONG_VALUE(x) ( ((x) >> 24) | ((x) << 24) | (((x) & 0x00ff0000) >> 8) | (((x) & 0x0000ff00) << 8))
#else
#define GET_SHORT_VALUE(x) (x)
#define GET_LONG_VALUE(x)  (x)
#endif

#include <dbgout.h>

/****************************** public member functions**********************/
/*****************************************************************
	Function	:	Constructor.
	Description	:	Intializes the object.
******************************************************************/
CWinNTHarddiskReader::CWinNTHarddiskReader()
{
	m_bIsDeviceOpen		=	false;
	m_dwSectorSize		=	0;
	m_dwSectorCount		=	0;
	m_ProgressPercentage=	0;
	m_bFlagStop			=	false;
	m_bFirstTime		=	false;
	m_writelocksucess	=	false;
	ZeroMemory(m_hLogicalDrive, sizeof(m_hLogicalDrive));
}

/*****************************************************************
	Function	:	Destructor.
	Description	:	Deallocate any buffer allocated.
******************************************************************/
CWinNTHarddiskReader::~CWinNTHarddiskReader()
{
	CloseDevice();
}

/*****************************************************************
	Function	:	OpenDevice
	
	Parameters	:	pszDevicePath - first character must be the
						selected FLOPPY drive letter.
					nByteOrder	- byte order used by the MegaRAC
						card.
				
	Returns		:	SUCCESS				- on success.
					DEVICE_ALREADY_OPEN - if in open state.
					ERROR_CODE			- error value if any other error

	Description	:	Validates the drive letter.
					Checks for existence of the drive.
******************************************************************/
int CWinNTHarddiskReader::OpenDevice(char *pszDevicePath, enum byteOrder nByteOrder)
{
	int				nRetval;
	char *driveno;
	/* check the flag whether last drive is closed */
	if( m_bIsDeviceOpen )
	{
		TCRIT("Device is already in open state\n");
		return DEVICE_ALREADY_OPEN;
	}
	driveno = (char *)malloc(20);
	ZeroMemory(m_nDriveLetter, sizeof(m_nDriveLetter));
	ZeroMemory(m_nDriveName, sizeof(m_nDriveName));
	driveno = strtok (pszDevicePath,"-");
	while (driveno != NULL)
	{
		if(!strlen(m_nDriveLetter))
		{
			strcpy(m_nDriveLetter,driveno);
		}
		else
		{
			if(!strlen(m_nDriveName))
			{
				strcpy(m_nDriveName,driveno);
			}
		}
		driveno = strtok (NULL, "-");
	}
	
	m_nByteOrder	= nByteOrder;
	
	/* Validate the drive, if fails returns media error.*/
	/*nRetval = ValidateDrive();

	if(nRetval != SUCCESS)
	{

		TCRIT("Invalid Harddisk Device\n");
		return nRetval;
	}*/

	/* Open the drive */
	nRetval = OpenDrive();
	if( nRetval < SUCCESS )
	{
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
int CWinNTHarddiskReader::CloseDevice()
{
	/*reset the OPEN flag*/
	if( m_bIsDeviceOpen )
	{
		CloseDrive();
		/* reset the open flag */
		m_bIsDeviceOpen	= false;
		m_bFirstTime	= false;
	}

	return SUCCESS;
}

static int IsHDFSFAT16(unsigned char *data)
{
	if( data[0x26] == 0x28 || data[0x26] == 0x29 )
		return 1;

	return 0;
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
int CWinNTHarddiskReader::ExecuteSCSICmd(IUSB_SCSI_PACKET *pRequestPkt,
										IUSB_SCSI_PACKET *pResponsePkt,
										unsigned int *pdwDataLen)
#else
int CWinNTHarddiskReader::ExecuteSCSICmd(IUSB_SCSI_PACKET *pRequestPkt,
										IUSB_SCSI_PACKET *pResponsePkt,
										unsigned long *pdwDataLen)
#endif										
{
	int						nRetval=SUCCESS;
	int 					nRet=SUCCESS;
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

	if(bMediaChanged)
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
			case SCSI_VERIFY: 
				nRetval == SUCCESS;
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

					ReadFmtCaps->BlockSize   = mac2blong(ReadFmtCaps->BlockSize);
					ReadFmtCaps->TotalBlocks = mac2blong(ReadFmtCaps->TotalBlocks);
				

				dwDataLen = sizeof(HD_READ_FORMAT_CAPACITIES);

				break;

			case SCSI_READ_CAPACITY:
				ScsiReadCap = (READ_CAPACITY *)(&(pResponsePkt->Data));

				nRetval = ReadCapacity(&ScsiReadCap->BlockSize,&ScsiReadCap->TotalBlocks);
				if(nRetval == SUCCESS )
				{
				
						ScsiReadCap->BlockSize   = mac2blong(ScsiReadCap->BlockSize);
						ScsiReadCap->TotalBlocks = mac2blong(ScsiReadCap->TotalBlocks-1);
				

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
				dwSectorNo = mac2blong(dwSectorNo);
				dwSectors  = mac2bshort(dwSectors);
			

				if ( dwSectors <= MAX_HARDDISK_READ_SECTORS)
				{
					/* Read till successfull */
					nRetval = ReadHarddisk(dwSectorNo,
								dwSectors,
								&dwDataLen, (char *)(&(pResponsePkt->Data)));
					
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
					dwSectorNo = mac2blong(dwSectorNo);
					dwSectors  = mac2bshort(dwSectors);
			

				if ( dwSectors <= MAX_HARDDISK_READ_SECTORS)
				{
		
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
							{ //harddisk
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
							{ //harddisk
								DiskPage->TransferRate = mac2bshort(0x01f4); //500kbit/s
								DiskPage->MotorONDelay = 0x05;
								DiskPage->MotorOFFDelay = 0x1e;
								DiskPage->MediumRotationRate = mac2bshort(0x12c);
							}
							else
							{//usbdisk
								DiskPage->TransferRate = mac2bshort(0xf000); //500kbit/s
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
			case SCSI_MODE_SENSE_6: 
				{
			#if defined (WIN64)
					unsigned int SectorSize=0;
					unsigned int SectorCount=0;	 
			#else
					unsigned long SectorSize=0;
					unsigned long SectorCount=0;	 
			#endif
					
			
					SCSI_MODE_SENSE_6_HEADER *ModeSenseResp = 
					(SCSI_MODE_SENSE_6_HEADER *) (&(pResponsePkt->Data));
						 
					dwDataLen=0;
					 
					nRetval = ReadCapacity(&SectorSize, &SectorCount);
			 
					if(nRetval ==SUCCESS)
					{
						memset(ModeSenseResp, 0, sizeof(SCSI_MODE_SENSE_6_HEADER));
						ModeSenseResp->Datalen=sizeof(SCSI_MODE_SENSE_6_HEADER)-1;
						
						nRet =  WriteProtected();
				
						if(nRet == WRITE_PROTECT_ERROR)
						{
							ModeSenseResp->Devicespecific = 0x80;
						}
						else
						{
							ModeSenseResp->Devicespecific = 0;								
						}
						if(!m_writelocksucess)
						{
							ModeSenseResp->Devicespecific = 0x80;
							TWARN("SETTING WRITE PROTECT DEVICE\n");
						}
						if( SectorSize == 512 && SectorCount == 2880 )
						{ //harddisk
							ModeSenseResp->Mediumtype = MEDIUM_TYPE_144_MB;
						}
						else
						{//usbdisk
							ModeSenseResp->Mediumtype = MEDIUM_TYPE_DEFAULT;
						}	
						dwDataLen = sizeof(SCSI_MODE_SENSE_6_HEADER);
					
				}
				else
					nRetval = UNSUPPORTED_COMMAND;
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
int	CWinNTHarddiskReader::ListHardDrives(char *DriveList,int type)
{
	TCHAR Buffer[HD_BUFFER_SIZE];
	int	nIndex=0;

#if defined (WIN64)
	unsigned int Size;
#else
	unsigned long Size;
#endif
	TCHAR *pBuff = &Buffer[0];
		
	nIndex  = ReInitUSB_Disk_Letter( &DriveList[nIndex],type);
	
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
#if 0
int	CWinNTHarddiskReader::ReadCapacity(unsigned long* SectorSize,unsigned long* SectorCount)
{
	unsigned long	dwNotUsed;
	DISK_GEOMETRY	dgHarddisk;
	int				nRetval;

	if (DeviceIoControl(m_hHarddiskDevice, IOCTL_DISK_GET_DRIVE_GEOMETRY,NULL,0,
							&dgHarddisk, sizeof(dgHarddisk),&dwNotUsed, NULL))
	{
		*SectorSize = dgHarddisk.BytesPerSector;
		*SectorCount= (unsigned long)(dgHarddisk.SectorsPerTrack * dgHarddisk.TracksPerCylinder *
					  dgHarddisk.Cylinders.QuadPart);

		nRetval = SUCCESS;
	}
	else
	{
		*SectorSize = 0;
		*SectorCount= 0;

		if(GetLastError() == ERROR_MEDIA_CHANGED)
		{
			nRetval = MEDIUM_CHANGE;
			/* Once the medium is changed, it needs to be closed
			and reopened again */
			CloseDrive();
			OpenDrive();
		}
		else
		{
			nRetval = NO_MEDIA;
		}
	}

	return nRetval;
}
#endif
#if defined (WIN64)
int	CWinNTHarddiskReader::ReadCapacity(unsigned int* SectorSize,unsigned int* SectorCount)
#else
int	CWinNTHarddiskReader::ReadCapacity(unsigned long* SectorSize,unsigned long* SectorCount)
#endif
{
	int nRetval = SUCCESS;
	char		buffer[512];
	BPB	*pBootSector;
	DWORD		dwResult;
	
	/* read BOOT SECTOR (0th sector) */
	if (SetFilePointer (m_hHarddiskDevice, 0, NULL, FILE_BEGIN) != -1)
	{
		if (ReadFile (m_hHarddiskDevice, buffer, HARDDISK_BLOCK_SIZE, &dwResult, NULL))
		{
			/* Check for Bios Parameter Block info (MS floppies only) */
			pBootSector = (BPB*)&buffer[0];
			//if( ( pBootSector->bsMedia >= 0xF0 ) && ( pBootSector->bsMedia <= 0xFF ) )
			if(false)
			{
				/* Image is from some MS formatted disk, so we can get more info */
				*SectorSize = GET_SHORT_VALUE( pBootSector->bsBytesPerSec );
				/**SectorCount = ( pBootSector->bsSectors == 0 ) ?
					GET_LONG_VALUE( pBootSector->bsHugeSectors ):
					GET_SHORT_VALUE( pBootSector->bsSectors );*/
				if      (pBootSector->bsSectors_ntfs0 != 0) *SectorCount = GET_LONG_VALUE(pBootSector->bsSectors_ntfs0);
				else if (pBootSector->bsHugeSectors   != 0) *SectorCount = GET_LONG_VALUE(pBootSector->bsHugeSectors);
				else if (pBootSector->bsSectors       != 0) *SectorCount = GET_SHORT_VALUE(pBootSector->bsSectors);


				return SUCCESS;
			}
			else
			{
				/* If control came here that means the disk is not Microsoft formatted 
					So Method # 2
				*/
				unsigned long	dwNotUsed;
				DISK_GEOMETRY	dgHarddisk;

				if (DeviceIoControl(m_hHarddiskDevice, IOCTL_DISK_GET_DRIVE_GEOMETRY,NULL,0,
										&dgHarddisk, sizeof(dgHarddisk),&dwNotUsed, NULL))
				{
					*SectorSize = dgHarddisk.BytesPerSector;
				#if defined (WIN64)
					*SectorCount= (unsigned int)(dgHarddisk.SectorsPerTrack * dgHarddisk.TracksPerCylinder *				
				#else
					*SectorCount= (unsigned long)(dgHarddisk.SectorsPerTrack * dgHarddisk.TracksPerCylinder *
				#endif
								  dgHarddisk.Cylinders.QuadPart);

					return SUCCESS;
				}
			}
		}
	}

	*SectorSize = 0;
	*SectorCount= 0;

	DWORD dwRetval = GetLastError();
	if(dwRetval == ERROR_MEDIA_CHANGED || dwRetval == ERROR_WRONG_DISK )
	{
		nRetval = MEDIUM_CHANGE;
		/* Once the medium is changed, it needs to be closed
		and reopened again */
		CloseDrive();
		if( OpenDrive() != SUCCESS )
		{
			TEMERG("Medium changed, unable to open the new media. returning media error");
			nRetval = MEDIA_ERROR;			
		}
	}
	else
	{
		nRetval = NO_MEDIA;
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
int CWinNTHarddiskReader::TestUnitReady()
{
	int nRetval;

	nRetval = ReadCapacity(&m_dwSectorSize, &m_dwSectorCount);

	return nRetval;
}

#define LAST_SECT_4GB	(0x800000)
/*****************************************************************
	Fucntion		:	ReadHarddisk
	
	Returns			:	SUCCESS - on success
						ERROR_VALUES - on any other error.
	
	Description		:	Read the desired sectors starting from
						SectorNo to SectorNo+Sectors.
******************************************************************/
#if defined (WIN64)
int	CWinNTHarddiskReader::ReadHarddisk(unsigned int SectorNo,unsigned int Sectors,
							unsigned int *dwDataLen,
							char *Buffer)
#else
int	CWinNTHarddiskReader::ReadHarddisk(unsigned long SectorNo,unsigned long Sectors,
							unsigned long *dwDataLen,
							char *Buffer)
#endif
{
	LPBYTE			lpSector;
	int				nRetval;
#if defined (WIN64)
	unsigned int	dwReadSize;
#else
	unsigned long	dwReadSize;
#endif
	unsigned long	dwResult;
	long	lHighSector = 0;

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

	if( SectorNo >= LAST_SECT_4GB )
	{
		lHighSector = SectorNo/LAST_SECT_4GB;
		SectorNo = SectorNo%LAST_SECT_4GB;
	}
	/* If no error occurs after this line, it means success. */
	nRetval = SUCCESS;

	/* Seek to the Desired Sector */
	if (SetFilePointer (m_hHarddiskDevice, SectorNo*m_dwSectorSize, &lHighSector, FILE_BEGIN) != -1)
	{
		/* Read sectors from the compact disc */
		if (!(ReadFile (m_hHarddiskDevice, lpSector, dwReadSize, &dwResult, NULL)))
		{
			if (dwResult == 0)
			{
				nRetval = SECTOR_RANGE_ERROR;
			}
			else
			{
				nRetval = MEDIA_ERROR;
			}
		}
	}
	else
	{
		nRetval = SECTOR_RANGE_ERROR;
	}

	/* Copy to local Buffer and Free it */
	if (nRetval == SUCCESS)
	{
		*dwDataLen = dwResult;
		memcpy(Buffer,lpSector,dwResult);
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
int CWinNTHarddiskReader::WriteHarddisk(unsigned int SectorNo,unsigned int Sectors,
							char *Buffer)
#else
int CWinNTHarddiskReader::WriteHarddisk(unsigned long SectorNo,unsigned long Sectors,
							char *Buffer)
#endif
{
#if defined (WIN64)
	unsigned int			dwReadSize;
#else
	unsigned long			dwReadSize;
#endif

	LPBYTE			lpSector;
	unsigned long	dwResult = 0;
	int				nRetval = SUCCESS;
	int				nError;
	long	lHighSector=0;
		
	ULONGLONG start_lba = SectorNo;
	ULONG num_sectors = Sectors;

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

    if( SectorNo >= LAST_SECT_4GB )
    {
        lHighSector = SectorNo/LAST_SECT_4GB;
        SectorNo = SectorNo%LAST_SECT_4GB;
    }
	if (SetFilePointer (m_hHarddiskDevice, SectorNo*m_dwSectorSize, &lHighSector, FILE_BEGIN) != -1)
	{
		/* Read sectors from the compact disc */
		if (!(WriteFile (m_hHarddiskDevice, lpSector, dwReadSize, &dwResult, NULL)))
		{
			if (dwResult == 0)
			{
				nError = GetLastError();

				if( nError == ERROR_WRITE_PROTECT)
				{
					nRetval = WRITE_PROTECT_ERROR;
				}
				else
				{
					nRetval = SECTOR_RANGE_ERROR;	
				}
			}
			else
			{
				nRetval = MEDIA_ERROR;
			}
		}
	}
	else
	{
		nRetval = SECTOR_RANGE_ERROR;
	}

	/* Copy to local Buffer and Free it */
	VirtualFree (lpSector, 0, MEM_RELEASE);

	return nRetval;
}

/**
converts given logical drive letter to the physical disk number.
*/
static int LogicalToPhysicaDrive(HANDLE hLogicalDevice, int *nPhysicalDrive)
{
	char	DiskExtentsBuffer[1024];
	VOLUME_DISK_EXTENTS	*pVolumeDiskExtents = (VOLUME_DISK_EXTENTS	*)DiskExtentsBuffer;
	unsigned long	bytesWritten=0;
	int nRetval = -1;

	
	*nPhysicalDrive = 0;
		
	if( DeviceIoControl( hLogicalDevice, IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS, NULL, 0, pVolumeDiskExtents, 
		sizeof(DiskExtentsBuffer), &bytesWritten, NULL ) )
	{
		if( pVolumeDiskExtents->NumberOfDiskExtents > 0 )
		{
			*nPhysicalDrive = pVolumeDiskExtents->Extents[0].DiskNumber;
			nRetval = 0;
		}
		
	}


	return nRetval;
}
#define	READ_WRITE				1
#define	READ					2
/*****************************************************************
	Fucntion		:	OpenDrive
	
	Returns			:	SUCCESS - on success
						ERROR_VALUES - on any other error.
	
	Description		:	Open the harddisk drive for reading.
******************************************************************/

/** OPENS AS A LOGICAL DEVICE **/
int CWinNTHarddiskReader::OpenDrive()
{
	char *DriveString = "\\\\.\\%s";
	char *DriveStringName = "\\\\.\\%c:";
	char DeviceName[30];
	char szTempDrive[50];
	int	nRetval;
	DWORD dwBytesReturned;
	char space = 0x20;
	boolean incomplete=false;
	ZeroMemory(DeviceName, sizeof(DeviceName));
	int len = strlen(m_nDriveName);
	int i = 0;
	for (i = 0; i < len; i++)
	{
		sprintf(DeviceName, DriveStringName,m_nDriveName[i]);
		if(m_nDriveName[i] != space)
		{
			m_hLogicalDrive[i] = CreateFile(DeviceName, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
			if (m_hLogicalDrive[i] == INVALID_HANDLE_VALUE)
			{
				incomplete = true;
				break;
			}
			if (!DeviceIoControl (m_hLogicalDrive[i], FSCTL_DISMOUNT_VOLUME, NULL, 0, NULL, 0, &dwBytesReturned, NULL))
			{
				incomplete = true;
				break;
			}
		}
		else
		{
			continue;
		}
	}
	if(incomplete)
	{
		for(int h=0;h<i;h++)
		{
			if(m_hLogicalDrive[i] != 0)
			{
				CloseHandle(m_hLogicalDrive[i]);
				m_hLogicalDrive[i]=0;
			}
		}
	}
	sprintf(DeviceName, DriveString, m_nDriveLetter);
	if(!incomplete)
	{
		m_hHarddiskDevice = CreateFile (DeviceName, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	
	if (m_hHarddiskDevice == INVALID_HANDLE_VALUE)
	{
			TWARN ("Couldn't open %s GetLastError returned %lu\n", DeviceName, GetLastError());
			return MEDIA_IN_USE;
		}
		if (!DeviceIoControl (m_hHarddiskDevice, FSCTL_LOCK_VOLUME, NULL, 0, NULL, 0, &dwBytesReturned, NULL))
		{
			TWARN("DeviceIoControl to lock drive failed with %u\n", GetLastError());
			return MEDIA_IN_USE;
		}
		else
		{
			TWARN ("locked drive\n");
			m_writelocksucess = true;
			nRetval = READ_WRITE;
		}
	}
	else
		{
			m_hHarddiskDevice = CreateFile (DeviceName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
			if (m_hHarddiskDevice == INVALID_HANDLE_VALUE)
			{
				TWARN ("Couldn't open %s GetLastError returned %lu\n", DeviceName, GetLastError());
				return MEDIA_IN_USE;
			}
			incomplete = false;
			nRetval = READ;
		}
	return nRetval;
}

/*****************************************************************
	Fucntion		:	CloseDrive
	
	Returns			:	SUCCESS - on success
	
	Description		:	Close the drive
******************************************************************/
int	CWinNTHarddiskReader::CloseDrive()
{
	DWORD         dwBytesReturned;
	if(m_writelocksucess)
	{
		if (!DeviceIoControl (m_hHarddiskDevice, FSCTL_UNLOCK_VOLUME, NULL, 0, NULL, 0, &dwBytesReturned, NULL))
		{
			 TWARN("DeviceIoControl to un lock drive failed with %u\n", GetLastError());
		}
		dwBytesReturned=0;
		if (!DeviceIoControl (m_hHarddiskDevice, IOCTL_DISK_UPDATE_PROPERTIES, NULL, 0, NULL, 0, &dwBytesReturned, NULL))
		{
			 TWARN("DeviceIoControl to DISK update failed with %u\n", GetLastError());
			 fflush(stdout);
		}
		int len = strlen(m_nDriveName);
		for (int i = 0; i < (len-1); i++)
		{
			CloseHandle(m_hLogicalDrive[i]);
			m_hLogicalDrive[i]=0;
		}
		m_writelocksucess = false;
	}
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
int CWinNTHarddiskReader::ValidateDrive()
{
	char	DriveList[HD_BUFFER_SIZE];
	
	char *DriveString = "PhysicalDrive%c";
	char DeviceName[30];
	int		nTotalDrives;
	TCHAR *DriveListptr = &DriveList[0];

	nTotalDrives = ListHardDrives(DriveListptr,1);
	
	for(int k=0;k<nTotalDrives;k++)
	{
		char driveno = *DriveListptr;
		sprintf(DeviceName, DriveString, driveno);
		
		if(strcmp(DeviceName,m_nDriveLetter))
		return SUCCESS;
		DriveListptr += strlen(DriveListptr)+1;
	}
	
	nTotalDrives = ListHardDrives(DriveListptr,2);

	for(int k=0;k<nTotalDrives;k++)
	{
		char driveno = *DriveListptr;
		sprintf(DeviceName, DriveString, driveno);
		
		if(strcmp(DeviceName,m_nDriveLetter))
		return SUCCESS;
		DriveListptr += strlen(DriveListptr)+1;
	}
		
	return 1;
}

/*****************************************************************
	Function	:	cWriteProtected
				
	Returns		:	SUCCESS	- on success.
					MEDIA_ERROR - on failure
	
	Description	:	check the harddisk drive for
					write protect.
******************************************************************/

int CWinNTHarddiskReader::WriteProtected()  
{
		char	DiskExtentsBuffer[1024];
		VOLUME_DISK_EXTENTS	*pVolumeDiskExtents = (VOLUME_DISK_EXTENTS	*)DiskExtentsBuffer;
		unsigned long	bytesWritten=0;
		int				nError,nRetval;
	
		if( DeviceIoControl( m_hHarddiskDevice, IOCTL_DISK_IS_WRITABLE, NULL, 0, pVolumeDiskExtents, 
		sizeof(DiskExtentsBuffer), &bytesWritten, NULL ) == 0  )
		{
           
			nError = GetLastError();

			if( nError == ERROR_WRITE_PROTECT)
				 
				{
					nRetval = WRITE_PROTECT_ERROR;
	 				return nRetval;
				}
			
		}

		return 0;
}
