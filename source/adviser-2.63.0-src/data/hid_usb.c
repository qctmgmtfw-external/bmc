/*****************************************************************
 **                                                             **
 **     (C) Copyright 2006-2009, American Megatrends Inc.       **
 **                                                             **
 **             All Rights Reserved.                            **
 **                                                             **
 **         5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                             **
 **         Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                             **
 ****************************************************************/

#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "coreTypes.h"
#include "dbgout.h"
//#include "icc_pdt.h"
#include "hidtypes.h"
#include "iusb.h"

#include "hidconf.h"
#include "stickyKeys.h"
#include "hid_usb.h"
#include "usb_ioctl.h"
#include "iusb.h"
#include "vmedia_ifc.h"
#include "hid_errno.h"
#include "hid_cmdhandlr.h"
#include "adviser_cfg.h"

#include "video_misc.h"
#include "video_interface.h"
#include "featuredef.h"

#define USB_DEVICE                    "/dev/usb"
#define ADVISER_KEY_FILE "/var/adviser_resource"
#define MAX_STRING_SIZE 80

u8	g_mouse_mode = ABSOLUTE_MOUSE_MODE;
u8	g_err_mouse_mode_init = MOUSE_MODE_SUCCESS;
int	is_LED_from_Client;
static  IUSB_IOCTL_DATA KeybdIoctlData;
static  IUSB_IOCTL_DATA MouseIoctlData;
static  IUSB_REQ_REL_DEVICE_INFO iUSBMouseDevInfo;
static  IUSB_REQ_REL_DEVICE_INFO iUSBKeybdDevInfo;
static  int gUsbDevice;          // handle to the USB device

int do_usb_ioctl(int cmd, u8 *data);
extern CoreFeatures_T g_corefeatures;
/**
 * @brief Checks if the packet is encryped, if encrypted decrypts the packet,
 *        calls the usb keyboard or mouse handler
 *
 * @param sock socket identifier
 * @param thread_id thread identifier
 * @param pPkt points to the data packet
 *
 * @return 0 on success, -1 on failure
 */
/**
 * @brief Issues USB_MOUSE_DATA ioctl to the USB driver
 *
 * @param pIusbPkt points to the USB Mouse data packet from the remote
 *
 * @returns returns 0
 */

