/*****************************************************************
******************************************************************
***                                                            ***
***        (C)Copyright 2008, American Megatrends Inc.         ***
***                                                            ***
***                    All Rights Reserved                     ***
***                                                            ***
***       5555 Oakbrook Parkway, Norcross, GA 30093, USA       ***
***                                                            ***
***                     Phone 770.246.8600                     ***
***                                                            ***
******************************************************************
******************************************************************
******************************************************************
*
* Filename: libipmi_fru.c
*
******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "dbgout.h"
#include "libipmi_fru.h"

#if LIBIPMI_IS_OS_LINUX()
#include <syslog.h>
#endif

#if LIBIPMI_IS_OS_WINDOWS()
#define snprintf sprintf_s
#endif

#ifdef _USE_ARR_MALLOCS_
#define MAX_MALLOCS			32

static uint8 *arrMallocs[MAX_MALLOCS];
static uint32 nMallocs = 0;
#endif

#define SAFE_FREE(ptr) { if((ptr)) { free(ptr); ptr=NULL; } }
#define IPMI_FRU_VALID_VERSION  0x01


#define CUSTOM_AREA_FIELD		0xC1

//! Helper function
/*static uint8 GetBits(uint8 orig, uint8 startbit, uint8 endbit)
{
	uint8  temp = orig;
	uint8  mask = 0x00;
	int i;
	for(i=startbit;i>=endbit;i--)
	{
		mask = mask | (1 << i);
	}

	return (temp & mask);
}*/

const char * buf2str(uint8 * buf, int len)
{
    static char str[1024]={0};
    int i=0;

    if (len <= 0 || (len*2) > 1024)
        return NULL;

    memset(str, 0, 1024);

    for (i=0; i<len; i++)
    {
        if(snprintf(str+(i*2),sizeof(str)-(i*2),"%2.2x", buf[i]) >= (signed)(sizeof(str)-(i*2)))
        {
            TCRIT("Buffer Overflow\n");
            return NULL;
        }
    }
    if((len*2) <= 1023)
    {
    str[len*2] = '\0';
    }
    else
    {
    	TCRIT("Buffer overflow: Off-by-one in %s\n",__FUNCTION__);
    	str[1023] = '\0';
    }

    return (const char *)str;
}

static uint32 GetStringFromType(uint8 *startBuff, uint8 **str)
{
	//static const char bcd_plus[] = "0123456789 -.:,_";
	uint8 type = 0;
	uint8 len = 0;
	uint8 size = 0;
	uint8 i, j, k;
	char binarystring[4];
	int loop;

	union
	{
		uint32 bits;
		char chars[4];
	}u;

	type =((*startBuff & 0xC0) >> 6);

	len = GetBits(*startBuff, 5, 0);


	switch (type)
	{
		case 0:
			/** 00b:
			 * binary/unspecified
			 **/
			//! hex dump -> 2x length
			size =(len*2);
			break;
		case 2:
			/** 10b:
			 * 6 - bit ASCII
			 **/
			//! 4 chars per group of 1 - 3 bytes
			size =((((len + 2)*4)/3) & ~3);
			break;
		case 3:
				/** 11b:
			8 - bit ASCII */
		case 1:
			/** 01b:
			 * BCD plus
			 **/
			//! no length adjustment
			size = len;
			break;
	}


	*str =(uint8 *) malloc(size + 1);
         if(*str == NULL)
           {
             TCRIT ("libipmi_fru.c: Error allocating memory for str\n");
	     return -1;
   	   }    

	startBuff++;

	switch (type)
	{
		case 0:                 //! Binary
			{
				memset(*str,0x00,size);
				for(loop = 0; loop < (size/2); ++loop)
				{
					sprintf(binarystring,"%02X",*(startBuff+loop));
					strncat((char*)(*str),binarystring,2);
				}
			}
			break;

		case 1:                 //! BCD plus
			*(*str)= '\0';
			break;

		case 2:                 //! 6 - bit ASCII
			for (i = j = 0; i < len; i += 3)
			{
				u.bits = 0;
				k =((len - i) < 3 ?(len - i) : 3);
		#if 1
				u.chars[3] = *(startBuff + i);
				u.chars[2] =(k > 1 ? *(startBuff + i + 1) : 0);
				u.chars[1] =(k > 2 ? *(startBuff + i + 2) : 0);
		#define CHAR_IDX 3
		#else
				memcpy((void *)&u.bits, startBuff + i, k);
		#define CHAR_IDX 0
		#endif
				for (k = 0; k < 4; k++)
				{
				*(*str + j) =((u.chars[CHAR_IDX] & 0x3f) + 0x20);
				u.bits >>= 6;
				j++;
				}
			}
			*(*str + j) = '\0';


			break;
		case 3:
			memcpy(*str, startBuff, len);
			*(*str + len) = '\0';
			break;
	}
	return (uint32) (size + 1);
}


static int  GetInternalUseArea(uint8 *pBuff, FRUData_T *FRU)
{
    FRUCommonHeader_T *pTmp =(FRUCommonHeader_T *) pBuff;
    uint8 *pInternalUseArea = pBuff +(pTmp->InternalUseAreaStartOffset * 8);
    uint32 len = 0;

    uint32 size = 0;
    uint8 *tmpPtr = NULL;
    FRUInternalInfo_T InternalInfo;

    memset(&InternalInfo, 0, sizeof(FRUInternalInfo_T));


    InternalInfo.FormatVersion =	*pInternalUseArea;
    pInternalUseArea++;

    len = GetBits(*pInternalUseArea, 5, 0);
    if (len == 0)
    {
        InternalInfo.Data =(INT8U *) malloc(8);
           if(InternalInfo.Data  == NULL)
           {
             TCRIT ("libipmi_fru.c: Error allocating memory for FRUData_T\n");
             return -1;
           }

         memset(InternalInfo.Data,0,8);
        pInternalUseArea += 1;
    }
    else
    {
        size = GetStringFromType(pInternalUseArea, &tmpPtr);
        InternalInfo.Data =(INT8U *) malloc(size);
	   if(InternalInfo.Data  == NULL)
           {
             TCRIT ("libipmi_fru.c: Error allocating memory for FRUData_T\n");
             if(tmpPtr)
            	 free(tmpPtr);
             return -1;
           }

        memcpy(InternalInfo.Data, tmpPtr, size);
        pInternalUseArea += len + 1;
        SAFE_FREE (tmpPtr);
    }

#ifdef _USE_ARR_MALLOCS_
	arrMallocs[nMallocs] = InternalInfo.Data;
    nMallocs++;
#endif

    memcpy(&FRU->InternalInfo, &InternalInfo, sizeof(FRUInternalInfo_T));
return 0;

}


