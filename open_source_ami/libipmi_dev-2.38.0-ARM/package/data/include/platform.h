
#ifndef __PLATFORM_AND_OS_DETECTION_H__
#define __PLATFORM_AND_OS_DETECTION_H__

/* OS detection */
#if defined(WINDOWS) || defined(_WIN32)
 #define LIBIPMI_OS_WINDOWS					1
#elif defined(__linux__)
 #define LIBIPMI_OS_LINUX					1
#else
 #error "OS Unsupported" 
 #define LIBIPMI_OS_OTHER					1
#endif

#define LIBIPMI_IS_OS_WINDOWS()				LIBIPMI_OS_WINDOWS
#define LIBIPMI_IS_OS_LINUX()				LIBIPMI_OS_LINUX


/* Platform detection */
#if LIBIPMI_IS_OS_LINUX()
 #if defined(__i386__) || defined(__amd64__) || defined(__x86_64__)
  #define LIBIPMI_PLATFORM_X86				1
  #define LIBIPMI_BYTEORDER_LITTLEENDIAN	1
 #elif defined(__SH4__) || defined (__SH3__)  || defined (__SH4A__)  || defined (__sh__) 
  #define LIBIPMI_PLATFORM_SH				1
  #define LIBIPMI_BYTEORDER_LITTLEENDIAN	1
 #elif defined(__PPC__)
  #define LIBIPMI_PLATFORM_PPC				1
  #define LIBIPMI_BYTEORDER_BIGENDIAN		1
 #elif defined(__mips__)
  #define LIBIPMI_PLATFORM_MIPS				1
  #define LIBIPMI_BYTEORDER_BIGENDIAN		1
 #elif defined(__arm__)
  #define LIBIPMI_PLATFORM_ARM			1
  #define LIBIPMI_BYTEORDER_LITTLEENDIAN	1
 #else
  #define LIBIPMI_PLATFORM_UNKNOWN			1
  #define LIBIPMI_BYTEORDER_UNKNOWN			1
 #endif
#elif LIBIPMI_IS_OS_WINDOWS()
 #if defined(_M_IX86) || defined (_M_X64)
  #define LIBIPMI_PLATFORM_X86				1
  #define LIBIPMI_BYTEORDER_LITTLEENDIAN	1
 #else
  #error "Encountered Windows OS on non x86 platform..dont know what to do..who are you?"
  #define LIBIPMI_PLATFORM_UNKNOWN			1
  #define LIBIPMI_BYTEORDER_UNKNOWN			1
 #endif
#else
 #error "OS Unsupported"
 #define LIBIPMI_PLATFORM_UNKNOWN			1
 #define LIBIPMI_BYTEORDER_UNKNOWN			1
#endif

#define LIBIPMI_IS_PLATFORM_X86()					LIBIPMI_PLATFORM_X86
#define LIBIPMI_IS_PLATFORM_PPC()					LIBIPMI_PLATFORM_PPC
#define LIBIPMI_IS_PLATFORM_MIPS()					LIBIPMI_PLATFORM_MIPS
#define LIBIPMI_IS_PLATFORM_ARM()					LIBIPMI_PLATFORM_ARM
#define LIBIPMI_IS_PLATFORM_SH()					LIBIPMI_PLATFORM_SH

#endif /* __PLATFORM_AND_OS_DETECTION_H__ */
