

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include "icc_what.h"

#ifdef ICC_OS_LINUX
#include <termios.h>
#include <unistd.h>
#endif


#include "IPMIDefs.h"
#include "libipmi_session.h"
#include "libipmi_errorcodes.h"
#include "libipmi_network.h"
#include "IPMI_LANConfig.h"
#include "IPMI_DeviceConfig.h"
#include "libipmi_XportDevice.h"
#include "IPMI_FRU.h"
#include "libipmi_fru.h"
#include <time.h>
#include "IPMI_AMIDevice.h"




#include "libipmi_IPM.h"
#include <dbgout.h>

#ifdef ICC_OS_LINUX
    /* for linux and PPC implementations */
    #define spstrcasecmp    strcasecmp
    #define spstrncasecmp   strncasecmp
#else
    /* for Windows implementations */
    #define spstrcasecmp    stricmp
    #define spstrncasecmp   strnicmp
#endif


#define INVALID_OP			( 0x00 )
#define SETSSHKEY_OP		( 0x01 )
#define DELSSHKEY_OP		( 0x02 )
#define GETDEVICEID_OP		( 0x03 )
#define GETUSERNAME_OP		( 0x04 )
#define GETALLSELENTRIES_OP	( 0x05 )

#define VERSION_NUMBER		"1.0"


#define PARAM_IPADDRSOURCE      4
#define PARAM_MAC               5
#define PARAM_IP                3
#define PARAM_MASK              6
#define PARAM_GATEWAY           12

/*Options*/
#define LANCONFIG				1
#define FRUDETAILS				2



uint8 byAuthType = 0x00;
uint8 byMedium = 0;
uint8 bySelection=0;
// following operation variable will be useful if we want to
// support many other commands using this same application
int operation = INVALID_OP;
int IpEntered = 0;
char device[32];
char filename[256];



int SetLANConfig(IPMI20_SESSION_T* pSession);

void printerror(unsigned short wErr);
extern int GetPasswordInput(char* pwd);

void printerror(unsigned short wErr)
{
	if(wErr != LIBIPMI_E_SUCCESS)
	{
		if( IS_MEDIUM_ERROR(wErr) )
			printf("Medium Error ");
		else
			printf("IPMI Error ");
		printf("%x\n", GET_ERROR_CODE(wErr) );
	}
}

void display_usage( void )
{
    printf("-------------------------------------------------\n");
    printf("IPMIApp application \n");
    printf("-------------------------------------------------\n");
    printf("Copyright 2007 American Megatrends Inc.\n\n");
    printf( "Usage:\n" );
    printf( "To Set/Get LAN config\n");
    printf( "nwusb --nw --ip <Device's IP Addr>  or --usb -lan\n" );
    printf( "To List the FRU details\n" );
    printf( "nwusb --nw --ip <Device's IP Addr>  or --usb -fru\n" );
    
    printf( "\n" );

    printf( "Medium options:\n");
    printf( "--nw		: Network Medium\n");
    printf( "--usb		: Local USB Medium\n\n");

    return;
}

void parse_args(int argc, char **argv)
{
    int i;    
    if( argc < 5 )
    {
        display_usage();
        exit( EXIT_FAILURE );
    }

    printf("-------------------------------------------------\n");
    printf("IPMIApp Application (Version %s)\n",VERSION_NUMBER);
    printf("-------------------------------------------------\n");
    printf("Copyright 2007 American Megatrends Inc.\n\n");

    
    for( i = 1; i < argc; i++ )
    {
        if( spstrcasecmp( argv[ i ], "--usb" ) == 0 )
        {
            if(byMedium !=0)
            {
                
                display_usage();
                exit( EXIT_FAILURE );
            }
            IpEntered = 0x01;		
            byMedium = USB_MEDIUM;
            break;
        }
     
        if(spstrcasecmp(argv[ i ], "--nw") == 0)
        {
            byMedium = NETWORK_MEDIUM;
            continue;
        }
        if(spstrcasecmp(argv[ i ], "-lan") == 0)
        {
            bySelection = LANCONFIG ;
            continue;
        }
        else if(spstrcasecmp(argv[ i ], "-fru") == 0)
        {
            bySelection = FRUDETAILS;
            continue;
        }
        if( spstrcasecmp( argv[ i ], "--ip" ) == 0 )
        {
            IpEntered = 0x01;
            if(byMedium != NETWORK_MEDIUM)
            {
                display_usage();
                exit( EXIT_FAILURE );
            }
            if( i + 1 < argc )
            {
                if( strlen( argv[ i + 1 ] ) < sizeof( device ) )
                {
                    strcpy( device, argv[ i + 1 ] );
                    i++;
                }
                else
                {
                    printf( "Network address string is too long.\n" );
                    exit( EXIT_FAILURE );
                }
            }
            else
            {
                printf( "Please supply the network address of the device.\n" );
                exit( EXIT_FAILURE );
            }
            continue;
        }
        else
        {
            printf( "Please supply the network address of the device.\n" );
            exit( EXIT_FAILURE );
        } 
        printf( "Unknown option %s\n", argv[ i ] );
        exit( EXIT_FAILURE );
    }
}


