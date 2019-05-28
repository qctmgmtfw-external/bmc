/****************************************************************
 **                                                            **
 **    (C)Copyright 2006-2009, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross              **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
****************************************************************/

#include <linux/module.h>
#include <linux/pci.h>
#include <linux/fs.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/delay.h>

#include "jtag.h"
#include "jtag_ioctl.h"
#include "ast_jtag.h"
#include "altera.h"

#define DEVICE_FAMILY_FUNC(func_id) context->func(func_id, 0, context)
#define CHECK_MANUFACTURER_ID_ONLY 

//the define must below the jtag function include
#define SIR jtag_sir
#define SDR jtag_sdr
#define RUNTEST_IDLE   jtag_runtest_idle

int device_ALTERA(uint32_t cmd,unsigned int* buf, struct DEVICE_INFO* const context);
extern unsigned int jed_program(char* , unsigned long );
extern uint32_t get_ProgramUsercode(void);


enum device_cmd{
	DeviceFamily_Init,

  DeviceFamily_CheckID,
  DeviceFamily_Erase,
  DeviceFamily_Program,
  DeviceFamily_Verify,
  DeviceFamily_CheckSum,
  
  DeviceFamily_Run_NewCode,

  DeviceFamily_ProgramMode_Enter,
  DeviceFamily_ProgramMode_Exit,
  
  DeviceFamily_UserCode,
};

const struct DEVICE_INFO device_list[]={
  #ifdef CONFIG_SPX_FEATURE_ALTERA_SUPPORT 
  {.idcode =ALTERA_CPLD_EPM570_IDCODE, .name=ALTERA_CPLD_EPM570_DEVNAME,.func = device_ALTERA, .support_loop_func = 0},
  #endif
  #if 0
  {.idcode =ALTERA_CPLD_MAX_V_IDCODE, .name=ALTERA_CPLD_MAX_V_DEVNAME,.func = device_ALTERA, .support_loop_func = 0},
  #endif
  {.idcode =0, .name={0}, .func = 0, .support_loop_func = 0}
};

static int g_program_mode_cnt  = 0 ;
static int g_program_retry_cnt = 0 ;



#ifdef DEBUG
/*
 * base tool funcs
 */
void echo_arry(char *pt,int len){ 
    int i __attribute__ ((unused)) ;
    for (i=0; i<len; i++){
        if((i & 0xf )== 0) printk("\n");
        printk("%02X ",(unsigned char)pt[i]);
    }
    printk("\n");
}
#endif


/*
 * device_data_check
 * compare_data = 0: return check sum of read_buffer 
 * compare_data = address: 
 * return 0 : two block data is same.
 * return 1 : two block data is different.
 */
uint32_t device_data_check(unsigned int* read_buffer, unsigned int* compare_data, unsigned int bits_leng ){
	int ret = 0;	
	printk("Altera should not use device_data_check function.\n");
	return ret;
}


/*
 * read_code_size_data
 * Altera not using this function.
 */
void read_code_size_data (unsigned int *JTAG_data_buffer,unsigned int TDI)
{
	printk("Altera should not use read_code_size_data function.\n");
}

/*
 * clean_unused_buffer
 */
void clean_unused_buffer(char * buf, int value, unsigned int Device_All_Rows, unsigned int Device_Using_Rows){
	int size = ((Device_All_Rows - Device_Using_Rows ) * JTAG_device_information.Device_Column_Length /8);
	unsigned int shift =  Device_Using_Rows * JTAG_device_information.Device_Column_Length / 8;
	 
	 //printk("size %d,shift :%d\n",size,shift);
	 
	if(size > 0 && shift > 0){
		memset(buf + shift, (value & 0xff) , size);
	}
}

#ifdef CONFIG_SPX_FEATURE_ALTERA_SUPPORT
/*
 * device_ALTERA
 */
