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

#include "jtag.h"
#include "jtag_ioctl.h"


#define assert(test,...) do{if(!(test)) printk(__VA_ARGS__);}while(0)
//--------------------------------------------------
// Change the string to hex.
//--------------------------------------------------
#define BITS_FSM_NONE     0
#define BITS_FSM_READ     1
#define BITS_FSM_RESET    2
#define BITS_FSM_BITS     3
#define BITS_FSM_PUSH_HighBIT '1'
#define BITS_FSM_PUSH_ZeroBIT '0'
#define BITS_FSM_END_MARK '*'

#define FILE_STORE_BLOCK_SIZE (4096) //4K byte for one store block.
#define STX (0x02)
#define ETX (0x03)

extern unsigned int hw_device_ctl(unsigned int, unsigned long *, unsigned long);


struct jed_head{
	//--------------------
  //int load_mark;
  unsigned int QF;
  unsigned int CFG_data_bits;
  unsigned int checksum;
  unsigned int user_data;
  //--------------------
  char DeviceName[64];  
};
//-----------------------------------------------------
struct jed_head jed_info={0};

void init_jed_info(void){
  jed_info.QF = 0;
  jed_info.CFG_data_bits =0;
  jed_info.checksum = 0;
  jed_info.DeviceName[0]= '\0';
  jed_info.user_data = 0;
}


/*
 * get_ProgramUsercode 
 */
uint32_t get_ProgramUsercode(void){
    return jed_info.user_data;
}


/*
 * bitStr2hex_fsm 
 */
static unsigned int bitStr2hex_fsm(unsigned int cmd){
  static unsigned int data =0;
  static unsigned int bits =0;
  unsigned int ret = 0;

  switch(cmd){
  	
    case BITS_FSM_BITS:
    	ret = bits;
      break;
      
    case BITS_FSM_RESET:
      data = 0;
      bits = 0;
      break;
      
    case BITS_FSM_PUSH_HighBIT:
      if(bits < 32){
      	data |= 1 << bits;
        bits++;
      }
      break;      
      
    case BITS_FSM_PUSH_ZeroBIT:
      if(bits < 32){
      	bits++;
      }
      break;

    case BITS_FSM_READ:
      ret = data;
      break;
      
    case '\n':
    case '\r':
      break;
      
    default://'*', '\0' or others.
      ret = -1;
      break;
  }
  return ret;
}


/*
 * pick_bits 
 * return the get data size
 * This function not re-enterable.
 */
static int pick_bits(unsigned int hex_buf[],char* str,int range){
		
  //char *str = (char *)pt;
  int i  __attribute__ ((unused)) =0;
  int index=0;
  int ret=0;

  bitStr2hex_fsm( BITS_FSM_RESET );
  //read until char is end(* or '\0')
  while( bitStr2hex_fsm(str[0]) != -1 && range-- >0){
  	if(bitStr2hex_fsm( BITS_FSM_BITS ) == 32) {
  		hex_buf[index++] = bitStr2hex_fsm(BITS_FSM_READ);
      bitStr2hex_fsm(BITS_FSM_RESET);
      }
      str++;
  }
  ret=index << 2;
  if(bitStr2hex_fsm( BITS_FSM_BITS ) != 0) {
  	hex_buf[index++] = bitStr2hex_fsm(BITS_FSM_READ);
    ret += bitStr2hex_fsm( BITS_FSM_BITS ) >> 3;
    bitStr2hex_fsm(BITS_FSM_RESET);
  }
  return ret;
}


/*
 * check_jed_mark 
 * check the jed file
 * jed file start code = STX:: 0x02.
 * jed file end code = ETX:: 0x03.
 * store_block_size
 */
unsigned int check_jed_mark(const char* jed_str, unsigned long size){
	int i, loop_end;
  unsigned int ret = 0;
  //--------------------------------------------------------

  do{
  	if(jed_str[0] != STX) {
  		printk("Error!! can't found the STX\n");
      ret = 1;
      break;
    }
    
    loop_end = size - FILE_STORE_BLOCK_SIZE; 
    
    for (i=size-1;i>0 && i>loop_end;i--){
      if((unsigned int)jed_str[i] == ETX )
      {
      	//printk("Found the ETX.\n");
        break;
      }
    }
    if(i <= 0 || i <= loop_end){
    	printk("Error!! can't found the ETX\n");
      ret = 1;
      break;
    }

  } while(0);
  //------------------------------------------------------------
  //echo_arry(&jed_str[(i& ~0xff)],SHIFT_BACK_SIZE);
  return ret;
}


/*
 * jed_2_hex 
 */
