////////////////////////////////////////////////////////////
//
// Module SCSIBUS.CPP
//
// ASPI class library
// SCSI interface class
//
// Project: A Programmer's Guide to SCSI
//
// Copyright (C) 1997, Brian Sawert
// Portions copyright (C) 1995, Larry Martin
// All rights reserved
//
////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "aspi.h"
#include "list.h"
#include "scsidev.h"
#include "scsibus.h"


ScsiInterface::ScsiInterface()
{
   AspiIsOpen = 0;
   NumDevices = 0;
   NumAdapters = 0;
   NumCDDrives = 0;

   AdapterList = (AdapterInfo *) malloc(
      sizeof(AdapterInfo) * MAX_HOST_ADAPTERS);

   CDDriveSCSIInfoMapList = (CDDriveSCSIInfoMap *) malloc(
	   sizeof(CDDriveSCSIInfoMap) * MAX_CD_DRIVES);

   // paranoia
   assert(AdapterList != NULL);
   assert(CDDriveSCSIInfoMapList != NULL);
}

ScsiInterface::ScsiInterface(int build_device_list, int type, int scan_luns)
{
   AspiIsOpen = 0;
   NumAdapters = 0;
   NumDevices = 0;
   NumCDDrives = 0;

   AdapterList = (AdapterInfo *) malloc(
      sizeof(AdapterInfo) * MAX_HOST_ADAPTERS);

   CDDriveSCSIInfoMapList = (CDDriveSCSIInfoMap *) malloc(
	   sizeof(CDDriveSCSIInfoMap) * MAX_CD_DRIVES);

   // paranoia
   assert(AdapterList != NULL);
   assert(CDDriveSCSIInfoMapList != NULL);

   if (build_device_list)
      BuildDeviceList(type,scan_luns);
}

ScsiInterface::~ScsiInterface()
{
   ClearDeviceList();

   free(AdapterList);
   free(CDDriveSCSIInfoMapList);
}

ScsiDevice *ScsiInterface::FindDevice(char *name)
{
   ScsiDevice *p;
   ItemListIterator i;

   for ( p=(ScsiDevice *) i.GetFirst(&ScsiDevList); p; p=(ScsiDevice *)i.IterateToNext() )
      {
      if (strnicmp(name,p->GetName(),strlen(name)) == 0)
         return p;
      }

   return NULL;
}

ScsiDevice *ScsiInterface::FindDevice(unsigned adapter, unsigned unit, unsigned lun)
{
   ScsiDevice *p;
   ItemListIterator i;

   for ( p=(ScsiDevice *)i.GetFirst(&ScsiDevList); p; p=(ScsiDevice *)i.IterateToNext() )
      {
      if (adapter == (unsigned) p->GetAdapter() &&
         unit == (unsigned) p->GetUnit() &&
         lun== (unsigned) p->GetLun())
         return p;
      }

   return NULL;
}


ScbError ScsiInterface::OpenAspiLayer()
{
   int x;

   x = aspi_GetSupportInfo();
   if ( ((x >> 8) & 0xFF) != SS_COMP )
      return Err_FailedInit;

   NumAdapters = x & 0xFF;
   AspiIsOpen = 1;

   return Err_None;
}


unsigned ScsiInterface::GetNumAdapters()
{
   if (!AspiIsOpen)
      {
      ScbError err;
      err = OpenAspiLayer();
      if (err)
         return 0;
      }

   return NumAdapters;
}