int main(int argc, char **argv)
{
    int uid = 0,CorrectFw=0;
    char username[64];
    char pass[64];
    GetDevIDRes_T		GetDevIDRes;
    IPMI20_SESSION_T hSession;	
    uint16 wRet = 0,i=0;
    uint8 byPrivLevel = PRIV_LEVEL_ADMIN;
    char *pUser = NULL;
    char *pPass = NULL;
    GetLanConfigReq_T pGetLanReq;
    GetLanConfigRes_T pGetLanRes;
    unsigned char str[64];	
    
    time_t tval;
    static const uint64  secs_from_1970_1996 = 820450800;
    static const char * chassis_type[] = {
     "Unspecified", "Other", "Unknown",
     "Desktop", "Low Profile Desktop", "Pizza Box",
     "Mini Tower", "Tower",
     "Portable", "LapTop", "Notebook", "Hand Held",
     "Docking Station", "All in One", "Sub Notebook",
     "Space-saving", "Lunch Box", "Main Server Chassis",
     "Expansion Chassis", "SubChassis", "Bus Expansion Chassis",
     "Peripheral Chassis", "RAID Chassis", "Rack Mount Chassis"};
    int tempval,j=0,l=0,opt=0;
    AMIGetFruDetailReq_T GetFruDetReq;
    AMIGetFruDetailRes_T GetFruDetRes; 
    uint32 dwResLen=0; 
    uint32 fru_size;
    FRUData_T fru_data_app;
    uint32 Ret = 0;    

    memset((void*)&hSession, 0, sizeof(IPMI20_SESSION_T) );
    strcpy(username,"\0");
    strcpy(pass,"\0");
    strcpy(device,"127.0.0.1");

    parse_args(argc,argv);
    if(IpEntered != 0x01)
    {
        printf("Please Enter the IP Address \n");
        exit( EXIT_FAILURE );	 
    }

    if (byMedium != USB_MEDIUM)
    {            
        printf("Please enter login information:\n");
        printf("User	 : ");
        scanf("%s",username);
        GetPasswordInput(pass);

        pUser = username;
        pPass = pass;
    }
    if( byMedium == NETWORK_MEDIUM )
    {
        byAuthType = AUTHTYPE_RMCP_PLUS_FORMAT;
        printf( "\nCreating IPMI session via network with address %s...", device );
    }	
    else
    {
        byAuthType = 0;
        pUser = NULL;
        pPass = NULL;
        printf( "\nCreating IPMI session via USB...");
    }

    wRet = LIBIPMI_Create_IPMI20_Session( &hSession, device, 623, pUser, pPass,
                                          byAuthType, 0, 0, &byPrivLevel,
                                          byMedium, 0xc2, 0x20, NULL, 0, 3000);
    if( wRet != LIBIPMI_E_SUCCESS )
    {
        printf( "\nError while establishing the session\n" );
        printerror( wRet );
        exit( EXIT_FAILURE );
    }
    else
        printf("Done\n");

    if(byMedium == NETWORK_MEDIUM)
    {
        if(bySelection==LANCONFIG)
        {         
            pGetLanReq.ChannelNum = 0x01;
            pGetLanReq.ParameterSelect = 0x03;
            pGetLanReq.SetSelect = 0x00;
            pGetLanReq.BlockSelect = 0x00;

            wRet = IPMICMD_GetLANConfig( &hSession,&pGetLanReq,&pGetLanRes,5000);
            if(wRet != 0)
            {
                printf(" Error in Getting Lan Config Parameters,error code %d \n",wRet);
                return wRet;			
            }	 
            printf("\n IPAddr     %d.%d.%d.%d \n",pGetLanRes.ConfigData.IPAddr[0],pGetLanRes.ConfigData.IPAddr[1],pGetLanRes.ConfigData.IPAddr[2],pGetLanRes.ConfigData.IPAddr[3]);

            pGetLanReq.ParameterSelect = 6;
            wRet = IPMICMD_GetLANConfig( &hSession,&pGetLanReq,&pGetLanRes,5000);
            if(wRet != 0)
            {
                printf(" Error in Getting Lan Config Parameters,error code %d \n",wRet);
                return wRet;			
            }
            printf("Subnetmask  %d.%d.%d.%d \n",pGetLanRes.ConfigData.SubNetMask[0],pGetLanRes.ConfigData.SubNetMask[1],pGetLanRes.ConfigData.SubNetMask[2],pGetLanRes.ConfigData.SubNetMask[3]);

            pGetLanReq.ParameterSelect = 0x05;
            wRet = IPMICMD_GetLANConfig( &hSession,&pGetLanReq,&pGetLanRes,5000);
            if(wRet != 0)
            {
                printf(" Error in Getting Lan Config Parameters,error code %d \n",wRet);
                return wRet;
            }
            printf("MacAddress  %2.2X:%2.2X:%2.2X:",pGetLanRes.ConfigData.MACAddr[0],pGetLanRes.ConfigData.MACAddr[1],pGetLanRes.ConfigData.MACAddr[2]);
            printf("%2.2X:%2.2X:%2.2X \n",pGetLanRes.ConfigData.MACAddr[3],pGetLanRes.ConfigData.MACAddr[4],pGetLanRes.ConfigData.MACAddr[5]);

            pGetLanReq.ParameterSelect = 12;
            wRet = IPMICMD_GetLANConfig( &hSession,&pGetLanReq,&pGetLanRes,5000);
            if(wRet != 0)
            {
                printf(" Error in Getting Lan Config Parameters,error code %d \n",wRet);
                return wRet;		
            }
            printf("Gateway IP  %d.%d.",pGetLanRes.ConfigData.DefaultGatewayIPAddr[0],pGetLanRes.ConfigData.DefaultGatewayIPAddr[1]);
            printf("%d.%d \n",pGetLanRes.ConfigData.DefaultGatewayIPAddr[2],pGetLanRes.ConfigData.DefaultGatewayIPAddr[3]);


            printf("\nDo you want to change network configuration? (yes/no) : ");
            scanf("%s",str);
            if(!strcmp(str,"yes"))
            {
                if(SetLANConfig(&hSession))
                printf("Error setting LAN configuration\n");
            }
            else if(strcmp(str,"no"))
                printf("Invalid option\n");
        }
        else if (bySelection==FRUDETAILS)
        {
            GetFruDetReq.FruReq = 0xFF;
            dwResLen = sizeof(AMIGetFruDetailReq_T);
            wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(&hSession,0 ,(0x32 <<2), 0x80,(uint8*)&GetFruDetReq, sizeof(AMIGetFruDetailReq_T),(uint8 *)&GetFruDetRes, &dwResLen,5000);
            if(wRet != 0)
            {
                printf(" Error in Getting Fru Details,error code %d \n",wRet);
                return wRet;
            }
            l=GetFruDetRes.TotalFru;
            for(j=0;j<l;j++)
            {
                GetFruDetReq.FruReq = j;
                dwResLen = sizeof(AMIGetFruDetailReq_T);
                wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(&hSession,0 ,(0x32 <<2), 0x80,(uint8*)&GetFruDetReq, sizeof(AMIGetFruDetailReq_T),(uint8 *)&GetFruDetRes, &dwResLen,5000);
                if(wRet != 0)
                {
                    printf("\nError in Getting Fru Details,error code %d \n",wRet);
                    return wRet;
                }
                Ret = LIBIPMI_HL_GetFRUData(&hSession,GetFruDetRes.DeviceNo,&fru_size,&fru_data_app,3000);
                if(Ret!=0)
                {
                    printf("\nDevice not present\n");
                    continue;
                }
                printf("\n");
                if(fru_data_app.CommonHeader.ChassisInfoAreaStartOffset != 0)
                {
                    tempval=0;
                    printf("\nChassisType                   : %s",(fru_data_app.ChassisInfo.ChassisType));
                    printf("\nChassis part number           : %s",(fru_data_app.ChassisInfo.ChassisPartNum));
                    printf("\nChassis serial number         : %s",(fru_data_app.ChassisInfo.ChassisSerialNum));
                    tempval=*(fru_data_app.ChassisInfo.Total_Chassis_CustomField);
                    for(i=0;i<tempval;i++)
                        printf("\nChassis Extra                 : %s",(fru_data_app.ChassisInfo.CustomFields[i]));
                }	
                if(fru_data_app.CommonHeader.BoardInfoAreaStartOffset != 0)
                {
                    tempval=0;
                    tval=((fru_data_app.BoardInfo.MfgDateTime[2] << 16) + (fru_data_app.BoardInfo.MfgDateTime[1] << 8) + (fru_data_app.BoardInfo.MfgDateTime[0]));
                    tval=tval * 60;
                    tval=tval + secs_from_1970_1996;
                    printf("\nBoard Mfg Date                : %s", asctime(localtime(&tval)));	
                    printf("Board Mfg                     : %s",(fru_data_app.BoardInfo.BoardMfr));
                    printf("\nBoard Product                 : %s",(fru_data_app.BoardInfo.BoardProductName));
                    printf("\nBoard Serial                  : %s",(fru_data_app.BoardInfo.BoardSerialNum ));
                    printf("\nBoard PartNum                 : %s",(fru_data_app.BoardInfo.BoardPartNum ));
                    printf("\nBoard FruFileID               : %s",(fru_data_app.BoardInfo.FRUFileID));
                    tempval=*(fru_data_app.BoardInfo.Total_Board_CustomField);
                    for(i=0;i<tempval;i++)
                        printf("\nBoard Extra                   : %s",(fru_data_app.BoardInfo.CustomFields[i]));
                }
                if(fru_data_app.CommonHeader.ProductInfoAreaStartOffset != 0)
                {
                    tempval=0;
                    printf("\nProduct Manufacturer          : %s",(fru_data_app.ProductInfo.MfrName ));
                    printf("\nProduct Name                  : %s",(fru_data_app.ProductInfo.ProductName));
                    printf("\nProduct Part Number           : %s",(fru_data_app.ProductInfo.ProductPartNum));
                    printf("\nProduct Version               : %s",(fru_data_app.ProductInfo.ProductVersion ));
                    printf("\nProduct Serial                : %s",(fru_data_app.ProductInfo.ProductSerialNum));
                    printf("\nProduct Asset Tag             : %s",(fru_data_app.ProductInfo.AssetTag));
                    printf("\nProduct FRUFieldId            : %s",(fru_data_app.ProductInfo.FRUFileID));
                    tempval=*(fru_data_app.ProductInfo.Total_Product_CustomField);
                    for(i=0;i<tempval;i++)
                        printf("\nProduct Extra                 : %s",(fru_data_app.ProductInfo.CustomFields[i]));
                }
            }
            printf("\n");	  
        }
    }
    else
    {
        wRet = IPMICMD_GetDeviceID( &hSession, &GetDevIDRes, 3000);
        if(wRet != 0)
        {
            printf("Could not communicate with BMC via IPMB when creating session error code %d!!\n",wRet);
            return wRet;
        }
        printf("\nResponse of GetDeviceId \n");
        printf("completion code     = %x\n",GetDevIDRes.CompletionCode);	
        printf("DeviceId            = %x\n",GetDevIDRes.DeviceID);
        printf("DevRevision         = %x\n",GetDevIDRes.DevRevision);
        printf("FirmwareRevision1   = %x\n",GetDevIDRes.FirmwareRevision1);
        printf("FirmwareRevision2   = %x\n",GetDevIDRes.FirmwareRevision2);
        printf("IPMIVersion         = %x\n",GetDevIDRes.IPMIVersion);
        printf("DevSupport          = %x\n",GetDevIDRes.DevSupport);
        printf("MfgID               = %x%x%x\n",GetDevIDRes.MfgID[0],GetDevIDRes.MfgID[1],GetDevIDRes.MfgID[2]);
        printf("productId           = %d\n",GetDevIDRes.ProdID);
        printf("AuxFirmwareRevision = %lx\n",GetDevIDRes.AuxFirmwareRevision);
     
    }	
    return 0;
}