int jed_2_hex(char* jed_str, unsigned long* size){
	
  int ret;
  int shift __attribute__ ((unused)) =0;
  unsigned int checksum;
  unsigned int QF;
  unsigned int cfg_data_size;
  unsigned int user_data;
  unsigned int checksum_count;
  int i __attribute__ ((unused)) ;
  char str_patten[64];
  char* jed_str_end=jed_str + *size ;
  char* pt_str;//point the string start.
	char *data_CFG = (char *)get_jtag_write_buffer();
  char *pt_s,*pt_e;

  ret = check_jed_mark(jed_str, *size);
  if(ret != 0) return ret;
  if(data_CFG == NULL){
  	return 1;
  }
  
	do{
  	//--------------------------------------------------------------------------------------
		i=0;
		//--------------------------------------------------------------------------------------
		// find patten [NOTE DEVICE NAME:].
		// and get string to last '-'.
		//--------------------------------------------------------------------------------------
		snprintf(str_patten,sizeof(str_patten),"\nNOTE DEVICE NAME:");
		pt_str = strstr(jed_str,str_patten);
		if(((unsigned int)pt_str <= 1 )|| ((unsigned int)pt_str > (unsigned int)jed_str_end)){
			printk("DEVICE NAME not found!!![%p].",pt_str );
		  break;
		}
		else{// find the last hyphen.
		  int str_len=0;
		  char tmp;
		  const int buf_size = sizeof(jed_info.DeviceName);
		  //------------------------------------------------
		  pt_s = pt_str +strlen(str_patten)+1;
		  pt_e = strchr( pt_s,'*');
		  str_len = pt_e - pt_s;
		  if (str_len >  buf_size){
		  	str_len = buf_size;
		  }
		  tmp = pt_s [str_len-1];
		  pt_s [str_len-1] = '\0';
		  pt_e = strrchr( pt_s,'-');
		  pt_s [str_len-1] =tmp;
		  str_len = pt_e - pt_s;
		  if (str_len > buf_size ){
		    str_len = buf_size;
		  }
		  strncpy(jed_info.DeviceName ,pt_s,str_len);
		  printk(" DeviceName:%s\n",jed_info.DeviceName);
		}
		//--------------------------------------------------------------------------------------
		// find patten [QF].
		// Get number of fuses in device.
		//--------------------------------------------------------------------------------------
		snprintf(str_patten,sizeof(str_patten),"\nQF");
		pt_str = strstr(jed_str,str_patten);
		
		if(((unsigned int)pt_str <= 1 )|| ((unsigned int)pt_str > (unsigned int)jed_str_end)){
			assert((unsigned int)pt_str < (unsigned int)jed_str_end,"error pt_str > jed_str_end!!\n%p > %p\n", pt_str, jed_str_end);
		  printk("QF not found!!!");
		  break;
		}
		else{
		  pt_s = pt_str +strlen(str_patten)+1;
		  pt_e = strchr( pt_s,'*');
		
		  pt_s = pt_str +strlen(str_patten);
		  sscanf((char *)pt_s,"%d*",&QF);
		  printk(" Number of fuses read = %d.\n",QF);
		  jed_info.QF = QF ;
		}
		
		//--------------------------------------------------------------------------------------
		// find patten [L00000].
		// Bit Data string start.
		//--------------------------------------------------------------------------------------
		// make PATTEN
		if(!strncmp(jed_info.DeviceName, "LCMXO2-7000HC", sizeof("LCMXO2-7000HC"))) {
			snprintf(str_patten,sizeof(str_patten),"\nL0000000");//The pattern is different with others
			printk("It's LCMXO2-7000HC, change pattern to L0000000\n");
		}
		else
			snprintf(str_patten,sizeof(str_patten),"\nL00000");//block
		// search PATTEN and get first position in jed_str. return 0 mean PATTEN not found in string.
		pt_str = strstr(jed_str,str_patten);
		
		if(((unsigned int)pt_str <= 1 )|| ((unsigned int)pt_str > (unsigned int)jed_str_end)){
		  printk("Data string address not found!!!");
		  ret = -1;
		  break;
		}
		else{
		  //For pick_bits function, we need modify point position.(Move to end of PATTEN +1, 1 for \r)
		  pt_s = pt_str +strlen(str_patten)+1;
		  shift = 0;
		  shift += pick_bits(((void *) data_CFG)+shift, pt_s,/*size*/(unsigned int)jed_str_end - (unsigned int)pt_s );
		  printk(" Data size = %d\n",shift );
		  jed_info.CFG_data_bits = shift*8;
		}
		//--------------------------------------------------------------------------------------
		// find patten [NOTE END CONFIG DATA].
		// Get config data size(bits).
		//--------------------------------------------------------------------------------------
		snprintf(str_patten,sizeof(str_patten),"END CONFIG DATA*");
		pt_str = strstr(jed_str,str_patten);
		
		if(((unsigned int)pt_str <= 1 )|| ((unsigned int)pt_str > (unsigned int)jed_str_end)){
		  assert((unsigned int)pt_str < (unsigned int)jed_str_end,"error pt_str > jed_str_end!!\n%p > %p\n",pt_str , jed_str_end);
		  printk("No CONFIG DATA.\n");
		  if( jed_info.CFG_data_bits != jed_info.QF){
		  	printk(" CFG_data_bits reading error.\n");
		    ret = -1;
		    break;
		  }
		  //printk("CONFIG DATA size not found!!!");
		}
		else{
			pt_s = pt_str +strlen(str_patten)+1;
		
		  sscanf((char *)pt_s,"\nL%06d*",&cfg_data_size);
		  printk(" Config data size read(bits) = %d.\n",cfg_data_size);
		  if( jed_info.CFG_data_bits != cfg_data_size){
		  	printk(" CFG_data_bits reading error.\n");
		    ret = -1;
		    break;
		  }
		}
		
		//--------------------------------------------------------------------------------------
		// find patten [C].
		// Get Checksum.
		//--------------------------------------------------------------------------------------
		snprintf(str_patten,sizeof(str_patten),"\nC");
		pt_str = strstr(jed_str,str_patten);
		
		if(((unsigned int)pt_str <= 1 )|| ((unsigned int)pt_str > (unsigned int)jed_str_end)){
			assert((unsigned int)pt_str < (unsigned int)jed_str_end,"error pt_str > jed_str_end!!\n%d > %d\n",(unsigned int)pt_str , (unsigned int)jed_str_end);
		  printk(" Checksum not found!!!");
		  ret = -1;
		  break;
		}
		else{
		  pt_s = pt_str +strlen(str_patten);
		  sscanf((char *)pt_s,"%04X*",&checksum);
		  printk(" Checksum read = %X.\n",checksum);
		  jed_info.checksum = checksum;
		}
		//--------------------------------------------------------------------------------------
		// find patten [UH].
		// Get User Data.
		//--------------------------------------------------------------------------------------
		snprintf(str_patten,sizeof(str_patten),"\nU");
		pt_str = strstr(jed_str,str_patten);
		
		if(((unsigned int)pt_str <= 1 )|| ((unsigned int)pt_str > (unsigned int)jed_str_end)){
		  assert((unsigned int)pt_str < (unsigned int)jed_str_end,"error pt_str > jed_str_end!!\n%p > %p\n", pt_str, jed_str_end);
		  ///printk("User Data not found!!!\n");
		  ret = -1;
		  break;
		}
		else{
		  pt_s = pt_str +strlen(str_patten);
		  //Hex or bits
		  if(*pt_s == 'H' ){
		  	pt_s++;
		    sscanf((char *)pt_s,"%08X*",&user_data);
		  }
		  else{
		    //shift = 0;
		    //shift += pick_bits(((void *) &user_data)+shift, pt_s,/*size*/32);
		    //printk(" UserData size = %d\n",shift );
        user_data = 0;
        for( i=0; i<32; i++ ){
          user_data <<=1;
          if(pt_s[i]=='1') user_data |=1;
        }
		  }
		
		  printk("User Data read = 0x%08X.\n",user_data);
		  jed_info.user_data = user_data;
		}
		//--------------------------------------------------------------------------------------
		// checksum_count
		//--------------------------------------------------------------------------------------
		{
			int i=0;
		  unsigned char* pt=(void*)data_CFG;
		  int len =jed_info.CFG_data_bits / 8;
		  checksum_count = 0;
		
		  for (i=0;i<len;i++){
		  	checksum_count +=*pt++;
		  }
		  checksum_count &= 0xffff;
		  printk(" Checksum count  = %X.\n", checksum_count);
		
		  if(checksum_count!=checksum){
		    printk(" CheckSum Error!!\n");
		    ret = -1;
		    break; //ret <0;
		  }
		}
	}while(0);
        
	if(data_CFG != (char *)get_jtag_write_buffer() ) kfree(data_CFG);

	return (ret < 0)?1:0;
}


