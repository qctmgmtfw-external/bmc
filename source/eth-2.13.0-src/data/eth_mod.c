/****************************************************************
 **                                                            **
 **    (C)Copyright 2006-2009, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
****************************************************************/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/notifier.h>
#include <linux/reboot.h>
#include <linux/fs.h>
#include <coreTypes.h>
#include "usb_hw.h"
#include "descriptors.h"
#include "bot.h"
#include "iusb.h"
#include "usb_ioctl.h"
#include "usb_core.h"
#include "helper.h"
#include "dbgout.h"
#include "mod_reg.h"
#include "cdc.h"
#include "ether.h"
#include "rndis.h"
#include "eth_mod.h"

/* Local Definitions */
#define VENDOR_ID			1131	/* AMI = Decimal 1131 */
#define PRODUCT_ID_HUB		0xFF01
#define PRODUCT_ID_COMP_HID	0xFF10	/*Composite Hid Device : Mouse & Keybd*/
#define PRODUCT_ID_MOUSE	0xFF11
#define PRODUCT_ID_KEYBD	0xFF12
#define PRODUCT_ID_CDROM	0xFF20
#define PRODUCT_ID_FIX_HD	0xFF30	/* Fixed Harddisk */
#define PRODUCT_ID_REM_HD	0xFF31	/* Removable Harddisk */
#define PRODUCT_ID_FLOPPY	0xFF40
#define PRODUCT_ID_SUP_COMP	0xFF90	/* All in One - Super Combo */
#define PRODUCT_ID_COMP	    0xFF91	/* CD & Floppy Combo */
#define PRODUCT_ID_LUN_COMP 0xFF92	/* CD & Floppy Lun Based Combo */
#define DEVICE_REV			0x0100
#define PRODUCT_ID_COMP_ETH	0xFFb0	/*Composite Hid and Ethernet Device*/


/* Global variables */
TDBG_DECLARE_DBGVAR(eth);
int NotifyEpNum [2];
int DataOutEpNum [2];
int DataInEpNum [2];
int EthDevNo;
USB_CORE UsbCore;

static int CreateEthernetDescriptor(uint8 DevNo);
static int EthIoctl (unsigned int cmd,unsigned long arg, int* RetVal);
static int FillDevInfo (IUSB_DEVICE_INFO* iUSBDevInfo);
static int SetInterfaceAuthKey (uint8 DevNo, uint8 IfNum, uint32 Key);
static int ClearInterfaceAuthKey (uint8 DevNo, uint8 IfNum);

/* static variables */
static uint32 IfaceAuthKeys[1] = {0};
static USB_DEV	UsbDev =  {
	.DevUsbCreateDescriptor 		= CreateEthernetDescriptor,
	.DevUsbIOCTL 					= EthIoctl,
	.DevUsbSetKeybdSetReport 		= NULL,
	.DevUsbMiscActions 				= NULL,
	.DevUsbRemoteScsiCall 			= NULL,
	.DevUsbFillDevInfo 				= FillDevInfo,
	.DevUsbSetInterfaceAuthKey		= SetInterfaceAuthKey,
	.DevUsbClearInterfaceAuthKey	= ClearInterfaceAuthKey,
				};

static int 	eth_notify_sys(struct notifier_block *this, unsigned long code, void *unused);
static struct notifier_block eth_notifier =
{
       .notifier_call = eth_notify_sys,
};

static char *HWModuleName = NULL;

static usb_device_driver eth_device_driver =
{
	.module 		= THIS_MODULE,
	.descriptor     = CREATE_ETH_DESCRIPTOR,
	.devnum         = 0xFF,
	.speeds         = SUPPORT_HIGH_SPEED,
	.maxlun			= 0, // it is 0 based field, 1 means this device uses LUN0 and LUN1
};

static struct ctl_table_header *my_sys 	= NULL;
static struct ctl_table EthctlTable [] =
{

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,11))
        {"DebugLevel",&(TDBG_FORM_VAR_NAME(eth)),sizeof(int),0644,NULL,&proc_dointvec,NULL},
#else
	{CTL_UNNUMBERED,"DebugLevel",&(TDBG_FORM_VAR_NAME(eth)),sizeof(int),0644,NULL,NULL,&proc_dointvec}, 
#endif

	{0} 
};
static int Interval = 1;
module_param (HWModuleName, charp, 0000);
MODULE_PARM_DESC(HWModuleName, "which hardware module is correspond to Eth device module");
module_param (Interval, int, S_IRUGO);
MODULE_PARM_DESC(Interval, "bInterval field of Endpoint Descriptor");