static int GetChassisInfoArea(uint8 *pBuff, FRUData_T *FRU)
{
    FRUCommonHeader_T *pTmp =(FRUCommonHeader_T *) pBuff;
    uint8 *pChassisArea = pBuff +(pTmp->ChassisInfoAreaStartOffset * 8);
    uint32 len = 0, chassisAreaLen = 0;
    uint32 size = 0; 
    uint8 *tmpPtr = NULL;
    int Index = 0;
    static const char * chassis_type[] = {
		"Unspecified", "Other", "Unknown",
		"Desktop", "Low Profile Desktop", "Pizza Box",
		"Mini Tower", "Tower",
		"Portable", "LapTop", "Notebook", "Hand Held",
		"Docking Station", "All in One", "Sub Notebook",
		"Space-saving", "Lunch Box", "Main Server Chassis",
		"Expansion Chassis", "SubChassis", "Bus Expansion Chassis",
		"Peripheral Chassis", "RAID Chassis", "Rack Mount Chassis"};

    FRU->ChassisInfo.ChassisInfoAreaFormatVersion =	*pChassisArea;
    pChassisArea++;
    FRU->ChassisInfo.ChassisInfoAreaLength = *pChassisArea;
    pChassisArea++;
    TDBG ("Chassis type : %d %d\n", (INT8U)*pChassisArea, sizeof (chassis_type)/ sizeof (char *));
    if ((*pChassisArea) >= sizeof (chassis_type)/ sizeof (char *))
        *pChassisArea = 0;
    size = strlen(chassis_type[*pChassisArea]);
    FRU->ChassisInfo.ChassisType =(INT8U *) malloc(size + 1);
	   if(FRU->ChassisInfo.ChassisType  == NULL)
           {
             TCRIT ("libipmi_fru.c: Error allocating memory for FRUData Table field ChassisType\n");
             return -1;
           }

    memcpy(FRU->ChassisInfo.ChassisType, chassis_type[*pChassisArea], size);
    FRU->ChassisInfo.ChassisType[size] = '\0';
    pChassisArea++;

    len = GetBits(*pChassisArea, 5, 0);
    if (len == 0)
    {
        FRU->ChassisInfo.ChassisPartNum =(INT8U *) malloc(8);
        if(FRU->ChassisInfo.ChassisPartNum == NULL)
           {
             TCRIT ("libipmi_fru.c: Error allocating memory for FRUData Table field ChassisPartNum\n");
             return -1;
           }

        memset(FRU->ChassisInfo.ChassisPartNum ,0,8);
        pChassisArea += 1;
    }
    else
    {
        size = GetStringFromType(pChassisArea, &tmpPtr);
        FRU->ChassisInfo.ChassisPartNum =(INT8U *) malloc(size);
        if(FRU->ChassisInfo.ChassisPartNum == NULL)
        {
             TCRIT ("libipmi_fru.c: Error allocating memory for FRUData Table field ChassisPartNum\n");
             if(tmpPtr)
            	 free(tmpPtr);
        	return -1;
        }
        memcpy(FRU->ChassisInfo.ChassisPartNum, tmpPtr, size);
        pChassisArea += len + 1;
        SAFE_FREE (tmpPtr);
    }
#ifdef _USE_ARR_MALLOCS_
    arrMallocs[nMallocs] = FRU->ChassisInfo.ChassisPartNum;
    nMallocs++;
#endif
    len = GetBits(*pChassisArea, 5, 0);
    if (len == 0)
    {
        FRU->ChassisInfo.ChassisSerialNum =(INT8U *) malloc(8);
        if(FRU->ChassisInfo.ChassisSerialNum == NULL)
        {
             TCRIT ("libipmi_fru.c: Error allocating memory for FRUData Table field ChassisSerialNum\n");
             return -1;
           }

        memset(FRU->ChassisInfo.ChassisSerialNum ,0,8);
        pChassisArea += 1;
    }
    else
    {
        size = GetStringFromType(pChassisArea, &tmpPtr);
        FRU->ChassisInfo.ChassisSerialNum =(INT8U *) malloc(size);
        if(FRU->ChassisInfo.ChassisSerialNum == NULL)
        {
             TCRIT ("libipmi_fru.c: Error allocating memory for FRUData Table field ChassisSerialNum\n");
             if(tmpPtr)
            	 free(tmpPtr);
             return -1;
        }
        memcpy(FRU->ChassisInfo.ChassisSerialNum, tmpPtr, size);
        pChassisArea += len + 1;
        SAFE_FREE (tmpPtr);
    }
#ifdef _USE_ARR_MALLOCS_
    arrMallocs[nMallocs] = FRU->ChassisInfo.ChassisSerialNum;
    nMallocs++;
#endif


    FRU->ChassisInfo.Total_Chassis_CustomField = 0;
    tmpPtr =  pChassisArea;
    chassisAreaLen = (unsigned long int)(((pBuff +(pTmp->ChassisInfoAreaStartOffset * 8)) + 
                            (FRU->ChassisInfo.ChassisInfoAreaLength * 8)) -1);
    while((*tmpPtr != CUSTOM_AREA_FIELD) && ((unsigned long int)tmpPtr <  chassisAreaLen))
    {
    	len = GetBits(*tmpPtr, 5, 0);
        tmpPtr += len + 1;
        FRU->ChassisInfo.Total_Chassis_CustomField += 1;
    }
    if(FRU->ChassisInfo.Total_Chassis_CustomField > 0)
    {
        FRU->ChassisInfo.CustomFields =(INT8U **) malloc(FRU->ChassisInfo.Total_Chassis_CustomField * sizeof(char *));
        if(FRU->ChassisInfo.CustomFields != NULL)
        {
        	while((*pChassisArea != CUSTOM_AREA_FIELD) && (Index < FRU->ChassisInfo.Total_Chassis_CustomField))
         	{
        		len = GetBits(*pChassisArea, 5, 0);
         		if (len == 0)
         		{	
         			//FRU->ChassisInfo.CustomFields =(INT8U **) malloc(1);
         			FRU->ChassisInfo.CustomFields[Index] =(INT8U *) malloc(8);
         			if( FRU->ChassisInfo.CustomFields[Index] != NULL )
         			{
         				memset(FRU->ChassisInfo.CustomFields[Index]  ,0,8);
         			}
         			else{
			             TCRIT ("libipmi_fru.c: Error allocating memory for FRUData Table ChassisInfo CustomFields\n");
			             return -1;
				    }
         			pChassisArea += 1;

         		}
       			else
       			{
       				size = GetStringFromType(pChassisArea, &tmpPtr);
       				//FRU->ChassisInfo.CustomFields =(INT8U **) malloc(1);
       				FRU->ChassisInfo.CustomFields[Index] =(INT8U *) malloc(size);
       				if(FRU->ChassisInfo.CustomFields[Index] != NULL)
       				{
       					memcpy((FRU->ChassisInfo.CustomFields[Index]), tmpPtr, size);
       				}
       				else{
			             TCRIT ("libipmi_fru.c: Error allocating memory for FRUData Table ChassisInfo CustomFields\n");
			             if(tmpPtr)
			            	 free(tmpPtr);
			             return -1;
				    } 
       				SAFE_FREE (tmpPtr);
       				pChassisArea += len + 1;
       				//FRU->ChassisInfo.Total_Chassis_CustomField +=1;
       			}
       			++ Index;
       		}
       	}
       	else{
             TCRIT ("libipmi_fru.c: Error allocating memory for FRUData Table ChassisInfo CustomFields\n");
             return -1;
        } 
   }

   return 0; 
}

