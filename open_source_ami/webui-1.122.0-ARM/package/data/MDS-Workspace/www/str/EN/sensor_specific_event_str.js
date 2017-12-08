//;*****************************************************************;
//;*****************************************************************;
//;**                                                             **;
//;**     (C) COPYRIGHT American Megatrends Inc. 2008-2009        **;
//;**                     ALL RIGHTS RESERVED                     **;
//;**                                                             **;
//;**  This computer software, including display screens and      **;
//;**  all related materials, are confidential and the            **;
//;**  exclusive property of American Megatrends, Inc.  They      **;
//;**  are available for limited use, but only pursuant to        **;
//;**  a written license agreement distributed with this          **;
//;**  computer software.  This computer software, including      **;
//;**  display screens and all related materials, shall not be    **;
//;**  copied, reproduced, published or distributed, in whole     **;
//;**  or in part, in any medium, by any means, for any           **;
//;**  purpose without the express written consent of American    **;
//;**  Megatrends, Inc.                                           **;
//;**                                                             **;
//;**                                                             **;
//;**                American Megatrends, Inc.                    **;
//;**           5555 Oakbook Parkway, Building 200                **;
//;**     Norcross,  Georgia - 30071, USA. Phone-(770)-246-8600.  **;
//;**                                                             **;
//;*****************************************************************;
//;*****************************************************************;

/*************************************** Sensor specific event codes ******************************************/
//eLang.sensor_specific_event_strings[SENSOR TYPE CODE][SENSOR-SPECIFIC OFFSET] = "EVENT DESCRIPTION";
/*********************************************************************************************************************/
eLang.sensor_specific_event_strings = new Array();
eLang.sensor_specific_event_strings[0] = "reserved";
eLang.sensor_specific_event_strings[1] = "Temperature";
eLang.sensor_specific_event_strings[2] = "Voltage";
eLang.sensor_specific_event_strings[3] = "Current";
eLang.sensor_specific_event_strings[4] = "Fan";

eLang.sensor_specific_event_strings[5] = new Array();
eLang.sensor_specific_event_strings[5][0] =      "General Chassis Intrusion";
eLang.sensor_specific_event_strings[5][1] =      "Drive Bay Intrusion";
eLang.sensor_specific_event_strings[5][2] =      "I/O Card Area Intrusion";
eLang.sensor_specific_event_strings[5][3] =      "Processor Area Intrusion";
eLang.sensor_specific_event_strings[5][4] =      "LAN Leash Lost (System unplugged from LAN)";
eLang.sensor_specific_event_strings[5][5] =      "Unauthorized Dock/Undock";
eLang.sensor_specific_event_strings[5][6] =      "Fan Area Intrusion";


eLang.sensor_specific_event_strings[6] = new Array();
eLang.sensor_specific_event_strings[6][0] =      "Secure Mode Violation Attempt";
eLang.sensor_specific_event_strings[6][1] =      "Pre-boot Password Violation - user password";
eLang.sensor_specific_event_strings[6][2] =      "Pre-boot Password Violation Attempt - setup password";
eLang.sensor_specific_event_strings[6][3] =      "Pre-boot Password Violation - network boot password";
eLang.sensor_specific_event_strings[6][4] =      "Other Pre-boot Password Violation";
eLang.sensor_specific_event_strings[6][5] =      "Out-of-Band Access Password Violation";

eLang.sensor_specific_event_strings[7] = new Array();
eLang.sensor_specific_event_strings[7][0] =      "IERR";
eLang.sensor_specific_event_strings[7][1] =      "Thermal Trip";
eLang.sensor_specific_event_strings[7][2] =      "FRB1/BIST Failure";
eLang.sensor_specific_event_strings[7][3] =      "FRB2/Hang in POST Faliure";
eLang.sensor_specific_event_strings[7][4] =      "FRB3/Processor Startup/Initialization Failure";
eLang.sensor_specific_event_strings[7][5] =      "Configuration Error (DMI)";
eLang.sensor_specific_event_strings[7][6] =      "SM BIOS 'Uncorrectable CPU-complex Error";
eLang.sensor_specific_event_strings[7][7] =      "Processor Presence Detected";
eLang.sensor_specific_event_strings[7][8] =      "Processor Disabled";
eLang.sensor_specific_event_strings[7][9] =      "Terminator Presence Detected";
eLang.sensor_specific_event_strings[7][10] =    "Processor Automatically Throttled";

eLang.sensor_specific_event_strings[8] = new Array();
eLang.sensor_specific_event_strings[8][0] =      "Presence Detected";
eLang.sensor_specific_event_strings[8][1] =      "Power Supply Failure Detected";
eLang.sensor_specific_event_strings[8][2] =      "Predictive Failure Asserted";
eLang.sensor_specific_event_strings[8][3] =      "Power Supply Input Lost (AC/DC)";
eLang.sensor_specific_event_strings[8][4] =      "Power Supply Input Lost or Out of Range";
eLang.sensor_specific_event_strings[8][5] =      "Power Supply Input Out of Range, but Present";
eLang.sensor_specific_event_strings[8][6] =      "Configuration Error";

