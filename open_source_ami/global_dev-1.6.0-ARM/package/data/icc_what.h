/****************************************************************
 **                                                            											 **
 **    (C)Copyright 2006-2009, American Megatrends Inc.        			 **
 **                                                            											 **
 **            All Rights Reserved.                            									 **
 **                                                            											 **
 **        5555 Oakbrook Pkwy Suite 200, Norcross,             					 **
 **                                                            											 **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         				 **
 **                                                            											 **
 ****************************************************************/


/****************************************************************************/
/*! @file icc_what.h
 *  Handles the following issues:
 *  - C or C++ struct wrappers<br>
 *      Assist in wrapping C struct's and extern's for compilation by C++ compilers, 
 *      see #define's ICC_CPP_EXTERN_C_*
 *
 *  - Are we in Debug mode or Release mode?<br>
 *      Different compilers use different definitions to indicate debugging is active,
 *      so #define ICC_DEBUG if any OS specific debug flags are set
 *
 *  - What underlying operating system
 *      - ICC_OS_WINDOWS, and one of
 *          - ICC_OS_WIN_NT
 *          - ICC_OS_WIN_2K
 *      - ICC_OS_UNIX, and one of
 *          - ICC_OS_UCLINUX
 *          - ICC_OS_LINUX
 *          - ICC_OS_HARDHAT
 *
 *  - What kind of program are we building?  
 *    Exactly one of the following #define's is created.
 *      - a  Driver         , then #define ICC_IS_DRIVER
 *      - or Application    , then #define ICC_IS_APPLICATION
 *
 *  - Shared object or normal object?<br>
 *      Decide how to declare a function as external based on the compiler/linker,
 *      see #define ICC_EXTERN
 *
 *  There is no corresponding icc_what.c file
 ****************************************************************************/
#ifndef ICC_WHAT_AM_I_H
#define ICC_WHAT_AM_I_H

/****************************************************************************/
/*! @def ICC_CPP_EXTERN_C_BEGIN  
 *  For C compilations, the #define is blank.  For C++ it is #define'd as 'extern "C" {'
 */
/*! @def ICC_CPP_EXTERN_C_END  
 *  For C compilations, the #define is blank.  For C++ it is #define'd as            '}'
 ****************************************************************************/
#if defined (__cplusplus)                       /* being compiled by a C++ compiler */
    #define ICC_CPP_EXTERN_C_BEGIN extern "C" { 
    #define ICC_CPP_EXTERN_C_END               }
#else                                           /* being compiled by a C compiler */
    #define ICC_CPP_EXTERN_C_BEGIN              
    #define ICC_CPP_EXTERN_C_END
#endif

ICC_CPP_EXTERN_C_BEGIN                  /* make definitions in this file compatible with C++ */

/****************************************************************************
 *  different compilers use different definitions to indicate debugging is active,
 *  so define ICC_DEBUG if any of the OS specific debug flags are set
 ****************************************************************************/
#if  defined(DBG)    ||                 /* specified by some people */      \
     defined(_DEBUG) ||                 /* specified in visual studio */    \
     defined(DEBUG)                     /* yet another choice */
    #define ICC_DEBUG                   /*!< Some operating system specific debug flag is true */
#endif                              

/****************************************************************************
 *  Make a determination about what underlying operating system this is being compiled for
 *  and whether it is an Application or Driver.
 *
 *  - Microsoft Windows NT4     ICC_OS_WIN_NT
 *  - Microsoft Windows 2000    ICC_OS_WIN_2K
 *  - Microsoft Windows         ICC_OS_WINDOWS  (includes all ICC_OS_WIN_* above)
 *
 *  - ucLinux                   ICC_OS_UCLINUX
 *  - RedHat    Linux           ICC_OS_LINUX
 *  - unix      all             ICC_OS_UNIX     (includes all unix variants above)
 *
 * Type of software:
 * - Driver                     ICC_IS_DRIVER
 * - Application                ICC_IS_APPLICATION
 ****************************************************************************/
 
#if  defined(THIS_IS_JUST_A_DUMMY_DEFINE)     /* ...so that the others are all #elif */
    #error "ICC_What.h: THIS_IS_JUST_A_DUMMY_DEFINE is actually defined"

/* ------------------------------------------------------------------------*/

