/****************************-*- andrew-c++ -*-*******************************/
/* Filename:    javafloppywrapper.cpp                                        */
/* Author:      Andrew McCallum (andrewm@ami.com)                            */
/* Created:     02/09/2005                                                   */
/* Modified:    03/11/2005                                                   */
/* Description: JNI native code wrapper for LIBFLOPPY, used in JavaRConsole  */
/*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "coreTypes.h"
#include "icc_what.h"

#if defined( ICC_OS_LINUX )
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/XKBlib.h>
#include "LinuxFloppyReader.h"
#elif defined( ICC_OS_SOLARIS )
#include "SolarisFloppyReader.h"
#elif defined( ICC_OS_WINDOWS )
//#include "StdAfx.h"
#include "WinNTFloppyReader.h"
#elif defined( ICC_OS_MACH )
#include "MacFloppyReader.h"
#include <X11/Xlib.h>
#include <X11/keysym.h>
#endif

#include "FloppyImgFileReader.h"
#include <jni.h>
#include "java_prototypes.h"

#define CORE_KEYBOARD "CoreKeyboard"
#define INPUT_DEVICE	"InputDevice"
#define SERVER_LAYOUT	"ServerLayout"
#define SECTION		"Section"
#define END_SECTION	"EndSection"
#define IDENTIFIER	"Identifier"
#define XKB_LAYOUT	"XkbLayout"
#define XKB_VARIANT	"XkbVariant"
#define OPTION		"Option"

#define S_BEGIN		(0)
#define S_SLB		(1)
#define S_SLE		(2)
#define S_KLB		(3)
#define	S_KLBX		(4)
#define S_KLBY		(5)
#define S_KLE		(6)

extern "C" JNIEXPORT jobjectArray JNICALL
Java_com_ami_iusb_FloppyRedir_listFloppyDrives( JNIEnv *env, jobject obj )
{
    CFloppyReader *FloppyReader;
    jclass cls, stringClass;
    jfieldID fid;
    jlong nativeReaderPointer;
    int len;
    char FloppyList[ 20 * 17 ];
    char *FloppyListPtr = FloppyList;
	memset(FloppyList,0,sizeof(FloppyList));
    jobjectArray FloppyPathArray;

    /* Get a reference to the class of which we are a member */
    cls = env->GetObjectClass( obj );

    /* Get a reference to the instance variable "nativeReaderPointer" */
    fid = env->GetFieldID( cls, "nativeReaderPointer", "J" );

    /* Get the value of the "nativeReaderPointer" variable */
    nativeReaderPointer = env->GetLongField( obj, fid );
    if( nativeReaderPointer < 0 )
    {
       printf( "No reader!  Aborting...\n" );
        return( NULL );
    }

    /* Cast the pointer to a CFloppyReader object */
#if defined (__x86_64__)|| defined (WIN64)
    FloppyReader = (CFloppyReader *)( (unsigned long)nativeReaderPointer );
#else
    FloppyReader = (CFloppyReader *)( (unsigned int)nativeReaderPointer );
#endif

    /* Get the list of the available Floppy drives */
    len = FloppyReader->ListFloppyDrives( FloppyList );
    if( len <= 0 )
        return( NULL );
 
    /* Get a reference to the String class */
    stringClass = env->FindClass( "java/lang/String" );

    /* Create an array of java Strings to hold the Floppy list we retrieved */
    FloppyPathArray = (jobjectArray)env->NewObjectArray( len, stringClass, NULL );

    /* Write a string for each drive into the array */
    for( int i = 0; i < len; i++ )
    {
#if defined( ICC_OS_LINUX ) || defined( ICC_OS_SOLARIS ) || defined (ICC_OS_MACH)
        /* The linux call returns floppy paths as null terminated strings */
        env->SetObjectArrayElement( FloppyPathArray, i, env->NewStringUTF( FloppyListPtr ) );
        FloppyListPtr += strlen( FloppyListPtr ) + 1;
#elif defined( ICC_OS_WINDOWS )
        /* The windows call returns floppy paths as drive letters, no terminator */
        char drive[ 2 ];
        drive[ 0 ] = *FloppyListPtr;
        drive[ 1 ] = '\0';
        env->SetObjectArrayElement( FloppyPathArray, i, env->NewStringUTF( drive ) );
        FloppyListPtr++;
#endif
    }

    return( FloppyPathArray );
}