/*
 * jed_program
 * Get the informaction from jed file, and check the informacation is enough or not.
 */
unsigned int jed_program(char* jed_str, unsigned long size){
	unsigned int ret = 0;
  unsigned int row_num = JTAG_device_information.Device_Row_Length;
   
  init_jed_info();

  ret = jed_2_hex(jed_str, &size);
  if(ret != 0){
  	printk("Error in Jed to Hex conver.\n");
    return ret;
  }
  //Jedec file QF not equal Device fuses.
  if(JTAG_device_information.Device_Fuses_Length != jed_info.QF){
    printk("Device Fuses Length not match with Jed file.\n");
    ret = 1;
    return ret;
  }
  
  do{
  	//Set device row if the fuse number is same.
		//check CFG_data_bits % col == 0
		if( (jed_info.CFG_data_bits % JTAG_device_information.Device_Column_Length) == 0){
			JTAG_device_information.Device_Row_Length = jed_info.CFG_data_bits / JTAG_device_information.Device_Column_Length;
		}
		else {
			ret = 1;
			break;
		}
		//program it!! device function.
		ret = hw_device_ctl(IOCTL_JTAG_PROGRAM_DEVICE,(void*)get_jtag_write_buffer(), jed_info.CFG_data_bits);
		
  }while(0);
  //set device row back;
  JTAG_device_information.Device_Row_Length = row_num;
  
  return ret;
}