eLang.sensor_specific_event_strings[9] = new Array();
eLang.sensor_specific_event_strings[9][0] =      "Power Off / Power Down";
eLang.sensor_specific_event_strings[9][1] =      "Power Cycle";
eLang.sensor_specific_event_strings[9][2] =      "240VA Power Down";
eLang.sensor_specific_event_strings[9][3] =      "Interlock Power Down";
eLang.sensor_specific_event_strings[9][4] =      "AC Lost";
eLang.sensor_specific_event_strings[9][5] =      "Soft Power Control Failure";
eLang.sensor_specific_event_strings[9][6] =      "Power Unit Failure Detected";
eLang.sensor_specific_event_strings[9][7] =      "Predictive Failure";

eLang.sensor_specific_event_strings[12] = new Array();
eLang.sensor_specific_event_strings[12][0] =      "Correctable ECC";
eLang.sensor_specific_event_strings[12][1] =      "Uncorrectable ECC";
eLang.sensor_specific_event_strings[12][2] =      "Parity";
eLang.sensor_specific_event_strings[12][3] =      "Memory Scrub Failure";
eLang.sensor_specific_event_strings[12][4] =      "Memory Device Disabled";
eLang.sensor_specific_event_strings[12][5] =      "Correctable ECC Logging Limit Reached";
eLang.sensor_specific_event_strings[12][6] =      "Presence Detected";
eLang.sensor_specific_event_strings[12][7] =      "Configuration Error";
eLang.sensor_specific_event_strings[12][8] =      "Spare";

eLang.sensor_specific_event_strings[15] = new Array();
eLang.sensor_specific_event_strings[15][0] =      "Error";
eLang.sensor_specific_event_strings[15][1] =      "Hang";
eLang.sensor_specific_event_strings[15][2] =      "Progress";



eLang.sensor_specific_event_strings[16] = new Array();
eLang.sensor_specific_event_strings[16][0] =      "Correctable Memory Error Logging Disabled";
eLang.sensor_specific_event_strings[16][1] =      "Event 'Type' Logging Disabled";
eLang.sensor_specific_event_strings[16][2] =      "Log Area Reset/Cleared";
eLang.sensor_specific_event_strings[16][3] =      "All Event Logging Disabled";
eLang.sensor_specific_event_strings[16][4] =      "SEL Full";
eLang.sensor_specific_event_strings[16][5] =      "SEL Almost Full";

eLang.sensor_specific_event_strings[17] = new Array();
eLang.sensor_specific_event_strings[17][0] =      "BIOS Watchdog Reset";
eLang.sensor_specific_event_strings[17][1] =      "OS Watchdog Reset";
eLang.sensor_specific_event_strings[17][2] =      "OS Watchdog Shutdown";
eLang.sensor_specific_event_strings[17][3] =      "OS Watchdog Power Down";
eLang.sensor_specific_event_strings[17][4] =      "OS Watchdog Power Cycle";
eLang.sensor_specific_event_strings[17][5] =      "OS Watchdog NMI";
eLang.sensor_specific_event_strings[17][6] =      "OS Watchdog Expired";
eLang.sensor_specific_event_strings[17][7] =      "OS Watchdog Pre-timeout Interrupt, non-NMI";

eLang.sensor_specific_event_strings[18] = new Array();
eLang.sensor_specific_event_strings[18][0] =      "System Reconfigured";
eLang.sensor_specific_event_strings[18][1] =      "OEM System Boot Event";
eLang.sensor_specific_event_strings[18][2] =      "Undetermined System Hardware Failure";
eLang.sensor_specific_event_strings[18][3] =      "Entry Added to Auxiliary Log";
eLang.sensor_specific_event_strings[18][4] =      "PEF Action";

eLang.sensor_specific_event_strings[19] = new Array();
eLang.sensor_specific_event_strings[19][0] =      "Front Panel NMI";
eLang.sensor_specific_event_strings[19][1] =      "Bus Timeout";
eLang.sensor_specific_event_strings[19][2] =      "I/O Channel Check NMI";
eLang.sensor_specific_event_strings[19][3] =      "Software NMI";
eLang.sensor_specific_event_strings[19][4] =      "PCI PERR";
eLang.sensor_specific_event_strings[19][5] =      "PCI SERR";
eLang.sensor_specific_event_strings[19][6] =      "EISA Fail Safe Timeout";
eLang.sensor_specific_event_strings[19][7] =      "Bus Correctable Error";
eLang.sensor_specific_event_strings[19][8] =      "Bus Uncorrectable Error";
eLang.sensor_specific_event_strings[19][9] =      "Fatal NMI";
eLang.sensor_specific_event_strings[19][10] =    "Bus Fatal Error";

