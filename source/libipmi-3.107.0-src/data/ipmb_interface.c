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
* Filename: ipmb_interface.c   
*
* Descriptions: Actual send/receive commands for use on the IPMB
*
* Author: Andrew McCallum
*
******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <errno.h>


/* Debug facilities from libami */
#include "dbgout.h"

#include "coreTypes.h"

/* I2C interface library */
#include "libi2c.h"

/* IPMI Structures, defines, and interfaces */
#include "ipmb_structs.h"

/* Internal IPMI utils and IPMB interfaces */
#include "ipmb_interface.h"
#include "ipmb_utils.h"

/* libipmi error codes */
#include "libipmi_errorcodes.h"

#include "unix.h"

#define I2C_GET_HOST_ADDR	0x0753	/* Get Slave Address of the Controller */


#define E_SUCCESS			(0x00)
#define E_NO_ENOUGH_MEMORY		(-0x01)
#define E_INVALID_HOST_ADDR		(-0x02)
#define E_I2C_WRITE_FAILURE		(-0x03)
#define E_I2C_READ_FAILURE		(-0x04)
#define E_I2C_BUS_SUSPEND		(-0x05)
#define E_SEQ_NUM_MISMATCH		(-0x06)
#define E_INSUFFICIENT_BUFFER_SIZE	(-0x07)
#define E_IPMB_LOCK_ACCESS_FAILED	(-0x08)
#define E_IPMB_COMM_FAILURE		(-0x09)
#define E_IPMI_CC_FAILURE		(-0x0a)

#define NETFUN(netfunlun)			(netfunlun>>2)
#define E_NETFUN_MISMATCH			(-0x0b)
#define E_CMD_MISMATCH			(-0x0c)

int ipmi_send_command( uint8 bus,u8 slave, u8 NetFnLUN,
                       u8 *command, size_t cmd_len, int i2cfd )
{
    IPMBMsg* ThisMsg;
    int seq_num = 0;
    int retval;
    int ioctl_ret = 0;

    /* Prepare some memory for the message, command, and checksum #1 */
    ThisMsg = malloc( sizeof( IPMBMsg ) + cmd_len + 1 );
    if( ThisMsg == NULL )
    {
        return( E_NO_ENOUGH_MEMORY );
    }    

	memset(ThisMsg,0,sizeof( IPMBMsg ) + cmd_len + 1);
    /* Get the current sequence number and update it. */
    seq_num = get_seq_num(bus);

    ThisMsg->header.netFnTargetLUN = NetFnLUN;
    ThisMsg->header.Checksum1 = ipmi_checksum( slave, (u8 *)ThisMsg, 1 );

    /* Determine the slave address of our controller dynamically */
    ioctl_ret = ioctl( i2cfd, I2C_GET_HOST_ADDR );
    if( ioctl_ret < 0 )
    {
        TCRIT( "Cannot get our own address" );
        free( ThisMsg );
        return( E_INVALID_HOST_ADDR );
    }
    else
        ThisMsg->header.SourceSA = (u8)ioctl_ret<<1;	

    ThisMsg->header.SourceSeqSourceLUN = SEQLUN( (u8)seq_num, (u8)00 );
    
    /* copy the command as is after the header */
    /*@-mayaliasunique@*/
    memcpy( ThisMsg->command.ipmbcmd, command, cmd_len );
    /*@=mayaliasunique@*/
    
    /* calculate checksum 2 and store it after the command */
    ThisMsg->command.ipmbcmd[ cmd_len ] =
        ipmi_checksum( (u8)0, (u8 *)&ThisMsg->header.SourceSA, cmd_len + 2 );
    
    /* Send the completed message over the I2C bus, skipping the first byte */
    /* The first byte is part of the start condition, and is written by the */
    /* i2c driver.                                                          */
	retval = (int)i2c_master_write_on_fd( i2cfd, (slave>>1), (u8 *)ThisMsg,
                                          sizeof( ThisMsg->header ) + cmd_len + 1 );

    free( ThisMsg );

    if( retval < 0 )
    {
        TCRIT( "Failed on i2c_master_write_on_fd" );
        return( E_I2C_WRITE_FAILURE );
    }
    else
        return( seq_num );
}


int ipmi_recv_response( int seq_num, u8 *buffer, size_t buf_len, int i2cfd )
{
    
    GenericMsgResponse *message;
    int retval = E_SUCCESS;

    retval = (int)i2c_slave_read_on_fd( i2cfd, buffer, buf_len );
    if( retval == HHM_FAILURE )
    {
        TCRIT( "i2c_slave_read_on_fd returned failure!\n" );
        return( E_I2C_READ_FAILURE );
    }
    else if( retval == HHM_SUSPENDED )
    {
        return( E_I2C_BUS_SUSPEND );
    }

    message = (GenericMsgResponse *)&buffer[1];

    /* Check that the completion code is OK */
    if( message->CompletionCode != 0 )
    {
        TDBG( "IPMI command returned completion code 0x%02x\n",
              (unsigned int)message->CompletionCode );
        retval = E_IPMI_CC_FAILURE;
    }

    /* If this is a response to some other command (bad!)... */
    if( SEQ( message->ipmbmsg.header.SourceSeqSourceLUN ) != (u8)seq_num )
    {
        TWARN( "Sequence number mismatch!  Got 0x%02x, expected 0x%02x",
               (unsigned int)SEQ( message->ipmbmsg.header.SourceSeqSourceLUN ),
               (unsigned)seq_num );
        retval = E_SEQ_NUM_MISMATCH;
    }

    return( retval );
}


