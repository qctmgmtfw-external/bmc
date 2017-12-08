/****************************************************************
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2010-2011, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        6145-F, Northbelt Parkway, Norcross,                **
 **                                                            **
 **        Georgia - 30071, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 ****************************************************************/
/*****************************************************************
 *
 * libEvnetLogDes.h
 * Gives Event Related Information
 *****************************************************************/
 
#include "IPMI_SEL.h"
#include "smtpclient.h"

#define EVNTREAD_THRESH    1
#define EVNTREAD_DISCRETE  7
#define SENSOR_SPECIFIC    0x6F

typedef struct
{
    INT8U SenId;
    char *SenDesp;
}SenType_T;

typedef struct
{
    INT8U SensorType;
    int Offset;
    char* Desp;
}EventType_T;

extern void FindSensorType (SELEventRecord_T *pEventRecord, char *SensorType);
extern void SELLog (SELEventRecord_T *pEventRecord, char *SELerror);
extern void  AlertSeverityLevel (SELEventRecord_T* pEvtRecord,char *Buffer);



