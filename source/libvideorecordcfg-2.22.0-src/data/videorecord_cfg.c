/****************************************************************
 **                                                            **
 **    (C) Copyright 2006-2009, American Megatrends Inc.       **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600          **
 **                                                            **
****************************************************************/
#define _GNU_SOURCE
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>
#include <string.h>
#include <sys/mount.h>
#include "dbgout.h"
#include "iniparser.h"
#include "dictionary.h"
#include "videorecord_cfg.h"
#include  "errno.h"
#include "semaph.h"
#include "featuredef.h"
#include "unix.h"
#include <dirent.h>
#include <sys/file.h>
#include <string.h>
#include "coreTypes.h"
#include <sys/time.h>
#include "ftw.h"
#include "hostname.h"


//#define ENABLED 1
#define DISABLED 0

#define UNMOUNTED  -1

/*--------------------global declarations------------------------*/
static sem_t        file_lock_sem;
static unsigned int    sem_inited = 0;        
/*---------------------------------------------------------------*/

int GetRecordBootCrashCfg(RecordBootCrashCfg_T * pRecordBootCrashCfg)
{
    dictionary *d = NULL;
    int err_value = 0xFFFFFF;
    char temp[MAX_TEMP_ARRAY_SIZE]={0};

    // Loads the autorecord configuration file
    //Using same config file for triggered and continuous recording 
    d = iniparser_loaddef(AUTORECORD_DEF_CFG_FILE, AUTORECORD_CFG_FILE);
    if( d == NULL )
    {
        TEMERG("Unable to find/load/parse Configuration file : %s", AUTORECORD_CFG_FILE);
        return VDO_RCD_FAILURE;
    }
    #if 0
    // Parses the current_state from config file
    sprintf(temp, "%s:%s", REC_BOOT_CRASH_SECTION_NAME, REC_BOOT_CRASH_CURRENT_STATE);
    pRecordBootCrashCfg->current_state = (unsigned char)dictionary_getint(d, temp, err_value);
    #endif
    // Parses the vdo_duration from config file
    snprintf(temp, sizeof(temp), "%s:%s", REC_BOOT_CRASH_SECTION_NAME, REC_BOOT_CRASH_VDO_DURATION);
    pRecordBootCrashCfg->vdo_duration = (unsigned int)dictionary_getint(d, temp, err_value);

    // Parses the vdo_quality from config file
    snprintf(temp, sizeof(temp), "%s:%s", REC_BOOT_CRASH_SECTION_NAME, REC_BOOT_CRASH_VDO_QUALITY);
    pRecordBootCrashCfg->vdo_quality = (unsigned char)dictionary_getint(d, temp, err_value);
    
    //Parses the vdo_compression_mode from config file    
    snprintf(temp, sizeof(temp), "%s:%s", REC_BOOT_CRASH_SECTION_NAME, REC_BOOT_CRASH_VDO_COMPRESSION_MODE);
    pRecordBootCrashCfg->vdo_compression_mode = (unsigned char)dictionary_getint(d, temp, err_value);

    // Parses the frame count/sec from config file
    snprintf(temp, sizeof(temp), "%s:%s", REC_BOOT_CRASH_SECTION_NAME, REC_BOOT_CRASH_FRAME_COUNT);
    pRecordBootCrashCfg->frame_count = (unsigned char)dictionary_getint(d, temp, err_value);

    // If any of the pulled records is not proper, then close everything and report error
    if (/*pRecordBootCrashCfg->current_state == err_value ||*/
        pRecordBootCrashCfg->vdo_duration == err_value ||
        pRecordBootCrashCfg->vdo_quality == err_value ||
        pRecordBootCrashCfg->vdo_compression_mode == err_value ||
        pRecordBootCrashCfg->frame_count == err_value)
    {
        TEMERG("Unable to read the values. %s may be corrupt.", AUTORECORD_CFG_FILE);
        iniparser_freedict(d);
        return -2;
    }
    // Closes the loaded config file
    iniparser_freedict(d);
    
    return VDO_RCD_SUCCESS;
    
}
//Fn. to validate User input parameter for record bootcrash feature
int ValidateRecordBootCrashCfg(RecordBootCrashCfg_T * pRecordBootCrashCfg)
{
    int sumVal = 0;

    if( pRecordBootCrashCfg == NULL )
    {
        TDBG("Input struct to validate is empty");
        return VDO_RCD_FAILURE;
    }
    #if 0
    //Enable/disabling is from event selection
    //If event set to monitor will start continuous recording
    if( pRecordBootCrashCfg->current_state == 0 )
    {
        TDBG("\nCommand to disable RecordBootCrash feature\n");
        //No Need to validate other parameters, as request is to disable this feature.
        return VDO_RCD_SUCCESS;
    }
    #endif
    //Check limits for each parameter
    if((pRecordBootCrashCfg->vdo_duration == 0) || ( pRecordBootCrashCfg->vdo_duration > REC_BOOT_CRASH_VDO_DURATION_MAX_LIMIT ) ||
        ( pRecordBootCrashCfg->vdo_duration%10 != 0 ) ||
        ( pRecordBootCrashCfg->vdo_quality > REC_BOOT_CRASH_VDO_QUALITY_MAX_LIMIT ) ||
        ( pRecordBootCrashCfg->vdo_compression_mode > REC_BOOT_CRASH_VDO_COMPRESSION_MODE_MAX_LIMIT ) ||
        ( pRecordBootCrashCfg->frame_count == REC_BOOT_CRASH_FRAME_COUNT_INVALID_VAL) ||
        ( pRecordBootCrashCfg->frame_count > REC_BOOT_CRASH_FRAME_COUNT_MAX_LIMIT))
    {
        TDBG("Invalid Parameter for RecordBootCrash Feature\n");
        return VDO_RCD_FAILURE;
    }

    //To limit recording duration according to given q,c,fps value
    sumVal = pRecordBootCrashCfg->vdo_compression_mode + pRecordBootCrashCfg->frame_count;
    if( sumVal <= REC_BOOT_CRASH_C_F_LOWER_LIMIT )
    {
        return VDO_RCD_SUCCESS; //For lower compression+fps duration upto 60 sec allowed
    }
    else if( sumVal <= REC_BOOT_CRASH_C_F_UPPR_LIMIT )
    {
        if(pRecordBootCrashCfg->vdo_duration <= REC_BOOT_CRASH_VDO_DURATION_AVERAGE_LIMIT )
        {
            return VDO_RCD_SUCCESS; //For compression+fps == 4/5 duration allowed upto 30 sec;
        }
        else
        {
            TDBG("For FPS+Compression == 4/5 ,duration <= 30\n");
            return VDO_RCD_FAILURE;
        }
    }
    else
    {
        if(pRecordBootCrashCfg->vdo_duration != DEF_REC_BOOT_CRASH_VDO_DURATION )
        {
            TDBG("For FPS+Compression > 5 ,duration should not exceed 10sec\n");
            return VDO_RCD_FAILURE; //For compression+fps == 4/5 duration allowed upto 30 sec;
        }
    }
    return VDO_RCD_SUCCESS;
}
int SetRecordBootCrashCfg(RecordBootCrashCfg_T * pRecordBootCrashCfg)
{
    dictionary *d = NULL;
    char temp[MAX_TEMP_ARRAY_SIZE]={0};
    char tempval[MAX_TEMP_ARRAY_SIZE]={0};
    FILE* fp;

    /* check for NULL pointer */
    if( pRecordBootCrashCfg == NULL )
    {
        printf(" Pointer that is passed to set configuration is NULL\n");
        return VDO_RCD_FAILURE;
    }
     d = iniparser_loaddef(AUTORECORD_DEF_CFG_FILE, AUTORECORD_CFG_FILE);
    if( d == NULL )
    {
        TEMERG("Unable to find/load/parse Configuration file : %s", AUTORECORD_CFG_FILE);
        return VDO_RCD_FAILURE;
    }
    #if 0
    sprintf(temp, "%s:%s", REC_BOOT_CRASH_SECTION_NAME, REC_BOOT_CRASH_CURRENT_STATE);
    sprintf(tempval, "%u", pRecordBootCrashCfg->current_state);
    iniparser_setstr(d, temp, tempval);
    #endif
    snprintf(temp, sizeof(temp), "%s:%s", REC_BOOT_CRASH_SECTION_NAME, REC_BOOT_CRASH_VDO_DURATION);
    snprintf(tempval, sizeof(tempval), "%u", pRecordBootCrashCfg->vdo_duration);
    iniparser_setstr(d, temp, tempval);

    snprintf(temp, sizeof(temp), "%s:%s", REC_BOOT_CRASH_SECTION_NAME, REC_BOOT_CRASH_VDO_QUALITY);
    snprintf(tempval, sizeof(tempval), "%u", pRecordBootCrashCfg->vdo_quality);
    iniparser_setstr(d, temp, tempval);

    snprintf(temp, sizeof(temp), "%s:%s", REC_BOOT_CRASH_SECTION_NAME, REC_BOOT_CRASH_VDO_COMPRESSION_MODE);
    snprintf(tempval, sizeof(tempval), "%u", pRecordBootCrashCfg->vdo_compression_mode);
    iniparser_setstr(d, temp, tempval);


    snprintf(temp, sizeof(temp), "%s:%s", REC_BOOT_CRASH_SECTION_NAME, REC_BOOT_CRASH_FRAME_COUNT);
    snprintf(tempval, sizeof(tempval), "%u", pRecordBootCrashCfg->frame_count);
    iniparser_setstr(d, temp, tempval);
    
    fp = fopen(AUTORECORD_CFG_FILE,"w");
        if(fp == NULL)
     {
        TEMERG("Could not open config file %s to set config\n", AUTORECORD_CFG_FILE);
        iniparser_freedict(d);
        return VDO_RCD_FAILURE;
     }
    iniparser_dump_ini(d,fp);
    fclose(fp);
    iniparser_freedict(d);    
    return VDO_RCD_SUCCESS;
}