static int
EthIoctl (unsigned int cmd,unsigned long arg, int* RetVal)
{

	switch (cmd)
	{
		default:
		TDBG_FLAGGED(eth, DEBUG_ETH,"Unsupported Ethernet IOCTL received...\n");
		return -1;
	}

}
#define CDC_SUPPORT 1
static int 
CreateEthernetDescriptor(uint8 DevNo)
{

	uint8 *MyDescriptor;
	uint16 TotalSize;
	uint16 Cfg1TotalSize = 0;
	uint16 Cfg2TotalSize = 0;
	char Mac[25];
	uint8 *m;
	ECM_ETH_DESC EthEcm;

	USB_CONFIG_DESC *Config;
	CDC_HEADER_DESC EthHeader;
	CDC_UNION_DESC EthUnion;
	
	CDC_CALLMGMT_DESC EthCallMgmt;
	CDC_ACM_DESC EthAcm;
	//USB_INTERFACE_ASSOC_DESC Assoc;

	INTERFACE_INFO EthIfInfo =
	{
		EthTxHandler, EthRxHandler, EthReqHandler, NULL, NULL, NULL, 
		EthRemHandler,NULL,0,0,0,0
	};  


	/* CONFIG for RNDIS */
	Cfg1TotalSize  = sizeof(USB_CONFIG_DESC);
	
	//Cfg1TotalSize += sizeof(USB_INTERFACE_ASSOC_DESC);
	
	//add for Ethernet
	Cfg1TotalSize += sizeof(USB_INTERFACE_DESC);
	Cfg1TotalSize += sizeof(CDC_HEADER_DESC);
	Cfg1TotalSize += sizeof(CDC_CALLMGMT_DESC);
	Cfg1TotalSize += sizeof(CDC_ACM_DESC);
	Cfg1TotalSize += sizeof(CDC_UNION_DESC);
	Cfg1TotalSize += sizeof(ECM_ETH_DESC);

	Cfg1TotalSize += (1*  sizeof(USB_ENDPOINT_DESC));
	
	Cfg1TotalSize += sizeof(USB_INTERFACE_DESC);
	Cfg1TotalSize += (2*  sizeof(USB_ENDPOINT_DESC));

#if CDC_SUPPORT == 1
	/* CONFIG for CDC */
	Cfg2TotalSize  = sizeof(USB_CONFIG_DESC);
	
	//Cfg2TotalSize += sizeof(USB_INTERFACE_ASSOC_DESC);
	
	//add for Ethernet
	Cfg2TotalSize += sizeof(USB_INTERFACE_DESC);
	Cfg2TotalSize += sizeof(CDC_HEADER_DESC);
	Cfg2TotalSize += sizeof(CDC_CALLMGMT_DESC);
	Cfg2TotalSize += sizeof(CDC_ACM_DESC);
	Cfg2TotalSize += sizeof(CDC_UNION_DESC);
	Cfg2TotalSize += sizeof(ECM_ETH_DESC);
	Cfg2TotalSize += (1*  sizeof(USB_ENDPOINT_DESC));
	
	// Deleberately not creating the NOP interface in order to
	// avoid the SET_ALT_SETTING request from host
	Cfg2TotalSize += sizeof(USB_INTERFACE_DESC);
	Cfg2TotalSize += (2*  sizeof(USB_ENDPOINT_DESC));
#endif

	TotalSize =  sizeof(USB_DEVICE_DESC) + Cfg1TotalSize + Cfg2TotalSize;

		
	//	Assoc.bLength = sizeof(USB_INTERFACE_ASSOC_DESC);
	//	Assoc.bDescriptorType = INTERFACE_ASSOCIATION;
	//	Assoc.bFirstInterface = 0;
	//	Assoc.bInterfaceCount = 2;
	//	Assoc.bFunctionClass = CDC_CONTROL_INTERFACE;
	//	Assoc.bFunctionSubClass = USB_RNDIS_SUBCLASS;
	//	Assoc.bFunctionProtocol = USB_RNDIS_PROTOCOL;
	//	Assoc.iFunction = AddStringDescriptor(DevNo, "AMI USB Ethernet Device");
		
	EthHeader.bLength = sizeof(CDC_HEADER_DESC);
	EthHeader.bDescriptorType = CS_INTERFACE;
	EthHeader.bDescriptorSubtype = 0x00;
	EthHeader.bcdCDCL = CDC_VERSION_MINOR;
	EthHeader.bcdCDCH = CDC_VERSION_MAJOR;
	
	EthCallMgmt.bLength = sizeof(CDC_CALLMGMT_DESC);
	EthCallMgmt.bDescriptorType = CS_INTERFACE;
	EthCallMgmt.bDescriptorSubtype = CDC_SUBTYPE_CALLMGMT;
	EthCallMgmt.bmCapabilities = 0x00;
	EthCallMgmt.bDataInterface = 0x01; //orig code has a value of 3 here.
	
	EthAcm.bLength = sizeof(CDC_ACM_DESC);
	EthAcm.bDescriptorType = CS_INTERFACE;
	EthAcm.bDescriptorSubtype = CDC_SUBTYPE_ACM;
	EthAcm.bmCapabilities = 0x00;
	
	EthUnion.bLength = sizeof(CDC_UNION_DESC);
	EthUnion.bDescriptorType = CS_INTERFACE;
	EthUnion.bDescriptorSubtype = 0x06; // union func desc
	EthUnion.bMaster = 0;
	EthUnion.bSlave = 1;


	MyDescriptor = UsbCore.CoreUsbCreateDescriptor(DevNo,TotalSize);
	if (MyDescriptor == NULL)
	{
		TCRIT("CreateEthernetDescriptor(): Memory Allocation Failure\n");
		return -1;
	}

	if (UsbCore.CoreUsbAddDeviceDesc(DevNo,VENDOR_ID,PRODUCT_ID_COMP_ETH,DEVICE_REV,
				"American Megatrends Inc.","Virtual Ethernet", 0) != 0)
	{
		TCRIT("CreateEthernetDescriptor(): Error in creating Device Desc\n");
		return -1;
	}

	{
		USB_DEVICE_DESC *device;
		device = UsbCore.CoreUsbGetDeviceDesc(DevNo);
		
		device->iSerialNumber = UsbCore.CoreUsbAddStringDescriptor(DevNo, "1234567890");
		//device->bDeviceClass = 0xEF;
		//device->bDeviceSubClass = 0x02;
		//device->bDeviceProtocol = 0x01;
		device->bDeviceClass = 2;
	}

	RndisSetVendor(VENDOR_ID, "American Megatrends Inc.");

	if (UsbCore.CoreUsbAddCfgDesc(DevNo, 2, 0) != 0)
	{
		TCRIT("CreateEthernetDescriptor(): Error in creating Cfg Desc\n");
		return -1;
	}

	Config = UsbCore.CoreUsbGetConfigDesc(DevNo, 1);
	Config->wTotalLengthL = LO_BYTE(Cfg1TotalSize);
	Config->wTotalLengthH = HI_BYTE(Cfg1TotalSize);

	//AddDescriptor(DevNo, (uint8 *)&Assoc, sizeof(USB_INTERFACE_ASSOC_DESC));
	
	/* Eth SECTION BEGIN*/
	EthSetIntf(0, 1);


	if (UsbCore.CoreUsbAddInterfaceDesc(DevNo,1,CDC_CONTROL_INTERFACE,USB_RNDIS_SUBCLASS,
	                     USB_RNDIS_PROTOCOL,"RNDIS Notification Interface",&EthIfInfo) !=0)
		
	{
		TCRIT("CreateEthernetDescriptor(): Error in creating Interface Desc\n");
		return -1;
	}


	if (UsbCore.CoreUsbAddDescriptor(DevNo, (uint8 *)&EthHeader, sizeof(CDC_HEADER_DESC)) != 0)
	{
		TCRIT("CreateEthernetDescriptor(): Error in creating CDC Header Desc\n");
		return -1;
	}

	if (UsbCore.CoreUsbAddDescriptor(DevNo, (uint8 *)&EthCallMgmt, sizeof(CDC_CALLMGMT_DESC)) != 0)
	{
		TCRIT("CreateEthernetDescriptor(): Error in creating CDC Call Management Desc\n");
		return -1;
	}

	if (UsbCore.CoreUsbAddDescriptor(DevNo, (uint8 *)&EthAcm, sizeof(CDC_ACM_DESC)) != 0)
	{
		TCRIT("CreateEthernetDescriptor(): Error in creating CDC ACM Desc\n");
		return -1;
	}

	if (UsbCore.CoreUsbAddDescriptor(DevNo, (uint8 *)&EthUnion, sizeof(CDC_UNION_DESC)) != 0)
	{
		TCRIT("CreateEthernetDescriptor(): Error in creating CDC Union Desc\n");
		return -1;
	}

	m = EthGetHostMac();
	snprintf(Mac, sizeof(Mac), "%02X%02X%02X%02X%02X%02X", 
	         m[0], m[1], m[2], m[3], m[4], m[5]);
	
	EthEcm.bMAC = UsbCore.CoreUsbAddStringDescriptor(DevNo, Mac);
	EthEcm.bLength = sizeof(ECM_ETH_DESC);
	EthEcm.bDescriptorType = CS_INTERFACE;
	EthEcm.bDescriptorSubtype = 0x0f;
	EthEcm.bStat[3] = EthEcm.bStat[2] = EthEcm.bStat[1] = EthEcm.bStat[0] = 0;
	EthEcm.bSegSizeL = USB_CDC_MTU & 0xFF;
	EthEcm.bSegSizeH = (USB_CDC_MTU >> 8) & 0xFF;
	EthEcm.bNumMCFiltersL = 0x00;
	EthEcm.bNumMCFiltersH = 0x00;
	EthEcm.bNumPwrFilters = 0x00;

	if (UsbCore.CoreUsbAddDescriptor(DevNo, (uint8 *)&EthEcm, sizeof(ECM_ETH_DESC)) != 0)
	{
		TCRIT("CreateEthernetDescriptor(): Error in creating ECM ETH Desc\n");
		return -1;
	}


	if (UsbCore.CoreUsbAddEndPointDesc(DevNo, IN,INTERRUPT,0x8,64,Interval,NON_DATA_EP) != 0)
	{
		TCRIT("CreateEthernetDescriptor(): Error in creating IN EndPointDesc\n");
		return -1;
	}

	NotifyEpNum [0] = UsbCore.CoreUsbGetNonDataEp(DevNo, 0);

	if (UsbCore.CoreUsbAddInterfaceDesc(DevNo,2,CDC_DATA_INTERFACE,0x00,
	                     0x00,"RNDIS Data Interface",&EthIfInfo) !=0)
		
	{
		TCRIT("CreateEthernetDescriptor(): Error in creating Interface Desc\n");
		return -1;
	}

	if (UsbCore.CoreUsbAddEndPointDesc(DevNo,OUT,BULK,64,512,0,DATA_EP) != 0)
	{
		TCRIT("CreateEthernetDescriptor(): Error in creating BULK OUT EndPointDesc\n");
		return -1;
	}

	DataOutEpNum [0] = UsbCore.CoreUsbGetDataOutEp(DevNo, 1);

	if (UsbCore.CoreUsbAddEndPointDesc(DevNo,IN,BULK,64,512,0,DATA_EP) != 0)
	{
		TCRIT("CreateEthernetDescriptor(): Error in creating BULK IN EndPointDesc\n");
		return -1;
	}

	DataInEpNum [0] = UsbCore.CoreUsbGetDataInEp(DevNo, 1);

#if CDC_SUPPORT == 1

	/* Eth SECTION START **/
	if (UsbCore.CoreUsbAddCfgDesc(DevNo, 2, 0) != 0)
	{
		TCRIT("CreateEthernetDescriptor(): Error in creating Cfg Desc\n");
		return -1;
	}

	Config = UsbCore.CoreUsbGetConfigDesc(DevNo, 2);

	Config->wTotalLengthL = LO_BYTE(Cfg2TotalSize);
	Config->wTotalLengthH = HI_BYTE(Cfg2TotalSize);
	
	//AddDescriptor(DevNo, (uint8 *)&Assoc, sizeof(USB_INTERFACE_ASSOC_DESC));

	/* Eth SECTION BEGIN*/
	EthSetIntf(0, 1);

	if (UsbCore.CoreUsbAddInterfaceDesc(DevNo,1,CDC_CONTROL_INTERFACE,USB_ECM_SUBCLASS,
	                     USB_ECM_PROTOCOL,"CDC Notification Interface",&EthIfInfo) !=0)
		
	{
		TCRIT("CreateEthernetDescriptor(): Error in creating Interface Desc\n");
		return 1;
	}

	if (UsbCore.CoreUsbAddDescriptor(DevNo, (uint8 *)&EthHeader, sizeof(CDC_HEADER_DESC)) != 0)
	{
		TCRIT("CreateEthernetDescriptor(): Error in creating CDC Header Desc\n");
		return -1;
	}

	if (UsbCore.CoreUsbAddDescriptor(DevNo, (uint8 *)&EthCallMgmt, sizeof(CDC_CALLMGMT_DESC)) != 0)
	{
		TCRIT("CreateEthernetDescriptor(): Error in creating CDC Call Management Desc\n");
		return -1;
	}

	if (UsbCore.CoreUsbAddDescriptor(DevNo, (uint8 *)&EthAcm, sizeof(CDC_ACM_DESC)) != 0)
	{
		TCRIT("CreateEthernetDescriptor(): Error in creating CDC ACM Desc\n");
		return -1;
	}

	if (UsbCore.CoreUsbAddDescriptor(DevNo, (uint8 *)&EthUnion, sizeof(CDC_UNION_DESC)) != 0)
	{
		TCRIT("CreateEthernetDescriptor(): Error in creating CDC Union Desc\n");
		return -1;
	}

	m = EthGetHostMac();
	snprintf(Mac, sizeof(Mac), "%02X%02X%02X%02X%02X%02X", 
	         m[0], m[1], m[2], m[3], m[4], m[5]);
	
	EthEcm.bMAC = UsbCore.CoreUsbAddStringDescriptor(DevNo, Mac);
	EthEcm.bLength = sizeof(ECM_ETH_DESC);
	EthEcm.bDescriptorType = CS_INTERFACE;
	EthEcm.bDescriptorSubtype = 0x0f;
	EthEcm.bStat[3] = EthEcm.bStat[2] = EthEcm.bStat[1] = EthEcm.bStat[0] = 0;
	EthEcm.bSegSizeL = USB_CDC_MTU & 0xFF;
	EthEcm.bSegSizeH = (USB_CDC_MTU >> 8) & 0xFF;
	EthEcm.bNumMCFiltersL = 0x00;
	EthEcm.bNumMCFiltersH = 0x00;
	EthEcm.bNumPwrFilters = 0x00;

	if (UsbCore.CoreUsbAddDescriptor(DevNo, (uint8 *)&EthEcm, sizeof(ECM_ETH_DESC)) != 0)
	{
		TCRIT("CreateEthernetDescriptor(): Error in creating ECM ETH Desc\n");
		return -1;
	}

	if (UsbCore.CoreUsbAddEndPointDesc(DevNo,IN,INTERRUPT,0x8,64,Interval,NON_DATA_EP) != 0)
	{
		TCRIT("CreateEthernetDescriptor(): Error in creating IN EndPointDesc\n");
		return -1;
	}

	NotifyEpNum [1] = UsbCore.CoreUsbGetNonDataEp(DevNo, 0);

	if (UsbCore.CoreUsbAddInterfaceDesc(DevNo,2,CDC_DATA_INTERFACE,0x00,
	                     0x00,"CDC Data Interface",&EthIfInfo) !=0)
		
	{
		TCRIT("CreateEthernetDescriptor(): Error in creating Interface Desc\n");
		return -1;
	}

	if (UsbCore.CoreUsbAddEndPointDesc(DevNo,OUT,BULK,64,512,0,DATA_EP) != 0)
	{
		TCRIT("CreateEthernetDescriptor(): Error in creating BULK OUT EndPointDesc\n");
		return -1;
	}

	DataOutEpNum [1] = UsbCore.CoreUsbGetDataOutEp(DevNo, 1);

	if (UsbCore.CoreUsbAddEndPointDesc(DevNo,IN,BULK,64,512,0,DATA_EP) != 0)
	{
		TCRIT("CreateEthernetDescriptor(): Error in creating BULK IN EndPointDesc\n");
		return -1;
	}
	
	DataInEpNum [1] = UsbCore.CoreUsbGetDataInEp(DevNo, 1);


	/*Eth SECTION END**/
#endif
	return 0;
}



