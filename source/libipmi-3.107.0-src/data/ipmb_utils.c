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
* Filename: ipmb_utils.h
*
* Author: Andrew McCallum 
*
******************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "coreTypes.h"
#include "unix.h"
#include "dbgout.h"
#include "libi2c.h"
#include "ipmb_private.h"



u8 ipmi_checksum( u8 starting_val, u8 *buffer, size_t buf_len )
{
    u8 checksum = starting_val;
    int i;
    
    for( i = 0; i < (int)buf_len; i++ )
        checksum += buffer[ i ];
    
    return( (u8)(-checksum) );
}


int get_seq_num(u8 bus )
{
    int snfd = 0;
    int seq_num = 0;
    int new_seq_num = 0;
    bool file_empty = false;
	char seq_file[32];

	/*
	 * Each bus needs to have a separate sequence number
	 */
	sprintf(seq_file,SEQ_NUM_FILE,bus);
    /* The current sequence number is kept in a file for easy access   */
    /* between multiple processes that need to access it and update it */
    snfd = open(seq_file, O_RDWR );
    if( snfd < 0 )
    {
        /* If the sequence number file doesn't exist, create it. */
        if( ( errno == EEXIST ) || ( errno == ENOENT ) )
        {
            file_empty = true;
            snfd = open( seq_file, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR );
            if( snfd < 0 )
                TCRIT( "Cannot obtain a new sequence number" );
        }
        else
            TCRIT( "Cannot obtain a new sequence number" );
    }

    if( snfd >= 0 )
    {
        /* Obtain an exclusive lock on the sequence number file */
        if( file_lock_write( snfd ) < 0 )
        {
            TWARN( "Cannot obtain exclusive lock on sequence number file" );
            (void)close( snfd );
            return( seq_num );
        }

        if( !file_empty )
        {
            if( read( snfd, &seq_num, sizeof( int ) ) != (ssize_t)sizeof( int ) )
            {
                TWARN( "Cannot obtain a new sequence number" );
                (void)file_unlock( snfd );
                (void)close( snfd );
                return( seq_num );
            }
        }

        /* Set the file pointer back to the beginning of the file */
        if( lseek( snfd, (off_t)0, SEEK_SET ) < 0 )
            TWARN( "Cannot update sequence number" );
        else
        {
            /* The sequence number is only 6 bits long, so it can't be */
            /* greater than 63.                                        */
            if( seq_num >= 63 )
                new_seq_num = 0;
            else
                new_seq_num = seq_num + 1;

            if( write( snfd, &new_seq_num, sizeof( int ) ) != (ssize_t)sizeof( int ) )
                TWARN( "Cannot update sequence number" );
        }
    }

    /* Release our lock on the sequence number file */
    if( file_unlock( snfd ) == HHM_FAILURE )
        TWARN( "Cannot release lock on sequence number file" );

    (void)close( snfd );

    return( seq_num );

}


int ipmi_access_lock( u8 bus )
{
    int i2cfd;
    int count = 0;
    int status = HEALTH_MON_INACTIVE;
    int ret = -1;
	char i2c_dev_name[48];

    /* We'll handle opening and closing the FD ourselves, so that we can    */
    /* maintain an advisory lock to prevent other IPMI commands while we're */
    /* getting a response to ours.                                          */
	sprintf(i2c_dev_name, I2CSTUB_DEV, bus);
                                              
    i2cfd = open( i2c_dev_name, O_RDWR );
    if( i2cfd < 0 )
    {
    	TCRIT( "Cannot open %s",i2c_dev_name );
    	return( HHM_FAILURE );
    }

    for( count = 0; count < 100; count++ )
    {
       // Get exclusive lock 
        ret = file_lock_write( i2cfd );
        if( ret >= 0 )
        {
            // Check if we are suspended 
            status = i2cbus_status();
            if( status == HEALTH_MON_INACTIVE )
                (void)file_unlock( i2cfd );
            else
                break;
        }

        select_sleep(0,100);
    }

    if( ret < 0 ){
    	close(i2cfd);
        return( ret );
    }
    else if( status == HEALTH_MON_ACTIVE )
        return( i2cfd );
    else
    {
		close(i2cfd);
        return( HHM_SUSPENDED );
	}
}


int ipmi_access_unlock( int i2cfd )
{
    /* Release our exclusive lock on the i2c device file */
    if( file_unlock( i2cfd ) < 0 )
        TWARN( "Cannot release exclusive lock on /dev/i2c0" );
    
    return( close( i2cfd ) );
}
