/*****************************************************************
*	File		:	LinuxMediaReader.cpp
*	Author		:	Rajasekhar J(rajasekharj@amiindia.co.in)
*	Created		:	18-June-2003
*	Description	:	Implements CDROM redirection routines
*					for Linux
******************************************************************/

/**
* @file LinuxMediaReader.cpp
* @author Raja Sekhar J(rajasekharj@amiindia.co.in)
* @date 18-Jun-2003
*
* @brief Implements routines for CDROM Redirection in WinNT/2K/XP.
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/cdrom.h>
#include "coreTypes.h"
#include "scsi.h"

#include "LinuxMediaReader.h"
#include "string.h"

#include <dbgout.h>

char scdname[6+16];
char scdstr[16];
/*****************************************************************
    Function    :    Constructor.
    Description    :    Intializes the object.
******************************************************************/
CLinuxMediaReader::CLinuxMediaReader()
{
    m_bIsDeviceOpen        =    false;
    m_dwSectorSize        =    0;
    m_dwSectorCount        =    0;
}

/*****************************************************************
    Function    :    Constructor.
    Description    :    Deallocate any buffer allocated.
******************************************************************/
CLinuxMediaReader::~CLinuxMediaReader()
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
int CLinuxMediaReader::OpenDevice(char *pszDevicePath, enum byteOrder nByteOrder)
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
int CLinuxMediaReader::CloseDevice()
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
int CLinuxMediaReader::ListCDROMDrives(char *pCDROMList)
{
    FILE *cdrom_info;
    char dev_temp[ 10 ];
    int device_num = 0;
    int name_index = 0;
    char device_buffer[20][16];
    int i;

    /* Read /proc/sys/dev/cdrom/info */
    /* The bit of awk script displays all input fields after the first 2 */
    cdrom_info = popen( "grep \"drive name\" /proc/sys/dev/cdrom/info | awk '{ for( i = 1; i <= NF; i++ ) { if( i > 2 ) print $i; } }'", "r" );

    /* Read the output of the above shell command.  Each line contains */
    /* the name of a cdrom device                                      */
    while( !feof( cdrom_info ) && !ferror( cdrom_info ) )
    {
        /* Read the output 1 character at a time... */
        if( fread( &device_buffer[ device_num ][ name_index ],
            sizeof( char ), (size_t)1, cdrom_info ) == 1 )
        {
        /* A newline marks the end of a device name */
        if( device_buffer[ device_num ][ name_index ] == '\n' )
        {
            device_buffer[ device_num ][ name_index ] = '\0';
            device_num++;
            name_index = 0;
        }
        else
            name_index++;
        }
    }


    pclose( cdrom_info );

    /* Convert the names of scsi disks to their proper names */
    /* IE, sr0 -> scd0, sr1 -> scd1, etc                     */
    for( i = 0; i < device_num; i++ )
    {
        if( strncmp( "sr", device_buffer[ i ], 2 ) == 0 )
	{
		struct stat devicenode;
		sprintf( scdstr, "/dev/scd%c", device_buffer[ i ][ 2 ] );
		if( stat(scdstr, &devicenode) == 0 ) {
			sprintf( device_buffer[ i ], "/dev/scd%c", device_buffer[ i ][ 2 ] );
		}
		else{
			sprintf( device_buffer[ i ], "/dev/sr%c", device_buffer[ i ][ 2 ] );
		}
	}
        else
        {
            strncpy( dev_temp, device_buffer[ i ], sizeof( dev_temp ) );
            sprintf( device_buffer[ i ], "/dev/%s", dev_temp );
        }

        strcpy(pCDROMList, device_buffer[i]);
        pCDROMList += (strlen(pCDROMList) + 1);
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
int CLinuxMediaReader::ExecuteSCSICmd(IUSB_SCSI_PACKET *pRequestPkt,
                    IUSB_SCSI_PACKET *pResponsePkt,
                    unsigned int *pdwDataLen)
#else
int CLinuxMediaReader::ExecuteSCSICmd(IUSB_SCSI_PACKET *pRequestPkt,
                    IUSB_SCSI_PACKET *pResponsePkt,
                    unsigned long *pdwDataLen)
#endif
{
    SCSI_COMMAND_PACKET    *ScsiPkt;
    READ_CAPACITY         *ScsiReadCap;
#if defined (__x86_64__)
    unsigned int        dwDataLen;
    unsigned int        dwSendSize;
    unsigned int         dwSectorNo, dwSectors, dwMaxTOCSize;
#else
    unsigned long        dwDataLen;
    unsigned long        dwSendSize;
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

            case SCSI_START_STOP_UNIT:
				if(mac2blong(ScsiPkt->Lba) == 512)
				{
					sprintf(scdname, "eject %s", scdstr);
					system(scdname);
				}
				break;
            /* Prevention of medium removal is not supported in this version */
            case SCSI_MEDIUM_REMOVAL:
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
int CLinuxMediaReader::ValidateDrive()
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
int CLinuxMediaReader::OpenDrive()
{
    int        nRetval = SUCCESS;

    m_hCDROMDevice = open( m_szDevicePath, O_RDONLY | O_NONBLOCK );
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
int CLinuxMediaReader::CloseDrive()
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
int CLinuxMediaReader::GetSectorSize(unsigned int *dwSectorSize)
#else
int CLinuxMediaReader::GetSectorSize(unsigned long *dwSectorSize)
#endif
{
    int     ioctl_ret;
    int    nRetval = SUCCESS;

    *dwSectorSize = 0;
    ioctl_ret = ioctl( m_hCDROMDevice, CDROM_DISC_STATUS );
    if( ioctl_ret < 0 )
    {
        nRetval = NO_MEDIA;
        return(nRetval);
    }

    switch( ioctl_ret )
    {
        case CDS_DATA_1:
            *dwSectorSize = 2048;
            nRetval = SUCCESS;
            break;
        case CDS_DATA_2:
            *dwSectorSize = 2336;
            nRetval = SUCCESS;
            break;
        case CDS_XA_2_1:
            *dwSectorSize = 2048;
            nRetval = SUCCESS;
            break;
        case CDS_XA_2_2:
            *dwSectorSize = 2328;
            nRetval = SUCCESS;
            break;
        case CDS_AUDIO:
            *dwSectorSize = 2352;
            nRetval = SUCCESS;
            break;
        case CDS_NO_INFO:
            *dwSectorSize = 0;
            nRetval = NO_MEDIA;
            break;
        case CDS_NO_DISC:
            *dwSectorSize = 0;
            nRetval = NO_MEDIA;
            break;
        default:
            *dwSectorSize = 0;
            nRetval = NO_MEDIA;
            break;
    }

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
int CLinuxMediaReader::ReadActualCapacity(unsigned int *dwSectorSize, unsigned int *dwSectorCount )
#else
int CLinuxMediaReader::ReadActualCapacity(unsigned long *dwSectorSize, unsigned long *dwSectorCount )
#endif
{
    FILE         *cdrom_file;
    off_t   position;
    int        nRetval=SUCCESS;

    *dwSectorSize    = 0;
    *dwSectorCount    = 0;

    nRetval = GetSectorSize(dwSectorSize);
    if( nRetval != SUCCESS )
    {
        return nRetval;
    }

    cdrom_file = fopen( m_szDevicePath, "r" );

    /* Use fseeko instead of fseek so that we're using offsets */
    /* This enables correct behaviour with extremely large ISO */
    /* images.                                                 */
    if( fseeko( cdrom_file, 0, SEEK_END ) == -1 )
    {
          fclose( cdrom_file );
        nRetval = NO_MEDIA;
        return nRetval;
    }

    /* Use ftello instead of ftell so that we're using offsets */
    /* This enables correct behaviour with extremely large ISO */
    /* images.                                                 */
    position = ftello( cdrom_file );
    if( position == -1 )
    {
        fclose( cdrom_file );
        nRetval = NO_MEDIA;
        return nRetval;
    }

    fclose( cdrom_file );

    *dwSectorCount = position / *dwSectorSize;

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
int CLinuxMediaReader::TestUnitReady()
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
int CLinuxMediaReader::ReadTOC(SCSI_COMMAND_PACKET* cdb, unsigned int *dwDataLen, char *Buffer)
#else
int CLinuxMediaReader::ReadTOC(SCSI_COMMAND_PACKET* cdb, unsigned long *dwDataLen, char *Buffer)
#endif
{
    struct cdrom_tochdr tochdr;
    struct cdrom_tocentry tocentry;
    CDROM_TOC *TOC;
    int nRetval = SUCCESS, i, j;
    int temp;
    int StartTrack;
#if defined (__x86_64__)
    unsigned int AllocLen;
#else
    unsigned long AllocLen;
#endif
    int requestedTracks;

#define MSF_BIT ( 0x02 )

    *dwDataLen = 0;

    /* Extract starting track and allocated data size */
    StartTrack = cdb->CmdLen.Cmd10.Reserved6;
    AllocLen = mac2bshort(cdb->CmdLen.Cmd10.Length);

    /* Determine the number of tracks requested from the AllocLen */
    requestedTracks = ( AllocLen - 4 ) / 8;

    /* Read the table of contents header */
    if( ioctl( m_hCDROMDevice, CDROMREADTOCHDR, &tochdr ) < 0 )
    {
        nRetval = UNREADABLE_MEDIA;
        return( nRetval );
    }

    TOC = (CDROM_TOC *)Buffer;
    TOC->FirstTrack = tochdr.cdth_trk0;
    TOC->LastTrack = tochdr.cdth_trk1;

    TDBG( "First Track: %d\n", TOC->FirstTrack );
    TDBG( "Last Track: %d\n", TOC->LastTrack );

    /* Do they want everything? */
    if( StartTrack == 0 )
    {
        int totalTracks = TOC->LastTrack - TOC->FirstTrack + 2;

        /* First actual track */
        StartTrack = 1;

        /* If there are fewer tracks than what they've allocated for, */
        /* then just send what is available.                          */
        if( totalTracks < requestedTracks )
            requestedTracks = totalTracks;
    }

    for( i = StartTrack, j = 0; j < requestedTracks; i++, j++ )
    {
        if( i > TOC->LastTrack )
            tocentry.cdte_track = CDROM_LEADOUT;
        else
            tocentry.cdte_track = i;

        if( ( cdb->Lun & MSF_BIT ) == MSF_BIT )
        {
            /* Use MSF format in addressing the CD */
            tocentry.cdte_format = CDROM_MSF;
        }
        else
        {
            /* Use LBA format in addressing the CD */
            tocentry.cdte_format = CDROM_LBA;
        }

        /* Get the TOC entry for this track */
        if( ioctl( m_hCDROMDevice, CDROMREADTOCENTRY, &tocentry ) < 0 )
        {
            nRetval = UNREADABLE_MEDIA;
            return( nRetval );
        }

        TOC->TrackData[ j ].TrackNumber = tocentry.cdte_track;
        TOC->TrackData[ j ].Control = tocentry.cdte_ctrl;
        TOC->TrackData[ j ].Adr = tocentry.cdte_adr;
        if( ( cdb->Lun & MSF_BIT ) == MSF_BIT )
        {
            TOC->TrackData[ j ].Address.MSFAddress[ 0 ] = 0;
            TOC->TrackData[ j ].Address.MSFAddress[ 1 ] = tocentry.cdte_addr.msf.minute;
            TOC->TrackData[ j ].Address.MSFAddress[ 2 ] = tocentry.cdte_addr.msf.second;
            TOC->TrackData[ j ].Address.MSFAddress[ 3 ] = tocentry.cdte_addr.msf.frame;
        }
        else
            TOC->TrackData[ j ].Address.LBAAddress = mac2blong( tocentry.cdte_addr.lba );

        TDBG( "Track Number (from IOCTL): %d\n", TOC->TrackData[ j ].TrackNumber );
        TDBG( "Control: %d\n", TOC->TrackData[ j ].Control );
        TDBG( "Adr: %d\n", TOC->TrackData[ j ].Adr );
        if( ( cdb->Lun & MSF_BIT ) == MSF_BIT )
        {
            TDBG( "Entry address (msf): %02x:%02x:%02x\n", 
                  TOC->TrackData[ j ].Address.MSFAddress[ 1 ],
                  TOC->TrackData[ j ].Address.MSFAddress[ 2 ],
                  TOC->TrackData[ j ].Address.MSFAddress[ 3 ] );
        }
        else
            TDBG( "Entry address (lba): 0x%08lx\n", tocentry.cdte_addr.lba );
    }

    /* Send back only what they asked for */
    TOC->Length = cdb->CmdLen.Cmd10.Length;

    /* Keep our local copy of the length in little endian format */
    *dwDataLen = mac2bshort( TOC->Length );

    nRetval = SUCCESS;
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
int CLinuxMediaReader::ReadCDROM(unsigned int SectorNo, unsigned int Sectors,
                                 unsigned int *dwDataLen, char *Buffer)
#else
int CLinuxMediaReader::ReadCDROM(unsigned long SectorNo, unsigned long Sectors,
                                 unsigned long *dwDataLen, char *Buffer)
#endif                                 
{
#if defined (__x86_64__)
    unsigned int    dwReadSize;
    unsigned int    dwTotalRead;
#else
    unsigned long    dwReadSize;
    unsigned long    dwTotalRead;
#endif	
    unsigned char    *lpSector;
    int    nRetval = SUCCESS;
    int i;
    off_t nSeekSector;

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

    /*Calculate Buffer Requirements*/
    dwReadSize = Sectors * m_dwSectorSize;

    lpSector = (unsigned char *)malloc( dwReadSize );
    if( lpSector == NULL )
    {
        nRetval = INVALID_PARAMS;
        return(nRetval);
    }

    nSeekSector = (off_t)(((off_t)SectorNo) * ((off_t)m_dwSectorSize));
    /* Seek to the Desired Sector */
    if( lseek64( m_hCDROMDevice, nSeekSector, SEEK_SET ) != (off_t)-1 )
    {
        /* Read sectors from the compact disc */
        if( ( dwTotalRead = read( m_hCDROMDevice, lpSector, dwReadSize ) ) <= 0 )
        {
            if( dwTotalRead == 0 )
                nRetval = SECTOR_RANGE_ERROR;
            else
                nRetval = MEDIA_ERROR;

            dwTotalRead = 0;
        }
    }
    else
    {
        nRetval = SECTOR_RANGE_ERROR;
        dwTotalRead = 0;
    }

    /* Copy to local Buffer and Free it */
    if( dwTotalRead > 0 )
        memcpy( Buffer, lpSector, dwTotalRead );

    free( lpSector );

    *dwDataLen = dwTotalRead;

    return( nRetval );
}