static int GetBoardInfoArea(uint8 *pBuff, FRUData_T *FRU)
{

    FRUCommonHeader_T *pTmp =(FRUCommonHeader_T *) pBuff;
    uint8 *pBoardArea = pBuff +(pTmp->BoardInfoAreaStartOffset * 8);
    uint32 len = 0, boardAreaLen = 0;
    int Index = 0;

    uint32 size = 0;  
    uint8 *tmpPtr = NULL;


    FRU->BoardInfo.BoardInfoAreaFormatVersion = *pBoardArea;
    pBoardArea++;
    FRU->BoardInfo.BoardInfoAreaLength = *pBoardArea;
    pBoardArea++;
    FRU->BoardInfo.Language = *pBoardArea;
    pBoardArea++;
    memcpy(FRU->BoardInfo.MfgDateTime, pBoardArea, 3);
    pBoardArea += 3;
    len = GetBits(*pBoardArea, 5, 0);
    if (len == 0)
    {
        FRU->BoardInfo.BoardMfr =(INT8U *) malloc(8);
        if(FRU->BoardInfo.BoardMfr == NULL)
		{
             TCRIT ("libipmi_fru.c: Error allocating memory for FRUData Table field BoardMfr\n");
             return -1; 
        }

        memset(FRU->BoardInfo.BoardMfr ,0,8);
        pBoardArea += 1;
    }
    else
    {
        size = GetStringFromType(pBoardArea, &tmpPtr);
        FRU->BoardInfo.BoardMfr =(INT8U *) malloc(size);
        if(FRU->BoardInfo.BoardMfr == NULL)
       	{
             TCRIT ("libipmi_fru.c: Error allocating memory for FRUData Table field BoardMfr\n");
             if(tmpPtr)
            	 free(tmpPtr);
             return -1; 
        }
        memcpy(FRU->BoardInfo.BoardMfr, tmpPtr, size);
        SAFE_FREE (tmpPtr);
        pBoardArea += len + 1;
    }
#ifdef _USE_ARR_MALLOCS_
    arrMallocs[nMallocs] = FRU->BoardInfo.BoardMfr;
    nMallocs++;
#endif
    len = GetBits(*pBoardArea, 5, 0);
    if (len == 0)
    {
        FRU->BoardInfo.BoardProductName =(INT8U *) malloc(8);
        if(FRU->BoardInfo.BoardProductName == NULL)
        {
             TCRIT ("libipmi_fru.c: Error allocating memory for FRUData Table field BoardProductName\n");
             return -1; 
        }
        memset(FRU->BoardInfo.BoardProductName ,0,8);
        pBoardArea += 1;
    }
    else
    {
        size = GetStringFromType(pBoardArea, &tmpPtr);
        FRU->BoardInfo.BoardProductName =(INT8U *) malloc(size);
        if(FRU->BoardInfo.BoardProductName == NULL)
        {
             TCRIT ("libipmi_fru.c: Error allocating memory for FRUData Table field BoardProductName\n");
             if(tmpPtr)
            	 free(tmpPtr);
       		 return -1;
        }
        memcpy(FRU->BoardInfo.BoardProductName, tmpPtr, size);
        SAFE_FREE (tmpPtr);
        pBoardArea += len + 1;
    }
#ifdef _USE_ARR_MALLOCS_
    arrMallocs[nMallocs] = FRU->BoardInfo.BoardProductName;
    nMallocs++;
#endif
    len = GetBits(*pBoardArea, 5, 0);
    if (len == 0)
    {
        FRU->BoardInfo.BoardSerialNum =(INT8U *) malloc(8);
        if(FRU->BoardInfo.BoardSerialNum == NULL)
        {
             TCRIT ("libipmi_fru.c: Error allocating memory for FRUData Table field BoardSerialNum\n");
        	return -1;
        } 
        memset(FRU->BoardInfo.BoardSerialNum ,0,8);
        pBoardArea += 1;
    }
    else
    {
        size = GetStringFromType(pBoardArea, &tmpPtr);
        FRU->BoardInfo.BoardSerialNum =(INT8U *) malloc(size);
        if(FRU->BoardInfo.BoardSerialNum == NULL)
        {
             TCRIT ("libipmi_fru.c: Error allocating memory for FRUData Table field BoardSerialNum\n");
             if(tmpPtr)
            	 free(tmpPtr);
             return -1; 
         }
        memcpy(FRU->BoardInfo.BoardSerialNum, tmpPtr, size);
        SAFE_FREE (tmpPtr);
        pBoardArea += len + 1;
    }
#ifdef _USE_ARR_MALLOCS_
    arrMallocs[nMallocs] = FRU->BoardInfo.BoardSerialNum;
    nMallocs++;
#endif
    len = GetBits(*pBoardArea, 5, 0);
    if (len == 0)
    {
        FRU->BoardInfo.BoardPartNum =(INT8U *) malloc(8);
		if(FRU->BoardInfo.BoardPartNum == NULL)
        {
             TCRIT ("libipmi_fru.c: Error allocating memory for FRUData Table field BoardPartNum\n");
             return -1; 
        }
        memset(FRU->BoardInfo.BoardPartNum ,0,8);
        pBoardArea += 1;
    }
    else
    {
        size = GetStringFromType(pBoardArea, &tmpPtr);
        FRU->BoardInfo.BoardPartNum =(INT8U *) malloc(size);
		if(FRU->BoardInfo.BoardPartNum == NULL)
        {
             TCRIT ("libipmi_fru.c: Error allocating memory for FRUData Table field BoardPartNum\n");
             if(tmpPtr)
            	 free(tmpPtr);
             return -1; 
        }
        memcpy(FRU->BoardInfo.BoardPartNum, tmpPtr, size);
        SAFE_FREE (tmpPtr);
        pBoardArea += len + 1;
    }
#ifdef _USE_ARR_MALLOCS_
   arrMallocs[nMallocs] = FRU->BoardInfo.BoardPartNum;
   nMallocs++;
#endif
    len = GetBits(*pBoardArea, 5, 0);
    if (len == 0)
    {
        FRU->BoardInfo.FRUFileID =(INT8U *) malloc(8);
		if(FRU->BoardInfo.FRUFileID == NULL)
        {
             TCRIT ("libipmi_fru.c: Error allocating memory for FRUData Table field FRUFileID\n");
             return -1; 
        }
        memset(FRU->BoardInfo.FRUFileID ,0,8);
        pBoardArea += 1;
    }
    else
    {
        size = GetStringFromType(pBoardArea, &tmpPtr);
        FRU->BoardInfo.FRUFileID =(INT8U *) malloc(size);
		if(FRU->BoardInfo.FRUFileID == NULL)
        {
             TCRIT ("libipmi_fru.c: Error allocating memory for FRUData Table field FRUFileID\n");
             if(tmpPtr)
            	 free(tmpPtr);
             return -1; 
        }
        memcpy(FRU->BoardInfo.FRUFileID, tmpPtr, size);
        SAFE_FREE (tmpPtr);
        pBoardArea += len + 1;
    }
#ifdef _USE_ARR_MALLOCS_
    arrMallocs[nMallocs] = FRU->BoardInfo.FRUFileID;
    nMallocs++;
#endif
 
     FRU->BoardInfo.Total_Board_CustomField = 0;
     tmpPtr =  pBoardArea;
     boardAreaLen = (unsigned long int)(((pBuff +(pTmp->BoardInfoAreaStartOffset * 8)) + 
                             (FRU->BoardInfo.BoardInfoAreaLength * 8)) -1); 
     while((*tmpPtr != CUSTOM_AREA_FIELD) && ((unsigned long int)tmpPtr <  boardAreaLen))
     {
     	len = GetBits(*tmpPtr, 5, 0);
     	tmpPtr += len + 1;
     	FRU->BoardInfo.Total_Board_CustomField += 1;
     }
     if(FRU->BoardInfo.Total_Board_CustomField > 0)
     {
     	FRU->BoardInfo.CustomFields =(INT8U **) malloc(FRU->BoardInfo.Total_Board_CustomField * sizeof(char *));
     	if( FRU->BoardInfo.CustomFields != NULL )
     	{
     		while((*pBoardArea!= CUSTOM_AREA_FIELD) && (Index < FRU->BoardInfo.Total_Board_CustomField))
     		{
     			len = GetBits(*pBoardArea, 5, 0);
     			if (len == 0)
     			{	
     				//FRU->BoardInfo.CustomFields =(INT8U **) malloc(1);
     				FRU->BoardInfo.CustomFields[Index] =(INT8U *) malloc(8);
     				if(FRU->BoardInfo.CustomFields[Index] !=  NULL )
     				{
     					 memset(FRU->BoardInfo.CustomFields[Index] ,0,8);
     				}
     				else{
			             TCRIT ("libipmi_fru.c: Error allocating memory for FRUData Table BoardInfo CustomFields\n");
        			     return -1;
				    } 
     				pBoardArea += 1;
     			}
     			else
     			{
     				size = GetStringFromType(pBoardArea, &tmpPtr);
     				//FRU->BoardInfo.CustomFields =(INT8U **) malloc(1);
     				FRU->BoardInfo.CustomFields[Index] =(INT8U *) malloc(size);
     				if(FRU->BoardInfo.CustomFields[Index] != NULL)
     				{
     					memcpy((FRU->BoardInfo.CustomFields[Index]), tmpPtr, size);
     				}
     				else{
			             TCRIT ("libipmi_fru.c: Error allocating memory for FRUData Table BoardInfo CustomFields\n");
			             if(tmpPtr)
			            	 free(tmpPtr);
			             return -1;
				    } 
     				SAFE_FREE (tmpPtr);
     				pBoardArea += len + 1;
     				//FRU->BoardInfo.Total_Board_CustomField +=1;
     			}
     			++ Index;
     		}
     	}
     	else{
             TCRIT ("libipmi_fru.c: Error allocating memory for FRUData Table BoardInfo CustomFields\n");
             return -1;
	    } 
     }
   return 0; 
}


