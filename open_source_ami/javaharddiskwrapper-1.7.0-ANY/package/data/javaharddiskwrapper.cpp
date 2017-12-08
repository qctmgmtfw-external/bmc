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
#include "LinuxHarddiskReader.h"
#elif defined( ICC_OS_SOLARIS )
#include "SolarisHarddiskReader.h"
#elif defined( ICC_OS_WINDOWS )
//#include "StdAfx.h"
#include "WinNTHarddiskReader.h"
#elif defined( ICC_OS_MACH )
#include "MacHarddiskReader.h"
#include <X11/Xlib.h>
#include <X11/keysym.h>
#endif

#include "HarddiskImgFileReader.h"
#include <jni.h>
#include "java_prototypes.h"

static int GETUSB_KEY_HDD = 1;
static int GETFIXED_HDD = 2;


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

int check_fixed_hdd_linux(char *DriveList,int len,char *path);
extern "C" JNIEXPORT jobjectArray JNICALL
Java_com_ami_iusb_HarddiskRedir_listHardDrivesFixed( JNIEnv *env, jobject obj )
{
    CHarddiskReader *HarddiskReader;
    jclass cls, stringClass;
    jfieldID fid;
    jlong nativeReaderPointer;
    int len;
    char HarddiskList[ 32 * 32 ];
	char *HarddiskListPtr = HarddiskList;
    jobjectArray HarddiskPathArray;
	
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

    /* Cast the pointer to a CHarddiskReader object */
#if defined (__x86_64__)|| defined (WIN64)
    HarddiskReader = (CHarddiskReader *)( (unsigned long)nativeReaderPointer );
#else
    HarddiskReader = (CHarddiskReader *)( (unsigned int)nativeReaderPointer );
#endif

    /* Get the list of the available Harddisk drives */
    len = HarddiskReader->ListHardDrives( HarddiskList,GETFIXED_HDD );
	
    if( len <= 0 )
        return( NULL );
 
    /* Get a reference to the String class */
    stringClass = env->FindClass( "java/lang/String" );

    /* Create an array of java Strings to hold the Harddisk list we retrieved */
    HarddiskPathArray = (jobjectArray)env->NewObjectArray( len, stringClass, NULL );

    /* Write a string for each drive into the array */
    for( int i = 0; i < len; i++ )
    {
#if defined( ICC_OS_LINUX ) || defined( ICC_OS_SOLARIS ) || defined (ICC_OS_MACH)
        /* The linux call returns floppy paths as null terminated strings */
        env->SetObjectArrayElement( HarddiskPathArray, i, env->NewStringUTF( HarddiskListPtr ) );
        HarddiskListPtr += strlen( HarddiskListPtr ) + 1;
#elif defined( ICC_OS_WINDOWS )
        
		env->SetObjectArrayElement( HarddiskPathArray, i, env->NewStringUTF( HarddiskListPtr ) );
        HarddiskListPtr += strlen( HarddiskListPtr ) + 1;
		
#endif
    }

    return( HarddiskPathArray );
}

extern "C" JNIEXPORT jobjectArray JNICALL
Java_com_ami_iusb_HarddiskRedir_listHardDrives( JNIEnv *env, jobject obj )
{
    CHarddiskReader *HarddiskReader;
    jclass cls, stringClass;
    jfieldID fid;
    jlong nativeReaderPointer;
    int len;
    char HarddiskList[ 32 * 32 ];
	//char HarddiskRemovableList[ 20 * 17 ];
    char *HarddiskListPtr = HarddiskList;
	memset(HarddiskList,0,sizeof(HarddiskList));
    jobjectArray HarddiskPathArray;
	
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

    /* Cast the pointer to a CHarddiskReader object */
#if defined (__x86_64__)|| defined (WIN64)
    HarddiskReader = (CHarddiskReader *)( (unsigned long)nativeReaderPointer );
#else
    HarddiskReader = (CHarddiskReader *)( (unsigned int)nativeReaderPointer );
#endif

    /* Get the list of the available Harddisk drives */
    len = HarddiskReader->ListHardDrives( HarddiskList,GETUSB_KEY_HDD );
	
    if( len <= 0 )
        return( NULL );
 
    /* Get a reference to the String class */
    stringClass = env->FindClass( "java/lang/String" );

    /* Create an array of java Strings to hold the Harddisk list we retrieved */
    HarddiskPathArray = (jobjectArray)env->NewObjectArray( len, stringClass, NULL );

    /* Write a string for each drive into the array */
    for( int i = 0; i < len; i++ )
    {
#if defined( ICC_OS_LINUX ) || defined( ICC_OS_SOLARIS ) || defined (ICC_OS_MACH)
        /* The linux call returns floppy paths as null terminated strings */
        env->SetObjectArrayElement( HarddiskPathArray, i, env->NewStringUTF( HarddiskListPtr ) );
        HarddiskListPtr += strlen( HarddiskListPtr ) + 1;
#elif defined( ICC_OS_WINDOWS )
        env->SetObjectArrayElement( HarddiskPathArray, i, env->NewStringUTF( HarddiskListPtr ) );
        HarddiskListPtr += strlen( HarddiskListPtr ) + 1;
#endif
    }

    return( HarddiskPathArray );
}