extern "C" JNIEXPORT void JNICALL
Java_com_ami_iusb_FloppyRedir_newFloppyReader( JNIEnv *env, jobject obj, jboolean physicalDevice )
{
    jclass cls;
    jfieldID fid;
    jlong nativeReaderPointer;
    CFloppyReader *FloppyReader;

    /* Get a reference to the class of which we are a member */
    cls = env->GetObjectClass( obj );

    /* Get a reference to the instance variable "nativeReaderPointer" */
    fid = env->GetFieldID( cls, "nativeReaderPointer", "J" );

    /* Get the value of the "nativeReaderPointer" variable */
    nativeReaderPointer = env->GetLongField( obj, fid );
    if( nativeReaderPointer > 0 )
    {
        /* We expect nativeReaderPointer to be -1 if there is no reader */
        /* allocated.  If it's > 0, it probably points at an existing   */
        /* reader.  Destroy it to prevent memory leaks.                 */
#if defined (__x86_64__) || defined (WIN64)
        FloppyReader = (CFloppyReader *)( (unsigned long)nativeReaderPointer );
#else
        FloppyReader = (CFloppyReader *)( (unsigned int)nativeReaderPointer );
#endif
        delete FloppyReader;
    }

    /* Create a new FloppyReader object */
    if( physicalDevice )
    {
#if defined( ICC_OS_LINUX )
        FloppyReader = new CLinuxFloppyReader;
#elif defined( ICC_OS_SOLARIS )
        FloppyReader = new CSolarisFloppyReader;
#elif defined( ICC_OS_WINDOWS )
        FloppyReader = new CWinNTFloppyReader;
#elif defined( ICC_OS_MACH )
        FloppyReader = new CMacFloppyReader;
#endif
    }
    else
    {
        FloppyReader = new CFloppyImgFileReader;
    }

    /* Cast the pointer to a jlong so java can hold it for us */
#if defined (__x86_64__) || defined (WIN64)
    nativeReaderPointer = (jlong)( (unsigned long )FloppyReader );
#else
    nativeReaderPointer = (jlong)( (unsigned int )FloppyReader );
#endif

    /* Copy the new nativeReaderPointer back to java */
    env->SetLongField( obj, fid, nativeReaderPointer );

    return;
}


extern "C" JNIEXPORT void JNICALL
Java_com_ami_iusb_FloppyRedir_deleteFloppyReader( JNIEnv *env, jobject obj )
{
    CFloppyReader *FloppyReader;
    jclass cls;
    jfieldID fid;
    jlong nativeReaderPointer;

    /* Get a reference to the class of which we are a member */
    cls = env->GetObjectClass( obj );

    /* Get a reference to the instance variable "nativeReaderPointer" */
    fid = env->GetFieldID( cls, "nativeReaderPointer", "J" );

    /* Get the value of the "nativeReaderPointer" variable */
    nativeReaderPointer = env->GetLongField( obj, fid );

    /* Cast the pointer to a CFloppyReader object */
#if defined (__x86_64__) || defined (WIN64)
    FloppyReader = (CFloppyReader *)( (unsigned long)nativeReaderPointer );
#else
    FloppyReader = (CFloppyReader *)( (unsigned int)nativeReaderPointer );
#endif

    /* Close the device before deleting the reader, just to be safe */
    FloppyReader->CloseDevice();
    delete FloppyReader;

    /* Set nativeReaderPointer to -1 to indicate there is no FloppyReader */
    nativeReaderPointer = -1;

    /* Set nativeReaderPointer in java */
    env->SetLongField( obj, fid, nativeReaderPointer );

    return;
}


