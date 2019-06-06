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
* Filename: sdr_cache.c
*
******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dbgout.h>
#include <errno.h>
#include "libipmi_session.h"
#include "libipmi_errorcodes.h"
#include <libipmi_sdr.h>
#include "std_macros.h"




typedef struct 
{
	int max_sdr_len;
	int sdr_count;

} SDR_CACHE_HEADER;


int SDRCacheExists()
{
	FILE *sdrfile;
	

	/* Open the SDR file */
	sdrfile = fopen( SDR_CACHE_PATH, "r" );
	if ( sdrfile == NULL )
	{
		/* If sdr file does not exist...    */
		if ( errno == ENOENT )
		{
			return 0;
		}
		else
		{
			TCRIT("Could not open SDR cache and error indicates something other than non-existent-file :%d\n",errno);
			return 0;
		}
	}
	else
	{
		fclose(sdrfile);
		return 1;
	}
}


/* will create a SDR cache by reading from the IPMI stack   */
int CreateSDRCache(IPMI20_SESSION_T* pSession)
{
	uint16 wRet;
    SDRRepositoryAllocInfo_T alloc_info;
	SDRRepositoryInfo_T repo_info;
	SDR_CACHE_HEADER sdrch;
	int 	max_sdr_len;
    int 	sdr_count;
	uint8*	sdr_buffer;

	FILE*	sdrfile;

	wRet = IPMICMD_GetSDRRepositoryAllocInfo( pSession, &alloc_info, 10 );
    if( wRet != LIBIPMI_E_SUCCESS )
	{
		TCRIT("Error getting sdr repo alloc info : 0x%x\n",wRet);
        return wRet;
	}



    /* Get repository information */
    wRet = IPMICMD_GetSDRRepositoryInfo( pSession, &repo_info, 10 );
    if( wRet != LIBIPMI_E_SUCCESS )
	{
		TCRIT("Error getting repo info :0x%x\n",wRet);
		return wRet;
	}



    max_sdr_len = alloc_info.MaxRecSize * ipmitoh_u16( alloc_info.AllocUnitSize );
    sdr_count = ipmitoh_u16( repo_info.RecCt );
	




    /*allocate memory for holding SDRs*/
	sdr_buffer = malloc(max_sdr_len*sdr_count);
	TCRIT_ASSERT(sdr_buffer != NULL,"error allocating memory for holding SDR buffer\n");



	/*get the sdrs*/
	wRet = LIBIPMI_HL_GetAllSDRs(pSession,sdr_buffer,max_sdr_len*sdr_count,10);
	if(wRet != 0)
	{
		TCRIT("Error getting all SDRs while creating SDR cache :0x%x\n",wRet);
		free(sdr_buffer);
		return wRet;
	}



	/*write SDR cache to file*/
	sdrfile = fopen(SDR_CACHE_PATH,"w");
	if(sdrfile==NULL)
	{
		TCRIT("Could not open sdr cache file for writing\n");
		free(sdr_buffer);
		return ERR_SDRCACHE_FILEOP;  
	}

	/*we wrtie the header data structure to indicate count and max len*/
	sdrch.max_sdr_len = max_sdr_len;
	sdrch.sdr_count = sdr_count;

	if((signed) fwrite( &sdrch, (size_t)1,
				sizeof(SDR_CACHE_HEADER), sdrfile ) < 0 )
	{
		TWARN( "Cannot write to SDR Cache file: 0x%x\n",  errno  );
		free(sdr_buffer);
		fclose( sdrfile );
		return ERR_SDRCACHE_FILEOP;
	}

	 /* Write the SDR data to file */
	if( (signed)fwrite( sdr_buffer, (size_t)1,
				max_sdr_len*sdr_count, sdrfile ) < 0 )
	{
		TWARN( "Cannot write to SDR Cache file: 0x%x\n",  errno  );
		free(sdr_buffer);
		fclose( sdrfile );
		return ERR_SDRCACHE_FILEOP;
	}

	free(sdr_buffer);
	fclose( sdrfile );
	return 0;
}

/*Reads SDR cache from cache file 
buf_size holds input buffer size and gives back actual buffer size on errors (except open or seek) or success
pass null buffer to get size
*/
int ReadSDRCache(uint8* sdr_buffer,uint32* buff_size,uint32* max_sdr_len,uint32* sdr_count)
{
	FILE* sdrfile;
	int retval;
	size_t	filelen;
	size_t sdr_only_size;
	SDR_CACHE_HEADER sdrch;

	sdrfile = fopen ( SDR_CACHE_PATH, "r" );
	if ( sdrfile == NULL )
	{
        TCRIT("Could not open SDR cache for reading :0x%x\n",errno);
		return ERR_SDRCACHE_FILEOP;
	}

    /* Determine the length of the file */
	retval = fseek( sdrfile, 0, SEEK_END );
	if( retval < 0 )
	{
		TWARN( "Cannot seek SDR file: %s\n", strerror( errno ) );
		fclose( sdrfile );
		return ERR_SDRCACHE_FILEOP;;
	}

	filelen = (size_t)ftell( sdrfile );
	sdr_only_size = filelen - sizeof(SDR_CACHE_HEADER);



	if(*buff_size < sdr_only_size)
	{
		if(sdr_buffer != NULL)
			TCRIT("insufficient buffer provided to read sdr cache\n");
		*buff_size = sdr_only_size ;
        fclose(sdrfile);
		return ERR_SDRCACHE_BUFFSIZE;
	}


	if(sdr_buffer == NULL)
	{
		TCRIT("Null buffer passed to ReadSDRCache\n");
		*buff_size = sdr_only_size;
        fclose(sdrfile);
		return ERR_SDRCACHE_INVALID_ARGS;
	}

	


  /* Back to the beginning of the file (after seeking to the end above) */
	retval = fseek( sdrfile, 0, SEEK_SET );
	if( retval < 0 )
	{
		TWARN( "Cannot seek SDR file: %s\n", strerror( errno ) );
		fclose( sdrfile );
        return ERR_SDRCACHE_FILEOP;
	}

	/*read the cache header*/
	if( fread( &sdrch, (size_t)1, sizeof(SDR_CACHE_HEADER), sdrfile ) != sizeof(SDR_CACHE_HEADER) )
	{
		TWARN( "Cannot read cache header from SDR file: %s\n", strerror( errno ) );
		fclose( sdrfile );
		return ERR_SDRCACHE_FILEOP;
	}

	*max_sdr_len = sdrch.max_sdr_len;
	*sdr_count = sdrch.sdr_count;


	/* Read the entire file into the buffer we got */

	if( fread( sdr_buffer, (size_t)1, sdr_only_size, sdrfile ) != sdr_only_size)
	{
		TWARN( "Cannot read from SDR file: %s\n", strerror( errno ) );
		fclose( sdrfile );
		return ERR_SDRCACHE_FILEOP;
	}

	fclose(sdrfile);
	return 0;



}


/* provide buffer to fill with sdrs and fill size in*/
int CacheGetAllSDRS(IPMI20_SESSION_T* pSession,uint8* sdr_buffer,uint32* buff_size,uint32* max_sdr_len,uint32* sdr_count)
{
	uint16 wRet;

	if(!SDRCacheExists())
	{
		if(CreateSDRCache(pSession)!=0)
			TCRIT("Error creating SDR cache\n");
	}

    wRet =  ReadSDRCache(sdr_buffer,buff_size,max_sdr_len,sdr_count);
	if(wRet != 0)
	{
		return wRet;
	}

	return 0;
}




