/*****************************************************************
*	File		:	Win9xMediaReader.cpp						 *
*	Author		:	Rajasekhar J(rajasekharj@amiindia.co.in)	 *
*	Created		:	18-June-2003								 *
*	Description	:	Implements CDROM redirection routines		 *
*					for Win95/98/ME								 *
******************************************************************/

/**
* @file Win9xMediaReader.cpp
* @author Raja Sekhar J(rajasekharj@amiindia.co.in)
* @date 18-Jun-2003
*
* @brief Implements the routines for CDROM redirection in Win95/98/ME
*
*/

#include "Win9xMediaReader.h"
#include "string.h"
#include "dbgout.h"

#include "ntddkincludes.h"

#if defined (WIN64)
extern int DoAspiCommand(ScsiRequestBlock *p, int timeout);
#else
extern int DoAspiCommand(ScsiRequestBlock *p, long timeout);
#endif
extern ScbError MapError(ScsiRequestBlock *srb);
extern ScsiRequestBlock LastSRB;

/*****************************************************************
	Function	:	Constructor.
	Description	:	Intializes the object.
******************************************************************/
CWin9xMediaReader::CWin9xMediaReader()
{
	m_bIsDeviceOpen		= false;
	m_dwSectorSize		= 0;
	m_dwSectorCount		= 0;
	m_ScsiBus			= NULL;
	m_pSelectedDrive	= NULL;
	m_nFirstTime		= 1;
}

