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

/*************************************** Event / Reading type codes *******************************************/
//eLang.event_strings[EVENT / READING TYPE CODE][GENERIC OFFSET] = "DESCRIPTION";
/*********************************************************************************************************************/
eLang.event_strings = new Array();
/******* Threshold Event Type Codes *********/
eLang.event_strings[1] = new Array();
eLang.event_strings[1][0] = "Lower Non-Critical - Going Low";
eLang.event_strings[1][1] = "Lower Non-Critical - Going High";
eLang.event_strings[1][2] = "Lower Critical - Going Low";
eLang.event_strings[1][3] = "Lower Critical - Going High";
eLang.event_strings[1][4] = "Lower Non-Recoverable - Going Low";
eLang.event_strings[1][5] = "Lower Non-Recoverable - Going High";
eLang.event_strings[1][6] = "Upper Non-Critical - Going Low";
eLang.event_strings[1][7] = "Upper Non-Critical - Going High";
eLang.event_strings[1][8] = "Upper Critical - Going Low";
eLang.event_strings[1][9] = "Upper Critical - Going High";
eLang.event_strings[1][10] = "Upper Non-Recoverable - Going Low";
eLang.event_strings[1][11] = "Upper Non-Recoverable - Going High";

/******* Generic Discrete Event Type Codes *********/
eLang.event_strings[2] = new Array();
eLang.event_strings[2][0] =      "Transition to Idle";
eLang.event_strings[2][1] =      "Transition to Active";
eLang.event_strings[2][2] =      "Transition to Busy";

/******* Digital Discrete Event Type Codes *********/
eLang.event_strings[3] = new Array();
eLang.event_strings[3][0] =      "State Deasserted";
eLang.event_strings[3][1] =      "State Asserted";

eLang.event_strings[4] = new Array();
eLang.event_strings[4][0] =      "Predictive Failure Deasserted";
eLang.event_strings[4][1] =      "Predictive Failure Asserted";

eLang.event_strings[5] = new Array();
eLang.event_strings[5][0] =      "Limit Not Exceeded";
eLang.event_strings[5][1] =      "Limit Exceeded";

eLang.event_strings[6] = new Array();
eLang.event_strings[6][0] =      "Performance Met";
eLang.event_strings[6][1] =      "Performance Lags";

eLang.event_strings[7] = new Array();
eLang.event_strings[7][0] =      "Transition to OK";
eLang.event_strings[7][1] =      "Transition to Non-Critical from OK";
eLang.event_strings[7][2] =      "Transition to Critical from less severe";
eLang.event_strings[7][3] =  	  "Transition to Non-Recoverable from less severe";
eLang.event_strings[7][4] =      "Transition to Non-Critical from more severe";
eLang.event_strings[7][5] =      "Transition to Critical from Non-Recoverable";
eLang.event_strings[7][6] =      "Transition to Non-Recoverable";
eLang.event_strings[7][7] =      "Monitor";
eLang.event_strings[7][8] =      "Informational";

eLang.event_strings[8] = new Array();
eLang.event_strings[8][0] =      "Device Removed / Device Absent";
eLang.event_strings[8][1] =      "Device Inserted / Device Present";

eLang.event_strings[9] = new Array();
eLang.event_strings[9][0] =      "Device Disabled";
eLang.event_strings[9][1] =      "Device Enabled";

eLang.event_strings[10] = new Array();
eLang.event_strings[10][0] =      "Transition to Running";
eLang.event_strings[10][1] =      "Transition to In Test";
eLang.event_strings[10][2] =      "Transition to Power Off";
eLang.event_strings[10][3] =      "Transition to On Line";
eLang.event_strings[10][4] =      "Transition to Off Line";
eLang.event_strings[10][5] =      "Transition to Off Duty";
eLang.event_strings[10][6] =      "Transition to Degraded";
eLang.event_strings[10][7] =      "Transition to Power Save";
eLang.event_strings[10][8] =      "Install Error";

eLang.event_strings[11] = new Array();
eLang.event_strings[11][0] =	     "Fully Redundant (Redundancy Regained)";
eLang.event_strings[11][1] =	     "Redundancy Lost";
eLang.event_strings[11][2] =	     "Redundancy Degraded";
eLang.event_strings[11][3] =	     "Non-redundant: Sufficient Resources from Redundant";
eLang.event_strings[11][4] =	     "Non-redundant: Sufficient Resources from Insufficient Resources";
eLang.event_strings[11][5] =	     "Non-redundant: Insufficient Resources";
eLang.event_strings[11][6] =	     "Redundancy Degraded From Fully Redundant";
eLang.event_strings[11][7] =	     "Redundancy Degraded From Non-redundant";

eLang.event_strings[12] = new Array();
eLang.event_strings[12][0] =	     "D0 Power State";
eLang.event_strings[12][1] =	     "D1 Power State";
eLang.event_strings[12][2] =	     "D2 Power State";
eLang.event_strings[12][3] =	     "D3 Power State";


