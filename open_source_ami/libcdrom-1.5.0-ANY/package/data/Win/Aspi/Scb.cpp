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

#include <stddef.h>
#include <string.h>
#include <time.h>

#include "scb.h"

#define min(a,b)  (((a) < (b)) ? (a) : (b))


ScbError MapError(ScsiRequestBlock *srb)
{
   switch (srb->io.SRB_Status)
      {
      case SS_PENDING:
         return Err_None;
      case SS_COMP:
         return Err_None;
      case SS_ABORTED:
         return Err_Aborted;
      case SS_ABORT_FAIL:
         return Err_Aborted;
      case SS_INVALID_CMD:
         return Err_InvalidCommand;
      case SS_INVALID_HA:
         return Err_InvalidHostAdapter;
      case SS_NO_DEVICE:
         return Err_NoDevice;
      case SS_INVALID_SRB:
         return Err_InvalidSrb;
      case SS_FAILED_INIT:
         return Err_FailedInit;
      case SS_ASPI_IS_BUSY:
         return Err_Busy;
      case SS_BUFFER_TO_BIG:
         return Err_BufferTooBig;
      case SS_ERR:
         switch (srb->io.SRB_HaStat)
            {
            case HASTAT_SEL_TO:
               return Err_SelectionTimeout;
            case HASTAT_DO_DU:
               return Err_DataOverrun;
            case HASTAT_BUS_FREE:
               return Err_UnexpectedBusFree;
            case HASTAT_PHASE_ERR:
               return Err_TargetPhase;
            case HASTAT_TIMEOUT:
               return Err_Timeout;
            case HASTAT_COMMAND_TIMEOUT:
               return Err_CommandTimeout;
            case HASTAT_MESSAGE_REJECT:
               return Err_MessageReject;
            case HASTAT_BUS_RESET:
               return Err_BusReset;
            case HASTAT_PARITY_ERROR:
               return Err_ParityError;
            case HASTAT_REQUEST_SENSE_FAILED:
               return Err_RequestSenseFailed;
            case HASTAT_OK:
               switch (srb->io.SRB_TargStat)
                  {
                  case 0x00:
                     // special case handling
                     if (srb->io.SRB_SenseArea[0] != 0)
                     // sense data present
                     {
                        return Err_CheckCondition;
                     }
                     return Err_None;
                  case 0x02:
                     return Err_CheckCondition;
                  case 0x08:
                     return Err_TargetBusy;
                  case 0x18:
                     return Err_TargetReservationConflict;
                  case 0x28:
                     return Err_TargetQueueFull;
                  default:
                     return Err_Unknown;
                  }
            default:
               return Err_Unknown;
            }
      default:
         return Err_Unknown;
      }
}


ScsiCmdBlock::ScsiCmdBlock()
{
   LastError = Err_None;
}


ScsiCmdBlock::~ScsiCmdBlock()
{
}


void ScsiCmdBlock::Init( unsigned cmd, unsigned adapter, unsigned target, unsigned lun )
{
   LastError = Err_None;
   memset(&srb,0,sizeof(srb));
   srb.io.SRB_Cmd = cmd;
   srb.io.SRB_HaId = adapter;
   switch (cmd)
      {
      case SC_GET_DEV_TYPE:
      case SC_EXEC_SCSI_CMD:
      case SC_RESET_DEV:
      case SC_GET_DISK_INFO:
         srb.io.SRB_Target = target;
         srb.io.SRB_Lun = lun;
         break;
      }
}

// The following routines assume an SC_EXEC_SCSI_CMD command
void ScsiCmdBlock::SetCdb(void *cdb, unsigned nbytes)
{
   srb.io.SRB_CDBLen = min(nbytes,sizeof(srb.io.SRB_CDBByte));
   memcpy(srb.io.SRB_CDBByte,cdb,min(nbytes,sizeof(srb.io.SRB_CDBByte)));
}


void ScsiCmdBlock::GetSense(void *sense, unsigned maxbytes)
{
   memcpy(sense,srb.io.SRB_SenseArea,min(maxbytes,SENSE_LEN));
}


void ScsiCmdBlock::SetDataBuffer(void *bufp, unsigned buflen)
{
   srb.io.SRB_BufPointer = (BYTE *) bufp;
   srb.io.SRB_BufLen = buflen;
}

#ifdef   WIN64
int DoAspiCommand(ScsiRequestBlock *p, int timeout)
#else
int DoAspiCommand(ScsiRequestBlock *p, long timeout)
#endif
{
   HANDLE hEvent;
   long wait;


   // get event handle
   hEvent = p->io.SRB_PostProc;

   // map timeout value
   wait = (timeout == -1L) ? INFINITE : timeout;

   ResetEvent(hEvent);

   aspi_SendCommand(p);

   if ( p->io.SRB_Status == SS_PENDING )
      {
      if (WaitForSingleObject(hEvent, wait) == WAIT_OBJECT_0)
      // event completed
      {
         ResetEvent(hEvent);
         return 1;
      }

      time_t elapsed_time;
      time_t starttime = time(NULL);

      while (p->io.SRB_Status == SS_PENDING)
         {
         elapsed_time = time(NULL) - starttime;
         if (timeout != -1)
            {
            if ( elapsed_time > (timeout/1000 + 1) )
               {
               if (p->io.SRB_Cmd != SC_ABORT_SRB)
                  {
                  // Abort it now
                  SRB_Abort a;
                  memset(&a,0,sizeof(a));
                  a.SRB_Cmd = SC_ABORT_SRB;
                  a.SRB_HaId = p->io.SRB_HaId;
                  a.SRB_ToAbort = p;
                  aspi_SendCommand(&a);
                  starttime = time(NULL);
                  while (a.SRB_Status == SS_PENDING)
                     {
                     if ( time(NULL) > (starttime + 4) )
                        {
                        // Something has gone horribly wrong.
                        // We can't even abort the command.
                        // Ignore the abort, and pretend the
                        // original command timed out.
                        break;
                        }
                     Sleep(10L);
                     }
                  }
               // Aborted, return code
               return 0;
               }
            }
         if (elapsed_time > 2)      // is this a long command?
            Sleep(1000L);        // if so, give the OS more time
         else
            Sleep(10L);          // else just give it a little
         }
      }

   return 1;
}

#if defined (WIN64)
ScbError ScsiCmdBlock::Execute(int timeout)
#else
ScbError ScsiCmdBlock::Execute(long timeout)
#endif
{
   HANDLE hEvent;

   // Create event to notify completion
   hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

   if (!hEvent)
   // Could not create event
   {
      return Err_NoEvent;
   }

   srb.io.SRB_PostProc = hEvent;
   srb.io.SRB_Flags |= SRB_EVENT_NOTIFY;

   // We have room for the sense data, so fill it in.
   srb.io.SRB_SenseLen = SENSE_LEN;
   // srb.io.SRB_SenseLen = 20;

   if (!DoAspiCommand(&srb, timeout))
      {
      LastError = Err_Timeout;
      }
   else
      {
      LastError = MapError(&srb);
      }

   CloseHandle(hEvent);

   return LastError;
}