/*****************************************************************
	Function	:	Constructor.
	Description	:	Deallocate any buffer allocated.
******************************************************************/
CWin9xMediaReader::~CWin9xMediaReader()
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
int CWin9xMediaReader::OpenDevice(char *pszDevicePath, enum byteOrder nByteOrder)
{
	int				nRetval;

	if( m_bIsDeviceOpen )
	{
		TCRIT("Device is already in open state\n");
		return DEVICE_ALREADY_OPEN;
	}

	/* First character is the driveletter of the CDROM Drive*/
	m_nDriveLetter	= pszDevicePath[0];
	m_nByteOrder	= nByteOrder;

	/* Check whether the drive letter belongs to a CDROM Drive */
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
int CWin9xMediaReader::CloseDevice()
{
	/* free the allocated buffer to scsi interface class */
	if( m_ScsiBus != NULL )
	{
		free(m_ScsiBus);
		m_ScsiBus = NULL;
		m_pSelectedDrive = NULL;
	}

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
int	CWin9xMediaReader::ListCDROMDrives(char *DriveList)
{
	int					nDriveNum=0;
	ScsiInterface		*pScsiBus;
	CDDriveSCSIInfoMap	*pCDSCSIInfoMapList = NULL;

	pScsiBus = new ScsiInterface(1);
	nDriveNum = pScsiBus->GetNumCDDrives();

	pCDSCSIInfoMapList = pScsiBus->CDDriveSCSIInfoMapList;
	for(int i = 0;i < nDriveNum; i++)
	{
		DriveList[i] = pCDSCSIInfoMapList->DriveLetter;
		pCDSCSIInfoMapList++;
	}

	free(pScsiBus);
	return nDriveNum;
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
int CWin9xMediaReader::ExecuteSCSICmd(IUSB_SCSI_PACKET *pRequestPkt, IUSB_SCSI_PACKET *pResponsePkt, unsigned int *pdwDataLen)
#else
int CWin9xMediaReader::ExecuteSCSICmd(IUSB_SCSI_PACKET *pRequestPkt, IUSB_SCSI_PACKET *pResponsePkt, unsigned long *pdwDataLen)
#endif
{
	SCSI_COMMAND_PACKET *ScsiPkt;
	READ_CAPACITY		*ScsiReadCap;
	int					nRetval=SUCCESS;
#if defined (WIN64)
	unsigned int		dwDataLen=0;
	unsigned int		dwSectorNo, dwSectors;
	unsigned int		dwMaxTOCSize;
#else
	unsigned long		dwDataLen=0;
	unsigned long		dwSectorNo, dwSectors;
	unsigned long		dwMaxTOCSize;
#endif

	/* Some Fields has to be sent back. So copy all as of now */
	memcpy(pResponsePkt,pRequestPkt,sizeof(IUSB_SCSI_PACKET));

	ScsiPkt=(SCSI_COMMAND_PACKET *)(&(pRequestPkt->CommandPkt));

	dwDataLen = 0;

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
				}
				else
				{
					/* This is not first time*/
					ScsiReadCap = (READ_CAPACITY *)(&(pResponsePkt->Data));

					nRetval = ReadActualCapacity(&ScsiReadCap->BlockSize,
													&ScsiReadCap->TotalBlocks);
					if(nRetval == SUCCESS )
					{
						/* if card follows bigendian format, then format the values
							readable by the card.
						*/
						if( m_nByteOrder == bigendian )
						{
							ScsiReadCap->BlockSize   = mac2blong(ScsiReadCap->BlockSize);
							ScsiReadCap->TotalBlocks = mac2blong(ScsiReadCap->TotalBlocks);
						}

						dwDataLen = 8;
					}
				}

				break;
			/* Read the sectors */
			case SCSI_READ_10:
			case SCSI_READ_12:
			
				dwSectorNo = ScsiPkt->Lba;
				dwSectors = (ScsiPkt->OpCode == SCSI_READ_10) ? ScsiPkt->CmdLen.Cmd10.Length : ScsiPkt->CmdLen.Cmd12.Length32;

				if( m_nByteOrder == bigendian )
				{
					dwSectorNo = mac2blong(dwSectorNo);
					dwSectors  = mac2bshort(dwSectors);
				}

				if (mac2bshort(ScsiPkt->CmdLen.Cmd10.Length) <= MAX_READ_SECTORS)
				{
					/* Read till successfull */
					for(int NumRetries = 0;NumRetries < MAX_READ_RETRIES;NumRetries++)
					{
				 		nRetval = ReadCDROM(dwSectorNo,dwSectors,
												&dwDataLen,
												(char *)(&(pResponsePkt->Data)));

						if(nRetval == SUCCESS) break;
					}
				}
				else
				{
					nRetval = INVALID_PARAMS;
					dwDataLen = 0;
				}
				break;

			/* Read Table of Contents */
			case SCSI_READ_TOC:
				nRetval = ReadTOC(ScsiPkt,&dwDataLen, (char *)(&(pResponsePkt->Data)));

				dwMaxTOCSize = ScsiPkt->CmdLen.Cmd10.Length;

				if( m_nByteOrder == bigendian )
					dwMaxTOCSize = mac2bshort(ScsiPkt->CmdLen.Cmd10.Length);

				if (dwDataLen > dwMaxTOCSize)
					dwDataLen = dwMaxTOCSize;
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
					DoAspiLoadUnloadCommand(1);
				}
				/* Load Media */
				else if(ScsiPkt->Lba == 0x00030000)
				{
					DoAspiLoadUnloadCommand(0);
				}

				nRetval = SUCCESS;
				break;

			default:
				nRetval = UNSUPPORTED_COMMAND;
				break;
		}/* switch */
	}/* else bIsOpned*/

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
int CWin9xMediaReader::ValidateDrive()
{
	unsigned int		index;
	unsigned int		numCDdrives;
	CDDriveSCSIInfoMap	*pCDSCSIInfoMapList;

	/* Validate & Initialize member variables */
	if( m_ScsiBus == NULL )
	{
		m_ScsiBus = new ScsiInterface(1);
	}

	numCDdrives = m_ScsiBus->GetNumCDDrives();

	pCDSCSIInfoMapList = m_ScsiBus->CDDriveSCSIInfoMapList;
	
	for(index = 0;index < numCDdrives ; index++)
	{
		if(m_nDriveLetter == pCDSCSIInfoMapList->DriveLetter)
		{
			break;
		}
		pCDSCSIInfoMapList++;
	}
	
	if( index == numCDdrives )
	{
		return MEDIA_ERROR;
	}

	m_pSelectedDrive = pCDSCSIInfoMapList;
	
	return SUCCESS;
}