int GetAutoRecordCfg(AutoRecordCfg_T* pAutoRecordCfg)
{
    dictionary *d = NULL;
    unsigned int err_value = 0xFFFFFF;
    char *str = NULL;
    char temp[MAX_TEMP_ARRAY_SIZE]={0};

    // Loads the autorecord configuration file 
    d = iniparser_loaddef(AUTORECORD_DEF_CFG_FILE, AUTORECORD_CFG_FILE);
    if( d == NULL )
    {
        TEMERG("Unable to find/load/parse Configuration file : %s", AUTORECORD_CFG_FILE);
        return -1;
    }

    // Parses the File path
    snprintf(temp,MAX_TEMP_ARRAY_SIZE, "%s:%s", AUTORECORD_SECTION_NAME, RECORD_FILE_PATH);
    str = dictionary_get(d, temp, NULL);    
    if (str != NULL)
    {
        snprintf(pAutoRecordCfg->path, sizeof(pAutoRecordCfg->path), "%s", str);
    }
    // Parses the max time of dump file
    snprintf(temp,MAX_TEMP_ARRAY_SIZE,"%s:%s", AUTORECORD_SECTION_NAME, MAX_TIME);
    pAutoRecordCfg->max_time = dictionary_getint(d, temp, err_value);
    // Parses the max size of dump file
    snprintf(temp,MAX_TEMP_ARRAY_SIZE,"%s:%s", AUTORECORD_SECTION_NAME, MAX_SIZE);
    pAutoRecordCfg->max_size = dictionary_getint(d, temp, err_value);

    // Parses the max number of dump files
    snprintf(temp,MAX_TEMP_ARRAY_SIZE, "%s:%s", AUTORECORD_SECTION_NAME, MAX_DUMP_FILES);
    pAutoRecordCfg->max_dumps = dictionary_getint(d, temp, err_value);
    // Parses the remote path support
    snprintf(temp,MAX_TEMP_ARRAY_SIZE,"%s:%s", AUTORECORD_SECTION_NAME, REMOTE_PATH_SUPPORT);
    pAutoRecordCfg->remote_path_support = dictionary_getint(d, temp, err_value);

    // Parses the Remote IP Address
    snprintf(temp,MAX_TEMP_ARRAY_SIZE,"%s:%s", AUTORECORD_SECTION_NAME, REMOTE_IP);
    str  = iniparser_getstr(d, temp);
    if (str != NULL)
    {
        if (strlen(str) <= MAX_IP_LEN)
        {
            snprintf(pAutoRecordCfg->ip_addr,sizeof(pAutoRecordCfg->ip_addr), "%s", str);
        }
        else
        {
            //IP exceeded maximum length
            TCRIT ("IP Address Exceeds Length\n");
        }
    }

    // Parses the Remote user Name
    snprintf(temp,MAX_TEMP_ARRAY_SIZE,"%s:%s", AUTORECORD_SECTION_NAME, REMOTE_USER_NAME);
    str  = iniparser_getstr(d, temp);
    if (str != NULL)
    {
        if (strlen(str) <= MAX_RMS_USERNAME_LEN)
        {
            snprintf(pAutoRecordCfg->username, sizeof(pAutoRecordCfg->username), "%s", str);
        }
        else
        {
            //Username exceeded maximum length
            TCRIT ("User Name Exceeds Length\n");
        }
    }

    // Parses the Remote user password
    snprintf(temp,MAX_TEMP_ARRAY_SIZE, "%s:%s", AUTORECORD_SECTION_NAME, REMOTE_USER_PASS);
    str  = iniparser_getstr(d, temp);
    if (str != NULL)
    {
        if (strlen(str) <= MAX_RMS_PASSWORD_LEN)
        {
            snprintf(pAutoRecordCfg->password, sizeof(pAutoRecordCfg->password), "%s", str);
        }
        else
        {
            //Password exceeded maximum length
            TCRIT ("Password Exceeds Length\n");
        }
    }

    // Parses the Remote machine share type
    snprintf(temp,MAX_TEMP_ARRAY_SIZE, "%s:%s", AUTORECORD_SECTION_NAME, REMOTE_SHARE_TYPE);
    str  = iniparser_getstr(d, temp);
    if (str != NULL)
    {
        if (strlen(str) <= MAX_SHARE_TYPE_LEN)
        {
            snprintf(pAutoRecordCfg->share_type, sizeof(pAutoRecordCfg->share_type), "%s", str);
        }
        else
        {
            //Share type exceeded maximum length
            TCRIT ("Share type Exceeds Length\n");
        }
    }

    // Parses the Remote machine domain
    snprintf(temp,MAX_TEMP_ARRAY_SIZE, "%s:%s", AUTORECORD_SECTION_NAME, REMOTE_DOMAIN);
    str  = iniparser_getstr(d, temp);
    if (str != NULL)
    {
        if (strlen(str) <= MAX_DOMAIN_LEN)
        {
            snprintf(pAutoRecordCfg->domain, sizeof(pAutoRecordCfg->domain), "%s", str);
        }
        else
        {
            //Domain exceeded maximum length
            TCRIT ("Domain Exceeds Length\n");
        }
    }

    // Parses the mount status
    snprintf(temp,MAX_TEMP_ARRAY_SIZE, "%s:%s", AUTORECORD_SECTION_NAME, MOUNT_STATUS);
    pAutoRecordCfg->mnt_successful= dictionary_getint(d, temp, err_value);

    // If any of the pulled records is not proper, then close everything and report error
    if (pAutoRecordCfg->path == NULL ||
        pAutoRecordCfg->max_time == err_value ||
        pAutoRecordCfg->max_size == err_value ||
        pAutoRecordCfg->max_dumps == err_value||
        pAutoRecordCfg-> remote_path_support == err_value)
    {
        TEMERG("Unable to read the values. %s may be corrupt.", AUTORECORD_CFG_FILE);
        iniparser_freedict(d);
        return -2;
    }

    // Closes the loaded config file
    iniparser_freedict(d);
    
    return 0;
}

