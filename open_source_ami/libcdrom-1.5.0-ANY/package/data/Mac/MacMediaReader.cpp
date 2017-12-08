/*****************************************************************
*	File		:	MacMediaReader.cpp
*	Author		:	Senguttuvan  M(senguttuvanm@amiindia.co.in)
*	Created		:	27-October-2009
*	Description	:	Implements CDROM redirection routines
*					for Linux
******************************************************************/

/**
* @file MacMediaReader.cpp
* @author Senguttuvan  M(senguttuvanm@amiindia.co.in)
* @date 27-October-2009
*
* @brief Implements routines for CDROM Redirection in WinNT/2K/XP.
*/

#include <stdio.h>
#include <string.h>
//#include <unistd.h>
//#include <ppc/types.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <paths.h>
#include <sys/param.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/IOBSD.h>
#include <IOKit/storage/IOMediaBSDClient.h>
#include <IOKit/storage/IOMedia.h>
#include <IOKit/storage/IOCDMedia.h>
#include <IOKit/storage/IODVDMedia.h>
#include <IOKit/storage/IOCDTypes.h>	
#include <CoreFoundation/CoreFoundation.h>
#include "MacMediaReader.h"
#include <dbgout.h>
#include <IOKit/storage/IOCDMediaBSDClient.h>

#ifndef IO_OBJECT_NULL
// This macro is defined in <IOKit/IOTypes.h> starting with Mac OS X 10.4.
#define	IO_OBJECT_NULL ((io_object_t) 0)
#endif

#define MSF_TO_LBA(msf) (((((msf).minute * 60UL) + (msf).second) * 75UL) + (msf).frame - 150)
#define DATACDSECTORSIZE 2048


/*****************************************************************
    Function    :    Constructor.
    Description    :    Intializes the object.
******************************************************************/
CMacMediaReader::CMacMediaReader()
{
    m_bIsDeviceOpen        =    false;
    m_dwSectorSize        =    0;
    m_dwSectorCount        =    0;
}

/*****************************************************************
    Function    :    Constructor.
    Description    :    Deallocate any buffer allocated.
******************************************************************/
CMacMediaReader::~CMacMediaReader()
{
    CloseDevice();
}

/*****************************************************************
    Function    :    OpenDevice

    Parameters    :    pszDevicePath - first character must be the
                        selected CDROM drive letter.
                    nByteOrder    - byte order used by the MegaRAC
                        card.

    Returns        :    SUCCESS    - on success.
                DEVICE_ALREADY_OPEN - if in open state.
                ERROR_CODE - error value if any other error

    Description    :    Validates the device
                    Checks for existence of the drive.
******************************************************************/
int CMacMediaReader::OpenDevice(char *pszDevicePath, enum byteOrder nByteOrder)
{
    int            nRetval;

    /* check the flag whether last drive is closed */
    if( m_bIsDeviceOpen )
    {
        TCRIT("Device is already in open state\n");
        return DEVICE_ALREADY_OPEN;
    }

    if( strlen(pszDevicePath) > MAX_DEVICE_PATH_SIZE )
    {
        TCRIT("Device is already in open state\n");
        return DEVICE_PATH_TOO_LONG;
    }

    strcpy( m_szDevicePath, pszDevicePath);
    m_nByteOrder    = nByteOrder;

    /* Validate the drive, if fails return NO_MEDIA.*/
    nRetval = ValidateDrive();

    if(nRetval != SUCCESS)
    {
        TCRIT("Invalid CDROM Device\n");
        CloseDevice();
        return nRetval;
    }

    TDBG("XStart Redirecting the CDROM device %s\n", m_szDevicePath );

    /*Set the OPEN flag*/
    m_bIsDeviceOpen = true;

    return SUCCESS;
}

