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

function getConsoles(javaAvailable, activeXAvailable)
{
	xmit.head('..'+top.gJavaJnlp,jviewer_check);
	function jviewer_check(arg)
	{
		top.settings.console = {};
		if (arg.status==200) {
			//top.settings.console.java= true;
			if (javaAvailable!=undefined) {
				javaAvailable(true);
			}
		} else {
			//top.settings.console.java= false;
			if (javaAvailable!=undefined) {
				javaAvailable(false);
			}
		}
		if (window.ActiveXObject) {
			top.settings.console.activex = {};
			top.settings.console.activex.cabname = "/OCX/WinKvmClient" + getBrowserPlatformVersion() + ".cab";
			xmit.head('..'+top.settings.console.activex.cabname, activex_check);
		} else {
			if (activeXAvailable!=undefined) {
				activeXAvailable(false);
			}
		}
	}

	function activex_check(arg)
	{
		if (arg.status==200) {
			top.settings.console.activex.available = true;
			if (activeXAvailable!=undefined) {
				activeXAvailable(true);
			}
		} else {
			top.settings.console.activex.available = false;
			if (activeXAvailable!=undefined) {
				activeXAvailable(false);
			}
		}
	}
}

doLaunchJava = function()
{
	var is_opera; // to check for specific opera browser.
	xmit.get({url:"/rpc/WEBSES/validate.asp",onrcv:function(arg)
	{
		if(arg.HAPI_STATUS==0)
		{
			if (top.user.isAdmin())
			{
				top.gConsoleOpen = true;
				is_opera = navigator.userAgent.toLowerCase();
				is_opera = (is_opera.indexOf("opera") != -1);
				if (is_opera) {
					window.open(top.gPageDir + "jviewer_launch.html?" + 
						"JNLPSTR=JViewer&JNLPNAME=" + top.gJavaJnlp,
						"RemoteConsole", "toolbar=0, resizable=yes," + 
						"width=400, height=320, left=450, top=270");
				} else {
					window.open(top.gPageDir + "jviewer_launch.html?" + 
						"JNLPSTR=JViewer&JNLPNAME=" + top.gJavaJnlp, 
						"RemoteConsole", "toolbar=0, resizable=yes, " +
						"width=400, height=110, left=350, top=300");
				}	
			}
			else
				alert(eLang.getString('common',"STR_CONF_ADMIN_PRIV"));
		}
		else
		{
			parent.gLogout=1;
			top.document.cookie = "SessionExpired=true;path=/";
			parent.location.href="login.html";
		}
	},evalit:false});
}

doLaunchActiveX = function()
{
	xmit.get({url:"/rpc/WEBSES/validate.asp",onrcv:function(arg)
	{
		if(arg.HAPI_STATUS==0)
		{
			if (top.user.isAdmin())
			{
				var req = new xmit.getset({url:'/rpc/getactivexcfg.asp',onrcv:getActiveXCfgRes, status:''});
				req.add ("OSVERSION",getBrowserPlatformVersion());
				req.send();
				delete req;
			}
			else
				alert(eLang.getString('common',"STR_CONF_ADMIN_PRIV"));
		}
		else
		{
			parent.gLogout=1;
			top.document.cookie = "SessionExpired=true;path=/";
			parent.location.href="login.html";
		}
	},evalit:false});
}

