/*****************************-*- andrew-c -*-********************************/
/* Filename:    ipmb_interface.h                                             */
/* Author:      Andrew McCallum (andrewm@ami.com)                            */
/* Created:     10/06/2003                                                   */
/* Modified:    12/14/2005 took from G3 libipmi (Rajasekhar)                 */
/* Description: Headers for use inside libipmi only.  Prototypes for the     */
/*              send/receive commands.                                       */
/*****************************************************************************/

/** \file ipmb_interface.h
    \brief Send/Receive command prototypes for IPMB IPMI commands
*/

#ifndef IPMB_INTERFACE_H
#define IPMB_INTERFACE_H

#include "coreTypes.h"

int ipmi_send_command( uint8 bus, uint8 slave, uint8 NetFnLUN,
                       uint8 *command, size_t cmd_len, int i2cfd );


int ipmi_recv_response( int seq_num, uint8 *buffer, size_t buf_len, int i2cfd );


uint16 ipmi_generic_command( uint8 bus, uint8 slave, uint8 NetFnLUN, uint8 *command,
                          size_t cmd_len, uint8 *response_buffer,
                          size_t *response_len );


#endif
