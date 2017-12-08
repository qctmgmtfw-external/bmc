/*****************************************************************
*	File		:	Win9xMediaReader.h							 *
*	Author		:	Rajasekhar J(rajasekharj@amiindia.co.in)	 *
*	Created		:	18-June-2003								 *
*	Description	:	Defines CDROM redirection routines			 *
*					for Win95/98/ME								 *
******************************************************************/

/**
* @file Win9xMediaReader.h
* @author Raja Sekhar J(rajasekharj@amiindia.co.in)
* @date 18-Jun-2003
*
* @brief Defines the routines for CDROM redirection in Win95/98/ME
*
*/

# ifndef __CWIN9XMEDIAREADER_H__
#define __CWIN9XMEDIAREADER_H__

#include "CDROMMediaReader.h"
#include "scsibus.h"
#include "scsidefs.h"

/**
 @brief Contains routines for reading a CDROM Media device in Win95/Win98/WinME
*/
class CWin9xMediaReader: public CCDROMMediaReader {

	private:
		ScsiInterface		*m_ScsiBus;	   /**< Scsi Bus */
		CDDriveSCSIInfoMap	*m_pSelectedDrive; /**< Selected Drive in the list of drives in Scsi Bus */
		unsigned char		m_nFirstTime;	   /**< Used for sending MEDIA CHANGE signal to the card */
		char			m_nDriveLetter;	   /**< Drive letter of the cdrom device to be redirected */

	protected:

		/** @brief Get the ready state of the cdrom device */
		int	TestUnitReady();

#if defined (WIN64)
		/** @brief Read the capacity parameters of the CDROM Device */
		int	ReadActualCapacity(unsigned int* SectorSize,unsigned int* SectorCount);

		/** @brief Read the Table Contents from CDROM Device */
		int	ReadTOC(SCSI_COMMAND_PACKET* cdb,unsigned int *dwDataLen,
						char *Buffer);

		/** @brief Read the Desired sectors from the CDROM Device */
		int	ReadCDROM(unsigned int SectorNo,unsigned int Sectors,
					unsigned int *dwDataLen,
					char *Buffer);
#else
		/** @brief Read the capacity parameters of the CDROM Device */
		int	ReadActualCapacity(unsigned long* SectorSize,unsigned long* SectorCount);

		/** @brief Read the Table Contents from CDROM Device */
		int	ReadTOC(SCSI_COMMAND_PACKET* cdb,unsigned long *dwDataLen,
						char *Buffer);

		/** @brief Read the Desired sectors from the CDROM Device */
		int	ReadCDROM(unsigned long SectorNo,unsigned long Sectors,
					unsigned long *dwDataLen,
					char *Buffer);
#endif

		/** @brief Validate the drive by checking the existence of the drive */
		int	ValidateDrive();

		/** @brief Abort Aspi Command */
		void		DoAspiAbortCommand();

		/** @brief used to lock or unlock the drive */
		int	DoAspiLockUnlockCommand(unsigned char lock);

		/** @brief Used to Load or Eject the Media */
		int	DoAspiLoadUnloadCommand(unsigned char eject);

#if defined (WIN64)
		/** @brief Basic Read Capacity Call */
		int	ReadCapacity(unsigned int* SectorSize,unsigned int* SectorCount);

		/** @brief Execute SCSI command */
		ScbError	ExecuteCmd(ScsiRequestBlock srb, int timeout);

		/** @brief Create aligned buffer */
		PUCHAR	AllocateAlignedBuffer(unsigned int size, unsigned int Align);

		/** @brief Create a SCSI packet and execute it. */
		ScbError	DoCommand(void *cdb, unsigned cdblen,
							void *dbuf, unsigned int dbuflen, int dir,
							int timeout );
#else
		/** @brief Basic Read Capacity Call */
		int	ReadCapacity(unsigned long* SectorSize,unsigned long* SectorCount);

		/** @brief Execute SCSI command */
		ScbError	ExecuteCmd(ScsiRequestBlock srb, long timeout);

		/** @brief Create aligned buffer */
		PUCHAR	AllocateAlignedBuffer(ULONG size, ULONG Align);

		/** @brief Create a SCSI packet and execute it. */
		ScbError	DoCommand(void *cdb, unsigned cdblen,
							void *dbuf, unsigned long dbuflen, int dir,
							long timeout );
#endif
	public:

		/** @brief Constructor */
		CWin9xMediaReader();

		/** @brief Destructor */
		~CWin9xMediaReader();

		/** @brief Lists all the CDROM Devices connected to the system
			Detects all the CDROM devices and returns the string of characters each
			identifying a driveletter of a cdrom device.

		    @param DriveList It is an OUT parameter string of Drive Letters

		    @retval the number of CDROM Devices currently connected to the system.
		*/
		int	ListCDROMDrives(char *DriveList);

		/** @brief Open the CDROM device identified by the Drive Letter in \a pszDevicePath
		    	Opens the device and validates the device whether it exists or not.

		    @param pszDevicePath refers to the drive letter of a cdrom device.
		    @param nByteOrder byte order of the originating system.

		    @retval SUCCESS if the device is successfully opened.
		    @retval DEVICE_ALREADY_OPEN if the device is already in open state.
		    @retval DEVICE_PATH_TOO_LONG if the length of path string exceeds \ref MAX_DEVICE_PATH_SIZE.
		    @retval Error value if any other error occurs \ref SUCCESS "ERROR_VALUES"
		*/
		int	OpenDevice(char *pszDevicePath, enum byteOrder nByteOrder);

		/** @brief Close the CDROM device.
		    	Close the CDROM device and Stops all the transactions.

		    @retval SUCCESS
		*/
		int	CloseDevice();

		/** @brief Executes the Command by redirecting it to the CDROM Device.
		    	The request command is redirected to the CDROM device and the result is
			stored in Response Packet. It also returns the datalength if any data added to
			the response packet.

		    @param pRequestPkt refers to the Request command packet that is to be sent to the CDROM Device.
		    @param pResponsePkt refers the result of the command from CDROM Device.
		    @param dwDataLen	refers to the length of data if any attached to the response packet.

		    @retval SUCCESS on success
		    @retval Error value if any other error occurs \ref SUCCESS "ERROR_VALUES"
		
		    @param pResponsePkt refers the result of the command from CDROM Device. */
#if defined (WIN64)
		int	ExecuteSCSICmd(IUSB_SCSI_PACKET *pRequestPkt,
					IUSB_SCSI_PACKET *pResponsePkt,
					unsigned int *dwDataLen);
#else
		int	ExecuteSCSICmd(IUSB_SCSI_PACKET *pRequestPkt,
					IUSB_SCSI_PACKET *pResponsePkt,
					unsigned long *dwDataLen);
#endif
};

#endif