int device_ALTERA(uint32_t cmd,unsigned int* buf, struct DEVICE_INFO* const context){
	uint32_t TDI __attribute__ ((unused)) =0, TDO __attribute__ ((unused)) =0;
  int ret = 0;

	printk("device_ALTERA get cmd: 0x%x!!\n", cmd);

  //---------------------------------------------------------
  //---------------------------------------------------------
	switch(cmd){
		case DeviceFamily_Init:
				g_program_retry_cnt = 0;
				g_program_mode_cnt = 0;
			break;
		case DeviceFamily_CheckID:
		//----------------------------------------------------------------------
		//! Check the IDCODE
		  SIR (SIR_BITS, IDCODE);
		  SDR (32, 0, (void *) &TDO);
		  //----------------------------------------------------------------------
		  if(TDO != context->idcode){
			printk("Error IdCode not match.[%08X / %08X]\n",TDO, context->idcode);
			ret = 1;
		  }
		  break;
	  case DeviceFamily_UserCode:
          SIR (SIR_BITS, USERCODE);
		  SDR (32, 0, (void *) &TDO);
          *buf=TDO;
		  break;
      case DeviceFamily_Program:
    	  printk("Altera Program action code is in jbc firmware file.!!\n");
    	  ret = 1;
    	  break;

	  default:
		  printk("Not support!!\n");
		  ret = -1;
		  break;
	}

	return ret;
}
#endif

/*
 * hw_device_ctl
 */
uint32_t hw_device_ctl(unsigned int cmd ,unsigned int *buf,unsigned long data_size){
	uint32_t ret=0;
  struct DEVICE_INFO *pDevice = 0;
  int i;
  
  // These commands can be used without device id check.
  if (cmd == IOCTL_JTAG_DEVICE_INDEX){
 	  ret = set_device_number(data_size);
 	  if(ret != 0) printk("Action fail.\n");
 	  
 	  return ret;
  }

  for(i = 0; i<sizeof(device_list)/sizeof(device_list[0]); i++){
  	pDevice =(struct DEVICE_INFO *)&device_list[i];

	printk("pDevice->idcode = 0x%x, JTAG_device_information.Device_ID = 0x%x\n", pDevice->idcode, JTAG_device_information.Device_ID);
    if(pDevice->idcode == 0){
    	printk("The device not in Altera supportlist [%d]\n", i);
      return -1;
    }
    if(pDevice->idcode == JTAG_device_information.Device_ID){
      g_is_Support_LoopFunc = pDevice->support_loop_func;
      break;
    }
#ifdef CHECK_MANUFACTURER_ID_ONLY    
    //Manufacturer
	if(Manufacturer_Identity(pDevice->idcode) ==Manufacturer_Identity(JTAG_device_information.Device_ID)){
		g_is_Support_LoopFunc = pDevice->support_loop_func;
		break;	  
	}
#endif //CHECK_MANUFACTURER_ID_ONLY
  }
  
  switch(cmd){
  	case IOCTL_JTAG_ERASE_DEVICE:
  		pDevice->func(DeviceFamily_Init,0,pDevice);    
    	ret = pDevice->func(DeviceFamily_Erase, 0, pDevice);
      break;
      
    case IOCTL_JTAG_DEVICE_CHECKSUM:
    	ret = pDevice->func(DeviceFamily_CheckSum, buf, pDevice);
      break;
      
    case IOCTL_JTAG_READ_USERCODE:
    	ret = pDevice->func(DeviceFamily_UserCode, buf, pDevice);
      break;
      
    case IOCTL_JTAG_IDCODE_READ:
    	ret = pDevice->func(DeviceFamily_CheckID, 0, pDevice);
      break;
      
    case IOCTL_JTAG_VERIFY_DEVICE:
    	pDevice->func(DeviceFamily_Init,0,pDevice);    
    	ret = pDevice->func(DeviceFamily_Verify, buf,pDevice);
      break;
      
    case IOCTL_JTAG_PROGRAM_DEVICE:
    	pDevice->func(DeviceFamily_Init,0,pDevice);    
    	ret = pDevice->func(DeviceFamily_Program, buf,pDevice);
      break;
      
    case IOCTL_JTAG_DEVICE_TFR:
    	printk("Try CPLD TFR...\n");
    	pDevice->func(DeviceFamily_Init,0,pDevice);            
    	ret = pDevice->func(DeviceFamily_Run_NewCode, 0,pDevice);
      break;
      
    case IOCTL_JTAG_UPDATE_DEVICE:
    	printk("IOCTL_JTAG_JED_PROGRAM...\n");
    	ret = jed_program((char*)buf, data_size);
      break;        
        
    default:
    	ret = -1;
    }

    if(ret != 0) printk("Action fail.\n");    

    return ret;
}


/*
 * load_device_info
 */
