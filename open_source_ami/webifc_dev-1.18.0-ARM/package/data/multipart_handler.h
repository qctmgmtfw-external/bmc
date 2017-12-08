/****************************************************************
 **                                                            										  **
 **    (C)Copyright 2006-2009, American Megatrends Inc.     **
 **                                                            										  **
 **            All Rights Reserved.                            						  **
 **                                                            										  **
 **        5555 Oakbrook Pkwy Suite 200, Norcross,             	  **
 **                                                            										  **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.           **
 **                                                            										  **
 ****************************************************************/

#ifndef MULTIPART_HANDLER_H
#define MULTIPART_HANDLER_H

/*      define error values     */
#define SUCCESS                 (0)
#define ERR_BOUNDARY_NOT_FOUND  (01)

/*          define max lengths          */
#define MAX_FILENAME_SIZE       (256)

/*      define constant strings */
#define STR_MULTIPART           "multipart/form-data"
#define STR_BOUNDARY            "boundary"
#define STR_NAME                "name"
#define STR_BOUNDARY_END        "--"
#define STR_ENDOOFDATA          "\r\n\r\n"

#define C_EQUALSIGN             '='
#define C_BLANKSPACE            ' '
#define C_DBLQUOTE              '"'
#define C_HIPHEN                '-'
#define C_BOUNDARY_START_CHAR   'b'

/*  HTTPMultipart Document's state  */
#define MULTIPART_STATE_UNDEF   (-1)    /* Added to identify the socket close event during multipart session */
#define DOC_START               (0)
#define DATA_START              (1)
#define INPROCESSING_FILENAME   (2)
#define AT_START_OF_NAME        (3)
#define BEYOND_FILENAME         (4)
#define BEYOND_HDR_NEWLINES     (5)
#define BOUNDARY_START          (6)
#define BOUNDARY_RECEIVED       (7)
#define LAST_BOUNDARY_STOP      (8)

/*  G2 specific definitions         */
#define OUTPUTFILE_BASE_LOC     ""    /*  Modify this to point to proper writable base location   */

/*  Maximum file size permitted */
#define MAX_FILE_SIZE_PERMITTED 4*1024*1024

//typedef unsigned char g2_char;
typedef char g2_char;


typedef struct HTTPMultipartElement_tag
{

    short   state;      /*  current state of the document   */
    g2_char delimiter[MAX_FILENAME_SIZE];
                        /*      delimiter string        */
    int     delimSize;  /*  original size of the delimiter  */
    g2_char fileName[MAX_FILENAME_SIZE];
                        /*  File name in which data value will be stored    */
    int     fd;         /*      file descriptor of the saved file       */
    int     fileSize;   /*  size of the file so far saved   */
    int     expectedFileSize;    /*  expected file size mentioned by the web page    */
    g2_char *dataString; /*      pointer to data string  */
    int     strSize;    /*  no of bytes of strStart */
    int 	schedid;
    unsigned char timeout;

} HTTPMultipartElement;


extern int ProcessMultipartData(HTTPMultipartElement *mpelement, int writePerm);
extern int GetBoundary(HTTPMultipartElement *mpelement);

extern int DataStart(HTTPMultipartElement *mpelement);
extern int InProcessingName(HTTPMultipartElement *mpelement);
extern int CheckForName(HTTPMultipartElement *mpelement, int writePerm);
extern int CheckEndOfDataHeader(HTTPMultipartElement *mpelement);
extern int SaveData(HTTPMultipartElement *mpelement);
extern int CheckBoundary(HTTPMultipartElement *mpelement);
extern int EOFProcessing(HTTPMultipartElement *mpelement);
extern int EODProcessing(HTTPMultipartElement *mpelement);



#endif  /*      MULTIPART_HANDLER_H       */
