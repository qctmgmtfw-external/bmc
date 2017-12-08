/*****************************************************************
*	File		:	CDROMReader.h								 *
*	Author		:	Rajasekhar J(rajasekharj@amiindia.co.in)	 *
*	Created		:	18-June-2003								 *
*	Description	:	Defines CDROM redirection routines			 *
******************************************************************/

/** @mainpage Documentation for CDROM Library
*
* @author Rajasekhar (rajasekharj@amiindia.co.in)
* @date   18-June-2003
*
* @brief Libraries for CDROM Redirection.
* CDROM redirection involves presenting a virtual CDROM device to 
* the managed server. This virtual CDROM may actually be serviced 
* from various sources to accomplish CD redirection.
*
* <br>The following sources are considered for now <br><br>
*		<b>1)</b>	Actual Physical CD media in Remote Console machine<br>
*		<b>2)</b>	CDROM image file in Remote Console Machine<br>
*		<b>3)</b>	CDROM Image file in G3 cards network accessible via CIFS/NFS share<br>
*/

/**
* @file CDROMReader.h
* @author Raja Sekhar J(rajasekharj@amiindia.co.in)
* @date 18-Jun-2003
*
* @brief Implements routines for CDROM Redirection.
*/

# ifndef __CCDROMREADER_H__
#define __CCDROMREADER_H__

#include <string>
#include "coreTypes.h"
#include "scsi.h"
#include "iusb.h"
#include "libmacro.h"
#include "imagereader.h"

using std::string;
/**
 @brief Contains routines for reading a CDROM Media/CDROM Image
*/
class CCDROMReader {

	protected:
		/** @brief Byte order of originating system */
		enum byteOrder		m_nByteOrder;

	public:

		/**@brief Opens CDROM device for reading */
		virtual int		OpenDevice(char *pszDevicePath, enum byteOrder nByteOrder)=0;
		
		/**@brief Closes CDROM device */
		virtual int		CloseDevice()=0;

#if defined (__x86_64__) || defined (WIN64)
		/**@brief Execute an operation on CDROM Device. */
		virtual	int		ExecuteSCSICmd(IUSB_SCSI_PACKET *pRequestPkt, 
											IUSB_SCSI_PACKET *pResponsePkt, 
											unsigned int *dwDataLen)=0;
#else
		/**@brief Execute an operation on CDROM Device. */
		virtual	int		ExecuteSCSICmd(IUSB_SCSI_PACKET *pRequestPkt, 
											IUSB_SCSI_PACKET *pResponsePkt, 
											unsigned long *dwDataLen)=0;
#endif
		
		/**@brief List all CDROM devices connected to the system */
		virtual int		ListCDROMDrives(char *DriveList)=0;

		virtual string GetVersion()=0;

};

#endif