/*****************************************************************
    Function    :    CloseDevice

    Returns        :    SUCCESS    - on success.

    Description    :    close the device and resets the OPEN flag.
******************************************************************/
int CMacMediaReader::CloseDevice()
{

    /*reset the OPEN flag*/
    m_bIsDeviceOpen = false;

    TDBG("Stop Redirecting the CDROM device %s\n", m_szDevicePath );
    
    return SUCCESS;
}

/*****************************************************************
    Function    :    ListCDROMDrives

    Parameters    :    DriveList - It is an OUT parameter returned with
                        a array of cdromdevices connected
                        to the system

    Returns        :    Number of cd drives connected to the system.

    Description    :    Search for the connected cdrom devices and
                return an array of strings identifying
                the cdrom devices.
******************************************************************/
int CMacMediaReader::ListCDROMDrives(char *pCDROMList)
{
	int device_num = 0;
    io_iterator_t	mediaIterator;
    io_object_t		nextMedia;
    kern_return_t	kernResult = KERN_FAILURE;
    int ret = 0;
    char			bsdPath[ MAXPATHLEN ];
	CFIndex maxPathSize = sizeof(bsdPath);
    BOOL isDVD = FALSE;
	*bsdPath = '\0';
	
	
    
    /*! @function IOIteratorNext
	 @abstract Returns the next object in an iteration.
	 @discussion This function returns the next object in an iteration, or zero if no more remain or the iterator is invalid.
	 @param iterator An IOKit iterator handle.
	 @result If the iterator handle is valid, the next element in the iteration is returned, otherwise zero is returned. */
	ret = FindEjectableCDMedia(&mediaIterator, kIOCDMediaClass);
    nextMedia = IOIteratorNext(mediaIterator);
	if(!nextMedia)
	{
		ret = FindEjectableCDMedia(&mediaIterator, kIODVDMediaClass);
    	nextMedia = IOIteratorNext(mediaIterator);
		isDVD = TRUE;
	}
    //if (nextMedia) {
	while (nextMedia) {
        CFTypeRef	bsdPathAsCFString = NULL;
        
		/*! @function IORegistryEntryCreateCFProperty
		 @abstract Create a CF representation of a registry entry's property.
		 @discussion This function creates an instantaneous snapshot of a registry entry property, creating a CF container analogue in the caller's task. Not every object available in the kernel is represented as a CF container; currently OSDictionary, OSArray, OSSet, OSSymbol, OSString, OSData, OSNumber, OSBoolean are created as their CF counterparts. 
		 @param entry The registry entry handle whose property to copy.
		 @param key A CFString specifying the property name.
		 @param allocator The CF allocator to use when creating the CF container.
		 @param options No options are currently defined.
		 @result A CF container is created and returned the caller on success. The caller should release with CFRelease. */
		
        bsdPathAsCFString = IORegistryEntryCreateCFProperty(nextMedia, 
															CFSTR(kIOBSDNameKey), 
															kCFAllocatorDefault, 
															0);
        if (bsdPathAsCFString) {
            size_t devPathLength;
    	    char  temp[ MAXPATHLEN ];
            int fileopen = 0;
            strcpy(bsdPath, _PATH_DEV);
            
            // Add "r" before the BSD node name from the I/O Registry to specify the raw disk
            // node. The raw disk nodes receive I/O requests directly and do not go through
            // the buffer cache.
            
            strcat(bsdPath, "r");
	    	devPathLength = strlen(bsdPath);
            
            if (CFStringGetCString((CFStringRef)bsdPathAsCFString,
								   bsdPath + devPathLength,
								   maxPathSize - devPathLength, 
								   kCFStringEncodingUTF8)) {
				device_num++;
		strcpy(temp, bsdPath);
		strcat(temp, "s0");
		fileopen = open( temp, O_RDONLY | O_NONBLOCK);
		if(fileopen >= 0 )
			strcpy(bsdPath, temp);
		close(fileopen);
                kernResult = KERN_SUCCESS;
            }
            
            CFRelease(bsdPathAsCFString);
        }
		
		/*! @function IOObjectRelease
		 @abstract Releases an object handle previously returned by IOKitLib.
		 @discussion All objects returned by IOKitLib should be released with this function when access to them is no longer needed. Using the object after it has been released may or may not return an error, depending on how many references the task has to the same object in the kernel.
		 @param object The IOKit object to release.
		 @result A kern_return_t error code. */
		strcpy(pCDROMList, bsdPath);
        pCDROMList += (strlen(pCDROMList) + 1);
		
        IOObjectRelease(nextMedia);
		nextMedia = IOIteratorNext(mediaIterator);
		if( (!nextMedia)  && isDVD != TRUE)
		{
			ret = FindEjectableCDMedia(&mediaIterator, kIODVDMediaClass);
    		nextMedia = IOIteratorNext(mediaIterator);
			isDVD = TRUE;
			
		}
		else if ((!nextMedia) && isDVD == TRUE)
		{
			break;
		}
   		
    }
	
    *pCDROMList = '\0';

    return device_num;
}

