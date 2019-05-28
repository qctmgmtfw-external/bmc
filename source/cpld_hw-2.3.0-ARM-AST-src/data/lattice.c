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
#include "lattice.h"

#define DEVICE_FAMILY_FUNC(func_id) context->func(func_id, 0, context)

//the define must below the jtag function include
#define SIR jtag_sir
#define SDR jtag_sdr
#define RUNTEST_IDLE   jtag_runtest_idle

int device_MACHOX(uint32_t cmd,unsigned int* buf, struct DEVICE_INFO * const);
int device_MACHOX2(uint32_t cmd,unsigned int* buf, struct DEVICE_INFO* const context);
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
  DeviceFamily_UesRead, //Quanta +UES
};

const struct DEVICE_INFO device_list[]={
	#ifdef CONFIG_SPX_FEATURE_LATTICE_LCMXO_SUPPORT
  {.idcode =LATTICE_CPLD_LCMXO256C_IDCODE, .name=LATTICE_CPLD_LCMXO256C_DEVNAME,.func = device_MACHOX, .support_loop_func = 0},
  #endif
  
  #ifdef CONFIG_SPX_FEATURE_LATTICE_LCMXO2_SUPPORT
  {.idcode =LATTICE_CPLD_LCMXO2_1200ZE_IDCODE, .name=LATTICE_CPLD_LCMXO2_1200ZE_DEVNAME,.func = device_MACHOX2, .support_loop_func = 1},
  {.idcode =LATTICE_CPLD_LCMXO2_640HC_IDCODE,  .name=LATTICE_CPLD_LCMXO2_640HC_DEVNAME, .func = device_MACHOX2, .support_loop_func = 1},
  #endif
  #if 0
  {.idcode =LATTICE_CPLD_LCMXO640C_IDCODE, .name=LATTICE_CPLD_LCMXO640C_DEVNAME,.func = device_MACHOX, .support_loop_func = 0},
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
	unsigned short	chksum = 0x0000;
	unsigned int leng = (bits_leng  + 31) / 32;
	int i;
	uint32_t ret = 0;
	
	if(compare_data == 0){
	// Calculate - the sum of all data (unit is byte)
		for (i = 0; i < leng; i++) {
			chksum += (read_buffer[i] >> 24) & 0xFF;
			chksum += (read_buffer[i] >> 16) & 0xFF;
			chksum += (read_buffer[i] >>  8) & 0xFF;
			chksum += (read_buffer[i]      ) & 0xFF;
		}
		printk ("Calculate Done.[%08x]\n",chksum);
		ret = chksum;
	}
	else{
		for (i = 0; i < leng; i++) {
			if (read_buffer[i] != compare_data[i]) {
				printk ("VERIFY ERROR!! ERROR at Index %08x\n", i);
				printk ("ERROR at row %08x\n", (unsigned int)((i*32)/(unsigned int)JTAG_device_information.Device_Column_Length ));
				printk ("The Original Value is %08x, Read Back Value is %08x\n", compare_data[i], read_buffer[i]);
				#ifdef DEBUG
					if(((i&~0x3) << 2) >= JTAG_device_information.Device_Column_Length /8){
						
					  printk ("Read back data, Index %08x:", (unsigned int )((i&~0x3) - JTAG_device_information.Device_Column_Length /8));
					  printk ("i %08x,len %08x.", (unsigned int )(i&~0x3) , (unsigned int)JTAG_device_information.Device_Column_Length /8);
        	  echo_arry((char *) &read_buffer[i&~0x3]- JTAG_device_information.Device_Column_Length /8 ,16*4);
        	  printk ("Source data:");
        	  echo_arry((char *) &compare_data[i&~0x3]- JTAG_device_information.Device_Column_Length /8 ,16*4);
					}
					else{
					  printk ("Read back data, Index from %08x:", (unsigned int )((i&~0x3)));
        	  echo_arry((char *) &read_buffer[i&~0x3] ,16*4);
        	  printk ("Source data:");
        	  echo_arry((char *) &compare_data[i&~0x3] ,16*4);
					}
				#endif
				ret = 1;
				break;
			}
		}
	}
	
	return ret;
}


/*
 * read_code_size_data
 */
