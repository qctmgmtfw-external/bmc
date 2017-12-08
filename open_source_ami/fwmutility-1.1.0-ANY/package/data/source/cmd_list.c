#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include "main.h"

int FWM_List(const FWM *pFWM)
{
	MODULE_INFO	*mod;
	FMH     	*pfmh= pFWM->pFileFMH;
	int 		i;
	unsigned char	Name[9];
	
	for( i = 0; i< pFWM->PartitionCount; i++){
		mod = &(pfmh->Module_Info);
		strncpy((char *)Name,(char *)mod->Module_Name,8);
		Name[8]=0;
		printf(" ==Partition %d ==\n", i);
		printf(" Name    : %s\n",Name);
		printf(" Ver     : %d.%d\n",mod->Module_Ver_Major,mod->Module_Ver_Minor);
		printf(" Type   : 0x%04x\n",le16_to_host(mod->Module_Type));
		printf(" Flags  : 0x%04x\n",le16_to_host(mod->Module_Flags));
		//printf("Size   : 0x%08x\n",le32_to_host(mod->Module_Size));
		printf(" Size   : 0x%08lx\n",pfmh->FMH_AllocatedSize);
		printf(" Location: 0x%08lx\n",pFWM->PartitionLocation[i]);
		printf(" FMHlocation: 0x%08lx\n",pFWM->PartitionLocationFMH[i]);
		printf(" LoadAddr: 0x%08x\n",le32_to_host(mod->Module_Load_Address));
		pfmh++;
	}
	return 0;
}
