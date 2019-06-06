#ifndef __LIBIPMI_USB_H__
#define __LIBIPMI_USB_H__

#ifdef __GNUC__
/** \brief gcc method for designating a packed struct */
#define PACKED __attribute__ ((packed))
#else
#define PACKED
#pragma pack( 1 )
#endif
#define MAX_REQUEST_SIZE_USB (1024 * 60)
typedef struct {
	uint8	byNetFnLUN;
	uint8	byCmd;
	uint8	byData[MAX_REQUEST_SIZE_USB];
}PACKED IPMIUSBRequest_T;

#undef PACKED
#ifndef __GNUC__
#pragma pack( )
#endif








#endif /* __LIBIPMI_USB_H__ */




