/****************************************************************************/
/*																			*/
/*	Module:			jbistub.c												*/
/*																			*/
/*					Copyright (C) Altera Corporation 1997-2001				*/
/*																			*/
/*	Description:	Jam STAPL ByteCode Player main source file				*/
/*																			*/
/*					Supports Altera ByteBlaster hardware download cable		*/
/*					on Windows 95 and Windows NT operating systems.			*/
/*					(A device driver is required for Windows NT.)			*/
/*																			*/
/*					Also supports BitBlaster hardware download cable on		*/
/*					Windows 95, Windows NT, and UNIX platforms.				*/
/*																			*/
/*	Revisions:		1.1 fixed control port initialization for ByteBlaster	*/
/*					2.0 added support for STAPL bytecode format, added code	*/
/*						to get printer port address from Windows registry	*/
/*					2.1 improved messages, fixed delay-calibration bug in	*/
/*						16-bit DOS port, added support for "alternative		*/
/*						cable X", added option to control whether to reset	*/
/*						the TAP after execution, moved porting macros into	*/
/*						jbiport.h											*/
/*					2.2 added support for static memory						*/
/*						fixed /W4 warnings									*/
/*																			*/
/****************************************************************************/

#ifndef NO_ALTERA_STDIO
#define NO_ALTERA_STDIO
#endif

#include "jbiport.h"

#if PORT == WINDOWS
#include <windows.h>
#else
typedef int BOOL;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
#define TRUE 1
#define FALSE 0
#endif

#if defined(USE_STATIC_MEMORY)
	#define N_STATIC_MEMORY_KBYTES ((unsigned int) USE_STATIC_MEMORY)
	#define N_STATIC_MEMORY_BYTES (N_STATIC_MEMORY_KBYTES * 1024)
	#define POINTER_ALIGNMENT sizeof(DWORD)
#else /* USE_STATIC_MEMORY */
	#define POINTER_ALIGNMENT sizeof(BYTE)
#endif /* USE_STATIC_MEMORY */

#if PORT == DOS
#include <bios.h>
#endif

#include "jbiexprt.h"

#if PORT == EMBEDDED
#include "jtag_ioctl.h"

#define Verbose(...) do{} while(0)

#define TDI_PIN 56
#define TDO_PIN	57
#define TCK_PIN	58
#define TMS_PIN	59

typedef enum {
	SET_GPIO_LOW = 0,
	SET_GPIO_HIGH,
	READ_GPIO_VAL,
	
	SET_GPIO_DIR_IN,
	SET_GPIO_DIR_OUT,
}eGpioactions;

int fg_Data_Readback = 0;
unsigned int *pReadData= NULL;


static int Pin_ctrl(int gpioNum, int action); 
#endif 
#if PORT == WINDOWS
#define PGDC_IOCTL_GET_DEVICE_INFO_PP 0x00166A00L
#define PGDC_IOCTL_READ_PORT_PP       0x00166A04L
#define PGDC_IOCTL_WRITE_PORT_PP      0x0016AA08L
#define PGDC_IOCTL_PROCESS_LIST_PP    0x0016AA1CL
#define PGDC_READ_INFO                0x0a80
#define PGDC_READ_PORT                0x0a81
#define PGDC_WRITE_PORT               0x0a82
#define PGDC_PROCESS_LIST             0x0a87
#define PGDC_HDLC_NTDRIVER_VERSION    2
#define PORT_IO_BUFFER_SIZE           256
#endif

#if PORT == WINDOWS
#ifdef __BORLANDC__
/* create dummy inp() and outp() functions for Borland 32-bit compile */
WORD inp(WORD address) { address = address; return(0); }
void outp(WORD address, WORD data) { address = address; data = data; }
#else
#pragma intrinsic (inp, outp)
#endif
#endif

/*
*	For Borland C compiler (16-bit), set the stack size
*/
#if PORT == DOS
#ifdef __BORLANDC__
extern unsigned int _stklen = 50000;
#endif
#endif

/************************************************************************
*
*	Global variables
*/

/* file buffer for Jam STAPL ByteCode input file */
#if PORT == DOS
unsigned char **file_buffer = NULL;
#else
unsigned char *file_buffer = NULL;
#endif
long file_pointer = 0L;
long file_length = 0L;

/* delay count for one millisecond delay */
long one_ms_delay = 0L;

/* serial port interface available on all platforms */
BOOL jtag_hardware_initialized = FALSE;
char *serial_port_name = NULL;
BOOL specified_com_port = FALSE;
int com_port = -1;
void initialize_jtag_hardware(void);
void close_jtag_hardware(void);

#if defined(USE_STATIC_MEMORY)
	unsigned char static_memory_heap[N_STATIC_MEMORY_BYTES] = { 0 };
#endif /* USE_STATIC_MEMORY */

#if defined(USE_STATIC_MEMORY) || defined(MEM_TRACKER)
	unsigned int n_bytes_allocated = 0;
#endif /* USE_STATIC_MEMORY || MEM_TRACKER */

#if defined(MEM_TRACKER)
	unsigned int peak_memory_usage = 0;
	unsigned int peak_allocations = 0;
	unsigned int n_allocations = 0;
#if defined(USE_STATIC_MEMORY)
	unsigned int n_bytes_not_recovered = 0;
#endif /* USE_STATIC_MEMORY */
	const DWORD BEGIN_GUARD = 0x01234567;
	const DWORD END_GUARD = 0x76543210;
#endif /* MEM_TRACKER */

/* function prototypes to allow forward reference */
extern void delay_loop(long count);

/*
*	This structure stores information about each available vector signal
*/
struct VECTOR_LIST_STRUCT
{
	char *signal_name;
	int  hardware_bit;
	int  vector_index;
};

struct VECTOR_LIST_STRUCT vector_list[] =
{
	/* add a record here for each vector signal */
	{ "", 0, -1 }
};

#define VECTOR_SIGNAL_COUNT ((int)(sizeof(vector_list)/sizeof(vector_list[0])))

BOOL verbose = FALSE;

/************************************************************************
*
*	Customized interface functions for Jam STAPL ByteCode Player I/O:
*
*	jbi_jtag_io()
*	jbi_message()
*	jbi_delay()
*/

