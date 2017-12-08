/**
 * @file   libipmi_XportDevice.h
 * @author Anurag Bhatia
 * @date   21-Sep-2004
 *
 * @brief  Contains exported APIs by LIBIPMI for
 *  		communicating with the BMC for NetFn type Transport.
 *
 */

#ifndef __LIBIPMI_XPORTDEVICE_H__
#define __LIBIPMI_XPORTDEVICE_H__

/* LIIPMI core header files */
#include "libipmi_session.h"
#include "libipmi_errorcodes.h"
#include "libipmiifc.h"

/* command specific header files */
#include "IPMIDefs.h"
#include "IPMI_DeviceConfig.h"
#include "IPMI_LANConfig.h" // For LAN configuration parameters union definition


#include "IPMI_SOLConfig.h"
#include "IPMI_SerialModem.h"


/* Handling for packing structs */
#ifdef __GNUC__
#define PACKED __attribute__ ((packed))
#else
#define PACKED
#pragma pack( 1 )
#endif

#ifdef  __cplusplus
extern "C" {
#endif

#define CHANNEL_MEDIUM_TYPE_LAN 0x04
	
/*---------- IPMI Command direct routines ----------*/
/* LAN related functions */
LIBIPMI_API uint16 IPMICMD_GetLANConfig(IPMI20_SESSION_T *pSession,
				GetLanConfigReq_T* pGetLANConfigReqData,
				GetLanConfigRes_T* pGetLANConfigResData,
				int timeout);
LIBIPMI_API uint16 IPMICMD_SetLANConfig(IPMI20_SESSION_T *pSession,
				SetLanConfigReq_T* pSetLANConfigReqData,
				unsigned int ReqDataLen,
				SetLanConfigRes_T* pSetLANConfigResData,
				int timeout);


/*---------- LIBIPMI Higher level routines -----------*/
#define LAN_DEST_TYPE_SNMP 0x0
#define LAN_DEST_TYPE_OEM1 0x6
#define LAN_DEST_ADDR_FORMAT_IPV4 0x0
#define LAN_DEST_ADDR_FORMAT_IPV6 0x1
#define VLAN_STATUS_BIT 0x8000

uint16 LIBIPMI_HL_GetNumOfLANInterfaces(IPMI20_SESSION_T *pSession, INT8U* pLanInfo,
					INT8U* pNumOfLANDestEntries, int timeout);

#ifdef CONFIG_SPX_FEATURE_PHY_SUPPORT
uint16 LIBIPMI_HL_GetPHYConfig(IPMI20_SESSION_T *pSession, INT8U Channel,
               		       PHYConfig_T *PHYConfigRes, int timeout);

uint16 LIBIPMI_HL_GetPHYConfigTable(IPMI20_SESSION_T *pSession, PHYConfig_T* pPHYConfigTable,
       				    INT8U* pNumOfLANInterfaces, int timeout);

uint16 LIBIPMI_HL_SetPHYConfig(IPMI20_SESSION_T *pSession, INT8U Channel, INT8U AutoNegotiation,
         		       INT16U Speed, INT8U Duplex, int timeout);
#endif

#ifdef CONFIG_SPX_FEATURE_NCSI_IPMI_COMMAND_SUPPORT
uint16 LIBIPMI_HL_SetNCSISetting(IPMI20_SESSION_T *pSession, INT8U Channel, INT8U Index,
                                                                   INT8U ChannelID, INT8U PackageID, int timeout);

uint16 LIBIPMI_HL_GetNCSISetting(IPMI20_SESSION_T *pSession, INT8U Channel,
                                                                    INT8U Index,INT8U *ChannelID,INT8U *PackageID, int timeout);

uint16 LIBIPMI_HL_GetNCSIConfigNUM(IPMI20_SESSION_T *pSession, INT8U Channel,
                                                                            INT8U *ConfigNum, int timeout);

#endif
uint16 LIBIPMI_HL_GetNumOfLANDestinationEntries(IPMI20_SESSION_T *pSession,
					INT8U* pNumOfLANDestEntries,
					uint8 Channel,
					int timeout);

uint16 LIBIPMI_HL_GetLANDestinationType(IPMI20_SESSION_T *pSession,
					uint8 Channel,
					uint8  index,
					LANDestType_T *LANDestTypeRes,
					int timeout);

uint16 LIBIPMI_HL_GetLANDestinationAddress(IPMI20_SESSION_T *pSession,
					uint8 Channel,
					uint8  index,
					LANDestAddr_T *LANDestAddrRes,
					int timeout);

uint16 LIBIPMI_HL_GetLANDestinationV6Address(IPMI20_SESSION_T *pSession,
					uint8 Channel,
					uint8  index,
					LANDestv6Addr_T *LANDestv6AddrRes,
					int timeout);

uint16 LIBIPMI_HL_GetLANDestinationType_AddrTable(IPMI20_SESSION_T *pSession,
					LANDestType_T* pDestTypeTable,
					LANDestAddr_T* pDestAddrTable,
					uint8* pNumOfLANDestEntries,
					uint8 Channel,
					int timeout);

uint16 LIBIPMI_HL_SetLANDestinationType(IPMI20_SESSION_T *pSession,
					uint8 Channel,
					uint8  index,
					uint8 desttype,
					int timeout);

uint16 LIBIPMI_HL_SetLANDestinationAddress(IPMI20_SESSION_T *pSession,
					uint8 Channel,
					uint8  index,
					uint8 addrFormat,
					uint8* destAddr,
					int timeout);

uint16 LIBIPMI_HL_SetLANDestinationV6Address(IPMI20_SESSION_T *pSession,
					uint8 Channel,
					uint8  index,
					uint8 addrFormat,
					uint8* destAddr,
					int timeout);

uint16 LIBIPMI_HL_SetLANAlertEntry(IPMI20_SESSION_T *pSession,
					uint8 DestSel,
					uint8* pDestAddr,uint8 channel,
					int timeout);

uint16 LIBIPMI_HL_SetLANAlertEntry_IPv6(IPMI20_SESSION_T *pSession,
					uint8 DestSel,
					uint8* pDestAddr,uint8 channel,
					int timeout);

uint16 LIBIPMI_HL_EnableVlan (IPMI20_SESSION_T *pSession, 
		            INT8U VlanStatus, INT16U vlanID, INT8U Priority, INT8U Channel, int timeout);

uint16 LIBIPMI_HL_GetVlanStatus (IPMI20_SESSION_T *pSession, 
		            INT8U *VlanStatus, INT16U *vlanID, INT8U *Priority, INT8U Channel, int timeout);

LIBIPMI_API uint16 IPMICMD_SetLANAlertEntryType(IPMI20_SESSION_T *pSession,
					uint8 DestSel,
					uint8 AlertType,uint8 Channel,
					int timeout);

uint16 LIBIPMI_HL_GetLANEnable(IPMI20_SESSION_T *pSession,
									uint8 EthIndex, uint8 *LANEnable, int timeout);

uint16 LIBIPMI_HL_SetLANEnable(IPMI20_SESSION_T *pSession,
									uint8 EthIndex, uint8 LANEnable, int timeout);

uint16 LIBIPMI_HL_GetBondEnable(IPMI20_SESSION_T *pSession,
                                                           uint8 Index,uint8 * BondCfg,int timeout);

uint16 LIBIPMI_HL_SetBondEnable(IPMI20_SESSION_T * pSession, 
                                                           uint8 * BondCfg, int timeout);

uint16 LIBIPMI_HL_SetActiveSlave(IPMI20_SESSION_T * pSession,uint8 Index,
                                                               uint8 ActiveSlave, int timeout);

uint16 LIBIPMI_HL_GetActiveSlave(IPMI20_SESSION_T * pSession, 
                                                                    uint8 Index, uint8 * Activeslave, int timeout);

uint16 LIBIPMI_HL_IsBondEnabled(IPMI20_SESSION_T * pSession, 
                                                                    uint8 * BondEnabled, int timeout);

uint16 LIBIPMI_HL_BondVLANEnabled(IPMI20_SESSION_T * pSession, 
                                                                    uint8 * BondVLANEnabled, int timeout);

uint16 LIBIPMI_HL_GetLANCount(IPMI20_SESSION_T * pSession, 
                                                         uint8* LANCount,uint8 * LANIndex, int timeout);

uint16 LIBIPMI_HL_GetChannelNum(IPMI20_SESSION_T * pSession, 
                                                                    uint8 Index, uint8 * Channel, int timeout);

uint16 LIBIPMI_HL_GetLANIndex(IPMI20_SESSION_T * pSession, 
                                                                    uint8 Index, uint8 * IfcName, int timeout);

uint16 LIBIPMI_HL_GetSNMPCommunity(IPMI20_SESSION_T *pSession,
                    uint8* pCommunityStr, uint8 Channel,
					int timeout);

uint16 LIBIPMI_HL_SetSNMPCommunity(IPMI20_SESSION_T *pSession,
                    uint8* CommunityStr,uint8 Channel,
					int timeout);

uint16 LIBIPMI_HL_GetMACAddress(IPMI20_SESSION_T *pSession,
		            INT8U *MACAddr,INT8U Channel,
		            int timeout);

uint16 LIBIPMI_HL_SetIPv4Source(IPMI20_SESSION_T *pSession, 
		            INT8U Source,INT8U Channel, 
		            int timeout);

uint16 LIBIPMI_HL_GetIPv4Source(IPMI20_SESSION_T *pSession,
		            INT8U *Source,INT8U Channel,
		            int timeout);

uint16 LIBIPMI_HL_SetIPv6Source(IPMI20_SESSION_T *pSession,
		            INT8U Source,INT8U Channel, 
		            int timeout);

uint16 LIBIPMI_HL_GetIPv6Source(IPMI20_SESSION_T *pSession, 
		            INT8U *Source,INT8U Channel, 
		            int timeout);

uint16 LIBIPMI_HL_SetIPv4Address(IPMI20_SESSION_T *pSession, 
		            INT8U *Address,INT8U Channel, 
		            int timeout);

uint16 LIBIPMI_HL_GetIPv4Address(IPMI20_SESSION_T *pSession,
		            INT8U *Address,INT8U Channel, 
		            int timeout);

uint16 LIBIPMI_HL_SetIPv6Address(IPMI20_SESSION_T *pSession,
		            INT8U *Address,INT8U Channel,
		            int timeout);

uint16 LIBIPMI_HL_GetIPv6Address(IPMI20_SESSION_T *pSession,
		            INT8U *Address,INT8U Channel,
		            int timeout);

uint16 LIBIPMI_HL_SetIPv4NetMask(IPMI20_SESSION_T *pSession,
		            INT8U *Mask,INT8U Channel,
		            int timeout);

uint16 LIBIPMI_HL_GetIPv4NetMask(IPMI20_SESSION_T *pSession, 
		            INT8U *Mask,INT8U Channel,
		            int timeout);

uint16 LIBIPMI_HL_SetIPv6Prefix(IPMI20_SESSION_T *pSession,
		            INT8U Prefix,INT8U Channel, 
		            int timeout);

uint16 LIBIPMI_HL_GetIPv6Prefix(IPMI20_SESSION_T *pSession,
		            INT8U *Prefix,INT8U Channel,
		            int timeout);

uint16 LIBIPMI_HL_SetIPv4Gateway(IPMI20_SESSION_T *pSession,
		            INT8U *Gateway,INT8U Channel,
		            int timeout);

uint16 LIBIPMI_HL_GetIPv4Gateway(IPMI20_SESSION_T *pSession,
		            INT8U *Gateway,INT8U Channel,
		            int timeout);

uint16 LIBIPMI_HL_SetIPv6Gateway(IPMI20_SESSION_T *pSession, 
		            INT8U *Gateway,INT8U Channel,
		            int timeout);

uint16 LIBIPMI_HL_GetIPv6Gateway(IPMI20_SESSION_T *pSession, 
		            INT8U *Gateway,INT8U Channel, 
		            int timeout);

uint16 LIBIPMI_HL_SetIPv6Enable(IPMI20_SESSION_T *pSession, 
		            INT8U Enable,INT8U Channel, 
		            int timeout);

uint16 LIBIPMI_HL_GetIPv6Enable(IPMI20_SESSION_T *pSession,
		            INT8U *Enable,INT8U Channel, 
		            int timeout);

uint16 LIBIPMI_HL_GetIPv4NetworkCfg(IPMI20_SESSION_T *pSession, 
					INT8U *Source,
					INT8U *Address,
					INT8U *Mask,
					INT8U *Gateway,
					INT8U Channel,
					int timeout);

uint16 LIBIPMI_HL_SetIPv4NetworkCfg(IPMI20_SESSION_T *pSession,
                                        INT8U *Address,
                                        INT8U *Mask,
                                        INT8U *Gateway,
                                        INT8U Channel,
                                        int timeout);

uint16 LIBIPMI_HL_GetIPv6NetworkCfg(IPMI20_SESSION_T *pSession, 
					INT8U *Source,
					INT8U *Address,
					INT8U *Prefix,
					INT8U *Gateway,
					INT8U Channel,
					int timeout);

uint16 LIBIPMI_HL_SetIPv6NetworkCfg(IPMI20_SESSION_T *pSession,
                                        INT8U *Address,
                                        INT8U Prefix,
                                        INT8U *Gateway,
                                        INT8U Channel,
                                        int timeout);

/********************SERIAL PORT RELATED************************************/


#define SOL_SERIAL_PORT 0x01
#define SERIAL_MODEM_PORT 0x02
#define DEFAULT_TIMEOUT 2
#pragma pack(1)
typedef struct
{
    GetSOLConfigRes_T Res;
    INT8U   BaudRate;

}  Wrapper_GetSOLConfigRes_T;



typedef struct
{
    SetSOLConfigReq_T Req;
    INT8U   BaudRate;

}  Wrapper_SetSOLConfigReq_T;

typedef struct
{

    struct
        {
        uint8   Reserved    : 4;
        uint8	DtrHangup   : 1;
        uint8	FlowControl : 2;
        
	
    } PACKED Control;

    struct 
    {
        uint8 BitRate	    : 4;
        uint8 Reserved	    : 4;
	

    }  PACKED BaudRate;

} PACKED Serial_Port_Settings_T;




typedef enum
{
    None_Parity,
    Odd_Parity,
    Even_Parity
} PARITY_TYPE;

typedef enum
{
    None_FlowControl,
    XonXoff_FlowControl,
    Hardware_FlowControl
} FLOW_CONTROL_TYPE;


typedef struct
{
    unsigned long BaudRate;
    unsigned char  DataBits;
    PARITY_TYPE Parity;
    unsigned char StopBits;
    FLOW_CONTROL_TYPE FlowControl;
    unsigned char DtrHangup;
    

} PACKED SERIAL_STRUCT;

typedef struct
{
    GetSerialModemConfigRes_T Res;
    Serial_Port_Settings_T serial;

}  PACKED Wrapper_GetSerialModemConfigRes_T;


typedef struct
{
    SetSerialModemConfigReq_T Req;
    Serial_Port_Settings_T serial;

}  Wrapper_SetSerialModemConfigReq_T;

#pragma pack()

/*---------- LIBIPMI Higher level routines -----------*/

uint16 IPMICMD_GetSerialCfg_SOL_Advanced( IPMI20_SESSION_T *pSession, INT8U* char_acc_interval,INT8U* char_send_threshold,int timeout );
uint16 IPMICMD_SetSerialCfg_SOL_Advanced( IPMI20_SESSION_T *pSession, INT8U char_acc_interval,INT8U char_send_threshold,int timeout );

uint16 LIBIPMI_HL_GetSerialCfg_Messaging(IPMI20_SESSION_T *pSession, \
					 SERIAL_STRUCT *serial, int timeout );

uint16 LIBIPMI_HL_SetSerialCfg_Messaging(IPMI20_SESSION_T *pSession,  \
					 SERIAL_STRUCT *serial, int timeout);

uint16 LIBIPMI_HL_GetSerialCfg_SOL(IPMI20_SESSION_T *pSession, \
					 SERIAL_STRUCT *serial, INT8U* Enabled,int timeout );

uint16 LIBIPMI_HL_SetSerialCfg_SOL(IPMI20_SESSION_T *pSession, \
                                   SERIAL_STRUCT *serial, INT8U Enabled,int timeout);


/** 
 * \breif Sets the Serial Port configurations of the serial ports, SOL, Modem/Serial 
 * 
 * @param pSession 
 * @param type SOL or Modem/Serial
 * @param serial 
 * @param timeout 
 * 
 * @return -1 on error 0 on success.
 */
uint16	LIBIPMI_HL_Generic_SetSerialConfig( IPMI20_SESSION_T *pSession, int type, \
						    SERIAL_STRUCT *serial, int timeout );

/** 
 * \brief Gets the Serial Port Configuration from the BMC.
 * 
 * @param pSession 
 * @param type SOL or Modem/Serial
 * @param serial 
 * @param timeout 
 * 
 * @return 
 */
uint16	LIBIPMI_HL_Generic_GetSerialConfig( IPMI20_SESSION_T *pSession, int type, \
					    SERIAL_STRUCT *serial, int timeout );

/********************SERIAL PORT RELATED************************************/

#ifdef  __cplusplus
}
#endif

#endif


