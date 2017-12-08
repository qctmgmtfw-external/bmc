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

static unsigned long gBlkSize;

#if 0
static unsigned int FillModuleInfo(MODULE_INFO *mod,char *InFile)
{
	struct stat InStat;
	unsigned long FileSize;
	unsigned long i,crc32;
	int fd;
	unsigned char data;

        /* Get the Module File Size */
        if (stat(InFile,&InStat) != 0)
		return 0;       /* Error in stat. Possibly a bad file */
        FileSize = InStat.st_size;

        /* Open the File for crc32 calculation */
        fd = open(InFile,O_RDONLY);
        if (fd < 0)
                return 0;

        /* Read the data and calculate crc32 */
        BeginCRC32(&crc32);
	for(i = 0; i < FileSize; i++)
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

        /* Fill Module Checksum */
        mod->Module_Checksum = crc32;

        /* Return Module Size */
        return FileSize;
}
#endif

static int WriteFMHtoFile(FILE *fd,FMH *fmh,ALT_FMH *altfmh,unsigned long Start, unsigned long BlockSize)
{
        unsigned long offset = 0;

        if (altfmh != NULL){
                offset = altfmh->FMH_Link_Address;
		printf(" Alternate partition that store the Link_Address is : %lx\n", offset);
	}

        /* Write FMH Header */
        if (fseek(fd,Start+offset,SEEK_SET) != 0){
                printf("ERROR: Unable to seek %ld for FMH Header\n",Start+offset);
                return 1;
        }
	//printf(" Write FMH Header at offset : 0x%lx\n", Start+ offset);
        if (fwrite((void *)fmh,sizeof(FMH),1,fd) != 1){
                printf("ERROR: Unable to write FMH Header\n");
                return 1;
        }

        /* Write Alternate FMH Header */
        if (altfmh != NULL){
                offset = BlockSize - sizeof(ALT_FMH);
		//printf(" Write alternate FMH Header at location : 0x%lx\n", Start+ offset);
                if (fseek(fd,Start+offset,SEEK_SET) != 0){
                        printf("ERROR: Unable to seek %ld for ALTFMH Header\n",Start+offset);
                        return 1;
                }
                if (fwrite((void *)altfmh,sizeof(ALT_FMH),1,fd) != 1){
                        printf("ERROR: Unable to write ALTFMH Header\n");
                        return 1;
                }
        }

        return 0;
}

static int WriteModuletoFile(FILE *Outfd, char *InFile, unsigned long Location)
{
        int Infd;
        char data;

        /* Open Module File */
        Infd = open(InFile,O_RDONLY);
        if (Infd < 0)
                return 1;

        /* Seek Output File */
        if (fseek(Outfd,Location,SEEK_SET) != 0)
        {
                printf("ERROR: Unable to seek %ld for Module\n",Location);
                close(Infd);
                return 1;
        }

        while (1)
        {
                if (read(Infd,&data,1) != 1)
                                break;
                if (fwrite((void *)&data,1,1,Outfd) != 1)
                {
                      printf("ERROR: Unable to write Module \n");
                        return 1;
                }
        }
        return 0;
}

static int WriteFirmwareInfo(FILE *Outfd,char *Data, unsigned long Size, unsigned long Location)
{
        /* Seek Output File */
	printf(" WriteFirmwareInfo at location 0x%x\n", (unsigned int)Location);
        if (fseek(Outfd,Location,SEEK_SET) != 0)
        {
                printf("ERROR: Unable to seek %x for Module\n",(unsigned int)Location);
                return 1;
        }

        /* Write Data */
        if (fwrite((void *)Data,Size,1,Outfd) != 1)
        {
		printf("ERROR: Unable to write Module\n");
		return 1;
        }
        return 0;
}