extern "C" JNIEXPORT jboolean JNICALL
Java_com_ami_iusb_FloppyRedir_openFloppy( JNIEnv *env, jobject obj,jbyteArray jpath )
{
	char *path;
	jint path_len;
    jclass cls;
    jfieldID fid;
    jlong nativeReaderPointer;
    CFloppyReader *FloppyReader;
    int retval;
    enum byteOrder device_byte_order = bigendian;
    jboolean functionReturn = JNI_TRUE;

    /* Pull the specified floppy path out of java */
    //str = (char *)env->GetStringUTFChars( path, NULL );
	path_len = env->GetArrayLength(jpath);
	path = (char *) malloc(path_len + 1);
	env->GetByteArrayRegion(jpath, 0, path_len, (jbyte *) path);
	path[path_len] = '\0';

    /* Get a reference to the class of which we are a member */
    cls = env->GetObjectClass( obj );

    /* Get a reference to the instance variable "nativeReaderPointer" */
    fid = env->GetFieldID( cls, "nativeReaderPointer", "J" );

    /* Get the value of the "nativeReaderPointer" variable */
    nativeReaderPointer = env->GetLongField( obj, fid );
    if( nativeReaderPointer < 0 )
    {
        /* No reader.  Return an error */
        printf( "No reader!  Returning an error\n" );
        fflush( stdout );
        functionReturn = JNI_FALSE;
    }
    else
    {
        /* Cast the pointer to a FloppyReader object */
#if defined (__x86_64__) || defined (WIN64)
        FloppyReader = (CFloppyReader *)( (unsigned long )nativeReaderPointer );
#else
        FloppyReader = (CFloppyReader *)( (unsigned int )nativeReaderPointer );
#endif

#if defined( ICC_OS_LINUX )
        /* Is this a scsi device? */
        if( strstr( path, "/dev/sd" ) != NULL )
        {
            /* Close the device before deleting the reader, just to be safe */
            FloppyReader->CloseDevice();
            delete FloppyReader;

            /* Create a new floppy image reader.  We handle scsi floppy */
            /* devices as images for now.                               */
            FloppyReader = new CFloppyImgFileReader;

            /* Cast the pointer to a jlong so java can hold it for us */
#if defined (__x86_64__) || defined (WIN64)
            nativeReaderPointer = (jlong)( (unsigned long )FloppyReader );
#else
            nativeReaderPointer = (jlong)( (unsigned int )FloppyReader );
#endif

            /* Copy the new nativeReaderPointer back to java */
            env->SetLongField( obj, fid, nativeReaderPointer );
        }
#endif
#if defined (ICC_OS_MACH)
	/* Close the device before deleting the reader, just to be safe */
	FloppyReader->CloseDevice();
	delete FloppyReader;

	/* Create a new floppy image reader.  We handle scsi floppy */
	/* devices as images for now.                               */
	FloppyReader = new CFloppyImgFileReader;
	/* Cast the pointer to a jlong so java can hold it for us */
#if defined (__x86_64__) || defined (WIN64)
	nativeReaderPointer = (jlong)( (unsigned long )FloppyReader );
#else
	nativeReaderPointer = (jlong)( (unsigned int )FloppyReader );
#endif

	/* Copy the new nativeReaderPointer back to java */
	env->SetLongField( obj, fid, nativeReaderPointer );

#endif
        /* Open the Floppy drive at the specified path */
        retval = FloppyReader->OpenDevice( path, device_byte_order );
        if( retval == SUCCESS )
            functionReturn = JNI_TRUE;
        else
            functionReturn = JNI_FALSE;
    }

    /* Release memory used for the path string */
   // env->ReleaseStringUTFChars( path, str ); 
	free(path);

    return( functionReturn );
}


extern "C" JNIEXPORT void JNICALL
Java_com_ami_iusb_FloppyRedir_closeFloppy( JNIEnv *env, jobject obj )
{
    CFloppyReader *FloppyReader;
    jclass cls;
    jfieldID fid;
    jlong nativeReaderPointer;

    /* Get a reference to the class of which we are a member */
    cls = env->GetObjectClass( obj );

    /* Get a reference to the instance variable "nativeReaderPointer" */
    fid = env->GetFieldID( cls, "nativeReaderPointer", "J" );

    /* Get the value of the "nativeReaderPointer" variable */
    nativeReaderPointer = env->GetLongField( obj, fid );
    if( nativeReaderPointer < 0 )
    {
        /* No reader.  Return an error */
        printf( "No reader!  Returning an error\n" );
        fflush( stdout );
        return;
    }

    /* Cast the pointer to a CFloppyReader object */
#if defined (__x86_64__) || defined (WIN64)
    FloppyReader = (CFloppyReader *)( (unsigned long)nativeReaderPointer );
#else
    FloppyReader = (CFloppyReader *)( (unsigned int)nativeReaderPointer );
#endif

    /* Close the Floppy drive */
    FloppyReader->CloseDevice();

    return;
}