static int GetProductInfoArea(uint8 *pBuff, FRUData_T *FRU)
{
    FRUCommonHeader_T *pTmp =(FRUCommonHeader_T *) pBuff;
    uint8 *pProductArea = pBuff +(pTmp->ProductInfoAreaStartOffset * 8);
    uint8 len = 0;
    uint32 size = 0, productAreaLen = 0;
    int Index = 0,ret=0;

    uint8 *tmpPtr = NULL;

    if(pProductArea == NULL)
	return -1;

    FRU->ProductInfo.ProductInfoAreaFormatVersion = *pProductArea;
    pProductArea++;
    FRU->ProductInfo.ProductInfoAreaLength = *pProductArea;
    pProductArea++;
    FRU->ProductInfo.Language = *pProductArea;
    pProductArea++;

    len = GetBits(*pProductArea, 5, 0);
    if (len == 0)
    {
        FRU->ProductInfo.MfrName =(uint8 *) malloc(8);
        if(FRU->ProductInfo.MfrName == NULL)
		{
		     TCRIT ("libipmi_fru.c: Error allocating memory for FRUData Table field ProductInfo MfrName\n");
		     return -1; 
		}
        memset(FRU->ProductInfo.MfrName,0,8);
        pProductArea += 1;
    }
    else
    {
        size = GetStringFromType(pProductArea, &tmpPtr);
        FRU->ProductInfo.MfrName =(uint8 *) malloc(size);
        if(FRU->ProductInfo.MfrName == NULL)
		{
		     TCRIT ("libipmi_fru.c: Error allocating memory for FRUData Table field ProductInfo MfrName\n");
             if(tmpPtr)
            	 free(tmpPtr);
		     return -1; 
		}
        ret=snprintf((char*)(FRU->ProductInfo.MfrName),size,"%s", (const char *)tmpPtr);
        if(ret >= (signed)size)
        {
        	TCRIT("Buffer overflow in %s\n",__FUNCTION__);
			if(tmpPtr)
            	 free(tmpPtr);
        	return -1;
        }
        pProductArea += len + 1;
        SAFE_FREE (tmpPtr);
    }
#ifdef _USE_ARR_MALLOCS_
    arrMallocs[nMallocs] = FRU->ProductInfo.MfrName;
    nMallocs++;
#endif

    len = GetBits(*pProductArea, 5, 0);
    if (len == 0)
    {
        FRU->ProductInfo.ProductName =(uint8 *) malloc(8);
        if(FRU->ProductInfo.ProductName == NULL)
		{
		     TCRIT ("libipmi_fru.c: Error allocating memory for FRUData Table field ProductInfo-ProductName\n");
		     return -1; 
		}
         memset(FRU->ProductInfo.ProductName,0,8);
        pProductArea += 1;
    }
    else
    {
        size = GetStringFromType(pProductArea, &tmpPtr);
        FRU->ProductInfo.ProductName =(uint8 *) malloc(size);
        if(FRU->ProductInfo.ProductName == NULL)
		{
		     TCRIT ("libipmi_fru.c: Error allocating memory for FRUData Table field ProductInfo-ProductName\n");
             if(tmpPtr)
            	 free(tmpPtr);
		     return -1;
		}
        ret=snprintf((char*)(FRU->ProductInfo.ProductName),size,"%s",(const char*)tmpPtr);
        if(ret >= (signed)size) 
        {
        	TCRIT("Buffer overflow in %s\n",__FUNCTION__);
			if(tmpPtr)
            	 free(tmpPtr);
        	return -1;
        }
        pProductArea += len + 1;
        SAFE_FREE (tmpPtr);
    }
#ifdef _USE_ARR_MALLOCS_
    arrMallocs[nMallocs] = FRU->ProductInfo.ProductName;
    nMallocs++;
#endif
    len = GetBits(*pProductArea, 5, 0);

    if (len == 0)
    {
        FRU->ProductInfo.ProductPartNum =(uint8 *) malloc(8);
        if(FRU->ProductInfo.ProductPartNum == NULL)
		{
		     TCRIT ("libipmi_fru.c: Error allocating memory for FRUData Table field ProductInfo-ProductPartNum\n");
		     return -1; 
		}
		memset(FRU->ProductInfo.ProductPartNum,0,8);
        pProductArea += 1;
    }
    else
    {
        size = GetStringFromType(pProductArea, &tmpPtr);
        FRU->ProductInfo.ProductPartNum =(uint8 *) malloc(size);
        if(FRU->ProductInfo.ProductPartNum == NULL)
		{
		     TCRIT ("libipmi_fru.c: Error allocating memory for FRUData Table field ProductInfo-ProductPartNum\n");
             if(tmpPtr)
            	 free(tmpPtr);
		     return -1; 
		}
        ret=snprintf((char*)(FRU->ProductInfo.ProductPartNum),size,"%s",(const char*)tmpPtr);
        if(ret >= (signed)size)
        {
            TCRIT("Buffer overflow in %s\n",__FUNCTION__);
			if(tmpPtr)
            	 free(tmpPtr);
            return -1;
        }
        pProductArea += len + 1;
        SAFE_FREE (tmpPtr);
    }

#ifdef _USE_ARR_MALLOCS_
    arrMallocs[nMallocs] = FRU->ProductInfo.ProductPartNum;
    nMallocs++;
#endif
    len = GetBits(*pProductArea, 5, 0);
    if (len == 0)
    {
        FRU->ProductInfo.ProductVersion =(uint8 *) malloc(8);
        if(FRU->ProductInfo.ProductVersion == NULL)
		{
		     TCRIT ("libipmi_fru.c: Error allocating memory for FRUData Table field ProductInfo-ProductVersion\n");
		     return -1; 
		}
		memset(FRU->ProductInfo.ProductVersion,0,8);
		
        pProductArea += 1;
    }
    else
    {
        size = GetStringFromType(pProductArea, &tmpPtr);
        FRU->ProductInfo.ProductVersion =(uint8 *) malloc(size);
        if(FRU->ProductInfo.ProductVersion == NULL)
		{
		     TCRIT ("libipmi_fru.c: Error allocating memory for FRUData Table field ProductInfo-ProductVersion\n");
             if(tmpPtr)
            	 free(tmpPtr);
		     return -1; 
		}
        ret=snprintf((char*)(FRU->ProductInfo.ProductVersion),size,"%s",(const char*)tmpPtr);
        if(ret >= (signed)size)
        {
        	TCRIT("Buffer overflow in %s\n",__FUNCTION__);
			if(tmpPtr)
            	 free(tmpPtr);
        	return -1;
        }
        pProductArea += len + 1;
        SAFE_FREE (tmpPtr);
    }

#ifdef _USE_ARR_MALLOCS_
    arrMallocs[nMallocs] = FRU->ProductInfo.ProductVersion;
    nMallocs++;
#endif
    len = GetBits(*pProductArea, 5, 0);
    if (len == 0)
    {
        FRU->ProductInfo.ProductSerialNum =(uint8 *) malloc(8);
        if(FRU->ProductInfo.ProductSerialNum == NULL)
		{
		     TCRIT ("libipmi_fru.c: Error allocating memory for FRUData Table field ProductInfo-ProductSerialNum\n");
		     return -1; 
		}
		memset(FRU->ProductInfo.ProductSerialNum,0,8);
        pProductArea += 1;
    }
    else
    {
        size = GetStringFromType(pProductArea, &tmpPtr);
        FRU->ProductInfo.ProductSerialNum =(uint8 *) malloc(size);
        if(FRU->ProductInfo.ProductSerialNum == NULL)
		{
		     TCRIT ("libipmi_fru.c: Error allocating memory for FRUData Table field ProductInfo-ProductSerialNum\n");
             if(tmpPtr)
            	 free(tmpPtr);
		     return -1; 
		}
        ret=snprintf((char*)(FRU->ProductInfo.ProductSerialNum),size,"%s",(const char*)tmpPtr);
        if(ret >= (signed)size)
        {
        	TCRIT("Buffer overflow in %s\n",__FUNCTION__);
			if(tmpPtr)
            	 free(tmpPtr);
        	return -1;
        }
        pProductArea += len + 1;
        SAFE_FREE (tmpPtr);
    }
#ifdef _USE_ARR_MALLOCS_
    arrMallocs[nMallocs] = FRU->ProductInfo.ProductSerialNum;
    nMallocs++;
#endif
    len = GetBits(*pProductArea, 5, 0);
    if (len == 0)
    {
        FRU->ProductInfo.AssetTag =(uint8 *) malloc(8);
        if(FRU->ProductInfo.AssetTag == NULL)
		{
		     TCRIT ("libipmi_fru.c: Error allocating memory for FRUData Table field ProductInfo-AssetTag\n");
		     return -1; 
		}
		memset(FRU->ProductInfo.AssetTag,0,8);
        pProductArea += 1;
    }
    else
    {
        size = GetStringFromType(pProductArea, &tmpPtr);
        FRU->ProductInfo.AssetTag =(uint8 *) malloc(size);
        if(FRU->ProductInfo.AssetTag == NULL)
		{
		     TCRIT ("libipmi_fru.c: Error allocating memory for FRUData Table field ProductInfo-AssetTag\n");
             if(tmpPtr)
            	 free(tmpPtr);
		     return -1; 
		}
        strcpy((char*)(FRU->ProductInfo.AssetTag), (const char*)tmpPtr);
        ret=snprintf((char*)(FRU->ProductInfo.AssetTag),size,"%s",(const char*)tmpPtr);
        if(ret >= (signed)size)
        {
        	TCRIT("Buffer overflow in %s\n",__FUNCTION__);
			if(tmpPtr)
            	 free(tmpPtr);
        	return -1;
        }
        pProductArea += len + 1;
        SAFE_FREE (tmpPtr);
    }
#ifdef _USE_ARR_MALLOCS_
    arrMallocs[nMallocs] = FRU->ProductInfo.AssetTag;
    nMallocs++;
#endif
    len = GetBits(*pProductArea, 5, 0);
    if (len == 0)
    {
        FRU->ProductInfo.FRUFileID =(uint8 *) malloc(8);
        if(FRU->ProductInfo.FRUFileID == NULL)
		{
		     TCRIT ("libipmi_fru.c: Error allocating memory for FRUData Table field ProductInfo-FRUFileID\n");
		     return -1; 
		}
		memset(FRU->ProductInfo.FRUFileID,0,8);
        pProductArea += 1;
    }
    else
    {
        size = GetStringFromType(pProductArea, &tmpPtr);
        FRU->ProductInfo.FRUFileID =(uint8 *) malloc(size);
        if(FRU->ProductInfo.FRUFileID == NULL)
		{
		     TCRIT ("libipmi_fru.c: Error allocating memory for FRUData Table field ProductInfo-FRUFileID\n");
             if(tmpPtr)
            	 free(tmpPtr);
		     return -1; 
		}
        ret= snprintf((char*)(FRU->ProductInfo.FRUFileID),size,"%s",(const char*)tmpPtr);
        if(ret >= (signed)size)
        {
        	TCRIT("Buffer overflow in %s\n",__FUNCTION__);
			if(tmpPtr)
            	 free(tmpPtr);
        	return -1;
        }
        pProductArea += len + 1;
        SAFE_FREE (tmpPtr);
    }
#ifdef _USE_ARR_MALLOCS_
    arrMallocs[nMallocs] = FRU->ProductInfo.FRUFileID;
    nMallocs++;
#endif


    FRU->ProductInfo.Total_Product_CustomField = 0;
    tmpPtr =  pProductArea;
    productAreaLen = (unsigned long int)(((pBuff +(pTmp->ProductInfoAreaStartOffset * 8)) + 
                            (FRU->ProductInfo.ProductInfoAreaLength * 8)) -1);
    while((*tmpPtr != CUSTOM_AREA_FIELD) && ((unsigned long int)tmpPtr <  productAreaLen))
    {
    	len = GetBits(*tmpPtr, 5, 0);
        tmpPtr += len + 1;
        FRU->ProductInfo.Total_Product_CustomField += 1;
    }
    if(FRU->ProductInfo.Total_Product_CustomField > 0)
    {
        FRU->ProductInfo.CustomFields =(INT8U **) malloc(FRU->ProductInfo.Total_Product_CustomField * sizeof(char *));
        if(FRU->ProductInfo.CustomFields  != NULL)
        {
        	while((*pProductArea!= CUSTOM_AREA_FIELD) && (Index < FRU->ProductInfo.Total_Product_CustomField))
        	{
        		len = GetBits(*pProductArea, 5, 0);
        		if (len == 0)
        		{	
        			//FRU->ProductInfo.CustomFields =(INT8U **) malloc(1);
        			FRU->ProductInfo.CustomFields[Index] =(INT8U *) malloc(8);
        			if(FRU->ProductInfo.CustomFields[Index] != NULL)
        			{
        				memset(FRU->ProductInfo.CustomFields[Index],0,8);
        			}
        			else{
			             TCRIT ("libipmi_fru.c: Error allocating memory for FRUData Table ProductInfo CustomFields\n");
			             return -1;			
				    } 
        			pProductArea += 1;
        		}
        		else
        		{
        			size = GetStringFromType(pProductArea, &tmpPtr);
        			//FRU->ProductInfo.CustomFields =(INT8U **) malloc(1);
        			FRU->ProductInfo.CustomFields[Index] =(INT8U *) malloc(size);
        			if(FRU->ProductInfo.CustomFields[Index] != NULL)
        			{
        				memcpy((FRU->ProductInfo.CustomFields[Index]), tmpPtr, size);
        			}
        			else{
			             TCRIT ("libipmi_fru.c: Error allocating memory for FRUData Table ProductInfo CustomFields\n");
			             if(tmpPtr)
			            	 free(tmpPtr);
			             return -1;
				    } 
        			SAFE_FREE (tmpPtr);
        			pProductArea += len + 1;
        			//FRU->ProductInfo.Total_Product_CustomField +=1;
        		}
        		++ Index;
        	}
        }
        else{
             TCRIT ("libipmi_fru.c: Error allocating memory for FRUData Table ProductInfo CustomFields\n");
             return -1;	
	    } 
    }

   return 0;

}

