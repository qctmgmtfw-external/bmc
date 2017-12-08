/*****************************************************************
*	File		:	LinuxMediaReader.h
*	Author		:	Rajasekhar J(rajasekharj@amiindia.co.in)
*	Created		:	18-June-2003
*	Description	:	Defines CDROM redirection routines
*					for Linux
******************************************************************/

/**
* @file LinuxMediaReader.h
* @author Raja Sekhar J(rajasekharj@amiindia.co.in)
* @date 18-Jun-2003
*
* @brief Defines the routines for CDROM redirection for Linux
*
*/

# ifndef __CLINUXMEDIAREADER_H__
#define __CLINUXMEDIAREADER_H__

#include "CDROMMediaReader.h"

#define MAX_TRACKS 100

typedef struct _TRACK_DATA
{
    uint8 Reserved;
    uint8 Control : 4;
    uint8 Adr : 4;
    uint8 TrackNumber;
    uint8 Reserved1;
	union
	{
		uint32 LBAAddress;
		uint8 MSFAddress[ 4 ];
	} Address;
} TRACK_DATA;

typedef struct _CDROM_TOC
{
    uint16 Length;
    uint8 FirstTrack;
    uint8 LastTrack;
    TRACK_DATA TrackData[ MAX_TRACKS ];
} CDROM_TOC;


/**
 @brief Contains routines for reading a CDROM Media device in Linux
*/
class CLinuxMediaReader: public CCDROMMediaReader {

	private:
		int			m_hCDROMDevice; /**< Handle to CDROM Device */
		char			m_szDevicePath[MAX_DEVICE_PATH_SIZE]; /**< redirected cdrom device */

	protected:
		/** @brief Get the ready state of the cdrom device */
		int			TestUnitReady();

#if defined (__x86_64__)
		/** @brief Read the capacity parameters of the CDROM Device */
		int			ReadActualCapacity(unsigned int* SectorSize,unsigned int* SectorCount);

		/** @brief Read the Table Contents from CDROM Device */
		int			ReadTOC(SCSI_COMMAND_PACKET* cdb,unsigned int *dwDataLen,
							char *Buffer);

		/** @brief Read the Desired sectors from the CDROM Device */
		int			ReadCDROM(unsigned int SectorNo, unsigned int Sectors,
								unsigned int *dwDataLen,
								char *Buffer);
#else
		/** @brief Read the capacity parameters of the CDROM Device */
		int			ReadActualCapacity(unsigned long* SectorSize,unsigned long* SectorCount);

		/** @brief Read the Table Contents from CDROM Device */
		int			ReadTOC(SCSI_COMMAND_PACKET* cdb,unsigned long *dwDataLen,
							char *Buffer);

		/** @brief Read the Desired sectors from the CDROM Device */
		int			ReadCDROM(unsigned long SectorNo, unsigned long Sectors,
								unsigned long *dwDataLen,
								char *Buffer);
#endif

		/** @brief Validate the drive by checking the existence of the drive */
		int			ValidateDrive();
#if defined (__x86_64__)
		/** @brief Gets the sector size of the cdrom media */
		int			GetSectorSize(unsigned int *dwSectorSize);
#else
		/** @brief Gets the sector size of the cdrom media */
		int			GetSectorSize(unsigned long *dwSectorSize);
#endif

		/** @brief Open the CDROM Drive */
		int			OpenDrive();

		/** @brief Close the CDROM device */
		int			CloseDrive();

	public:
		/** @brief Constructor */
		CLinuxMediaReader();
		/** @brief Desctructor */
		virtual ~CLinuxMediaReader();

		/** @brief Lists all the CDROM Devices connected to the system
			Detects all the CDROM devices and returns array of strings
			each identifying a cdrom device.

		    @param pCDROMList It is an OUT parameter string of Drive Letters

		    @retval the number of CDROM Devices currently connected to the system.
		*/
		int 			ListCDROMDrives(char *pCDROMList);

		/** @brief Open the CDROM device identified by the Drive Letter in \a pszDevicePath
		    	Opens the device and validates the device whether it exists or not.

		    @param pszDevicePath refers to the drive letter of a cdrom device.
		    @param nByteOrder byte order of the originating system.

		    @retval SUCCESS if the device is successfully opened.
		    @retval DEVICE_ALREADY_OPEN if the device is already in open state.
		    @retval DEVICE_PATH_TOO_LONG if the length of path string exceeds \ref MAX_DEVICE_PATH_SIZE.
		    @retval Error value if any other error occurs \ref SUCCESS "ERROR_VALUES"
		*/
		int 			OpenDevice(char *pszDevicePath, enum byteOrder nByteOrder);

		/** @brief Close the CDROM device.
		    	Close the CDROM device and Stops all the transactions.

		    @retval SUCCESS
		*/
		int 			CloseDevice();

		/** @brief Executes the Command by redirecting it to the CDROM Device.
		    	The request command is redirected to the CDROM device and the result is
			stored in Response Packet. It also returns the datalength if any data added to
			the response packet.

		    @param pRequestPkt refers to the Request command packet that is to be sent to the CDROM Device.
		    @param pResponsePkt refers the result of the command from CDROM Device.
		    @param dwDataLen	refers to the length of data if any attached to the response packet.

		    @retval SUCCESS on success
		    @retval Error value if any other error occurs \ref SUCCESS "ERROR_VALUES"
		*/
#if defined (__x86_64__)
		int 			ExecuteSCSICmd(IUSB_SCSI_PACKET *pRequestPkt,
							IUSB_SCSI_PACKET *pResponsePkt,
							unsigned int *dwDataLen);
#else
		int 			ExecuteSCSICmd(IUSB_SCSI_PACKET *pRequestPkt,
							IUSB_SCSI_PACKET *pResponsePkt,
							unsigned long *dwDataLen);
#endif
};

#endif