int jbi_jtag_io(int tms, int tdi, int read_tdo)
{
	int data __attribute__((unused)) = 0;
	int tdo __attribute__((unused)) = 0;
	int i __attribute__((unused)) = 0;
	int result __attribute__((unused)) = 0;
	char ch_data __attribute__((unused)) = 0;

	if (!jtag_hardware_initialized)
	{
		initialize_jtag_hardware();
		jtag_hardware_initialized = TRUE;
	}
	
    //write first (clock low and set other signals.)
    Pin_ctrl(TCK_PIN, 0); 
    Pin_ctrl(TDI_PIN, (tdi?1:0));
    Pin_ctrl(TMS_PIN, (tms?1:0));

    //read_tdo
    if (read_tdo){
        tdo = Pin_ctrl(TDO_PIN, READ_GPIO_VAL);	
    }
    //write second , clock H and clock low  

    Pin_ctrl(TCK_PIN, 1);
    Pin_ctrl(TCK_PIN, 0);

	return (tdo);
}

void jbi_message(char *message_text)
{
    /**** Try to get data back*****/
    if(fg_Data_Readback != 0){
        char* pStr = NULL;    
        pStr=strstr (message_text,"CODE");
        if (pStr != NULL){
            if(sscanf(pStr,"CODE is %X",pReadData)== 1);
            else if(sscanf(pStr,"CODE code is %X",pReadData)== 1);
        }
    }
    else {
        printk("%s.\n",message_text);
    }
}

void jbi_export_integer(char *key, long value)
{
	if (verbose)
	{
		printk("Export: key = \"%s\", value = %ld\n", key, value);
	}
}

#define HEX_LINE_CHARS 72
#define HEX_LINE_BITS (HEX_LINE_CHARS * 4)

char conv_to_hex(unsigned long value)
{
	char c;

	if (value > 9)
	{
		c = (char) (value + ('A' - 10));
	}
	else
	{
		c = (char) (value + '0');
	}

	return (c);
}

void jbi_export_boolean_array(char *key, unsigned char *data, long count)
{
	char string[HEX_LINE_CHARS + 1];
	long i, offset;
	unsigned long size, line, lines, linebits, value, j, k;

	if (verbose)
	{
		if (count > HEX_LINE_BITS)
		{
			printk("Export: key = \"%s\", %ld bits, value = HEX\n", key, count);
			lines = (count + (HEX_LINE_BITS - 1)) / HEX_LINE_BITS;

			for (line = 0; line < lines; ++line)
			{
				if (line < (lines - 1))
				{
					linebits = HEX_LINE_BITS;
					size = HEX_LINE_CHARS;
					offset = count - ((line + 1) * HEX_LINE_BITS);
				}
				else
				{
					linebits = count - ((lines - 1) * HEX_LINE_BITS);
					size = (linebits + 3) / 4;
					offset = 0L;
				}

				string[size] = '\0';
				j = size - 1;
				value = 0;

				for (k = 0; k < linebits; ++k)
				{
					i = k + offset;
					if (data[i >> 3] & (1 << (i & 7))) value |= (1 << (i & 3));
					if ((i & 3) == 3)
					{
						string[j] = conv_to_hex(value);
						value = 0;
						--j;
					}
				}
				if ((k & 3) > 0) string[j] = conv_to_hex(value);

				printk("%s\n", string);
			}

		}
		else
		{
			size = (count + 3) / 4;
			string[size] = '\0';
			j = size - 1;
			value = 0;

			for (i = 0; i < count; ++i)
			{
				if (data[i >> 3] & (1 << (i & 7))) value |= (1 << (i & 3));
				if ((i & 3) == 3)
				{
					string[j] = conv_to_hex(value);
					value = 0;
					--j;
				}
			}
			if ((i & 3) > 0) string[j] = conv_to_hex(value);

			printk("Export: key = \"%s\", %ld bits, value = HEX %s\n", 
				key, count, string);
		}
	}
}

void jbi_delay(long microseconds)
{
#if PORT == WINDOWS
	/* if Windows NT, flush I/O cache buffer before delay loop */
	if (windows_nt && (port_io_count > 0)) flush_ports();
#endif

	delay_loop(microseconds *
		((one_ms_delay / 1000L) + ((one_ms_delay % 1000L) ? 1 : 0)));
}

int jbi_vector_map
(
	int signal_count,
	char **signals
)
{
	int signal, vector, ch_index, diff;
	int matched_count = 0;
	char l, r;

	for (vector = 0; (vector < VECTOR_SIGNAL_COUNT); ++vector)
	{
		vector_list[vector].vector_index = -1;
	}

	for (signal = 0; signal < signal_count; ++signal)
	{
		diff = 1;
		for (vector = 0; (diff != 0) && (vector < VECTOR_SIGNAL_COUNT);
			++vector)
		{
			if (vector_list[vector].vector_index == -1)
			{
				ch_index = 0;
				do
				{
					l = signals[signal][ch_index];
					r = vector_list[vector].signal_name[ch_index];
					diff = (((l >= 'a') && (l <= 'z')) ? (l - ('a' - 'A')) : l)
						- (((r >= 'a') && (r <= 'z')) ? (r - ('a' - 'A')) : r);
					++ch_index;
				}
				while ((diff == 0) && (l != '\0') && (r != '\0'));

				if (diff == 0)
				{
					vector_list[vector].vector_index = signal;
					++matched_count;
				}
			}
		}
	}

	return (matched_count);
}

