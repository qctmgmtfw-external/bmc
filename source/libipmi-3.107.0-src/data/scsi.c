/*****************************************************************
******************************************************************
***                                                            ***
***        (C)Copyright 2008, American Megatrends Inc.         ***
***                                                            ***
***                    All Rights Reserved                     ***
***                                                            ***
***       5555 Oakbrook Parkway, Norcross, GA 30093, USA       ***
***                                                            ***
***                     Phone 770.246.8600                     ***
***                                                            ***
******************************************************************
******************************************************************
******************************************************************
*
* Filename: scsi.c
*
******************************************************************/

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <scsi/scsi_ioctl.h>
#include <scsi/scsi.h>
int 	CheckGenericDevice();
//int scsiProbe(int probeFlags, char *dev) 
//uses a new method for probing for scsi devices
//it takes an OUT argument to an array of ints - it fills this with
//possilbe candidates for AMI device that is used for communication
//this is because with the composite device in RHEL3 of G4 sometimes
//host gets confused..so eventhough as per its order CDROM should be 2 
//it will be sg3 which will actually list as floppy in /prc/scsi/sg
//guess usb composite layer confuses it
//therefore we will find all AMI devices and then return possible
//numbers in this array which the library can then try identify command with

//it also takes an IN/OUT numdev hich indictaes the length of the array
//input is length caller allocated, output is length filled
int scsiProbeNew(int* num_ami_devices,int* sg_nos)
{
	int numdevfound = 0;
	char linebuf[81] = "";
	char vendor[81] = "";
    char sg_file[81] = "";
	int lineno = 0;
    int i = 0;
	
	//we will actuallyuse proc/scsi/sg/device_strs to find us

	FILE* fp = NULL;

    int sg_count = 0;
    DIR* dirp = NULL;
    struct dirent * entry = NULL;

#if 0
	if(CheckGenericDevice()==-1)
	{
		*num_ami_devices = 0;
		return 1;
	}
#endif
	fp = fopen("/proc/scsi/sg/device_strs","r");
	if(fp == NULL)
	{
        dirp = opendir("/sys/class/scsi_generic");
        if (dirp != NULL)
        {
            while((entry = readdir(dirp)) != NULL)
            {
                if (strncmp(entry->d_name,"sg",2) == 0)
                {
                    sg_count++;
                }
            }
            closedir(dirp);

            for(i = 0; i < sg_count; i++)
            {
                sprintf(sg_file,"/sys/class/scsi_generic/sg%d/device/vendor",i);
                fp = fopen(sg_file,"r");
                if (fp != NULL)
                {
                    if(fgets(linebuf,80,fp) != NULL)
                    {
	    	            sscanf(linebuf,"%s",vendor);
		                if(strcasecmp(vendor,"AMI") == 0)
                        {
                            numdevfound++;
                            sg_nos[numdevfound - 1] = i;
                        }
                    }
                    fclose(fp);
                }
            }
        }
	}
    else
    {   
        while(1)
    	{   
	    	//ok now read line by line but we need to read only 3 	characters for
		    //finding AMI..
    		if(fgets(linebuf,80,fp) != NULL)
            {
	    	    sscanf(linebuf,"%s",vendor);
    		    if(strcasecmp(vendor,"AMI") == 0)
	    	    {
		    	    numdevfound++;
			        sg_nos[numdevfound -1] = lineno;
		        }
		        lineno++;
            }
            else
            {
                fclose(fp);
                break;
            }
	    }
    }

    *num_ami_devices = numdevfound;
    if(numdevfound == 0)
    {
        return 1;
    }
    else 
    {
        return 0;
    }
}


int CheckGenericDevice()
{
	
	FILE* fp;
	char linebuf[81];
	char module[81];
	char Status[81];
	fp = fopen("/proc/modules","r");
	if(fp == NULL)
	{
		return -1;
	}
	else
	{
		while(fgets(linebuf,80,fp) != NULL)
		{
			sscanf(linebuf,"%s %*s %*s - %s",module,Status);
			if((strcasecmp(module,"sg")==0)&&(strcasecmp(Status,"LIVE")==0))
			{
				fclose(fp);
				return 0;
			}
		}
		
	}
	system("modprobe sg");
	fclose(fp);
	fp = fopen("/proc/modules","r");
        if(fp == NULL)
        {
                return -1;
        }
        else
        {
                while(fgets(linebuf,80,fp) != NULL)
                {
                        sscanf(linebuf,"%s %*s %*s - %s",module,Status);
                        if((strcasecmp(module,"sg")==0)&&(strcasecmp(Status,"LIVE")==0))
                        {
                                fclose(fp);
                                return 0;
                        }
                }
        }
	fclose(fp);
	return -1;
}

