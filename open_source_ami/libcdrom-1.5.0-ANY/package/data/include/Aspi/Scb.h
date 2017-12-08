////////////////////////////////////////////////////////////
//
// Module SCB.HPP
//
// ASPI class library
//
// Project: A Programmer's Guide to SCSI
//
// Copyright (C) 1997, Brian Sawert
// Portions copyright (C) 1995, Larry Martin
// All rights reserved
//
////////////////////////////////////////////////////////////


#ifndef SCB_HPP_INCLUDED
#define SCB_HPP_INCLUDED


#include "aspi.h"


typedef enum ScbError
      {
      Err_None                = 0,
      Err_CheckCondition            = 1,
      Err_Aborted                = 2,
      Err_InvalidCommand            = 3,
      Err_InvalidHostAdapter        = 4,
      Err_NoDevice               = 5,
      Err_InvalidSrb             = 6,
      Err_FailedInit             = 7,
      Err_Busy                = 8,
      Err_BufferTooBig           = 9,
      Err_SelectionTimeout       = 10,
      Err_DataOverrun               = 11,
      Err_UnexpectedBusFree         = 12,
      Err_TargetPhase               = 13,
      Err_Timeout                = 14,
      Err_CommandTimeout            = 15,
      Err_MessageReject          = 16,
      Err_BusReset               = 17,
      Err_ParityError               = 18,
      Err_RequestSenseFailed        = 19,
      Err_TargetBusy             = 20,
      Err_TargetReservationConflict = 21,
      Err_TargetQueueFull           = 22,
      Err_OutOfMemory               = 23,
      Err_AspiDriver             = 24,
      Err_Unknown                = 25,
      Err_NoEvent                = 26
      } ScbError;

class ScsiCmdBlock
   {

   public:

   ScsiRequestBlock srb;
   ScbError LastError;

   ScsiCmdBlock();
   ~ScsiCmdBlock();

   void Init( unsigned cmd, unsigned adapter=0, unsigned target=0, unsigned lun=0 );

   // The following routines assume an SC_EXEC_SCSI_CMD type command
   void SetCdb(void *cdb, unsigned nbytes);
   void GetSense(void *sense, unsigned maxbytes);
   void SetDataBuffer(void *bufp, unsigned buflen);
#if defined (WIN64)
   ScbError Execute(int timeout = -1L);
#else
   ScbError Execute(long timeout = -1L);
#endif

   };


#endif