/*****************************************************************
    Function    :    ExecuteSCSICmd

    Parameters    :    pRequestPkt - command request packet sent by
                        the MegaRAC Card.
                    pResponsePkt - response packet sent to the
                        MegaRAC Card.
                    pdwDataLen    - length of data if any data
                        added to the response packet. This will
                        be useful while sending the reponse packet
                        to the card.

    Returns        :    SUCCESS    - on success.
                ERROR_CODE - error value if any other error

    Description    :    Execute the command on local cdrom drive.
                    Get the status and fill the response packet.
                    Add the data to reponse packet as a result if any.
                    fill data-length field.
******************************************************************/
#if defined (__x86_64__)
	int CMacMediaReader::ExecuteSCSICmd(IUSB_SCSI_PACKET *pRequestPkt,
						IUSB_SCSI_PACKET *pResponsePkt,
						unsigned int *pdwDataLen)
#else
	int CMacMediaReader::ExecuteSCSICmd(IUSB_SCSI_PACKET *pRequestPkt,
						IUSB_SCSI_PACKET *pResponsePkt,
						unsigned long *pdwDataLen)
#endif

{
    SCSI_COMMAND_PACKET    *ScsiPkt;
    READ_CAPACITY         *ScsiReadCap;

#if defined (__x86_64__)
    unsigned int        dwDataLen;
    unsigned int         dwSectorNo, dwSectors, dwMaxTOCSize;
#else
    unsigned long        dwDataLen;
    unsigned long         dwSectorNo, dwSectors, dwMaxTOCSize;
#endif
    int            nRetval = SUCCESS;
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
                ScsiReadCap = (READ_CAPACITY *)(&(pResponsePkt->Data));

                nRetval = ReadActualCapacity(&ScsiReadCap->BlockSize,&ScsiReadCap->TotalBlocks);
                if(nRetval == SUCCESS )
                {
                    /* if card follows bigendian format, then format the values
                       readable by the card.
                    */

                    if ( m_nByteOrder == bigendian )
                    {
                        ScsiReadCap->BlockSize   = mac2blong(ScsiReadCap->BlockSize);
                        ScsiReadCap->TotalBlocks = mac2blong(ScsiReadCap->TotalBlocks-1);
                    }
                    dwDataLen = 8;
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

                if( dwSectors <= MAX_READ_SECTORS )
                {

                    nRetval = ReadCDROM( dwSectorNo,
                            dwSectors,
                            &dwDataLen,
                            (char *)(&( pResponsePkt->Data )));
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
                    dwMaxTOCSize = mac2bshort(dwMaxTOCSize);

                if (dwDataLen > dwMaxTOCSize)
                    dwDataLen = dwMaxTOCSize;
                break;

            /* Prevention of medium removal is not supported in this version */
            case SCSI_MEDIUM_REMOVAL:
            case SCSI_START_STOP_UNIT:
            default:
                nRetval = UNSUPPORTED_COMMAND;
                break;
        }/* switch case */

        CloseDrive();
    }/* if nRetval == SUCCESS */

    /* Set the error flags for response packet based on the error */
    SetErrorStatus(nRetval, pResponsePkt);

    /* Set the datalength of response data */
    pResponsePkt->DataLen    =    dwDataLen;
    *pdwDataLen        =    dwDataLen;

    return nRetval;

}