#elif defined(_DOXYGEN)                     /* just trying to generate documentation */
    #define ICC_IS_DRIVER                   /*!< Is a driver     , not a application  */
    #define ICC_IS_APPLICATION              /*!< Is a application, not a driver  */

    #define ICC_OS_WINDOWS                  /*!< Generically Windows */
    #define ICC_OS_WIN_NT                   /*!< Is exactly NT4 */
    #define ICC_OS_WIN_2K                   /*!< Is exactly W2K */

    #define ICC_OS_UNIX                     /*!< Generically unix */
    #define ICC_OS_UCLINUX                  /*!< Is specifically ucLinux */
    #define ICC_OS_LINUX                    /*!< Is specifically Linux */
    #define ICC_OS_MACH                     /*!< Is specifically Mac */

/* ------------------------------------------------------------------------*/

#elif defined(_IDWBUILD)                    /* automatically specified by WinNT driver compiler */
    #define ICC_IS_DRIVER
    #define ICC_OS_WIN_NT                   /* is exactly NT4, not W2K */
    #define ICC_OS_WINDOWS                  /* but is also generically Windows */

/* ------------------------------------------------------------------------*/

#elif defined(NTVERSION)                    /* automatically specified by win2000 driver environment */
    #define ICC_IS_DRIVER                   /* ...actually its required for NuMega DriverWorks */
    #define ICC_OS_WIN_2K                   /* is exactly W2K, not NT4 */
    #define ICC_OS_WINDOWS                  /* but is also generically Windows */

/* ------------------------------------------------------------------------*/

#elif defined(_MSC_VER)                     /* automatically specified by VisualStudio compiler */
    #define ICC_OS_WINDOWS                  /* is generic Windows */
    #define ICC_IS_APPLICATION              /* assuming its an application */

/* ------------------------------------------------------------------------*/

#elif defined(__uClinux__)                  /* (probably) manually specified */
    #define ICC_OS_UNIX                     /* some flavor of unix */
    #define ICC_OS_UCLINUX                  /* specifically its ucLinux */
    #define ICC_IS_APPLICATION              /* assume app, don't know how we do drivers */

/* ------------------------------------------------------------------------*/

#elif defined(__linux__)                    /* automatically specified by Linux compiler */
    #define ICC_OS_UNIX                     /* some flavor of unix */
    #define ICC_OS_LINUX                    /* specifically its Linux */
    #ifdef __KERNEL__                       /* manually specified (but mandatory) for driver development */
        #define ICC_IS_DRIVER
    #else
        #define ICC_IS_APPLICATION
    #endif
#elif defined(__MACH__)                    /* automatically specified by Mac compiler */
    #define ICC_OS_MACH                    /* some flavor of mac */
    #ifdef __KERNEL__                       /* manually specified (but mandatory) for driver development */
        #define ICC_IS_DRIVER
    #else
        #define ICC_IS_APPLICATION
    #endif

/* ------------------------------------------------------------------------*/


#else
    
    #error "ICC_What.h: unknown OS"

#endif                                /* #if defined THIS_IS_JUST_A_DUMMY_DEFINE */

/***************************************************************************
Platform checkers
Find if X86 
Find if PPC
These are the platfomr detections for now
Extend as needed
***************************************************************************/

#if defined(ICC_OS_LINUX)
 #if defined(__PPC__)
   #define ICC_PLATFORM_PPC 
   #define ICC_PLATFORM_BIG_ENDIAN 
 #elif defined(__i386__) || defined(__amd64__) || defined(__x86_64__)
   #define ICC_PLATFORM_X86 
   #define ICC_PLATFORM_LITTLE_ENDIAN
 #elif defined(__arm__)
   #define ICC_PLATFORM_ARM
   #if defined(__ARMEL__)
   	#define ICC_PLATFORM_LITTLE_ENDIAN
   #elif defined(__ARMEB__)
   	#define ICC_PLATFORM_BIG_ENDIAN
   #else
	#error "Cannot determine Endianess of ARM !!!!"
   #endif
 #elif defined(__mips__)
   #define ICC_PLATFORM_MIPS
   #if defined(__MIPSEL__)
   	#define ICC_PLATFORM_LITTLE_ENDIAN
   #elif defined(__MIPSEB__)
   	#define ICC_PLATFORM_BIG_ENDIAN
   #else
	#error "Cannot determine Endianess of MIPS !!!!"
   #endif
 #elif defined(__SH4__) || defined (__SH3__)  || defined (__SH4A__)  || defined (__sh__) 
   #define ICC_PLATFORM_SH
   #define ICC_PLATFORM_LITTLE_ENDIAN
 #else
   #error "Encountered Linux on Unknown Platform..dont know what to do..who are you?"
 #endif
