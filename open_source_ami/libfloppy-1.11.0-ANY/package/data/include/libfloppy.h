/*****************************************************************
*	File		:	floppy.h									 *
*	Author		:	Rajasekhar J(rajasekharj@amiindia.co.in)	 *
*	Created		:	18-July-2003								 *
*	Description	:	Defines the data structures used 			 *
*					for floppy redirection						 *
******************************************************************/

#ifndef __FLOPPY_9X_H__
#define __FLOPPY_9X_H__

#define FLOPPY_VERSION	"1.9"

#define NUM_OF_TRACKS	(0x3f)
#define	NUM_OF_HEADS	(0xff)

#ifdef __GNUC__
#define PACKED __attribute__ ((packed))
#else
#define PACKED
#pragma pack( 1 )
#endif


typedef struct tagCONTROLBLOCK
{
  DWORD StartingSector ;
  WORD NumberOfSectors ;
  DWORD pBuffer;
} PACKED CONTROLBLOCK;

typedef struct _DIOC_REGISTERS
{
    DWORD reg_EBX;
    DWORD reg_EDX;
    DWORD reg_ECX;
    DWORD reg_EAX;
    DWORD reg_EDI;
    DWORD reg_ESI;
    DWORD reg_Flags;
} DIOC_REGISTERS ;

//For More information please read LIBFLOPPY/docs
typedef struct _BPB {
	BYTE    bsJump[3];      // jmp instruction
	char    bsOemName[8];   // OEM name and version

	// This portion is the BPB (BIOS Parameter Block)
	WORD    bsBytesPerSec;          // bytes per sector
	BYTE    bsSecPerClust;          // sectors per cluster
	WORD    bsResSectors;           // number of reserved sectors
	BYTE    bsFATs;                 // number of file allocation tables
	WORD    bsRootDirEnts;          // number of root-directory entries
	WORD    bsSectors;              // total number of sectors
	BYTE    bsMedia;                // media descriptor
	WORD    bsFATsecs;              // number of sectors per FAT
	WORD    bsSecPerTrack;          // number of sectors per track
	WORD    bsHeads;                // number of read/write heads
#if defined (__x86_64__) || defined (WIN64)
	unsigned int   bsHiddenSectors;		// number of hidden sectors
	unsigned int   bsHugeSectors;          // number of sectors if bsSectors==0
#else
	DWORD   bsHiddenSectors;		// number of hidden sectors
	DWORD   bsHugeSectors;          // number of sectors if bsSectors==0
#endif	
} PACKED BPB; //For More information please read LIBFLOPPY/docs

typedef struct _BOOTSECT_12_16 {
	BPB		bsParamBlock;			//Bios Parameter Block
	BYTE    bsDriveNumber;          // 80h if first hard drive
	BYTE    bsReserved;
	BYTE    bsBootSignature;		// 29h if extended boot-signature record
	DWORD   bsVolumeID;             // volume ID number
	char    bsVolumeLabel[11];		// volume label
	char    bsFileSysType[8];		// file-system type (FAT12 or FAT16)
} PACKED BOOTSECTOR_12_16; //For More information please read LIBFLOPPY/docs

typedef struct _BOOTSECT_32 {
	BPB	bsParamBlock;			//Bios Parameter Block
	DWORD	bsSecPerFat;			//sectors per FAT
	WORD	bsFATflags;				// FAT flags
									// 15-8	Reserved for future use. Always initialize to 0, preserve current value thereafter.
									// 7-4	Active FAT number (if mirroring is disabled)
									// 3-1	Reserved for future use. Always initialize to 0, preserve current value thereafter.
									// 0	FAT mirroring: 0 = enabled 1 = disabled
	
	WORD	bsFSVer;				// File system version number
	DWORD	bsRootDirStartClust;	//root directory start cluster
	WORD	bsSuperBlockNum;		// superblock ("filesystem info sector") number
	WORD	bsBackupBlockNum;		// block containing the backup of the boot block
	BYTE	bsReserved1[12];		//Reserved
	BYTE    bsDriveNumber;          // 80h if first hard drive
	BYTE    bsReserved;
	BYTE    bsBootSignature;		// 29h if extended boot-signature record
	DWORD   bsVolumeID;             // volume ID number
	char    bsVolumeLabel[11];		// volume label
	char    bsFileSysType[8];		// file-system type (FAT12 or FAT16)
} PACKED BOOTSECTOR_32; //For More information please read LIBFLOPPY/docs

typedef struct _BOOTSECT_WINNT {
	BPB	bsParamBlock;			//Bios Parameter Block

	BYTE    bsDriveNumber;          // 80h if first hard drive
	BYTE	bsFlags;
	BYTE    bsBootSignature;		// 29h if extended boot-signature record
	BYTE    bsReserved;
	DWORD	bsHugeSectorsHigh;		// High DWORD of HugeSectors
	DWORD	bsHugeSecorsLow;		// Low DWORD of HugeSectors
	DWORD	bsFirstClustHigh;		// High DWORD of First Cluster number
	DWORD	bsFirstClustLow;		// Low DOWRD of Fist Cluster number
	DWORD	bsMirrorFirstClustHigh;	// High DWORD of Mirror First Cluster number
	DWORD	bsMirrorFirstClustLow;	// Low DOWRD of Mirror Fist Cluster number
	DWORD	bsRecordSize;			// Record Size
	DWORD	bsIndexBlockSize;		// Index Block Size
	DWORD   bsVolumeIDHigh;         // volume ID number- high dword
	DWORD   bsVolumeIDLow;          // volume ID number - low dword
	DWORD	bsCheckSum;				
} PACKED BOOTSECTOR_WINNT; //For More information please read LIBFLOPPY/docs


/*
Media Descriptor (For More information please read LIBFLOPPY/docs)
FF - 5 1/4 floppy, 320 KB
FE - 5 1/4 floppy, 160 KB
FD - 5 1/4 floppy, 360 KB
FC - 5 1/4 floppy, 360 KB
F9 - 5 1/4 floppy, 1.2 MB in DOS Version 3.0
F9 - 3 1/2 floppy, 720 KB in DOS Version 3.2
F8 - Any Hard Drive
F0 - 3 1/2 floppy, 1.44 MB
*/

#define VWIN32_DIOC_DOS_IOCTL		1
#define VWIN32_DIOC_DOS_INT25		2
#define VWIN32_DIOC_DOS_INT26		3
#define VWIN32_DIOC_DOS_INT13		4
#define VWIN32_DIOC_DOS_DRIVEINFO	6

#define DIOC_AH(regs) (((unsigned char*)&((regs)->reg_EAX))[1])
#define DIOC_AL(regs) (((unsigned char*)&((regs)->reg_EAX))[0])
#define DIOC_BH(regs) (((unsigned char*)&((regs)->reg_EBX))[1])
#define DIOC_BL(regs) (((unsigned char*)&((regs)->reg_EBX))[0])
#define DIOC_DH(regs) (((unsigned char*)&((regs)->reg_EDX))[1])
#define DIOC_DL(regs) (((unsigned char*)&((regs)->reg_EDX))[0])
#define DIOC_AX(regs) (((unsigned short*)&((regs)->reg_EAX))[0])
#define DIOC_BX(regs) (((unsigned short*)&((regs)->reg_EBX))[0])
#define DIOC_CX(regs) (((unsigned short*)&((regs)->reg_ECX))[0])
#define DIOC_DX(regs) (((unsigned short*)&((regs)->reg_EDX))[0])

#ifdef __GNUC__
#undef PACKED
#else
#undef PACKED
#pragma pack()
#endif

#endif