uint16 ipmi_generic_command(uint8 bus, u8 slave, u8 NetFnLUN, u8 *command,
                          size_t cmd_len, u8 *response_buffer,
                          size_t *response_len )
{
    int i2cfd;
    int retval = -1;
    int seq_num = 0;
    int retries = 0;


	GenericMsgResponse *message = NULL;
    *response_buffer = (u8)0;

    /* Obtain exclusive access to the I2C bus to avoid access problems */
    i2cfd = ipmi_access_lock(bus);
    if( i2cfd == HHM_FAILURE )
    {
        return STATUS_CODE(MEDIUM_ERROR_FLAG, LIBIPMI_E_IPMB_LOCK_ACCESS_FAILED );
    }
    
//	printf("ipmi_generic_command,i2cfd = %d\n",i2cfd);
    /* Try to send the command and get a response up to IPMI_COMMAND_RETRIES times */
    while( ( retval < 0 ) && ( retries++ < IPMI_COMMAND_RETRIES ) )
    {
    	if(E_SEQ_NUM_MISMATCH!=retval)
		{	
		/* Send the IPMI command - any command at all */
        retval = ipmi_send_command(bus, slave, NetFnLUN, command,
                                    cmd_len, i2cfd );
		if( retval < 0 )
		{
			select_sleep(0,10);
        	continue;
		}
        else
            seq_num = retval;

    	}
        /* Receive the response from the IPMI device.  */
        retval = ipmi_recv_response( seq_num, response_buffer, *response_len, i2cfd );
		if(E_SEQ_NUM_MISMATCH==retval)
		{fprintf(stderr,"Sequence MisMatch\n");
			continue;
		}
		message = (GenericMsgResponse *)&response_buffer[1];

		
		/* If this is a response to some other command (bad!)... */
	    if(NETFUN(message->ipmbmsg.header.netFnTargetLUN)!=(NETFUN(NetFnLUN)+0x01))
	    {
	    	TWARN( "NetFun mismatch! ");
			retval = E_NETFUN_MISMATCH;
	    }
		 /* If this is a response to some other command (bad!)... */
		if(command[0]!=(message->ipmbmsg.command.ipmbcmd[0]))
		{

			TWARN( "Cmd mismatch! ");
			retval = E_CMD_MISMATCH;
		}

		if(E_IPMI_CC_FAILURE==retval)	//No need to try again if ipmi error came
			break;

		
    }

    /* We got out response, release the bus */
    (void)ipmi_access_unlock( i2cfd );

    if( retval < 0 )
    {
        if ( retval  ==  E_NO_ENOUGH_MEMORY)
		return STATUS_CODE(MEDIUM_ERROR_FLAG, LIBIPMI_E_NO_ENOUGH_MEMORY );
	else if( retval  ==  E_INVALID_HOST_ADDR)
		return STATUS_CODE(MEDIUM_ERROR_FLAG, LIBIPMI_E_INVALID_HOST_ADDR );
	else if( retval  ==  E_I2C_WRITE_FAILURE)
		return STATUS_CODE(MEDIUM_ERROR_FLAG, LIBIPMI_E_I2C_WRITE_FAILURE );
	else if( retval  ==  E_I2C_READ_FAILURE)
		return STATUS_CODE(MEDIUM_ERROR_FLAG, LIBIPMI_E_I2C_READ_FAILURE );
	else if( retval  ==  E_I2C_BUS_SUSPEND)
		return STATUS_CODE(MEDIUM_ERROR_FLAG, LIBIPMI_E_I2C_BUS_SUSPEND );
	else if( retval  ==  E_SEQ_NUM_MISMATCH)
		return STATUS_CODE(MEDIUM_ERROR_FLAG, LIBIPMI_E_SEQ_NUM_MISMATCH );
	else if( retval  ==  E_IPMI_CC_FAILURE)
	{
		//GenericMsgResponse *message;
		//message = (GenericMsgResponse *)response_buffer;

		message = (GenericMsgResponse *)&response_buffer[1];
		return STATUS_CODE(IPMI_ERROR_FLAG, message->CompletionCode);
	}
	else
		return STATUS_CODE(MEDIUM_ERROR_FLAG, LIBIPMI_E_IPMB_UNKNOWN_ERROR );
    }
    
    *response_len = retval;

    /* Return the length of the response */
    return LIBIPMI_E_SUCCESS;
}
