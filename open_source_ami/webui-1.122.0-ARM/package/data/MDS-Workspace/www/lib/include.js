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

if([].indexOf==undefined)
{
	Array.prototype.indexOf = function(value)
	{
		var iPtr;
		for(iPtr=0;iPtr<this.length;iPtr++)
		{
			if(this[iPtr]==value)
			{
				break;
			}
		}
		if(iPtr==this.length) iPtr = -1;

		return iPtr;
	}
}

if([].deleteValue == undefined)
{
	Array.prototype.deleteValue = function(value)
	{
		for(var i=0; i<this.length; i++)
		{
			if(this[i]==value) break;
		}
		if(i!=this.length) this.splice(i,1);

	}
}

if([].max == undefined)
{
	Array.prototype.max = function()
	{
		var max = this[0];

		for(var i=0; i<this.length-1; i++)
		{
			if(max<this[i+1]) max=this[i+1];
		}
		return max;
	}
}

if([].nullLessLength == undefined)
{
	Array.prototype.nullLessLength = function()
	{
		var count=0;
		for(var i=0; i<this.length; i++)
		{
			if(this[i]==null || this[i]=="") continue;
			count++;
		}
		return count;
	}
}

if([].compareWith==undefined)
{
	Array.prototype.compareWith = function(value)
	{
		var iPtr;
		for(iPtr=0;iPtr<this.length;iPtr++)
		{
			if(this[iPtr]!=value[iPtr])
			{
				return false;
			}
		}
		return true;
	}
}

if(Date.getUTCTime == undefined)
{
	Date.prototype.getUTCTime = function()
	{
		return (this.getTime() + (this.getTimezoneOffset()*60*1000));
	}
}


var gLoadedModules = [];
var gLoadingQueue = [];


var ct_mll;

function loadModule(moduleName, path, language, fnpOnload)
{
	if(gLoadedModules.indexOf(moduleName)!=-1) return; //module loaded already


	var script = document.createElement('script');
	script.src = '../'+path+'/'+moduleName+'.js';
	script.language = language;

	var head = document.getElementsByTagName('head')[0];
	head.appendChild(script);

	gLoadingQueue.push({mName:moduleName,fn_ptr_onload:fnpOnload});

	moduleLoadListener();
}

function loadLibrary(moduleName, language, fnpOnload)
{
	loadModule(moduleName, 'lib', language, fnpOnload);
}
/*
gLoadingQueue = ['ui','bitset','eExt','eVal','menu','events','factory','xmit'];
for(var i=0; i<gLoadingQueue.length; i++)
{
	loadLibrary(gLoadingQueue[i],'javascript');
}

loadLibrary('ui','javascript');
loadLibrary('bitset','javascript');
loadLibrary('eExt','javascript');
loadLibrary('eVal','javascript');
loadLibrary('menu','javascript');
loadLibrary('events','javascript');
loadLibrary('factory','javascript');
loadLibrary('xmit','javascript');
*/
function moduleLoadListener()
{
	var allModsLoaded = true;
	try{
		for(var i=0;i<gLoadingQueue.length; i++)
		{
			var moduleName = eval(gLoadingQueue[i].mName);

			if(moduleName.loaded!=undefined && moduleName.loaded)
			{
				_debug("Loaded module:"+gLoadingQueue[i].mName);
				if(gLoadedModules.indexOf(gLoadingQueue[i].mName)<0)
				{
					gLoadedModules.push(gLoadingQueue[i].mName);
					allModsLoaded = true;
				}
				gLoadingQueue[i].fn_ptr_onload();
			}else
			{
				_debug("Loading Module:"+gLoadingQueue[i].mName);
				allModsLoaded = false;
			}
		}

		clearTimeout(ct_mll);
		if(!allModsLoaded)
		{
			ct_mll= setTimeout("moduleLoadListener();",200);
		}else
		{
			delete gLoadingQueue;
			gLoadingQueue = [];
		}

	}catch(e)
	{
		clearTimeout(ct_mll);
		ct_mll = setTimeout("moduleLoadListener();",200);
	}
}



var curfile = top.getCurFile(location);

if (location.pathname.indexOf("oem") != -1)	//OEM Feature
	top.gOemDir = "../../../";
else
	top.gOemDir = "../";

document.write("<script language='javascript' src='"+top.gOemDir+"lib/bitset.js'><\/script>");
document.write("<script language='javascript' src='"+top.gOemDir+"lib/eExt.js'><\/script>");
document.write("<script language='javascript' src='"+top.gOemDir+"lib/eVal.js'><\/script>");
document.write("<script language='javascript' src='"+top.gOemDir+"lib/events.js'><\/script>");
document.write("<script language='javascript' src='"+top.gOemDir+"lib/factory.js'><\/script>");
document.write("<script language='javascript' src='"+top.gOemDir+"lib/ui.js'><\/script>");
document.write("<script language='javascript' src='"+top.gOemDir+"lib/xmit.js'><\/script>");
document.write("<script language='javascript' src='"+top.gOemDir+"lib/common_fn.js'><\/script>");
document.write("<script language='javascript' src='"+top.gOemDir+"lib/eLang.js'><\/script>");
document.write("<script language='javascript' src='"+top.gOemDir+"str/"+top.gLangSetting+"/err_str.js'><\/script>");
document.write("<script language='javascript' src='../str/"+top.gLangSetting+"/common_str.js'><\/script>");
document.write("<script language='javascript' src='../impl/"+curfile+"_imp.js'><\/script>");
document.write("<script language='javascript' src='../str/"+top.gLangSetting+"/"+curfile+"_str.js'><\/script>");

//It must be called after eLang.js is loaded
if (location.pathname.indexOf("oem") != -1)	//OEM Feature
{
	document.write("<script language='javascript' src='"+top.gOemDir+"str/"+top.gLangSetting+"/common_str.js'><\/script>");
}
