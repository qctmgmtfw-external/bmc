////////////////////////////////////////////////////////////
//
// Module ASPI.CPP
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
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <windows.h>
#include "aspi.h"

ScsiRequestBlock LastSRB;

HMODULE WinAspiHandle;
#if defined (WIN64)
unsigned int (*GetASPI32SupportInfo)(void);
unsigned int (*SendASPI32Command)(void *);
#else
DWORD (*GetASPI32SupportInfo)(void);
DWORD (*SendASPI32Command)(void *);
#endif

static int RunningNT;

// use for debugging console apps
static int PrintAspiCommands;
static char PrintBuf[128];


int aspi_GetSupportInfo(void)
   {
   OSVERSIONINFO v;

   if (getenv("DEBUGASPI"))
      PrintAspiCommands = 1;

   RunningNT = 0;

   v.dwOSVersionInfoSize = sizeof(v);
   if (!GetVersionEx(&v))
      return SS_FAILED_INIT << 8;

   switch (v.dwPlatformId)
      {
      case VER_PLATFORM_WIN32_NT:
         RunningNT = 1;
         // fall through to Win95

      case VER_PLATFORM_WIN32_WINDOWS:
         {
         WinAspiHandle = LoadLibrary("WNASPI32.DLL");
         if (!WinAspiHandle)
            return SS_FAILED_INIT << 8;

#if defined (WIN64)
         GetASPI32SupportInfo = (unsigned int (*)(void)) GetProcAddress(WinAspiHandle,"GetASPI32SupportInfo");
         SendASPI32Command = (unsigned int (*)(void *)) GetProcAddress(WinAspiHandle,"SendASPI32Command");
#else
         GetASPI32SupportInfo = (DWORD (*)(void)) GetProcAddress(WinAspiHandle,"GetASPI32SupportInfo");
         SendASPI32Command = (DWORD (*)(void *)) GetProcAddress(WinAspiHandle,"SendASPI32Command");
#endif		 

         if (!GetASPI32SupportInfo || !SendASPI32Command)
            {
            FreeLibrary(WinAspiHandle);
            return SS_FAILED_INIT << 8;
            }

         int rval =  GetASPI32SupportInfo();
         return rval;
         }

      default:
         // unknown platform
         return SS_FAILED_INIT << 8;
      }
   }

void aspi_SendCommand(LPSRB p)
{
	memcpy(&LastSRB,p,sizeof(ScsiRequestBlock));

    SendASPI32Command(p);
}

