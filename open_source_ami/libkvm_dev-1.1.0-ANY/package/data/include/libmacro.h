/*****************************************************************
*	File		:	cdrom.h										 *
*	Author		:	Rajasekhar J(rajasekharj@amiindia.co.in)	 *
*	Created		:	18-June-2003								 *
*	Description	:	Definitions used by cdrom redirection		 *
*						routines								 *
******************************************************************/

/**
* @file cdrom.h
* @author Raja Sekhar J(rajasekharj@amiindia.co.in)
* @date 18-June-2003
*
* @brief Definitions used for CDROM redirection.
*/
#ifndef __LIBCDROM_H__
#define __LIBCDROM_H__

#define CDROM_VERSION	"1.8"

/**
* @brief Byte Order of the originating system.
*/
enum byteOrder {
	bigendian, littleendian
};

/**
* @brief Maximum limit for full path size of an CDROM Device
*/
#define	MAX_DEVICE_PATH_SIZE	256

#define  mac2bshort(x) 	(uint16)((((uint16)(x) >> 8) | ((uint16)(x) << 8)))
#define  mac2blong(x)	( ((x) >> 24) | ((x) << 24) | (((x) & 0x00ff0000) >> 8) | (((x) & 0x0000ff00) << 8))

/**
* @brief contains the capacity paramters of a cdrom image.
*/
typedef struct tag_READ_CAPACITY {
#if defined (__x86_64__) || defined (WIN64)
	unsigned int TotalBlocks;
	unsigned int BlockSize;
#else
	unsigned long TotalBlocks;
	unsigned long BlockSize;
#endif
}READ_CAPACITY;


/** @name ERROR_VALUES */
/** @{ */
/** Operation Successfull */
#define	SUCCESS				0

/** Requested Sector is out of range */
#define SECTOR_RANGE_ERROR	-1

/** Wrong media */
#define	WRONG_MEDIA			-2

/** There was medium change since last operation */
#define	MEDIUM_CHANGE		-3

/** Error occured while accessing media */
#define	MEDIA_ERROR			-4

/** No Media detected to perform an operation */
#define NO_MEDIA			-5

/** Invalid request parameters */
#define INVALID_PARAMS		-6

/** Unable to read media */
#define UNREADABLE_MEDIA	-7

/** Unable to eject media */
#define REMOVAL_PREVENTED	-8

/** Command not supported in this version */
#define UNSUPPORTED_COMMAND	-9



#define	DEVICE_PATH_TOO_LONG	-10
#define	DEVICE_ERROR			-11
#define	DEVICE_ALREADY_OPEN		-12


#define MEDIUM_GETTING_READY -14
/** Meida is already opened and is in use by some other program */
#define MEDIA_IN_USE		-15

/** @} */
#endif
