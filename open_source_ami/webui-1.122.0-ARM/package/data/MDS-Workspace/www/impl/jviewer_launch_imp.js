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

function doInit()
{
	var externalIP = "";		//External Client IP
	var jnlpName;		//JNLP name to be launched
	var jnlpString;		//JNLP string to be used by client JNLP
	var jnlpURL;		//JNLP url with arguments to be launched
	var videoFile;		//Video File name to be play or download
	var windowURL;		//Window URL with full path

	//externalIP = window.location.href.split("/");
	jnlpString = eExt.parseURLvars("JNLPSTR");
	jnlpName = eExt.parseURLvars("JNLPNAME");
	windowURL = window.location.href.split("/");
	windowURL = windowURL[2];

	if (windowURL.indexOf("]") != -1) {
		externalIP = windowURL.split("]");
		externalIP = externalIP[0] + "]";
	} else {
		externalIP = windowURL.split(":");
		externalIP = externalIP[0];
	}
	jnlpURL = jnlpName + "?EXTRNIP=" + externalIP + "&JNLPSTR=" + jnlpString;
	if (jnlpString.indexOf("PlayVideo") != -1 || 
		jnlpString.indexOf("SaveVideo") != -1) {
		videoFile = eExt.parseURLvars("FILENAME");
		jnlpURL += "&VIDEOFILE=" + videoFile;
	}
	window.location.href = jnlpURL;
	setTimeout(function(){window.close()},20000);
}