extern "C" JNIEXPORT jint JNICALL
Java_com_ami_iusb_FloppyRedir_executeFloppySCSICmd(
    JNIEnv *env, jobject obj, jobject requestBuf, jobject responseBuf )
{
    CFloppyReader *FloppyReader;
    jclass cls;
    jfieldID fid;
    jlong nativeReaderPointer;
#if defined (__x86_64__) || defined (WIN64)
    unsigned int dataLen;
#else
    unsigned long dataLen;
#endif
    IUSB_SCSI_PACKET *request;
    IUSB_SCSI_PACKET *response;

    /* Get a reference to the class of which we are a member */
    cls = env->GetObjectClass( obj );

    /* Get a reference to the instance variable "nativeReaderPointer" */
    fid = env->GetFieldID( cls, "nativeReaderPointer", "J" );

    /* Get the value of the "nativeReaderPointer" variable */
    nativeReaderPointer = env->GetLongField( obj, fid );
    if( nativeReaderPointer < 0 )
    {
        printf( "No reader!  Aborting...\n" );
        return( -1 );
    }

    /* Cast the pointer to a CFloppyReader object */
#if defined (__x86_64__) || defined (WIN64)
    FloppyReader = (CFloppyReader *)( (unsigned long)nativeReaderPointer );
#else
    FloppyReader = (CFloppyReader *)( (unsigned int)nativeReaderPointer );
#endif

    /* Get the addresses of the request and response buffers from java */
    request = (IUSB_SCSI_PACKET *)env->GetDirectBufferAddress( requestBuf );
    response = (IUSB_SCSI_PACKET *)env->GetDirectBufferAddress( responseBuf );

    /* Execute the requested scsi command.  The response is written into */
    /* the response buffer which is accessible via java, since we use    */
    /* directly allocated buffers for the request and response buffers   */
    FloppyReader->ExecuteSCSICmd( request, response, &dataLen );

    /* Return the total length of the response */
    return( dataLen + sizeof( IUSB_SCSI_PACKET ) - 1 );
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_ami_iusb_FloppyRedir_getVersion( JNIEnv *env, jobject obj )
{
    jclass cls;
    jfieldID fid;
    jlong nativeReaderPointer;
    CFloppyReader *FloppyReader;

    /* Get a reference to the class of which we are a member */
    cls = env->GetObjectClass( obj );

    /* Get a reference to the instance variable "nativeReaderPointer" */
    fid = env->GetFieldID( cls, "nativeReaderPointer", "J" );

    /* Get the value of the "nativeReaderPointer" variable */
    nativeReaderPointer = env->GetLongField( obj, fid );
    if( nativeReaderPointer < 0 )
    {
        printf( "No reader!  Aborting...\n" );
        return( NULL );
    }

    /* Cast the pointer to a CFloppyReader object */
#if defined (__x86_64__) || defined (WIN64)
    FloppyReader = (CFloppyReader *)( (unsigned long)nativeReaderPointer );
#else
    FloppyReader = (CFloppyReader *)( (unsigned int)nativeReaderPointer );
#endif

    /* Return the version number from the Floppy library */
    return( env->NewStringUTF( FloppyReader->GetVersion().c_str() ) );
}

extern "C" JNIEXPORT jbyte JNICALL 
Java_com_ami_iusb_FloppyRedir_GetLEDStatus(JNIEnv *, jobject)
{
	jbyte led_status = (jbyte)-1;
#if defined( ICC_OS_LINUX ) || defined (ICC_OS_MACH)
	XKeyboardState	values;
	Display* dpy;
	
	dpy = XOpenDisplay(NULL);	
	if (dpy != NULL) 
	{
		if( XGetKeyboardControl(dpy, &values) == 1 )
		{
			//printf("XGetKeyboardControl success\n");
			led_status = 0;
			if(values.led_mask & 0x01) led_status |= 0x02; // capslock
			if(values.led_mask & 0x02) led_status |= 0x01; // numlock
			if(values.led_mask & 0x04) led_status |= 0x04; // scrolllock
		}
		
		XCloseDisplay(dpy);
	}
#elif defined( ICC_OS_WINDOWS )
	led_status = 0;
	
	led_status |= (GetKeyState(VK_NUMLOCK)&0x01)?0x01:0x00;
	led_status |= (GetKeyState(VK_CAPITAL)&0x01)?0x02:0x00;
	led_status |= (GetKeyState(VK_SCROLL)&0x01)?0x04:0x00;
#endif

	return led_status;
}




extern "C" JNIEXPORT jstring JNICALL 
Java_com_ami_iusb_FloppyRedir_GetKeyboardName(JNIEnv *env, jobject)
{
	
	
	
	
	
#if defined( ICC_OS_LINUX )
	FILE *fp;
	char buf[1024];
	char *ptr, *tptr, *nptr;
	char inputdev[1024];
	int state = S_BEGIN;
	char layout[1024];


	fp = fopen("/etc/X11/xorg.conf", "rb");

	if(fp) {
		while(!feof(fp) && state != S_KLE) {
			//printf("Press any key tyo continue...\n");
			//getchar();
			fgets(buf, sizeof(buf), fp);
			//printf("Line: %s", buf);
			//printf("State: %d\n", state);
			switch(state) {			
				case S_BEGIN:
					ptr = strstr(buf, SECTION );
					if(ptr) {
						//section 
						tptr = strstr(ptr+sizeof(SECTION), SERVER_LAYOUT);
						if(tptr) {
							state = S_SLB;
						}
					}
					break;
				case S_SLB:
					ptr = strstr(buf, INPUT_DEVICE);
					if(ptr) {
						tptr = strstr(ptr+sizeof(INPUT_DEVICE), CORE_KEYBOARD);
						if(tptr) {
							tptr = strstr(ptr+sizeof(INPUT_DEVICE), "\"");
							if(tptr) {
								nptr = strstr(tptr+1, "\"");
								if(nptr) {
									strncpy(inputdev, tptr+1, (nptr-tptr-1));
									inputdev[nptr-tptr-1] = '\0';
									//printf("input device: %s\n", inputdev);
									state = S_SLE;
								}
							}
						}
					}
					break;
				case S_SLE:
					ptr = strstr(buf, END_SECTION );
					if(ptr) {
						state = S_KLB;
					}
					break;
				case S_KLB:
					ptr = strstr(buf, SECTION );
					if(ptr) {
						tptr = strstr(ptr+sizeof(SECTION), INPUT_DEVICE);
						if(tptr) {
							state = S_KLBX;
						}
					}
					break;
				case S_KLBX:
					ptr = strstr(buf, IDENTIFIER );
					if(ptr) {
						tptr = strstr(ptr+sizeof(IDENTIFIER),inputdev );
						if(tptr) {
							state = S_KLBY;
						}
					}
					break;

				case S_KLBY:
					ptr = strstr(buf, OPTION );
					if(ptr) {
						tptr = strstr(ptr+sizeof(OPTION), XKB_LAYOUT );
						if(tptr) {
							tptr = strstr(tptr+sizeof(XKB_LAYOUT)+1, "\"");
							if(tptr) {
								nptr = strstr(tptr+1, "\"");
								if(nptr) {
									strncpy(layout, tptr+1, nptr-tptr-1);
									layout[nptr-tptr-1] = '\0';
									printf("Layout is : %s\n", layout);
									state = S_KLE;
								}
							}
						}
					}
					break;
			}

		}
		fclose(fp);
	}

return (env->NewStringUTF(layout));
#elif defined( ICC_OS_WINDOWS )
	char kbdlayout[256];
	GetKeyboardLayoutName(kbdlayout);

	return (env->NewStringUTF(kbdlayout));
#elif defined( ICC_OS_MACH )
	return (env->NewStringUTF("EN"));
#endif
	
}