extern "C" JNIEXPORT void JNICALL
Java_com_ami_iusb_HarddiskRedir_newHarddiskReader( JNIEnv *env, jobject obj, jboolean physicalDevice )
{
    jclass cls;
    jfieldID fid;
    jlong nativeReaderPointer;
    CHarddiskReader *HarddiskReader;

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
        HarddiskReader = (CHarddiskReader *)( (unsigned long)nativeReaderPointer );
#else
        HarddiskReader = (CHarddiskReader *)( (unsigned int)nativeReaderPointer );
#endif
        delete HarddiskReader;
    }

    /* Create a new HarddiskReader object */
    if( physicalDevice )
    {
#if defined( ICC_OS_LINUX )
        HarddiskReader = new CLinuxHarddiskReader;
#elif defined( ICC_OS_SOLARIS )
        HarddiskReader = new CSolarisHarddiskReader;
#elif defined( ICC_OS_WINDOWS )
        HarddiskReader = new CWinNTHarddiskReader;
#elif defined( ICC_OS_MACH )
        HarddiskReader = new CMacHarddiskReader;
#endif
    }
    else
    {
        HarddiskReader = new CHarddiskImgFileReader(0,0);
    }

    /* Cast the pointer to a jlong so java can hold it for us */
#if defined (__x86_64__) || defined (WIN64)
    nativeReaderPointer = (jlong)( (unsigned long )HarddiskReader );
#else
    nativeReaderPointer = (jlong)( (unsigned int )HarddiskReader );
#endif

    /* Copy the new nativeReaderPointer back to java */
    env->SetLongField( obj, fid, nativeReaderPointer );

    return;
}


extern "C" JNIEXPORT void JNICALL
Java_com_ami_iusb_HarddiskRedir_deleteHarddiskReader( JNIEnv *env, jobject obj )
{
    CHarddiskReader *HarddiskReader;
    jclass cls;
    jfieldID fid;
    jlong nativeReaderPointer;

    /* Get a reference to the class of which we are a member */
    cls = env->GetObjectClass( obj );

    /* Get a reference to the instance variable "nativeReaderPointer" */
    fid = env->GetFieldID( cls, "nativeReaderPointer", "J" );

    /* Get the value of the "nativeReaderPointer" variable */
    nativeReaderPointer = env->GetLongField( obj, fid );

    /* Cast the pointer to a CHarddiskReader object */
#if defined (__x86_64__) || defined (WIN64)
    HarddiskReader = (CHarddiskReader *)( (unsigned long)nativeReaderPointer );
#else
    HarddiskReader = (CHarddiskReader *)( (unsigned int)nativeReaderPointer );
#endif

    /* Close the device before deleting the reader, just to be safe */
    HarddiskReader->CloseDevice();
    delete HarddiskReader;

    /* Set nativeReaderPointer to -1 to indicate there is no HarddiskReader */
    nativeReaderPointer = -1;

    /* Set nativeReaderPointer in java */
    env->SetLongField( obj, fid, nativeReaderPointer );

    return;
}


