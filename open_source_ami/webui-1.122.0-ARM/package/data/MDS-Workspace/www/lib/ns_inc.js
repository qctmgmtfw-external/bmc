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

if(top==self)
{
        top.gLangSettingSet = true;
        top.gLangSetting = 'EN';
        top.getCurFile = function(lObj)
        {
                var fileAry = lObj.pathname.split('/');
                var file = fileAry[fileAry.length-1];

                return  file.split('.')[0];
        }
}

document.writeln("<script language='javascript' src='../lib/eLang.js'><\/script>");
document.writeln("<script language='javascript' src='../lib/events.js'><\/script>");
document.writeln("<script language='javascript' src='../lib/ui.js'><\/script>");
document.writeln("<script language='javascript' src='../lib/xmit.js'><\/script>");
var curfile = top.getCurFile(location);
document.writeln("<script language='javascript' src='../impl/"+curfile+"_imp.js'><\/script>");
document.writeln("<script language='javascript' src='../str/"+top.gLangSetting+"/"+curfile+"_str.js'><\/script>");
document.writeln("<script language='javascript' src='../str/"+top.gLangSetting+"/common_str.js'><\/script>");
