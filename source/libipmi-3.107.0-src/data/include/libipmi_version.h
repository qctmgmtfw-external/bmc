#ifndef __LIBIPMI_VERSION_H__
#define __LIBIPMI_VERSION_H__

/*
Version - 1.1
Date - 14-Dec-2005
Changes:
-Added IPMB Interface.
-Added LIBIPMI_HL_GetSDR in libipmi_sdr.c, a high level function to retreive SDRs based on medium type. In IPMB medium, SDRs must be retreived in chunks whereas in other mediums they can be retreived in a single stretch.
-Changed LIBIPMI_GetCompleteSDR to use LIBIPMI_HL_GetSDR instead of IPMICMD_GetSDR.
-Version information added.
-Fix: In LIBIPMI_SetUser, expected response length field was unassigned. It is corrected.
*/

#define LIBIPMI_VERSION		"1.1"

#endif



