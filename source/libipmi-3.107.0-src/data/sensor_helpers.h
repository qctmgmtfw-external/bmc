/*****************************************************************
******************************************************************
***                                                            ***
***        (C)Copyright 2008, American Megatrends Inc.         ***
***                                                            ***
***                    All Rights Reserved                     ***
***                                                            ***
***       5555 Oakbrook Parkway, Norcross, GA 30093, USA       ***
***                                                            ***
***                     Phone 770.246.8600                     ***
***                                                            ***
******************************************************************
******************************************************************
******************************************************************
* 
* Filename: sensor_helpers.h
*
* Description: Prototypes for sensor helper functions 
*
* Author: Andrew McCallum
*
******************************************************************/
#ifndef SENSOR_HELPERS_H_
#define SENSOR_HELPERS_H_

#include "libipmi_sensor.h"

int ipmi_convert_reading( u8 *sdr_buffer, u8 raw_reading, double *converted_reading );
int read_sensor_sdrs( IPMI20_SESSION_T *pSession, struct sensor_info **sensor_list,
					  int timeout );
int get_sdr_sensor_state( u8 sensor_reading, u8 *sdr_buffer, u16 *current_state );
int GetSensorState(IPMI20_SESSION_T *pSession, INT8U sensor_reading, u8 *sdr_buffer, u16 *current_state, int timeout  );
int ipmi_conv_reading(u8 SDRType, u8 raw_reading, double *converted_reading ,u8 Min,u8 Max,u8 Units1,u8 Lin,u8 Mval,u8 Bval,u8 M_Tol,u8 B_Acc,u8 R_B_Ex);
int SensorState( INT8U sensor_reading, u16 *current_state,u8 SDRType,u8 Lin,u8 Units1,u8 EvtType,u8 AssertEvt1,u8 AssertEvt2,u8 DeassertEvt1,u8 DeassertEvt2);
unsigned int sdr_convert_sensor_value_to_raw(FullSensorRec_T	*sdr_record, double val);
#endif