static int
HandleUsbMOUSECmd (IUSB_HID_PACKET* pIusbPkt)
{
    //u8 Protocol;

#ifdef CONFIG_IPACK_ANALOG
	int retval = 0;
	static int HostIsNotReadingMouse = 0;
	static int Count = 0;
#endif /* CONFIG_IPACK_ANALOG */

    u8 AbsMousePlayback[sizeof(IUSB_HID_PACKET)+sizeof(DIRECT_ABSOLUTE_USB_MOUSE_PKT)];

    IUSB_HID_PACKET* pAbsMouseHidHdr = (IUSB_HID_PACKET*)AbsMousePlayback;
    DIRECT_ABSOLUTE_USB_MOUSE_PKT* pDirAbsMousePkt = (DIRECT_ABSOLUTE_USB_MOUSE_PKT*)(&(pAbsMouseHidHdr->Data));

    //HOST_MOUSE_PACKET is what come sin from the remote app and has the data.
    //we convert this to an apt form and send it to the driver
    HOST_MOUSE_PACKET* HostMousePkt = (HOST_MOUSE_PACKET*)(&(pIusbPkt->Data));


    //Protocol = pIusbPkt->Header.Protocol;

	do_usb_ioctl(USB_MOUSE_DATA, (u8*)pIusbPkt);

	return 0;

    if(pIusbPkt->DataLen == 5)
    {
        memcpy(pAbsMouseHidHdr,pIusbPkt,sizeof(IUSB_HID_PACKET));
        pDirAbsMousePkt->ScaledX = HostMousePkt->AbsX;
        pDirAbsMousePkt->ScaledY = HostMousePkt->AbsY;

#ifdef CONFIG_IPACK_ANALOG

		// In K1 with Linux host,rebooting the linux host will cuase the mouse
		// packets accumulated in the USB driver.The USB driver will time out
		// for each of the packet and will come out of this time out after
		// "N Seconds" which is proportional to the number of packets.
		// The above condition may cause the user to think that the card is frozen.
		// This patch will address this USB mouse issue in case of K1 products.
		// The following patch will discard the mouse packets until a
		// mouse click packet arrives or the packet count exceeds 1000.
		if(HostIsNotReadingMouse == 1)
		{
				Count++;
				if( (HostMousePkt->Event != 0) || (Count == MOUSE_DISCARD_COUNT))
				{
					TDBG("will try host again\n");
					Count = 0;
					HostIsNotReadingMouse = 0;
				}
		}

		if(HostIsNotReadingMouse == 0)
		{
			retval = do_usb_ioctl(USB_MOUSE_DATA,(u8*)AbsMousePlayback);

			if(retval == 1)
			{
					TDBG("host is not reading mouse RETVAL :%d\n",retval);
					HostIsNotReadingMouse = 1;
			}
		}

#else
		if ( g_mouse_mode == RELATIVE_MOUSE_MODE )
		{
			if ( set_usb_mouse_mode(ABSOLUTE_MOUSE_MODE) == -1 )
				return -1;

			g_mouse_mode = ABSOLUTE_MOUSE_MODE;
		}


		do_usb_ioctl(USB_MOUSE_DATA,(u8*)pIusbPkt);

#endif
    }
    else
    {
		if ( g_mouse_mode == ABSOLUTE_MOUSE_MODE )
		{
			if ( set_usb_mouse_mode(RELATIVE_MOUSE_MODE) == -1 )
				return -1;

			g_mouse_mode = RELATIVE_MOUSE_MODE;
		}
//        TCRIT("Realtive mouse recd in Hidserver playback...old style??\n");
        do_usb_ioctl(USB_MOUSE_DATA, (u8*)pIusbPkt);
    }

    return 0;
}

static int
SendKeyData(IUSB_HID_PACKET* pIusbPkt)
{
    unsigned char	keycode;
    int		index;
    unsigned char	*keybdData;
    unsigned char	stickykey;
    char keyPacket[ ( sizeof(IUSB_HID_PACKET)-1 + 8 ) ];

        /* take a backup before doing an ioctl */
    memcpy(keyPacket, pIusbPkt, ( sizeof(IUSB_HID_PACKET)-1 + 8 ) );


        /* if the key is other than sticky key,
           send the BREAK key code also
           after sending the MAKE code.
        */
    keybdData = ( (unsigned char *)keyPacket ) + sizeof(IUSB_HID_PACKET) - 1 ;

        /* this is where MAKE key-code exists */
    keycode = keybdData[2];

        /* Send MAKE code */
    do_usb_ioctl(USB_KEYBD_DATA, (u8 *)pIusbPkt);

    if ( keycode == 0 )
    {
        return 0;
    }

        /* check if sticky key.*/
    index = 0;
    while( ( stickykey = sticky_keys[index++]) != 0x00 )
    {
        if ( keycode == stickykey )
            return 0;
    }

        /* if not sticky key, send BREAK code */
    keybdData[2] = 0;

        /* Send BREAK code */
    do_usb_ioctl(USB_KEYBD_DATA, (u8*)keyPacket);


    return 0;
}

/**
 * @brief Issues USB_KEYBD_DATA ioctl to the USB driver
 *
 * @param pIusbPkt points to the USB keybd data packet from the remote
 *
 * @returns returns 0
 */
static int HandleUsbKEYBDCmd (IUSB_HID_PACKET* pIusbPkt)
{

    unsigned char	*keybdData;
    TDBG("Handling Usb KeyBd Command.\n");

   // DumpBuffer((u8*)pIusbPkt,48);

    keybdData = ( (unsigned char *)pIusbPkt) + sizeof(IUSB_HID_PACKET) - 1 ;

        /* if reserved bit is set to 1 */
    if( keybdData[1] == 1 )
    {	/* Auto KeyBreak Mode is ON */

            /* reset the reserved bit */
        keybdData[1] = 0;
        SendKeyData(pIusbPkt);

	/* If we receive a LED packet from client, the driver will be woken up due to LED event
 	*  We are hence, not sending the LED status back to the client  */
	is_LED_from_Client = 1;

    }
    else
    {
            /* Auto KeyBreak Mode is OFF */
        do_usb_ioctl(USB_KEYBD_DATA, (u8*)pIusbPkt);
    }

    return 0;
}