/*****************************************************************
    Function    :    ValidateDrive

    Returns        :    SUCCESS    - on success.
                    MEDIA_ERROR - on failure

    Description    :    Validate the cdrom drive letter for
                    existance of the CDROM Drive.
******************************************************************/
int CMacMediaReader::ValidateDrive()
{
    int    nRetval;

    /* Just open and close the device to make sure that
    it exists .
    */
    nRetval = OpenDrive();
    CloseDrive();

    return nRetval;
}

/*****************************************************************
    Fucntion    :    OpenDrive

    Returns        :    SUCCESS - on success
                NO_MEDIA on failure.

    Description    :    Open the cddrive for reading.
******************************************************************/
int CMacMediaReader::OpenDrive()
{
    int        nRetval = SUCCESS;

    m_hCDROMDevice = open( m_szDevicePath, O_RDONLY | O_NONBLOCK);
    if( m_hCDROMDevice < 0 )
    {
        nRetval = NO_MEDIA;
    }
    else
    {
        nRetval = SUCCESS;
    }

    return( nRetval );
}

/*****************************************************************
    Fucntion    :    CloseDrive

    Returns        :    SUCCESS - on success

    Description    :    Close the drive
******************************************************************/
int CMacMediaReader::CloseDrive()
{
    close(m_hCDROMDevice);

    return SUCCESS;
}

/*****************************************************************
    Fucntion    :    GetSectorSize

    Parameters    :    dwSectorSize - size of each sector

    Returns        :    SUCCESS - on success

    Description    :    Get the cdrom media information
******************************************************************/
#if defined (__x86_64__)
	int CMacMediaReader::GetSectorSize(unsigned int *dwSectorSize)
#else
	int CMacMediaReader::GetSectorSize(unsigned long *dwSectorSize)
#endif
{
    int     ioctl_ret;
    int     nRetval = SUCCESS;
    *dwSectorSize = 0;


	// kCDSectorSizeCDDA        = 2352,
	// kCDSectorSizeMode1       = 2048,
	// kCDSectorSizeMode2       = 2336,
	// kCDSectorSizeMode2Form1  = 2048,
	// kCDSectorSizeMode2Form2  = 2328,
	// kCDSectorSizeWhole       = 2352. 
    // Below ioctl call return the sector size as 2352.
    // But Data CD sector size is 2048.
    // As of now sector size is hardcoded.
    // TODO: Need to find the sector size

	/*
    ioctl_ret = ioctl(m_hCDROMDevice, DKIOCGETBLOCKSIZE, dwSectorSize);
    if( ioctl_ret < 0 )
    {
        nRetval = NO_MEDIA;
        return ioctl_ret;
    }
    */
	
    *dwSectorSize = DATACDSECTORSIZE;
    return(nRetval);
}

/*****************************************************************
    Fucntion        :    ReadActualCapacity

    Returns            :    SUCCESS - on success
                        ERROR_VALUES - on any other error.

    Description        :    Gets the sectorsize and sectorcount
                        parameters of a cdmedia
******************************************************************/
#if defined (__x86_64__)
	int CMacMediaReader::ReadActualCapacity(unsigned int *dwSectorSize, unsigned int *dwSectorCount )
#else
	int CMacMediaReader::ReadActualCapacity(unsigned long *dwSectorSize, unsigned long *dwSectorCount )
