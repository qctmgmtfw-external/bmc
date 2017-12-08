/*****************************************************************
*	File		:	MacFloppyReader.cpp
*	Author		:	Senguttuvan  M(senguttuvanm@amiindia.co.in)
*	Created		:	27-October-2009
*	Description	:	Implements Floppy redirection routines
*					for Mac
******************************************************************/

/**
* @file MacFloppyReader.cpp
* @author Senguttuvan M(senguttuvanm@amiindia.co.in)
* @date 27-October-2009
*
* @brief Implements routines for Floppy Redirection in linux.
*/


#include "MacFloppyReader.h"
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <dbgout.h>
#include "libfloppy.h"
#include <IOKit/storage/IOMediaBSDClient.h>


/****************************** public member functions**********************/
/*****************************************************************
	Function	:	Constructor.
	Description	:	Intializes the object.
******************************************************************/
CMacFloppyReader::CMacFloppyReader()
{
	m_bIsDeviceOpen		=	false;
	m_dwSectorSize		=	0;
	m_dwSectorCount		=	0;
	m_ProgressPercentage=	0;
	m_bFlagStop			=	false;
	m_bFirstTime		=	false;
	m_hFloppyDevice	    =   0;
}

/*****************************************************************
	Function	:	Destructor.
	Description	:	Deallocate any buffer allocated.
******************************************************************/
CMacFloppyReader::~CMacFloppyReader()
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
int CMacFloppyReader::OpenDevice(char *pszDevicePath, enum byteOrder nByteOrder)
{
	int				nRetval;

	/* check the flag whether last drive is closed */
	if( m_bIsDeviceOpen )
	{
		TCRIT("Device is already in open state\n");
		return DEVICE_ALREADY_OPEN;
	}

	/* first character contains the drive letter.*/
	strcpy(m_szDevicePath, pszDevicePath);
	m_nByteOrder	= nByteOrder;

	/* Validate the drive, if fails returns media error.*/
	nRetval = ValidateDrive();

	if(nRetval != SUCCESS)
	{
		TCRIT("Invalid Floppy Device\n");
		CloseDevice();
		return nRetval;
	}

	TDBG("Start Redirecting the Floppy device %s\n", m_szDevicePath );

	/*Set the OPEN flag*/
	m_bIsDeviceOpen = true;
	m_bFirstTime	= true;

	return nRetval;
}