ScbError ScsiInterface::AttachDevice(unsigned adapter, unsigned unit, unsigned lun, int type)
{
   ScbError err;
   ScsiCmdBlock scb;
   ScsiDevice *dev;

   if (!AspiIsOpen)
      {
      err = OpenAspiLayer();
      if (err)
         return err;
      }

   // Make sure we don't already have it attached
   if ( (dev=FindDevice(adapter,unit,lun)) != NULL )
      {
      if ( (type == -1) || (type == dev->GetType()) )
         return Err_None;
      else
         return Err_NoDevice; // Wrong type
      }

   // See if device really exists by getting its device type
   // This should cut down on init time
   scb.Init(SC_GET_DEV_TYPE,adapter,unit,lun);
   err = scb.Execute(1000L);
   if (err)
      return err;

   if ( (type != -1) && (type != scb.srb.gdt.SRB_DeviceType) )
      {
      // Wrong type
      return Err_NoDevice;
      }

   dev = new ScsiDevice;
   if (!dev)
      return Err_OutOfMemory;

   err = dev->Init(adapter,unit,lun);
   if (err)
      {
      delete dev;
      return err;
      }

   dev->SetName(dev->GetRealName());
   if (!ScsiDevList.AddItem(dev))
   {
      delete dev;
      return Err_OutOfMemory;
   }
   NumDevices++;

   /* If it's a CD Drive then also add in ScsiCDDrivesList */
   if(dev->GetType() == 5)
   {
	   if (!ScsiCDDrivesList.AddItem(dev))
	   {
			delete dev;
			return Err_OutOfMemory;
	   }
	   GetCDDrivesLetters();
       NumCDDrives++;
   }

   return Err_None;
}


void ScsiInterface::RemoveDevice(unsigned adapter, unsigned unit, unsigned lun)
{
   ScsiDevice *dev;

   dev = FindDevice(adapter,unit,lun);
   if (dev)
      {
      ScsiDevList.RemoveItem(dev);
      delete dev;
      NumDevices--;
      }
}


void ScsiInterface::ClearDeviceList()
{
   ScsiDevice *dev;

   // Destroy all allocated devices
   while ( (dev=(ScsiDevice *)ScsiDevList.GetFirstItem()) != NULL )
      {
      ScsiDevList.RemoveItem(dev);
      delete dev;
      }

   NumDevices = 0;
   NumCDDrives = 0;
}


int ScsiInterface::BuildDeviceList(int type,int scan_luns)
{
   unsigned adapter,unit,lun;
   ScbError err;

   if (!AspiIsOpen)
      {
      err = OpenAspiLayer();
      if (err)
         return 0;
      }

   // paranoia
   assert(NumAdapters <= MAX_HOST_ADAPTERS);

   for (adapter=0; adapter<NumAdapters; adapter++)
      {
      ScsiCmdBlock scb;
      scb.Init(SC_HA_INQUIRY,adapter,0,0);
      err = scb.Execute(3000);
      if (!err)
         {
         unsigned host_unit = scb.srb.hai.HA_SCSI_ID;
         unsigned max_units = scb.srb.hai.HA_Unique[3];
         if (max_units == 0)     // old ASPI managers?
            max_units = 8;

         // Save host adapter information
         if (AdapterList)
            {
            char *s;
            int i;

            AdapterList[adapter].AdapterNum = adapter;
            AdapterList[adapter].ScsiId = host_unit;
            AdapterList[adapter].MaxUnits = max_units;
            AdapterList[adapter].Residual =
               scb.srb.hai.HA_Unique[2];
            AdapterList[adapter].Align =
               ((WORD) scb.srb.hai.HA_Unique[0] |
               (WORD) scb.srb.hai.HA_Unique[1] << 16);

            // Save adapter manager ID
            s = AdapterList[adapter].ManagerId;

            for (i=0; i<sizeof(scb.srb.hai.HA_ManagerId); i++)
               *s++ = scb.srb.hai.HA_ManagerId[i];
            *s = '\0';

            // Trim trailing spaces
            while (--s > AdapterList[adapter].ManagerId)
               {
               if (isascii(*s) && isspace(*s))
                  *s = '\0';
               else
                  break;
               }

            // Save adapter identifier
            s = AdapterList[adapter].Identifier;

            for (i=0; i<sizeof(scb.srb.hai.HA_Identifier); i++)
               *s++ = scb.srb.hai.HA_Identifier[i];
            *s = '\0';

            // Trim trailing spaces
            while (--s > AdapterList[adapter].Identifier)
               {
               if (isascii(*s) && isspace(*s))
                  *s = '\0';
               else
                  break;
               }
            }

         for (unit=0; unit<max_units; unit++)
            {
            if (unit != host_unit)
               {
               if (scan_luns)
                  {
                  for (lun=0; lun<8; lun++)
                     {
                     err = AttachDevice(adapter,unit,lun,type);
                     if (err)
                        break;
                     }
                  }
               else
                  {
                  lun = 0;
                  AttachDevice(adapter,unit,lun,type);
                  }
               }
            }
         }
      }

   return NumDevices;
}