extern "C" JNIEXPORT jint JNICALL
Java_com_ami_iusb_HarddiskRedir_openHarddisk( JNIEnv *env, jobject obj,jbyteArray jpath ,jboolean physicalDevice )
{
	char *path;
	jint path_len;
    jclass cls;
    jfieldID fid;
    jlong nativeReaderPointer;
    CHarddiskReader *HarddiskReader;
    //int retval;
    enum byteOrder device_byte_order = bigendian;
    jint functionReturn = JNI_TRUE;
	
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
        functionReturn = JNI_FALSE;
    }
    else
    {
        /* Cast the pointer to a HarddiskReader object */
#if defined (__x86_64__) || defined (WIN64)
        HarddiskReader = (CHarddiskReader *)( (unsigned long )nativeReaderPointer );
#else
        HarddiskReader = (CHarddiskReader *)( (unsigned int )nativeReaderPointer );
#endif

#if defined( ICC_OS_LINUX )
        /* Is this a scsi device? */
        if( strstr( path, "/dev/sd" ) != NULL || strstr( path, "/dev/hd" ) != NULL)
        {
            /* Close the device before deleting the reader, just to be safe */
            HarddiskReader->CloseDevice();
			char HarddiskList[ 32 * 32 ];
			int len = HarddiskReader->ListHardDrives( HarddiskList,2 );
			int ret_hdd = check_fixed_hdd_linux(HarddiskList,len,path);
            delete HarddiskReader;

            /* Create a new floppy image reader.  We handle scsi floppy */
            /* devices as images for now.                               */
		if(ret_hdd == -1)
			 HarddiskReader = new CHarddiskImgFileReader(0,0);
		else
			 HarddiskReader = new CHarddiskImgFileReader(1,0);
            /* Cast the pointer to a jlong so java can hold it for us */
#if defined (__x86_64__) || defined (WIN64)
            nativeReaderPointer = (jlong)( (unsigned long )HarddiskReader );
#else
            nativeReaderPointer = (jlong)( (unsigned int )HarddiskReader );
#endif

            /* Copy the new nativeReaderPointer back to java */
            env->SetLongField( obj, fid, nativeReaderPointer );
        }
#endif
#if defined (ICC_OS_MACH)
	/* Close the device before deleting the reader, just to be safe */
	HarddiskReader->CloseDevice();
	delete HarddiskReader;

	/* Create a new floppy image reader.  We handle scsi floppy */
	/* devices as images for now.                               */
	HarddiskReader = new CHarddiskImgFileReader(0,physicalDevice);
	/* Cast the pointer to a jlong so java can hold it for us */
#if defined (__x86_64__) || defined (WIN64)
	nativeReaderPointer = (jlong)( (unsigned long )HarddiskReader );
#else
	nativeReaderPointer = (jlong)( (unsigned int )HarddiskReader );
#endif

	/* Copy the new nativeReaderPointer back to java */
	env->SetLongField( obj, fid, nativeReaderPointer );

#endif
        /* Open the Harddisk drive at the specified path */
        functionReturn = HarddiskReader->OpenDevice( path, device_byte_order );
    }

    /* Release memory used for the path string */
	free(path);
  //  env->ReleaseStringUTFChars( path, str );

    return( functionReturn );
}

int check_fixed_hdd_linux(char *DriveList,int len,char *path)
{
	int k=0;
	char DeviceName[30];
	for(k=0;k<len;k++)
	{
		strncpy(DeviceName,DriveList,8);
		if(strncmp(DeviceName,path,8) == 0)
		return SUCCESS;
		DriveList += strlen(DriveList)+1;
	}
	return -1;
}

extern "C" JNIEXPORT void JNICALL
Java_com_ami_iusb_HarddiskRedir_closeHarddisk( JNIEnv *env, jobject obj )
{
    CHarddiskReader *HarddiskReader;
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
        return;
    }

    /* Cast the pointer to a CHarddiskReader object */
#if defined (__x86_64__) || defined (WIN64)
    HarddiskReader = (CHarddiskReader *)( (unsigned long)nativeReaderPointer );
#else
    HarddiskReader = (CHarddiskReader *)( (unsigned int)nativeReaderPointer );
#endif

    /* Close the Harddisk drive */
    HarddiskReader->CloseDevice();

    return;
}


extern "C" JNIEXPORT jint JNICALL
Java_com_ami_iusb_HarddiskRedir_executeHarddiskSCSICmd(
    JNIEnv *env, jobject obj, jobject requestBuf, jobject responseBuf )
{
    CHarddiskReader *HarddiskReader;
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

    /* Cast the pointer to a CHarddiskReader object */
#if defined (__x86_64__) || defined (WIN64)
    HarddiskReader = (CHarddiskReader *)( (unsigned long)nativeReaderPointer );
#else
    HarddiskReader = (CHarddiskReader *)( (unsigned int)nativeReaderPointer );
#endif

    /* Get the addresses of the request and response buffers from java */
    request = (IUSB_SCSI_PACKET *)env->GetDirectBufferAddress( requestBuf );
    response = (IUSB_SCSI_PACKET *)env->GetDirectBufferAddress( responseBuf );

    /* Execute the requested scsi command.  The response is written into */
    /* the response buffer which is accessible via java, since we use    */
    /* directly allocated buffers for the request and response buffers   */
    HarddiskReader->ExecuteSCSICmd( request, response, &dataLen );

    /* Return the total length of the response */
    return( dataLen + sizeof( IUSB_SCSI_PACKET ) - 1 );
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_ami_iusb_HarddiskRedir_getVersion( JNIEnv *env, jobject obj )
{
    jclass cls;
    jfieldID fid;
    jlong nativeReaderPointer;
    CHarddiskReader *HarddiskReader;

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

    /* Cast the pointer to a CHarddiskReader object */
#if defined (__x86_64__) || defined (WIN64)
    HarddiskReader = (CHarddiskReader *)( (unsigned long)nativeReaderPointer );
#else
    HarddiskReader = (CHarddiskReader *)( (unsigned int)nativeReaderPointer );
#endif

    /* Return the version number from the Harddisk library */
    return( env->NewStringUTF( HarddiskReader->GetVersion().c_str() ) );
}

extern "C" JNIEXPORT jbyte JNICALL 
Java_com_ami_iusb_HarddiskRedir_GetLEDStatus(JNIEnv *, jobject)
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