int SetAutoRecordCfg(AutoRecordCfg_T* pAutoRecordCfg)
{
	int retval = 0 ;
	int mntretval = -0;

	unMount(DEF_VDO_DUMP_PATH);
	if (pAutoRecordCfg->remote_path_support == ENABLED)
	{
		//mount remote path if support is enabled
		pAutoRecordCfg->mnt_successful	=  MountRemotePath(pAutoRecordCfg,DEF_VDO_DUMP_PATH);

		if(pAutoRecordCfg->mnt_successful  !=0)
		{
			mntretval =  MOUNT_FAILED;
		}
	}

	if ((pAutoRecordCfg->remote_path_support == DISABLED) || (mntretval !=0))
	{
		//reset values to default  either mount failed or  remote path support disabled
		pAutoRecordCfg->max_time = DEF_MAX_VDO_DUMP_TIME ;
		pAutoRecordCfg->max_dumps = DEF_MAX_VDO_DUMP;
		pAutoRecordCfg->max_size = DEF_MAX_VDO_DUMP_SIZE;
		pAutoRecordCfg->mnt_successful = -1;
	}

	retval = SaveAutoRecordCfg(pAutoRecordCfg);
	if(retval != 0)
	{
		//config saving to file failed so unmount
		if(mntretval == 0)
		{
			unMount(DEF_VDO_DUMP_PATH);
		}
	}
	return (retval +mntretval);
}
int SaveAutoRecordCfg(AutoRecordCfg_T* pAutoRecordCfg)
{
    FILE *fp=NULL;
    dictionary *d = NULL;
    char temp[MAX_TEMP_ARRAY_SIZE]={0};
    char tempval[MAX_TEMP_ARRAY_SIZE]={0};
    
    if( pAutoRecordCfg == NULL )
        {
        printf(" Pointer that is passed to set configuration is NULL\n");
        return -1;
        }

        //d = iniparser_load(AUTORECORD_CFG_FILE);

         d = iniparser_loaddef(AUTORECORD_DEF_CFG_FILE, AUTORECORD_CFG_FILE);
        if( d == NULL )
        {
        printf("Unable to find/load/parse Configuration file : %s", AUTORECORD_CFG_FILE);
        return -1;
        }
    // Add the File path
    memset(temp,0,sizeof(temp));
    snprintf(temp, sizeof(temp), "%s:%s", AUTORECORD_SECTION_NAME, RECORD_FILE_PATH);
    iniparser_setstr(d, temp, pAutoRecordCfg->path);

    // Add the max time of dump file
    snprintf(temp, sizeof(temp), "%s:%s", AUTORECORD_SECTION_NAME, MAX_TIME);
    snprintf(tempval, sizeof(tempval), "%u", pAutoRecordCfg->max_time);
    iniparser_setstr(d, temp, tempval);

    // Add the max size of dump file
    snprintf(temp, sizeof(temp), "%s:%s", AUTORECORD_SECTION_NAME, MAX_SIZE);
    snprintf(tempval, sizeof(tempval), "%u", pAutoRecordCfg->max_size);
    iniparser_setstr(d, temp, tempval);

    // Add the max number of dump files
    snprintf(temp, sizeof(temp), "%s:%s", AUTORECORD_SECTION_NAME, MAX_DUMP_FILES);
    snprintf(tempval, sizeof(tempval), "%u", pAutoRecordCfg->max_dumps);
    iniparser_setstr(d, temp, tempval);

    // Add the remote path support
    snprintf(temp, sizeof(temp), "%s:%s", AUTORECORD_SECTION_NAME, REMOTE_PATH_SUPPORT);
    snprintf(tempval, sizeof(tempval), "%u", pAutoRecordCfg->remote_path_support);
    iniparser_setstr(d, temp, tempval);

    // Add the Remote IP Address
    pAutoRecordCfg->ip_addr[strlen(pAutoRecordCfg->ip_addr)] = 0;
    snprintf(temp, sizeof(temp), "%s:%s", AUTORECORD_SECTION_NAME, REMOTE_IP);
    iniparser_setstr(d, temp, pAutoRecordCfg->ip_addr);

    // Add the Remote user Name
    pAutoRecordCfg->username[strlen(pAutoRecordCfg->username)] = 0;
    snprintf(temp, sizeof(temp), "%s:%s", AUTORECORD_SECTION_NAME, REMOTE_USER_NAME);
    iniparser_setstr(d, temp, pAutoRecordCfg->username);

    // Add the Remote user password
    pAutoRecordCfg->password[strlen(pAutoRecordCfg->password)] = 0;
    snprintf(temp, sizeof(temp), "%s:%s", AUTORECORD_SECTION_NAME, REMOTE_USER_PASS);
    iniparser_setstr(d, temp, pAutoRecordCfg->password);
 
    // Add the Remote machine share type
    pAutoRecordCfg->share_type[strlen(pAutoRecordCfg->share_type)] = 0;
    snprintf(temp, sizeof(temp), "%s:%s", AUTORECORD_SECTION_NAME, REMOTE_SHARE_TYPE);
    iniparser_setstr(d, temp, pAutoRecordCfg->share_type);
    
    // Add the Remote machine domain
    pAutoRecordCfg->domain[strlen(pAutoRecordCfg->domain)] = 0;
    snprintf(temp, sizeof(temp), "%s:%s", AUTORECORD_SECTION_NAME, REMOTE_DOMAIN);
    iniparser_setstr(d, temp, pAutoRecordCfg->domain);
    
    // Add the mount status
    snprintf(temp, sizeof(temp), "%s:%s", AUTORECORD_SECTION_NAME, MOUNT_STATUS);
    snprintf(tempval, sizeof(tempval), "%d", pAutoRecordCfg->mnt_successful);
    iniparser_setstr(d, temp, tempval);

    /* open the configuration file to write */
    fp = fopen(AUTORECORD_CFG_FILE,"w");
    if( fp == NULL )
    {
        TEMERG("Unable to load Configuration file for writing: %s", AUTORECORD_CFG_FILE);
        iniparser_freedict(d);
        return -2;
    }
    iniparser_dump_ini(d,fp);
    fclose(fp);
    iniparser_freedict(d);
    
    return 0;

}

