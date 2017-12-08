// USBDisk.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
//#include "resource.h"

#include <stdio.h>

#include <dbt.h>						// For DeviceChange.
#include <winioctl.h>					// For DeviceIOCtl.

#define MAX_LOADSTRING 100

// Add fro USB Task.

// USB letters' container
//TCHAR   szMoveDiskName[HD_BUFFER_SIZE];
TCHAR   Cd_Drive[32][32];

//TCHAR   szMoveFixedDiskName[33];
TCHAR	CD_szDrvName[33];
//BOOL	bInitUSBs;
// Drive type names
#define DRVUNKNOWN		0
#define DRVFIXED		1
#define DRVREMOTE		2
#define DRVRAM			3
#define DRVCD			4
#define DRVREMOVE		5

// IOCTL control code
#define IOCTL_STORAGE_QUERY_PROPERTY   CTL_CODE(IOCTL_STORAGE_BASE, 0x0500, METHOD_BUFFERED, FILE_ANY_ACCESS)

//// The following structures all can find at MSDN.
// enumeration type specifies the various types of storage buses
/*typedef enum _STORAGE_BUS_TYPE {
    BusTypeUnknown = 0x00,
    BusTypeScsi,
    BusTypeAtapi,
    BusTypeAta,
    BusType1394,
    BusTypeSsa,
    BusTypeFibre,
    BusTypeUsb,
    BusTypeRAID,
    BusTypeMaxReserved = 0x7F
} STORAGE_BUS_TYPE, *PSTORAGE_BUS_TYPE;*/

// retrieve the storage device descriptor data for a device. 
typedef struct _STORAGE_DEVICE_DESCRIPTOR {
  ULONG  Version;
  ULONG  Size;
  UCHAR  DeviceType;
  UCHAR  DeviceTypeModifier;
  BOOLEAN  RemovableMedia;
  BOOLEAN  CommandQueueing;
  ULONG  VendorIdOffset;
  ULONG  ProductIdOffset;
  ULONG  ProductRevisionOffset;
  ULONG  SerialNumberOffset;
  STORAGE_BUS_TYPE  BusType;
  ULONG  RawPropertiesLength;
  UCHAR  RawDeviceProperties[1];

} STORAGE_DEVICE_DESCRIPTOR, *PSTORAGE_DEVICE_DESCRIPTOR;

// retrieve the properties of a storage device or adapter. 
typedef enum _STORAGE_QUERY_TYPE {
  PropertyStandardQuery = 0,
  PropertyExistsQuery,
  PropertyMaskQuery,
  PropertyQueryMaxDefined

} STORAGE_QUERY_TYPE, *PSTORAGE_QUERY_TYPE;

// retrieve the properties of a storage device or adapter. 
typedef enum _STORAGE_PROPERTY_ID {
  StorageDeviceProperty = 0,
  StorageAdapterProperty,
  StorageDeviceIdProperty

} STORAGE_PROPERTY_ID, *PSTORAGE_PROPERTY_ID;

// retrieve the properties of a storage device or adapter. 
typedef struct _STORAGE_PROPERTY_QUERY {
  STORAGE_PROPERTY_ID  PropertyId;
  STORAGE_QUERY_TYPE  QueryType;
  UCHAR  AdditionalParameters[1];

} STORAGE_PROPERTY_QUERY, *PSTORAGE_PROPERTY_QUERY;

///****************************************************************************
//*
//*    FUNCTION: GetDisksProperty(HANDLE hDevice, PSTORAGE_DEVICE_DESCRIPTOR pDevDesc)
//*
//*    PURPOSE:  get the info of specified device
//*
//****************************************************************************/
//BOOL GetDisksProperty(HANDLE hDevice, PSTORAGE_DEVICE_DESCRIPTOR pDevDesc)
//{
//	STORAGE_PROPERTY_QUERY	Query;	// input param for query
//	DWORD dwOutBytes;				// IOCTL output length
//	BOOL bResult;					// IOCTL return val
//
//	// specify the query type
//	Query.PropertyId = StorageDeviceProperty;
//	Query.QueryType = PropertyStandardQuery;
//
//	// Query using IOCTL_STORAGE_QUERY_PROPERTY 
//	bResult = ::DeviceIoControl(hDevice,			// device handle
//			IOCTL_STORAGE_QUERY_PROPERTY,			// info of device property
//			&Query, sizeof(STORAGE_PROPERTY_QUERY),	// input data buffer
//			pDevDesc, pDevDesc->Size,				// output data buffer
//			&dwOutBytes,							// out's length
//			(LPOVERLAPPED)NULL);					
//
//	return bResult;
//}
//
///****************************************************************************
//*
//*    FUNCTION: chFirstDriverFrameMask(ULONG unitmask)
//*
//*    PURPOSE:  get the logic name of driver
//*
//****************************************************************************/
//char chFirstDriveFromMask (ULONG unitmask)
//{
//
//      char i;
//      for (i = 0; i < 26; ++i)  
//      {
//           if (unitmask & 0x1) 
//				break;
//            unitmask = unitmask >> 1;
//      }
//    return (i + 'A');
//}

/****************************************************************************
*
*    FUNCTION: ReInitUSB_Disk_Letter()
*
*    PURPOSE:  get the usb disks, and filling the 'szMoveDiskName' with them
*
****************************************************************************/
int ReInitUSB_Disk_Letter(char *Drivelist)
{
	int k = 0;
	DWORD			MaxDriveSet, CurDriveSet;
	DWORD			 drivetype;	
	int drive,index=0;	
	
	int l = 0;
	int len=0;
	
	memset(Cd_Drive,0,sizeof(Cd_Drive));
	
	k = 0;		
//	 Get available drives we can monitor
	MaxDriveSet = CurDriveSet = 0;

	MaxDriveSet = GetLogicalDrives();
	
	len=0;

	CurDriveSet = MaxDriveSet;
	for ( drive = 0; drive < 32; ++drive )  
	{
		if ( MaxDriveSet & (1 << drive) )  
		{
			DWORD temp = 1<<drive;
			int error_ret = 0;
			_stprintf( CD_szDrvName, _T("%c:\\"), 'A'+drive );
			
			int Drivetype = GetDriveType( CD_szDrvName );
			
			switch ( GetDriveType( CD_szDrvName ) )  
			{
				case 0:					// The drive type cannot be determined.
				case 1:					// The root directory does not exist.
					drivetype = DRVUNKNOWN;
					break;
				case DRIVE_CDROM:		// The drive is a CD-ROM drive.	

					strncpy(Drivelist,CD_szDrvName,1);
			
					if(*Drivelist != '\0')
					{	
						Drivelist+=strlen(Drivelist);
						len++;
					}
					break;
				case DRIVE_REMOVABLE:
				case DRIVE_FIXED:		// The disk cannot be removed from the drive.					
					break;
				case DRIVE_REMOTE:		// The drive is a remote (network) drive.
					break;
				case DRIVE_RAMDISK:		// The drive is a RAM disk.
					drivetype = DRVRAM;
					break;
			}
		
		}
	}
		
	return len;
}