/*****************************************************************
	Fucntion		:	TestUnitReady
	
	Returns			:	SUCCESS - on Success
						ERROR_VALUES - on other errors
	
	Description		:	Checks the CDROM drive for 
						medium change, ready state,
						and informs the state of cdrom drive.
******************************************************************/
int CWin9xMediaReader::TestUnitReady()
{
	SCSI_Cdb_TestUnitReady_t cdb;
	static unsigned char	MediumToNoMedium=0;
	int						nRetval = SUCCESS;
#if defined (WIN64)
	unsigned int			dwSectorSize;
	unsigned int			dwSectorCount;
#else
	unsigned long			dwSectorSize;
	unsigned long			dwSectorCount;
#endif

	/* Run for First time only */
	if(m_nFirstTime == 1)
	{
		m_nFirstTime = 0;
		nRetval	= MEDIUM_CHANGE;
		return nRetval;
	}

	memset(&cdb,0,sizeof(cdb));
	cdb.CommandCode = SCSI_Cmd_TestUnitReady;
	cdb.Lun = m_pSelectedDrive->Lun;

	if(DoCommand(&cdb,6,NULL,0,SRB_DIR_SCSI,1L*1000L) == Err_None)
	{
		nRetval = SUCCESS;
		MediumToNoMedium = 1;

		/**Find if media changed here**/
		nRetval = ReadActualCapacity(&dwSectorSize,&dwSectorCount);

		if( nRetval == SUCCESS)
		{
			if(m_dwSectorCount != dwSectorCount)
			{
				nRetval = MEDIUM_CHANGE;
			}

			m_dwSectorCount	= dwSectorCount;
			m_dwSectorSize	= dwSectorSize;
		}
		else
		{
			m_dwSectorCount	= 0;
			m_dwSectorSize	= 0;
			
			nRetval = NO_MEDIA;
		}
	}
	else
	{
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
	Fucntion		:	ReadCapacity
	
	Returns			:	SUCCESS - on success
						ERROR_VALUES - on any other error.
	
	Description		:	Gets the sectorsize and sectorcount 
						parameters of a cdmedia
******************************************************************/
#if defined (WIN64)
int CWin9xMediaReader::ReadCapacity(unsigned int *SectorSize, unsigned int *SectorCount)
#else
int CWin9xMediaReader::ReadCapacity(unsigned long *SectorSize, unsigned long *SectorCount)
#endif
{
	SCSI_Cdb_ReadCapacity_t cdb;
	SCSI_ReadCapacityData_t cd;

	memset(&cd, 0, sizeof(cd));
	memset(&cdb,0,sizeof(cdb));
	cdb.CommandCode = SCSI_Cmd_ReadCapacity;
	cdb.Lun = m_pSelectedDrive->Lun;

	if(!DoCommand(&cdb,sizeof(SCSI_Cdb_ReadCapacity_t),&cd,sizeof(cd),SRB_DIR_IN,1L*1000L))
	   return NO_MEDIA;

#if defined (WIN64)
	*SectorCount = ((unsigned int)cd.BlockAddress3 << 24) |
			  ((unsigned int)cd.BlockAddress2 << 16) |
			  ((unsigned int)cd.BlockAddress1 << 8) |
			  ((unsigned int)cd.BlockAddress0);

	*SectorSize = ((unsigned int)cd.BlockSize3 << 24) |
			  ((unsigned int)cd.BlockSize2 << 16) |
			  ((unsigned int)cd.BlockSize1 << 8) |
			  ((unsigned int)cd.BlockSize0);
#else
	*SectorCount = ((unsigned long)cd.BlockAddress3 << 24) |
			  ((unsigned long)cd.BlockAddress2 << 16) |
			  ((unsigned long)cd.BlockAddress1 << 8) |
			  ((unsigned long)cd.BlockAddress0);

	*SectorSize = ((unsigned long)cd.BlockSize3 << 24) |
			  ((unsigned long)cd.BlockSize2 << 16) |
			  ((unsigned long)cd.BlockSize1 << 8) |
			  ((unsigned long)cd.BlockSize0);
#endif

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
int CWin9xMediaReader::ReadActualCapacity(unsigned int* SectorSize,unsigned int* SectorCount)
#else
int CWin9xMediaReader::ReadActualCapacity(unsigned long* SectorSize,unsigned long* SectorCount)
#endif
{
	SCSI_COMMAND_PACKET readcap_pkt;
	READ_CAPACITY		*pReadCapData;
	int					nRetval=SUCCESS;


	nRetval = ReadCapacity(SectorSize,SectorCount);

	pReadCapData = (READ_CAPACITY*)AllocateAlignedBuffer(sizeof(READ_CAPACITY),1);

	if(pReadCapData == NULL)
	{
		*SectorSize	 = 0;
		*SectorCount = 0;
		nRetval = NO_MEDIA;
		return nRetval;
	}

	memset(&readcap_pkt,0,sizeof(SCSI_COMMAND_PACKET));
	readcap_pkt.OpCode = SCSI_READ_CAPACITY;

	if(DoCommand(&readcap_pkt,sizeof(SCSI_COMMAND_PACKET),pReadCapData,sizeof(READ_CAPACITY),SRB_DIR_IN, 1L*1000L) != 0)
	{
		*SectorSize	 = 0;
		*SectorCount = 0;
		free(pReadCapData);
		nRetval = NO_MEDIA;
		return nRetval;
	}

	*SectorSize	= mac2blong(pReadCapData->BlockSize);
	*SectorCount= mac2blong(pReadCapData->TotalBlocks);
	free(pReadCapData);
	
	return SUCCESS;
}

/*****************************************************************
	Fucntion		:	AllocateAlignedBuffer
	
	Returns			:	Pointer to buffer on Success.
						and NULL on failure, 
	
	Description		:	allocates a alligned buffer.
******************************************************************/
#if defined (WIN64)
PUCHAR CWin9xMediaReader::AllocateAlignedBuffer(unsigned int size, unsigned int Align)
#else
PUCHAR CWin9xMediaReader::AllocateAlignedBuffer(ULONG size, ULONG Align)
#endif
{
    PUCHAR ptr;

    UINT_PTR    Align64 = (UINT_PTR)Align;

    if (!Align) {
       ptr = (PUCHAR)malloc(size);
       }
    else {
       ptr = (PUCHAR)malloc(size + Align);
       ptr = (PUCHAR)(((UINT_PTR)ptr + Align64) & ~Align64);
       }

	return ptr;
}

/*****************************************************************
	Fucntion		:	ReadCDROM
	
	Returns			:	SUCCESS - on success
						ERROR_VALUES - on any other error.
	
	Description		:	Read the desired sectors starting from
						SectorNo to SectorNo+Sectors.
******************************************************************/
#if defined (WIN64)
int	CWin9xMediaReader::ReadCDROM(unsigned int SectorNo,unsigned int Sectors, unsigned int *dwDataLen, char *Buffer)
#else
int	CWin9xMediaReader::ReadCDROM(unsigned long SectorNo,unsigned long Sectors, unsigned long *dwDataLen, char *Buffer)
#endif
{
#if defined (WIN64)
	unsigned int		dwReadSizeBytes;
#else
	unsigned long		dwReadSizeBytes;
#endif
	PUCHAR				lpSector;
	ScbError			err;
	int					nRetval;
	SCSI_COMMAND_PACKET ReadCmd;

	dwReadSizeBytes  = Sectors * 2048;

	lpSector = AllocateAlignedBuffer(dwReadSizeBytes,1);

	if(lpSector == NULL)
	{
		nRetval = INVALID_PARAMS;
		*dwDataLen = 0;
		return nRetval;
	}

	ReadCmd.OpCode = SCSI_READ_10;
	ReadCmd.Lba = mac2blong(SectorNo);
	ReadCmd.CmdLen.Cmd10.Length = mac2bshort(Sectors);

	err = DoCommand(&ReadCmd,sizeof(SCSI_COMMAND_PACKET),lpSector,dwReadSizeBytes,SRB_DIR_IN,120L*1000L);
	if(err == 0)
	{
		memcpy(Buffer,lpSector,dwReadSizeBytes);
		nRetval = SUCCESS;
		*dwDataLen = dwReadSizeBytes;
	}
	else
	{
		*dwDataLen = 0;
		nRetval = NO_MEDIA;
	}

	free(lpSector);
	return nRetval;
}

/*****************************************************************
	Fucntion		:	ReadTOC
	
	Returns			:	SUCCESS - on success
						ERROR_VALUES - on any other error.
	
	Description		:	Read TOC of CDMEDIA.
******************************************************************/
#if defined (WIN64)
int	CWin9xMediaReader::ReadTOC(SCSI_COMMAND_PACKET* cdb,
							   unsigned int *dwDataLen, char *Buffer)
#else
int	CWin9xMediaReader::ReadTOC(SCSI_COMMAND_PACKET* cdb,
							   unsigned long *dwDataLen, char *Buffer)
#endif
{
	CDROM_TOC*	lpTOC;
	int			nRetval=SUCCESS;

#if defined (WIN64)
	unsigned int dwTOCSize = cdb->CmdLen.Cmd10.Length;
#else
	unsigned long dwTOCSize = cdb->CmdLen.Cmd10.Length;
#endif
	if( m_nByteOrder == bigendian )
	{
	
	}
	/*allocate 810 bytes to cover table of contents*/
	/*even if someone asks us for 12 we should read the whole thing..send them only what is needed
	and give them what length is left*/
	lpTOC = (CDROM_TOC*) AllocateAlignedBuffer(mac2bshort(cdb->CmdLen.Cmd10.Length),1);

	if(lpTOC == NULL)
	{
		nRetval = INVALID_PARAMS;
		*dwDataLen = 0;
		return nRetval;
	}

	if(DoCommand(cdb,sizeof(SCSI_COMMAND_PACKET),lpTOC,mac2bshort(cdb->CmdLen.Cmd10.Length),SRB_DIR_IN,120L*1000L) == 0)
	{
		memcpy(Buffer,lpTOC,mac2bshort(cdb->CmdLen.Cmd10.Length));
		*dwDataLen = mac2bshort(cdb->CmdLen.Cmd10.Length);
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

#if defined (WIN64)
ScbError CWin9xMediaReader::DoCommand(void *cdb, unsigned cdblen,
                        void *dbuf, unsigned int dbuflen, int dir,
                        int timeout )
#else
ScbError CWin9xMediaReader::DoCommand(void *cdb, unsigned cdblen,
                        void *dbuf, unsigned long dbuflen, int dir,
                        long timeout )
#endif           
{
   ScsiRequestBlock srb;
   ScbError err = Err_None;

   memset(&srb,0,sizeof(srb));
   srb.io.SRB_Cmd = SC_EXEC_SCSI_CMD;
   srb.io.SRB_HaId = m_pSelectedDrive->AdapterNum;
   srb.io.SRB_Target = m_pSelectedDrive->ScsiId;
   srb.io.SRB_Lun = m_pSelectedDrive->Lun;

   srb.io.SRB_CDBLen = cdblen;
   memcpy(srb.io.SRB_CDBByte,cdb,cdblen);

   srb.io.SRB_BufPointer = (BYTE *) dbuf;
   srb.io.SRB_BufLen = dbuflen;

   srb.io.SRB_Flags = dir;
   return ExecuteCmd(srb,timeout);
}

#if defined (WIN64)
ScbError CWin9xMediaReader::ExecuteCmd(ScsiRequestBlock srb, int timeout)
#else
ScbError CWin9xMediaReader::ExecuteCmd(ScsiRequestBlock srb, long timeout)
#endif
{
   HANDLE hEvent;
   ScbError err = Err_None;

   // Create event to notify completion
   hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

   srb.io.SRB_PostProc = hEvent;
   srb.io.SRB_Flags |= SRB_EVENT_NOTIFY;

   // We have room for the sense data, so fill it in.
   srb.io.SRB_SenseLen = SENSE_LEN;

   if (!DoAspiCommand(&srb, timeout))
   {
      err =  Err_Timeout;
   }
   else
   {
      err = MapError(&srb);
   }

   CloseHandle(hEvent);
   return err;
}

void CWin9xMediaReader::DoAspiAbortCommand()
{
      ScsiRequestBlock a;
      
      memset(&a,0,sizeof(a));
      a.abort.SRB_Cmd = SC_ABORT_SRB;
      a.abort.SRB_HaId = m_pSelectedDrive->AdapterNum;
      a.abort.SRB_ToAbort = &LastSRB;
	  ExecuteCmd((ScsiRequestBlock)a, 3L*1000L);
}

int CWin9xMediaReader::DoAspiLockUnlockCommand(unsigned char lock)
{
   SCSI_Cdb_PreventAllow_t cdb;
   char cd[10];
   int	nRetval = SUCCESS;
   
   memset(&cdb,0,sizeof(cdb));
   cdb.CommandCode = SCSI_Cmd_PreventAllow;
   cdb.Lun = m_pSelectedDrive->Lun;
   cdb.Prevent = lock;

   if( DoCommand(&cdb,sizeof(SCSI_Cdb_PreventAllow_t),&cd,sizeof(cd),SRB_DIR_SCSI,1L*1000L) != Err_None )
   {
		nRetval = REMOVAL_PREVENTED;
   }

   return nRetval;
}

int CWin9xMediaReader::DoAspiLoadUnloadCommand(unsigned char eject)
{
	/* Do some ASPI stuff to do the same thing */
   SCSI_Cdb_LoadUnload_t cdb;
   char cd[10];
   int	nRetval = SUCCESS;

   memset(&cdb,0,sizeof(cdb));
   cdb.CommandCode = SCSI_Cmd_LoadUnload;
   cdb.Lun = m_pSelectedDrive->Lun;
   cdb.LoadEject = eject;

   if( DoCommand(&cdb,sizeof(SCSI_Cdb_LoadUnload_t),&cd,sizeof(cd),SRB_DIR_SCSI,120L*1000L) != Err_None )
   {
		nRetval = REMOVAL_PREVENTED;
   }

   return nRetval;
}