void *jbi_malloc(unsigned int size)
{
	unsigned int n_bytes_to_allocate = 
#if defined(USE_STATIC_MEMORY) || defined(MEM_TRACKER)
		sizeof(unsigned int) +
#endif /* USE_STATIC_MEMORY || MEM_TRACKER */
#if defined(MEM_TRACKER)
		(2 * sizeof(DWORD)) +
#endif /* MEM_TRACKER */
		(POINTER_ALIGNMENT * ((size + POINTER_ALIGNMENT - 1) / POINTER_ALIGNMENT));

	unsigned char *ptr = 0;


#if defined(MEM_TRACKER)
	if ((n_bytes_allocated + n_bytes_to_allocate) > peak_memory_usage)
	{
		peak_memory_usage = n_bytes_allocated + n_bytes_to_allocate;
	}
	if ((n_allocations + 1) > peak_allocations)
	{
		peak_allocations = n_allocations + 1;
	}
#endif /* MEM_TRACKER */

#if defined(USE_STATIC_MEMORY)
	if ((n_bytes_allocated + n_bytes_to_allocate) <= N_STATIC_MEMORY_BYTES)
	{
		ptr = (&(static_memory_heap[n_bytes_allocated]));
	}
#else /* USE_STATIC_MEMORY */ 
	ptr = (unsigned char *) kmalloc(n_bytes_to_allocate, GFP_DMA|GFP_KERNEL);
	

#endif /* USE_STATIC_MEMORY */

#if defined(USE_STATIC_MEMORY) || defined(MEM_TRACKER)
	if (ptr != 0)
	{
		unsigned int i = 0;

#if defined(MEM_TRACKER)
		for (i = 0; i < sizeof(DWORD); ++i)
		{
			*ptr = (unsigned char) (BEGIN_GUARD >> (8 * i));
			++ptr;
		}
#endif /* MEM_TRACKER */

		for (i = 0; i < sizeof(unsigned int); ++i)
		{
			*ptr = (unsigned char) (size >> (8 * i));
			++ptr;
		}

#if defined(MEM_TRACKER)
		for (i = 0; i < sizeof(DWORD); ++i)
		{
			*(ptr + size + i) = (unsigned char) (END_GUARD >> (8 * i));
			/* don't increment ptr */
		}

		++n_allocations;
#endif /* MEM_TRACKER */

		n_bytes_allocated += n_bytes_to_allocate;
	}
#endif /* USE_STATIC_MEMORY || MEM_TRACKER */

	return ptr;
}

void jbi_free(void *ptr)
{
	if
	(
#if defined(MEM_TRACKER)
		(n_allocations > 0) &&
#endif /* MEM_TRACKER */
		(ptr != 0)
	)
	{
		unsigned char *tmp_ptr = (unsigned char *) ptr;

#if defined(USE_STATIC_MEMORY) || defined(MEM_TRACKER)
		unsigned int n_bytes_to_free = 0;
		unsigned int i = 0;
		unsigned int size = 0;
#endif /* USE_STATIC_MEMORY || MEM_TRACKER */
#if defined(MEM_TRACKER)
		DWORD begin_guard = 0;
		DWORD end_guard = 0;


		tmp_ptr -= sizeof(DWORD);
#endif /* MEM_TRACKER */
#if defined(USE_STATIC_MEMORY) || defined(MEM_TRACKER)
		tmp_ptr -= sizeof(unsigned int);
#endif /* USE_STATIC_MEMORY || MEM_TRACKER */
		ptr = tmp_ptr;

#if defined(MEM_TRACKER)
		for (i = 0; i < sizeof(DWORD); ++i)
		{
			begin_guard |= (((DWORD)(*tmp_ptr)) << (8 * i));
			++tmp_ptr;
		}
#endif /* MEM_TRACKER */

#if defined(USE_STATIC_MEMORY) || defined(MEM_TRACKER)
		for (i = 0; i < sizeof(unsigned int); ++i)
		{
			size |= (((unsigned int)(*tmp_ptr)) << (8 * i));
			++tmp_ptr;
		}
#endif /* USE_STATIC_MEMORY || MEM_TRACKER */

#if defined(MEM_TRACKER)
		tmp_ptr += size;

		for (i = 0; i < sizeof(DWORD); ++i)
		{
			end_guard |= (((DWORD)(*tmp_ptr)) << (8 * i));
			++tmp_ptr;
		}

		if ((begin_guard != BEGIN_GUARD) || (end_guard != END_GUARD))
		{
			printk( "Error: memory corruption detected for allocation #%d... bad %s guard\n",
				n_allocations, (begin_guard != BEGIN_GUARD) ? "begin" : "end");
		}

		--n_allocations;
#endif /* MEM_TRACKER */

#if defined(USE_STATIC_MEMORY) || defined(MEM_TRACKER)
		n_bytes_to_free = 
#if defined(MEM_TRACKER)
		(2 * sizeof(DWORD)) +
#endif /* MEM_TRACKER */
		sizeof(unsigned int) +
		(POINTER_ALIGNMENT * ((size + POINTER_ALIGNMENT - 1) / POINTER_ALIGNMENT));
#endif /* USE_STATIC_MEMORY || MEM_TRACKER */

#if defined(USE_STATIC_MEMORY)
		if ((((unsigned long) ptr - (unsigned long) static_memory_heap) + n_bytes_to_free) == (unsigned long) n_bytes_allocated)
		{
			n_bytes_allocated -= n_bytes_to_free;
		}
#if defined(MEM_TRACKER)
		else
		{
			n_bytes_not_recovered += n_bytes_to_free;
		}
#endif /* MEM_TRACKER */
#else /* USE_STATIC_MEMORY */
#if defined(MEM_TRACKER)
		n_bytes_allocated -= n_bytes_to_free;
#endif /* MEM_TRACKER */
		kfree(ptr);
#endif /* USE_STATIC_MEMORY */
	}
#if defined(MEM_TRACKER)
	else
	{
		if (ptr != 0)
		{
			printk( "Error: attempt to free unallocated memory\n");
		}
	}
#endif /* MEM_TRACKER */
}

/************************************************************************
*
*	get_tick_count() -- Get system tick count in milliseconds
*
*	for DOS, use BIOS function _bios_timeofday()
*	for WINDOWS use GetTickCount() function
*	for UNIX use clock() system function
*/
DWORD get_tick_count(void)
{
	DWORD tick_count = 0L;

#if PORT == WINDOWS
	tick_count = GetTickCount();
#elif PORT == DOS
	_bios_timeofday(_TIME_GETCLOCK, (long *)&tick_count);
	tick_count *= 55L;	/* convert to milliseconds */
#elif PORT == EMBEDDED
	tick_count = 1000L;
#else
	/* assume clock() function returns microseconds */
	tick_count = (DWORD) (clock() / 1000L);
#endif

	return (tick_count);
}

#define DELAY_SAMPLES 10
#define DELAY_CHECK_LOOPS 10000

void calibrate_delay(void)
{
	int sample __attribute__((unused)) = 0;
	int count __attribute__((unused)) = 0;
	DWORD tick_count1 __attribute__((unused)) = 0L;
	DWORD tick_count2 __attribute__((unused)) = 0L;

	one_ms_delay = 0L;

#if PORT == WINDOWS || PORT == DOS
	for (sample = 0; sample < DELAY_SAMPLES; ++sample)
	{
		count = 0;
		tick_count1 = get_tick_count();
		while ((tick_count2 = get_tick_count()) == tick_count1) {};
		do { delay_loop(DELAY_CHECK_LOOPS); count++; } while
			((tick_count1 = get_tick_count()) == tick_count2);
		one_ms_delay += ((DELAY_CHECK_LOOPS * (DWORD)count) /
			(tick_count1 - tick_count2));
	}

	one_ms_delay /= DELAY_SAMPLES;
#else
	/* This is system-dependent!  Update this number for target system */
	one_ms_delay = 1000L;
#endif
}

