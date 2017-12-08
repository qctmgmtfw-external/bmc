/*****************************************************************
*	File		:	WinNTMediaReader.h							 *
*	Author		:	Rajasekhar J(rajasekharj@amiindia.co.in)	 *
*	Created		:	18-July-2003								 *
*	Description	:	Defines Harddisk redirection routines			 *
*					for WinNT/2K/XP								 *
******************************************************************/

/**
* @file WinNTMediaReader.h
* @author Raja Sekhar J(rajasekharj@amiindia.co.in)
* @date 18-July-2003
*
* @brief Defines the routines for Harddisk redirection in WinNT/2K/XP
*
*/

#ifndef __WINNT_HARDDISK_READER_H__
#define __WINNT_HARDDISK_READER_H__

#include "HarddiskMediaReader.h"
#include <windows.h>

/**
 @brief Contains routines for reading a Harddisk Media device in Win95/Win98/WinME
*/
class CWinNTHarddiskReader : public CHarddiskMediaReader {

	private:
		HANDLE	m_hHarddiskDevice; /**< Handle to Harddisk Device */
		char	m_nDriveLetter[16]; /**< DriveLetter of the redirected drive */
		char	m_nDriveName[16]; /** < Redirected physical drive logical name */
		HANDLE  m_hLogicalDrive[20];	/** <hadnle for the each logical drive to dismount */
		boolean m_writelocksucess; /**For writing lock success or not */
	protected:
		/** @brief Test the ready state of the Harddisk Device */
		int		TestUnitReady();

#if defined (WIN64)
		/** @brief Read the capacity of Harddisk Device */
		int		ReadCapacity(unsigned int* SectorSize,unsigned int* SectorCount);

		/** @brief Read Desired sectors from a Harddisk Device */
		int		ReadHarddisk(unsigned int SectorNo,unsigned int Sectors,
									unsigned int *dwDataLen,
									char *Buffer);

		/** @brief Write Desired sectors to a Harddisk Device */
		int		WriteHarddisk(unsigned int SectorNo,unsigned int Sectors,
									char *Buffer);
#else
		/** @brief Read the capacity of Harddisk Device */
		int		ReadCapacity(DWORD* SectorSize,DWORD* SectorCount);

		/** @brief Read Desired sectors from a Harddisk Device */
		int		ReadHarddisk(DWORD SectorNo,DWORD Sectors,
									unsigned long *dwDataLen,
									char *Buffer);

		/** @brief Write Desired sectors to a Harddisk Device */
		int		WriteHarddisk(DWORD SectorNo,DWORD Sectors,
									char *Buffer);
#endif
		
		int		ValidateDrive();
		int		OpenDrive();
		int		CloseDrive();
	public:

		/** @brief Constructor */
		CWinNTHarddiskReader();

		/** @brief Destructor */
		~CWinNTHarddiskReader();

		/** @brief Open the Harddisk device identified by the Drive Letter in \a pszDevicePath
		    	Opens the device and validates the device whether it exists or not.

		    @param pszDevicePath refers to the drive letter of a Harddisk device.
		    @param nByteOrder byte order of the originating system.

		    @retval SUCCESS if the device is successfully opened.
		    @retval DEVICE_ALREADY_OPEN if the device is already in open state.
		    @retval DEVICE_PATH_TOO_LONG if the length of path string exceeds \ref MAX_DEVICE_PATH_SIZE.
		    @retval Error value if any other error occurs \ref SUCCESS "ERROR_VALUES"
		*/
		int		OpenDevice(char *pszDevicePath, enum byteOrder nByteOrder);

		/** @brief Close the Harddisk device.
		    	Close the Harddisk device and Stops all the transactions.

		    @retval SUCCESS
		*/
		int		CloseDevice();

		/** @brief Executes the Command by redirecting it to the Harddisk Device.
		    	The request command is redirected to the Harddisk device and the result is
			stored in Response Packet. It also returns the datalength if any data added to
			the response packet.

		    @param pRequestPkt refers to the Request command packet that is to be sent to the Harddisk Device.
		    @param pResponsePkt refers the result of the command from Harddisk Device.
		    @param dwDataLen	refers to the length of data if any attached to the response packet.

		    @retval SUCCESS on success
		    @retval Error value if any other error occurs \ref SUCCESS "ERROR_VALUES"
		*/
#if defined (WIN64)
		int		ExecuteSCSICmd(IUSB_SCSI_PACKET *pRequestPkt,
											IUSB_SCSI_PACKET *pResponsePkt,
											unsigned int *dwDataLen);
#else
		int		ExecuteSCSICmd(IUSB_SCSI_PACKET *pRequestPkt,
											IUSB_SCSI_PACKET *pResponsePkt,
											unsigned long *dwDataLen);
#endif

		/** @brief Lists all the Harddisk Devices connected to the system
			Detects all the Harddisk devices and returns the string of characters each
			identifying a driveletter of a Harddisk device.

		    @param DriveList It is an OUT parameter string of Drive Letters

		    @retval the number of Harddisk Devices currently connected to the system.
		*/
		int		ListHardDrives(char *DriveList,int type);
		int		WriteProtected();
};

#endif

