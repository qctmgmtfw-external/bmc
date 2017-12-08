//;*****************************************************************;
//;*****************************************************************;
//;**                                                             **;
//;**     (C) COPYRIGHT American Megatrends Inc. 2008-2009        **;
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

//----------------------------------------------------------------//
// eLang language support library
//----------------------------------------------------------------//

var eLang = {}; 				// declare empty object

//----------------------------------------------------------------//
// getString
// optBit - added by chandrasekarr@amiindia.co.in
// purpose - For multidimensional declaration of strings.
eLang.getString = function(widget, token)
{
	var optBit = arguments[2];



	if (token=="")
		return " ";
	else if (widget=="")
		return "DEVERROR: String class not specified";
	// Cascade to try and find the string, which could be included either in the
	// frame/page that has loaded this copy of eLang, or it may be in the topmost
	// frameset.
	// [BrandonB] 7/2/2007

	// First look for string group
	if (eLang[widget+"_strings"]!=undefined)
		{
		//Optional bit check
		if(optBit!=undefined && eLang[widget+"_strings"][token][optBit]!=undefined)
		{
		return eLang[widget+"_strings"][token][optBit];
		}

	if (eLang[widget+"_strings"][token]!=undefined)
		{
		return eLang[widget+"_strings"][token];
		}
		}
	else
		{
		if (top.eLang[widget+"_strings"]!=undefined)
			{
			//Optional bit check
			if(optBit!=undefined && top.eLang[widget+"_strings"][token][optBit]!=undefined)
			return top.eLang[widget+"_strings"][token][optBit];

			if (top.eLang[widget+"_strings"][token]!=undefined)
				return top.eLang[widget+"_strings"][token];
			}
		else
			{
			if (eLang.global_strings!=undefined)
				{
				//Optional bit check
				if(optBit!=undefined && eLang.global_strings[token][optBit]!=undefined)
				return eLang.global_strings[token][optBit];

				if (eLang.global_strings[token]!=undefined)
					return eLang.global_strings[token];
				}
			}
		}
	return "DEVERROR: Cannot locate string eLang."+widget+"_strings["+token+"]"+(optBit!=undefined)?"["+optBit+"]":"";

}


eLang.loaded = true;
