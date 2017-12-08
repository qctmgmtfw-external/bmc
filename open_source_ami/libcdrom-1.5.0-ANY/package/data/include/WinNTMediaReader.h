/*****************************************************************
*	File		:	WinNTMediaReader.h							 *
*	Author		:	Rajasekhar J(rajasekharj@amiindia.co.in)	 *
*	Created		:	18-June-2003								 *
*	Description	:	Defines CDROM redirection routines			 *
*					for WinNT/2K/XP								 *
******************************************************************/

/**
* @file WinNTMediaReader.h
* @author Raja Sekhar J(rajasekharj@amiindia.co.in)
* @date 18-Jun-2003
*
* @brief Defines the routines for CDROM redirection in WinNT/2K/XP
*
*/

# ifndef __CWINNTMEDIAREADER_H__
#define __CWINNTMEDIAREADER_H__

#include "CDROMMediaReader.h"
#include <windows.h>
#define MAX_CHUNK (0x20)

/**
 @brief Contains routines for reading a CDROM Media device in WinNT/2K/XP
*/
class CWinNTMediaReader: public CCDROMMediaReader {

	private:
		HANDLE				m_hCDROMDevice; /**< Handle to CDROM Device */
		char				m_nDriveLetter; /**< DriveLetter of the redirected drive */
		unsigned char		m_nFirstTime;   /**< Used for sending MEDIA CHANGE signal to the card */

	protected:
		/** @brief Get the ready state of the cdrom device */
		int			TestUnitReady();

#if defined (WIN64)
		/** @brief Read the capacity parameters of the CDROM Device */
		int			ReadActualCapacity(unsigned int* SectorSize,unsigned int* SectorCount);

		/** @brief Read the Table Contents from CDROM Device */
		int			ReadTOC(SCSI_COMMAND_PACKET* cdb,unsigned int *dwDataLen,
								char *Buffer);

		/** @brief Read the Desired sectors from the CDROM Device */
		int			ReadCDROM(unsigned int SectorNo,unsigned int Sectors,
								unsigned int *dwDataLen,
								char *Buffer);
#else
		/** @brief Read the capacity parameters of the CDROM Device */
		int			ReadActualCapacity(unsigned long* SectorSize,unsigned long* SectorCount);

		/** @brief Read the Table Contents from CDROM Device */
		int			ReadTOC(SCSI_COMMAND_PACKET* cdb,unsigned long *dwDataLen,
								char *Buffer);

		/** @brief Read the Desired sectors from the CDROM Device */
		int			ReadCDROM(unsigned long SectorNo,unsigned long Sectors,
								unsigned long *dwDataLen,
								char *Buffer);
#endif
		int FindAlignment();

		/** @brief Validate the drive by checking the existence of the drive */
		int			ValidateDrive();

#if defined (WIN64)
		/** @brief Basic Read Capacity Call */
		int			ReadCapacity(unsigned int* SectorSize,unsigned int* SectorCount);

		/** @brief Create aligned buffer */
		PUCHAR			AllocateAlignedBuffer(unsigned int size);
		/** @brief Create a SCSI command packet and execute it */
		unsigned int		SendScsiCmd(void* cdb_buf,unsigned char cdb_len,
							  void* data_buf,unsigned int data_len,
							  int direction,
							  void* sense_buf,unsigned slen,
							  int timeout,
							  unsigned int* ActualDataLen
							  );
#else
		/** @brief Basic Read Capacity Call */
		int			ReadCapacity(unsigned long* SectorSize,unsigned long* SectorCount);

		/** @brief Create aligned buffer */
		PUCHAR			AllocateAlignedBuffer(ULONG size);
		/** @brief Create a SCSI command packet and execute it */
		unsigned long		SendScsiCmd(void* cdb_buf,unsigned char cdb_len,
							  void* data_buf,unsigned long data_len,
							  int direction,
							  void* sense_buf,unsigned slen,
							  long timeout,
							  unsigned long* ActualDataLen
							  );
#endif

		/** @brief Open the CDROM Drive */
		int			OpenDrive();

		/** @brief Close the CDROM device */
		int			CloseDrive();

	public:

		/** @brief Constructor */
		CWinNTMediaReader();
		/** @brief Desctructor */
		~CWinNTMediaReader();

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
		*/
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
