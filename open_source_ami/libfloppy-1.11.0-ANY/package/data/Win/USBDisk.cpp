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
TCHAR   FD_Index_Drive[32][32];
TCHAR   FD_IDE_Drive[32][32];


//TCHAR   szMoveFixedDiskName[33];
TCHAR	FD_szDrvName[33];
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

/****************************************************************************
*
*    FUNCTION: GetFloppyDisksProperty(HANDLE hDevice, PSTORAGE_DEVICE_DESCRIPTOR pDevDesc)
*
*    PURPOSE:  get the info of specified device
*
****************************************************************************/
BOOL GetFloppyDisksProperty(HANDLE hDevice, PSTORAGE_DEVICE_DESCRIPTOR pDevDesc)
{
	STORAGE_PROPERTY_QUERY	Query;	// input param for query
	DWORD dwOutBytes;				// IOCTL output length
	BOOL bResult;					// IOCTL return val

	// specify the query type
	Query.PropertyId = StorageDeviceProperty;
	Query.QueryType = PropertyStandardQuery;

	// Query using IOCTL_STORAGE_QUERY_PROPERTY 
	bResult = ::DeviceIoControl(hDevice,			// device handle
			IOCTL_STORAGE_QUERY_PROPERTY,			// info of device property
			&Query, sizeof(STORAGE_PROPERTY_QUERY),	// input data buffer
			pDevDesc, pDevDesc->Size,				// output data buffer
			&dwOutBytes,							// out's length
			(LPOVERLAPPED)NULL);					
	int error = GetLastError();
	return error;
}

/****************************************************************************
*
*    FUNCTION: chFirstDriverFrameMask(ULONG unitmask)
*
*    PURPOSE:  get the logic name of driver
*
****************************************************************************/
char chFDFirstDriveFromMask (ULONG unitmask)
{

      char i;
      for (i = 0; i < 26; ++i)  
      {
           if (unitmask & 0x1) 
				break;
            unitmask = unitmask >> 1;
      }
    return (i + 'A');
}

/****************************************************************************
*
*    FUNCTION: ReInitFloppy_Disk_Letter()
*
*    PURPOSE:  get the usb disks, and filling the 'szMoveDiskName' with them
*
****************************************************************************/
int ReInitFloppy_Disk_Letter(char *Drivelist)
{
	int k = 0;
	DWORD			MaxDriveSet, CurDriveSet;
	DWORD			drivetype;
	TCHAR			szBuf[300];
	int drive;
	HANDLE			hDevice;
	
	int l = 0;
	int len=0;
	PSTORAGE_DEVICE_DESCRIPTOR pDevDesc;
	memset(FD_Index_Drive,0,sizeof(FD_Index_Drive));
	memset(FD_IDE_Drive,0,sizeof(FD_Index_Drive));
	
	k = 0;		
//	 Get available drives we can monitor
	MaxDriveSet = CurDriveSet = 0;

	MaxDriveSet = GetLogicalDrives();
	
	CurDriveSet = MaxDriveSet;
	for ( drive = 0; drive < 32; ++drive )  
	{
		if ( MaxDriveSet & (1 << drive) )  
		{
			DWORD temp = 1<<drive;
			int error_ret = 0;
			_stprintf( FD_szDrvName, _T("%c:\\"), 'A'+drive );
			
			int Drivetype = GetDriveType( FD_szDrvName );
			
			switch ( GetDriveType( FD_szDrvName ) )  
			{
				case 0:					// The drive type cannot be determined.
				case 1:					// The root directory does not exist.
					drivetype = DRVUNKNOWN;
					break;
				case DRIVE_CDROM:		// The drive is a CD-ROM drive.					
					break;
				case DRIVE_REMOVABLE:
				case DRIVE_FIXED:		// The disk cannot be removed from the drive.
					drivetype = DRVFIXED;
					sprintf(szBuf, "\\\\.\\%c:", 'A'+drive);
					hDevice = CreateFile(szBuf, 0,
							0, NULL, OPEN_EXISTING, 0, NULL);
					
					
					if (hDevice != INVALID_HANDLE_VALUE)
					{
						STORAGE_DEVICE_NUMBER sdn;
						DWORD returned;
						
						pDevDesc = (PSTORAGE_DEVICE_DESCRIPTOR)new BYTE[sizeof(STORAGE_DEVICE_DESCRIPTOR) + 512 - 1];

						pDevDesc->Size = sizeof(STORAGE_DEVICE_DESCRIPTOR) + 512 - 1;
						BOOL diskProperty_Ret = GetFloppyDisksProperty(hDevice, pDevDesc);
						
						if(diskProperty_Ret == 0)
						{
							if(pDevDesc->BusType == BusTypeUsb)
							{							
								if (DeviceIoControl(hDevice,IOCTL_STORAGE_GET_DEVICE_NUMBER,NULL ,0,&sdn,sizeof(sdn),&returned,NULL))
								{	
									if(sdn.PartitionNumber == -1 || sdn.PartitionNumber == 0)
									{				
										strncpy(&FD_Index_Drive[sdn.DeviceNumber][0],FD_szDrvName,1);
									}else
									if(sdn.PartitionNumber > 0)
									{	
									}
								}
							}
							else
							{								
								if (DeviceIoControl(hDevice,IOCTL_STORAGE_GET_DEVICE_NUMBER,NULL ,0,&sdn,sizeof(sdn),&returned,NULL))
								{
									if(sdn.PartitionNumber == -1)
									{												
										strncpy(&FD_Index_Drive[sdn.DeviceNumber][2],FD_szDrvName,1);
									}else
									if(sdn.PartitionNumber > 0)
									{
									}
								}								
							}
						}
						else if(diskProperty_Ret == 1)
						{
							
							strncpy(&FD_IDE_Drive[k++][0],FD_szDrvName,1);

						}
						
						delete pDevDesc;
						CloseHandle(hDevice);
					}
					
					break;
				case DRIVE_REMOTE:		// The drive is a remote (network) drive.
					break;
				case DRIVE_RAMDISK:		// The drive is a RAM disk.
					drivetype = DRVRAM;
					break;
			}
		
		}
	}

	len=0;
	for(int n=0;n<32;n++)
	{
		strcpy(Drivelist,FD_Index_Drive[n]);
	
		if(*Drivelist != '\0')
		{	
			Drivelist+=strlen(Drivelist);
			len++;
		}
	}
	for(int n=0;n<32;n++)
	{
		strcpy(Drivelist,FD_IDE_Drive[n]);
	
		if(*Drivelist != '\0')
		{	
			Drivelist+=strlen(Drivelist);
			len++;
		}
	}
		
	return len;
}
