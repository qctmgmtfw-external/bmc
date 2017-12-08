#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include "main.h"

unsigned char CalculateModule100(unsigned char *Buffer, unsigned long Size);

REPLACE_FOLDER_RULES g_ReplaceRules[]={
	{"www"		, DEF_IMG_TYPE_CRAMFS},
	{"3wabt"	, DEF_IMG_TYPE_CRAMFS},
	{"3wlogo"	, DEF_IMG_TYPE_CRAMFS},
	{"jverabt"	, DEF_IMG_TYPE_CRAMFS},
	{"conf"		, DEF_IMG_TYPE_JFFS2},//SPX
	{"bkupconf"	, DEF_IMG_TYPE_JFFS2},//SPX
	{"params"	, DEF_IMG_TYPE_JFFS2},//SP2
	{"paramsbk"	, DEF_IMG_TYPE_JFFS2},//SP2
};

static int create_bin_img(char *filename, char *name, unsigned long type, char* OutputPath, unsigned long BlkSize)
{
	char cmd[255];
	int rtn;

	memset(cmd, 0 , sizeof(cmd));

	//first, create the binary file
	switch(type){
		case DEF_IMG_TYPE_CRAMFS:
			sprintf(cmd, "%smkfs-cramfs %s %s%s.bin",DEF_PREFIX_SCRIPTS, filename, OutputPath, name);
			rtn = system(cmd);
			if( rtn != 0){
				printf("ERROR: Unable to create cramfs file CMD:%s\n", cmd);
				return 1;
			} 
			break;
		case DEF_IMG_TYPE_JFFS2:
			sprintf(cmd, "%smkfs-jffs2 %s %s%s.bin 0x%x",DEF_PREFIX_SCRIPTS, filename, OutputPath, name, (unsigned int)BlkSize);
			rtn = system(cmd);
			if( rtn != 0){
				printf("ERROR: Unable to create cramfs file CMD:%s\n", cmd);
				return 1;
			}
			break;
		default:
			return 1;
	}

	return 0;
}
static int create_fmh_img(const FWM *pFWM, unsigned long idx, char *InputPath, char* name, char* OutputPath)
{
	FILE *Outfd;
	int Infd, fd;
	MODULE_INFO *mod;
	FMH *pfmh = pFWM->pFileFMH;
	FMH newfmh;
	char output_fname[255];
	char stat_fname[255];
	unsigned long   pattern_erase = 0xFFFFFFFF;
	int i, size, rtn;
	struct stat fstat;
	unsigned long crc32;
	char data;

	pfmh+=idx; //Point to the current FMH
	memcpy(&newfmh, pfmh, sizeof(FMH));
	mod = &(newfmh.Module_Info);

	//Update the Module_Size
	sprintf(stat_fname, "%s%s.bin", InputPath, name);
	rtn = stat(stat_fname, &fstat);
	if( rtn != 0){
		printf("ERROR: Unable to query file %s\n", stat_fname);
		return 1;
	}
	mod->Module_Size = fstat.st_size;

       /* Update crc32 */
	fd = open(stat_fname, O_RDONLY);
	if( fd < 0)
		return 1;
        BeginCRC32(&crc32);
        for(i = 0; i < mod->Module_Size; i++)
        {
                if (read(fd,&data,1) != 1)
                {
                        close(fd);
                        return 0;
                }
                DoCRC32(&crc32,data);
        }
        EndCRC32(&crc32);
	close(fd);
	mod->Module_Checksum = crc32;
	
	//Check the size
	if(mod->Module_Size > (pfmh->FMH_AllocatedSize-(pFWM->PartitionLocation[idx]-pFWM->PartitionLocationFMH[idx]))){
		printf("ERROR: The new firmware is larger than the free space of partition in firmware image, new size: 0x%x old size: 0x%x\n", (unsigned int)mod->Module_Size, (unsigned int)(pfmh->FMH_AllocatedSize-(pFWM->PartitionLocation[idx]-pFWM->PartitionLocationFMH[idx])));
		return 1;
	}

	/*Calculate Header Checksum*/
	newfmh.FMH_Header_Checksum = 0;
	newfmh.FMH_Header_Checksum = CalculateModule100((unsigned char *)&newfmh,sizeof(FMH));

	//Create FMH image
	sprintf(output_fname, "%s%s.fmh", OutputPath, name);
	Outfd = fopen((char*)output_fname,"w+b");
	if( Outfd == NULL){
		printf("Error: Unable to open the file %s\n", output_fname);
		return 1;
	}
	for(i = 0; i<newfmh.FMH_AllocatedSize/4; i++){
		size = fwrite(&pattern_erase, 1, 4, Outfd);
		if(size != 4){
			printf("ERROR: (DUMMY_PATTERN)Unable to write the file %s\n%d", output_fname, size);
			return 1;
		}
	}
	
	rtn = fseek(Outfd, 0, SEEK_SET);
        if(rtn !=0){
                printf("ERROR: Unable to seek the file\n");
                return 1;
        }

	//Write FMH
	size = fwrite(&newfmh, 1, sizeof(newfmh), Outfd);
	if(size != sizeof(newfmh)){
		printf("ERROR: (FMH)Unable to write the file %s\n", output_fname);
		return 1;
	}

	//Write binary image
	rtn = fseek(Outfd, pFWM->PartitionLocation[idx] - pFWM->PartitionLocationFMH[idx] , SEEK_SET);
	if(rtn !=0){
		printf("ERROR: Unable to seek the file\n");
		return 1;
	}

        /* Open Module File */
        Infd = open(stat_fname,O_RDONLY);
        if (Infd < 0){
		printf("ERROR: Unable to open the file %s\n", stat_fname);
                return 1;
	}

	while (1){
		if (read(Infd,&data,1) != 1)
			break;
		if (fwrite((void *)&data,1,1,Outfd) != 1){
			printf("ERROR: Unable to write Module.\n");
			return 1;
		}else{
			size++;
		}
	}
	close(Infd);
	fclose(Outfd);

	//Everything is fine, so update the information to current environment structure
	//If user set -o option to generate new image, the new image will use current environment structure, not retrive from the FMH file.
	memcpy(pfmh, &newfmh, sizeof(FMH));	

	return 0;
}