int InterpretFRUData(uint8 *pBuff, FRUData_T *FRU,uint32  FRU_Size)
{
	FRUCommonHeader_T *pTmp =(FRUCommonHeader_T *) pBuff;

	if (pBuff == NULL)
	{
		memset(FRU, 0, sizeof(FRUData_T));
	}
	else
	{
		//! Copy full Common Header
		memcpy(&(FRU->CommonHeader), pTmp, sizeof(FRUCommonHeader_T));
	}

	/**
	 * ********************** Internal Use Area *******************************
	 **/
	if (FRU->CommonHeader.InternalUseAreaStartOffset != 0)
	{
		if((uint32)((pTmp->InternalUseAreaStartOffset * 8)) > FRU_Size)
		{		  	
			return FRU_INVALID_AREA;
		}
		if(GetInternalUseArea(pBuff, FRU))
		{
                      return -1;
		}
	}

	/**
	 * ********************** Chassis Info Area *******************************
	 **/
	if (FRU->CommonHeader.ChassisInfoAreaStartOffset != 0)
	{
		if((uint32)((pTmp->ChassisInfoAreaStartOffset * 8)) > FRU_Size)
		{		  	
			return FRU_INVALID_AREA;
		}
		if(GetChassisInfoArea(pBuff, FRU)!= 0)
		{
                      return -1;
		}
	}

	/**
	 * ********************** Board Info Area *******************************
	 **/
	if (FRU->CommonHeader.BoardInfoAreaStartOffset != 0)
	{
		if((uint32)((pTmp->BoardInfoAreaStartOffset * 8)) > FRU_Size)
		{		  	
			return FRU_INVALID_AREA;
		}
		if(GetBoardInfoArea(pBuff, FRU)!=0)
		{
                      return -1;
		}
	}

	/**
	 * ********************** Product Info Area *******************************
	 **/
	if (FRU->CommonHeader.ProductInfoAreaStartOffset != 0)
	{
		if((uint32)((pTmp->ProductInfoAreaStartOffset * 8)) > FRU_Size)
		{		  	
			return FRU_INVALID_AREA;
		}
		if( GetProductInfoArea(pBuff, FRU) != 0)
		{
                      return -1;
		}
	}
	
return 0;

}