static int CalculateImageChecksum(FILE* fd,unsigned long ImageHeaderStart)
{
        unsigned long FileSize;
        unsigned long i,crc32;
        unsigned char data;
        unsigned char Buffer[128];
        unsigned char Mod100Checksum = 0;
	int size;


        /* We want to calculate the checksum until the end of FIRMWARE MODULE section. */
        FileSize = ImageHeaderStart+gBlkSize;
        printf(" FileSize = 0x%lX\n",FileSize);

        /* Rewind file to make sure that it's at the begining */
        rewind(fd);
        /* Read the data and calculate crc32 */
        BeginCRC32(&crc32);
        for(i = 0; i < FileSize; i++)
        {
                if (fread(&data,sizeof(unsigned char),1,fd) != 1)
                {
                        printf("ERROR: Unabled to read the data for calculating checksum\n");
                        return 0;
                }
                if((i >= (ImageHeaderStart+FMH_MODULE_CHECKSUM_START_OFFSET) && i <= (ImageHeaderStart+FMH_MODULE_CHCKSUM_END_OFFSET)) || (i == (ImageHeaderStart+FMH_FMH_HEADER_CHECKSUM_OFFSET)))
                        continue;
                DoCRC32(&crc32,data);
        }
        EndCRC32(&crc32);
        crc32 = host_to_le32(crc32);
        printf(" Image checksum is 0x%lX\n",crc32);
        /* Fill this image checksum in Module Checksum field of MODULE FIRMWARE */
        fseek(fd,ImageHeaderStart+FMH_MODULE_CHECKSUM_START_OFFSET,SEEK_SET);
        if(fwrite(&crc32,sizeof(unsigned long),1,fd) == 0)
        {
                printf("ERROR: Unable to write the new checksum\n");
                return 0;
        }
        /* Update FMH Modulo100 checksum now */
        /* Again, the assumption here is that the firmware module is at 
        the begining (START) of the image. And also, that there's no alternate
        FMH header for firmware module */
	//printf("ImageHeaderStart 0x%x\n", (unsigned int)ImageHeaderStart);
        fseek(fd,ImageHeaderStart,SEEK_SET);
        size = fread(Buffer,sizeof(unsigned char),sizeof(FMH),fd);
	if(size != sizeof(FMH)){
		printf("ERROR: Unable to update DMH Modulo100 checksum\n");
		return 0;
	}
	Buffer[23] = 0x00; //The Module100 must be 0x00
        Mod100Checksum = CalculateModule100(Buffer,sizeof(FMH));
        printf(" Mod100 Checksum is 0x%X\n",Mod100Checksum);
        fseek(fd,ImageHeaderStart+FMH_FMH_HEADER_CHECKSUM_OFFSET,SEEK_SET);
        if(fwrite(&Mod100Checksum,sizeof(unsigned char),1,fd) == 0)
        {
                printf("ERROR: Unable to write the FMH mod 100 checksum\n");
                return 0;
        }
        return 1;

}
int FWM_Generate(const FWM *pFWM, char* OutputPath)
{
	MODULE_INFO	*mod;
	FMH     	*pfmh= pFWM->pFileFMH;
	int 		i;
	unsigned char	Name[9+4];
	unsigned char   output_fname[255];
	unsigned char	InFile[255];
	unsigned long	pattern_erase = 0xFFFFFFFF;
	FILE *Outfd;
	unsigned long ImageHeaderStart = 0xFFFFFFFF;
	unsigned char ModuleFormat;
	ALT_FMH *paltfmh;	/* Alternate Flash Module Header */
	unsigned long Location;

    int ii;
    char TempName[9];

	memset(output_fname, 0, sizeof(output_fname));
	strncpy((char*)output_fname, OutputPath, strlen((char*)OutputPath));
	strncpy((char*)(output_fname+strlen((char*)output_fname)), pFWM->OutputFileName, strlen(pFWM->OutputFileName));		
	printf(" Creating \"%s\" ...\n",output_fname);
        printf(" FlashSize = 0x%lx BlockSize = 0x%lx\n", pFWM->FileSize, pFWM->BlockSize);

	gBlkSize = pFWM->BlockSize;

	Outfd = fopen((char*)output_fname,"w+b");
	if(Outfd == NULL){
		printf("Error: Unable to open the file %s\n", pFWM->OutputFileName);
		goto error;
	}

	//Fill the pattern to the output file
	for(i = 0; i<pFWM->FileSize/4; i++)
		fwrite(&pattern_erase, 4, 1, Outfd);
	
	for( i = 0; i< pFWM->PartitionCount; i++){
		memset(InFile, 0, sizeof(InFile));
		memset(Name, 0, sizeof(Name));

		mod = &(pfmh->Module_Info);
		strncpy((char *)Name,(char *)mod->Module_Name,8);


		//Module type
		ModuleFormat = (mod->Module_Type >> 8);

		/* Module Firmware is a dummy section. It does not have any data */
                if ((mod->Module_Type != MODULE_FMH_FIRMWARE) &&
                    (mod->Module_Type != MODULE_FIRMWARE_1_4))
                {
		
                     ii=0;
                     while( isascii(mod->Module_Name[ii]) && ii<8 )
                     {
                        ii++;                       
                     }

                     strncpy(TempName, mod->Module_Name, ii);
                     TempName[ii] = '\0';


                    sprintf((char*)InFile, "%s%s.bin", DEF_PREFIX_PROCESSING, TempName);          


		}else{
			printf(" Now FWM is processing Module Firmware section\n");
		}

		//We are not supporting the START and END because firmware image doesn't include those information
		if(strncmp((const char*)Name, "boot", strlen("boot")) == 0){
			//Bootloader must start from 0
			Location = 0;
		}else{
			Location = pFWM->PartitionLocationFMH[i];
		}
		//debug
		//printf(" Location : %lx\n", Location);

		if ((mod->Module_Type == MODULE_FMH_FIRMWARE) ||
                    (mod->Module_Type == MODULE_FIRMWARE_1_4))
                {
                        ImageHeaderStart = Location;
			printf(" ImageHeaderStart Location : %lx\n", ImageHeaderStart);
                }

		if ((mod->Module_Type != MODULE_FMH_FIRMWARE) && (mod->Module_Type != MODULE_FIRMWARE_1_4)){
			int rtn;
			if(strncmp((const char*)Name, "boot", strlen("boot")) == 0){
				printf(" Writing [boot] at location: 0x%x....", 0);
				rtn = WriteModuletoFile(Outfd, (char*)InFile, 0);
			}else{
				printf(" Writing [%s] at location : 0x%x....", Name, (unsigned int)pFWM->PartitionLocation[i]);
	                        rtn = WriteModuletoFile(Outfd, (char*)InFile, pFWM->PartitionLocation[i]);
			}
			if(rtn !=0){
				printf("ERROR:  Unable to write Module of Section %s\n", mod->Module_Name);
				break;
			}
			printf(" Done\n");
                }else{
                        if (mod->Module_Size > 0){
                                if (WriteFirmwareInfo(Outfd, (char*)&pFWM->FirmwareInfo, mod->Module_Size, pFWM->FileSize- pFWM->BlockSize+ mod->Module_Location)!= 0){
                                        printf("ERROR: Unable to Write Firmware Info in Section %s\n", mod->Module_Name);
					break;
                                }
                        }
                }
		if (1){
			if(strncmp((const char*)Name, "boot", strlen("boot")) == 0){
				paltfmh = (ALT_FMH*)&pFWM->AFMH;
				if (WriteFMHtoFile(Outfd, pfmh, paltfmh, 0, pFWM->BlockSize) != 0){
					printf("ERROR: Unable to Write FMH of Section %s\n", mod->Module_Name);
					break;
				}
			}
			else{
				paltfmh = NULL;
	                        if (WriteFMHtoFile(Outfd, pfmh, paltfmh, pFWM->PartitionLocationFMH[i], pFWM->BlockSize) != 0){
        	                        printf("ERROR: Unable to Write FMH of Section %s\n", mod->Module_Name);
                	                break;
                        	}
			}
                }
		pfmh++;
	}

        /* Calculate complete image checksum now and fill in the MODULE INFO checksum field */
        if (ImageHeaderStart != 0xFFFFFFFF)
        {
                if(CalculateImageChecksum(Outfd, ImageHeaderStart) == 0)
                        printf("ERROR: Image Checksum calculation failed\n");
        }

	fclose(Outfd);
	return 0;
error:
	return 1;
}
