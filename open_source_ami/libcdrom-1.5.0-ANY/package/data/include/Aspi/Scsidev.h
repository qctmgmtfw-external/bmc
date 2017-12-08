////////////////////////////////////////////////////////////
//
// Module SCSIDEV.HPP
//
// ASPI class library
// SCSI device class
//
// Project: A Programmer's Guide to SCSI
//
// Copyright (C) 1997, Brian Sawert
// Portions copyright (C) 1995, Larry Martin
// All rights reserved
//
////////////////////////////////////////////////////////////


#ifndef SCSIDEV_HPP_INCLUDED
#define SCSIDEV_HPP_INCLUDED


#ifndef SCB_HPP_INCLUDED
#include "scb.h"
#endif


class ScsiDevice
   {
   public:

   int Adapter;
   int Unit;
   int Lun;
   int Type;
   char *RealName;
   char *Name;
   char *Revision;
   unsigned int AnsiVersion;
   unsigned int bRemovable;
   unsigned int bWide32;
   unsigned int bWide16;
   unsigned int bSync;
   unsigned int bLinked;
   unsigned int bQueue;
   unsigned int bSoftReset;
#if defined (WIN64)
   int RetryOnScsiBusy;            // milliseconds to wait
   int RetryOnScsiError;           // milliseconds to wait
   int RetryOnUnitAttention;       // milliseconds to wait
   int RetryOnTargetBusy;          // milliseconds to wait
   int RetryOnTargetNotReady;      // milliseconds to wait
   int RetryOnTargetBecomingReady; // milliseconds to wait
#else
   long RetryOnScsiBusy;            // milliseconds to wait
   long RetryOnScsiError;           // milliseconds to wait
   long RetryOnUnitAttention;       // milliseconds to wait
   long RetryOnTargetBusy;          // milliseconds to wait
   long RetryOnTargetNotReady;      // milliseconds to wait
   long RetryOnTargetBecomingReady; // milliseconds to wait
#endif   

   ScsiDevice();
   ~ScsiDevice();

   char *GetName()      { return Name; };
   char *GetRealName()  { return RealName; };
   char *GetRevision()  { return Revision; };
   int GetAdapter()  { return Adapter; };
   int GetUnit()     { return Unit; };
   int GetLun()      { return Lun; };
   int GetType()     { return Type; };
   unsigned int GetAnsiVersion() { return AnsiVersion; };
   unsigned int IsRemovable()    { return bRemovable; };
   unsigned int IsWide32()       { return bWide32; };
   unsigned int IsWide16()       { return bWide16; };
   unsigned int IsSync()         { return bSync; };
   unsigned int IsLinked()       { return bLinked; };
   unsigned int IsQueue()        { return bQueue; };
   unsigned int IsSoftReset()    { return bSoftReset; };
   int SetName(char *name);

   ScbError Init(unsigned adapter, unsigned unit, unsigned lun );

#if defined (WIN64)
   ScbError ExecuteScb( ScsiCmdBlock &scb, int timeout );
#else
   ScbError ExecuteScb( ScsiCmdBlock &scb, long timeout );
#endif
   };


#endif