#elif defined(ICC_OS_MACH)
    #if defined(__PPC__)
        #define ICC_PLATFORM_PPC 
        #define ICC_PLATFORM_BIG_ENDIAN 
    #elif defined(__i386__) || defined(__amd64__) || defined(__x86_64__)
        #define ICC_PLATFORM_X86 
        #define ICC_PLATFORM_LITTLE_ENDIAN
    #elif defined(__arm__)
        #define ICC_PLATFORM_ARM
    #if defined(__ARMEL__)
        #define ICC_PLATFORM_LITTLE_ENDIAN
    #elif defined(__ARMEB__)
        #define ICC_PLATFORM_BIG_ENDIAN
    #else
        #error "Cannot determine Endianess of ARM !!!!"
    #endif
    #elif defined(__mips__)
        #define ICC_PLATFORM_MIPS
    #if defined(__MIPSEL__)
        #define ICC_PLATFORM_LITTLE_ENDIAN
    #elif defined(__MIPSEB__)
        #define ICC_PLATFORM_BIG_ENDIAN
    #else
        #error "Cannot determine Endianess of MIPS !!!!"
    #endif
    #elif defined(__SH4__) || defined (__SH3__)  || defined (__SH4A__)  || defined (__sh__) 
       #define ICC_PLATFORM_SH
       #define ICC_PLATFORM_LITTLE_ENDIAN
    #else
       #error "Encountered MACH on Unknown Platform..dont know what to do..who are you?"
    #endif
#elif defined(ICC_OS_WINDOWS)
 #if defined(_M_IX86) || defined (_M_X64)
   #define ICC_PLATFORM_X86 
   #define ICC_PLATFORM_LITTLE_ENDIAN
 #else
   #error "Encountered Windows OS on non x86 platform..dont know what to do..who are you?"
 #endif
#else
  #error "Non Windows or Linux or Mach compilation in platform detection!!"
#endif

 


/****************************************************************************/
/*! @def ICC_EXTERN
 *  Based on the type of program being built:
 *  - Windows DLL's        will export the function declaration with '__declspec(dllexport)'
 *  - Windows applications will import the function declaration with '__declspec(dllimport)', 
 *              but the application must #define ICC_EXTERN_IMPORT for this to occur
 *  - Windows Libraries    will export the function declaration with a normal ansi-c 'extern'
 *  - Unix programs will use a normal ansi-c 'extern' and can create shared objects and apps
 ****************************************************************************/
#ifdef _MSC_VER                                     /* specified by Visual C compiler */

    #if   defined(_USRDLL)                          /* creating a *.dll file */
         #define ICC_EXTERN __declspec(dllexport)   /* so make function visible */

    #elif defined(ICC_EXTERN_IMPORT)                /* creating a application to use the *.dll */
         #define ICC_EXTERN __declspec(dllimport)   /* so reference the function */

    #elif defined(_LIB)                             /* creating a *.lib file */
         #define ICC_EXTERN extern

    #endif
#endif

#if !defined(ICC_EXTERN)                            /* normal compiler or 'normal' microsoft */
    #define  ICC_EXTERN extern                      /* ...so do the normal ansi-c behaviour */
#endif                                      

/*****************************************************************************
 *
 *  sanity checking
 *
 ****************************************************************************/
#if  defined(ICC_IS_DRIVER) &&  defined(ICC_IS_APPLICATION)
    #error "ICC_What.h: both driver and application declared"
#endif

#if !defined(ICC_IS_DRIVER) && !defined(ICC_IS_APPLICATION)
    #error "ICC_What.h: neither driver nor application declared"
#endif

#if !defined(ICC_PLATFORM_X86) && !defined(ICC_PLATFORM_PPC) && !defined(ICC_PLATFORM_ARM)&& !defined(ICC_PLATFORM_MIPS)  && !defined(ICC_PLATFORM_SH)
    #error "Neither PPC nor X86 nor ARM nor MIPS nor SH is defined"
#endif

#if !defined(ICC_PLATFORM_BIG_ENDIAN) && !defined(ICC_PLATFORM_LITTLE_ENDIAN)
    #error "Neither big nor little endian detected"
#endif

#if defined(ICC_PLATFORM_BIG_ENDIAN) && defined(ICC_PLATFORM_LITTLE_ENDIAN)
    #error "Both Big endian and little endian defined"
#endif


ICC_CPP_EXTERN_C_END
#endif /* ICC_WHAT_AM_I_H */