uint32_t load_device_info(unsigned long id_code){
	int i;
	//----------------------------------------------------------------------------------
	const JTAG_DEVICE_INFO Altera_information[]=
	{

		#ifdef CONFIG_SPX_FEATURE_ALTERA_SUPPORT
		{  
		  .Device_Name    =               ALTERA_CPLD_EPM570_DEVNAME,
		  .Device_ID      =               ALTERA_CPLD_EPM570_IDCODE,
		  .Device_Column_Length   =       0,
		  .Device_Row_Length      =       0,
		  .Device_All_Bits_Length =       0,
		  .Device_Bscan_Length    =       0,
		  .Device_Fuses_Length    =       0
		},
		{  
		  .Device_Name    =               ALTERA_CPLD_EPM1270_DEVNAME,
		  .Device_ID      =               ALTERA_CPLD_EPM1270_IDCODE,
		  .Device_Column_Length   =       0,
		  .Device_Row_Length      =       0,
		  .Device_All_Bits_Length =       0,
		  .Device_Bscan_Length    =       0,
		  .Device_Fuses_Length    =       0
		},
		{  
		  .Device_Name    =               ALTERA_CPLD_EPM240_DEVNAME,
		  .Device_ID      =               ALTERA_CPLD_EPM240_IDCODE,
		  .Device_Column_Length   =       0,
		  .Device_Row_Length      =       0,
		  .Device_All_Bits_Length =       0,
		  .Device_Bscan_Length    =       0,
		  .Device_Fuses_Length    =       0
		},
		{  
		  .Device_Name    =               ALTERA_CPLD_EPM240Z_DEVNAME,
		  .Device_ID      =               ALTERA_CPLD_EPM240Z_IDCODE,
		  .Device_Column_Length   =       0,
		  .Device_Row_Length      =       0,
		  .Device_All_Bits_Length =       0,
		  .Device_Bscan_Length    =       0,
		  .Device_Fuses_Length    =       0
		},

		#endif

		#if 0
		{ //ALTERA_CPLD_MAX_V 
		  .Device_Name    =               ALTERA_CPLD_MAX_V_DEVNAME,
		  .Device_ID      =               ALTERA_CPLD_MAX_V_IDCODE,
		  .Device_Column_Length   =       0,
		  .Device_Row_Length      =       0,
		  .Device_All_Bits_Length =       0,
		  .Device_Bscan_Length    =       0,
		  .Device_Fuses_Length    =       0
		},    
		#endif
	};
	//----------------------------------------------------------------------------------    
	JTAG_device_information.Device_ID = 0xffffffff; //init var, device not found yet.
	for(i = 0 ; i < sizeof(Altera_information)/sizeof(JTAG_DEVICE_INFO);i++){
		if(Altera_information[i].Device_ID == Filter_Version(id_code)){
			memcpy((void*)&JTAG_device_information ,(void*)&Altera_information[i], sizeof(JTAG_DEVICE_INFO));
			break;
		}
		printk("[%d]Device_ID [%08X == %08X] !!\n", i, (int)Altera_information[i].Device_ID, (int)id_code );
	}

#ifdef CHECK_MANUFACTURER_ID_ONLY
	//Manufacturer
	if (JTAG_device_information.Device_ID == 0xffffffff){
		if(Manufacturer_Identity(Altera_information[0].Device_ID ) ==Manufacturer_Identity(id_code)){
			JTAG_device_information.Device_ID = id_code;
			memcpy((void*)&JTAG_device_information.Device_Name ,"ALTERA CPLD", sizeof(JTAG_device_information.Device_Name));
		}
	}
#endif //CHECK_MANUFACTURER_ID_ONLY
  
    return 0;
}


/*
 * get_device_idcode
 * Returns the JTAG hw device's IDCODE
 * The first 32bits in receive buffer is IDCODE after you send IDCODE command to device
 */
uint32_t get_device_idcode (unsigned long *id_code)
{
  	jtag_boundary_scan(0);
	//! Check the IDCODE
	SIR(SIR_BITS, IDCODE);
	SDR(32, 0,(void *)id_code);

	load_device_info (*id_code);
  
	if(JTAG_device_information.Device_ID == 0xffffffff) 
	{
		printk ("Unable to get device idcode.\n");
		return 1;
	}
	#ifdef DEBUG
	printk ("Device IDCODE = %08x ---> %s\n", JTAG_device_information.Device_ID, JTAG_device_information.Device_Name);
	#endif /* DEBUG */  

	return 0;
}
