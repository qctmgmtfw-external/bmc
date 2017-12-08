/*****************************************************************
 **                                                             **
 **     (C) Copyright 2006-2009, American Megatrends Inc.       **
 **                                                             **
 **             All Rights Reserved.                            **
 **                                                             **
 **         5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                             **
 **         Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                             **
 ****************************************************************/

/**
 * Avse.h
 * Adaptive Video Server Engine
**/

#include "session_data.h"

#ifndef AVSE_H
#define AVSE_H

/**
 * @fn avse_main
 * Adaptive Video Server Engine main thread.
**/
void avse_main (void);

/**  
 * @fn cmdhndlr_main  
 * Cmdhndlr Main thread - Handles commands from Client  
 **/  
void cmdhndlr_main (void);  

/**  
 * @fn videorecord_main  
 * Video Record Main thread - Handles the auto video recording
 **/  
void videorecord_main (void);  

void
send_stop_signal (session_list_t* send_list, session_list_t* session_list);

void close_descriptors();
void close_webtoken_pipe (int fd);
int ReleaseResources (void);
#endif	/* AVSE_H */