int
HandleUsbPacket (int sock, u8* pPkt)
{
    IUSB_HID_PACKET* pIusbPkt = (IUSB_HID_PACKET* )pPkt;
	if(0)
	{
		sock=sock; /* -Wextra, fix for unused parameters */
	}
#ifdef ENCRYPTION_SUPPORT
    if (pIusbPkt->Header.Reserved [0])
    {
            //TINFO ("Trying to Decrypt the Packet..");

        if (-1 == DecryptData (sock, thread_id, PKTTYPE_IUSB, (char*)pIusbPkt))
		{
            TWARN ("Adviserd : %s : Decryption failed.", __FUNCTION__ );
			return -1;
		}
    }

#endif

    switch (pIusbPkt->Header.DeviceType)
    {
        case IUSB_DEVICE_MOUSE:
            return HandleUsbMOUSECmd (pIusbPkt);
            break;

        case IUSB_DEVICE_KEYBD:
            return HandleUsbKEYBDCmd (pIusbPkt);
            break;

        default:
            TWARN ("Adviserd : %s : Invalid USB packet.", __FUNCTION__ );
            return HID_EINVALID_CMD;
    }

	return HID_EINVALID_CMD;
}

u8		get_usb_mouse_mode()
{
	u8	mouse_mode;

    if (0 != do_usb_ioctl(MOUSE_GET_CURRENT_MODE, &mouse_mode) )
	{
		TWARN("Adviserd : %s : Unable to get current mouse mode", __FUNCTION__ );
		return 0xff;
	}

	return mouse_mode;

}

int		set_usb_mouse_mode(u8	mouse_mode)
{
	u8	current_mouse_mode;

    if (0 != do_usb_ioctl(MOUSE_GET_CURRENT_MODE, &current_mouse_mode) )
	{
    	TWARN("Adviserd : %s : Unable to get current mouse mode", __FUNCTION__ );
		return -1;
	}


	if ( current_mouse_mode == mouse_mode )
		return 0; /* no change required */

	//Added the check for mousemode othermode and relative or same no need to send to change  the mouse mode to driver

    if( current_mouse_mode == RELATIVE_MOUSE_MODE && mouse_mode == OTHER_MOUSE_MODE )
    {
            return 0;
    }

    if( current_mouse_mode == OTHER_MOUSE_MODE && mouse_mode == RELATIVE_MOUSE_MODE )
    {
            return 0;
    }

	if ( mouse_mode == ABSOLUTE_MOUSE_MODE )
	{
		/* Absolute mouse mode */
		if (0 != do_usb_ioctl(MOUSE_REL_TO_ABS, NULL) )
		{
    		TWARN("Adviserd : %s : Unable to set mouse mode to absolute", __FUNCTION__ );
			return -1;
		}
		return 0;
	}
	else if ( mouse_mode == RELATIVE_MOUSE_MODE || mouse_mode == OTHER_MOUSE_MODE )
	{
		/* Relative mouse mode */
		if (0 != do_usb_ioctl(MOUSE_ABS_TO_REL, NULL) )
		{
    		TWARN("Adviserd : %s : Unable to set mouse mode to relative", __FUNCTION__ );
			return -1;
		}
		return 0;
	}
	else
	{
		/* Unknown mouse mode */
		TWARN("Adviserd : %s : Invalid mouse mode : %d", __FUNCTION__, mouse_mode);
	}

	return 0;
}

