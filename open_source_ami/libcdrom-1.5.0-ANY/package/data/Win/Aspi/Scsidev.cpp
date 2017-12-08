////////////////////////////////////////////////////////////
//
// Module SCSIDEV.CPP
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

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "scb.h"
#include "scsidev.h"
#include "scsidefs.h"


ScsiDevice::ScsiDevice()
{
   Adapter = 0;
   Unit = 0;
   Lun = 0;
   RealName = NULL;
   Name = NULL;
   Revision = NULL;
   RetryOnScsiBusy = 3*1000L;          // milliseconds
   RetryOnScsiError = 2*1000L;            // milliseconds
   RetryOnUnitAttention = 1*1000L;        // milliseconds
   RetryOnTargetNotReady = 0L;            // milliseconds
   RetryOnTargetBusy = 30*1000L;       // milliseconds
   RetryOnTargetBecomingReady = 30*1000L; // milliseconds
}


ScsiDevice::~ScsiDevice()
{
   if (RealName)
      free(RealName);

   if (Name)
      free(Name);

   if (Revision)
      free(Revision);
}

int ScsiDevice::SetName(char *s)
{
   if (Name)
      free(Name);

   Name = strdup(s);

   return Name ? 1 : 0;
}

ScbError ScsiDevice::Init(unsigned adapter, unsigned unit, unsigned lun )
{
   ScbError err;
   ScsiCmdBlock scb;
   SCSI_Cdb_Inquiry_t cdb;
   SCSI_InquiryData_t inq;

   Adapter = adapter;
   Unit = unit;
   Lun = lun;

   scb.Init(SC_EXEC_SCSI_CMD,Adapter,Unit,Lun);

   memset(&cdb,0,sizeof(cdb));
   cdb.CommandCode = SCSI_Cmd_Inquiry;
   cdb.Lun = Lun;
   cdb.Evpd = 0;
   cdb.PageCode = 0;
   cdb.AllocationLength = sizeof(inq);
   scb.SetCdb(&cdb,6);

   memset(&inq,0,sizeof(inq));
   scb.SetDataBuffer(&inq,sizeof(inq));

   {
#if defined(WIN64)
   int tmp = RetryOnScsiError;
#else
   long tmp = RetryOnScsiError;
#endif
   err = ExecuteScb(scb,3000L);
   RetryOnScsiError = tmp;
   }

   if (!err)
      {
      Type = inq.DeviceType;
      if (RealName)
         free(RealName);
      RealName = (char *) malloc(sizeof(inq.VendorId)+sizeof(inq.ProductId)+2);
      if (RealName)
         {
         char *s = RealName;
         int i;
         for (i=0; i<sizeof(inq.VendorId); i++)
            *s++ = inq.VendorId[i];
         // Trim trailing spaces
         while (--s > RealName)
            {
            if (!(isascii(*s) && isspace(*s)))
               break;
            }
         s++;
         *s++ = ' ';
         for (i=0; i<sizeof(inq.ProductId); i++)
            *s++ = inq.ProductId[i];
         *s = '\0';
         // Trim trailing spaces
         while (--s > RealName)
            {
            if (isascii(*s) && isspace(*s))
               *s = '\0';
            else
               break;
            }
         if (Name == NULL)
            SetName(RealName);
         }

      // Save revision string
      if (Revision)
         free(Revision);
      Revision = (char *) malloc(sizeof(inq.ProductRevisionLevel+1));
      if (Revision)
         {
         char *s = Revision;
         int i;
         for (i=0; i<sizeof(inq.ProductRevisionLevel); i++)
            *s++ = inq.ProductRevisionLevel[i];
         *s = '\0';

         // Trim trailing spaces
         while (--s > Revision)
            {
            if (isascii(*s) && isspace(*s))
               *s = '\0';
            else
               break;
            }
         }

      // save other properties
         AnsiVersion = inq.AnsiVersion;
         bRemovable = inq.RemovableMedia;
         bWide32 = inq.WideBus32Support;
         bWide16 = inq.WideBus16Support;
         bSync = inq.SynchronousTransferSupport;
         bLinked = inq.LinkedCommandSupport;
         bQueue = inq.CommandQueueSupport;
         bSoftReset = inq.SoftResetSupport;
      }
   else
      {
      Type = 0x1F;
      }

   return err;
}

