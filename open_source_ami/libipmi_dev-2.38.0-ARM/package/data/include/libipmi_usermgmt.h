#ifndef __LIBIPMI_USER_MANAGEMENT_H__
#define __LIBIPMI_USER_MANAGEMENT_H__

#ifdef __GNUC__
#define PACK __attribute__ ((packed))
#else
#define PACK
#pragma pack( 1 )
#endif

#define			MAX_USERS				64
#define			MAX_USER_NAME_LEN		16

typedef struct {
	uint8		byUserID;
	char		szUserName[16];
} SetUser_T;

typedef struct {
	uint8		byCompletionCode;
	char		szUserName[16];
} GetUser_T;

#undef PACK
#ifndef __GNUC__
#pragma pack()
#endif


#endif