int  init_usb_mouse_mode()
{
	AdviserCfg_T AdviserCfg;
	u8	current_mouse_mode;
	u8  config_mouse_mode = ABSOLUTE_MOUSE_MODE;

	/* Get current mouse mode */
	g_mouse_mode = current_mouse_mode = get_usb_mouse_mode();

	if( current_mouse_mode == 0xff )
	{
		g_err_mouse_mode_init = MOUSE_MODE_ERR_GET_DRV;
		return -1;
	}


	if(0 == GetAdviserCfg(&AdviserCfg))
	{
		if (AdviserCfg.mouse_mode == RELATIVE_MOUSE_MODE)
		{
			config_mouse_mode = RELATIVE_MOUSE_MODE;
		}
		else if (AdviserCfg.mouse_mode == ABSOLUTE_MOUSE_MODE)
		{
			config_mouse_mode = ABSOLUTE_MOUSE_MODE;
		}
		else if (AdviserCfg.mouse_mode == OTHER_MOUSE_MODE)
		{
			config_mouse_mode = OTHER_MOUSE_MODE;
		}
		else
		{
			TWARN("Adviserd : %s :Invalid mouse mode in configuration file. Using default mouse mode %s", __FUNCTION__, (current_mouse_mode==ABSOLUTE_MOUSE_MODE)?"ABSOLUTE":"RELATIVE");
			g_err_mouse_mode_init = MOUSE_MODE_ERR_GET_CFG;
			return -1;
		}
	}
	else
	{
		TWARN("Adviserd : %s :Unable to read adviserd configuration file. Using default mouse mode %s", __FUNCTION__, (current_mouse_mode==ABSOLUTE_MOUSE_MODE)?"ABSOLUTE":"RELATIVE");
		g_err_mouse_mode_init = MOUSE_MODE_ERR_GET_CFG;
		return -1;
	}

	/* same no need to change the mouse mode */
	if( config_mouse_mode == current_mouse_mode )
		return 0;

	if ( config_mouse_mode == ABSOLUTE_MOUSE_MODE )
	{
		/* Absolute mouse mode */
		if (0 != do_usb_ioctl(MOUSE_REL_TO_ABS, NULL) )
		{
    		TWARN("Adviserd : %s : Unable to set mouse mode to absolute. using old relative", __FUNCTION__ );
			g_err_mouse_mode_init = MOUSE_MODE_ERR_SET_DRV;
			return -1;
		}

		TDBG("Mouse mode set to ABSOLUTE successfully\n");
		g_mouse_mode = config_mouse_mode;
		return 0;
	}
	// from usbe driver point of view, relative mouse mode 
	// and other mouse mode are same because they use same mouse report.
	else if ( config_mouse_mode == RELATIVE_MOUSE_MODE || config_mouse_mode == OTHER_MOUSE_MODE )
	{
		/* Relative mouse mode */
		if (0 != do_usb_ioctl(MOUSE_ABS_TO_REL, NULL) )
		{
    		TWARN("Adviserd : %s : Unable to set mouse mode to relative. using old absolute", __FUNCTION__ );
			g_err_mouse_mode_init = MOUSE_MODE_ERR_SET_DRV;
			return -1;
		}

		TDBG("Mouse mode set to RELATIVE successfully\n");
		g_mouse_mode = config_mouse_mode;
		return 0;
	}
	else
	{
		/* Unknown mouse mode */
		TWARN("Adviserd : %s : Invalid mouse mode : %d", __FUNCTION__, config_mouse_mode);
		g_err_mouse_mode_init = MOUSE_MODE_INVALID;
		return -1;
	}

	//g_mouse_mode = current_mouse_mode;

	return 0;
}

int Refresh_HID_Device()
{	
	struct timespec sleep = {0, 1000000*1000};
        if( do_usb_ioctl( USB_DEVICE_DISCONNECT, NULL) != 0 )
        {
                TCRIT( "Unable to detach vmedia devices\n");
                return -1;
        }

        nanosleep(&sleep, NULL);

        if( do_usb_ioctl( USB_DEVICE_RECONNECT, NULL) != 0 )
        {
                TCRIT( "Unable to detach vmedia devices\n");
                return -1;
        }

        return 0;
}


