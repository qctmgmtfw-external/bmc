#include "fmhcore/fmh.h"

#define DEF_MAX_FMH_SUPPORT	100
#define DEF_PREFIX_OUTPUT	"./output/"
#define DEF_PREFIX_INPUT	"./input/"
#define DEF_PREFIX_PROCESSING	"./processing/"
#define DEF_PREFIX_REPLACING	"./replace/"
#define DEF_PREFIX_REPLACING_FMH	"bin/"
#define DEF_PREFIX_REPLACING_SRC	"src/"
#define DEF_PREFIX_SCRIPTS	"./scripts-internal/"
#define false			0
#define true			1

#define DEF_FLASHSIZE_LIMIT	32*1024*1024

#define CMD_INPUT	1<<31
#define CMD_OUTPUT	1<<30
#define CMD_SEPRATE	1<<29
#define CMD_LIST	1<<28
#define CMD_REPLACE	1<<27


enum IMAGE_TYPE{
	DEF_IMG_TYPE_JFFS2	=0,
	DEF_IMG_TYPE_CRAMFS	=1,
};

typedef struct{
	char FileIsValid;
	char FileName[256];
	char OutputFileName[256];
	long FileSize;
	long PartitionCount;
	unsigned long BlockSize;	
	FMH *pFileFMH;
	ALT_FMH AFMH;
	unsigned long PartitionLocationFMH[DEF_MAX_FMH_SUPPORT];
	unsigned long PartitionLocation[DEF_MAX_FMH_SUPPORT];
	char FirmwareInfo[64*1024];
}FWM;

typedef struct{
	char		ModuleName[8];
	unsigned long	ImgType;
}REPLACE_FOLDER_RULES;