#endif
{
    int        nRetval=SUCCESS;
    int ioctl_ret = 0;
    *dwSectorSize    = 0;
    *dwSectorCount    = 0;

    ioctl_ret = ioctl(m_hCDROMDevice, DKIOCGETBLOCKCOUNT, dwSectorCount);
    if( ioctl_ret < 0 )
    {
        nRetval = NO_MEDIA;
        return nRetval;
    }
    nRetval = GetSectorSize(dwSectorSize);
    if( nRetval != SUCCESS )
    {
       nRetval = NO_MEDIA;
       return nRetval;
    }
    return nRetval;
}

/*****************************************************************
    Fucntion        :    TestUnitReady

    Returns            :    SUCCESS - on Success
                        ERROR_VALUES - on other errors

    Description        :    Checks the CDROM drive for
                        medium change, ready state,
                        and informs the state of cdrom drive.
******************************************************************/
int CMacMediaReader::TestUnitReady() 
{
    int        nRetval=SUCCESS;

#if defined (__x86_64__)
    unsigned int    dwSectorSize;
    unsigned int    dwSectorCount;
#else
    unsigned long    dwSectorSize;
    unsigned long    dwSectorCount;
#endif

    nRetval = ReadActualCapacity(&dwSectorSize, &dwSectorCount);

    if(nRetval == SUCCESS)
    {
        /* If differs, that means there is a change in media */
        if( dwSectorCount != m_dwSectorCount )
        {
            nRetval = MEDIUM_CHANGE;
        }
    }

    m_dwSectorCount = dwSectorCount;
    m_dwSectorSize = dwSectorSize;

    return nRetval;
}

/*****************************************************************
    Fucntion        :    ReadTOC

    Returns            :    SUCCESS - on success
                        ERROR_VALUES - on any other error.

    Description        :    Read TOC of CDMEDIA.
******************************************************************/
#if defined (__x86_64__)
	int CMacMediaReader::ReadTOC(SCSI_COMMAND_PACKET* cdb, unsigned int *dwDataLen, char *Buffer)
#else
	int CMacMediaReader::ReadTOC(SCSI_COMMAND_PACKET* cdb, unsigned long *dwDataLen, char *Buffer)
