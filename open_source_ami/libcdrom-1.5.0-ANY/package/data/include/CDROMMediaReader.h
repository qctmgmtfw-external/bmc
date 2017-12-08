/*****************************************************************
*	File		:	CDROMMediaReader.h							 *
*	Author		:	Rajasekhar J(rajasekharj@amiindia.co.in)	 *
*	Created		:	18-June-2003								 *
*	Description	:	Defines routines for redirecting a			 *
*						CDROM Media.							 *
******************************************************************/

/**
* @file CDROMMediaReader.h
* @author Raja Sekhar J(rajasekharj@amiindia.co.in)
* @date 18-Jun-2003
*
* @brief Defines routines for redirecting a CDROM media.
*/

# ifndef __CCDROMMEDIAREADER_H__
#define __CCDROMMEDIAREADER_H__

#include "coreTypes.h"
#include "scsi.h"
#include "iusb.h"
#include "CDROMReader.h"

#define MAX_READ_SECTORS   	64
#define MAX_READ_RETRIES	3

/**
 @brief Contains routines for reading a CDROM Media device
*/
class CCDROMMediaReader:public CCDROMReader {

	protected:
		
		bool			m_bIsDeviceOpen; /**< CDROM Device staus Opened/Closed*/

#if defined(__x86_64__) || defined(WIN64)
		unsigned int	m_dwSectorSize;  /**< Sector size of the CDROM */
		unsigned int	m_dwSectorCount; /**< Total sectors in the CDROM */
#else
		unsigned long	m_dwSectorSize;  /**< Sector size of the CDROM */
		unsigned long	m_dwSectorCount; /**< Total sectors in the CDROM */
#endif
	
	protected:
		/** @brief Test the ready state of the CDROM Device */
		virtual int		TestUnitReady()=0;

#if defined (__x86_64__) || defined (WIN64)
		/** @brief Read the capacity of CDROM Device */
		virtual int		ReadActualCapacity(unsigned int* SectorSize,unsigned int* SectorCount)=0;
		
		/** @brief Read Table of Contents */
		virtual int		ReadTOC(SCSI_COMMAND_PACKET* cdb,
									unsigned int *dwDataLen,
									char *Buffer)=0;
		
		/** @brief Read Desired sectors from a CDROM Device */
		virtual int		ReadCDROM(unsigned int SectorNo,unsigned int Sectors, 
									unsigned int *dwDataLen, 
									char *Buffer)=0;
#else
		/** @brief Read the capacity of CDROM Device */
		virtual int		ReadActualCapacity(DWORD* SectorSize,DWORD* SectorCount)=0;
		
		/** @brief Read Table of Contents */
		virtual int		ReadTOC(SCSI_COMMAND_PACKET* cdb,
									unsigned long *dwDataLen,
									char *Buffer)=0;
		
		/** @brief Read Desired sectors from a CDROM Device */
		virtual int		ReadCDROM(DWORD SectorNo,DWORD Sectors, 
									unsigned long *dwDataLen, 
									char *Buffer)=0;
#endif
		
		/** @brief Sets the error flags in Response Packet.
			It takes the errorvalue \a nError  as an argument 
			and sets the error flags based on that error value.

			@param nError	Error Value \ref SUCCESS "ERROR_VALUES"
			@param pResponsePkt Response Packet in which the flags are set.
		*/
		virtual void 	SetErrorStatus(int nError, IUSB_SCSI_PACKET *pResponsePkt);

		virtual std::string GetVersion();
};

#endif