static u8*
AddAuthInfo (int cmd, IUSB_REQ_REL_DEVICE_INFO* pdev_info, u8* data)
{
        switch (cmd)
        {
                case USB_MOUSE_DATA:
                        SetAuthInfo (iUSBMouseDevInfo, data, 1);
                        return data;
                case MOUSE_ABS_TO_REL:
                case MOUSE_REL_TO_ABS:
                case MOUSE_GET_CURRENT_MODE:
                        SetAuthInfo (iUSBMouseDevInfo, (u8*)&MouseIoctlData, 0);
                        return (u8*)&MouseIoctlData;
                case USB_KEYBD_DATA:
                case USB_KEYBD_LED:
                case USB_KEYBD_LED_NO_WAIT:
                case USB_KEYBD_LED_RESET:
                        SetAuthInfo (iUSBKeybdDevInfo, data, 1);
                        return data;
                case USB_DEVICE_DISCONNECT:
                case USB_DEVICE_RECONNECT:
                        SetAuthInfo (*pdev_info, data, 0);
                        return data;
                case USB_KEYBD_EXIT:
                        SetAuthInfo (iUSBKeybdDevInfo, (u8*)&KeybdIoctlData, 0);
                        return (u8*)&KeybdIoctlData;
                default:
                        fprintf (stderr, "Error: AddAuthInfo() received unsupported IOCTL command\n");
        }
        return NULL;
}


int do_usb_ioctl(int cmd, u8 *data)
{
        int va = -1;
        u8* ModifiedData;

        if( gUsbDevice > 0 )
        {
        		if(g_corefeatures.power_consumption_virtual_device_usb == ENABLED)
        		{
        			if((cmd == USB_ENABLE_ALL_DEVICE ) || (cmd == USB_DISABLE_ALL_DEVICE) || (cmd == USB_GET_ALL_DEVICE_STATUS))
        			{
        				va = ioctl(gUsbDevice, cmd, data);
        			  	return va;
        			}
        		}
        			
                if ((cmd == USB_DEVICE_DISCONNECT) || (cmd == USB_DEVICE_RECONNECT))
                {
                        ModifiedData = (u8*) AddAuthInfo (cmd, &iUSBMouseDevInfo, (u8*)&MouseIoctlData);
                        if (ModifiedData == NULL)
                                return va;
                        va = ioctl(gUsbDevice, cmd, ModifiedData);

                        ModifiedData = (u8*) AddAuthInfo (cmd, &iUSBKeybdDevInfo, (u8*)&KeybdIoctlData);
                        if (ModifiedData == NULL)
                                return va;
                        va = ioctl(gUsbDevice, cmd, ModifiedData);
                }
                else
                {
                        ModifiedData = (u8*) AddAuthInfo (cmd, NULL, data);
                        if (ModifiedData == NULL)
                                return va;
                        va = ioctl(gUsbDevice, cmd, ModifiedData);
                }
        }
        if ((0 == va) && (cmd == MOUSE_GET_CURRENT_MODE))
                *data = ((IUSB_IOCTL_DATA*)ModifiedData)->Data;

        return va;
}


int save_usb_resource(void)
{
    FILE *fp;

    fp = fopen(ADVISER_KEY_FILE,"w");
    if(fp == NULL)
    {
        TCRIT("Unable to open file %s \n",ADVISER_KEY_FILE);
        return -1;
    }

    fprintf(fp,"Hid :%x %x %x\n",iUSBKeybdDevInfo.Key,iUSBKeybdDevInfo.DevInfo.DevNo,iUSBKeybdDevInfo.DevInfo.IfNum);
    fprintf(fp,"Mouse :%x %x %x",iUSBMouseDevInfo.Key,iUSBMouseDevInfo.DevInfo.DevNo,iUSBMouseDevInfo.DevInfo.IfNum);
    fclose(fp);
    return 0;
}