eLang.sensor_specific_event_strings[20] = new Array();
eLang.sensor_specific_event_strings[20][0] =      "Power Button Pressed";
eLang.sensor_specific_event_strings[20][1] =      "Sleep Button Pressed";
eLang.sensor_specific_event_strings[20][2] =      "Reset Button Pressed";
eLang.sensor_specific_event_strings[20][3] =      "FRU Latch Open";
eLang.sensor_specific_event_strings[20][4] =      "FRU Service Request Button";

eLang.sensor_specific_event_strings[25] = new Array();
eLang.sensor_specific_event_strings[25][0] =      "Soft Power Control Failure";

eLang.sensor_specific_event_strings[29] = new Array();
eLang.sensor_specific_event_strings[29][0] =      "Initiated By Power Up";
eLang.sensor_specific_event_strings[29][1] =      "Initiated By Hard Reset";
eLang.sensor_specific_event_strings[29][2] =      "Initiated By Warm Reset";
eLang.sensor_specific_event_strings[29][3] =      "User Requested PXE Boot";
eLang.sensor_specific_event_strings[29][4] =      "Automatic Boot to Diagnostic";

eLang.sensor_specific_event_strings[30] = new Array();
eLang.sensor_specific_event_strings[30][0] =      "No Bootable Media";
eLang.sensor_specific_event_strings[30][1] =      "Non-Bootable Diskette Left in Drive";
eLang.sensor_specific_event_strings[30][2] =      "PXE Server Not Found";
eLang.sensor_specific_event_strings[30][3] =      "Invalid Boot Sector";
eLang.sensor_specific_event_strings[30][4] =      "Timeout Waiting For User Selection of Boot Source";

eLang.sensor_specific_event_strings[31] = new Array();
eLang.sensor_specific_event_strings[31][0] =      "A: Boot Completed";
eLang.sensor_specific_event_strings[31][1] =      "C: Boot Completed";
eLang.sensor_specific_event_strings[31][2] =      "PXE Boot Completed";
eLang.sensor_specific_event_strings[31][3] =      "Diagnostic Boot Completed";
eLang.sensor_specific_event_strings[31][4] =      "CD-ROM Boot Completed";
eLang.sensor_specific_event_strings[31][5] =      "ROM Boot Completed";
eLang.sensor_specific_event_strings[31][6] =      "Boot Completed - Boot Device Not Specified";

eLang.sensor_specific_event_strings[32] = new Array();
eLang.sensor_specific_event_strings[32][0] =      "Stop During OS Load / Initialization";
eLang.sensor_specific_event_strings[32][1] =      "Run-Time Stop";

eLang.sensor_specific_event_strings[33] = new Array();
eLang.sensor_specific_event_strings[33][0] =      "Fault Status Asserted";
eLang.sensor_specific_event_strings[33][1] =      "Identify Status Asserted";
eLang.sensor_specific_event_strings[33][2] =      "Slot/Connector Device Installed/Attached";
eLang.sensor_specific_event_strings[33][3] =      "Slot/Connector Ready For Device Installation";
eLang.sensor_specific_event_strings[33][4] =      "Slot/Connector Ready For Device Removal";
eLang.sensor_specific_event_strings[33][5] =      "Slot Power is Off";
eLang.sensor_specific_event_strings[33][6] =      "Slot/Connector Device Removal Request";
eLang.sensor_specific_event_strings[33][7] =      "Interlock Asserted";
eLang.sensor_specific_event_strings[33][8] =      "Slot is Disabled";
eLang.sensor_specific_event_strings[33][9] =      "Slot Holds Spare Device";

eLang.sensor_specific_event_strings[34] = new Array();
eLang.sensor_specific_event_strings[34][0] =      "S0/G0 'Working";
eLang.sensor_specific_event_strings[34][1] =      "S1 'Sleeping With System HW & Processor Context Maintained'";
eLang.sensor_specific_event_strings[34][2] =      "S2 'Sleeping, Processor Context Lost'";
eLang.sensor_specific_event_strings[34][3] =      "S3 'Sleeping, Processor & HW Context Lost, Memory Retained'";
eLang.sensor_specific_event_strings[34][4] =      "S4 'Non-Volatile Sleep/Suspend To Disk'";
eLang.sensor_specific_event_strings[34][5] =      "S5/G2 'Soft Off'";
eLang.sensor_specific_event_strings[34][6] =      "S4/S5 Soft Off, Particular S4/S5 State Cannot Be Determined";
eLang.sensor_specific_event_strings[34][7] =      "G3 / Mechanical Off";
eLang.sensor_specific_event_strings[34][8] =      "Sleeping in S1, S2, or S3";
eLang.sensor_specific_event_strings[34][9] =      "G1 Sleeping";
eLang.sensor_specific_event_strings[34][10] =      "S5 Entered By Override";
eLang.sensor_specific_event_strings[34][11] =      "Legacy ON State";
eLang.sensor_specific_event_strings[34][12] =      "Legacy OFF State";
eLang.sensor_specific_event_strings[34][13] =      "Unknown";