int SetFileAccess(char *FileName)
{
	char DataFile[MAX_FILE_NAME_LEN] = {0};
    int RetVal = 0;

    if (!sem_inited)
    {
        RetVal = sem_init(&file_lock_sem, 0, 0);
        if (RetVal < 0)
        {
            TDBG("Unable to Initialize the File Lock Semaphore\n");
            return ACCESS_DENIED;
        }
    
        sem_inited = 1;
    }

    // Prepare our Data File path and the Lock File path
	snprintf(DataFile,MAX_FILE_NAME_LEN,"%s/%s", AUTORECORD_WWW_PATH, FileName); 

    // Check if the Data file is available. 
    // If the source data file is not available, return appropriate error
    if (access(DataFile, F_OK) < 0)
    {
        TEMERG("Data File Not Available. Possible Deleted or Corruption in FileSystem...\n");
        return FILE_NOT_AVAILABLE;
    }

    // Raise the flag to indicate that the file is being used by some Process
    RetVal = sem_post(&file_lock_sem);
    if (RetVal < 0)
    {
        TDBG("Unable to Post the File Lock. File will not be locked..\n");
        return ACCESS_DENIED;
    }
        
    return ACCESS_GRANTED;
}

int ResetFileAccess(char *FileName)
{
    int RetVal = 0;
    int semval = 0;
    if(0){
        FileName=FileName;
    }
    // Check the flag for that file, to identify if it is indeed being used by any process
    RetVal = sem_getvalue(&file_lock_sem, &semval);
    if (RetVal < 0)
    {
        TDBG("Unable to Fetch the Lock Status. Possible corruption !!\n");
        return FILE_LOCKED;
    }
    
    // If being used by some process, then bring down the flag to reset the lock
    // If a false call is made, then just ignore it.
    if (semval)
    {
        TDBG ("Resetting the file lock\n");
        sem_wait(&file_lock_sem);
    }
    else
    {
        TDBG ("No lock currently held. Ignoring...\n");
    }
    
    return VIDEO_FILE_SUCCESS;
}

int DeleteDataFile(char *FileName)
{
    char Folder[MAX_FILE_NAME_LEN] = {0};
    char DataFile[MAX_PATH_NAME_LEN] = {0};
    char *file = NULL;  
    AutoRecordCfg_T    AutoRecordCfg ;
    int RetVal = 0;
    int loop = 0;
    int semval = 0;
    int semlocked = 0;
    sem_t *semaphore;

    memset((char *)&AutoRecordCfg, 0, sizeof(AutoRecordCfg_T));
    if (GetAutoRecordCfg(&AutoRecordCfg) < 0)
    {
        TCRIT ("Unable to Fetch the Configuration parameters. Possible Corruption\n");
        memcpy(AutoRecordCfg.path, DEF_VDO_DUMP_PATH, strlen(DEF_VDO_DUMP_PATH));
    }

    if( GetVideoFolder(&Folder[0],GET_FOLDER_NAME) !=0)
    {
        TCRIT ("Unable to Fetch the video folder name\n");
        return FILE_NOT_AVAILABLE;
    }

    //find file name comes with folder
    file = strrchr(FileName,'/');
    if(file == NULL)
    {
        file = FileName;
    }

    // Prepare our Data File Path
    RetVal = snprintf(DataFile,MAX_PATH_NAME_LEN, "%s/%s",Folder, file);
    if(RetVal > MAX_PATH_NAME_LEN)
    {
        TCRIT ("Buffer OverFlow\n");
        return FILE_NOT_AVAILABLE;
    }
    DataFile[MAX_PATH_NAME_LEN -1] = '\0';

    // Fetch the status of the file lock
    RetVal = sem_getvalue(&file_lock_sem, &semval);
    if (RetVal < 0)
    {
        TDBG ("Unable to fetch the status of the File Lock. Please retry again later.\n");
        return FILE_LOCKED;
    }
    
    // If file is being used by some other process, then don't proceed with Delete Operation
    if (semval)
    {
        TDBG ("File being used by some other Process. Please retry again later\n");
        return FILE_LOCKED;
    }    

    // Control here means, no process is using this data file, and it is safe to be deleted
    
    // Check if the Data file is available. 
    // If the source data file is not available, return appropriate error
    if (access(DataFile, F_OK) < 0)
    {
        TCRIT ("Data File Not Available. Possible Deleted or Corruption in FileSystem...\n");
        return FILE_NOT_AVAILABLE;
    }

    //validate semaphore lock for video recording
    semaphore = CreateSemaphore(FileName);

    semlocked = GetSemaphore(semaphore);
    //If semlock is 0 then some other process is locked the file
    if (semlocked == 0)
    {
        TDBG ("File being used by Recording Process. Please retry again later\n");
        return FILE_LOCKED;
    }

    // Delete the data file, with a retry mechanism of 5 times to delete it.
    for (loop = 0; loop < MAX_RETRIES; loop++)
    {
        RetVal = unlink(DataFile);
        if (RetVal == 0)
        {
            TDBG ("Deleted the appropriate Data File.\n");
            break;
        }
        
        sleep(1);
    }

    if (RetVal != 0)
    {
        TDBG ("Unable to Delete the Data File.\n");
        return FAILED;
    }

    return VIDEO_FILE_SUCCESS;
}