int get_usb_resource(USBHid_T *keybd,USBHid_T *mouse)
{
    FILE *fp;
    char line[MAX_STRING_SIZE];
    unsigned int dev,ifnum;

    fp = fopen(ADVISER_KEY_FILE,"r");
    if(fp == NULL)
    {
        TCRIT("Unable to open file %s \n",ADVISER_KEY_FILE);
        return -1;
    }

    while(!feof(fp))
    {
        if(fgets(line,MAX_STRING_SIZE,fp) == NULL)
        {
            break;
        }

        if(strstr(line,"Hid :"))
        {
            sscanf(line,"Hid :%x %x %x",&keybd->key,&dev,&ifnum);
            keybd->DevNo = (uint8)dev;
            keybd->IfNum = (uint8)ifnum;
        }

        if(strstr(line,"Mouse :"))
        {
            sscanf(line,"Mouse :%x %x %x",&mouse->key,&dev,&ifnum);
            mouse->DevNo = (uint8)dev;
            mouse->IfNum = (uint8)ifnum;
        }
    }
    fclose(fp);
    return 0;
}

static int OpenUSBDevice(void)
{
        gUsbDevice = open (USB_DEVICE, O_RDWR);
        if ( gUsbDevice < 0 )
        {
		return -1;
	}
	return 0;
}

static void CloseUSBDevice(void)
{
	if ( gUsbDevice > 0 )
	{
		do_usb_ioctl(USB_KEYBD_EXIT, NULL);
		ReleaseiUSBDevice (&iUSBMouseDevInfo);
		ReleaseiUSBDevice (&iUSBKeybdDevInfo);
		close(gUsbDevice);
		gUsbDevice = -1;
	}
}


int init_usb_resources (void)
{

        USBHid_T keybd = {0,0,0};
        USBHid_T mouse= {0,0,0};

    if ( 0 != OpenUSBDevice() )
    {
        fprintf( stderr, "Unable to open USB device. Closing Adviser\n");
        return -1;
    }

    iUSBKeybdDevInfo.DevInfo.DeviceType = IUSB_DEVICE_KEYBD;
    iUSBKeybdDevInfo.DevInfo.LockType = LOCK_TYPE_EXCLUSIVE;
    if (0 != RequestiUSBDevice (&iUSBKeybdDevInfo))
    {
            /*Helps in retrieving usb resources that needs to be freed 
               properly before requesting */
            get_usb_resource(&keybd,&mouse);
            if(keybd.key != 0)
            {
                iUSBKeybdDevInfo.Key = keybd.key;
                iUSBKeybdDevInfo.DevInfo.DevNo = keybd.DevNo;
                iUSBKeybdDevInfo.DevInfo.IfNum = keybd.IfNum;
                ReleaseiUSBDevice(&iUSBKeybdDevInfo);
                if(0 != RequestiUSBDevice(&iUSBKeybdDevInfo))
                {
                    CloseUSBDevice();
                    return -1;
                }
            }
            else
            {
                //fprintf (stderr, "Error Unable to find iUSB KEYBD Device \n");
                TCRIT("Error Unable to find iUSB KEYBD Device \n");
                CloseUSBDevice();
                return -1;
            }
    }

    iUSBMouseDevInfo.DevInfo.DeviceType = IUSB_DEVICE_MOUSE;
    iUSBMouseDevInfo.DevInfo.LockType = LOCK_TYPE_EXCLUSIVE;
    if (0 != RequestiUSBDevice (&iUSBMouseDevInfo))
    {
            /*Helps in retrieving usb resources that needs to be freed 
                properly before requesting */
            get_usb_resource(&keybd,&mouse);
            if(mouse.key != 0)
            {
                iUSBMouseDevInfo.Key = mouse.key;
                iUSBMouseDevInfo.DevInfo.DevNo = mouse.DevNo;
                iUSBMouseDevInfo.DevInfo.IfNum = mouse.IfNum;
                ReleaseiUSBDevice (&iUSBMouseDevInfo);
                if(0 != RequestiUSBDevice(&iUSBMouseDevInfo))
                {
                    CloseUSBDevice();
                    return -1;
                }
            }
            else
            {
                //fprintf (stderr, "Error Unable to find iUSB Mouse Device \n");
                TCRIT("Error Unable to find iUSB Mouse Device \n");
                CloseUSBDevice();
                return -1;
            }
    }
    return 0;
}

void release_usb_resources (void)
{
	CloseUSBDevice();
}
