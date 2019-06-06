/* ***************************-*- andrew-c -*-****************************** */
/* Filename:    libsensor.h                                                  */
/* Author:      Andrew McCallum (andrewm@ami.com)                            */
/* Format:      Indent: 4 spaces, no tabs                                    */
/* Created:     09/21/2004                                                   */
/* Modified:    09/21/2004                                                   */
/* Description: Prototypes and defines for sensor helper functions           */
/* ************************************************************************* */

#ifndef LIBSENSOR_H_
#define LIBSENSOR_H_

#include "libipmi_session.h"

/* IPMI threshold state definitions for monitoring */
#define THRESH_UNINITIALIZED        ( (u16)0x00 )
#define THRESH_NORMAL               ( (u16)0x01 )
#define THRESH_UP_NONCRIT           ( (u16)0x02 )
#define THRESH_UP_CRITICAL          ( (u16)0x04 )
#define THRESH_LOW_NONCRIT          ( (u16)0x08 )
#define THRESH_LOW_CRITICAL         ( (u16)0x10 )
#define THRESH_ACCESS_FAILED        ( (u16)0x20 )
#define THRESH_UP_NON_RECOV         ( (u16)0x40 )
#define THRESH_LOW_NON_RECOV        ( (u16)0x80 )

//#pragma pack(1)
struct sensor_data
{
	/*
		sensor_name array length increased from 17 to 20.
		In some cases sensor_name length is more than 17 characters. webgo was giving some special character to web page in those cases and webpage was giving some weird behaviour when it found that special character.
	*/
    UINT8 sensor_num;
    char sensor_name[ 20 ];
    UINT8 owner_id;
    UINT8 owner_lun;
    UINT8 sensor_type;
    UINT8 sensor_units[ 3 ];
    double sensor_reading;
    UINT8 raw_reading;
    UINT16 sensor_state;
    UINT8 discrete_state;
    double low_non_recov_thresh;
    double low_crit_thresh;
    double low_non_crit_thresh;
    double high_non_crit_thresh;
    double high_crit_thresh;
    double high_non_recov_thresh;
    int SensorAccessibleFlags;
    INT16U Settable_Readable_ThreshMask;
	INT8U sdr_type;//Quanta+
};
//#pragma pack()

int getsensorcount( IPMI20_SESSION_T *pSession );
int getallsensorreadings( struct sensor_data *sensors, IPMI20_SESSION_T *pSession );


#endif