unsigned ScsiInterface::GetNumDevices()
{
   if (!AspiIsOpen)
      {
      ScbError err;
      err = OpenAspiLayer();
      if (err)
         return 0;
      }

   return NumDevices;
}

unsigned ScsiInterface::GetNumCDDrives()
{
   if (!AspiIsOpen)
      {
      ScbError err;
      err = OpenAspiLayer();
      if (err)
         return 0;
      }

   return NumCDDrives;
}

ScsiDevice *ScsiInterface::GetDevice(unsigned n)
{
   ItemListIterator i;
   unsigned j;

   i.GetFirst(&ScsiDevList);  // Sets it up

   for (j=0; j<n; j++)
      {
      if (i.IterateToNext() == NULL)
         return NULL;
      }

   return (ScsiDevice *) i.GetCurrent();
}

void ScsiInterface::GetCDDrivesLetters()
{
		ScsiDevice *pScsiCDDrivesList = NULL;
		ItemListIterator i;
		BYTE drive;
		int cdNum = 0;

		for (pScsiCDDrivesList=(ScsiDevice *) i.GetFirst(&(ScsiCDDrivesList)); pScsiCDDrivesList; pScsiCDDrivesList=(ScsiDevice *)i.IterateToNext())
		{
				drive = GetCDInfo(pScsiCDDrivesList->Adapter,
							pScsiCDDrivesList->Unit,
							pScsiCDDrivesList->Lun);
				if(CDDriveSCSIInfoMapList)
				{
					CDDriveSCSIInfoMapList[cdNum].AdapterNum = pScsiCDDrivesList->Adapter;
					CDDriveSCSIInfoMapList[cdNum].ScsiId = pScsiCDDrivesList->Unit;
					CDDriveSCSIInfoMapList[cdNum].Lun = pScsiCDDrivesList->Lun;
					CDDriveSCSIInfoMapList[cdNum].DriveLetter = drive;
					cdNum++;
				}
		}
}

BYTE ScsiInterface::GetCDInfo(int adapter, int targetid, int lun)
{
	  ScbError err;
      ScsiCmdBlock scb;

      scb.Init(SC_GET_DISK_INFO,adapter,targetid,lun);
      err = scb.Execute(3000);
	  return (scb.srb.gdi.SRB_Int13HDriveInfo + 0x41);
#if 0
		  /* print everything to see what u got */
			printf("SRB_Cmd = %x\n",scb.srb.gdi.SRB_Cmd);
			printf("SRB_Status = %x\n",scb.srb.gdi.SRB_Status);
			printf("SRB_HaId = %x\n",scb.srb.gdi.SRB_HaId);
			printf("SRB_Flags = %x\n",scb.srb.gdi.SRB_Flags);
			printf("SRB_Target = %x\n",scb.srb.gdi.SRB_Target);
			printf("SRB_Lun = %x\n",scb.srb.gdi.SRB_Lun);
			printf("SRB_DriveFlags = %x\n",scb.srb.gdi.SRB_DriveFlags);
			printf("SRB_Int13HDriveInfo = %x\n",scb.srb.gdi.SRB_Int13HDriveInfo);
			printf("SRB_Heads = %x\n",scb.srb.gdi.SRB_Heads);
			printf("SRB_Sectors = %x\n",scb.srb.gdi.SRB_Sectors);
			printf("\n");
#endif

}

ScbError ScsiInterface::RescanBus(int type, int scan_luns)
{
   ScbError err;
   ScsiCmdBlock scb;
   unsigned count;


   if (!AspiIsOpen)
      {
      err = OpenAspiLayer();
      if (err)
         return err;
      }

   // If we have a device list, clear it
   if ( NumDevices > 0 )
      {
         ClearDeviceList();
      }

   // Loop through adapters
   for (count = 0; count < NumAdapters; count++)
   {
      // Execute ASPI rescan command
      scb.Init(SC_RESCAN_SCSI_BUS,count,0,0);
      err = scb.Execute(1000L);
      if (err)
         return err;
   }

   // Strange things happen here.
   // Under Win95, there is a delay while
   // the O/S updates device maps.
   // Under NT, this will not detect that
   // devices have been removed.

   // Rebuild device list
   BuildDeviceList(type,scan_luns);

   return Err_None;
}