char *error_text[] =
{
/* JBIC_SUCCESS            0 */ "success",
/* JBIC_OUT_OF_MEMORY      1 */ "out of memory",
/* JBIC_IO_ERROR           2 */ "file access error",
/* JAMC_SYNTAX_ERROR       3 */ "syntax error",
/* JBIC_UNEXPECTED_END     4 */ "unexpected end of file",
/* JBIC_UNDEFINED_SYMBOL   5 */ "undefined symbol",
/* JAMC_REDEFINED_SYMBOL   6 */ "redefined symbol",
/* JBIC_INTEGER_OVERFLOW   7 */ "integer overflow",
/* JBIC_DIVIDE_BY_ZERO     8 */ "divide by zero",
/* JBIC_CRC_ERROR          9 */ "CRC mismatch",
/* JBIC_INTERNAL_ERROR    10 */ "internal error",
/* JBIC_BOUNDS_ERROR      11 */ "bounds error",
/* JAMC_TYPE_MISMATCH     12 */ "type mismatch",
/* JAMC_ASSIGN_TO_CONST   13 */ "assignment to constant",
/* JAMC_NEXT_UNEXPECTED   14 */ "NEXT unexpected",
/* JAMC_POP_UNEXPECTED    15 */ "POP unexpected",
/* JAMC_RETURN_UNEXPECTED 16 */ "RETURN unexpected",
/* JAMC_ILLEGAL_SYMBOL    17 */ "illegal symbol name",
/* JBIC_VECTOR_MAP_FAILED 18 */ "vector signal name not found",
/* JBIC_USER_ABORT        19 */ "execution cancelled",
/* JBIC_STACK_OVERFLOW    20 */ "stack overflow",
/* JBIC_ILLEGAL_OPCODE    21 */ "illegal instruction code",
/* JAMC_PHASE_ERROR       22 */ "phase error",
/* JAMC_SCOPE_ERROR       23 */ "scope error",
/* JBIC_ACTION_NOT_FOUND  24 */ "action not found",
};

#define MAX_ERROR_CODE (int)((sizeof(error_text)/sizeof(error_text[0]))+1)

/************************************************************************/
#include "ast_jtag.h"
#define set_jtag_io jtag_io
#define read_jtag_io jtag_io
static int Jtag_io_test(int gpioNum, int action){
	
	int pin=0;
	int retval = 0, Value = 0;
	
	switch(gpioNum){
		case TDI_PIN: 	pin = 0; break;
		case TDO_PIN:	pin = 1; break;
		case TCK_PIN:	pin = 2; break;
		case TMS_PIN:	pin = 3; break;
		default:
			printk("error Pin!!!\n");
			return -1;
			break;
	}

	switch ( action ){
		case SET_GPIO_LOW:
			
			if ( -1 ==  set_jtag_io(pin, action)){
				printk("Set low failed\n");
				retval = -1;
				goto error_out;
			}
			break;
		case SET_GPIO_HIGH:
			if ( -1 == set_jtag_io(pin, action)){
				printk("Set Gpio high failed\n");
				retval = -1;
				goto error_out;
			}
			break;

		case READ_GPIO_VAL:
			Value = read_jtag_io(pin, action);
			if ( -1 == Value ){
				printk("Read Jtag failed\n");
				retval = -1;
	            goto error_out;
			}
			retval = Value;
			break;
			
		case SET_GPIO_DIR_IN:
		case SET_GPIO_DIR_OUT:

			break;	
		}
	error_out:
	    return retval;
}

static int Pin_ctrl(int gpioNum, int action){

    int retval = 0, Value = 0;
	switch ( action ) {
        case SET_GPIO_LOW:		
        case SET_GPIO_HIGH:
            Jtag_io_test(gpioNum, action);		
            break;
        case READ_GPIO_VAL:
            
            Value = Jtag_io_test(gpioNum, action);		
            if ( -1 == Value )
            {
                //printf ( "Read Gpio failed\n");
                retval = -1;
                goto error_out;
            }
            retval = Value;
            break;
            
        case SET_GPIO_DIR_IN:
            
            break;
        case SET_GPIO_DIR_OUT:
            
            break;	
	}

error_out:
    return retval;
}
/************************************************************************/

