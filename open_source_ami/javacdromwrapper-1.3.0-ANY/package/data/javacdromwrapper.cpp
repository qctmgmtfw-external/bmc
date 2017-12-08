/****************************-*- andrew-c++ -*-*******************************/
/* Filename:    javacdromwrapper.cpp                                         */
/* Author:      Andrew McCallum (andrewm@ami.com)                            */
/* Created:     01/27/2005                                                   */
/* Modified:    03/11/2005                                                   */
/* Description: JNI native code wrapper for LIBCDROM, used in JavaRConsole   */
/*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <string>

#include "coreTypes.h"
#include "icc_what.h"

#if defined( ICC_OS_LINUX )
	#include "LinuxMediaReader.h"
#elif defined( ICC_OS_SOLARIS )
	#include "SolarisMediaReader.h"
#elif defined( ICC_OS_MACH )
	#include "MacMediaReader.h"
#endif

#if defined( ICC_OS_WINDOWS )
	#include "StdAfx.h"
	#include "WinNTMediaReader.h"
#endif

#include "CDROMImgFileReader.h"

#include <jni.h>
#include "java_prototypes.h"


extern "C" JNIEXPORT void JNICALL
Java_com_ami_iusb_CDROMRedir_newCDROMReader( JNIEnv *env, jobject obj, jboolean physicalDevice )
{
    jclass cls;
    jfieldID fid;
    jlong nativeReaderPointer;
    CCDROMReader *CDROMReader;

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
#if defined (__x86_64__)|| defined (WIN64)
        CDROMReader = (CCDROMReader *)( (unsigned long)nativeReaderPointer );
#else
        CDROMReader = (CCDROMReader *)( (unsigned int)nativeReaderPointer );
#endif
        delete CDROMReader;
    }

    /* Create a new CDROMReader object */
    if( physicalDevice )
    {
#if defined( ICC_OS_LINUX )
        CDROMReader = new CLinuxMediaReader;
#elif defined( ICC_OS_SOLARIS )
        CDROMReader = new CSolarisMediaReader;
#elif defined( ICC_OS_WINDOWS )
        CDROMReader = new CWinNTMediaReader;
#elif defined( ICC_OS_MACH )
	CDROMReader = new CMacMediaReader;
#endif
    }
    else
        CDROMReader = new CCDROMImgFileReader;

    /* Cast the pointer to a jlong so java can hold it for us */
#if defined (__x86_64__)|| defined (WIN64)
    nativeReaderPointer = (jlong)( (unsigned long )CDROMReader );
#else
    nativeReaderPointer = (jlong)( (unsigned int )CDROMReader );
#endif

    /* Copy the new nativeReaderPointer back to java */
    env->SetLongField( obj, fid, nativeReaderPointer );

    return;
}


extern "C" JNIEXPORT void JNICALL
Java_com_ami_iusb_CDROMRedir_deleteCDROMReader( JNIEnv *env, jobject obj )
{
    CCDROMReader *CDROMReader;
    jclass cls;
    jfieldID fid;
    jlong nativeReaderPointer;

    /* Get a reference to the class of which we are a member */
    cls = env->GetObjectClass( obj );

    /* Get a reference to the instance variable "nativeReaderPointer" */
    fid = env->GetFieldID( cls, "nativeReaderPointer", "J" );

    /* Get the value of the "nativeReaderPointer" variable */
    nativeReaderPointer = env->GetLongField( obj, fid );

    /* Cast the pointer to a CCDROMReader object */
#if defined (__x86_64__)|| defined (WIN64)
    CDROMReader = (CCDROMReader *)( (unsigned long)nativeReaderPointer );
#else
    CDROMReader = (CCDROMReader *)( (unsigned int)nativeReaderPointer );
#endif

    /* Close the device before deleting the reader, just to be safe */
    CDROMReader->CloseDevice();
    delete CDROMReader;

    /* Set nativeReaderPointer to -1 to indicate there is no CDROMReader */
    nativeReaderPointer = -1;

    /* Set nativeReaderPointer in java */
    env->SetLongField( obj, fid, nativeReaderPointer );

    return;
}


extern "C" JNIEXPORT jboolean JNICALL
Java_com_ami_iusb_CDROMRedir_openCDROM( JNIEnv *env, jobject obj, jbyteArray jpath )
{
    char *path;
	jint path_len;
    jclass cls;
    jfieldID fid;
    jlong nativeReaderPointer;
    CCDROMReader *CDROMReader;
    int retval;
    enum byteOrder device_byte_order = bigendian;
    jboolean functionReturn = JNI_TRUE;

    /* Pull the specified cdrom path out of java */
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
        /* Cast the pointer to a CCDROMReader object */
#if defined (__x86_64__)|| defined (WIN64)
        CDROMReader = (CCDROMReader *)( (unsigned long )nativeReaderPointer );
#else
        CDROMReader = (CCDROMReader *)( (unsigned int )nativeReaderPointer );
#endif

        /* Open the CDROM drive at the specified path */
        retval = CDROMReader->OpenDevice( path, device_byte_order );
        if( retval == SUCCESS )
            functionReturn = JNI_TRUE;
        else
            functionReturn = JNI_FALSE;
    }

    /* Release memory used for the path string */
    free(path);

    return( functionReturn );
}


