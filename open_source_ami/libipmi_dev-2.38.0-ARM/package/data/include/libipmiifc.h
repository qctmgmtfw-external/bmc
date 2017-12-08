/**
 * @file   libipmiifc.h
 * @author Rajasekhar (rajasekharj@amiindia.co.in)
 * @date   02-Sep-2004
 *
 * @brief  Contains definitions for exporting the functions
 *			
 */

#ifndef __LIBIPMI_IFC_H__
#define __LIBIPMI_IFC_H__

#ifdef _MSC_VER     /* automatically specified by Visual C compiler */
    #ifdef LIBIPMI_EXPORTS
    #define LIBIPMI_API __declspec(dllexport)
    #elif LIBIPMI_IMPORTS
    #define LIBIPMI_API __declspec(dllimport)
	#else
    #define LIBIPMI_API
    #endif
#else
    #define LIBIPMI_API extern
#endif



#endif