#endif
{
	dk_cd_read_toc_t		cdTOC;
	CDTOC *				tocData		= NULL;
	CDROM_TOC * 		TOC			= NULL;
	uint16_t			size		= 0;
        int nRetval = SUCCESS, i;
        int ioctlerror = 0;
	m_isDVD = FALSE;
    
#define MSF_BIT ( 0x02 )

	*dwDataLen = 0;
	
    // DKIOCCDREADTOC see IOCDMedia::readTOC() in IOCDMedia.h
	bzero ( ( void * ) &cdTOC, sizeof ( dk_cd_read_toc_t ) );
	
	tocData = ( CDTOC * ) calloc ( 1, sizeof ( CDTOC ) );
	if( tocData == NULL)
	{
		nRetval = UNREADABLE_MEDIA;
        return( nRetval );
	}
	cdTOC.format			= kCDTOCFormatTOC;
	cdTOC.formatAsTime		= 0;
	cdTOC.address.session		= 0;
	cdTOC.bufferLength		= sizeof ( dk_cd_read_toc_t );
	cdTOC.buffer			= tocData;
	
	TDBG("Reading number of tracks\n");
        ioctlerror = ioctl(m_hCDROMDevice, DKIOCCDREADTOC, &cdTOC);

        if(errno == EIO)
 	{
	     nRetval = UNREADABLE_MEDIA;
             free ( tocData );
             return( nRetval );
        }

    /* Read the table of contents header */
    if( ioctlerror  < 0 )
    {
    	  //perror("CDTOC");
          m_isDVD = TRUE;
          // NumTracks is hardcoded to value 4. Have to find the way to find the numTracks value for DVD TOC. 
	  SInt32 numTracks =  GetDVDTOCFromDRUTILCommand( "drutil status | grep \"Tracks\"");	
	  TOC = (CDROM_TOC *)Buffer;  
	  TOC->FirstTrack = GetDVDTOCFromDRUTILCommand( "drutil discinfo | grep \"firstTrack(lastSession)\"");    
	  TOC->LastTrack = GetDVDTOCFromDRUTILCommand( "drutil discinfo | grep \"lastTrack(lastSession)\"");		
	  TDBG("Num tracks: %d\n", numTracks);
	  for ( i =0; i < numTracks; i++)	
	  {		
	  	  TOC->TrackData[ i ].TrackNumber = GetDVDTOCFromDRUTILCommand( "drutil trackinfo | grep \"trackNumber\"");
	      TOC->TrackData[i].Control = GetDVDTOCFromDRUTILCommand( "drutil trackinfo | grep \"trackMode\"");	
		  //TODO Below information is needed for the DVD redirection for RHEL host. Need to find the value instead of hardcoded value. 
		  if( ( cdb->Lun & MSF_BIT ) == MSF_BIT )
		  {
			    TOC->TrackData[ i ].Address.MSFAddress[ 0 ] = 0;
				TOC->TrackData[ i ].Address.MSFAddress[ 1 ] = 0;
			    TOC->TrackData[ i ].Address.MSFAddress[ 2 ] = 2;
			    TOC->TrackData[ i ].Address.MSFAddress[ 3 ] = 0;
		  }
	  }
    }
    else
    {
	size = OSSwapBigToHostInt16 ( tocData->length ) + sizeof ( tocData ->length );
	tocData = ( CDTOC * ) calloc ( 1, size );
	cdTOC.format	 = kCDTOCFormatTOC;
	cdTOC.formatAsTime	 = 0;
	cdTOC.address.session	 = 0;
	cdTOC.bufferLength	 = size;
	cdTOC.buffer	 = tocData;
		
	TDBG("Reading all tracks\n");
	if( ioctl(m_hCDROMDevice, DKIOCCDREADTOC, &cdTOC) < 0 )
        {
             nRetval = UNREADABLE_MEDIA;
	     return( nRetval );
	}	
	SInt32 numTracks = CDTOCGetDescriptorCount(tocData);
		
	TOC = (CDROM_TOC *)Buffer;
	TOC->FirstTrack = tocData->sessionFirst;
	TOC->LastTrack = tocData->sessionLast;
	TDBG("Num tracks: %d\n", numTracks);
	TDBG("Session first: %d\n", tocData->sessionFirst);
	TDBG("Session last: %d\n", tocData->sessionLast);

	for ( i =0; i < numTracks; i++)
	{
	    TOC->TrackData[ i ].Control = tocData->descriptors[i].control;
            TOC->TrackData[ i ].Adr = tocData->descriptors[i].adr;
    	    TOC->TrackData[ i ].TrackNumber = 1;
	    // I assumed that the last track having the details of the track. other tracks are having the subtrack info.  Need to be find the right logic
	    // TODO:: Find the correct logic to get the CD track info (minute, second and frame). This was needed for the RHEL host.
	    CDMSF msf = tocData->descriptors[numTracks -1].p;

	    if( ( cdb->Lun & MSF_BIT ) == MSF_BIT )
            {
            	TOC->TrackData[ i ].Address.MSFAddress[ 0 ] = 0;
            	TOC->TrackData[ i ].Address.MSFAddress[ 1 ] = msf.minute;
            	TOC->TrackData[ i ].Address.MSFAddress[ 2 ] = msf.second;
            	TOC->TrackData[ i ].Address.MSFAddress[ 3 ] = msf.frame;
            }
	}	
    }
	
    /* Send back only what they asked for */
    TOC->Length =  cdb->CmdLen.Cmd10.Length;
	
    /* Keep our local copy of the length in little endian format */
    *dwDataLen = mac2bshort(TOC->Length);

    nRetval = SUCCESS;
    if(tocData != NULL)
    free ( tocData );
    return( nRetval );
}

