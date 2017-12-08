
#define LPCUART_CTL_FILE   "/dev/lpcuart0"

#define source_port_uart 1
#define source_port_com  2
#define source_port_both 3



/****This is the structure that is passed back and forth between userspace and driver as an ioctl arg*****/
typedef struct 
{
	unsigned short uart_port;
	unsigned short source_port;
	unsigned short destination_port;
	unsigned short source_port_type;
}  __attribute__((packed)) lpcuart_data_t;	


typedef lpcuart_data_t lpcuart_ioctl_data;



int enable_uart_port( unsigned short uart_port );
int disable_uart_port( unsigned short uart_port );
int route_uart_to_com( unsigned short uart_port, unsigned short com_port );
int route_com_to_uart( unsigned short uart_port, unsigned short com_port );
int route_uart_to_uart( unsigned short uart_source, unsigned short uart_des );



