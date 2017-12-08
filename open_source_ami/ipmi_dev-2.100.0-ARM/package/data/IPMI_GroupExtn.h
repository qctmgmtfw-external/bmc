/****************************************************************
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2008-2009, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        Suite 200, 5555 oakbrook pkwy, Norcross             **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 ****************************************************************
 *****************************************************************
 *
 * IPMI_GroupExtn.h
 * IPMI Group Extension Application Command numbers
 * 
 * Author: Rama Bisa <ramab@ami.com>
 *
 *****************************************************************/

/*** Group Extension (NetFn Pair 0x2C/0x2D) Specific Commands ***/

/*** SSI Compute Blade Specific Commands ***/
#ifdef CONFIG_SPX_FEATURE_SSICOMPUTEBLADE_SUPPORT
#define CMD_SSICB_GET_COMPUTE_BLADE_PROPERTIES      0x00
#define CMD_SSICB_GET_ADDR_INFO                     0x01
#define CMD_SSICB_PLATFORM_EVENT_MESSAGE            0x02
#define CMD_SSICB_MODULE_BMI_CONTROL                0x03
#define CMD_SSICB_MODULE_PAYLOAD_CONTROL            0x04
#define CMD_SSICB_SET_SYSTEM_EVENT_LOG_POLICY       0x05
#define CMD_SSICB_SET_MODULE_ACTIVATION_POLICY      0x0A
#define CMD_SSICB_GET_MODULE_ACTIVATION_POLICY      0x0B
#define CMD_SSICB_SET_MODULE_ACTIVATION             0x0C
#define CMD_SSICB_SET_POWER_LEVEL                   0x11
#define CMD_SSICB_GET_POWER_LEVEL                   0x12
#define CMD_SSICB_RENEGOTIATE_POWER                 0x13
#define CMD_SSICB_GET_SERVICE_INFO                  0x16
#define CMD_SSICB_GET_APPLET_PACKAGE_URI            0x17
#define CMD_SSICB_GET_SERVICE_ENABLE_STATE          0x18
#define CMD_SSICB_SET_SERVICE_ENABLE_STATE          0x19
#define CMD_SSICB_SET_SERVICE_TICKET                0x20
#define CMD_SSICB_STOP_SERVICE_SESSION              0x21
#endif

/*** DCMI Specific Commands ***/
#define CMD_GET_DCMI_CAPABILITY_INFO                0x01
#define CMD_GET_POWER_READING                       0x02
#define CMD_GET_POWER_LIMIT                         0x03
#define CMD_SET_POWER_LIMIT                         0x04
#define CMD_ACTIVATE_POWER_LIMIT                    0x05
#define CMD_GET_ASSET_TAG                           0x06
#define CMD_GET_DCMI_SENSOR_INFO                    0x07
#define CMD_SET_ASSET_TAG                           0x08
#define CMD_GET_MANAGEMENT_CONTROLLER_ID_STRING     0x09
#define CMD_SET_MANAGEMENT_CONTROLLER_ID_STRING     0x0A