void ConvertIPstrToIPnum(unsigned char *str)
{
    char localStr[8] = {0,0,0,0,0,0,0,0};
	int i = 0;

	sscanf(str,"%u.%u.%u.%u",&localStr[0],&localStr[1],&localStr[2],&localStr[3]);
	localStr[4] = 0;
	for(i = 0; i < 4; i++)
		str[i] = (unsigned char)localStr[i];
	str[i] = 0;
}

int SetLANConfig(IPMI20_SESSION_T* pSession)
{
	int wRet;
	SetLanConfigReq_T SetLANReqBuff;
	SetLanConfigRes_T SetLANResBuff;
	unsigned int ReqDataLen = 0;
	int IPAddrSource = 0;
	char IP[64];
	char Mask[64];
	char Gateway[64];

	printf("Network Configuration:\n");
	printf("----------------------\n");
	printf("Static(1) or DHCP(2): ");
	scanf("%d",&IPAddrSource);
	if(IPAddrSource == 2)
	{
		goto set_dhcp;
	}
	printf("IP: ");	scanf("%s",IP);
	printf("Mask: "); scanf("%s",Mask);
	printf("Gateway: "); scanf("%s",Gateway);


set_dhcp:
	printf("\nSetting Network coniguration...");
	SetLANReqBuff.ChannelNum = 1;
	SetLANReqBuff.ParameterSelect = PARAM_IPADDRSOURCE;
	SetLANReqBuff.ConfigData.IPAddrSrc = (unsigned char) IPAddrSource;
	//printf("IP Addr Source = %d\n",SetLANReqBuff.ConfigData.IPAddrSrc);

	ReqDataLen = 2 + sizeof(SetLANReqBuff.ConfigData.IPAddrSrc);
	wRet = IPMICMD_SetLANConfig(pSession,
					&SetLANReqBuff,
					ReqDataLen,
					&SetLANResBuff, 15);
	if(wRet != 0)
	{
		printf("Error setting LAN IP Addr Source %d\n",wRet);
		goto error_out;
	}
	if(IPAddrSource == 2)
	{
		/* We don't need to write anything else */
		goto error_out;
	}

	ConvertIPstrToIPnum(IP);

	SetLANReqBuff.ParameterSelect = PARAM_IP;
	memcpy(SetLANReqBuff.ConfigData.IPAddr,IP,4);
	ReqDataLen = 2 + sizeof(SetLANReqBuff.ConfigData.IPAddr);
	wRet = IPMICMD_SetLANConfig(pSession,
					&SetLANReqBuff,
					ReqDataLen,
					&SetLANResBuff, 5);
	if(wRet != 0)
	{
		printf("Error setting LAN IP.\n");
		goto error_out;
	}

	ConvertIPstrToIPnum(Mask);

	SetLANReqBuff.ParameterSelect = PARAM_MASK;
	memcpy(SetLANReqBuff.ConfigData.SubNetMask,Mask,4);
	ReqDataLen = 2 + sizeof(SetLANReqBuff.ConfigData.SubNetMask);
	wRet = IPMICMD_SetLANConfig(pSession,
					&SetLANReqBuff,
					ReqDataLen,
					&SetLANResBuff, 5);
	if(wRet != 0)
	{
		printf("Error setting LAN Mask.\n");
		goto error_out;
	}

	ConvertIPstrToIPnum(Gateway);
	SetLANReqBuff.ParameterSelect = PARAM_GATEWAY;
	memcpy(SetLANReqBuff.ConfigData.DefaultGatewayIPAddr,Gateway,4);
	ReqDataLen = 2 + sizeof(SetLANReqBuff.ConfigData.DefaultGatewayIPAddr);
	wRet = IPMICMD_SetLANConfig(pSession,
					&SetLANReqBuff,
					ReqDataLen,
					&SetLANResBuff, 5);
	if(wRet != 0)
	{
		printf("Error setting LAN Gateway.\n");
		goto error_out;
	}

	

error_out:
    printf("Done\n");
    return wRet;
}