function getActiveXCfgRes(arg)
{
	if (arg.HAPI_STATUS)
	{
		errstr = eLang.getString('common','STR_CONSOLE_ACTIVEX_GETVAL')
		errstr += (eLang.getString('common','STR_IPMI_ERROR')+ GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	}
	else
	{
		res = WEBVAR_JSONVAR_GETACTIVEXCFG.WEBVAR_STRUCTNAME_GETACTIVEXCFG[0];
		top.settings.console.activex.classid = res.CLASSID;
		top.settings.console.activex.version = res.VERSION;
		//alert ("Cab Filename::" + top.settings.console.activex.cabname + "\nClass Code::" + top.settings.console.activex.classid + "\nVersion::" + top.settings.console.activex.version);
		window.open('..'+top.gPageDir+'activex_launch.html','RemoteConsole','toolbar=0,resizable=yes,width=400,height=110,left=350,top=300')
	}
}

function getBrowserPlatformVersion()
{
	if (window.navigator.platform.indexOf("Win32") != -1)
		return 32;
	else if (window.navigator.platform.indexOf("Win64") != -1)
		return 64
	else
		return 32;
}

function elmOffset(elm,type)
{
	var offset = 0;

	if(type.toLowerCase()=='left' && elm.x) return elm.x;
	if(type.toLowerCase()=='top' && elm.y) return elm.y;

	try{
	while(elm.nodeName.toUpperCase()!='BODY')
	{
		if(type.toLowerCase()=='left') type='Left';
		if(type.toLowerCase()=='top') type='Top';
		offset += eval('elm.offset'+type);
		elm = elm.offsetParent;
	}
	}catch(e){}
	return offset;
}

function createWebEventsRecord(gSettings)
{
	//top.reloadValues is a global set to true when page refreshes. See header_imp.js - Manoj (Bug : 11734)
	if((gEventData = parent.gEventData)==undefined || top.reloadValues) 
	{
		//Change top.reloadValues to false so that this doesn't become an infinite recursion - Manoj (Bug : 11734)
		top.reloadValues = false;
		xmit.get({url:'/rpc/getallselentries.asp',
		onrcv:function(arg)
		{
			if(arg.HAPI_STATUS==0)
			{
				gEventData = WEBVAR_JSONVAR_HL_GETALLSELENTRIES.WEBVAR_STRUCTNAME_HL_GETALLSELENTRIES;
				//gEventData has not been passed by reference so we also need to
				//alter parent.gEventData so that the recursion is effective - Manoj (Bug : 11734)
				parent.gEventData = gEventData;
				createWebEventsRecord(gSettings);
			}else
			{
				alert("Error getting events records");
			}
		}});
		return;
	}

	try{
	if((gMaxEventData=parent.gMaxEventData)==undefined)
	{
		//assume 1024
		gMaxEventData = 1024;
	}
	}catch(e){gMaxEventData = 1024;};

	var graphData = [];

	var uniqueSensors = [];
	var alert_txt = [];
	for(var i=0; i<gEventData.length;i++)
	{
		var id=null;
		if((id=uniqueSensors.indexOf(gEventData[i].SensorName))==-1)
		{
			uniqueSensors.push(gEventData[i].SensorName);
			graphData.push({sector_name:gEventData[i].SensorName,
                            sector_type:gEventData[i].SensorType,
                            sector_percentage:1,
							sector_link:"event_log.html?sid="+getSensorID(gEventData[i].SensorName),
							sub_sectors:[]});
		}else
		{
			if(id==null){alert('No such sensors'); break};
			graphData[id].sector_percentage++;
		}
	}

	delete uniqueSensors;

	//convert occurance to percentage and assign it's unique color
    var pieColor = '#000000';
    
    if(!M_CUSTOM_COLOR) graphColors = new palette(graphData.length, 150, 255, true, false);
    
	for(var i=0;i<graphData.length;i++)
	{
        if(M_CUSTOM_COLOR){
            pieColor = getSensorColor(graphData[i].sector_type, graphData[i].sector_name);
        } else {
            pieColor = graphColors.popColor();
        }
        
        graphData[i].sector_color = pieColor;
		graphData[i].sector_percentage = graphData[i].sector_percentage/gMaxEventData * 100;
	}

	gSettings.gData=graphData;
	createEventGraph(gSettings,false);
}

function createEventGraph(gSettings, fresh)
{
	if(fresh)
	{
		gEventData = undefined;
	}

	if(gSettings.gData==undefined || gSettings.gData==null)
	{
		createWebEventsRecord(gSettings);
		return;
	}

	graph._init(gSettings.canvasName,gSettings);
	graph.drawPieChart(gSettings.radius);

}


function palette(count, min, max, noGray, shuffle){
/*  
    palette is a class declaration. Instantiate to get a palette object
    which generates the required number of distinct colors
    count   : number of colors required
    min     : minimum value for R,G and B
    max     : maximum value for R,G and B
    noGray  : set true to avoid whites, grays and blacks
    shuffle : set true to randomize order of colors

    After instantiating use the palette.popColor() function. It returns
    the colors one by one in #xxxxxx format
    
    Usage   : myPalette = new palette(256, 150, 250, true, true);
              newColor = myPalette.popColor();
              newColor2 = myPalette.popColor();
*/
    
    //**********Private methods**********
    function rgb(r,g,b){
    //r,g,b : RGB components in decimal (0 to 255)
    //returns the color in #xxxxxx format
        return "#" + hex(r) + hex(g) + hex(b);
    
        function hex(dec){
        //convert decimal to hex (only 0 to 255)
        //returns two digit hex number
            var secondDigit = dec % 16;
            var firstDigit = Math.floor(dec / 16) % 16;
            
            return toChar(firstDigit) + toChar(secondDigit);
        }
    }
    
    function forward(){
    //moves red, green and blue to point to the next color                
        if(j < step){
            j++;
        } else if(j == step){
            j = 0;
            if(i < step){
                i++;
            } else if(i == step){
                i = j = 0;
                if(main_comp == null){
                    main_comp = 'red';
                } else if(main_comp == 'red'){
                    main_comp = 'green';
                } else if(main_comp == 'green'){
                    main_comp = 'blue';
                } else if(main_comp == 'blue'){
                    main_comp = 'red';
                    if(step < components.length){
                        step++;
                    }
                }
            }
        }
        
        
        if(main_comp == 'red'){
            red = step;
            green = i;
            blue = j;
        } else if(main_comp == 'green'){
            red = i;
            green = step;
            blue = j;
            if(isGray() || (i == step)) forward();
        } else if(main_comp == 'blue'){
            red = i;
            green = j;
            blue = step;
            if(isGray() || (i == step) || (j == step)) forward();
        }

        if(noGray && isGray()) forward();
    }
    
    function isGray(){
    //if all components are equal, it's gray : return true
        if((components[red] == components[green]) && (components[green] == components[blue])){
            return true;
        } else {
            return false;
        }
    }
    
    function __constructor(){
    //Constructor
        palette.count = count;
        
        //We'll split up red, blue and green components into equal intervals.
        //x will determine the number of shades we need in each component
        //to create atleast 'count' number of colors.
        var x = Math.ceil(Math.pow(palette.count, 1/3));
        if(noGray){
            while( ((x*x*x) - x) < palette.count ) x++;
        }

        var shades = (x < 3)? 3 : x;
        if(shades > 3){
            var interval = (max - min) / (shades - 2);
            for(var i = 2; i < (shades - 1); i++){
                components[i] = Math.floor(min + (interval * (i - 1)))
            }
        }
        components[shades - 1] = max;
        
        if(shuffle) components.shuffle();

        //reverse will make brighter colors come first
        components.reverse();

        //break in
        forward();
        
        if(noGray && isGray()) forward();
    }
    //**********End of private methods**********
    
    //**********Private variables**********
    var components = new Array(0, min); //values of distinct shades of each RGB component
    var popped = 0;                     //number of colors popped so far
    var red, green, blue;               //index of red, green and blue shades respectively
    red = green = blue = 0;
    
    var main_comp = null;
    var step, i, j;
    step = i = j = 0;
    
    var palette = this;                 //reference to the current object (to use inside nested functions)
    //**********End of private variables**********

    //**********Public methods**********
    this.popColor = function(){
        var color = rgb(components[red], components[green], components[blue]);
        if(popped < (this.count - 1)){
            forward();
            popped++;
        }
        return color;
    }
    //**********End of public methods**********
    
    //**********Declarations over, call constructor**********
    __constructor();
    
    //**********End of palette**********//
}

Array.prototype.shuffle = function(){
//Shuffle array - Manoj
    var len = this.length;
    var temp = rand = 0;
    for(var i = 0; i < len; i++){
        rand = parseInt(Math.random() * len);
        temp = this[i]; this[i] = this[rand]; this[rand] = temp;
    }
}

function toHex(d) {
    var r = d % 16;
    var result;
    if (d-r == 0)
        result = toChar(r);
    else
        result = toHex( (d-r)/16 ) + toChar(r);
    return result;
}

function toChar(n) {
    var alpha = "0123456789ABCDEF";
    return alpha.charAt(n);
}

var M_CUSTOM_COLOR = false;
var graphColors = null;

gSensorTable = [];
/* Fill table automatically

 ## Enable this once the color is added to i files ##

xmit.get({url:"/rpc/getallsensors.asp",function(arg){
	if(arg.HAPI_STATUS==0)
	{
		SENSORINFO_DATA = WEBVAR_JSONVAR_HL_GETALLSENSORS.WEBVAR_STRUCTNAME_HL_GETALLSENSORS;
		for(var i=0; i=SENSORINFO_DATA.length; i++)
		{
			gSensorTable[SENSORINFO_DATA[i].SensorType]={name: SENSORINFO_DATA[i].SensorName, color: SENSORINFO_DATA[i].color};
		}
	}
}, status:''});

*/

function getSensorColor(sensorType,sensorName)
{   
    return '#000000'; //remove this line and enable the below lines once the color is added to i files ##
    /*while(gSensorTable[sensorType].name!=sensorName){};
    return gSensorTable[sensorType].color;*/
}

var m_Max_allowed_offset = new Array();
var m_Max_allowed_SensorSpecific_offset = new Array();

// Please refer the spec . This array contain max allowed offet for particular generic Event /Reading Type for 0x1 to 0x0c
m_Max_allowed_offset[0] = 0x0;
m_Max_allowed_offset[1] = 0x0b;
m_Max_allowed_offset[2] = 0x3;
m_Max_allowed_offset[3] = 0x2;
m_Max_allowed_offset[4] = 0x2;
m_Max_allowed_offset[5] = 0x2;
m_Max_allowed_offset[6] = 0x2;
m_Max_allowed_offset[7] = 0x8;
m_Max_allowed_offset[8] = 0x2;
m_Max_allowed_offset[9] = 0x2;
m_Max_allowed_offset[10] = 0x8;
m_Max_allowed_offset[11] = 0x7;
m_Max_allowed_offset[12] = 0x3;

// Please refer the spec . This array contain max allowed offet for particular generic Event /Reading Type for 0x6f
m_Max_allowed_SensorSpecific_offset[5] = 6;
m_Max_allowed_SensorSpecific_offset[6] = 5;
m_Max_allowed_SensorSpecific_offset[7] = 10;
m_Max_allowed_SensorSpecific_offset[8] = 6;
m_Max_allowed_SensorSpecific_offset[9] = 7;
m_Max_allowed_SensorSpecific_offset[12] = 8;
m_Max_allowed_SensorSpecific_offset[15] = 2;
m_Max_allowed_SensorSpecific_offset[16] = 5;
m_Max_allowed_SensorSpecific_offset[17] = 7;
m_Max_allowed_SensorSpecific_offset[18] = 5;
m_Max_allowed_SensorSpecific_offset[19] = 10;
m_Max_allowed_SensorSpecific_offset[20] = 4;
m_Max_allowed_SensorSpecific_offset[25] = 0;
m_Max_allowed_SensorSpecific_offset[29] = 4;
m_Max_allowed_SensorSpecific_offset[30] = 4;
m_Max_allowed_SensorSpecific_offset[31] = 6;
m_Max_allowed_SensorSpecific_offset[32] = 1;
m_Max_allowed_SensorSpecific_offset[33] = 9 ;
m_Max_allowed_SensorSpecific_offset[34] = 13;
m_Max_allowed_SensorSpecific_offset[35] = 8;
m_Max_allowed_SensorSpecific_offset[36] = 3;
m_Max_allowed_SensorSpecific_offset[37] = 2;
m_Max_allowed_SensorSpecific_offset[39] = 1;
m_Max_allowed_SensorSpecific_offset[40] =3 ;
m_Max_allowed_SensorSpecific_offset[41] =2 ;
m_Max_allowed_SensorSpecific_offset[42] = 1;
m_Max_allowed_SensorSpecific_offset[43] = 7 ;
m_Max_allowed_SensorSpecific_offset[44] = 7 ;

function processEventRecordForGraph(rec)
{
	str = "";
	type = getbits(rec.EventDirType,6,0);

	//var sensor_specific_count=0;
	var t_discrete=0;
	var t_lnr=0;
	var t_lc=0;
	var t_lnc=0;
	var t_hnr=0;
	var t_hc=0;
	var t_hnc=0;
	var other_event=0;

	// If event type is 0x6F then read from sensor-specific table
	// else, read from event_strings table
	if ( type == 0)
	{
		/* Unspecified */
	}
	else if (( type >= 0x01) && ( type <= 0x0C))		//Threshold-based and Generic Discrete
	{
		offset = getbits(rec.EventData1,3,0);
		if(m_Max_allowed_offset[type]  >= offset)
		{
			str = eLang.getString('event',type,offset);
			if(type==0x01 && getbits(rec.EventDirType,7,7)==0)
			{
				switch(offset)
				{
					case 0:	case 1:		t_lnc++; break;
					case 2:	case 3:		t_lc++; break;
					case 4:	case 5:		t_lnr++; break;
					case 6:	case 7:		t_hnc++; break;
					case 8:	case 9:		t_hc++; break;
					case 10:case 11:	t_hnr++; break;
					default:			other_event++;
				}
			}
			else if(( type >= 0x02) && ( type <= 0x0C))	//Generic Discrete
			{
				t_discrete++;
			}
		}
		else
		{
			str =eLang.getString("common","INVALID_OFFSET");
		}
	}
	else if ( type == 0x6F )	//Sensor-specific Discrete
	{
		offset = getbits(rec.EventData1,3,0);
		if(m_Max_allowed_SensorSpecific_offset[rec.SensorType] >= offset)
		{
			str = eLang.getString('sensor_specific_event',rec.SensorType,offset);
			t_discrete++;
		}
		else
		{
			str =eLang.getString("common","INVALID_OFFSET");
		}
	}
	else
	{
		/* OEM Discrete (Range is 70h-7Fh)*/
	}

	//if it is a BIOS Post event then
	if ( (rec.GenID1 >= 0x01) && (rec.GenID1 <= 0x1F) && ( rec.SensorType == 0xf) )
	{
		//look up bios_post_String using offset and evtdata2
		//we are clean..event data 2 should be seen only if this bits indicate evtdata2 has something

		//Only 0 and 1 table are in Bios_post_event_str.js  //
		if (getbits(rec.EventData1,7,6) == 0xC0  &&( (offset >=0 ) && (offset <= 2)))
		{
			/* Since SensorType 0xf ,Offset 1 and 2 use the same table Entry  */
			if(2 == offset)
				offset = 1;
			str += "-" + eLang.getString('bios_post_event',offset,getbits(rec.EventData2,3,0));
		}
		else
		{
			str += "-" + eLang.getString('common',"STR_UNKNOWN");
		}
		other_event++;
	}

	return {'str':str,
			'other_events': other_event,
			//'sensor_specific': sensor_specific_count,
			'discrete':t_discrete,
			'lnr':t_lnr,
			'lc':t_lc,
			'lnc':t_lnc,
			'hnr':t_hnr,
			'hc':t_hc,
			'hnc':t_hnc}
}

function getFormattedDate(duration)
{
	var seconds = Math.floor(duration/1000);
	var ms = duration%1000;
	var minutes = Math.floor(seconds/60);
	seconds = seconds%60;
	var hour = Math.floor(minutes/60);
	minutes = minutes%60;

	hour = hour.toString();
	minutes = minutes.toString();
	seconds = seconds.toString();

	return (hour?hour:"00")+' hr '+(minutes?((minutes.length==1?'0':'')+minutes):"0")+" min "+(seconds.length==1?'0':'')+seconds+" sec "+ms+" ms";
}

function getSensorID(sensorName)
{
	for(var i=0; i<SENSORINFO_DATA.length; i++)
	{
		if(sensorName==SENSORINFO_DATA[i].SensorName)
		{
			break;
		}
	}

	if(i!=SENSORINFO_DATA.length) return i;
	else return false;
}

function getStateAndUnit(j)
{
	if(SENSORINFO_DATA[j].SensorAccessibleFlags == 0xD5)
	{
		unitstr = "";
		SensorReading = eLang.getString('common',"STR_NOT_AVAILABLE");
	}
	else
	{
		unitstr = eLang.getString('unittype',SENSORINFO_DATA[j].SensorUnit2);
		SensorReading = (SENSORINFO_DATA[j].SensorReading)/1000;
	}

	if(SENSORINFO_DATA[j].SensorState)
	{
		state = "";
		for (whichbit = 0; whichbit <= 7; ++whichbit)
		{

			if (SENSORINFO_DATA[j].SensorState & (0x01 << whichbit))
			{
				state = state + eLang.getString('threshstate',(0x01 << whichbit)) + " ";
			}
		}
	}
	else
	{
		state = "";
		DiscreteSensorReading = SensorReading;
		if ((SENSORINFO_DATA[j].DiscreteState >= 0x02) && (SENSORINFO_DATA[j].DiscreteState <= 0x0C))
		{
			for(whichbit = 0;whichbit <= 7 && DiscreteSensorReading;whichbit++)
			{
				if(DiscreteSensorReading & 0x01)
				{
					state = state + eLang.getString('event',SENSORINFO_DATA[j].DiscreteState,whichbit) + " ";
				}
				DiscreteSensorReading = DiscreteSensorReading >> 1;
			}

		}
		else if (0x6F == SENSORINFO_DATA[j].DiscreteState)
		{
			for(whichbit = 0;whichbit <= 7 && DiscreteSensorReading;whichbit++)
			{
				if(DiscreteSensorReading & 0x01)
				{
					state = state + eLang.getString('sensor_specific_event',SENSORINFO_DATA[j].SensorType,whichbit) + " ";
				}
				DiscreteSensorReading = DiscreteSensorReading >> 1;
			}
		}

		SensorReading = ConvertDiscreteReading(SensorReading);
		unitstr = "";
		if (!state)
			state = eLang.getString('common',"STR_APP_STR_ALL_DEASSERTED");
	}

	if(SENSORINFO_DATA[j].SensorAccessibleFlags == 0xD5)
	{
		unitstr = "";
		SensorReading = eLang.getString('common',"STR_NOT_AVAILABLE");
	}
	else
	{
	}


	if(!state) state = eLang.getString('common',"STR_NOT_AVAILABLE");

	return {'state':state, 'unitstr':unitstr, 'SensorReading': SensorReading};
}


function ConvertDiscreteReading(SensorReading)
{
	if (isNaN(SensorReading))
		return SensorReading;

	var converted = "0x";
	var i;
	for (i = 0; i < 4; i++)
	{
		var hexChars = "0123456789ABCDEF";
		var n = (SensorReading >> 12) & 0xf;
		SensorReading = SensorReading << 4;

		converted += hexChars.charAt(n);
	}

	return converted;
}

function convertToLocale(dateString)
{
	var CardDate = new Date(dateString + " GMT");
	return ( CardDate.toLocaleString() );
}

function setFlashMode(bool)
{
	top.gFlashMode = bool;
}

function prepareDeviceShutdown()
{
	parent.oMenu.disableNavBar();
	parent.StopPolling();
	parent.closeAllWidgets();
	parent.web_alerts.stop();
	top.headerFrame.clearActions();
}

function rebootDevice(FLASH_SUCCESS_RESET)
{
	xmit.get({url:(FLASH_SUCCESS_RESET==true)?"/rpc/postflashreset.asp":"/rpc/rebootcard.asp", onrcv:function(){}, status:''});
	var mb = MessageBox(eLang.getString("common","STR_DEVICE_RESET_TITLE"),p(eLang.getString("common","STR_DEVICE_RESET_DESC")),[],true);
	showWait(false);
/*	var safari = (navigator.userAgent.toLowerCase().indexOf('safari')!=-1);
	if (!safari)		//Safari clear cookies first, before sending above rpc call, So clearCookies is blocked for Safari.
		clearCookies();*/
}

function resetNetwork()
{
	showWait(false);
	prepareDeviceShutdown();
	var mb = MessageBox(eLang.getString("common","STR_CONF_NW_RESET_TITLE"),
				p(eLang.getString("common","STR_CONF_NW_RESET_DESC")),[],true);
	clearCookies();
}

function clearCookies()
{
	top.fnCookie.erase('SessionCookie');
	top.fnCookie.erase('Username');
	top.fnCookie.erase('Privilege');
	top.fnCookie.erase('PNO');
	top.fnCookie.erase('SessionExpired');
	top.fnCookie.erase('WebServer');
	top.fnCookie.erase('settings');
}

function comboParser(hashdata, selectid)
{
	if(hashdata.indexOf(selectid) != -1)
	{
		var selVal = 0;
		arr = hashdata.split(',');
		for (i=0;i<arr.length;i++)
			if (arr[i].indexOf(selectid) != -1)
				selVal = (arr[i].split('=')[1]);
		return selVal;
	}
}

function tabParser(hashdata)
{
	var tabID = null;
	if (hashdata) {
		tabID = hashdata.split("#");
		tabID = tabID[1].split(",");
		if (tabID.length > 0) {
			return (tabID[0]);
		} else {
			return null;
		}
	} else {
		return null;
	}
}

function restartWebServer()
{
	prepareDeviceShutdown();

	var RPC_ResetCard = new xmit.getset({url:"/rpc/restartwebserver.asp",onrcv:function(){}});
	RPC_ResetCard.send();
	var mb = MessageBox(eLang.getString("common","STR_WEB_RESET_TITLE"),p(eLang.getString("common","STR_WEB_RESET_DESC")),[],true);
	//clearCookies();
}
function restartHTTPSService(alertstr)
{
	xmit.get({url:"/rpc/restarthttps.asp",onrcv:function(){}});
	if (window.location.href.indexOf("https:") != -1) {
		prepareDeviceShutdown();
		var mb = MessageBox(eLang.getString("common","STR_WEB_RESET_TITLE"),p(eLang.getString("common","STR_HTTPS_RESET_DESC")),[],true);
		return true;
	} else {
		alert(eLang.getString("common", alertstr));
		return false;
	}
}
function deviceInFlashMode()
{
	oMenu.disableNavBar();
	top.headerFrame.clearActions();
	closeAllWidgets();
	var mb = MessageBox(eLang.getString("common","STR_DEVICE_FLASHMODE_TITLE"),p( eLang.getString("common","STR_DEVICE_FLASHMODE_DESC")), [], true);
	//clearCookies();
}

function getIndexFromETHIndex(ethIndex)
{
	var index = -1;
	for (var i = 0; i < top.settings.eth.length; i++)
	{
		if (top.settings.eth[i] == ethIndex)
		{
			index = i;
			break;
		}
	}
	if (index == -1)
	{
		alert ("Error in getting Index for ETH Index::" + ethIndex);
		index = 0;
	}
	return index;
}

function getIndexFromLANChannel(lanChannel)
{
	var index = -1;
	for (var i = 0; i < top.settings.lan.length; i++)
	{
		if (top.settings.lan[i] == lanChannel)
		{
			index = i;
			break;
		}
	}
	if (index == -1)
	{
		alert ("Error in getting Index for LAN Channel::" + lanChannel);
		index = 0;
	}
	return index;
}

function getIndexFromEthString(ethString)
{
	var index = -1;
	for (var i = 0; i < top.settings.ethstr.length; i++)
	{
		if (top.settings.ethstr[i] == ethString)
		{
			index = i;
			break;
		}
	}
	if (index == -1)
	{
		alert ("Error in getting Index for Interface Name::" + ethString);
		index = 0;
	}
	return index;
}

function getUTCSeconds (utcoffset_str)
{
	var utc_split;
	var utc_hr = 0, utc_min=0;
	var utc_sec = 0;

	if (-1 == utcoffset_str.indexOf("+/-0"))
	{
		utc_split = utcoffset_str.split(":");					//utc_offset data format is hh:mm
		utc_hr = parseInt(utc_split[0],10);
		utc_min = parseInt(utc_split[1]);
	}
	else
	{
		return (utc_sec);
	}

	if (utc_hr < 0)
	{
		utc_sec = ((utc_hr * 60) - utc_min) * 60;	//converting hours to minutes, then to seconds.
	}
	else if (utc_hr > 0)
	{
		utc_sec = ((utc_hr * 60) + utc_min) * 60;
	}
	else
	{
		if(utc_split[0]=="+00")
			utc_sec=(utc_min * 60);
		else
			utc_sec=(utc_min * 60 * (-1));
	}
	return (utc_sec);
}

function getUTCString(utcInSeconds)
{
	var utcString = "";
	var utcMinutes = 0;
	if (utcInSeconds == 0)
	{
		utcString = "+/-0";
	}
	else
	{
		utcMinutes = utcInSeconds/60;
		if (utcMinutes > 0)		//Its positive value
		{
			utcString += parseInt(utcMinutes/60) < 10 ? "+0": "+";
		}
		else if (utcMinutes < 0)
		{
			utcString += parseInt(Math.abs(utcMinutes/60)) < 10 ? "-0": "-";
		}
		utcString += parseInt(Math.abs(utcMinutes/60));		//To get UTC Hours
		utcString += parseInt(Math.abs(utcMinutes%60)) < 10 ? ":0": ":";
		utcString += parseInt(Math.abs(utcMinutes%60));		//To get UTC seconds
	}
	return utcString;
}

function getUTCOffset (fnptrRes)
{
	xmit.get({url:"/rpc/getdatetime.asp", onrcv:getUTCOffsetRes, status:''});

	function getUTCOffsetRes(arg)
	{
		var utc_offset_data = 0;
		if (arg.HAPI_STATUS != 0)
		{
			errstr = eLang.getString('common','STR_CONF_DATE_TIME_GETVAL');
			errstr += (eLang.getString('common','STR_IPMI_ERROR')+GET_ERROR_CODE(arg.HAPI_STATUS));
			alert (errstr);
		}
		else
		{
			utc_offset_data = WEBVAR_JSONVAR_GETDATETIME.WEBVAR_STRUCTNAME_GETDATETIME[0].UTCMINUTES;
		}

		if(fnptrRes != undefined)
			fnptrRes (utc_offset_data);
	}
}

function getTimeStamp(timeInSeconds)
{
	var evtTimeStamp = "";
	var eventDate = new Date(timeInSeconds * 1000);

	var evtMonth=eventDate.getUTCMonth() + 1;
	evtMonth =((evtMonth < 10)?"0":"") + evtMonth;
	var evtDate = eventDate.getUTCDate();
	evtDate=((evtDate < 10)?"0":"") + evtDate;
	evtTimeStamp = evtMonth+'/'+evtDate+'/'+eventDate.getUTCFullYear();

	var evtHours =eventDate.getUTCHours();
	evtHours =((evtHours < 10)?"0":"") + evtHours;
	var evtMins = eventDate.getUTCMinutes();
	evtMins =((evtMins < 10)?"0":"") + evtMins;
	var evtSecs = eventDate.getUTCSeconds();
	evtSecs =((evtSecs < 10)?"0":"") + evtSecs;
	evtTimeStamp += '  '+evtHours+':'+evtMins+':'+evtSecs;
	return (evtTimeStamp);
}

function removeDuplicate(lstBoxID)
{
	if ( lstBoxID.options.length > 0 )
	{
		for ( var i=1,n=lstBoxID.options.length; i<n; i++ )
		{
			if ( lstBoxID.options[i].value == lstBoxID.options[i-1].value )
			{
				lstBoxID.options[i] = null;
				n--
			}
		}
	}
}

/*
 * This function is used to check the project configuration for the given 
 * feature. If the feature is present in the Project configuration list,
 * then it returns true, otherwise it returns false.
 * @param feature string, feature name to be searched in the list.
 * @return boolean, true-if feature exists, false-not exists.
 */
function checkProjectCfg(feature)
{
	var projectCfg;		//Project features configuration
	eval("projectCfg = " + fnCookie.read("settings"));
	if (projectCfg.features != undefined) {
		if (projectCfg.features.indexOf(feature) != -1) {
			return true;
		} else {
			return false;
		}
	} else {
		return false;
	}
}

if(top!=self)
{
	CONSTANTS = top.CONSTANTS;
}

gFormNameList = [];
gFormNameList[0] = "modAlert";
gFormNameList[1] = "advancedADFrm";
gFormNameList[2] = "addRoleGroup";
gFormNameList[3] = "modRoleGroup";
gFormNameList[4] = "addUserForm";
gFormNameList[5] = "modUserForm";
gFormNameList[6] = "addAlertPolicyForm";
gFormNameList[7] = "modifyAlertPolicyForm";
gFormNameList[8] = "addLANDestForm";
gFormNameList[9] = "modifyLANDestForm";
gFormNameList[10] = "addImageForm";
gFormNameList[11] = "replaceImageForm";

common_fn={loaded:true};
