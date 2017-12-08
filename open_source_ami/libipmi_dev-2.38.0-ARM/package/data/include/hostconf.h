#ifndef HOSTCONF_H
#define HOSTCONF_H

//#include <curi.h>

#define BEGIN_SIG   "$G2-CONFIG-HOST$"
#define BEGIN_SIG_LEN 16

#pragma pack(1)

typedef struct
{
    uint8   BeginSig[BEGIN_SIG_LEN];
    uint16  Command;
    uint16  Status;
    uint32  DataInLen;          /* Sent to us       */
    uint32  DataOutLen;         /* Sent by us       */
    uint32  InternalUseDataIn;
    uint32  InternalUseDataOut;
 } CONFIG_CMD;


/* Error Code for Status  */
#define IN_PROCESS 0x8000       /* Bit 15 of Status */

#define ERR_SUCCESS     0       /* Success */
#define ERR_BIG_DATA    1       /* Too Much Data */
#define ERR_NO_DATA     2       /* No/Less Data Available */
#define ERR_UNSUPPORTED 3       /* Unsupported Command */

/* Maximum data that can be sent/received in a single command */


/* Host Commands */
#define CMD_RESERVED        0x0000  /* Reserved (for CURI comaptible) */
#define CMD_OS_HEART_BEAT   0xfffe
#define CMD_GET_HOST_MOUSE  0xFFFF

#pragma pack()

#endif
