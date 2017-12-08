////////////////////////////////////////////////////////////
//
// Module SCSIBUS.HPP
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


#ifndef SCSIBUS_HPP_INCLUDED
#define SCSIBUS_HPP_INCLUDED


#ifndef SCSIDEV_HPP_INCLUDED
#include "scsidev.h"
#endif

#ifndef LIST_HPP_INCLUDED
#include "list.h"
#endif


#define MAX_HOST_ADAPTERS  10
#define MAX_CD_DRIVES	   10

typedef struct {
   int AdapterNum;         // adapter number
   int ScsiId;             // host SCSI ID
   int MaxUnits;           // max devices
   int Residual;           // residual byte support
   int Align;              // alignment mask
   char ManagerId[18];     // ASPI manager
   char Identifier[18];    // adapter string
} AdapterInfo;

typedef struct {
   int AdapterNum;         // adapter number
   int ScsiId;             // host SCSI ID
   int Lun;				   // Logical Unit Number
   int DriveLetter;		   // Drive Letter
} CDDriveSCSIInfoMap;

class ScsiInterface {

   public:

   int AspiIsOpen;
   unsigned NumAdapters;
   unsigned NumDevices;
   unsigned NumCDDrives;

   AdapterInfo *AdapterList;
   CDDriveSCSIInfoMap *CDDriveSCSIInfoMapList;

   ItemList ScsiDevList;
   ItemList ScsiCDDrivesList;

   ScsiInterface();
   ScsiInterface(int BuildDeviceList, int type=-1, int scan_luns=0);
   ~ScsiInterface();

   ScbError OpenAspiLayer();
   unsigned GetNumAdapters();

   unsigned GetNumDevices();
   ScsiDevice *GetDevice(unsigned i);

   int BuildDeviceList(int type=-1, int scan_luns=0);
   void ClearDeviceList();
   ScbError RescanBus(int type=-1, int scan_luns=0);

   ScbError AttachDevice(unsigned adapter, unsigned unit, unsigned lun, int type=-1);
   void RemoveDevice(unsigned adapter, unsigned unit, unsigned lun);

   ScsiDevice *FindDevice(char *name);
   ScsiDevice *FindDevice(unsigned adapter, unsigned unit, unsigned lun);

   void GetCDDrivesLetters();
   BYTE GetCDInfo(int adapter, int targetid, int lun);
   unsigned GetNumCDDrives();

   };


#endif