int jbcmain(int argc, char **argv)
{
	BOOL help = FALSE;
	BOOL error = FALSE;
	long offset = 0L;
	long error_address = 0L;
	JBI_RETURN_TYPE crc_result = JBIC_SUCCESS;
	JBI_RETURN_TYPE exec_result = JBIC_SUCCESS;
	unsigned short expected_crc = 0;
	unsigned short actual_crc = 0;
	char key[33] = {0};
	char value[257] = {0};
	int exit_status = 0;
	int arg = 0;
	int exit_code = 0;
	int format_version = 0;
	char *workspace = NULL;
	char *action = NULL;
	char *init_list[10];
	int init_count = 0;
	long workspace_size = 0;
	char *exit_string = NULL;
	int reset_jtag = 1;
	int execute_program = 1;
	int action_count = 0;
	int procedure_count = 0;
	int index = 0;
	char *action_name = NULL;
	char *description = NULL;
	JBI_PROCINFO *procedure_list = NULL;
	JBI_PROCINFO *procptr = NULL;

	verbose = FALSE;

	init_list[0] = NULL;

	/* print out the version string and copyright message */
	printk("Jam STAPL ByteCode Player Version 2.2\nCopyright (C) 1998-2001 Altera Corporation\n\n");

	for (arg = 1; arg < argc; arg++)
	{
#if PORT == UNIX
		if (argv[arg][0] == '-')
#else
		if ((argv[arg][0] == '-') || (argv[arg][0] == '/'))
#endif
		{
			// toupper(argv[arg][1])
			if( argv[arg][1] >= 'a' && argv[arg][1] <= 'z'){
				argv[arg][1] += 'A' - 'a';
			}
			             
			//switch(toupper(argv[arg][1]))
			switch(argv[arg][1])
			{
			case 'A':				/* set action name */
				if (action == NULL)
				{
					action = &argv[arg][2];
				}
				else
				{
					error = TRUE;
				}
				break;

#if PORT == WINDOWS || PORT == DOS
			case 'C':				/* Use alternative ISP download cable */
				if(toupper(argv[arg][2]) == 'L')
					alternative_cable_l = TRUE;
				else if(toupper(argv[arg][2]) == 'X')
					alternative_cable_x = TRUE;
				break;
#endif

			case 'D':				/* initialization list */
				if (argv[arg][2] == '"')
				{
					init_list[init_count] = &argv[arg][3];
				}
				else
				{
					init_list[init_count] = &argv[arg][2];
				}
				init_list[++init_count] = NULL;
				break;

#if PORT == WINDOWS || PORT == DOS
			case 'P':				/* set LPT port address */
				specified_lpt_port = TRUE;
				if (sscanf(&argv[arg][2], "%d", &lpt_port) != 1) error = TRUE;
				if ((lpt_port < 1) || (lpt_port > 3)) error = TRUE;
				if (error)
				{
					if (sscanf(&argv[arg][2], "%x", &lpt_port) == 1)
					{
						if ((lpt_port == 0x3bc) ||
							(lpt_port == 0x378) ||
							(lpt_port == 0x278))
						{
							error = FALSE;
							specified_lpt_addr = TRUE;
							lpt_addr = (WORD) lpt_port;
							lpt_port = 1;
						}
					}
				}
				break;
#endif

			case 'R':		/* don't reset the JTAG chain after use */
				reset_jtag = 0;
				break;

			case 'S':				/* set serial port address */
				serial_port_name = &argv[arg][2];
				specified_com_port = TRUE;
				break;

			case 'M':				/* set memory size */
				if (sscanf(&argv[arg][2], "%ld", &workspace_size) != 1)
					error = TRUE;
				if (workspace_size == 0) error = TRUE;
				break;

			case 'H':				/* help */
				help = TRUE;
				break;

			case 'V':				/* verbose */
				verbose = TRUE;
				break;

			case 'I':				/* show info only, do not execute */
				verbose = TRUE;
				execute_program = 0;
				break;
#if PORT == EMBEDDED
			case '@':				/* get memory buffer address */
                if (sscanf(&argv[arg][2], "%08x", (unsigned int *)&file_buffer) != 1) error = TRUE;
				break;
			case 'L':				/* get memory buffer size */
                if (sscanf(&argv[arg][2], "%ld", &file_length) != 1) error = TRUE;
				break;
            case '$':				/* get 4-byte memory address */
                if (sscanf(&argv[arg][2], "%08x", (unsigned int *)&pReadData) != 1) error = TRUE;
                if (error == FALSE){
                    fg_Data_Readback = 1;
                }
				break;
#endif
			default:
				error = TRUE;
				break;
			}
		}
		if (error)
		{
			printk("Illegal argument: \"%s\"\n", argv[arg]);
			help = TRUE;
			error = FALSE;
		}
	}

	if (help)
	{
		printk( "Usage:  jbi [options] <filename>\n");
		printk( "\nAvailable options:\n");
		printk( "    -h          : show help message\n");
		printk( "    -v          : show verbose messages\n");
		printk( "    -i          : show file info only - does not execute any action\n");
		printk( "    -a<action>  : specify an action name (Jam STAPL)\n");
		printk( "    -d<var=val> : initialize variable to specified value (Jam 1.1)\n");
		printk( "    -d<proc=1>  : enable optional procedure (Jam STAPL)\n");
		printk( "    -d<proc=0>  : disable recommended procedure (Jam STAPL)\n");
		printk( "    -s<port>    : serial port name (for BitBlaster)\n");
		printk( "    -r          : don't reset JTAG TAP after use\n");
		exit_status = 1;
	}
	else if ((workspace_size > 0) &&
		((workspace = (char *) jbi_malloc((size_t) workspace_size)) == NULL))
	{
		printk( "Error: can't allocate memory (%d Kbytes)\n",
			(int) (workspace_size / 1024L));
		exit_status = 1;
	}
	else
	{
        do{ //for exit(1) changed code.
		if (exit_status == 0)
		{
			
			/*
			*	Get Operating System type
			*/
#if PORT == WINDOWS
			windows_nt = !(GetVersion() & 0x80000000);
#endif

			/*
			*	Calibrate the delay loop function
			*/
			calibrate_delay();

			/*
			*	Check CRC
			*/
			crc_result = jbi_check_crc(file_buffer, file_length,
				&expected_crc, &actual_crc);

			if (verbose || (crc_result == JBIC_CRC_ERROR))
			{
				switch (crc_result)
				{
				case JBIC_SUCCESS:
					printk("CRC matched: CRC value = %04X\n", actual_crc);
					break;

				case JBIC_CRC_ERROR:
					printk("CRC mismatch: expected %04X, actual %04X\n",
						expected_crc, actual_crc);
					break;

				case JBIC_UNEXPECTED_END:
					printk("Expected CRC not found, actual CRC value = %04X\n",
						actual_crc);
					break;

				case JBIC_IO_ERROR:
					printk("Error: File format is not recognized.\n");
					//exit(1);
                    exec_result = JBIC_IO_ERROR;
					exit_status = 1;
                    continue;
					break;

				default:
					printk("CRC function returned error code %d\n", crc_result);
					break;
				}
			}

			if (verbose)
			{
				/*
				*	Display file format version
				*/
				jbi_get_file_info(file_buffer, file_length,
					&format_version, &action_count, &procedure_count);

				printk("File format is %s ByteCode format\n",
					(format_version == 2) ? "Jam STAPL" : "pre-standardized Jam 1.1");

				/*
				*	Dump out NOTE fields
				*/
				while (jbi_get_note(file_buffer, file_length,
					&offset, key, value, 256) == 0)
				{
					printk("NOTE \"%s\" = \"%s\"\n", key, value);
				}

				/*
				*	Dump the action table
				*/
				if ((format_version == 2) && (action_count > 0))
				{
					printk("\nActions available in this file:\n");

					for (index = 0; index < action_count; ++index)
					{
						jbi_get_action_info(file_buffer, file_length,
							index, &action_name, &description, &procedure_list);

						if (description == NULL)
						{
							printk("%s\n", action_name);
						}
						else
						{
							printk("%s \"%s\"\n", action_name, description);
						}

#if PORT == DOS
						if (action_name != NULL) jbi_free(action_name);
						if (description != NULL) jbi_free(description);
#endif

						procptr = procedure_list;
						while (procptr != NULL)
						{
							if (procptr->attributes != 0)
							{
								printk("    %s (%s)\n", procptr->name,
									(procptr->attributes == 1) ?
									"optional" : "recommended");
							}

#if PORT == DOS
							if (procptr->name != NULL) jbi_free(procptr->name);
#endif

							procedure_list = procptr->next;
							jbi_free(procptr);
							procptr = procedure_list;
						}
					}

					/* add a blank line before execution messages */
					if (execute_program) printk("\n");
				}
			}

			if (execute_program)
			{
				/*
				*	Execute the Jam STAPL ByteCode program
				*/
				exec_result = jbi_execute(file_buffer, file_length, workspace,
					workspace_size, action, init_list, reset_jtag,
					&error_address, &exit_code, &format_version);


				if (exec_result == JBIC_SUCCESS)
				{
					if (format_version == 2)
					{
						switch (exit_code)
						{
						case  0: exit_string = "Success"; break;
						case  1: exit_string = "Checking chain failure"; break;
						case  2: exit_string = "Reading IDCODE failure"; break;
						case  3: exit_string = "Reading USERCODE failure"; break;
						case  4: exit_string = "Reading UESCODE failure"; break;
						case  5: exit_string = "Entering ISP failure"; break;
						case  6: exit_string = "Unrecognized device"; break;
						case  7: exit_string = "Device revision is not supported"; break;
						case  8: exit_string = "Erase failure"; break;
						case  9: exit_string = "Device is not blank"; break;
						case 10: exit_string = "Device programming failure"; break;
						case 11: exit_string = "Device verify failure"; break;
						case 12: exit_string = "Read failure"; break;
						case 13: exit_string = "Calculating checksum failure"; break;
						case 14: exit_string = "Setting security bit failure"; break;
						case 15: exit_string = "Querying security bit failure"; break;
						case 16: exit_string = "Exiting ISP failure"; break;
						case 17: exit_string = "Performing system test failure"; break;
						default: exit_string = "Unknown exit code"; break;
						}
					}
					else
					{
						switch (exit_code)
						{
						case 0: exit_string = "Success"; break;
						case 1: exit_string = "Illegal initialization values"; break;
						case 2: exit_string = "Unrecognized device"; break;
						case 3: exit_string = "Device revision is not supported"; break;
						case 4: exit_string = "Device programming failure"; break;
						case 5: exit_string = "Device is not blank"; break;
						case 6: exit_string = "Device verify failure"; break;
						case 7: exit_string = "SRAM configuration failure"; break;
						default: exit_string = "Unknown exit code"; break;
						}
					}

                    if((fg_Data_Readback == 0) && (exit_code != 0))
                        printk("Exit code = %d... %s\n", exit_code, exit_string);
				}
				else if ((format_version == 2) &&
					(exec_result == JBIC_ACTION_NOT_FOUND))
				{
					if ((action == NULL) || (*action == '\0'))
					{
						printk("Error: no action specified for Jam STAPL file.\nProgram terminated.\n");
					}
					else
					{
						printk("Error: action \"%s\" is not supported for this Jam STAPL file.\nProgram terminated.\n", action);
					}
				}
				else if (exec_result < MAX_ERROR_CODE)
				{
					printk("Error at address %ld: %s.\nProgram terminated.\n",
						error_address, error_text[exec_result]);
				}
				else
				{
					printk("Unknown error code %ld\n", (long int)exec_result);
				}

			}
		}
        }while(0);  //for exit(1) changed code.
	}

	if (jtag_hardware_initialized) close_jtag_hardware();

	if (workspace != NULL) jbi_free(workspace);

#if defined(MEM_TRACKER)
	if (verbose)
	{
#if defined(USE_STATIC_MEMORY)
		printk( "Memory Usage Info: static memory size = %ud (%dKB)\n", N_STATIC_MEMORY_BYTES, N_STATIC_MEMORY_KBYTES);
#endif /* USE_STATIC_MEMORY */
		printk( "Memory Usage Info: peak memory usage = %ud (%dKB)\n", peak_memory_usage, (peak_memory_usage + 1023) / 1024);
		printk( "Memory Usage Info: peak allocations = %d\n", peak_allocations);
#if defined(USE_STATIC_MEMORY)
		if ((n_bytes_allocated - n_bytes_not_recovered) != 0)
		{
			fprintf(stdout, "Memory Usage Info: bytes still allocated = %d (%dKB)\n", (n_bytes_allocated - n_bytes_not_recovered), ((n_bytes_allocated - n_bytes_not_recovered) + 1023) / 1024);
		}
#else /* USE_STATIC_MEMORY */
		if (n_bytes_allocated != 0)
		{
			printk( "Memory Usage Info: bytes still allocated = %d (%dKB)\n", n_bytes_allocated, (n_bytes_allocated + 1023) / 1024);
		}
#endif /* USE_STATIC_MEMORY */
		if (n_allocations != 0)
		{
			printk( "Memory Usage Info: allocations not freed = %d\n", n_allocations);
		}
	}
#endif /* MEM_TRACKER */

    if (exec_result != JBIC_SUCCESS) return -1;
    return exit_code;
}

