//;*****************************************************************;
//;*****************************************************************;
//;**                                                             **;
//;**     (C) COPYRIGHT American Megatrends Inc. 2008-2010        **;
//;**                     ALL RIGHTS RESERVED                     **;
//;**                                                             **;
//;**  This computer software, including display screens and      **;
//;**  all related materials, are confidential and the            **;
//;**  exclusive property of American Megatrends, Inc.  They      **;
//;**  are available for limited use, but only pursuant to        **;
//;**  a written license agreement distributed with this          **;
//;**  computer software.  This computer software, including      **;
//;**  display screens and all related materials, shall not be    **;
//;**  copied, reproduced, published or distributed, in whole     **;
//;**  or in part, in any medium, by any means, for any           **;
//;**  purpose without the express written consent of American    **;
//;**  Megatrends, Inc.                                           **;
//;**                                                             **;
//;**                                                             **;
//;**                American Megatrends, Inc.                    **;
//;**           5555 Oakbook Parkway, Building 200                **;
//;**     Norcross,  Georgia - 30071, USA. Phone-(770)-246-8600.  **;
//;**                                                             **;
//;*****************************************************************;
//;*****************************************************************;

// File Name  : fru_info
// Brief      : 
// Author Name:
var FRU_DATA;

function doInit()
{
	exposeElms(['_FRUDeviceID',
				'_FRUDeviceName',
				'_CI_ChassisInfoAreaFormatVersion',
				'_CI_ChassisType',
				'_CI_ChassisPartNum',
				'_CI_ChassisSerialNum',
				'_CI_CustomFields',
				'_BI_BoardInfoAreaFormatVersion',
				'_BI_Language',
				'_BI_MfgDateTime',
				'_BI_BoardMfr',
				'_BI_BoardProductName',
				'_BI_BoardSerialNum',
				'_BI_BoardPartNum',
				'_BI_FRUFileID',
				'_BI_CustomFields',
				'_PI_ProductInfoAreaFormatVersion',
				'_PI_Language',
				'_PI_MfrName',
				'_PI_ProductName',
				'_PI_ProductPartNum',
				'_PI_ProductVersion',
				'_PI_ProductSerialNum',
				'_PI_AssetTag',
				'_PI_FRUFileID',
				'_PI_CustomFields']);

	FRUDeviceID.onchange = reloadFRUInfo;
	_begin();
}

function _begin()
{
	getFRUInfo();
}

function getFRUInfo()
{
	xmit.get({url:'/rpc/getfruinfo.asp',onrcv:getFRUInfoRes,status:''});
}

function getFRUInfoRes(arg)
{
	if (arg.HAPI_STATUS != 0)
	{
		errstr = eLang.getString('common','STR_FRU_INFO_GETVAL')
		errstr += (eLang.getString('common','STR_IPMI_ERROR')+ GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	}
	else
	{
		FRU_DATA = WEBVAR_JSONVAR_HL_GETALLFRUINFO.WEBVAR_STRUCTNAME_HL_GETALLFRUINFO;
		for (i=0;i<FRU_DATA.length;i++)
			FRUDeviceID.add(new Option(FRU_DATA[i].FRUDeviceID,i), isIE?i:null);
		reloadFRUInfo();
	}
}

function reloadFRUInfo()
{
	i = FRUDeviceID.value;
	FRUDeviceName.innerHTML = replaceNULL(FRU_DATA[i].FRUDeviceName);

	//Chassis Information
	CI_ChassisInfoAreaFormatVersion.innerHTML = replaceNULL(FRU_DATA[i].CI_ChassisInfoAreaFormatVersion);
	CI_ChassisType.innerHTML = replaceNULL(FRU_DATA[i].CI_ChassisType);
	CI_ChassisPartNum.innerHTML = replaceNULL(FRU_DATA[i].CI_ChassisPartNum);
	CI_ChassisSerialNum.innerHTML = replaceNULL(FRU_DATA[i].CI_ChassisSerialNum);
	CI_CustomFields.innerHTML = replaceNULL(FRU_DATA[i].CI_CustomFields);

	//Board Information
	BI_BoardInfoAreaFormatVersion.innerHTML = replaceNULL(FRU_DATA[i].BI_BoardInfoAreaFormatVersion);
	BI_Language.innerHTML = replaceNULL(FRU_DATA[i].BI_Language);
	BI_MfgDateTime.innerHTML = replaceNULL(FRU_DATA[i].BI_MfgDateTime);
	BI_BoardMfr.innerHTML = replaceNULL(FRU_DATA[i].BI_BoardMfr);
	BI_BoardProductName.innerHTML = replaceNULL(FRU_DATA[i].BI_BoardProductName);
	BI_BoardSerialNum.innerHTML = replaceNULL(FRU_DATA[i].BI_BoardSerialNum);
	BI_BoardPartNum.innerHTML = replaceNULL(FRU_DATA[i].BI_BoardPartNum);
	BI_FRUFileID.innerHTML = replaceNULL(FRU_DATA[i].BI_FRUFileID);
	BI_CustomFields.innerHTML = replaceNULL(FRU_DATA[i].BI_CustomFields);

	//Product Information
	PI_ProductInfoAreaFormatVersion.innerHTML = replaceNULL(FRU_DATA[i].PI_ProductInfoAreaFormatVersion);
	PI_Language.innerHTML = replaceNULL(FRU_DATA[i].PI_Language);
	PI_MfrName.innerHTML = replaceNULL(FRU_DATA[i].PI_MfrName);
	PI_ProductName.innerHTML = replaceNULL(FRU_DATA[i].PI_ProductName);
	PI_ProductPartNum.innerHTML = replaceNULL(FRU_DATA[i].PI_ProductPartNum);
	PI_ProductVersion.innerHTML = replaceNULL(FRU_DATA[i].PI_ProductVersion);
	PI_ProductSerialNum.innerHTML = replaceNULL(FRU_DATA[i].PI_ProductSerialNum);
	PI_AssetTag.innerHTML = replaceNULL(FRU_DATA[i].PI_AssetTag);
	PI_FRUFileID.innerHTML = replaceNULL(FRU_DATA[i].PI_FRUFileID);
	PI_CustomFields.innerHTML = replaceNULL(FRU_DATA[i].PI_CustomFields);
}

function replaceNULL(originalStr, replaceStr)
{
	replaceStr = replaceStr ? replaceStr : "";
	return (originalStr == "(null)" ? replaceStr : originalStr);
}