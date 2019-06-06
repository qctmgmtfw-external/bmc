/*****************************-*- andrew-c -*-********************************/
/* Filename:    ipmi_utils.h                                                 */
/* Author:      Andrew McCallum (andrewm@ami.com)                            */
/* Created:     10/06/2003                                                   */
/* Modified:    12/14/2005 took from G3 libipmi (Rajasekhar)                 */
/* Description: Prototypes for functions useful inside of libipmi, but not   */
/*              outside of it.  These are basic utility functions used to    */
/*              facilitate IPMI commands and separate distinct bits of code. */
/*****************************************************************************/

/** \file ipmi_utils.h
 *  \brief Utility functions for working with IPMI
 *
 *  Prototypes for functions useful inside of libipmi, but not outside of it.
 *  These are basic utility functions used to facilitate IPMI commands and
 *  separate distinct bits of code.
*/


#ifndef IPMI_UTILS_H
#define IPMI_UTILS_H

#include "coreTypes.h"

extern u8 ipmi_checksum( u8 starting_val, u8 *buffer, size_t buf_len );
extern int get_seq_num( u8 bus );
extern int ipmi_access_lock( u8 bus );
extern int ipmi_access_unlock( int i2cfd );


#endif