//For whole FHM partition type = 1 , folder type = 0
int FWM_Replace(const FWM *pFWM, char* NewDataPath, char* OldDataPath, char type)
{
	int i, j;
	MODULE_INFO	*mod;
	char		Name[9+4];
	struct stat	fstat;
	int		rtn;
	char		input_fname[100];
	FMH		*pfmh = pFWM->pFileFMH;
	char		cmd[100];
	unsigned char	chk = 0;
	unsigned char	valid;

	switch(type){
		case 1:
			for(i = 0; i< pFWM->PartitionCount; i++){
				memset(Name, 0, sizeof(Name));
				memset(input_fname, 0, sizeof(input_fname));

				mod = &(pfmh->Module_Info);
				strncpy((char *)Name,(char *)mod->Module_Name,8);
				Name[8] = 0;
				sprintf(input_fname, "%s%s%s.fmh", OldDataPath, DEF_PREFIX_REPLACING_FMH, Name);	
				rtn = stat(input_fname, &fstat);
				if( rtn != 0){
					pfmh++;
					continue;
				}
				if( fstat.st_size == pfmh->FMH_AllocatedSize){
					chk++;
					printf(" [%s] will be overrived by %s\n", Name, input_fname); 
				}else{
					printf(" [%s] cannot be overrided by%s , the size is invalid\n", Name, input_fname);
					pfmh++;
					continue;
				}
				
				memset(cmd, 0, sizeof(cmd));
				sprintf(cmd, "%scpFile %s %s -f", DEF_PREFIX_SCRIPTS, input_fname, NewDataPath);
				rtn = system(cmd);
				if( rtn ){
					printf("ERROR: Unable to copy the file %s to %s", input_fname, NewDataPath);
				}
				pfmh++;
			}	
			if(chk == 0){
				printf(" Nothing to do. Skip\n");
			}else{
				printf(" Totally replace %d partition(s)\n", chk);
			}
			break;
		case 2:
			for( i = 0; i< pFWM->PartitionCount; i++){
				memset(Name, 0, sizeof(Name));
				memset(input_fname, 0, sizeof(input_fname));

				mod = &(pfmh->Module_Info);
				strncpy((char *)Name,(char *)mod->Module_Name,8);
				Name[8] = 0;
				sprintf(input_fname, "%s%s%s", OldDataPath, DEF_PREFIX_REPLACING_SRC, Name);
				rtn = stat(input_fname, &fstat);
				if( rtn != 0){
					pfmh++;
					continue;
				}
				if( S_ISDIR(fstat.st_mode)){
					valid = 0;
					for( j= 0 ; j< sizeof(g_ReplaceRules)/sizeof(REPLACE_FOLDER_RULES); j++){
						if(strncmp(Name, g_ReplaceRules[j].ModuleName, strlen(Name)) != 0){
							continue;
						}else{
							valid = 1;
							break;
						}
					}
					if(valid){
						printf(" [%s] will be overrived by %s\n", Name, input_fname);
						rtn = create_bin_img(input_fname, Name, g_ReplaceRules[j].ImgType, NewDataPath, pFWM->BlockSize);
						if(rtn != 0){
							printf(" [%s] cannot be replace by %s\n", Name, input_fname);
							pfmh++;
							continue;
						}
						rtn = create_fmh_img(pFWM, i, NewDataPath, Name, NewDataPath);
                                                if(rtn != 0){
                                                        printf(" [%s] FMH cannot be replace by %s\n", Name, input_fname);
                                                        pfmh++;
							return 1;
                                                }
						chk++;
					}else{
						printf(" The [%s] is not supporting by this application\n", Name);
					}
				}
				pfmh++;
			}
			if(chk == 0){
				printf(" Nothing to do. Skip\n");
			}else{
				printf(" Totally replace %d folder(s)\n", chk);
			}
			break;
		default:
			printf("ERROR: Unknow type for replacing function.\n");
			return -1;
	}
	return 0;
}