int GetPreEventFileCount()
{
    AutoRecordCfg_T    AutoRecordCfg;
    int FileCount = 0,loop = 0,maxFileCount = 0;
    char preResetDump[REC_FILE_PATH_SIZE] = {0};
    char preCrashDump[REC_FILE_PATH_SIZE] = {0};
    
    // Load the Configuration from the File
    if (GetAutoRecordCfg(&AutoRecordCfg) < 0)
    {
        TDBG("Unable to Fetch the Configuration parameters. Possible Corruption\n");
        memcpy(AutoRecordCfg.path, DEF_VDO_DUMP_PATH, strlen(DEF_VDO_DUMP_PATH));
        AutoRecordCfg.max_time = DEF_MAX_VDO_DUMP_TIME;
        AutoRecordCfg.max_size = DEF_MAX_VDO_DUMP_SIZE;
        AutoRecordCfg.max_dumps = DEF_MAX_VDO_DUMP;
    }
    if((AutoRecordCfg.remote_path_support != 1) || (AutoRecordCfg.mnt_successful !=0))
    {
        //If no remote path will store only 1 pre-event video recording
        maxFileCount = DEF_REC_FILE_COUNT;
    }
    else
    {
        //If remotepath mounted will display upto 3 pre-event video recording 
        maxFileCount = MAX_REC_FILE_COUNT;
    }

    for(loop = 0; loop < maxFileCount; loop++)
    {
        snprintf(preCrashDump, sizeof(preCrashDump), "%s/%s%d.dat", DEF_CRASH_VIDEO_FILE_PATH, PRE_CRASH_VIDEO_DUMP_PREFIX,loop);
        snprintf(preResetDump, sizeof(preCrashDump), "%s/%s%d.dat", DEF_CRASH_VIDEO_FILE_PATH, PRE_RESET_VIDEO_DUMP_PREFIX,loop);
        if (access(&preCrashDump[0], F_OK) == 0)
        {
            FileCount++;
        }

        if (access(&preResetDump[0], F_OK) == 0)
        {
            FileCount++;
        }
    }
    return FileCount;
}

int GetFileCount()
{
    AutoRecordCfg_T    AutoRecordCfg;
    int FileCount = 0;
    unsigned int loop = 0;
    char DataFile[MAX_FILE_NAME_LEN] = {0};

    // Clear off the junk in the File Structure
    memset((char *)&AutoRecordCfg, 0, sizeof(AutoRecordCfg_T));

    // Load the Configuration from the File
    if (GetAutoRecordCfg(&AutoRecordCfg) < 0)
    {
        TDBG("Unable to Fetch the Configuration parameters. Possible Corruption\n");
        memcpy(AutoRecordCfg.path, DEF_VDO_DUMP_PATH, strlen(DEF_VDO_DUMP_PATH));
        AutoRecordCfg.max_time = DEF_MAX_VDO_DUMP_TIME;
        AutoRecordCfg.max_size = DEF_MAX_VDO_DUMP_SIZE;
        AutoRecordCfg.max_dumps = DEF_MAX_VDO_DUMP;
    }

    // Loop to the possible number of dump files and identify if they are available
    // If available, update the FileCfg Structure, else just continue until loop ends.
    for (loop = 0; loop < AutoRecordCfg.max_dumps; loop++)
    {
        // Frame the File with its full path
        snprintf(DataFile,MAX_FILE_NAME_LEN, "%s/%s%d.dat", DEF_VDO_DUMP_PATH, VDO_DUMP_FILE, loop);

        // Check if the file is Available first.
        if (access(DataFile, F_OK) < 0)
        {
            TDBG("File does not exist\n");
            continue;
        }

        FileCount++;
    }

    return FileCount;
}

int GetFileInfo(void *pFileStructure)
{
    unsigned char flag=0;
    AutoRecordFile_T*    pRecordFile = NULL;
	char DataFile[MAX_FILE_NAME_LEN] = {0};
    AutoRecordCfg_T    AutoRecordCfg;
    int RetVal = 0;
    unsigned int loop = 0,maxFileCount = 0;
    struct stat FileStat;
    char *pTimeStamp = NULL;
    char preResetDump[REC_FILE_PATH_SIZE] = {0};
    char preCrashDump[REC_FILE_PATH_SIZE] = {0};

    // Clear off the junk in the File Structure
    memset((char *)&AutoRecordCfg, 0, sizeof(AutoRecordCfg_T));

    // Load the Configuration from the File
    if (GetAutoRecordCfg(&AutoRecordCfg) < 0)
    {
        TDBG("Unable to Fetch the Configuration parameters. Possible Corruption\n");
        memcpy(AutoRecordCfg.path, DEF_VDO_DUMP_PATH, strlen(DEF_VDO_DUMP_PATH));
    }

    pRecordFile = (AutoRecordFile_T*)(pFileStructure);
    memset((char *)pFileStructure, 0, sizeof(AutoRecordFile_T));

    // Loop to the possible number of dump files and identify if they are available
    // If available, update the FileCfg Structure, else just continue until loop ends.
    for (loop = 0; loop < AutoRecordCfg.max_dumps; loop++)
    {
        // Frame the File with its full path
		snprintf(DataFile,MAX_FILE_NAME_LEN,"%s/%s%d.dat", DEF_VDO_DUMP_PATH, VDO_DUMP_FILE, loop);

        // Check if the file is Available first.
        if (access(DataFile, F_OK) < 0)
        {
            TDBG("File does not exist !!!\n");
            continue;
        }

        // File is available. Getting the Statistics for that file.
        RetVal = stat(DataFile, &FileStat);
        if (RetVal < 0)
        {
            TCRIT("Unable to fetch the File Information\n");
            continue;
        }

        // Fill in the data structure with the actual data
        snprintf(pRecordFile->FileName, sizeof(pRecordFile->FileName), "%s%d.dat", VDO_DUMP_FILE, loop);

        pTimeStamp = ctime(&FileStat.st_mtime);
		if(pTimeStamp == NULL)
		{
            	    TCRIT("Error in representing calendar time \n");
            	    continue;
		}
        snprintf(pRecordFile->TimeStamp, sizeof(pRecordFile->TimeStamp), "%s", pTimeStamp);

        // Increment to next in list    
        pRecordFile++;
    }
    if(IsFeatureEnabled("CONFIG_SPX_FEATURE_PRE_EVENT_VIDEO_RECORDING"))
    {
        if((AutoRecordCfg.remote_path_support != 1) || (AutoRecordCfg.mnt_successful !=0))
        {
            //If no remote path will store only 1 pre-event video recording
            maxFileCount = DEF_REC_FILE_COUNT;
        }
        else
        {
            //If remotepath mounted will display upto 3 pre-event video recording 
            maxFileCount = MAX_REC_FILE_COUNT;
        }

        for(loop = 0; loop < maxFileCount; loop++)
        {
            snprintf(preCrashDump, sizeof(preResetDump), "%s/%s%d.dat", DEF_CRASH_VIDEO_FILE_PATH, PRE_CRASH_VIDEO_DUMP_PREFIX,loop);
            snprintf(preResetDump, sizeof(preResetDump), "%s/%s%d.dat", DEF_CRASH_VIDEO_FILE_PATH, PRE_RESET_VIDEO_DUMP_PREFIX,loop);
            if (access(preCrashDump, F_OK) == 0)
            {
                RetVal = stat(preCrashDump, &FileStat);
                if (RetVal == 0)
                {
                    strncpy(DataFile,PRE_CRASH_VIDEO_DUMP_PREFIX,REC_FILE_PATH_SIZE);
                    flag = 1;
                }
            }
            
            if ( access(preResetDump, F_OK) == 0 )
            {
                RetVal = stat(preResetDump, &FileStat);
                if (RetVal == 0)
                {
                    strncpy(DataFile,PRE_RESET_VIDEO_DUMP_PREFIX,REC_FILE_PATH_SIZE);
                    flag = 1;
                }
            }
            if( flag != 1 )
            {
                flag = 0;
                continue;
            }
            // Fill in the data structure with the actual data
            snprintf(pRecordFile->FileName, sizeof(pRecordFile->FileName), "%s%d.dat", DataFile, loop);
            pTimeStamp = ctime(&FileStat.st_mtime);
    		if(pTimeStamp == NULL)
    		{
                    TCRIT("Error in representing calendar time \n");
                    continue;
    		}
            snprintf(pRecordFile->TimeStamp, sizeof(pRecordFile->TimeStamp), "%s", pTimeStamp);

            // Increment to next in list    
            pRecordFile++;
            flag = 0;
        }

    }

    return 0;
}    