static uint16 get_fru_info (IPMI20_SESSION_T* psession,
				  u8 fru_device_id,
				  u32* p_fru_size,
				  FRUData_T* fru_data,
				  int timeout)
{
	FRUInventoryAreaInfoReq_T 	fru_inv_area_req;
	FRUInventoryAreaInfoRes_T 	fru_inv_area_res;
	u8*							pFRUBuff = NULL;
	u8*							tempFRUBuff = NULL;
	u8*							fru_res_data = NULL;
	u32							fru_res_data_len = 0;
	FRUReadReq_T				fru_read_req;

	u32							wRet = 0;
	u32							fru_size = 0;
	const u8					BLOCK_OF_DATA_TO_READ = 128;
	u8                         isValidFRU = 0;
	FRUCommonHeader_T *pTmp = NULL;

	/**
	 * returns the overall size(in bytes) of the FRU
	 * inventory area in this device
	 **/
	fru_inv_area_req.FRUDeviceID = fru_device_id;

	wRet = IPMICMD_GetFRUInventoryAreaInfo (psession,
											&fru_inv_area_req,
											&fru_inv_area_res,
											timeout);
	if ((wRet != 0) &&((fru_inv_area_res.CompletionCode & 0xFF) != 0xCB))
	{

		goto _exit_get_fru_info_;
	}
	else
	{

		#if LIBIPMI_IS_OS_LINUX()
		syslog (LOG_INFO, "libipmi:get_fru_info: fru-size obtained for device-%d\n", fru_device_id);
		#endif

		if ((fru_inv_area_res.CompletionCode & 0xFF) == 0xCB)
		{
			goto InterpretFRU;
		}

		*p_fru_size = fru_size = (u32)fru_inv_area_res.Size;

		//! Allocate the buffer size to read FRU data for this FRU device
		tempFRUBuff = pFRUBuff 	= (u8* ) malloc (fru_size);
		if( tempFRUBuff == NULL)
		{
		     TCRIT ("libipmi_fru.c: Error allocating memory for tempFRUBuff\n");
		     return -1; 
		}

		if (0 == pFRUBuff ) goto _exit_get_fru_info_;


		//! Read complete FRU Data now
		fru_read_req.FRUDeviceID = fru_device_id;
		fru_read_req.Offset 	 = 0x00; //! go to start of FRU area

		fru_res_data_len		 = 	sizeof(FRUReadRes_T) + BLOCK_OF_DATA_TO_READ;
		fru_res_data 			 = (u8* ) malloc ( fru_res_data_len );
		if( fru_res_data == NULL)
		{
			 SAFE_FREE(pFRUBuff);  //Quanta coverity
		     TCRIT ("libipmi_fru.c: Error allocating memory for tempFRUBuff\n");
		     return -1; 
		}

		if (NULL == fru_res_data)
		{
			SAFE_FREE (tempFRUBuff);
			goto _exit_get_fru_info_;
		}

		// Initially read only the FRU header.
		fru_read_req.CountToRead = sizeof (FRUCommonHeader_T);
		
		while (fru_size) //! while there is data to read
		{
			wRet = IPMICMD_ReadFRUData (psession,
										&fru_read_req,
										(FRUReadRes_T *) fru_res_data,
										timeout);
			if (wRet != 0)
			{
				#if LIBIPMI_IS_OS_LINUX()
				syslog (LOG_ERR,
						"libipmi:get_fru_info: error reading fru data for device-%d\n",
						fru_device_id);
				#endif

				SAFE_FREE (fru_res_data);
				SAFE_FREE (tempFRUBuff);
				goto _exit_get_fru_info_;

			}
			else
			{
			    
				// Validate the FRU Header and proceed for further bytes.
				if (0 == isValidFRU)
				{
					pTmp = (FRUCommonHeader_T *) (fru_res_data + sizeof(FRUReadRes_T));
					if(pTmp->CommonHeaderFormatVersion != IPMI_FRU_VALID_VERSION)
					{
						SAFE_FREE (fru_res_data);
						SAFE_FREE (tempFRUBuff);
						wRet = FRU_INVALID_HEADER_VERSION;
						goto _exit_get_fru_info_;
					}
					isValidFRU = 1;
				}
				memcpy (pFRUBuff,
						(fru_res_data + sizeof(FRUReadRes_T)),
						fru_read_req.CountToRead);
				pFRUBuff += (fru_read_req.CountToRead);
			}
			
			//! recalculate the size left out
			fru_size 			-= fru_read_req.CountToRead;
			fru_read_req.Offset  = htoipmi_u16((ipmitoh_u16(fru_read_req.Offset) + fru_read_req.CountToRead));
			
			//! read only 128 bytes at-a-time
			fru_read_req.CountToRead = ( (fru_size > BLOCK_OF_DATA_TO_READ) ? BLOCK_OF_DATA_TO_READ : fru_size );
		}
		SAFE_FREE (fru_res_data);

InterpretFRU:
		memset(fru_data, 0, sizeof(FRUData_T));

		//! Interpret FRU Data
		wRet = InterpretFRUData (tempFRUBuff, fru_data,*p_fru_size);

		SAFE_FREE (tempFRUBuff);
		if(wRet!=0)
			goto _exit_get_fru_info_;

		
	}

_exit_get_fru_info_:
	return wRet;
}

