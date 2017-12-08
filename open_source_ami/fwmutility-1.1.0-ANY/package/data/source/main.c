#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include "main.h"
//#include "buildno.h"

//Function Declaration
int FWM_Generate(const FWM *pFWM,char* OutputPath);
int FWM_Seprate(const FWM *pFWM,char* OutputPath);
int FWM_List(const FWM *pFWM);
int FWM_Replace(const FWM *pFWM, char* NewDataPath, char* OldDataPath, char type);

//Global variables
unsigned long FWM_Command = 0;
FWM FileInfo;

static void ShowHelper(void)
{
	printf("Usage : -i <Input File Name> options\n");
	printf("Options:\n");
	printf(">>Maintenance\n");
	printf("  -l		list all of the FMH partitions\n");
	printf("  -s		separate the partitions as FMH file(s) and binary file(s)\n");
	printf(">>Replacement\n");
	printf("  -r		replace the partition(s) by giving source file\n");
	printf("  -R		replace the partition(s) by giving FMH image\n");
	printf(">>Generation\n");
	printf("  -o <File Name>	generate the new rom image\n");
	printf("\n\nThe default paths for replacement:\n");
	printf("  source file	./replace/src/<partition name>/{FILES}\n");
	printf("  FMH image	./replace/bin/<partition name>.fmh\n");
	printf("The default paths for Generation and Separation:\n");
	printf("  ./output\n");
}
static void ShowHeader(void)
{
	printf("======================================================\n");
	printf("      AMI MegaRAC SP Firmware Maintenance Utility     \n");
	printf("======================================================\n");
	printf("Version 1.0     \n\n");//build no : %s\n\n", buildno);
}
static char ChkIsFileValid(void)
{
	if(FileInfo.FileIsValid == false)
		printf("Please specify the input file name or make sure the file is existing!\n");
	return FileInfo.FileIsValid;
}
static void ChkEnv(void)
{
	char cmd[255];

	sprintf(cmd,"%schkEnv", DEF_PREFIX_SCRIPTS);
	system(cmd);
}
int main(int argc, char **argv)
{
	int fd = 0;
	char *ptr;
	struct stat fstat;
	unsigned long filter;
	int opt, i;
	FMH	*pfmh;
	char	replace_type = 0;
	char 	new_img = 0;
	int 	fwm_rtn;

	ShowHeader();
	ChkEnv();

	//Initialize the data structre
	memset((char*)&FileInfo, 0, sizeof(FileInfo));
	FileInfo.FileIsValid = false;

	while((opt = getopt(argc, argv, "i:o:slRrh")) != -1){
		switch(opt){
			case 'i':
				FWM_Command |= CMD_INPUT;
				printf("==>Input file : %s\n", optarg);
				memcpy(&FileInfo.FileName, optarg, sizeof(FileInfo.FileName));
				//Once FWM get the filename, parse the FMH immediately.
			        FileInfo.pFileFMH = (FMH*)malloc(sizeof(FMH) * DEF_MAX_FMH_SUPPORT);
			        if(FileInfo.pFileFMH == NULL){
			                printf("ERROR: Out of memory\n");
			                goto error;
			        }
			        fd = open(FileInfo.FileName, O_RDONLY);

			        if( stat(FileInfo.FileName, &fstat) != 0){
					printf("ERROR: File not found\n");
					goto error;
				}
				FileInfo.FileSize = fstat.st_size;
				
				//Check the size
				/*
				if(FileInfo.FileSize > DEF_FLASHSIZE_LIMIT){
					printf("ERROR: Invalid firmware size, perhaps you input wrong image file.\n");
					goto error;
				}
				*/
			        ptr = mmap(NULL, FileInfo.FileSize, PROT_READ, MAP_PRIVATE, fd, 0);

			        if( ptr == MAP_FAILED){
			                printf("Error: Unable to map the file %s as memory pointer\n", FileInfo.FileName);
			                goto error;
			        }
			        FileInfo.PartitionCount = ListFMH(ptr, FileInfo.FileSize, FileInfo.pFileFMH, FileInfo.PartitionLocation, FileInfo.PartitionLocationFMH, DEF_MAX_FMH_SUPPORT);
				FileInfo.FileIsValid = true;
				//Calculate the Blocksize by checking conf partition, assume conf is always using JFFS2 format
				FileInfo.BlockSize = 0xFFFFFFFF;
				pfmh = FileInfo.pFileFMH;
				for( i = 0; i< FileInfo.PartitionCount; i++){
					MODULE_INFO	*mod;
					unsigned char	Name[9+4];
					mod = &pfmh->Module_Info;
					memset(Name, 0, sizeof(Name));
					strncpy((char *)Name,(char *)mod->Module_Name,8);
					
					//SPX
					if(strncmp((const char*)Name, "conf", strlen("conf")) == 0){
						FileInfo.BlockSize = FileInfo.PartitionLocation[i] - FileInfo.PartitionLocationFMH[i]; 
						break;
					}
					//SP2
					if(strncmp((const char*)Name, "params", strlen("params")) == 0){
						FileInfo.BlockSize = FileInfo.PartitionLocation[i] - FileInfo.PartitionLocationFMH[i]; 
						break;
					}
					pfmh++;
				}

				//Check the firmware image is valid or not
				if(FileInfo.BlockSize == 0xFFFFFFFF){
					printf("ERROR: Cannot found the proper BlockSize in firmware image, skip\n");
					goto error;
				}else{
					printf(" BlockSize is %lx \n", FileInfo.BlockSize);
				}
				if((FileInfo.FileSize % FileInfo.BlockSize) != 0){
					printf("ERROR: Cannot determine the image type, the image should not be a valid file.\n");
					goto error;
				}
	
				//Find alternate FMH
				pfmh = FileInfo.pFileFMH;
				for( i = 0; i< FileInfo.PartitionCount; i++){
					MODULE_INFO     *mod;
                                        unsigned char   Name[9+4];
                                        mod = &pfmh->Module_Info;
					memset(Name, 0, sizeof(Name));
                                        strncpy((char *)Name,(char *)mod->Module_Name,8);				
					if(strncmp((const char*)Name, "boot", strlen("boot")) == 0){
						printf(" Found the alternate FMH at : %lx\n", FileInfo.BlockSize- sizeof(ALT_FMH));
						memcpy(&FileInfo.AFMH, ptr+ FileInfo.BlockSize- sizeof(ALT_FMH), sizeof(ALT_FMH));
					}
					pfmh++;
				}
				
				//Find Firmware Info section
				pfmh = FileInfo.pFileFMH;
				for( i = 0; i< FileInfo.PartitionCount; i++){
					MODULE_INFO     *mod;
					mod = &pfmh->Module_Info;

					if( mod ->Module_Type == 2){
						printf(" Found the firmware info. location: 0x%lx\n", FileInfo.FileSize- FileInfo.BlockSize+ mod->Module_Location);
						memcpy(&FileInfo.FirmwareInfo, ptr+ FileInfo.FileSize- FileInfo.BlockSize+ mod->Module_Location, sizeof(FileInfo.FirmwareInfo));
					}
					pfmh++;
				}
				munmap(ptr, FileInfo.FileSize);
				close(fd);
				
				//Seprate the partition for FWM
				fwm_rtn = FWM_Seprate(&FileInfo, DEF_PREFIX_PROCESSING);
				if(fwm_rtn != 0){
					printf("ERROR: FWM Internal error\n");
					goto error;
				}
				break;
			case 'o':
				FWM_Command |= CMD_OUTPUT;
				memcpy(&FileInfo.OutputFileName, optarg, sizeof(FileInfo.FileName));	
				if(!ChkIsFileValid()){
					goto error;
				}else{
				}
				break;
			case 's':
				FWM_Command |= CMD_SEPRATE;
				if(!ChkIsFileValid()){
					goto error;
				}else{
				}	
				break;
			case 'l':
				FWM_Command |= CMD_LIST;
				printf("==>List all of partitions information\n");
                                if(!ChkIsFileValid()){
                                        goto error;
                                }else{
					FWM_List(&FileInfo);
                                }		
				break;
			case 'R':
				FWM_Command |= CMD_REPLACE;
				replace_type = 1;
				break;
			case 'r':
				FWM_Command |= CMD_REPLACE;
				replace_type = 2;
				break;
			case 'h':
				ShowHelper();
				break;
			default:
				ShowHelper();
				goto error;				
		}
	}
	if(FWM_Command == 0){
		ShowHelper();
		goto error;
	}
//Frontend processing
	filter = CMD_REPLACE;
	if((FWM_Command & filter) == filter){
		printf("==>Replace the current image\n");
		fwm_rtn = FWM_Replace(&FileInfo, DEF_PREFIX_PROCESSING, DEF_PREFIX_REPLACING, replace_type);
		if(fwm_rtn !=0 ){
			printf("INFO: FWM cannot perform replacement function.\n");
			goto error;
		}
	}
//Frontend processing


//Backend processing
	filter = CMD_OUTPUT;
	if((FWM_Command & filter) == filter){
		printf("==>Output file : %s\n", FileInfo.OutputFileName);
		fwm_rtn = FWM_Generate(&FileInfo, DEF_PREFIX_OUTPUT);
		if(fwm_rtn !=0){
			printf("INFO: FWM cannot perform generation function.\n");
			goto error;
		}
		new_img = 1;
	}
	filter = CMD_SEPRATE;
	if((FWM_Command & filter) == filter){
		printf("==>Seprate the partitions as binary image\n");
		if(new_img == 1){
			printf("INFO: Sepration function can not work with generation fuction together, skip sepration.\n");
			goto error;
		}else{
			fwm_rtn = FWM_Seprate(&FileInfo, DEF_PREFIX_OUTPUT);
			if(fwm_rtn !=0){
				printf("INFO: FWM cannot perform sepration function\n");
			}
		}
	}
//Backend processing --end		
	printf("INFO: FWM done.\n");
	return 0;
error:
	printf("INFO: FWM abnormal end.\n");
	return -1;
}

