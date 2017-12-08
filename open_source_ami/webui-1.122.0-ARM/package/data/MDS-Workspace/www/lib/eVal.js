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

// Library Module : eVal.js
// Separated By : chandrasekarr@amiindia.co.in
// Reason : For the purpose of validation in MDS

var 	EXTENDED_ERROR = 10;

var eVal =
{
	verbose : true,
	lastErrorNum : 0,

	islower : function(c)
	{
		if((c >= "a") && (c <= "z"))
			return true;
		return false;
	},
	isupper : function(c)
	{
		if((c >= "A") && (c <= "Z"))
			return true;
		return false;
	},
	isalpha : function(c)
	{
		if(this.islower(c) || this.isupper(c))
			return true;
		return false;
	},
	isdigit : function(c)
	{
		if ((c >= "0") && (c <= "9"))
			return true;
		return false;
	},
	/*
	 * This function is used to check the passing string contains only 
	 * numbers. 
	 * @param str string, value need to be validated.
	 * @return boolean, true-string contains only numbers, 
	 * false-contains other than numbers.
	 * 
	 */
	isnum : function(str)
	{
		var numfilter = /[0-9]*/;	//regex to check for only numbers
		return eVal.isblank(str.replace(numfilter,""));
		//return typeof n == 'number' && isFinite(n);
	},
	//Inputs
	//isnumstr can be used to check for the given string is whole number(not allows floating point value) or not. 
	//str : Numeric String to be validated
	//min : minimum numeric value, its optional argument, default value is 0
	//max : maximum numeric value, its optional argument, default value is 300
	isnumstr : function(str, min, max)
	{
		min = (min) ? min : 0;
		max = (max) ? max : 300;
		var patt = /[^0-9]/;
		if(patt.test(str))			//Checks for other than numbers
			return false;

		var v = parseInt(str, 10);
		if (v < min || v > max)		//Check for min & max range
			return false;

		return typeof v == 'number' && isFinite(v);	//Check for Number
	},
	isblank : function(str)
	{
		if(0 == str.length)
			return true;
		return false;
	},
	email : function(emailstr)
	{
		var befatpatt = /^[a-zA-Z0-9]+([_\.-]?[a-zA-Z0-9]+)*$/;
		var aftatpatt = /^[a-zA-Z0-9]+([\-\_]?[a-zA-Z0-9]+)*$/;
		var emailendpatt = /^[a-zA-Z0-9]{2,63}$/;
		
		var atsplit = emailstr.split('@');
		if (atsplit.length == 2)
		{
			if (befatpatt.test(atsplit[0]) == false)
				return false;
		
			dotsplit = atsplit[1].split('.');
			if (dotsplit.length >= 2)
			{
				var n = dotsplit.length;
				if (emailendpatt.test(dotsplit[n-1]) == false)
					return false;
		
				for (i=0; i<n-1; i++)
				{
					if (aftatpatt.test(dotsplit[i]) == false)
						return false;
				}
			}
			else
				return false;
		}
		else
			return false;
		
		return true;
	},
	str : function(n)
	{
		var filter = /^([a-zA-Z0-9_\-])+$/;
		if (n.match(filter)) {
			return true;
		}
		return false;
	},
	trim : function(stringToTrim)
	{
		return stringToTrim.replace(/^\s+|\s+$/g,"");
	},
	port : function(p)
	{
		var retval =  eVal.isnumstr(p,1,65535);
		return retval;
	},
	ip : function(ip, boolAllow0000, boolNetMask)
	{
		var i;	//loop counter
		boolAllow0000 = boolAllow0000 ? boolAllow0000 : false;
		boolNetMask = boolNetMask ? boolNetMask : false;
		var ipv = ip.split(".");
		if(ipv.length != 4)
		{
			if(this.verbose)
				eLang.getString('err',EXTENDED_ERROR+30);
			this.lastErrorNum = EXTENDED_ERROR+30;
			return false;
		}
		for(i = 0; i < 4 ;i++) {
			if( isNaN(ipv[i]) || ipv[i]=="" || 
				(!eVal.isnumstr(ipv[i],0,255)) || ipv[i].length > 3) {
				if(this.verbose) {
					eLang.getString('err',EXTENDED_ERROR+30);
				}
				this.lastErrorNum = EXTENDED_ERROR+30;
				return false;
			}
		}

// Disabled now that IP validation is done in HAPI
// [BrandonB] 1/12/04
//       // Now just compare directly to some invalid IPs
		var invalidIPs=[
			[255,255,255,0],
			[127,0,0,1],
			[0,24,56,4],
			[255,255,255,255],
			[91,91,91,91],
			[0,0,0,0]            // <--- 0.0.0.0 ALWAYS the last entry!
			]

		var arLen=invalidIPs.length;
		if (boolAllow0000) {
			arLen--;
		} else if (ipv[0]==0) {
			return false;
		}
		if (boolNetMask) {
			i = 1;
		} else {
			i = 0;
		}
		
		if (ipv[0]==224  || ipv[0]==240){
			if (this.verbose) {
				eLang.getString('err',EXTENDED_ERROR+30);
			}
			this.lastErrorNum = EXTENDED_ERROR+30;
			return false;
		}

		for (;i < arLen; i++) {
			if ((ipv[0]==invalidIPs[i][0])&&
				(ipv[1]==invalidIPs[i][1])&&
				(ipv[2]==invalidIPs[i][2])&&
				(ipv[3]==invalidIPs[i][3])) {
				if (this.verbose) {
					eLang.getString('err',EXTENDED_ERROR+30);
				}
				this.lastErrorNum = EXTENDED_ERROR+30;
				return false;
			}
		}
		return true;
	},

	/*
	 * This function is used to compare two IPv4 address matches or not.
	 * @param ip1 string, value of IP address to compare with.
	 * @param ip2 string, value of IP address to be compared.
	 * @return boolean, true-IP matches, false-IP differs.
	 */
	compareip : function(ip1,ip2)
	{
		var iparr1 = [4];		//IP Address1 in array of integer
		var iparr2 = [4];		//IP Address2 in array of integer
		var i;		//loop counter
		if (eVal.ip(ip1,true) && eVal.ip(ip2,true)) {
			iparr1 = ip1.split(".");
			iparr2 = ip2.split(".");
			for(i = 0; i < 4; i++) {
				iparr1[i] = parseInt(iparr1[i], 10);
				iparr2[i] = parseInt(iparr2[i], 10);
			}
			return (iparr1.compareWith(iparr2));
		} else {
			return false;
		}
	},
	/*
	 * This function is used to validate the subnet mask IP address.
	 * Binary of subnet mask should have 1s followed of 0s.
	 * @param subnetMask string, subnet mask to be validated.
	 * @return boolean, true-valid subnet, false-invalid subnet
	 */
	netMask : function(subnetMask)
	{
		//nested function paddedBinary
		//produces an 8 character binary string from the given decimal value
		function paddedBinary(decimal)
		{
			var binary = "";
			while(decimal > 0) {
				binary = binary + (decimal % 2);
				decimal = decimal >> 1;
			}
			binary = binary.split("").reverse().join("");
			while(binary.length < 8) {
				binary = "0" + binary;
			}
			return binary;
		}
		//Validating for IP address
		if (!eVal.ip(subnetMask, false, true)) {
			return false;
		}

		var parts = subnetMask.split(".");
		var binaryIp = "";
		//Generating the binary format of subnet mask
		for(var i = 0; i < parts.length; i++) {
			binaryIp = binaryIp + paddedBinary(parseInt(parseFloat(parts[i])));
		}

		//Subnet is always a string of 1s followed by a string of 0s
		//So we should not find a 1 after we find the first 0
		if(binaryIp.indexOf("1",binaryIp.indexOf("0")) != -1) {
			return false;
		}
		return true;
	},
	//This function validates IPv6 IP.
	//Input data :: ipstr, allowlocal, allow00
	//str - IPv6 IP String
	//allowlocal - boolean, true - skips validation for link local IP, false - performs link local IP validation.
	//allow00 - boolean, true - allow IP address with 0. 
	ipv6 : function (ipstr, allowlocal, allow00)
	{
		//This function checks for valid IPv6 octet.
		//Input data :: str, allowlocal, allow00  
		//str - array of strings, contains IPv6 octets.
		//allowlocal - boolean, true - skips validation for link local IP, false - performs link local IP validation.
		//allow00 - boolean, true - allow IP address with octet as 0. 
		function checkHexa(str, allowlocal, allow00)
		{
			var i;
			var pattern = /^([a-f][A-F][0-9])/;
			for (i=0; i<str.length; i++) {
				if(pattern.test(str[i])){	//Check characters other than Hexa
					//alert ("Invalid hexa numbers::" + str[i]);
					return true;
				}
				decip = parseInt(str[i], 16);
				if (isNaN(decip)) {
					//alert ("Invalid hex value::" + i);
					return true;
				} else if(decip > 0xFFFF || decip < 0) {
					//alert ("Hex value exceeds the range::"+decip);
					return true;
				}
				if (i==0 && decip >= 0xFF00) {
					//alert ("FF00->Broadcast IP Not allowed");
					return true;
				}
				if (i==0 && decip >= 0x1000 && decip <= 0x2000) {
					//alert ("Quanta Not allowed");
					return true;
				}
				if (i==0 && decip >= 0xFF00) {
					//alert ("FF00->Broadcast IP Not allowed");
					return true;
				}
				
				if(!allowlocal) {
					if (i==0 && decip == 0xFE80) {
						//alert ("FE80->Link local IP Not allowed");
						return true;
					}
				}
				if(!allowlocal) {
					if (i==0 && decip == 0xFE80) {
						//alert ("FE80->Link local IP Not allowed");
						return true;
					}
				}
				
				if(!allowlocal){
					if( (decip & 0xF000) == 0x1000)  //1000::/4
					return true;
				}
				if(!allowlocal){
					if( (decip & 0xFF00) == 0x0000)  //0000::/8
					return true;
				}
				if(!allowlocal){
					if( (decip & 0xFF00) == 0x0100)  //0100::/8
					return true;
				}
				if(!allowlocal){
					if( (decip & 0xFE00) == 0x0100)  //0200::/7
					return true;
				}
				if(!allowlocal){
					if( (decip & 0xFC00) == 0x0100)  //0400::/6
					return true;
				}
				if(!allowlocal){
					if( (decip & 0xF800) == 0x0800)  //0200::/5
					return true;
				}
				if(!allowlocal){
					if( (decip & 0xFE00) == 0x0100)  //0200::/7
					return true;
				}
				if(!allowlocal){
					if( (decip & 0xE000) == 0x4000)  //4000::/3
					return true;
				}
				if(!allowlocal){
					if( (decip & 0xE000) == 0x6000)  //6000::/3
					return true;
				}
				if(!allowlocal){
					if( (decip & 0xE000) == 0x8000)  //8000::/3
					return true;
				}
				if(!allowlocal){
					if( (decip & 0xE000) == 0xA000)  //A000::/3
					return true;
				}
				if(!allowlocal){
					if( (decip & 0xE000) == 0xC000)  //C000::/3
					return true;
				}
				if(!allowlocal){
					if( (decip & 0xF000) == 0xE000)  //E000::/4
					return true;
				}
				if(!allowlocal){
					if( (decip & 0xF800) == 0xF000)  //F000::/5
					return true;
				}
				if(!allowlocal){
					if( (decip & 0xFC00) == 0xF800)  //F800::/6
					return true;
				}
				if(!allowlocal){
					if( (decip & 0xFE00) == 0xFC00)  //FC00::/7
					return true;
				}
				if(!allowlocal){
					if( (decip & 0xFF00) == 0xFF00)  //FF00::/8
					return true;
				}
				if(!allowlocal){
					if( (decip & 0xFF80) == 0xFE00)  //FE00::/9
					return true;
				}
				if(!allowlocal){
					if( (decip & 0xFFC0) == 0xFE80)  //FE80::/10
					return true;
				}
				if(!allowlocal){
					if( (decip & 0xFFC0) == 0xFEC0)  //FEC0::/10
					return true;
				}
				
				if(!allow00) {
					if (i == 0 && decip == 0) {
						//alert ("First octet should not be zero");
						return true;
					}
				}
			}
			return false;
		}

		allow00 = allow00 ? allow00 : false;
		allowlocal = allowlocal ? allowlocal : false;
		splitip = ipstr.split("::");
		if(splitip.length == 2){		//Check for double colon
			var firstpart = splitip[0];
			var secondpart = splitip[1];
			var firstsplit = firstpart.split(':');
			var secondsplit = secondpart.split(':');

			if (!allow00) {
				if (eVal.isblank(firstpart)) {
					//alert ("Zero is not allowed in first octet");
					return false;
				}
			}
			if (!eVal.isblank(firstpart) && !eVal.isblank(secondpart)) {
				if ((firstsplit.length) + (secondsplit.length) > 7) {
					//alert ("Both part should not greater than 7");
					return false;
				}
			}
			if (!eVal.isblank(firstpart)) {
				if(checkHexa(firstsplit, allowlocal, allow00)) {
					//alert ("First part in double colon fails");
					return false;
				}
			}
			if (!eVal.isblank(secondpart)) {
				if(checkHexa(secondsplit, false, true)) {
					//alert ("Second part in double colon fails");
					return false;
				}
			}
		} else if(splitip.length < 2) {	//No double colon, check for single colon
			splitip = ipstr.split(":");
			if(splitip.length == 8) {		//Max of 8 single colon allowed
				if(checkHexa(splitip, allowlocal, allow00)) {
					//alert ("single colon content fails");
					return false;
				}
			} else {
				//alert ("Invalid single colon count");
				return false;
			}
		} else {
			//alert ("double colon count exceeds");
			return false;
		}
		return true;
	},

	/*
	 * This function is used to compare two IPv6 address matches or not.
	 * @param ip1 string, value of IP address to compare with.
	 * @param ip2 -> string value of IP address to be compared.
	 * @return boolean -> true-IP matches, false-IP differs.
	 */
	comparev6ip : function (ip1,ip2)
	{
		var splitIP1 = [8];		//IP Address1 in array of integer
		var splitIP2 = [8];		//IP Address2 in array of integer

		function splitOctet(ipv6)
		{
			var i;	//loop counter
			var octet = [8];	//IPv6 as octet value in integer format
			var splitIP;
			var colonsplit;
			for (i = 0; i < 8; i++) {
				//Initializes all 8 octets to 0.
				octet[i] = 0;
			}

			splitIP = ipv6.split("::");
			colonsplit = splitIP[0].split(":");
			for (i = 0; i < colonsplit.length; i++) {
				octet[i] = parseInt(colonsplit[i], 16);
			}
			if(splitIP.length == 2) {
				colonsplit = splitIP[1].split(":");
				for (i = colonsplit.length - 1, j = 7; i >= 0; i--, j--) {
					octet[j] = parseInt(colonsplit[i], 16);
				}
			}
			return(octet);
		}
		
		if (eVal.ipv6(ip1, true, true) && eVal.ipv6(ip2, true, true)) {
			splitIP1 = splitOctet(ip1);
			splitIP2 = splitOctet(ip2);
			return (splitIP1.compareWith(splitIP2));
		}
	},
	username : function (username, fltr, minlen, maxlen)		//2nd parameter "fltr" is used to choose the specific filter
	{
		minlen = minlen ? minlen : 4;
		maxlen = maxlen ? maxlen : 32;
		if((username.length > maxlen) || (username.length < minlen))
		{
			if(this.verbose)
				eLang.getString('err',EXTENDED_ERROR+27);
			this.lastErrorNum = EXTENDED_ERROR+27;
			return false;
		}
		if(!this.isalpha(username.charAt(0)))
		{
			if(this.verbose)
				eLang.getString('err',EXTENDED_ERROR+26);
			this.lastErrorNum = EXTENDED_ERROR+26;
			return false;
		}
		var usernamecopy = new String(username);
		var invalidchar_filter = new RegExp();
		if (fltr == 'IPMI')		//choosing filter for IPMI user validation
		{
			invalidchar_filter = /([.,;:/\\() ])/;
		}
		else					//choosing filter for SMTP user validation
		{
			invalidchar_filter = /([,:;\\ ])/;
		}
		if(usernamecopy.match(invalidchar_filter))
		{
			if(this.verbose)
				eLang.getString('err',EXTENDED_ERROR+25);
			this.lastErrorNum = EXTENDED_ERROR+25;
			return false;
		}
		invalidchar_filter = null;
		return true;
	},
	password : function (password, minlen, maxlen)
	{
		minlen = (minlen)?minlen:8;
		maxlen = (maxlen)?maxlen:32;
		if((password.length > maxlen) || (password.length < minlen))
		{
			if(this.verbose)
				eLang.getString('err',EXTENDED_ERROR+24);
			this.lastErrorNum = EXTENDED_ERROR+24;
			return false;
		}
		else
		{
			var re=/^[^ ]+$/;
			if (re.test(password)==false)
				return false;
		}
		return true;
	},
	hostname : function (hname)
	{
		var filter =  /^([a-zA-Z0-9\_\-])+$/;
		var v = new String(hname);
		if ((v.length > 64) || (v.length < 1))
		{
			delete v;
			return false;
		}

		if (v.match(filter))
		{
			delete v;
			return true;
		}
	
		delete v;
		return false;
	},
	/*
	 * This function is used to validate the given domain name.
	 * @param str string, domain name to be validated.
	 * @param atleastonedot boolean, true-atleast one dot allowed
	 * false-no dots allowed.
	 * @return boolean, true-valid domain name, false-invalid domain name
	 */
	domainname : function (str,atleastonedot)
	{
		var filter = /[a-zA-Z0-9]{1}[a-zA-Z0-9\-\_]*[a-zA-Z0-9]*/;
		//regex to check starts with alpha numericals, then follow LDH
		var numfilter = /[0-9]*/;	//regex to check for only numbers
		var splitStr;		//string used to split the domain name in dots
		var count = 0;		//number to count the numerical octets
		var replaceStr;		//string used to replace with regex
		var i;			//loop counter
		if((str.length <= 0) || (str.length > 255)) {
			return false;
		}

		splitStr = str.split(".");
		if ((atleastonedot) && (1 == splitStr.length)) {
		//if there is no dot, it returns false. (atleast one dot must)
			return false;
		}
		if ((!atleastonedot) && (1 < splitStr.length)) {
		//if there is dot, it returns false. (dot not allowed)
			return false;
		}

		if(splitStr[splitStr.length-1] != "") {
			for(i = 0; i < splitStr.length; i++) {
				if((splitStr[i].length <= 0) || (splitStr[i].length > 63)) {
				//Each octet should not exists the max size of 63
					return false;
				}

				replaceStr = splitStr[i].replace(filter,"");
				if((splitStr[i] == "") || (replaceStr.length > 0)) {
				//Each octet should have only LDH(letters,digits & hyphen)
					return false;
				}
				replaceStr = splitStr[i].replace(numfilter,"");
				if (replaceStr.length == 0) {
				//Each octet should not contain full of numbers
					count++;
				}
			}
			if (splitStr.length == count) {
				return false;
			}
		} else {
			return false;
		}
		return true;
	},

	/*
	 * This function is used to validate the LDAP/E-Directory Bind DN and
	 * SearchBase.
	 * @param str string, string to be validated.
	 * @param property string, specifies either binddn or searchbase.
	 * @return boolean, true-validation success, false-invalid data
	 */
	ldap : function (str, property)
	{
		var v = "";
		var cn = /^(cn=)[a-z]{1}[\w\-\.]{2,63}$/i;
		var uid = /^(uid=)[a-z]{1}[\w\-\.]{2,63}$/i;
		var ou = /^(ou=)[a-z]{1}[\w\-\.]{1,63}$/i;
		var dc = /^(dc=)[a-z]{1}[\w\-\.]{1,63}$/i;
		var o = /^(o=)[a-z]{1}[\w\-\.]{1,63}$/i;
		var min = (property == "searchbase" ) ? 1 : 2;

		if((str.length < 4) || (str.length > 63)) {
			return false;
		}

		v = str.split(",");
		if (v.length < min) {
			return false;
		}

		if (property == "binddn") {
			if (!(cn.test(v[0]) || uid.test(v[0]))) {
				return false;
			}
		}

		for (i = min-1; i < v.length; i++) {
			if ((!dc.test(v[i])) && (!ou.test(v[i])) && (!(cn.test(v[i]) || uid.test(v[i])))) {
				if (!((property == "searchbase" ) && (o.test(v[i])))) {
					return false;
				}
			}
		}

		return true;
	},

	// Added validation routine [BrandonB] 3/9/04
	// Input:
	//  value = hex string, can have leading 0x or just the number
	//  rangelow = lowest possible value
	//  rangehigh = highest possible value
	//  numdigits = max number of digits
	//  func = null=just check value, 1=return just hex digits as a string
	// Output:
	//  0 - ok
	//  1 - error with range
	//  2 - error with number of digits

	hexnumber:function (value, rangelow, rangehigh, numdigits, func)
	{
		var HEX_OK=0;
		var HEX_RANGE_ERR=1;
		var HEX_NUMDIGITS_ERR=2;
		var HEX_NUMERIC_ERR=3;

		var retVal=HEX_OK;
		var re=/^[a-fA-F0-9]*$/;
		// Strip off leading 0's, x or X and get just the digits
		var digitsstr=value.substr(value.search(/[a-fA-F1-9]/));	// the string value of just the hex digits

		// Step 1 check number of digits
		if (digitsstr.length>numdigits)
			retVal=HEX_NUMDIGITS_ERR;
		else
		{
			// Step 2 check hex value
			if (re.exec(digitsstr)!=null)
			{
				// Step 3 check ranges
				var digitsvalue=parseInt(digitsstr,16);	// the actual numeric value of the hex digits
				if ( (digitsvalue>=rangelow)&&(digitsvalue<=rangehigh))
					retVal=HEX_OK;
				else
					retVal=HEX_RANGE_ERR;
			}
			else
				retVal=HEX_NUMERIC_ERR;
		}
		if (func==1)
			return (digitsstr);
		else
			return retVal;
	},
	elm : function(name, type)
	{
		if(type == 'ip')
			return this.ip(document.getElementById(name).value);
		else if(type == 'str')
			return this.str(document.getElementById(name).value);
		else if(type == 'email')
			return this.email(document.getElementById(name).value);
		else if(type == 'username')
			return this.username(document.getElementById(name).value);
		else if(type == 'password')
			return this.password(document.getElementById(name).value);
		else if(type == 'port')
			return this.port(document.getElementById(name).value);
		else if(type == 'netMask')
			return this.netMask(document.getElekemntById(name).value);
		return false;
	},
	alertRangeInput : function(obj,lval,hval)
	{
		if((obj.value >= lval)&&(obj.value <= hval))
			return true;
		alert(eLang.getErrString(EXTENDED_ERROR+23)+lval+' - '+hval);
		obj.value = ''+lval+'';
	},
	hapiStatus : function(jvar, extramsg) //argument: JSON VAR NAME
	{
		if(jvar.HAPI_STATUS)
		{
			if(this.verbose)
				eLang.getString('err',jvar.HAPI_STATUS, extramsg);
			return false;
		}
		return true;
	},
	getLastErrorStr : function()
	{
		return eLang.getString('err',this.lastErrorNum);
	},
	/*
	* function endsWith
	* Checks whether the string 'str' ends with the string 'lookfor'
	*/
	endsWith : function(str, lookfor)
	{
		var strlen = str.length;
		var lookforlen = lookfor.length;

		var lookforptr = lookforlen;
		while( lookforptr > 0 )
		{
			if( str.charAt(strlen-lookforptr) != lookfor.charAt(lookforlen-lookforptr) )
			return false;
			lookforptr--;
		}
		return true;
	},
	
	/*
	* This function will validate the string value against the specified set of
	* Special characters. 
	* @param : str -> Required. String to be validated. 
	* @param : splCharFilter -> Optional. String of special characters required
	* for validation. By default, it will validate for all special characters. 
	* [~#$.,@!%^&*:;/\\ ()]. 
	* @return : true -> if 'str' is not having matched special characters, 
	* @return : false -> if 'str' is having matched special characters. 
	*/
	specialCharacter : function(str, splCharFilter) {
		var invalidchar_filter;	//Reg Exp for special character validation.
		invalidchar_filter = (splCharFilter) ? 
			(new RegExp("([" + splCharFilter + "])")) :
			(/([~#$.,@!%^&*:;/\\ ()])/);
		if (str.match(invalidchar_filter)) {
			delete invalidchar_filter;
			return false;
		}
		delete invalidchar_filter;
		return true;
	}
}

eVal.loaded = true;
