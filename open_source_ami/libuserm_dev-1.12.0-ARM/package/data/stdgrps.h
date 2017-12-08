/***********************************************************************************
This file contains standard groups for a specific product
any extensions are performed here
Extend as neccessary for your product with product ifdefs
 ***********************************************************************************/

#ifndef __STDGRPS_H__
#define __STDGRPS_H__

/************************************Standard Group Names***********************************/

#define RAC_GRP_NAME_ROOT   "root"
#define RAC_GRP_NAME_ADMIN  RAC_GRP_NAME_ROOT
#define RAC_GRP_NAME_WEB    "web"
#define RAC_GRP_NAME_VIDEO  "video"
#define RAC_GRP_NAME_CURI   "curi"
#define RAC_GRP_NAME_SSH    "ssh"
#define RAC_GRP_NAME_SNMP   "snmp"
#define RAC_GRP_NAME_VMEDIA "vmedia"
#define RAC_GRP_NAME_RESERVED       "reserved"

#define PRIV_LAN_OEM			0x005
#define PRIV_LAN_ADMIN			0x004
#define PRIV_LAN_OPERATOR		0x003
#define PRIV_LAN_USER			0x002
#define PRIV_LAN_CALLBACK		0x001
#define PRIV_LAN_NOACCESS		0x00F

#define PRIV_UNKNOWN			0x000

#define RAC_GRP_NAME_RO(grpname)  grpname "R"
#define RAC_GRP_NAME_RW(grpname) grpname "RW"

/*******************************************************************************************/

typedef enum _tag_stdgrplist
{
    RAC_GRP_NUM_BEGIN = 0,
    RAC_GRP_NUM_ADMIN,
    RAC_GRP_NUM_WEB,
    RAC_GRP_NUM_VIDEO,
    RAC_GRP_NUM_CURI,
    RAC_GRP_NUM_SSH,
    RAC_GRP_NUM_SNMP,
    RAC_GRP_NUM_VMEDIA,

    //insert standard groups here and not below end

    RAC_GRP_NUM_END //special enum to indicate end of groups
}
STD_GRP_ENUM;




#endif