/*****************************************************************
	Function	:	CloseDevice

	Returns		:	SUCCESS	- on success.

	Description	:	close the device and resets the OPEN flag.
******************************************************************/
int CMacFloppyReader::CloseDevice()
{
	/*reset the OPEN flag*/
	m_bIsDeviceOpen	= false;
	m_bFirstTime = false;

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

	Description	:	Execute the command on local floppy drive.
					Get the status and fill the response packet.
					Add the data to reponse packet as a result if any.
					fill data-length field.
******************************************************************/

#if defined (__x86_64__)
int CMacFloppyReader::ExecuteSCSICmd(IUSB_SCSI_PACKET *pRequestPkt,
						IUSB_SCSI_PACKET *pResponsePkt,
						unsigned int *pdwDataLen)
#else
int CMacFloppyReader::ExecuteSCSICmd(IUSB_SCSI_PACKET *pRequestPkt,
						IUSB_SCSI_PACKET *pResponsePkt,
						unsigned long *pdwDataLen)
#endif

{
	int			nRetval=SUCCESS;
#if defined (__x86_64__)
	unsigned int		dwDataLen=0;
	unsigned int		dwSectorNo, dwSectors;
#else
	unsigned long		dwDataLen=0;
	unsigned long		dwSectorNo, dwSectors;
#endif

	SCSI_COMMAND_PACKET	*ScsiPkt;
	READ_CAPACITY		*ScsiReadCap;
	READ_FORMAT_CAPACITIES	*ReadFmtCaps;
	static bool		bMediaChanged=false;

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

	if( nRetval == SUCCESS )
		nRetval = OpenDriveReadOnly();

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
				ReadFmtCaps = (READ_FORMAT_CAPACITIES *)(&(pResponsePkt->Data));
				ReadFmtCaps->ListLength  = 8;

				nRetval = ReadCapacity(&ReadFmtCaps->BlockSize,&ReadFmtCaps->TotalBlocks);

				if(nRetval != SUCCESS)
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

				dwDataLen = sizeof(READ_FORMAT_CAPACITIES);

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

				if ( dwSectors <= MAX_FLOPPY_READ_SECTORS)
				{
					/* Read till successfull */
					nRetval = ReadFloppy(dwSectorNo,
								dwSectors,
								&dwDataLen, (char *)(&(pResponsePkt->Data)));
					// To support Virtual floppy boot, the following
					// values must be modified everytime we supply
					// the zeroth sector
					//  bsHiddenSectors must be 0
					//  bsHeads must be 0
					//  bsSecPerTrack must be 0x3f for devices larger than 8mb
					//  bsHeads must be 0xff for devices  larger than 8mb
					if( nRetval == SUCCESS && dwSectorNo == 0)
					{
						#if defined (__x86_64__)
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
								if( IsFSFAT32((unsigned char *)(&(pResponsePkt->Data)) ) )
								{
									BOOTSECTOR_32 *pBootSector = (BOOTSECTOR_32 *)(&(pResponsePkt->Data));

									pBootSector->bsParamBlock.bsHiddenSectors = 0;
									pBootSector->bsDriveNumber = 0;
									pBootSector->bsParamBlock.bsSecPerTrack = NUM_OF_TRACKS;
									pBootSector->bsParamBlock.bsHeads = NUM_OF_HEADS;
								}
								else if( IsFSFAT16((unsigned char *)(&(pResponsePkt->Data)) ) )
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
				/* Close the drive and open in write mode */
				CloseDrive();

				nRetval = OpenDriveWritable();
				if( nRetval == SUCCESS )
				{
					dwSectorNo = ScsiPkt->Lba;
					dwSectors  = ScsiPkt->CmdLen.Cmd10.Length;

					/* convert the from bigendian*/
					if( m_nByteOrder == bigendian )
					{
						dwSectorNo = mac2blong(dwSectorNo);
						dwSectors  = mac2bshort(dwSectors);
					}

					if ( dwSectors <= MAX_FLOPPY_READ_SECTORS)
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
							int nRet;
							#if defined (__x86_64__)
								unsigned int dwTemp=0;
							#else
								DWORD dwTemp=0;
							#endif

							nRet = ReadFloppy(0, 1, &dwTemp, (char*)SectZero);

							if( nRet == SUCCESS)
							{

								#if defined (__x86_64__)
									unsigned int SectorSize = 0;
									unsigned int SectorCount = 0;
								#else
									unsigned long SectorSize = 0;
									unsigned long SectorCount = 0;
								#endif
								if( ReadCapacity(&SectorSize,&SectorCount) == SUCCESS )
								{

									if( SectorCount >= (NUM_OF_TRACKS*NUM_OF_HEADS) )
									{
										if( IsFSFAT32((unsigned char *)(&(pResponsePkt->Data)) ) )
										{
											BOOTSECTOR_32 *pBootSector = (BOOTSECTOR_32 *)(&(pResponsePkt->Data));
											BOOTSECTOR_32 *pOrigBootSector = (BOOTSECTOR_32 *)(SectZero);

											pBootSector->bsParamBlock.bsHiddenSectors = pOrigBootSector->bsParamBlock.bsHiddenSectors;
											pBootSector->bsDriveNumber = pOrigBootSector->bsDriveNumber;
											pBootSector->bsParamBlock.bsSecPerTrack = pOrigBootSector->bsParamBlock.bsSecPerTrack;
											pBootSector->bsParamBlock.bsHeads = pOrigBootSector->bsParamBlock.bsHeads;
										}
										else if( IsFSFAT16((unsigned char *)(&(pResponsePkt->Data)) ) )
										{
											BOOTSECTOR_12_16 *pBootSector = (BOOTSECTOR_12_16 *)(&(pResponsePkt->Data));
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
						nRetval = WriteFloppy(dwSectorNo,
									dwSectors,
									(char *)(&(pRequestPkt->Data)));
					}
					else
					{
						nRetval = INVALID_PARAMS;
					}
				}

				dwDataLen = 0;
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
							#if defined (__x86_64__)
								unsigned int	dwTemp=0;
							#else
								unsigned long	dwTemp=0;
							#endif

							nRetval = ReadFloppy(0,1,&dwTemp, (char *)FirstSector);

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
			default:
				nRetval = UNSUPPORTED_COMMAND;
				break;
		}/* end switch case*/

		CloseDrive();
	}/* nRetval; == SUCCESS */

	if(nRetval != SUCCESS)
	{
		//nRetval =GetDiskGeo();
		nRetval = MEDIUM_CHANGE;
		// Once the medium is changed, it needs to be closed
		//and reopened again 
		CloseDrive();
		if( OpenDriveReadOnly() != SUCCESS )
		{
			printf("Medium changed, unable to open the new media. returning media error");
			nRetval = MEDIA_ERROR;			
		}
	}

	/* Set the error flags for response packet based on the error */
	SetErrorStatus(nRetval, pResponsePkt);

	/* Set the datalength of response data */
	pResponsePkt->DataLen	=	dwDataLen;
	*pdwDataLen		=		dwDataLen;

	return nRetval;
}

static int ListUSBKeys(char *pDriveList, int *drive_count)
{
	FILE	*usb_key_list;
	        char buf[512];
		int index = 0;
		char delims[] = "/rdisk";
		char *result = NULL;
		char value[50] ;
	        char character;
		// To get the Harddisk and USB disk list
	        usb_key_list = popen( "ls /dev/rdisk*", "r" );

	    /* Read the output of the above shell command.  Each line contains */
	    /* the name of a cdrom device                                      */
		while( !feof( usb_key_list ) && !ferror( usb_key_list ) )
	        {
	           /* Read the output 1 character at a time... */
	           if( fread( &buf[ index ], sizeof( char ), (size_t)1, usb_key_list ) == 1 )
	           {
			index++;
	           }
	        }

		result = strstr( buf, delims );
		index = 0;
		int		harddiskDevice = 0;
		unsigned long SectorSize = 0;
		unsigned long SectorCount = 0;
		while( result != NULL ) {
	                result++;
			 strncpy(value, result, 6);
	                 value[6] = '\0';
			sprintf(pDriveList, "/dev/%s", value);
			result = strstr(result+8, delims );
			harddiskDevice = open (pDriveList, O_RDONLY, 0);

			if (harddiskDevice < 0 )
			{
				close(harddiskDevice);
				continue;
			}
			else
			{
				// Checking for Harddisk and USB key. Harddisk and USB key are having sector size as 512.
				ioctl(harddiskDevice, DKIOCGETBLOCKCOUNT, &SectorCount);
				ioctl(harddiskDevice, DKIOCGETBLOCKSIZE, &SectorSize);
				close(harddiskDevice);
				
				if(SectorSize == 512 && SectorCount == 2880)
				{
					pDriveList += (strlen(pDriveList) + 1);
					(*drive_count)++;
				}

			}
		}

	        pclose( usb_key_list );
	*pDriveList = '\0';
	return 0;
}
/*****************************************************************
	Function	:	ListFloppyDrives

	Parameters	:	DriveList - It is an OUT parameter returned with
						a string of floppy drive letters connected
						to the system

	Returns		:	Number of floppy drives connected to the system.

	Description	:	Search for the fddrives connected to the system
					and form a string of fd driveletters and return
******************************************************************/
int	CMacFloppyReader::ListFloppyDrives(char *DriveList)
{
	int i;
	char drive[ 10 ];
	int fd;
	int count = 0;
	char drvtype[ 16 ];
	int usb_key_count=0;
	char usb_key_drive_str[256];

    /****** Enumarate USB keys ******/
	if( ListUSBKeys(usb_key_drive_str, &usb_key_count) == 0 )
	{
		// copy the list and update the counter
        char *p_usb_keys = usb_key_drive_str;

		for(i=0;i<usb_key_count;i++) {
			strcpy( DriveList, p_usb_keys );
			DriveList += ( strlen( DriveList ) + 1 );
			p_usb_keys += ( strlen( p_usb_keys ) + 1 );
			count++;
		}
	}

/********************************/
	return count;
}
/**********************************************************************************/

/***************************protected member functions*****************************/
/*****************************************************************
	Fucntion		:	ReadCapacity

	Returns			:	SUCCESS - on success
						ERROR_VALUES - on any other error.

	Description		:	Gets the sectorsize and sectorcount
						parameters of a floppy media
******************************************************************/
#if defined (__x86_64__)
	int CMacFloppyReader::ReadCapacity(unsigned int* SectorSize,unsigned int* SectorCount)
#else
	int CMacFloppyReader::ReadCapacity(unsigned long* SectorSize,unsigned long* SectorCount)
#endif

{
	int			nRetval;
	nRetval = SUCCESS;

    /* Check whether there is any media change since last operation. */
	if (ioctl(m_hFloppyDevice, DKIOCGETBLOCKSIZE, SectorSize) < 0 )
	{
		*SectorSize = 0;
		*SectorCount= 0;
		nRetval = NO_MEDIA;
	}

	if(ioctl(m_hFloppyDevice, DKIOCGETBLOCKCOUNT, SectorCount) < 0)
	{
		*SectorSize = 0;
		*SectorCount= 0;
		nRetval = NO_MEDIA;
	}

	return nRetval;
}

/*****************************************************************
	Fucntion		:	TestUnitReady

	Returns			:	SUCCESS - on Success
					
	Description		:	Return SUCCESS always
******************************************************************/
int CMacFloppyReader::TestUnitReady()
{
	int nRetval;
	//nRetval = ReadCapacity(&m_dwSectorSize, &m_dwSectorCount);

	return nRetval;
}

/*****************************************************************
	Fucntion		:	GetDiskGeo
	
	Returns			:	SUCCESS - on Success
						ERROR_VALUES - on other errors
	
	Description		:	Checks the Floppy drive for 
						medium change, ready state,
						and informs the state of floppy drive.
******************************************************************/
int CMacFloppyReader::GetDiskGeo()
{
	int nRetval;
	nRetval = ReadCapacity(&m_dwSectorSize, &m_dwSectorCount);

	return nRetval;
}

/*****************************************************************
	Fucntion		:	ReadFloppy

	Returns			:	SUCCESS - on success
						ERROR_VALUES - on any other error.

	Description		:	Read the desired sectors starting from
						SectorNo to SectorNo+Sectors.
******************************************************************/
#if defined (__x86_64__)
	int CMacFloppyReader::ReadFloppy(unsigned int SectorNo,unsigned int Sectors,
								unsigned int *dwDataLen,
								char *Buffer)
#else
	int CMacFloppyReader::ReadFloppy(unsigned long SectorNo,unsigned long Sectors,
								unsigned long *dwDataLen,
								char *Buffer)
#endif
{
	uint32 dwReadSize;
	unsigned char * lpSector;
	ssize_t nResult = 0;
	int nRetval;
	off_t	nSeekSector;

	/* If Disk Geometry is not read, Read it now */
	if (m_dwSectorSize == 0)
	{
		nRetval = GetDiskGeo();

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

	lpSector = (unsigned char *)malloc( dwReadSize );
	if (lpSector == NULL)
	{
		nRetval = INVALID_PARAMS;
		return nRetval;
	}

	nSeekSector = (off_t) (((off_t)SectorNo) * ((off_t)m_dwSectorSize));
	nRetval = SUCCESS;

	/* Seek to the Desired Sector */
	if (lseek (m_hFloppyDevice, nSeekSector, SEEK_SET) != (off_t)-1 )
	{
		/* Read sectors from the compact disc */
		if( ( nResult = read( m_hFloppyDevice, lpSector, dwReadSize ) ) <= 0 )
		{
			if (nResult == 0)
			{
				/* EOF Reached */
				nRetval = SECTOR_RANGE_ERROR;
			}
			else
			{
				/* MEDIA ERROR*/
				nRetval = MEDIA_ERROR;
			}
		}
	}
	else
	{
		/* EOF Reached */
		nRetval = SECTOR_RANGE_ERROR;
	}

	/* Copy to local Buffer and Free it */
	if (nRetval == SUCCESS)
	{
		memcpy(Buffer,lpSector,nResult);
		*dwDataLen = nResult;
	}

	free(lpSector);

	return nRetval;
}

/*****************************************************************
	Fucntion		:	WriteFloppy

	Returns			:	SUCCESS - on success
						ERROR_VALUES - on any other error.

	Description		:	Write to desired sectors starting from
						SectorNo to SectorNo+Sectors.
******************************************************************/
#if defined (__x86_64__)
	int CMacFloppyReader::WriteFloppy(unsigned int SectorNo,unsigned int Sectors,
								char *Buffer)
#else
	int CMacFloppyReader::WriteFloppy(unsigned long SectorNo,unsigned long Sectors,
								char *Buffer)
#endif
{
	uint32 dwReadSize;
	unsigned char * lpSector;
	ssize_t nResult = 0;
	int nRetval;
	off_t nSeekSector;

	if (m_dwSectorSize == 0)
	{
		nRetval = GetDiskGeo();

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
	lpSector = (unsigned char *)malloc( dwReadSize );

	if (lpSector == NULL)
	{
		nRetval = INVALID_PARAMS;
		return nRetval;
	}

	/* copy from buffer to Allocated Memory */
	memcpy(lpSector,Buffer,dwReadSize);

	nSeekSector = (off_t) (((off_t)SectorNo) * ((off_t)m_dwSectorSize));
	nRetval = SUCCESS;

	/* Seek to the Desired Sector */
	if (lseek (m_hFloppyDevice, nSeekSector, SEEK_SET) != (off_t)-1 )
	{
		/* Write sectors to the floppy */
		if( ( nResult = write( m_hFloppyDevice, lpSector, dwReadSize ) ) <= 0 )
		{
			if (nResult == 0)
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
	free(lpSector);

	return nRetval;
}

/*****************************************************************
	Fucntion		:	OpenDriveReadOnly

	Returns			:	SUCCESS - on success
						ERROR_VALUES - on any other error.

	Description		:	Open the floppy drive for reading.
******************************************************************/
int CMacFloppyReader::OpenDriveReadOnly()
{
	int nRetval = SUCCESS;

	m_hFloppyDevice = open (m_szDevicePath, O_RDONLY);

	if (m_hFloppyDevice < 0 )
	{
		nRetval = NO_MEDIA;
	}
	else
		nRetval = SUCCESS;

	return nRetval;
}

/*****************************************************************
	Fucntion		:	OpenDriveWritable

	Returns			:	SUCCESS - on success
						ERROR_VALUES - on any other error.

	Description		:	Open the floppy drive for writing.
******************************************************************/
int CMacFloppyReader::OpenDriveWritable()
{
	int nRetval;

	m_hFloppyDevice = open (m_szDevicePath, O_RDWR);

	if (m_hFloppyDevice < 0 )
	{
		if( errno == EROFS )
			nRetval = WRITE_PROTECT_ERROR;
		else
			nRetval = NO_MEDIA;
	}
	else
		nRetval = SUCCESS;

	return nRetval;
}

/*****************************************************************
	Fucntion		:	CloseDrive

	Returns			:	SUCCESS - on success

	Description		:	Close the drive
******************************************************************/
int CMacFloppyReader::CloseDrive()
{
	close(m_hFloppyDevice);

	return SUCCESS;
}

/*****************************************************************
	Function	:	ValidateDrive

	Returns		:	SUCCESS	- on success.
					MEDIA_ERROR - on failure

	Description	:	Validate the floppy drive letter for
					existance of FLOPPY drive.
******************************************************************/
int CMacFloppyReader::ValidateDrive()
{
	char		szFloppyDrives[1024];
	char		*pszDriveList;
	int		nTotalDrives, nIndex;

	pszDriveList = &szFloppyDrives[0];

	nTotalDrives = ListFloppyDrives(pszDriveList);

	for(nIndex=0; nIndex<nTotalDrives; nIndex++)
	{
		if( strcmp(m_szDevicePath, pszDriveList) == 0 )
		{
			return SUCCESS;
		}

		pszDriveList += ( strlen(pszDriveList) + 1 );
	}

	return NO_MEDIA;
}