static void ReplaceChar (char * String, char FindChar, char ReplaceWith)
{
    char * pChar;
    if (NULL == String)
    {
        //Invalid string
        return;
    }

    //Find the charater to be repalced in the string
    pChar = strchr(String, FindChar);
    while (NULL != pChar)
    {
        //Replace the found character
        *pChar = ReplaceWith;

        //Find the next instance of character in string
        pChar = strchr(String, FindChar);
    }
    return;
}

//Releases file lock when adviser restart
void releaseFileLock()
{
    int loop = 0;
    sem_t *semaphore;
    int semlocked =-1;
    char filename[40] = {0};
	char DataFile[MAX_FILE_NAME_LEN] = {0};
    
    for (loop = 0; loop < DEF_MAX_VDO_DUMP; loop++)    
    {
        memset(filename,0,strlen(filename));
        memset(DataFile,0,strlen(DataFile));
        
        snprintf(filename, sizeof(filename), "%s%d.dat",VDO_DUMP_FILE, loop);
        // Prepare our Data File Path
		snprintf(DataFile,MAX_FILE_NAME_LEN,"%s/%s", DEF_VDO_DUMP_PATH, filename);

        if (access(DataFile, F_OK) >= 0)
        {
            
            semaphore = CreateSemaphore(filename);
            if(semaphore == NULL)
            {
                TCRIT(" Not able to getSemaphore\n");
                continue;
            }

            semlocked = GetSemaphore(semaphore);
            //if semlocked is zero file is locked release it
             if (semlocked == 0)
            {
                ReleaseSemaphore(semaphore,&filename[0]);
             }
        }
    }
    return;
}

int unMount(const char *mountPath)
{
    int retval = 0;
    
    unMountPath(mountPath);


    retval = updateMntStatus(UNMOUNTED);

    return retval;    
}

int unMountPath(const char *mountPath)
{
    char cmd[MAX_CMD_NAME_LEN] = {0};
    char path[MAX_PATH_NAME_LEN] = {0};
    int retry = 0 ;
    int status = -1;
    int len;
    
    if (snprintf (path, MAX_PATH_NAME_LEN, "%s", mountPath) >= MAX_PATH_NAME_LEN)
    {
        TCRIT("Buffer Overflow\n");
        path[MAX_PATH_NAME_LEN -1] = 0;
    }
    
    len = strlen(path);
    if(path[len-1] == '/')
    	path[len-1] = '\0';
    
    status = IsMounted(path, NULL, NULL);
    if(status == 0)
    	return 0;
    
    
    for ( retry = 0 ;retry < MAX_RETRIES ;retry++)
    {
	    snprintf(cmd, MAX_CMD_NAME_LEN,"umount -l %s", mountPath);
        status = system (cmd);
        if(0 == status)
        {
            //Mount Successful
            return status;
        }
    }
    return -1;
}

int updateMntStatus(int status)
{
    AutoRecordCfg_T    AutoRecordCfg;

    // Clear off the junk in the File Structure
    memset((char *)&AutoRecordCfg, 0, sizeof(AutoRecordCfg_T));

    if((status != UNMOUNTED) && (status != 0))
        return -1;

    // Load the Configuration from the File
    if (GetAutoRecordCfg(&AutoRecordCfg) < 0)
    {
        TDBG("Unable to Fetch the Configuration parameters. Possible Corruption\n");
        return -1;
    }
    AutoRecordCfg.mnt_successful =  status;
    if(SaveAutoRecordCfg(&AutoRecordCfg)< 0)
    {
        TDBG("Unable to set the Configuration parameters. \n");
        return -1;
    }
    return 0;
    
}
int MountRemotePath(AutoRecordCfg_T	*AutoRecordCfg,const char *mountPath)
{

    char RemotePath [MAX_PATH_NAME_LEN] = {0};
    int RetVal = -1;
    int len =0, size=sizeof (RemotePath) ,ret;
    
    memset (RemotePath, 0, MAX_PATH_NAME_LEN);
    unMountPath(mountPath);
    
    //if remote support is not enabled return from here
    if(AutoRecordCfg->remote_path_support == 0){
        return RetVal;
    }

    if (NULL == AutoRecordCfg->path)
    {
        //Cannot Mount
        return RetVal;
    }
    
    //Windows
    if ((0 == strcmp(SHARE_TYPE_CIFS, AutoRecordCfg->share_type)) ||
        (0 == strcmp(SHARE_TYPE_UNKNOWN, AutoRecordCfg->share_type)))
    {
        len = snprintf (RemotePath,MAX_PATH_NAME_LEN, "\"//%s",AutoRecordCfg->ip_addr);
        if (len >= MAX_PATH_NAME_LEN)
        {
        	TCRIT("buffer overflow");
        }

        //append / before  folder name if not given
        if( (AutoRecordCfg->path[0] != '\\') && (AutoRecordCfg->path[0] !='/') )
        {
        	ret=0;
            ret = snprintf (RemotePath+len, (size>len)?(size-len):0, "/");
            if (ret >= ((size>len)?(size-len):0))
            {
            	len += (size>len)?(size-len):0;
            }
            else
            {
            	len += ret;
            }
        }

        //Remove the last slash and copy
        if(snprintf (RemotePath+len, (size>len)?(size-len):0, "%s\"", AutoRecordCfg->path)>=(size>len)?(size-len):0){
            printf("buffer overflow");
        }

        //Replace backslash with slash for linux format
        //path should be like "//server_ip/share" 
        ReplaceChar (RemotePath, '\\', '/');    

        RetVal = MountShare (AutoRecordCfg,RemotePath,mountPath);
        if (0 != RetVal)
        {
            printf ("Accessing Samba Share failed\n");
            return RetVal;
        }
    }
    //Linux
    if (((0 == strcmp (SHARE_TYPE_NFS, AutoRecordCfg->share_type) ||
        (0 == strcmp (SHARE_TYPE_UNKNOWN, AutoRecordCfg->share_type))) && (0 != RetVal)))
    {
        len=snprintf (RemotePath,MAX_PATH_NAME_LEN,"\"%s:", AutoRecordCfg->ip_addr);

        //append / before  folder name if not given
        if( (AutoRecordCfg->path[0] != '\\') && (AutoRecordCfg->path[0] !='/') )
        {
        	ret = 0;
            ret = snprintf (RemotePath+len, (size>len)?(size-len):0, "/");
            if (ret >= ((size>len)?(size-len):0))
            {
            	len += (size>len)?(size-len):0;
            }
            else
            {
            	len += ret;
            }
        }
        
        if(snprintf (RemotePath+len, (size>len)?(size-len):0, "%s\"", AutoRecordCfg->path)>=(size>len)?(size-len):0){
            printf("buffer overflow");
        }

        //Replace backslash with slash for linux format
        //Path should be like "server_ip:/share"
        ReplaceChar (RemotePath, '\\', '/');
        RetVal = MountShare (AutoRecordCfg,RemotePath,mountPath);
        if (0 != RetVal)
        {
            printf ("Accessing nfs Share failed\n");
            return RetVal;
        }
    }
    return RetVal;
}

