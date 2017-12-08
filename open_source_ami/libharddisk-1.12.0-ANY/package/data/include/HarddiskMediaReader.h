/*****************************************************************
*	File		:	HarddiskMediaReader.h							 *
*	Author		:	Rajasekhar J(rajasekharj@amiindia.co.in)	 *
*	Created		:	18-July-2003								 *
*	Description	:	Defines routines for redirecting a			 *
*						Harddisk Media.							 *
******************************************************************/

/**
* @file HarddiskMediaReader.h
* @author Raja Sekhar J(rajasekharj@amiindia.co.in)
* @date 18-July-2003
*
* @brief Defines routines for redirecting a Harddisk media.
*/

#ifndef __HARDDISK_MEDIA_READER_H__
#define __HARDDISK_MEDIA_READER_H__

#include <string>
#include "HarddiskReader.h"

#define HD_BUFFER_SIZE  (32*32)

/**
 @brief Contains routines for reading a Harddisk Media device
*/
class CHarddiskMediaReader:public CHarddiskReader {

	protected:

		bool			m_bIsDeviceOpen; /**< Harddisk Device staus Opened/Closed*/
#if defined (__x86_64__) || defined (WIN64)
		unsigned int	m_dwSectorSize;  /**< Sector size of the Harddisk */
		unsigned int	m_dwSectorCount; /**< Total sectors in the Harddisk */
#else
		unsigned long	m_dwSectorSize;  /**< Sector size of the Harddisk */
		unsigned long	m_dwSectorCount; /**< Total sectors in the Harddisk */
#endif
		bool			m_bFirstTime;

	protected:
		/** @brief Test the ready state of the Harddisk Device */
		virtual int		TestUnitReady()=0;

#if defined (__x86_64__) || defined (WIN64)
		/** @brief Read the capacity of Harddisk Device */
		virtual int		ReadCapacity(unsigned int* SectorSize,unsigned int* SectorCount)=0;

		/** @brief Read Desired sectors from a Harddisk Device */
		virtual int		ReadHarddisk(unsigned int SectorNo,unsigned int Sectors,
									unsigned int *dwDataLen,
									char *Buffer)=0;

		/** @brief Write Desired sectors to a Harddisk Device */
		virtual int		WriteHarddisk(unsigned int SectorNo,unsigned int Sectors,
									char *Buffer)=0;
#else
		/** @brief Read the capacity of Harddisk Device */
		virtual int		ReadCapacity(DWORD* SectorSize,DWORD* SectorCount)=0;

		/** @brief Read Desired sectors from a Harddisk Device */
		virtual int		ReadHarddisk(DWORD SectorNo,DWORD Sectors,
									unsigned long *dwDataLen,
									char *Buffer)=0;

		/** @brief Write Desired sectors to a Harddisk Device */
		virtual int		WriteHarddisk(DWORD SectorNo,DWORD Sectors,
									char *Buffer)=0;
#endif		

		/** @brief Sets the error flags in Response Packet.
			It takes the errorvalue \a nError  as an argument
			and sets the error flags based on that error value.

			@param nError	Error Value \ref SUCCESS "ERROR_VALUES"
			@param pResponsePkt Response Packet in which the flags are set.
		*/
		virtual void 	SetErrorStatus(int nError, IUSB_SCSI_PACKET *pResponsePkt);

		/** @brief Generates a floppy image and stores in a disk file. 
			
			@param pszOutputfile	Path of an image file to be created.
			
			@retval SUCCESS - on success
			@retval nError	on Error \ref ERROR "ERROR_VALUES"
		*/
		int		CreateHarddiskImage(char *pszOutputfile);
		
		/** @brief Reads a floppy image file and loads the data
			into the floppy disk media.
			
			@param pszOutputfile Path of an image file to be copied.
			
			@retval SUCCESS - on success
			@retval nError	on Error \ref ERROR "ERROR_VALUES"
		*/
		int		LoadFromImage(char *pszImageFile);

		std::string GetVersion();
};

#endif