extern "C" JNIEXPORT void JNICALL
Java_com_ami_iusb_CDROMRedir_closeCDROM( JNIEnv *env, jobject obj )
{
    CCDROMReader *CDROMReader;
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

    /* Cast the pointer to a CCDROMReader object */
#if defined (__x86_64__) || defined (WIN64)
    CDROMReader = (CCDROMReader *)( (unsigned long)nativeReaderPointer );
#else
    CDROMReader = (CCDROMReader *)( (unsigned int)nativeReaderPointer );
#endif

    /* Close the CDROM drive */
    CDROMReader->CloseDevice();

    return;
}


extern "C" JNIEXPORT jobjectArray
JNICALL Java_com_ami_iusb_CDROMRedir_listCDROMDrives( JNIEnv *env, jobject obj )
{
    CCDROMReader *CDROMReader;
    jclass cls, stringClass;
    jfieldID fid;
    jlong nativeReaderPointer;
    int len;
    char CDROMList[ 20 * 17 ];
    char *CDROMListPtr = CDROMList;
    jobjectArray CDROMPathArray;
	memset(CDROMList,0,sizeof(CDROMList));
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

    /* Cast the pointer to a CCDROMReader object */
#if defined (__x86_64__)|| defined (WIN64)
    CDROMReader = (CCDROMReader *)( (unsigned long)nativeReaderPointer );
#else
    CDROMReader = (CCDROMReader *)( (unsigned int)nativeReaderPointer );
#endif

    /* Get the list of the available CDROM drives */
    len = CDROMReader->ListCDROMDrives( CDROMList );
    if( len <= 0 )
        return( NULL );
 
    /* Get a reference to the String class */
    stringClass = env->FindClass( "java/lang/String" );

    /* Create an array of java Strings to hold the CDROM list we retrieved */
    CDROMPathArray = (jobjectArray)env->NewObjectArray( len, stringClass, NULL );

    /* Write a string for each drive into the array */
    for( int i = 0; i < len; i++ )
    {
#if defined( ICC_OS_LINUX ) || defined( ICC_OS_SOLARIS ) || defined (ICC_OS_MACH)
        /* The linux call returns cdrom paths as null terminated strings */
        env->SetObjectArrayElement( CDROMPathArray, i, env->NewStringUTF( CDROMListPtr ) );
        CDROMListPtr += strlen( CDROMListPtr ) + 1;
#elif defined( ICC_OS_WINDOWS )
        /* The windows call returns cdrom paths as drive letters, no terminator */
        char drive[ 2 ];
        drive[ 0 ] = *CDROMListPtr;
        drive[ 1 ] = '\0';
        env->SetObjectArrayElement( CDROMPathArray, i, env->NewStringUTF( drive ) );
        CDROMListPtr++;
#endif
    }

    return( CDROMPathArray );
}


extern "C" JNIEXPORT jint JNICALL
Java_com_ami_iusb_CDROMRedir_executeCDROMSCSICmd(
    JNIEnv *env, jobject obj, jobject requestBuf, jobject responseBuf )
{
    CCDROMReader *CDROMReader;
    jclass cls;
    jfieldID fid;
    jlong nativeReaderPointer;
#if defined (__x86_64__)|| defined (WIN64)
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

    /* Cast the pointer to a CCDROMReader object */
#if defined (__x86_64__) || defined (WIN64)
    CDROMReader = (CCDROMReader *)( (unsigned long)nativeReaderPointer );
#else
    CDROMReader = (CCDROMReader *)( (unsigned int)nativeReaderPointer );
#endif

    /* Get the addresses of the request and response buffers from java */
    request = (IUSB_SCSI_PACKET *)env->GetDirectBufferAddress( requestBuf );
    response = (IUSB_SCSI_PACKET *)env->GetDirectBufferAddress( responseBuf );
    if( ( request == NULL ) || ( response == NULL ) )
    {
        printf( "DIRECT BUFFERS NOT SUPPORTED IN JNI\n" );
        return( -1 );
    }

    /* Execute the requested scsi command.  The response is written into */
    /* the response buffer which is accessible via java, since we use    */
    /* directly allocated buffers for the request and response buffers   */
    CDROMReader->ExecuteSCSICmd( request, response, &dataLen );

    /* Return the total length of the response */
    return( dataLen + sizeof( IUSB_SCSI_PACKET ) - 1 );
}


extern "C" JNIEXPORT jstring JNICALL
Java_com_ami_iusb_CDROMRedir_getVersion( JNIEnv *env, jobject obj )
{
    jclass cls;
    jfieldID fid;
    jlong nativeReaderPointer;
    CCDROMReader *CDROMReader;

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

    /* Cast the pointer to a CCDROMReader object */
#if defined (__x86_64__) || defined (WIN64)
    CDROMReader = (CCDROMReader *)( (unsigned long)nativeReaderPointer );
#else
    CDROMReader = (CCDROMReader *)( (unsigned int)nativeReaderPointer );
#endif

    /* Return the version number from the CDROM library */
    return( env->NewStringUTF( CDROMReader->GetVersion().c_str() ) );
}