uint16 LIBIPMI_HL_GetFRUData(IPMI20_SESSION_T *pSession,
					u8 fru_device_id,
				  	u32* p_fru_size,
				  	FRUData_T* fru_data,
					int timeout)
{
	return(get_fru_info(pSession,fru_device_id,p_fru_size,fru_data,timeout));

}

/*!
  * @brief       Free the fru data which includes chassis, board and product informations.
  * @param[in] fru_data - which contains the fru data.
  */
void LIBIPMI_HL_FreeFRUData( FRUData_T* fru_data )
{
    int i;
    
    //Chassis type
	SAFE_FREE(fru_data->ChassisInfo.ChassisType);
    
    //Chassis Part Num
    SAFE_FREE(fru_data->ChassisInfo.ChassisPartNum);

    //Chassis Serial Num
    SAFE_FREE(fru_data->ChassisInfo.ChassisSerialNum)

    // Chassis Custom field
    for (i = 0; i< fru_data->ChassisInfo.Total_Chassis_CustomField; i++)
    {
        SAFE_FREE(fru_data->ChassisInfo.CustomFields[i]);
    }
    SAFE_FREE(fru_data->ChassisInfo.CustomFields);
    
    //InternalInof Data
    SAFE_FREE(fru_data->InternalInfo.Data);
    
    // Board MfrName
    SAFE_FREE(fru_data->BoardInfo.BoardMfr);

    // Borad product name
    SAFE_FREE(fru_data->BoardInfo.BoardProductName);

    //Board Serial Num
    SAFE_FREE(fru_data->BoardInfo.BoardSerialNum);

    //Board Part Num
    SAFE_FREE(fru_data->BoardInfo.BoardPartNum);

    //Board FRUID
    SAFE_FREE(fru_data->BoardInfo.FRUFileID);

    //Board Custom fields
    for (i = 0; i< fru_data->BoardInfo.Total_Board_CustomField; i++)
    {
        SAFE_FREE(fru_data->BoardInfo.CustomFields[i]);
    }
    SAFE_FREE(fru_data->BoardInfo.CustomFields);

    //Product MfrName
    SAFE_FREE(fru_data->ProductInfo.MfrName);
    
    //Product Name
    SAFE_FREE(fru_data->ProductInfo.ProductName);

    //Product Part Num
    SAFE_FREE(fru_data->ProductInfo.ProductPartNum);

    // Freeing Product version
    SAFE_FREE(fru_data->ProductInfo.ProductVersion);

    //Product Serial Num
    SAFE_FREE(fru_data->ProductInfo.ProductSerialNum);

    //Product asset tag
    SAFE_FREE(fru_data->ProductInfo.AssetTag);

    // Product FRUID
    SAFE_FREE(fru_data->ProductInfo.FRUFileID);

    // Product Custom filed
    for (i = 0; i< fru_data->ProductInfo.Total_Product_CustomField; i++)
    {
        SAFE_FREE(fru_data->ProductInfo.CustomFields[i]);
    }
    SAFE_FREE(fru_data->ProductInfo.CustomFields);
 }

