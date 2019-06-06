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
* Filename: ipmb_private.h
*
* Descriptions: Private IPMI related functions and defines.  Nobody should
*   use this stuff outside of libipmi.
*
* Author: Andrew McCallum
*
******************************************************************/
#ifndef LIBIPMI_PRIVATE_H
#define LIBIPMI_PRIVATE_H

/* Location of the file containing the most recent sequence number */
#define SEQ_NUM_FILE           "/var/state/i2c_seq_num%d"

/* The number of times we will try to send a command and get a response */
/* before we give up, and allow the error to filter up to the caller    */
#define IPMI_COMMAND_RETRIES    ( 3 )

#endif