#if defined (WIN64)
ScbError ScsiDevice::ExecuteScb( ScsiCmdBlock &scb, int timeout )
#else
ScbError ScsiDevice::ExecuteScb( ScsiCmdBlock &scb, long timeout )
#endif
{
   ScbError err;
   unsigned tries = 0;
#if defined(WIN64)
   int start_time = 0;
#else
   long start_time = 0;
#endif
   int retry;
   int scsi_error;
   int scsi_busy;
   int target_busy;
   int target_becoming_ready;
   int target_not_ready;
   int target_unit_attention;

   do
      {
      retry = 0;
      scsi_error = 0;
      scsi_busy = 0;
      target_busy = 0;
      target_becoming_ready = 0;
      target_not_ready = 0;
      target_unit_attention = 0;

      err = scb.Execute(timeout);

      switch (err)
         {
         case Err_None:
            return err;

         case Err_Busy:
            scsi_busy = 1;
            if (RetryOnScsiBusy)
               retry = 1;
            break;

         case Err_TargetBusy:
         case Err_TargetQueueFull:
            target_busy = 1;
            if (RetryOnTargetBusy)
               retry = 1;
            break;

         case Err_SelectionTimeout:
         case Err_UnexpectedBusFree:
         case Err_TargetPhase:
         case Err_BusReset:
         case Err_ParityError:
            scsi_error = 1;
            if (RetryOnScsiError)
               retry = 1;
            break;

         case Err_CheckCondition:
            {
            SCSI_SenseData_t sense;
            scb.GetSense(&sense,sizeof(sense));
            if (sense.ErrorCode == 0x70)
               {
               switch (sense.SenseKey)
                  {
                  case SCSI_Skey_RecoveredError:
                     return Err_None;
                     break;

                  case SCSI_Skey_NotReady:
                     switch ( (sense.Asc << 8) | sense.Asq )
                        {
                        case 0x0401:   // coming ready?
                           target_becoming_ready = 1;
                           if (RetryOnTargetBecomingReady)
                              retry = 1;
                           break;
                        default:
                           target_not_ready = 1;
                           if (RetryOnTargetNotReady)
                              retry = 1;
                        }
                     break;

                  case SCSI_Skey_UnitAttention:
                     target_unit_attention = 1;
                     if (RetryOnUnitAttention)
                        retry = 1;
                     break;

                  default:
                     return err;
                  }
               }
            }
            break;

         default:
            return err;
         }

      if (retry)
         {
#if defined(WIN64)
         int elapsed_time;
#else
         long elapsed_time;
#endif

         if (start_time == 0)
            {
            start_time = time(NULL);
            elapsed_time = 0;
            }
         else
            elapsed_time = time(NULL) - start_time;

         if (scsi_busy)
            {
            if (elapsed_time > (RetryOnScsiBusy/1000 + 1) )
               return err;
            }
         else if (scsi_error)
            {
            if (elapsed_time > (RetryOnScsiError/1000 + 1) )
               return err;
            }
         else if (target_busy)
            {
            if (elapsed_time > (RetryOnTargetBusy/1000 + 1) )
               return err;
            Sleep(1000L);
            }
         else if (target_becoming_ready)
            {
            if (elapsed_time > (RetryOnTargetBecomingReady/1000 + 1) )
               return err;
            Sleep(1000L);
            }
         else if (target_unit_attention)
            {
            if (elapsed_time > (RetryOnUnitAttention/1000 + 1) )
               return err;
            }
         else if (target_not_ready)
            {
            if (elapsed_time > (RetryOnTargetNotReady/1000 + 1) )
               return err;
            Sleep(1000L);
            }
         else
            return err;
         }

      } while (retry);


   return err;
}