static int
FillDevInfo (IUSB_DEVICE_INFO* iUSBDevInfo)
{
	uint8 NumDevices = 0;

	iUSBDevInfo->DeviceType = IUSB_DEVICE_COMM;
	iUSBDevInfo->DevNo      = eth_device_driver.devnum;
    iUSBDevInfo->IfNum      = 0; 
	iUSBDevInfo->LockType	= LOCK_TYPE_SHARED;
	iUSBDevInfo++;
	NumDevices++;

	return NumDevices;
}

static int 
SetInterfaceAuthKey (uint8 DevNo, uint8 IfNum, uint32 Key)
{
	TDBG_FLAGGED(eth, DEBUG_ETH,"SetInterfaceAuthKey() is called\n");
	if (DevNo != eth_device_driver.devnum)
	{
		TWARN ("SetInterfaceAuthKey(): Invalid Device Number %d\n", DevNo);
		return -1;
	}
	if (IfNum != 0)
	{
		TWARN ("SetInterfaceAuthKey(): Invalid Interface Number %d\n", IfNum);
		return -1;
	}
	IfaceAuthKeys[IfNum] = Key;
	return 0;
}

static int 
ClearInterfaceAuthKey (uint8 DevNo, uint8 IfNum)
{
	TDBG_FLAGGED(eth, DEBUG_ETH,"ClearInterfaceAuthKey() is called\n");
	if (DevNo != eth_device_driver.devnum)
	{
		TWARN ("ClearInterfaceAuthKey(): Invalid Device Number %d\n", DevNo);
		return -1;
	}
	if (IfNum != 0)
	{
		TWARN ("ClearInterfaceAuthKey(): Invalid Interface Number %d\n", IfNum);
		return -1;
	}
	IfaceAuthKeys[IfNum] = 0;
	return 0;
}