eLang.sensor_specific_event_strings[35] = new Array();
eLang.sensor_specific_event_strings[35][0] =      "Timer Expired";
eLang.sensor_specific_event_strings[35][1] =      "Hard Reset";
eLang.sensor_specific_event_strings[35][2] =      "Power Down";
eLang.sensor_specific_event_strings[35][3] =      "Power Cycle";
eLang.sensor_specific_event_strings[35][4] =      "Reserved";
eLang.sensor_specific_event_strings[35][5] =      "Reserved";
eLang.sensor_specific_event_strings[35][6] =      "Reserved";
eLang.sensor_specific_event_strings[35][7] =      "Reserved";
eLang.sensor_specific_event_strings[35][8] =      "Timer Interrupt";

eLang.sensor_specific_event_strings[36] = new Array();
eLang.sensor_specific_event_strings[36][0] =      "Platform Generated Page";
eLang.sensor_specific_event_strings[36][1] =      "Platform Generated LAN Alert";
eLang.sensor_specific_event_strings[36][2] =      "Platform Event Trap Generated";
eLang.sensor_specific_event_strings[36][3] =      "Platform Generated SNMP Trap, OEM Format";

eLang.sensor_specific_event_strings[37] = new Array();
eLang.sensor_specific_event_strings[37][0] =      "Entity Present";
eLang.sensor_specific_event_strings[37][1] =      "Entity Absent";
eLang.sensor_specific_event_strings[37][2] =      "Entity Disabled";

eLang.sensor_specific_event_strings[39] = new Array();
eLang.sensor_specific_event_strings[39][0] =      "LAN Heartbeat Lost";
eLang.sensor_specific_event_strings[39][1] =      "LAN Heartbeat";

eLang.sensor_specific_event_strings[40] = new Array();
eLang.sensor_specific_event_strings[40][0] =      "Sensor Access Degraded or Unavailable";
eLang.sensor_specific_event_strings[40][1] =      "Controller Access Degraded or Unavailable";
eLang.sensor_specific_event_strings[40][2] =      "Management Controller Offline";
eLang.sensor_specific_event_strings[40][3] =      "Management Controller Unavailable";

eLang.sensor_specific_event_strings[41] = new Array();
eLang.sensor_specific_event_strings[41][0] =      "Battery Low";
eLang.sensor_specific_event_strings[41][1] =      "Battery Failed";
eLang.sensor_specific_event_strings[41][2] =      "Battery Presence Detected";

eLang.sensor_specific_event_strings[42] = new Array();
eLang.sensor_specific_event_strings[42][0] =      "Session Activated";
eLang.sensor_specific_event_strings[42][1] =      "Session Deactivated";

eLang.sensor_specific_event_strings[43] = new Array();
eLang.sensor_specific_event_strings[43][0] =      "Hardware Change Detected";
eLang.sensor_specific_event_strings[43][1] =      "Firmware / Software Change Detected";
eLang.sensor_specific_event_strings[43][2] =      "Hardware Incompatibility Detected";
eLang.sensor_specific_event_strings[43][3] =      "Firmware / Software Incompatibility Detected";
eLang.sensor_specific_event_strings[43][4] =      "Invalid or Unsupported Hardware Version";
eLang.sensor_specific_event_strings[43][5] =      "Invalid or Unsupported Firmware / Software Version";
eLang.sensor_specific_event_strings[43][6] =      "Hardware Change Detected was successful";
eLang.sensor_specific_event_strings[43][7] =      "Firmware / Software Change Detected was successful";

eLang.sensor_specific_event_strings[44] = new Array();
eLang.sensor_specific_event_strings[44][0] =      "FRU Not Installed";
eLang.sensor_specific_event_strings[44][1] =      "FRU Inactive";
eLang.sensor_specific_event_strings[44][2] =      "FRU Activation Requested";
eLang.sensor_specific_event_strings[44][3] =      "FRU Activation In Progress";
eLang.sensor_specific_event_strings[44][4] =      "FRU Active";
eLang.sensor_specific_event_strings[44][5] =      "FRU Deactivation Requested";
eLang.sensor_specific_event_strings[44][6] =      "FRU Deactivation In Progress";
eLang.sensor_specific_event_strings[44][7] =      "FRU Communication Lost";
