/********************************************
Helper function to build project based on
previously declared variables for the project
********************************************/
function Helper_BuildProject()
{

	var CmdString = "msdev " + ProjectDSWName[PRODUCT] + " /MAKE " + "\"" + ProjectName[PRODUCT] + "\"";
	var RebuildCmd = CmdString +  " /REBUILD ";


	WScript.Echo("ReBuilding all....");
	retval = gShell.Run(RebuildCmd,1,true);
	if(retval != 0)
	{
		WScript.Echo("Could not build project..return value is !!!!"+retval);
		WScript.Quit(retval);
	}
	else
	{
		WScript.echo("successful Release build");
	}
}




/********************************************
Helper function to Checkout a file based on
previously declared variables for the project
********************************************/
function Helper_CheckoutHelper(VSSPROJBASE,PD)
{
	VSSFULLPATH = VSSPROJBASE+"/"+PD.VSSLocation+PD.Filename;

	VSSVAR_SSEXE = "ss.exe";
	var CmdString = VSSVAR_SSEXE + " checkout " + VSSFULLPATH ;
	CmdString += " -G-";

	WScript.Echo("Cmd string is " + CmdString);
	
	Retval = gShell.Run(CmdString,1,true);

	return Retval;
}


/********************************************
Helper function to Set Current Project in VSS
previously declared variables for the project
********************************************/
function Helper_SetCurProjToDeliv(VSSPROJBASE,PD)
{
	VSSPROJNAME = VSSPROJBASE+"/"+PD.VSSLocation;
	VSSVAR_SSEXE = "ss.exe";
	var CmdString = VSSVAR_SSEXE + " CP " + VSSPROJNAME;

	Retval = gShell.Run(CmdString,1,true);
}


/********************************************
Helper function to checkin project files
previously declared variables for the project
********************************************/
function Helper_CheckinHelper(VSSPROJNAME,PD,COMMENT)
{
	Helper_SetCurProjToDeliv(VSSPROJNAME,PD);

	var topdir = gShell.CurrentDirectory;

	gShell.CurrentDirectory = PD.DiskLocation;

	VSSVAR_SSEXE = "ss.exe";
	var CmdString = VSSVAR_SSEXE + " checkin " + PD.Filename + " -C" + COMMENT + " -I-Y";  

	Retval = gShell.Run(CmdString,1,true);

	gShell.CurrentDirectory = topdir;

	return Retval;
}


/********************************************
Megsa Checkin function for all deliverables
previously declared variables for the project
********************************************/
function Helper_CheckInDeliverables(VSSPROJ,COMMENT)
{

	for(i=0;i<PD[PRODUCT].length;i++)
	{
		
		Retval = Helper_CheckoutHelper(VSSPROJ,PD[PRODUCT][i]);

		if(Retval != 0)
		{
			WScript.Echo("Error checking out deliverable..will quit");
			WScript.Quit(Retval);
		}
		
		Retval = Helper_CheckinHelper(VSSPROJ,PD[PRODUCT][i],COMMENT);		
		if(Retval != 0)
		{
			WScript.Echo("Error checking in deliverable..will quit");
			WScript.Quit(Retval);
		}
		
	}
	
}

/*******************************************
Helper functions for those that need platform SDK Headers
to be registered in the path for compilation
*******************************************/
function Helper_RegisterPlatformSDKHeaders()
{
	var SDKUTIL_PATH = "C:\\Program Files\\Microsoft SDK\\";
	var SDKUTIL_REG = "\"C:\\Program Files\\Microsoft SDK\\Setup\\VCIntegrate.exe\"";


	//Register it back after building
	CmdLine = SDKUTIL_REG + " /I " + SDKUTIL_PATH;
	Retval = gShell.Run (CmdLine, 1, true);

}

function Helper_UnRegisterPlatformSDKHeaders()
{
	var SDKUTIL_PATH = "C:\\Program Files\\Microsoft SDK\\";
	var SDKUTIL_REG = "\"C:\\Program Files\\Microsoft SDK\\Setup\\VCIntegrate.exe\"";

	//unregister it after building
	var CmdLine = SDKUTIL_REG + " /U";
	gShell.Run(CmdLine, 1, true);
}



function Helper_InitShell()
{
	gShell = WScript.CreateObject("WScript.Shell");
}


function Helper_CheckArgs()
{
	if(WScript.Arguments.length != 2)
	{
		WScript.Echo("Usage is build PDT BUILD/CHECKIN");
		WScript.Quit(1);
	}
}

function Helper_ChangeAttrib()
{
	gShell.Run("attrib -r *.* /s",1,true);
}

function Helper_SetArgs()
{
	PRODUCT = WScript.Arguments(0);
	BUILDCTRL = WScript.Arguments(1);

}

function Helper_ExtractThisVSSProj()
{
	THISENV = gShell.Environment("PROCESS");
	THISVSSPROJ = THISENV("THISVSSPROJ");
	COMMENT = THISENV("COMMENT");
	
	return THISVSSPROJ;
}


function Helper_Init()
{

	Helper_InitShell();

	

	Helper_CheckArgs();

	Helper_SetArgs();

	Helper_ChangeAttrib();

	


}