static int
init_eth_device_module (void)
{

	int ret_val;
	int devnum;
	uint8 hostmac[ETH_ALEN] = {0x00, 0x24, 0x8C, 0x46, 0x64, 0x2C };
	
	random_ether_addr(hostmac);
	EthSetHostMac(hostmac);

	printk ("Loading Ethernet Device Module...\n");
	if (0 != get_usb_core_funcs (&UsbCore))
	{
		TCRIT ("init_eth_device_module: Unable to get Core Module Functions...\n");
		return -1;
	}
	if (0 != EthCreateDevice())
	{
		TCRIT ("init_eth_device_module: Unable to Create Eth Device...\n");
		return -1;
	}
	if ((HWModuleName != NULL) && (sizeof(eth_device_driver.name) > strlen(HWModuleName)))
	{
		ret_val = snprintf(eth_device_driver.name , sizeof(eth_device_driver.name), "%s", HWModuleName);
		if( sizeof(eth_device_driver.name) <= (unsigned int)ret_val || ret_val == -1)
		{
			TCRIT("Buffer Overflow..in %s:%d\n", __FUNCTION__, __LINE__);
			return -1;
		}
	}
	devnum = UsbCore.CoreUsbGetFreeUsbDev (&eth_device_driver);
	if (devnum == -1)
	{
		TCRIT ("init_eth_device_module: Unable to get Free USB Hardware Module for Ethernet Device");
		EthRemoveDevice ();
		return -1;
	}
	eth_device_driver.devnum = devnum;
	EthDevNo = devnum;
	if(NULL != DevInfo[devnum].UsbDevice.UsbHwSetCurrentDevType)
	{
		if(DevInfo[devnum].UsbDevice.UsbHwSetCurrentDevType (devnum, eth_device_driver.descriptor))
		{
			TCRIT ("Error setting Current registering Dev Type to USB Hw module.\n");
			return -1;
		}
	}	
	if (0 != CreateEthernetDescriptor (devnum))
	{
		TCRIT ("init_eth_device_module: Unable to create Ethernet device descriptor\n");
		EthRemoveDevice ();
		UsbCore.CoreUsbDestroyDescriptor (devnum);
		return -1;
	}
	ret_val = register_usb_device (&eth_device_driver, &UsbDev);
	if (!ret_val)
	{
		register_reboot_notifier (&eth_notifier);
 		my_sys = AddSysctlTable("eth",&EthctlTable[0]);
	}
	else
	{
		TCRIT ("init_eth_device_module: Ethernet Device failed to register with USB core\n");
		EthRemoveDevice ();
	}
	return ret_val;
}

static void
exit_eth_device_module (void)
{
	printk ("Unloading Ethernet Device Module...\n");
	unregister_usb_device (&eth_device_driver);
	EthRemoveDevice ();
	unregister_reboot_notifier (&eth_notifier);
	if (my_sys) RemoveSysctlTable(my_sys);
	return;
}

static int
eth_notify_sys(struct notifier_block *this, unsigned long code, void *unused)
{

	TDBG_FLAGGED(eth, DEBUG_ETH,"Ethernet Device Reboot notifier...\n");
	if (code == SYS_DOWN || code == SYS_HALT)
	{
	   unregister_usb_device (&eth_device_driver);
	}
   return NOTIFY_DONE;
}


module_init(init_eth_device_module);
module_exit(exit_eth_device_module);

MODULE_AUTHOR("Rama Rao Bisa <RamaB@ami.com>");
MODULE_DESCRIPTION("USB Ethernet Device module");
MODULE_LICENSE("GPL");