#if PORT==WINDOWS
#ifndef __BORLANDC__
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
*	SEARCH_DYN_DATA
*
*	Searches recursively in Windows 95/98 Registry for parallel port info
*	under HKEY_DYN_DATA registry key.  Called by search_local_machine().
*/
void search_dyn_data
(
	char *dd_path,
	char *hardware_key,
	int lpt
)
{
	DWORD index;
	DWORD size;
	DWORD type;
	LONG result;
	HKEY key;
	int length;
	WORD address;
	char buffer[1024];
	FILETIME last_write = {0};
	WORD *word_ptr;
	int i;

	length = strlen(dd_path);

	if (RegOpenKeyEx(
		HKEY_DYN_DATA,
		dd_path,
		0L,
		KEY_READ,
		&key)
		== ERROR_SUCCESS)
	{
		size = 1023;

		if (RegQueryValueEx(
			key,
			"HardWareKey",
			NULL,
			&type,
			(unsigned char *) buffer,
			&size)
			== ERROR_SUCCESS)
		{
			if ((type == REG_SZ) && (stricmp(buffer, hardware_key) == 0))
			{
				size = 1023;

				if (RegQueryValueEx(
					key,
					"Allocation",
					NULL,
					&type,
					(unsigned char *) buffer,
					&size)
					== ERROR_SUCCESS)
				{
					/*
					*	By "inspection", I have found five cases: size 32, 48,
					*	56, 60, and 80 bytes.  The port address seems to be
					*	located at different offsets in the buffer for these
					*	five cases, as shown below.  If a valid port address
					*	is not found, or the size is not one of these known
					*	sizes, then I search through the entire buffer and
					*	look for a value which is a valid port address.
					*/

					word_ptr = (WORD *) buffer;

					if ((type == REG_BINARY) && (size == 32))
					{
						address = word_ptr[10];
					}
					else if ((type == REG_BINARY) && (size == 48))
					{
						address = word_ptr[18];
					}
					else if ((type == REG_BINARY) && (size == 56))
					{
						address = word_ptr[22];
					}
					else if ((type == REG_BINARY) && (size == 60))
					{
						address = word_ptr[24];
					}
					else if ((type == REG_BINARY) && (size == 80))
					{
						address = word_ptr[24];
					}
					else address = 0;

					/* if not found, search through entire buffer */
					i = 0;
					while ((i < (int) (size / 2)) &&
						(address != 0x278) &&
						(address != 0x27C) &&
						(address != 0x378) &&
						(address != 0x37C) &&
						(address != 0x3B8) &&
						(address != 0x3BC))
					{
						if ((word_ptr[i] == 0x278) ||
							(word_ptr[i] == 0x27C) ||
							(word_ptr[i] == 0x378) ||
							(word_ptr[i] == 0x37C) ||
							(word_ptr[i] == 0x3B8) ||
							(word_ptr[i] == 0x3BC))
						{
							address = word_ptr[i];
						}
						++i;
					}

					if ((address == 0x278) ||
						(address == 0x27C) ||
						(address == 0x378) ||
						(address == 0x37C) ||
						(address == 0x3B8) ||
						(address == 0x3BC))
					{
						lpt_addresses_from_registry[lpt] = address;
					}
				}
			}
		}

		index = 0;

		do
		{
			size = 1023;

			result = RegEnumKeyEx(
				key,
				index++,
				buffer,
				&size,
				NULL,
				NULL,
				NULL,
				&last_write);

			if (result == ERROR_SUCCESS)
			{
				dd_path[length] = '\\';
				dd_path[length + 1] = '\0';
				strcpy(&dd_path[length + 1], buffer);

				search_dyn_data(dd_path, hardware_key, lpt);
	
				dd_path[length] = '\0';
			}
		}
		while (result == ERROR_SUCCESS);

		RegCloseKey(key);
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
*	SEARCH_LOCAL_MACHINE
*
*	Searches recursively in Windows 95/98 Registry for parallel port info
*	under HKEY_LOCAL_MACHINE\Enum.  When parallel port is found, calls
*	search_dyn_data() to get the port address.
*/
void search_local_machine
(
	char *lm_path,
	char *dd_path
)
{
	DWORD index;
	DWORD size;
	DWORD type;
	LONG result;
	HKEY key;
	int length;
	char buffer[1024];
	FILETIME last_write = {0};

	length = strlen(lm_path);

	if (RegOpenKeyEx(
		HKEY_LOCAL_MACHINE,
		lm_path,
		0L,
		KEY_READ,
		&key)
		== ERROR_SUCCESS)
	{
		size = 1023;

		if (RegQueryValueEx(
			key,
			"PortName",
			NULL,
			&type,
			(unsigned char *) buffer,
			&size)
			== ERROR_SUCCESS)
		{
			if ((type == REG_SZ) &&
				(size == 5) &&
				(buffer[0] == 'L') &&
				(buffer[1] == 'P') &&
				(buffer[2] == 'T') &&
				(buffer[3] >= '1') &&
				(buffer[3] <= '4') &&
				(buffer[4] == '\0'))
			{
				/* we found the entry in HKEY_LOCAL_MACHINE, now we need to */
				/* find the corresponding entry under HKEY_DYN_DATA.  */
				/* add 5 to lm_path to skip over "Enum" and backslash */
				search_dyn_data(dd_path, &lm_path[5], (buffer[3] - '1'));
			}
		}

		index = 0;

		do
		{
			size = 1023;

			result = RegEnumKeyEx(
				key,
				index++,
				buffer,
				&size,
				NULL,
				NULL,
				NULL,
				&last_write);

			if (result == ERROR_SUCCESS)
			{
				lm_path[length] = '\\';
				lm_path[length + 1] = '\0';
				strcpy(&lm_path[length + 1], buffer);

				search_local_machine(lm_path, dd_path);
	
				lm_path[length] = '\0';
			}
		}
		while (result == ERROR_SUCCESS);

		RegCloseKey(key);
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
*	GET_LPT_ADDRESSES_FROM_REGISTRY
*
*	Searches Win95/98 registry recursively to get I/O port addresses for
*	parallel ports.
*/
void get_lpt_addresses_from_registry()
{
	char lm_path[1024];
	char dd_path[1024];

	strcpy(lm_path, "Enum");
	strcpy(dd_path, "Config Manager");
	search_local_machine(lm_path, dd_path);
}
#endif
#endif

void initialize_jtag_hardware()
{
    //For gpio pin.
	Pin_ctrl(TDI_PIN, SET_GPIO_DIR_OUT);
	Pin_ctrl(TDO_PIN, SET_GPIO_DIR_IN);
	Pin_ctrl(TCK_PIN, SET_GPIO_DIR_OUT);
	Pin_ctrl(TMS_PIN, SET_GPIO_DIR_OUT);
}

void close_jtag_hardware()
{
	fg_Data_Readback = 0;
}

#if PORT == WINDOWS
/**************************************************************************/
/*                                                                        */

BOOL initialize_nt_driver()

/*                                                                        */
/*  Uses CreateFile() to open a connection to the Windows NT device       */
/*  driver.                                                               */
/*                                                                        */
/**************************************************************************/
{
	BOOL status = FALSE;

	ULONG buffer[1];
	ULONG returned_length = 0;
	char nt_lpt_str[] = { '\\', '\\', '.', '\\',
		'A', 'L', 'T', 'L', 'P', 'T', '1', '\0' };


	nt_lpt_str[10] = (char) ('1' + (lpt_port - 1));

	nt_device_handle = CreateFile(
		nt_lpt_str,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (nt_device_handle == INVALID_HANDLE_VALUE)
	{
		printk(
			"I/O error:  cannot open device %s\nCheck port number and device driver installation",
			nt_lpt_str);
	}
	else
	{
		if (DeviceIoControl(
			nt_device_handle,			/* Handle to device */
			PGDC_IOCTL_GET_DEVICE_INFO_PP,	/* IO Control code */
			(ULONG *)NULL,					/* Buffer to driver. */
			0,								/* Length of buffer in bytes. */
			&buffer,						/* Buffer from driver. */
			sizeof(ULONG),					/* Length of buffer in bytes. */
			&returned_length,				/* Bytes placed in data_buffer. */
			NULL))							/* Wait for operation to complete */
		{
			if (returned_length == sizeof(ULONG))
			{
				if (buffer[0] == PGDC_HDLC_NTDRIVER_VERSION)
				{
					status = TRUE;
				}
				else
				{
					printk(
						"I/O error:  device driver %s is not compatible\n(Driver version is %lu, expected version %lu.\n",
						nt_lpt_str,
						(unsigned long) buffer[0],
						(unsigned long) PGDC_HDLC_NTDRIVER_VERSION);
				}
			}
			else
			{
				printk( "I/O error:  device driver %s is not compatible.\n",
					nt_lpt_str);
			}
		}

		if (!status)
		{
			CloseHandle(nt_device_handle);
			nt_device_handle = INVALID_HANDLE_VALUE;
		}
	}

	if (!status)
	{
		/* error message already given */
		exit(1);
	}

	return (status);
}
#endif

#if PORT == WINDOWS || PORT == DOS
/**************************************************************************/
/*                                                                        */

void write_byteblaster
(
	int port,
	int data
)

/*                                                                        */
/**************************************************************************/
{
#if PORT == WINDOWS
	BOOL status = FALSE;

	int returned_length = 0;
	int buffer[2];


	if (windows_nt)
	{
		/*
		*	On Windows NT, access hardware through device driver
		*/
		if (port == 0)
		{
			port_io_buffer[port_io_count].data = (USHORT) data;
			port_io_buffer[port_io_count].command = PGDC_WRITE_PORT;
			++port_io_count;

			if (port_io_count >= PORT_IO_BUFFER_SIZE) flush_ports();
		}
		else
		{
			if (port_io_count > 0) flush_ports();

			buffer[0] = port;
			buffer[1] = data;

			status = DeviceIoControl(
				nt_device_handle,			/* Handle to device */
				PGDC_IOCTL_WRITE_PORT_PP,	/* IO Control code for write */
				(ULONG *)&buffer,			/* Buffer to driver. */
				2 * sizeof(int),			/* Length of buffer in bytes. */
				(ULONG *)NULL,				/* Buffer from driver.  Not used. */
				0,							/* Length of buffer in bytes. */
				(ULONG *)&returned_length,	/* Bytes returned.  Should be zero. */
				NULL);						/* Wait for operation to complete */

			if ((!status) || (returned_length != 0))
			{
				printk( "I/O error:  Cannot access ByteBlaster hardware\n");
				CloseHandle(nt_device_handle);
				exit(1);
			}
		}
	}
	else
#endif
	{
		/*
		*	On Windows 95, access hardware directly
		*/
		outp((WORD)(port + lpt_addr), (WORD)data);
	}
}

/**************************************************************************/
/*                                                                        */

int read_byteblaster
(
	int port
)

/*                                                                        */
/**************************************************************************/
{
	int data = 0;

#if PORT == WINDOWS

	BOOL status = FALSE;

	int returned_length = 0;


	if (windows_nt)
	{
		/* flush output cache buffer before reading from device */
		if (port_io_count > 0) flush_ports();

		/*
		*	On Windows NT, access hardware through device driver
		*/
		status = DeviceIoControl(
			nt_device_handle,			/* Handle to device */
			PGDC_IOCTL_READ_PORT_PP,	/* IO Control code for Read */
			(ULONG *)&port,				/* Buffer to driver. */
			sizeof(int),				/* Length of buffer in bytes. */
			(ULONG *)&data,				/* Buffer from driver. */
			sizeof(int),				/* Length of buffer in bytes. */
			(ULONG *)&returned_length,	/* Bytes placed in data_buffer. */
			NULL);						/* Wait for operation to complete */

		if ((!status) || (returned_length != sizeof(int)))
		{
			printk( "I/O error:  Cannot access ByteBlaster hardware\n");
			CloseHandle(nt_device_handle);
			exit(1);
		}
	}
	else
#endif
	{
		/*
		*	On Windows 95, access hardware directly
		*/
		data = inp((WORD)(port + lpt_addr));
	}

	return (data & 0xff);
}

#if PORT == WINDOWS
void flush_ports(void)
{
	ULONG n_writes = 0L;
	BOOL status;

	status = DeviceIoControl(
		nt_device_handle,			/* handle to device */
		PGDC_IOCTL_PROCESS_LIST_PP,	/* IO control code */
		(LPVOID)port_io_buffer,		/* IN buffer (list buffer) */
		port_io_count * sizeof(struct PORT_IO_LIST_STRUCT),/* length of IN buffer in bytes */
		(LPVOID)port_io_buffer,	/* OUT buffer (list buffer) */
		port_io_count * sizeof(struct PORT_IO_LIST_STRUCT),/* length of OUT buffer in bytes */
		&n_writes,					/* number of writes performed */
		0);							/* wait for operation to complete */

	if ((!status) || ((port_io_count * sizeof(struct PORT_IO_LIST_STRUCT)) != n_writes))
	{
		printk( "I/O error:  Cannot access ByteBlaster hardware\n");
		CloseHandle(nt_device_handle);
		exit(1);
	}

	port_io_count = 0;
}
#endif /* PORT == WINDOWS */
#endif /* PORT == WINDOWS || PORT == DOS */


void delay_loop(long count)
{
#if 1
	while (count != 0L){
		if (count > 20000){//20ms
			msleep(count/1000);			
			count= count % 1000;
		}
		else{
			static unsigned int idle_count = 0;
			//msleep :: for kernel switch other task.
			if((++ idle_count  ) % 1024 == 0){
				msleep(0);
				idle_count = 0;
			}
			udelay( count);
			count-= count;
		}
	}
#else
	while (count != 0L) count--;
#endif
}