/*****************************************************************
    Fucntion        :    ReadCDROM

    Returns            :    SUCCESS - on success
                        ERROR_VALUES - on any other error.

    Description        :    Read the desired sectors starting from
                        SectorNo to SectorNo+Sectors.
******************************************************************/
#if defined (__x86_64__)
	int CMacMediaReader::ReadCDROM(unsigned int SectorNo, unsigned int Sectors,
									 unsigned int *dwDataLen, char *Buffer)
#else
	int CMacMediaReader::ReadCDROM(unsigned long SectorNo, unsigned long Sectors,
									 unsigned long *dwDataLen, char *Buffer)
#endif 

{
#if defined (__x86_64__)
		unsigned int	dwReadSize;
#else
		unsigned long	 dwReadSize;
#endif	

    unsigned char    *lpSector;
    int    nRetval = SUCCESS;
    *dwDataLen = 0;

	
    /* If Disk Geometry is not read, Read it now */
    if( m_dwSectorSize == 0 )
    {
        nRetval = GetSectorSize(&m_dwSectorSize);
        if(nRetval != SUCCESS)
        {
            return nRetval;
        }
    }

    dwReadSize = Sectors * m_dwSectorSize;
    lpSector = (unsigned char *)malloc( dwReadSize );
	memset(lpSector, 0, dwReadSize);
    if( lpSector == NULL )
    {
        nRetval = INVALID_PARAMS;
        return(nRetval);
    }

	// CD read and DVD read are different. While reading the CD content using the read() function, it throws the error as inappropriate argument.
	// Reading the DVD content using the IOCTL call throws the error as inappropriate IOCTL call. So that only I used the different method to read the DVD
	// and CD content.
	
	if(m_isDVD == TRUE)
	{
		long dwTotalRead = 0;
		dwTotalRead = lseek( m_hCDROMDevice, (SectorNo*m_dwSectorSize), SEEK_SET );
		if( dwTotalRead != (off_t)-1 )    
		{        
			/* Read sectors from the compact disc */        
			if( ( dwTotalRead = read( m_hCDROMDevice, lpSector, dwReadSize ) ) <= 0 )        
			{        	
				if( dwTotalRead == 0 )                
					nRetval = SECTOR_RANGE_ERROR;            
				else                
					nRetval = MEDIA_ERROR;           
			}    

		}    
		else    
		{    	
			nRetval = SECTOR_RANGE_ERROR;        
			dwTotalRead = 0;    
		}
	}
	else
	{
		dk_cd_read_t cd_read;
	        memset( &cd_read, 0, sizeof(cd_read) );
	        cd_read.sectorArea  = kCDSectorAreaUser;
		cd_read.buffer	  = lpSector;
		cd_read.sectorType  = kCDSectorTypeMode1;
		cd_read.offset	  = SectorNo * kCDSectorSizeMode1;
		cd_read.bufferLength = Sectors * kCDSectorSizeMode1;
		if(ioctl( m_hCDROMDevice, DKIOCCDREAD, &cd_read ) == -1 )
		{
			//perror("CDREAD");
			return -1;
		}
	}
    memcpy( Buffer, lpSector, dwReadSize );
    free( lpSector );
   *dwDataLen = dwReadSize;

    return( nRetval );
}
// Returns an iterator across all CD media (class IOCDMedia). Caller is responsible for releasing
// the iterator when iteration is complete.
//kern_return_t CMacMediaReader::FindEjectableCDMedia(io_iterator_t *mediaIterator)
int CMacMediaReader::FindEjectableCDMedia(io_iterator_t *mediaIterator, const char * name)
{
    kern_return_t			kernResult; 
    CFMutableDictionaryRef	classesToMatch;
	
	/*! @function IOServiceMatching
	 @abstract Create a matching dictionary that specifies an IOService class match.
	 @discussion A very common matching criteria for IOService is based on its class. IOServiceMatching will create a matching dictionary that specifies any IOService of a class, or its subclasses. The class is specified by C-string name.
	 @param name The class name, as a const C-string. Class matching is successful on IOService's of this class or any subclass.
	 @result The matching dictionary created, is returned on success, or zero on failure. The dictionary is commonly passed to IOServiceGetMatchingServices or IOServiceAddNotification which will consume a reference, otherwise it should be released with CFRelease by the caller. */
	
    // CD media are instances of class kIOCDMediaClass
    classesToMatch = IOServiceMatching(name); 
    if (classesToMatch == NULL) {
        TCRIT("IOServiceMatching returned a NULL dictionary.\n");
    }
    else {
		/*!
		 @function CFDictionarySetValue
		 Sets the value of the key in the dictionary.
		 @param theDict The dictionary to which the value is to be set. If this
		 parameter is not a valid mutable CFDictionary, the behavior is
		 undefined. If the dictionary is a fixed-capacity dictionary and
		 it is full before this operation, and the key does not exist in
		 the dictionary, the behavior is undefined.
		 @param key The key of the value to set into the dictionary. If a key 
		 which matches this key is already present in the dictionary, only
		 the value is changed ("add if absent, replace if present"). If
		 no key matches the given key, the key-value pair is added to the
		 dictionary. If added, the key is retained by the dictionary,
		 using the retain callback provided
		 when the dictionary was created. If the key is not of the sort
		 expected by the key retain callback, the behavior is undefined.
		 @param value The value to add to or replace into the dictionary. The value
		 is retained by the dictionary using the retain callback provided
		 when the dictionary was created, and the previous value if any is
		 released. If the value is not of the sort expected by the
		 retain or release callbacks, the behavior is undefined.
		 */
		
		CFDictionarySetValue(classesToMatch, CFSTR(kIOMediaEjectableKey), kCFBooleanTrue); 
        // Each IOMedia object has a property with key kIOMediaEjectableKey which is true if the
        // media is indeed ejectable. So add this property to the CFDictionary we're matching on. 
    }
    
    /*! @function IOServiceGetMatchingServices
	 @abstract Look up registered IOService objects that match a matching dictionary.
	 @discussion This is the preferred method of finding IOService objects currently registered by IOKit. IOServiceAddNotification can also supply this information and install a notification of new IOServices. The matching information used in the matching dictionary may vary depending on the class of service being looked up.
	 @param masterPort The master port obtained from IOMasterPort().
	 @param matching A CF dictionary containing matching information, of which one reference is consumed by this function. IOKitLib can contruct matching dictionaries for common criteria with helper functions such as IOServiceMatching, IOOpenFirmwarePathMatching.
	 @param existing An iterator handle is returned on success, and should be released by the caller when the iteration is finished.
	 @result A kern_return_t error code. */
	
    kernResult = IOServiceGetMatchingServices(kIOMasterPortDefault, classesToMatch, mediaIterator);    
    if (KERN_SUCCESS != kernResult) {
        TCRIT("IOServiceGetMatchingServices returned 0x%08x\n", kernResult);
        return -1;
    }
    
    return 1;
}

int CMacMediaReader::GetDVDTOCFromDRUTILCommand(char *command)
{
    FILE *dvd_info;
    char buf[150];
    int index = 0;
    char *value;

    dvd_info = popen( command, "r" );
	memset(buf, '\0', sizeof(buf));
	
    /* Read the output of the above shell command.  Each line contains */
    /* the name of a cdrom device                                      */
    while( !feof( dvd_info ) && !ferror( dvd_info ) )
    {
        /* Read the output 1 character at a time... */
        if( fread( &buf[ index ], sizeof( char ), (size_t)1, dvd_info ) == 1 )
        {
			index++;
        }
    }
   if(index != 0)
   {
       // Outut of the commands  are like firstTrack(lastSession): 2 , lastTrack(lastSession): 4. So that only we are searchig for the ":" character position
       value = strrchr(buf, ':');
      // There was a space after the ":" thats why we increment the value by 2 to get the value
      value = value+2;
   }
   pclose( dvd_info );
   return atoi(value);
}

