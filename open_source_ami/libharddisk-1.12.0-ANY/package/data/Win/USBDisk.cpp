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
TCHAR   Index_Drive[32][32];

//TCHAR   szMoveFixedDiskName[33];
TCHAR	szDrvName[33];
BOOL	bInitUSBs;
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
*    FUNCTION: GetDisksProperty(HANDLE hDevice, PSTORAGE_DEVICE_DESCRIPTOR pDevDesc)
*
*    PURPOSE:  get the info of specified device
*
****************************************************************************/
BOOL GetDisksProperty(HANDLE hDevice, PSTORAGE_DEVICE_DESCRIPTOR pDevDesc)
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

	return bResult;
}

/****************************************************************************
*
*    FUNCTION: chFirstDriverFrameMask(ULONG unitmask)
*
*    PURPOSE:  get the logic name of driver
*
****************************************************************************/
char chFirstDriveFromMask (ULONG unitmask)
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
*    FUNCTION: GetPartitionCount(char *Drive)
*
*    PURPOSE:  get the partition count of driver
*
****************************************************************************/
int GetPartitionCount(char *Drive)
{

	BOOL bResult;                 // results flag
	DWORD returned;                   // discard results
	HANDLE	hDevice;
	char DrvName[24];
	memset(DrvName,0,24);
	

	SECURITY_ATTRIBUTES saTest;	

	saTest.bInheritHandle = TRUE;
	saTest.nLength = sizeof(SECURITY_ATTRIBUTES);
	saTest.lpSecurityDescriptor = NULL;


	strcat(DrvName,"\\\\.\\");
	strcat(DrvName,Drive);
	hDevice = CreateFile(DrvName, GENERIC_WRITE | GENERIC_READ,
	FILE_SHARE_READ | FILE_SHARE_WRITE, &saTest,
	OPEN_EXISTING, 0, NULL);

	if (hDevice != INVALID_HANDLE_VALUE)
	{
		DWORD nPartitions = 16;
		DWORD nBufferSize = sizeof(DRIVE_LAYOUT_INFORMATION) + nPartitions * sizeof(PARTITION_INFORMATION);

	DRIVE_LAYOUT_INFORMATION* pDLI = 
	(DRIVE_LAYOUT_INFORMATION*)malloc(sizeof(DRIVE_LAYOUT_INFORMATION) + nPartitions * sizeof(PARTITION_INFORMATION));

	bResult = DeviceIoControl(hDevice,IOCTL_DISK_GET_DRIVE_LAYOUT,NULL,0,pDLI,nBufferSize,&returned,NULL); 
 
	CloseHandle(hDevice);

		return (pDLI->PartitionCount);
	}
	return 0;
}
/****************************************************************************
*
*    FUNCTION: ReInitUSB_Disk_Letter()
*
*    PURPOSE:  get the usb disks, and filling the 'szMoveDiskName' with them
*
****************************************************************************/
int ReInitUSB_Disk_Letter(char *Drivelist,int type)
{
	int k = 0;
	DWORD			MaxDriveSet, CurDriveSet;
	DWORD			drive, drivetype;
	TCHAR			szBuf[300];
	HANDLE			hDevice;
	
	int l = 0;
	int len=0;
	PSTORAGE_DEVICE_DESCRIPTOR pDevDesc;
	memset(Index_Drive,0,sizeof(Index_Drive));
	
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
			_stprintf( szDrvName, _T("%c:\\"), 'A'+drive );
			
			int Drivetype = GetDriveType( szDrvName );
			
			switch ( GetDriveType( szDrvName ) )  
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
					
					error_ret=GetLastError();
					
					if (hDevice != INVALID_HANDLE_VALUE)
					{
						STORAGE_DEVICE_NUMBER sdn;
						DWORD returned;
						
						pDevDesc = (PSTORAGE_DEVICE_DESCRIPTOR)new BYTE[sizeof(STORAGE_DEVICE_DESCRIPTOR) + 512 - 1];

						pDevDesc->Size = sizeof(STORAGE_DEVICE_DESCRIPTOR) + 512 - 1;

						if(GetDisksProperty(hDevice, pDevDesc))
						{
							if(pDevDesc->BusType == BusTypeUsb)
							{
								if(type == 1)
								{	
									if (DeviceIoControl(hDevice,IOCTL_STORAGE_GET_DEVICE_NUMBER,NULL ,0,&sdn,sizeof(sdn),&returned,NULL))
									{	
										if(sdn.PartitionNumber == -1)
										{
										}else
										if(sdn.PartitionNumber > 0)
										{	
											Index_Drive[sdn.DeviceNumber][1] = 48+Drivetype;
											Index_Drive[sdn.DeviceNumber][sdn.PartitionNumber+1]= chFirstDriveFromMask(temp);
											Index_Drive[sdn.DeviceNumber][0] = 48+sdn.DeviceNumber;
										}
									}
								}
							}
							else
							{
								if(type == 2)
								{
									if (DeviceIoControl(hDevice,IOCTL_STORAGE_GET_DEVICE_NUMBER,NULL ,0,&sdn,sizeof(sdn),&returned,NULL))
									{
										if(sdn.PartitionNumber == -1)
										{											
										}else
										if(sdn.PartitionNumber > 0)
										{
											Index_Drive[sdn.DeviceNumber][1] = 48+Drivetype;
											Index_Drive[sdn.DeviceNumber][sdn.PartitionNumber+1]= chFirstDriveFromMask(temp);
											Index_Drive[sdn.DeviceNumber][0] = 48+sdn.DeviceNumber;
										}
									}
								}
							}
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
	
	int j,z=0;
	len=0;
	TCHAR   szDrive[32];	
	for(int i=0;i<32;i++)
	{
		if( Index_Drive[i][0] != NULL)
		{
			//char DrvList[16] = "physicaldrive";
			//strncat(DrvList,&Index_Drive[i][0],1);
			//int DrvCount = GetPartitionCount(DrvList) + 2; // here 2 is added to include drive type and number
			for ( j=0;j< 32 ;j++)
			{
				if(Index_Drive[i][j] != NULL)
				{
					szDrive[z] = Index_Drive[i][j];
					z++;
				}
			}
			szDrive[z]= '\0';
			strcpy(Drivelist,szDrive);
			z=0;
			memset(szDrive,0,sizeof(szDrive));
			if(*Drivelist != '\0')
			{	
				Drivelist+=strlen(Drivelist)+1;
				len++;		
			}
		}
	}  
	return len;
}