int MountShare (AutoRecordCfg_T *pAutoRecordCfg, const char *RemotePath,const char *mountPath)
{
    char MountCmd [MAX_CMD_NAME_LEN]={0};
    int MntStatus = -1;
    int len = 0, size=sizeof(MountCmd) ,ret;

	if( (RemotePath==NULL) || (mountPath==NULL) )
	{
		return MntStatus;
	}	
	
	memset (MountCmd, 0, MAX_CMD_NAME_LEN);

    //NFS Mount
    if ((0 == strcmp(SHARE_TYPE_NFS, pAutoRecordCfg->share_type)) ||
        (0 == strcmp(SHARE_TYPE_UNKNOWN, pAutoRecordCfg->share_type)))
    {
		snprintf (MountCmd,MAX_CMD_NAME_LEN,"mount -t nfs -o rw,nolock,vers=2  %s %s", RemotePath, mountPath);

        MntStatus = system (MountCmd);
        if(0 != MntStatus)
        {
            //MntStatus = WEXITSTATUS(MntStatus);
            //Mount failed
            printf ("NFS mount failed\n");
        }
        return MntStatus;
    }

    //CIFS Mount
    if ((0 == strcmp (SHARE_TYPE_CIFS, pAutoRecordCfg->share_type)) ||
        (0 == strcmp (SHARE_TYPE_UNKNOWN, pAutoRecordCfg->share_type)))
    {
	// added noserverino to prevent EOVERFLOW
	len = snprintf (MountCmd, MAX_CMD_NAME_LEN, "mount -t cifs -o rw,nolock,noserverino");

        if (pAutoRecordCfg->username[0] != '\0')
        {
        	ret=0;
            ret = snprintf (MountCmd+len, (size>len)?(size-len):0,",username=%s", pAutoRecordCfg->username);
            if (ret >= ((size>len)?(size-len):0))
            {
            	len += (size>len)?(size-len):0;
            }
            else
            {
            	len += ret;
            }
        }
        if (pAutoRecordCfg->password[0] != '\0')
        {
        	ret=0;
            ret = snprintf (MountCmd+len, (size>len)?(size-len):0,",password=%s", pAutoRecordCfg->password);
            if (ret >= ((size>len)?(size-len):0))
            {
            	len += (size>len)?(size-len):0;
            }
            else
            {
            	len += ret;
            }
        }
        else
        {
        	ret=0;
            ret = snprintf (MountCmd+len, (size>len)?(size-len):0,",guest");
            if (ret >= ((size>len)?(size-len):0))
            {
            	len += (size>len)?(size-len):0;
            }
            else
            {
            	len += ret;
            }
        }
        if (pAutoRecordCfg->domain[0] != '\0')
        {
        	ret=0;
            ret = snprintf (MountCmd+len, (size>len)?(size-len):0,",domain=%s", pAutoRecordCfg->domain);
            if (ret >= ((size>len)?(size-len):0))
            {
            	len += (size>len)?(size-len):0;
            }
            else
            {
            	len += ret;
            }
        }
        /*Don't remove the space between two conversion specifiers*/
        ret=0;
        ret = snprintf (MountCmd+len, (size>len)?(size-len):0, " %s %s", RemotePath,mountPath);
        if (ret >= ((size>len)?(size-len):0))
        {
        	len += (size>len)?(size-len):0;
        }
        else
        {
        	len += ret;
        }
        if ((size-len) < 0 )
            TCRIT ("Destination string has less buffer size or source string not null terminated\n");

        printf ("Trying CIFS mount\n");
        MntStatus = system (MountCmd);
        if(0 != MntStatus)
        {
            //MntStatus = WEXITSTATUS(MntStatus);
            //Mount failed
            printf ("CIFS mount failed\n");
        }
        return MntStatus;
    }
    //Cannot Mount
    return MntStatus;
}

/*
 * fn : GetVideoRecordStatus
 * return : 0 - Video Record is not going on.
 *          1 - Video record is going on.
 */
unsigned char GetVideoRecordStatus ()
{
    unsigned char status = 0;
    FILE *fp = fopen(AUTORECORD_STATUS_FILE, "r");
    if (fp != NULL)
    {
        fscanf (fp, "%hhu", (unsigned char *)&status);
        fclose(fp);
    }
    return status;
}

/*
* endsWith : verified file name ends with given string
* prefix : search string
* str: string
* return value : 0 on Not match : 1 on  file match
*/
static int endsWith(const char *str, const char *suffix) {
    if (!str || !suffix) {
        return 0;
    }
    size_t lenstr = strlen(str);
    size_t lensuffix = strlen(suffix);
    return (lensuffix > lenstr) ? 0 : strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}

/*
* startsWith : verified file name starts with given string
* prefix : search string
* str: string
* return value : 0 on Not match : 1 on  file match
*/
static int startsWith(const char *prefix, const char *str) {
    if (!prefix || !str) {
        return 0;
    }
    size_t lenprefix = strlen(prefix);
    size_t lenstr = strlen(str);
    return lenstr < lenprefix ? 0 : strncmp(prefix, str, lenprefix) == 0;
}

/*
* isDatFile : Filters dat files from file list
* pnamelist : dirent structure
* return value : 0 on Not match : 1 on Dat file match
*/
static int isDatFile(const struct dirent *pfilelist) {
    int record_pre_boot_or_crash_video = IsFeatureEnabled("CONFIG_SPX_FEATURE_PRE_EVENT_VIDEO_RECORDING");

    if (pfilelist == NULL) {
        return 0;
    }

    // skip "." ".." or 'hidden' files ".login" etc.
    // Skip directories too
    if ((pfilelist->d_name[0] == '.') || (pfilelist->d_type == DT_DIR)){
        return 0;
    }

    if (ENABLED  == record_pre_boot_or_crash_video) {
        // checking if file has name matching video_dump_*.dat(or) 
        //pre_crash_video_*.dat (or) pre_reset_video_*.dat
        if ((startsWith(VDO_DUMP_FILE, pfilelist->d_name) ||
            startsWith(PRE_CRASH_VIDEO_DUMP_PREFIX, pfilelist->d_name) ||
            startsWith(PRE_RESET_VIDEO_DUMP_PREFIX, pfilelist->d_name)) && 
            endsWith(pfilelist->d_name, VDO_DUMP_FILE_EXT)) {
            return 1;
        }
    }
    else{
        // checking if file has name matching video_dump_*.dat
        if (startsWith(VDO_DUMP_FILE, pfilelist->d_name) && endsWith(pfilelist->d_name, VDO_DUMP_FILE_EXT)) {
            return 1;
        }
    }
    return 0;
}

