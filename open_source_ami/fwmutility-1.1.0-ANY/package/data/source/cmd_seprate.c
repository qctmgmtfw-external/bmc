#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include "main.h"

//#define DEBUG
#define ENABLE_FMH_EXTRACTION

int FWM_Seprate(const FWM *pFWM,char* OutputPath)
{
	MODULE_INFO	*mod;
	FMH     	*pfmh= pFWM->pFileFMH;
	int 		i, size, rtn;
	unsigned char	Name[9+4];
	unsigned char	output_fname[255];
	int fd_rd, fd_wr;
#ifdef ENABLE_FMH_EXTRACTION
	int fd_wrfmh;
	unsigned char	output_fnamefmh[255];
#endif
	char *ptr_rd, *ptr_currentrd;
	
	memset(output_fname, 0, sizeof(output_fname));

	fd_rd = open(pFWM->FileName, O_RDONLY);
	if( fd_rd < 0)
		goto error;

	ptr_rd = mmap(NULL, pFWM->FileSize, PROT_READ, MAP_PRIVATE, fd_rd, 0);
	if( ptr_rd == MAP_FAILED){
		close(fd_rd);
		printf("Error: Unable to map file %s as memory pointer\n", Name);
	}

	for( i = 0; i< pFWM->PartitionCount; i++){
		memset(output_fname, 0, sizeof(output_fname));
#ifdef ENABLE_FMH_EXTRACTION
		memset(output_fnamefmh, 0, sizeof(output_fnamefmh));
                mod = &(pfmh->Module_Info);
		memset(Name, 0, sizeof(Name));
                strncpy((char *)Name,(char *)mod->Module_Name,8);
                Name[8+4]=0;
                strncpy((char*)(Name+ strlen((char*)Name)), ".fmh", 5);

                strncpy((char*)output_fnamefmh, OutputPath, strlen((char*)OutputPath));
                strncpy((char*)(output_fnamefmh+ strlen((char*)output_fnamefmh)), (const char*)Name, strlen((char*)Name));
                fd_wrfmh = open((const char*)output_fnamefmh, O_WRONLY| O_CREAT);
                if(fd_wrfmh < 0){
                        printf("Error: Unable to open the file %s\n", output_fnamefmh);
                        goto error;
                }
#endif
		memset(Name, 0, sizeof(Name));
		mod = &(pfmh->Module_Info);
		strncpy((char *)Name,(char *)mod->Module_Name,8);
		Name[8+4]=0;
		strncpy((char*)(Name+ strlen((char*)Name)), ".bin", 5);
		
		strncpy((char*)output_fname, OutputPath, strlen((char*)OutputPath));
		strncpy((char*)(output_fname+ strlen((char*)output_fname)), (const char*)Name, strlen((char*)Name));
		fd_wr = open((const char*)output_fname, O_WRONLY| O_CREAT);
		if(fd_wr < 0){
			printf("Error: Unable to open the file%s\n", output_fname);
			goto error;
		}

		printf(" Seprating the paratition [%s]...", Name);
#ifdef ENABLE_FMH_EXTRACTION
                size =  pfmh->FMH_AllocatedSize;
		ptr_currentrd = ptr_rd+  pFWM->PartitionLocationFMH[i];
		//printf(" FMH binary size is : 0x%x", size);
		if(strncmp((const char*)Name, "boot", strlen("boot")) == 0){
			rtn = write(fd_wrfmh, ptr_rd, size);
		}else{
	                rtn = write(fd_wrfmh, ptr_currentrd, size);
		}
		if(rtn == size)
			printf(" FMH Done");
		else
			printf(" FMH Failed");
                close(fd_wrfmh);		
#endif
		ptr_currentrd = ptr_rd+ pFWM->PartitionLocation[i];
		size =  mod->Module_Size;
		//printf(" binary size is : 0x%lx", size);
		if(strncmp((const char*)Name, "boot", strlen("boot")) == 0){
			rtn = write(fd_wr, ptr_rd, size);
		}else{
			rtn = write(fd_wr, ptr_currentrd, size);
		}
		if(rtn == size)
			printf(" Done\n");
		else
			printf(" Failed\n");
		close(fd_wr);
#ifdef DEBUG
		printf("\n ===[%d]===\n", i);
		printf("\n pfmh->FMH_AllocatedSize : 0x%08lx\n", pfmh->FMH_AllocatedSize);
		printf(" pFWM->PartitionLocationFMH: 0x%08lx\n", pFWM->PartitionLocationFMH[i]);
		printf(" pFWM->PartitionLocation   : 0x%08lx\n", pFWM->PartitionLocation[i]);
#endif
		pfmh++;
	}
	munmap(ptr_rd, pFWM->FileSize);
	close(fd_rd);
	return 0;
error:
#ifdef ENABLE_FMH_EXTRACTION
	close(fd_wrfmh);
#endif
	munmap(ptr_rd, pFWM->FileSize);
	close(fd_rd);
	return 1;
}