void read_code_size_data (unsigned int *JTAG_data_buffer,unsigned int TDI)
{
	unsigned long index = 0;
	unsigned long row_count, col_count;	

	row_count = JTAG_device_information.Device_Row_Length;
	col_count = JTAG_device_information.Device_Column_Length;

	while (row_count > 0) {
		//TDI = 0x00000000 or 0xFFFFFFFF, SDR will set SDI 0x00 or 0xff when jtag Transmit .
		jtag_sdr(col_count, &TDI ,(&JTAG_data_buffer[index]));
		index += col_count / 32;
		
		if (row_count == 1)
			break;
		else
			row_count--;
			
		if (g_is_Support_LoopFunc == 1)
			RUNTEST_IDLE (2, 1);//XO2
		else
			RUNTEST_IDLE (5, 1);//XO
	}
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

#ifdef CONFIG_SPX_FEATURE_LATTICE_LCMXO_SUPPORT
/*
 * device_MACHOX
 */
int device_MACHOX(uint32_t cmd,unsigned int* buf, struct DEVICE_INFO* const context){
	uint32_t TDI __attribute__ ((unused)) =0, TDO __attribute__ ((unused)) =0;
  int ret = 0;
  int i = 0;
//  uint32_t Tdo_32bits;
  unsigned int Device_All_Rows=0;
  unsigned int Device_Using_Rows=0;
  //---------------------------------------------------------
  //---------------------------------------------------------
	switch(cmd){
		case DeviceFamily_Init:{
			g_program_retry_cnt = 0;
	    g_program_mode_cnt = 0;
	    }	        
	    break;
	    
		case DeviceFamily_CheckID:
      //----------------------------------------------------------------------
      //! Check the IDCODE
      SIR (8, IDCODE);
      SDR (32, 0, (void *) &TDO);
      //----------------------------------------------------------------------
      if(TDO != context->idcode){
      	printk("Error IdCode not match.[%08X / %08X]\n",TDO, context->idcode);
        ret = 1;
      }
      break;
		    
	  case DeviceFamily_Run_NewCode:{
	    // Transparent Field Reconfiguration (updata data from flash to sram)
	    ret = DEVICE_FAMILY_FUNC(DeviceFamily_CheckID);
      if (ret!=0) {                   
      	break;
      }
	    //! Shift in Preload(0x1C) instruction
      SIR(8, SAMPLE);
      jtag_dr_pause(1);
      
      //! Transparent Field Reconfiguration
      SIR(8, ISC_SRAM_ENABLE);RUNTEST_IDLE(5, 1);
      SIR(8, BULK_ERASE);RUNTEST_IDLE(5, 1);
      SIR(8, PROGRAM_ENABLE);RUNTEST_IDLE(5, 1);
      
      SIR(8, PROGRAM_DISABLE);RUNTEST_IDLE(5, 1);
      SIR(8, BYPASS);
      
      printk ("TFR Done\n");
      }
      break;
      
	  case DeviceFamily_CheckSum:{
      ret = DEVICE_FAMILY_FUNC(DeviceFamily_CheckID);
      if (ret!=0) {                   
      	break;
      }
      DEVICE_FAMILY_FUNC(DeviceFamily_ProgramMode_Enter);
	  Device_All_Rows=JTAG_device_information.Device_All_Bits_Length / JTAG_device_information.Device_Column_Length;
	  Device_Using_Rows = JTAG_device_information.Device_Row_Length;
      //-----------------------------------------------
      //! Shift in LSCC RESET ADDRESS(0x21) instruction
      SIR(8, LSCC_RESET_ADDRESS);
      RUNTEST_IDLE(5, 1);
      //! Shift in BYPASS(0xFF) instruction
      SIR(8, BYPASS);
      RUNTEST_IDLE(5, 1);
      //! Shift in LSCC READ INCR RTI(0x6A) instruction
      SIR(8, LSCC_READ_INCR_RTI);
      RUNTEST_IDLE(5, 1);

	  //All rows
	  JTAG_device_information.Device_Row_Length = Device_All_Rows;
	  read_code_size_data(get_jtag_read_buffer(),0xffffffff);
	  JTAG_device_information.Device_Row_Length = Device_Using_Rows;
      //-----------------------------------------------
      //! Shift in READ USERCODE(0x17) instruction
      SIR(8, UES_READ);
      RUNTEST_IDLE(5, 1);
      TDI =0xffffffff;
      SDR(32,&TDI,0);
      
      //! Shift in ISC DISABLE(0x1E) instruction
      SIR(8, PROGRAM_DISABLE);
      RUNTEST_IDLE(5, 50);
      SIR(8, BYPASS);
      RUNTEST_IDLE(5, 1);
      //-----------------------------------------------
      //get checkSum                
      *buf = device_data_check(get_jtag_read_buffer(), 0, JTAG_device_information.Device_All_Bits_Length);
      DEVICE_FAMILY_FUNC(DeviceFamily_Init);
      }
		  break;
		  
//Quanta----
	  case DeviceFamily_UesRead:{
			ret = DEVICE_FAMILY_FUNC(DeviceFamily_CheckID);
			if (ret!=0) {                   
			  break;
			  }
			DEVICE_FAMILY_FUNC(DeviceFamily_ProgramMode_Enter);
			//-----------------------------------------------
			//! Shift in READ USERCODE(0x17) instruction
			SIR(8, UES_READ);
			RUNTEST_IDLE(5, 1);
			mdelay(1000);//The 1000ms is a experimental result
			//-----------------------------------------------
			JTAG_device_information.Device_Row_Length = 1;
			JTAG_device_information.Device_Column_Length = 32;
			read_code_size_data(get_jtag_read_buffer(),0xffffffff);
			//-----------------------------------------------
	        *buf=*(get_jtag_read_buffer());
		    //----------------------------------------------------------------------
			//! Shift in ISC DISABLE(0x1E) instruction
			SIR(8, PROGRAM_DISABLE);
			RUNTEST_IDLE(5, 50);
			SIR(8, BYPASS);
			RUNTEST_IDLE(5, 1);
			//----------------------------------------------------------------------
			DEVICE_FAMILY_FUNC(DeviceFamily_Init);
			}
	  	  break;
//---Quanta		  
		case DeviceFamily_Erase:{
      printk ("Starting to Erase Device . . .(this will take a few seconds)\n");
      //-----------------------------------------------
      //! Shift in ISC ERASE(0x03) instruction
      SIR(8, BULK_ERASE);
      RUNTEST_IDLE(5,1);

      //LOOP_RUNTEST_IDLE
      /*TDI = 0; TDO = 1;
      for (i = 0; i<100; i++)
      {
      	RUNTEST_IDLE(5,100);
      	SDR(1, (void*)&TDI,(void*)&Tdo_32bits);
      	if ((Tdo_32bits ) == TDO)
      		break;
      }*/
      //After applying the SDR of above loop, CPLD can hardly be programmed. The retry issue occurs.
      //Here uses the reference code from ASPEED.
      mdelay(1000);//The 1000ms is a experimental result
      SIR(8, DISCHARGE);
      RUNTEST_IDLE(5,1);
      //-----------------------------------------------     
      printk ("Erase Done\n");
      }		    
		  break;
		case DeviceFamily_Verify:
      printk ("Starting to Verify Device . . . (this will take a few seconds)\n");
	  Device_All_Rows=JTAG_device_information.Device_All_Bits_Length / JTAG_device_information.Device_Column_Length;
	  Device_Using_Rows = JTAG_device_information.Device_Row_Length;
	  clean_unused_buffer((char *) buf, 0xffffffff, Device_All_Rows, Device_Using_Rows);	  
      //-----------------------------------------------
      //! Shift in LSCC RESET ADDRESS(0x21) instruction
      SIR(8, PLD_INIT_ADDR_FOR_PROG);
      RUNTEST_IDLE(5, 1);
      //! Shift in BYPASS(0xFF) instruction
      SIR(8, BYPASS);
      RUNTEST_IDLE(5, 1);
      //! Shift in LSCC READ INCR RTI(0x6A) instruction
      SIR(8, LSCC_READ_INCR_RTI);
      RUNTEST_IDLE(5, 1);

      //loop body
	  //Verify All rows
	  JTAG_device_information.Device_Row_Length = Device_All_Rows;
	  read_code_size_data(get_jtag_read_buffer(),0xffffffff);
	  JTAG_device_information.Device_Row_Length = Device_Using_Rows;

      //! Shift in READ USERCODE(0x17) instruction
      SIR(8, UES_READ);
      RUNTEST_IDLE(5, 1);
      TDI =0xffffffff;
      SDR(32,&TDI,0);

      // Compare        	
      ret = device_data_check(get_jtag_read_buffer(), buf, JTAG_device_information.Device_All_Bits_Length);
      //-----------------------------------------------

      if(ret == 0) printk ("Verify Done\n");
		  break;
		  
		case DeviceFamily_Program:
		  if(g_program_mode_cnt == 0){
        ast_jtag_reset();
            
        RUNTEST_IDLE(15,1);
        ret = DEVICE_FAMILY_FUNC(DeviceFamily_CheckID);
        if (ret != 0) break;
		  }
		  
		  DEVICE_FAMILY_FUNC(DeviceFamily_ProgramMode_Enter);
      
      if ( DEVICE_FAMILY_FUNC(DeviceFamily_Erase) != 0 ) {
      	printk ("ERASE CHIP FAILED!! Please Check if the JTAG Device is in the support list and the JTAG connector\n");
        ret = 1; 
        g_program_mode_cnt  = 1; 
        DEVICE_FAMILY_FUNC(DeviceFamily_ProgramMode_Exit);
        break;
      }
      
      //! Shift in READ STATUS(0xB2) instruction
      SIR(8, READ_STATUS);
      RUNTEST_IDLE(5,1);
      TDI = 0, TDO=0;
      SDR(1,&TDI,&TDO);
      
      printk ("Starting to Program Device . . . (this will take a few seconds)\n");
      //-----------------------------------------------
      //! Shift in INIT ADDRESS(0x21) instruction
      SIR(8, PLD_INIT_ADDR_FOR_PROG); 
      RUNTEST_IDLE(5,1);
      //! Shift in BYPASS(0xFF) instruction
      SIR(8, BYPASS); 
      RUNTEST_IDLE(5,1);

      {
        unsigned long index = 0;
        unsigned long row_count, col_count;
        
        // initial all variables
        row_count = JTAG_device_information.Device_Row_Length;
        col_count = JTAG_device_information.Device_Column_Length;
        
        while (row_count > 0) {
        	//! Shift in DATA SHIFT(0x02) instruction
          SIR(8, ISC_DATA_SHIFT);
          SDR(col_count, (&buf[index]), 0);
          index += col_count / 32;
           //! Shift in LSCC PROGRAM INCR RTI(0x67) instruction
          SIR(8, LSCC_PROG_INCR_RTI);///update to flash/rom.
          RUNTEST_IDLE(5, 1);
          //-------------------------------------------------------------------------------
          //enter dr loop but no bits out.
          for (i = 0; i<10; i++){
          	jtag_dr_pause(1);
          }
          //-------------------------------------------------------------------------------
          row_count--;
        }
      }
      //-----------------------------------------------      
      //! Shift in INIT ADDRESS(0x21) instruction
      SIR(8, LSCC_RESET_ADDRESS);
      RUNTEST_IDLE(5,1);
      //! Shift in BYPASS(0xFF) instruction
      SIR(8, BYPASS);
      RUNTEST_IDLE(5,1);
      //-----------------------------------------------
      //! Program USERCODE
      //! Shift in READ USERCODE(0x17) instruction
      SIR(8, UES_READ);
      TDI = get_ProgramUsercode();
      SDR(32, &TDI, 0);
      //! Shift in ISC PROGRAM USERCODE(0x1A) instruction
      SIR(8, UES_PROGRAM);
      RUNTEST_IDLE(5, 10);
      //! Read the status bit
      //! Shift in READ STATUS(0xB2) instruction
      SIR(8, READ_STATUS);
      RUNTEST_IDLE(5, 1);
      TDI = 0; TDO = 0;
      SDR(1,&TDI,&TDO);
      //-----------------------------------------------
      
      ret = context->func(DeviceFamily_Verify, buf, context);

      if(ret != 0){
      	if (g_program_retry_cnt < JTAG_PROGRAM_RETRY_MAX){
         	g_program_retry_cnt ++;
          printk("\n retry...[%d / %d]\n",g_program_retry_cnt , JTAG_PROGRAM_RETRY_MAX);
          
          ret = context->func(DeviceFamily_Program, buf, context);
        }
        else{
        	printk("Error Program fail.\n");
        }
      }
      
    	TDI=0,TDO=1;
      //-----------------------------------------------
      //! Shift in ISC PROGRAM DONE(0x2F) instruction
      SIR(8, PROGRAM_DONE);
      RUNTEST_IDLE(5, 50);
      SDR(1,&TDI,&TDO);
      //! Shift in BYPASS(0xFF) instruction
      SIR(8, BYPASS);

      DEVICE_FAMILY_FUNC(DeviceFamily_ProgramMode_Exit);
      if(g_program_mode_cnt == 0){
      	printk ("Program Done\n");
      }
		  break;
		  
		case DeviceFamily_ProgramMode_Enter:
			if(0 == g_program_mode_cnt ){
				printk("_only_BackgroundProgramMode_\n");
        //-----------------------------------------------
        //XPROGRAM mode
        //! Shift in XPROGRAM ENABLE(0x35) instruction
        SIR(8, XPROGRAM_ENABLE);
        RUNTEST_IDLE(5, 1);
        //! Shift in BYPASS(0xFF) instruction
        SIR(8, BYPASS);
        RUNTEST_IDLE(5, 1);
        //-----------------------------------------------
        g_program_mode_cnt++;
		  }
		  else{
		  	g_program_mode_cnt++;
		  }
		  break;
		  
    case DeviceFamily_ProgramMode_Exit:
    	//printk("p_cnt[%d]\n",g_program_mode_cnt);
    	if(1 == g_program_mode_cnt){
    		       
        //! Shift in READ STATUS(0xB2) instruction
        SIR(8, READ_STATUS);
        RUNTEST_IDLE(5,1);
        TDI = 0, TDO=0;
        SDR(1,&TDI,&TDO);
        
        //! Shift in ISC DISABLE(0x1E) instruction
        SIR(8, PROGRAM_DISABLE);
        RUNTEST_IDLE(5,50);
        //! Shift in BYPASS(0xFF) instruction
        SIR(8, BYPASS);
        RUNTEST_IDLE(5,1);
        //-----------------------------------------------
        g_program_mode_cnt--;
        }
        else{
        	g_program_mode_cnt--;
        }
        break;
	}

	return ret;
}
#endif

#ifdef CONFIG_SPX_FEATURE_LATTICE_LCMXO2_SUPPORT
/*
 * device_MACHOX2
 */
int device_MACHOX2(uint32_t cmd,unsigned int* buf, struct DEVICE_INFO* const context){
	uint32_t TDI __attribute__ ((unused)) =0,TDO __attribute__ ((unused)) =0;
  int ret = 0;
  int i = 0;
  uint32_t Tdo_32bits;  
  unsigned int Device_All_Rows=0;
  unsigned int Device_Using_Rows=0;
  //---------------------------------------------------------

	switch(cmd){
		
		case DeviceFamily_Init:{
			g_program_retry_cnt = 0;
	    g_program_mode_cnt = 0;
	    }	        
	    break;
	    
		case DeviceFamily_CheckID:{
      //----------------------------------------------------------------------
      //! Shift in IDCODE_PUB(0xE0) instruction
      SIR(8, IDCODE_PUB);
      TDI = 0;
      SDR(32, &TDI,&TDO);
      //----------------------------------------------------------------------
      if(TDO != context->idcode){
      	printk("Error IdCode not match.[%08X / %08X]\n",TDO, context->idcode);
        ret = 1;
        }
      }
		  break;
		  
    case DeviceFamily_Run_NewCode:{
    	// Transparent Field Reconfiguration (updata data from flash to sram)
	    ret = DEVICE_FAMILY_FUNC(DeviceFamily_CheckID);
      if (ret!=0) {                   
      	break;
      	}
      printk ("CPLD reload not support ...\n");
	    }
      break;
      
	  case DeviceFamily_CheckSum:{
	  	ret = DEVICE_FAMILY_FUNC(DeviceFamily_CheckID);
      if (ret!=0) {                   
      	break;
      }
      DEVICE_FAMILY_FUNC(DeviceFamily_ProgramMode_Enter);
	  Device_All_Rows=JTAG_device_information.Device_All_Bits_Length / JTAG_device_information.Device_Column_Length;
	  Device_Using_Rows = JTAG_device_information.Device_Row_Length;
      //-----------------------------------------------
      //! Shift in LSC_INIT_ADDRESS(0x46) instruction
      SIR(8, LSC_INIT_ADDRESS);
      TDI = 0x4;TDO = 0;
      SDR(8, &TDI,0);
      RUNTEST_IDLE(2,1);

      SIR(8, LSC_READ_INCR_NV);
      RUNTEST_IDLE(2,1);
      
	  //All rows
	  JTAG_device_information.Device_Row_Length = Device_All_Rows;
	  read_code_size_data(get_jtag_read_buffer(),0x00000000);
	  JTAG_device_information.Device_Row_Length = Device_Using_Rows;
      //-----------------------------------------------
      //! Shift in LSC_INIT_ADDRESS(0x46) instruction
      SIR	(8, LSC_INIT_ADDRESS);
      TDI = 0x02;
      SDR	(8,	&TDI, 0);
      RUNTEST_IDLE(2, 10);
      //! Shift in LSC_READ_FEATURE (0xE7) instruction
      SIR(8, LSC_READ_FEATURE);
      RUNTEST_IDLE(2, 1);
      TDI = 0xFFFFFFFF;
      SDR(64, &TDI, 0);
      //! Shift in in LSC_READ_FEABITS(0xFB) instruction                
      SIR(8, LSC_READ_FEABITS);
      RUNTEST_IDLE(2, 1);
      TDI= 0xffff;
      SDR(16,	&TDI, 0);
      
      //! Shift in READ USERCODE(0xC0) instruction                
      SIR(8, USERCODE); RUNTEST_IDLE(2, 1);
      TDI = 0;
      SDR(32, &TDI, 0);                
      //! Shift in ISC DISABLE(0x26) instruction                
      SIR(8, ISC_DISABLE); RUNTEST_IDLE(2, 1000);
      //! Shift in BYPASS(0xFF) instruction                
      SIR(8, BYPASS); RUNTEST_IDLE(2, 100);
      //-----------------------------------------------
      *buf = device_data_check(get_jtag_read_buffer(), 0, JTAG_device_information.Device_All_Bits_Length);
      DEVICE_FAMILY_FUNC(DeviceFamily_Init);
      }

		  break;
		  
//Quanta---
	  case DeviceFamily_UesRead:{
			ret = DEVICE_FAMILY_FUNC(DeviceFamily_CheckID);
			if (ret!=0) {                   
			  break;
			  }
			DEVICE_FAMILY_FUNC(DeviceFamily_ProgramMode_Enter);
			//-----------------------------------------------
			//! Shift in READ USERCODE(0xC0) instruction
			SIR(8, USERCODE); 
			RUNTEST_IDLE(2, 1);
			mdelay(1000);//The 1000ms is a experimental result
			//-----------------------------------------------
			JTAG_device_information.Device_Row_Length = 1;
			JTAG_device_information.Device_Column_Length = 32;
			read_code_size_data(get_jtag_read_buffer(),0x00000000);
			//-----------------------------------------------
			//assign UES data to buf
			*buf=*(get_jtag_read_buffer());
			//----------------------------------------------------------------------
		    //! Shift in ISC DISABLE(0x26) instruction                
		    SIR(8, ISC_DISABLE); 
		    RUNTEST_IDLE(2, 1000);
		    //! Shift in BYPASS(0xFF) instruction                
		    SIR(8, BYPASS); 
		    RUNTEST_IDLE(2, 100);
			//----------------------------------------------------------------------
			DEVICE_FAMILY_FUNC(DeviceFamily_Init);
			}
		  break;
//---Quanta			  
		case DeviceFamily_Erase:{
			printk ("Starting to Erase Device . . .(this will take a few seconds)\n");
      //-----------------------------------------------
      //    Erase the Flash
      //! Shift in ISC ERASE(0x0E) instruction
      SIR(8, ISC_ERASE);
      //Operands bit of TDI
      //17 Erase Feature Row
      //18 Erase Configuration Flash
      //19 Erase UFM
      //TDI = 0xE; // Erase ALL
      TDI = 0x4; // Erase Configuration Flash
      SDR(8, &TDI, 0);
      RUNTEST_IDLE(2,0);

      SIR(8, LSC_CHECK_BUSY);
 
      //LOOP_RUNTEST_IDLE
      TDI = 0; TDO = 0;
      for (i = 0; i<350; i++)
      {
      	RUNTEST_IDLE(2,10);
      	SDR(1, (void*)&TDI,(void*)&Tdo_32bits);
      	if ((Tdo_32bits ) == TDO) break;
      }
                        
      SIR(8, LSC_READ_STATUS);
      RUNTEST_IDLE(2,1); 
      TDI = 0;
      SDR(32, &TDI,&TDO);

      if((TDO& 0x00003000) != 0) printk("Check Erase status fail![%x]\n",(TDO& 0x00003000));
      //-----------------------------------------------                
      printk ("Erase Done\n");
      }		    
		  break;
		  
		case DeviceFamily_Verify:
      printk ("Starting to Verify Device . . . (this will take a few seconds)\n");
	  Device_All_Rows=JTAG_device_information.Device_All_Bits_Length / JTAG_device_information.Device_Column_Length;
	  Device_Using_Rows = JTAG_device_information.Device_Row_Length;
	  clean_unused_buffer((char *) buf, 0x00000000, Device_All_Rows, Device_Using_Rows);
      //-----------------------------------------------
      SIR(8, LSC_INIT_ADDRESS);
      TDI = 0x4;TDO = 0;
      SDR(8, &TDI,0);
      RUNTEST_IDLE(2,1);

      SIR(8, LSC_READ_INCR_NV);
      RUNTEST_IDLE(2,1);
	  //Verify All rows
	  JTAG_device_information.Device_Row_Length = Device_All_Rows;
	  read_code_size_data(get_jtag_read_buffer(),0x00000000);
	  JTAG_device_information.Device_Row_Length = Device_Using_Rows;
      //-----------------------------------------------
      //! Shift in LSC_INIT_ADDRESS(0x46) instruction
      SIR	(8, LSC_INIT_ADDRESS);
      TDI = 0x02;
      SDR	(8,	&TDI, 0);
      RUNTEST_IDLE(2, 10);
      //! Shift in LSC_READ_FEATURE (0xE7) instruction
      SIR(8, LSC_READ_FEATURE);
      RUNTEST_IDLE(2, 1);
      TDI = 0xFFFFFFFF;
      SDR(64, &TDI, 0);
      //! Shift in in LSC_READ_FEABITS(0xFB) instruction
      SIR(8, LSC_READ_FEABITS);
      RUNTEST_IDLE(2, 1);
      TDI= 0xffff;
      SDR(16,	&TDI, 0);
      
      //! Shift in READ USERCODE(0xC0) instruction
      SIR(8, USERCODE); RUNTEST_IDLE(2, 1);
      TDI = 0;
      SDR(32, &TDI, 0);
      
      //-----------------------------------------------                
      // Compare
      ret = device_data_check(get_jtag_read_buffer(),  buf, JTAG_device_information.Device_All_Bits_Length);
      if(ret == 0) printk ("Verify Done\n");
		  break;
		  
		case DeviceFamily_Program:
		  if(g_program_mode_cnt == 0){
		  	ast_jtag_reset();
             
        ret = DEVICE_FAMILY_FUNC(DeviceFamily_CheckID);
        if (ret != 0) break;
		  }

		  DEVICE_FAMILY_FUNC(DeviceFamily_ProgramMode_Enter);

      if ( DEVICE_FAMILY_FUNC(DeviceFamily_Erase) != 0 ) {
      	printk ("ERASE CHIP FAILED!! Please Check if the JTAG Device is in the support list and the JTAG connector\n");
        ret = 1; 
        g_program_mode_cnt  = 1; 
        DEVICE_FAMILY_FUNC(DeviceFamily_ProgramMode_Exit);
        break;
      }
      printk ("Starting to Program Device . . . (this will take a few seconds)\n");
      //===============================================
      //    Program CFG
      //! Shift in LSC_INIT_ADDRESS(0x46) instruction 
      SIR(8, LSC_INIT_ADDRESS);
      TDI = 0x04;
      SDR(8, &TDI, 0);
      RUNTEST_IDLE(2,10);

      {
      	uint32_t Row  = 1;
        unsigned long index = 0;
        unsigned long row_count, col_count;
        // initial all variables
        row_count = JTAG_device_information.Device_Row_Length;
        col_count = JTAG_device_information.Device_Column_Length;

        while (row_count > 0) {
        	//-------------------------------------------------------------------------------
          //Jtag only, check the row is not empty, when empty data, reset address.
          #ifdef JTAG_PROGRAM_DEBUG_NOT_SVF
          	if( (buf[index]== 0) && (buf[index+1]== 0)&& (buf[index+2]== 0)&& (buf[index+3]== 0)){///need check,
          		#ifdef JTAG_PROGRAM_DEBUG
          	  printk("! Shift in LSC_WRITE_ADDRESS(0xB4) instruction\n");
          	  #endif /* JTAG_PROGRAM_DEBUG */
          	  SIR(8, LSC_WRITE_ADDRESS);
          	  TDI = Row;
          	  SDR(32, &TDI, 0);
          	  RUNTEST_IDLE(5,10);
          	}
          	else
          #endif
          {
          	//-------------------------------------------------------------------------------
          	//! Shift in LSC_PROG_INCR_NV(0x70) instruction
          	SIR(8, LSC_PROG_INCR_NV);
          	
          	#ifdef JTAG_PROGRAM_DEBUG
          		printk("! Shift in Data Row = %d\n",(unsigned int) Row);
          	#endif /* JTAG_PROGRAM_DEBUG */
          	SDR(col_count, (void *)(&(buf[index])),(void*)0);
          	RUNTEST_IDLE(2,0);
          	//-------------------------------------------------------------------------------
          	//! Shift in LSC_CHECK_BUSY(0xF0) instruction
          	SIR(8, LSC_CHECK_BUSY);
            //LOOP_RUNTEST_IDLE
            TDI = 0; TDO = 0;
            for (i = 0; i<10; i++)
            {
            	RUNTEST_IDLE(0,1);
            	SDR(1, (void*)&TDI,(void*)&Tdo_32bits);
            	if ((Tdo_32bits ) == TDO) break;
            }
          	//-------------------------------------------------------------------------------
          }
          index += col_count / 32;
          Row++;
          row_count--;
        }
      }
      //===============================================
      ///Program USERCODE
      //! Shift in READ USERCODE(0xC0) instruction
      #ifdef JTAG_PROGRAM_DEBUG
      	printk("! Shift in READ USERCODE(0xC0) instruction\n");
      #endif /* JTAG_PROGRAM_DEBUG */
      SIR(8, USERCODE);
      TDI = get_ProgramUsercode();
      SDR(32, &TDI, 0);
      #ifdef JTAG_PROGRAM_DEBUG
      	printk("! Shift in ISC PROGRAM USERCODE(0xC2) instruction\n");
      #endif /* JTAG_PROGRAM_DEBUG */
      SIR(8, ISC_PROGRAM_USERCODE);
      RUNTEST_IDLE(2, 10);
           
      //  Read the status bit
      //! Shift in LSC_READ_STATUS(0x3C) instruction
      #ifdef JTAG_PROGRAM_DEBUG
      	printk("! Shift in LSC_READ_STATUS(0x3C) instruction\n");
      #endif /* JTAG_PROGRAM_DEBUG */
      SIR(8, LSC_READ_STATUS);
      RUNTEST_IDLE(2,1);
      TDI = 0;
      SDR(32, &TDI,&TDO);

      if((TDO& 0x00003000) != 0) printk("Program USERCODE status fail![%x]\n",(TDO& 0x00003000));
      
      //Verify
      ret = context->func(DeviceFamily_Verify, buf, context);
            
      if(ret != 0){
      	if (g_program_retry_cnt < JTAG_PROGRAM_RETRY_MAX){
      		g_program_retry_cnt ++;
         printk("\n retry...[%d / %d]\n",g_program_retry_cnt , JTAG_PROGRAM_RETRY_MAX);
         
         ret = context->func(DeviceFamily_Program, buf, context);
         }
       else{
       	printk("Error Program fail.\n");
         ret = 1;

         #ifdef JTAG_ERASE_WHEN_PROGRAM_FAIL
         	if ( DEVICE_FAMILY_FUNC(DeviceFamily_Erase) != 0 ) {
         		printk ("ERASE CHIP FAILED!! Please Check if the JTAG Device is in the support list and the JTAG connector\n");                        
         	  g_program_mode_cnt  = 1; 
         	  DEVICE_FAMILY_FUNC(DeviceFamily_ProgramMode_Exit);
         	  break;
         	 }
         #endif /* JTAG_ERASE_WHEN_PROGRAM_FAIL */
         }
      }
            
      if(g_program_mode_cnt == 1){//close when exit program mode.
      	//! Program Feature Rows //Don't Program it!! by WEIKENG.
        //  ...
        //    Program DONE bit
        //! Shift in ISC PROGRAM DONE(0x5E) instruction
        SIR(8, ISC_PROGRAM_DONE);
        RUNTEST_IDLE(2,0);
        
        //! Shift in LSC_CHECK_BUSY(0xF0) instruction
        SIR(8, LSC_CHECK_BUSY);
        //LOOP_RUNTEST_IDLE
        TDI = 0; TDO = 0;
        for (i = 0; i<10; i++)
        {
        	RUNTEST_IDLE(0,1);
        	SDR(1, (void*)&TDI,(void*)&Tdo_32bits);
        	if ((Tdo_32bits ) == TDO) break;
        }

        //! Shift in BYPASS(0xFF) instruction
        SIR(8, BYPASS);
      }
      DEVICE_FAMILY_FUNC(DeviceFamily_ProgramMode_Exit);
      if(g_program_mode_cnt == 0){
      	printk ("Program Done\n");
      }
		  break;
		   
		case DeviceFamily_ProgramMode_Enter:
			if(0 == g_program_mode_cnt ){
				printk("_only_BackgroundProgramMode_\n");
        //-----------------------------------------------
        //!!XPROGRAM mode
        //! Shift in LSC_ENABLE_X(0x74) instruction
        
        SIR(8, ISC_ENABLE_X); //<<>>> 0x74 or 0xc6 //SIR(8, ISC_ENABLE); //normal mode program.
        TDI = 0x08; 
        SDR(8, &TDI, 0); 
        RUNTEST_IDLE(2,10);
        
        //!!Check the Key Protection fuses
        //! Shift in LSC_READ_STATUS(0x3C) instruction
        SIR(8, LSC_READ_STATUS);
        RUNTEST_IDLE(2,1);
        TDI = 0;
        SDR(32, &TDI,&TDO);
        if((TDO& 0x00024040) != 0) printk("Check the Key Protection fuses1 fail![%x]\n",(TDO& 0x00024040));
        
        //! Shift in LSC_READ_STATUS(0x3C) instruction
        SIR(8, LSC_READ_STATUS);
        RUNTEST_IDLE(2,1);
        TDI = 0;
        SDR(32, &TDI,&TDO);
        if((TDO& 0x00010000) != 0) printk("Check the Key Protection fuses2 fail![%x]\n",(TDO& 0x00010000));
        //-----------------------------------------------
        g_program_mode_cnt++;
		  }
		  else{
		  	g_program_mode_cnt++;
		  }
		  break;
		  
		case DeviceFamily_ProgramMode_Exit:
			if(1 == g_program_mode_cnt){                
				//-----------------------------------------------
				//! Shift in ISC DISABLE(0x26) instruction
        SIR(8, ISC_DISABLE);
        RUNTEST_IDLE(2,1000);
        //! Shift in BYPASS(0xFF) instruction
        SIR(8, BYPASS);
        RUNTEST_IDLE(2,100);
        //-----------------------------------------------
        g_program_mode_cnt--;
        }
		  else{
		  	g_program_mode_cnt--;
		    }
		  break;
		    
    default:
    	printk("Not support!!\n");
    	ret = 1;
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

  for(i = 0; i<sizeof(device_list)/sizeof(device_list[0]); i++){
  	pDevice =(struct DEVICE_INFO *)&device_list[i];
    if(pDevice == NULL)
    {
    	printk("Error : Dereferencing Null Pointer File:%s Line: %d  Function : %s\n",__FILE__, __LINE__, __func__);
        return 1;
    }

    if(pDevice->idcode == 0){
    	printk("The device not in supportlist\n");
      return -1;
    }
    if(pDevice->idcode == JTAG_device_information.Device_ID){
      g_is_Support_LoopFunc = pDevice->support_loop_func;
      break;
    }
  }
  
  switch(cmd){
  	case IOCTL_JTAG_ERASE_DEVICE:
  		pDevice->func(DeviceFamily_Init,0,pDevice);    
    	ret = pDevice->func(DeviceFamily_Erase, 0, pDevice);
      break;
      
    case IOCTL_JTAG_DEVICE_CHECKSUM:
    	ret = pDevice->func(DeviceFamily_CheckSum, buf, pDevice);
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
        
//Quanta---
    case IOCTL_JTAG_UES_READ:
    	ret = pDevice->func(DeviceFamily_UesRead, buf, pDevice);
      break;
//---Quanta
    default:
    	ret = 1;
    }
    //if(ret == 0); else printk("Fail.\n"); return ret;
    if(ret != 0) printk("Action fail.\n");    

    return ret;
}


/*
 * load_device_info
 */
uint32_t load_device_info(unsigned long id_code){
	int i;
	//----------------------------------------------------------------------------------
  const JTAG_DEVICE_INFO MachXO_information[]=
  {
    #ifdef CONFIG_SPX_FEATURE_LATTICE_LCMXO_SUPPORT
    { //LATTICE_CPLD_LCMXO256C_IDCODE: 
      .Device_Name    =               LATTICE_CPLD_LCMXO256C_DEVNAME,
      .Device_ID      =               LATTICE_CPLD_LCMXO256C_IDCODE,
      .Device_Column_Length   =       LATTICE_COLS_LCMXO256C_LENGTH,
      .Device_Row_Length      =       LATTICE_ROWS_LCMXO256C_LENGTH,
      .Device_All_Bits_Length =       LATTICE_BITS_LCMXO256C_LENGTH,
      .Device_Bscan_Length    =       LATTICE_BSCAN_LCMXO256C_LENGTH,
      .Device_Fuses_Length    =       LATTICE_FUSES_LCMXO256C_LENGTH
    },
    #endif
    
    #ifdef CONFIG_SPX_FEATURE_LATTICE_LCMXO2_SUPPORT
    { //LATTICE_CPLD_LCMXO2_1200ZE_DEVNAME: 
      .Device_Name    =               LATTICE_CPLD_LCMXO2_1200ZE_DEVNAME,
      .Device_ID      =               LATTICE_CPLD_LCMXO2_1200ZE_IDCODE,
      .Device_Column_Length   =       LATTICE_COLS_LCMXO2_1200ZE_LENGTH,
      .Device_Row_Length      =       LATTICE_ROWS_LCMXO2_1200ZE_LENGTH,
      .Device_All_Bits_Length =       LATTICE_BITS_LCMXO2_1200ZE_LENGTH,
      .Device_Bscan_Length    =       LATTICE_BSCAN_LCMXO2_1200ZE_LENGTH,
      .Device_Fuses_Length    =       LATTICE_FUSES_LCMXO2_1200ZE_LENGTH
    },
    { //LATTICE_CPLD_LCMXO2_640HC_DEVNAME: 
      .Device_Name    =               LATTICE_CPLD_LCMXO2_640HC_DEVNAME,
      .Device_ID      =               LATTICE_CPLD_LCMXO2_640HC_IDCODE,
      .Device_Column_Length   =       LATTICE_COLS_LCMXO2_640HC_LENGTH,
      .Device_Row_Length      =       LATTICE_ROWS_LCMXO2_640HC_LENGTH,
      .Device_All_Bits_Length =       LATTICE_BITS_LCMXO2_640HC_LENGTH,
      .Device_Bscan_Length    =       LATTICE_BSCAN_LCMXO2_640HC_LENGTH,
      .Device_Fuses_Length    =       LATTICE_FUSES_LCMXO2_640HC_LENGTH
    },    
    #endif
    #if 0
  	{ //LATTICE_CPLD_LCMXO640C_IDCODE: need check
  		.Device_Name    =               LATTICE_CPLD_LCMXO640C_DEVNAME,
      .Device_ID      =               LATTICE_CPLD_LCMXO640C_IDCODE,
      .Device_Column_Length   =       LATTICE_COLS_LCMXO640C_LENGTH,
      .Device_Row_Length      =       LATTICE_ROWS_LCMXO640C_LENGTH,
      .Device_All_Bits_Length =       LATTICE_BITS_LCMXO640C_LENGTH,
      .Device_Bscan_Length    =       LATTICE_BSCAN_LCMXO640C_LENGTH,
      .Device_Fuses_Length    =       LATTICE_FUSES_LCMXO640C_LENGTH
    },    
    #endif
  };
  //----------------------------------------------------------------------------------    
  JTAG_device_information.Device_ID = 0xffffffff; //init var, device not found yet.
  for(i = 0 ; i < sizeof(MachXO_information)/sizeof(JTAG_DEVICE_INFO);i++){
  	if(MachXO_information[i].Device_ID == id_code){
  		memcpy((void*)&JTAG_device_information ,(void*)&MachXO_information[i], sizeof(JTAG_DEVICE_INFO));
  		break;
      }
      //printk("[%i]Device_ID [%08X == %08X] !!\n", i, (int)MachXO_information[i].Device_ID, (int)*id_code );
  }
	return 0;
}


/*
 * get_device_idcode
 * Returns the JTAG hw device's IDCODE
 * The first 32bits in receive buffer is IDCODE after you send IDCODE command to device
 */
uint32_t get_device_idcode (unsigned long *id_code)
{
  //! Check the IDCODE
  SIR(8, IDCODE);
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