/*
* getvideolist : scans /var/video folder and retrives available video file name
* filecnt : returns avaible file count
*fileinfo : returns arrray of avaible file names
* return value : 0 on success : errno on failure
*/
int getvideolist(u32 *filecnt,AutoRecordFile_T **fileinfo)
{
    struct dirent **filelist = NULL;
    int cnt = 0,loop=0,num_count =0;
    int retval = 0;
    struct stat fileStat;
    char file[MAX_PATH_NAME_LEN*2] = {0};
    char folder[MAX_FILE_NAME_LEN] = {0};
    AutoRecordFile_T *temprec;
    char *pTimeStamp = NULL;


    if(GetVideoFolder(&folder[0],GET_FOLDER_NAME) !=0)
    {
        TCRIT(" GetVideoFolder failed\n");
        return retval;
    }

    cnt = scandir(&folder[0], &filelist, isDatFile, alphasort);
    if (cnt > 0) {
        if( (temprec = malloc((size_t)(cnt * sizeof(AutoRecordFile_T)))) == NULL )
        {
            TCRIT("Unable to allocate memory for video file list in folder: %s", &folder[0]);
            return errno;
        }

        while (loop < cnt) {
            memset(file,0,sizeof(file));
            retval = snprintf(file, sizeof(file), "%s%s",folder,filelist[loop]->d_name);
            if(retval > MAX_PATH_NAME_LEN*2)
            {
            	 TCRIT(" Buffer overflow\n");
                //free entire array
                free(filelist);
                free(temprec);
                return -1;
            }
            file[sizeof(file)-1] ='\0';
            if (stat(file, &fileStat) == 0) {
                pTimeStamp = ctime(&fileStat.st_mtime);
                if(pTimeStamp == NULL)
                {
                    TCRIT("Error in representing calendar time \n");
                    free(filelist);
                    free(temprec);
                    return -1;
                }
                if(num_count >= cnt)
                {
                    TCRIT("Buffer Overflow \n");
                    free(filelist);
                    free(temprec);
                    return -1;
                }
                memset(&temprec[num_count],0,sizeof(AutoRecordFile_T));
                if( snprintf(temprec[num_count].FileName,sizeof(temprec[num_count].FileName),"%s%s",&folder[sizeof(DEF_VDO_DUMP_PATH)-1],filelist[loop]->d_name) >= (long int)sizeof(temprec[num_count].FileName))
                {
                    TCRIT("Buffer Overflow \n");
                    free(filelist);
                    free(temprec);
                    return -1;
                }
                strncpy(temprec[num_count].TimeStamp,pTimeStamp,sizeof(temprec[num_count].TimeStamp)-1);
                num_count++;
            }
            //free each entry
            free(filelist[loop]);
            loop ++;
        }
        *fileinfo = temprec;
        *filecnt  = num_count;
        //everything went well so reset retval to 0
        retval = 0;
    }
    else
    {
        retval = errno;
        //errno 2 is for no such file or directory so filecount is 0
        if(retval == 2)
            retval =0;
    }

    //free entire array
    free(filelist);
    return retval;
}


/*
* GetVideoFolder : gives video foldername,filename,file full path
* path : returns requested path
*option : 1 - folder name,2- file name ,3-file full path
* return value : 0 on success : -1 on failure
*/
int GetVideoFolder(char *path,int option)
{
	char hostName[MAX_HOSTNAME_LEN] = {0};
	time_t now;
	struct tm *date = NULL;
	struct timeval curtime;
	int ret = -1;

	if(path == NULL)
	{
		return ret;
	}

	if(gethostname(hostName,MAX_HOSTNAME_LEN) != 0)
	{
		TCRIT("Gethostname fialed\n");
		return ret;
	}

	if(option == GET_FOLDER_NAME)
	{
		ret = snprintf(&path[0],MAX_FILE_NAME_LEN , "%s%s/", DEF_VDO_DUMP_PATH,hostName);
	}
	else if(option == GET_FILE_NAME)
	{
		now = time(NULL);
		date=localtime(&now);
		if(date == NULL)
		{
            	    TCRIT("Error in representing calendar time \n");
            	    return -1;
		}
		gettimeofday(&curtime, NULL);

		/*
			video_dump_%y-%m-%d_%h-%m-%s.dat
			video_dump_2014y-07m-24d_17h-41m-43s.dat
		*/
		ret = snprintf(&path[0],MAX_FILE_NAME_LEN,"%s%dy-%02dm-%02dd_%02dh-%02dm-%02d%ss",
							 VDO_DUMP_FILE,date->tm_year + 1900,date->tm_mon+1,date->tm_mday,
							date->tm_hour,date->tm_min,date->tm_sec,VDO_DUMP_FILE_EXT);

	}
	else if(option == GET_FULL_PATH)
	{
	
		now = time(NULL);
		date=localtime(&now);
		if(date == NULL)
		{
	            TCRIT("Error in representing calendar time \n");
	            return -1;
		}
		gettimeofday(&curtime, NULL);

		/*
			video_dump_%y-%m-%d_%h-%m-%s.dat
			video_dump_2014y-07m-24d_17h-41m-43s.dat
		*/
		ret = snprintf(&path[0],MAX_FILE_NAME_LEN,"%s%s/%s%dy-%02dm-%02dd_%02dh-%02dm-%02ds%s",
							 DEF_VDO_DUMP_PATH,hostName,VDO_DUMP_FILE,date->tm_year + 1900,date->tm_mon+1,date->tm_mday,
							date->tm_hour,date->tm_min,date->tm_sec,VDO_DUMP_FILE_EXT);

	}

	if((ret >= MAX_FILE_NAME_LEN ) ||(ret <= 0 ))
	{
		TCRIT("GetVideoFolder : Buffer overflow\n");
		return -1;
	}
	path[MAX_FILE_NAME_LEN - 1] = '\0';

	return 0;
}


unsigned long dirsize = 0;
int FolderSize(const char *fpath, const struct stat *sb, int typeflag) 
{
    if(0){
        fpath=fpath;
        typeflag=typeflag;
    }
    dirsize += sb->st_size;
    return 0;
}

int IsMaxVideoFolderSizeReached()
{
    AutoRecordCfg_T RecordCfg;
    memset(&RecordCfg,0,sizeof(AutoRecordCfg_T));

    if (0 != GetAutoRecordCfg(&RecordCfg))
    {
        TCRIT("GetAutoRecordCfg failed \n");
        return -1;
    }

    if((RecordCfg.remote_path_support != 1) || (RecordCfg.mnt_successful !=0))
    {
        dirsize = 0;
         if (ftw(DEF_VDO_DUMP_PATH, &FolderSize, 1))
         {
            perror("ftw");
            return -1;
        }

         if (dirsize >= RecordCfg.max_size)
        {
            return 1;
        }
    }

    return 0;
}


