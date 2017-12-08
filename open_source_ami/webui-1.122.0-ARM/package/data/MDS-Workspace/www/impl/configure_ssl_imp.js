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


// File Name  : configure_ssl_imp.js
// Brief      : This implementation is to display the uploaded SSL certificate 
// information and allow the user to upload the SSL certificate or allow to 
// generate the SSL certificate. It contains implementation to upload, generate
// and view the SSL certificate.
// Author Name: Arockia Selva Rani. A

var varCertificateInfo;
var varPrivateKeyInfo;
var varCertExists;
var key;
var SSLFILE_INFO;

/*
 * This function will be called when its corresponding page gets loaded.
 * It will expose all the user controls and checks for user privilege.
 * Finally it will invoke the begin method. 
 */
function doInit()
{
	exposeElms(["_tabUploadSSL",
		"_tabGenerateSSL",
		"_tabViewSSL",
		"_rowUploadSSL",
		"_txtCertInfo",
		"_fleCertBrowse",
		"_txtKeyInfo",
		"_fleKeyBrowse",
		"_btnUpload",
		"_rowGenerateSSL",
		"_txtCommonName",
		"_txtOrganization",
		"_txtOrgUnit",
		"_txtCity",
		"_txtState",
		"_txtCountry",
		"_txtEmailID",
		"_txtValid",
		"_lstKeyLength",
		"_btnGenerate",
		"_rowViewSSL",
		"_lblVersion",
		"_lblSerialNo",
		"_lblSignAlg",
		"_lblPublicKey",
		"_lblFromCommonName",
		"_lblFromOrganization",
		"_lblFromOrgUnit",
		"_lblFromCity",
		"_lblFromState",
		"_lblFromCountry",
		"_lblFromEmailID",
		"_lblValidFrom",
		"_lblValidTo",
		"_lblToCommonName",
		"_lblToOrganization",
		"_lblToOrgUnit",
		"_lblToCity",
		"_lblToState",
		"_lblToCountry",
		"_lblToEmailID"
		]);

	if(!top.user.isAdmin()) {
		disableActions();
	}

	tabUploadSSL.onclick = doUploadSSL;
	tabGenerateSSL.onclick = doGenerateSSL;
	tabViewSSL.onclick = doViewSSL;

	_begin();
}

/*
 * It will invoke the upload SSL method, which the default tab to be displayed 
 * When the user chooses configure SSL option.
 */
function _begin()
{
	top.gSSLCert = false;
	var tabLastVisit = tabParser(top.mainFrame.pageFrame.location.hash);
	if (tabLastVisit != null) {
		$(tabLastVisit).onclick();
	} else {
		doUploadSSL();
	}
}

/*
 * It will clear the styles and apply basic style for all the tabs.
 */
function clearSSLUI()
{
	tabUploadSSL.style.fontWeight = "normal";
	tabGenerateSSL.style.fontWeight = "normal";
	tabViewSSL.style.fontWeight = "normal";
	rowGenerateSSL.className = "hiddenRow";
	rowViewSSL.className = "hiddenRow";
	rowUploadSSL.className = "hiddenRow";
}

/*
 * It will invoke the methods required to upload the SSL certificate.
 * Also it will hide other tab UI controls and make upload SSL UI controls to 
 * visible.
 */
function doUploadSSL()
{
	clearSSLUI();
	rowUploadSSL.className = "visibleRow";
	tabUploadSSL.style.fontWeight = "bold";
	btnUpload.onclick = validateSSLFile;
	reloadHelp();
	initUploadSSL();
}

function initUploadSSL()
{
	varCertificateInfo = eLang.getString("common", "STR_NOT_AVAILABLE");
	varPrivateKeyInfo = eLang.getString("common", "STR_NOT_AVAILABLE");
	varCertExists = 0;

	if (fnCookie.read("WebServer").indexOf("lighttpd") != -1) {
		fleCertBrowse.setAttribute("NAME", "/tmp/actualcert.pem");
		fleCertBrowse.name = "/tmp/actualcert.pem";
		fleKeyBrowse.setAttribute("NAME", "/tmp/actualprivkey.pem");
		fleKeyBrowse.name = "/tmp/actualprivkey.pem";
	} else {
		fleCertBrowse.setAttribute("NAME", "/tmp/actualcert.pem?16384");
		fleCertBrowse.name = "/tmp/actualcert.pem?16384";
		fleKeyBrowse.setAttribute("NAME", "/tmp/actualprivkey.pem?16384");
		fleKeyBrowse.name = "/tmp/actualprivkey.pem?16384";
	}

	fleCertBrowse.onkeydown = fleKeyBrowse.onkeydown = function(e) {
		if(!e){e = window.event;}
		if(e.keyCode != 13 && e.keyCode != 9) {
			return false;
		}
	}
	getSSLCertStatus();
}

function getSSLCertStatus()
{
	try{
		document.forms["frmCert"].reset();
		document.forms["frmPrivKey"].reset();
	}catch(e){
		alert(e);
	}
	xmit.get({url:"/rpc/getsslcertstatus.asp", onrcv:getSSLCertStatusRes, 
		status:""});
}

function getSSLCertStatusRes (arg)
{
	if(arg.HAPI_STATUS == 0) {
		SSLFILE_INFO = WEBVAR_JSONVAR_SSLCERTSTATUS.WEBVAR_STRUCTNAME_SSLCERTSTATUS[0];
		varCertExists = (SSLFILE_INFO.CERT_EXISTS != 0) ? 1 : 0;
		txtCertInfo.value = SSLFILE_INFO.CERT_INFO;
		//Ignores backspace functionality
		txtCertInfo.onkeydown = checkBackspace;
		txtKeyInfo.value = SSLFILE_INFO.PRIVKEY_INFO;
		//Ignores backspace functionality
		txtKeyInfo.onkeydown = checkBackspace;
	} else {
		var errstr = eLang.getString("common", "STR_CONF_SSL_FILE_GETVAL");
		errstr +=  (eLang.getString("common", "STR_IPMI_ERROR") + 
			GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	}
}

function validateSSLFile()
{
	if (top.user.isAdmin()) {
		if (eVal.isblank(fleCertBrowse.value)) {
			alert(eLang.getString("common", "STR_CONF_SSL_INVALID_CERT1"));
			fleCertBrowse.focus();
			return;
		} else {
			if (!eVal.endsWith(fleCertBrowse.value, ".pem")) {
				alert(eLang.getString("common", "STR_CONF_SSL_INVALID_CERT2"));
				fleCertBrowse.focus();
				return;
			}
		}

		if(eVal.isblank(fleKeyBrowse.value)) {
			alert(eLang.getString("common", "STR_CONF_SSL_INVALID_PRIVKEY1"));
			fleKeyBrowse.focus();
			return;
		} else {
			if(!eVal.endsWith(fleKeyBrowse.value, ".pem")) {
				alert(eLang.getString("common", "STR_CONF_SSL_INVALID_PRIVKEY2"));
				fleKeyBrowse.focus();
				return;
			}
		}
		uploadSSLFile();
	} else {
		alert(eLang.getString("common", "STR_CONF_ADMIN_PRIV"));
		return;
	}
}

function uploadSSLFile()
{
	if (!confirm(eLang.getString("common", "STR_CONF_SSL_UPLD_CONFIRM" + 
		varCertExists))) {
		return;
	} else {
		top.gSSLCert = true;
		btnUpload.disabled = true;
		showWait(true,"Uploading");
		document.forms["frmCert"].submit();
	}
}

function uploadComplete()
{
	if (top.gSSLCert) {
		top.gSSLCert = false;
		showWait(true, "Uploading");
		document.forms["frmPrivKey"].submit();
	} else {
		validateSSLCert();
	}
}

function validateSSLCert()
{
	xmit.get({url:"/rpc/validatesslcert.asp", onrcv:validateSSLCertRes, 
		status:""});
}

/*
 * This is the response function for validate SSL RPC. 
 * Need to check HAPI_STATUS, intimate end user if it returns non-zero value.
 * If success, check the validation code of the response data and give exact 
 * information to the user. If validation code is success, we have invoke the
 * method to restart the HTTPs service.
 * @param arg object, RPC response data from xmit library
 */
function validateSSLCertRes (arg)
{
	var sslValid = 0;
	if(arg.HAPI_STATUS == 0) {
		sslValid = WEBVAR_JSONVAR_VALIDATESSLCERT.WEBVAR_STRUCTNAME_VALIDATESSLCERT[0].SSL_VALID;
		switch (sslValid) {
		case 0:
			if (restartHTTPSService("STR_CONF_SSL_SAVE_SUCCESS")) {
				return true;
			}
			break;
		case 1: case 2:
		case 3: case 5:
		case 0x80: case 0x81:
			alert(eLang.getString("common", "STR_CONF_SSL_CERT_ERR" + sslValid));
			break;
		//case 4: case 6:
		default:
			alert(eLang.getString("common", "STR_CONF_SSL_CERT_ERR4"));
			break;
		}
	} else {
		errstr = eLang.getString("common", "STR_CONF_SSL_VALIDATE_ERR");
		errstr += (eLang.getString("common", "STR_IPMI_ERROR") + 
			GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	}
	btnUpload.disabled = false;
	getSSLCertStatus();
	//location.href = "configure_ssl.html";
}

function doGenerateSSL()
{
	clearSSLUI();
	rowGenerateSSL.className = "visibleRow";
	tabGenerateSSL.style.fontWeight = "bold";
	reloadHelp();
	clearGenerateSSL();
	fillKeyLength();
	btnGenerate.onclick = validateSSLCfg;
}

function clearGenerateSSL()
{
	txtCommonName.value = "";
	txtOrganization.value = "";
	txtOrgUnit.value = "";
	txtCity.value = "";
	txtState.value = "";
	txtCountry.value = "";
	txtEmailID.value = "";
	txtValid.value = "";
}

function fillKeyLength()
{
	lstKeyLength.innerHTML = "";
	lstKeyLength.add(new Option("512", 512), isIE ? 1 : null);
	lstKeyLength.add(new Option("1024", 1024), isIE ? 2 : null);
}

function validateSSLCfg()
{
	if((eVal.isblank(eVal.trim(txtCommonName.value))) || 
		(!eVal.specialCharacter(txtCommonName.value, "#$")) || 
		(eVal.isnum(txtCommonName.value))) {
		alert(eLang.getString("common", "STR_CONF_SSL_COMMON_NAME_ERR") + 
			eLang.getString("common", "STR_HELP_INFO"));
		txtCommonName.focus();
		return;
	}
	if((eVal.isblank(eVal.trim(txtOrganization.value))) || 
		(!eVal.specialCharacter(txtOrganization.value, "#$")) || 
		(eVal.isnum(txtOrganization.value))) {
		alert(eLang.getString("common", "STR_CONF_SSL_ORGANIZATION_ERR") + 
			eLang.getString("common", "STR_HELP_INFO"));
		txtOrganization.focus();
		return;
	}
	if((eVal.isblank(eVal.trim(txtOrgUnit.value))) || 
		(!eVal.specialCharacter(txtOrgUnit.value, "#$")) || 
		(eVal.isnum(txtOrgUnit.value))) {
		alert(eLang.getString("common", "STR_CONF_SSL_ORG_UNIT_ERR") + 
			eLang.getString("common", "STR_HELP_INFO"));
		txtOrgUnit.focus();
		return;
	}
	if((eVal.isblank(eVal.trim(txtCity.value))) || 
		(!eVal.specialCharacter(txtCity.value, "#$")) || 
		(eVal.isnum(txtCity.value))) {
		alert(eLang.getString("common", "STR_CONF_SSL_CITY_ERR") + 
			eLang.getString("common", "STR_HELP_INFO"));
		txtCity.focus();
		return;
	}
	if((eVal.isblank(eVal.trim(txtState.value))) || 
		(!eVal.specialCharacter(txtState.value, "#$")) || 
		(eVal.isnum(txtState.value))) {
		alert(eLang.getString("common", "STR_CONF_SSL_STATE_ERR") + 
			eLang.getString("common", "STR_HELP_INFO"));
		txtState.focus();
		return;
	}
	if((eVal.isblank(eVal.trim(txtCountry.value))) || 
		(!eVal.specialCharacter(txtCountry.value)) || 
		(eVal.isnum(txtCountry.value))) {
		alert(eLang.getString("common", "STR_CONF_SSL_COUNTRY_ERR") + 
			eLang.getString("common", "STR_HELP_INFO"));
		txtCountry.focus();
		return;
	}
	if(!eVal.email(txtEmailID.value)) {
		alert(eLang.getString("common", "STR_INVALID_EMAILADDR") + 
			eLang.getString("common", "STR_HELP_INFO"));
		txtEmailID.focus();
		return;
	}
	if(!eVal.isnumstr(txtValid.value, 1, 3650)) {
		alert(eLang.getString("common", "STR_CONF_SSL_VALID_FOR_ERR") + 
			eLang.getString("common", "STR_HELP_INFO"));
		txtValid.focus();
		return;
	}
	generateSSLCfg();
}

function generateSSLCfg()
{
	if (confirm(eLang.getString("common","STR_CONF_SSL_GNRT_CONFIRM"))) {
		var req = new xmit.getset({url:"/rpc/generatessl.asp", onrcv:generateSSLCfgRes, status:""});
		req.add("CommonName", txtCommonName.value);
		req.add("Organization", txtOrganization.value);
		req.add("OrgUnit", txtOrgUnit.value);
		req.add("City", txtCity.value);
		req.add("State", txtState.value);
		req.add("Country", txtCountry.value);
		req.add("EmailID", txtEmailID.value);
		req.add("ValidDays", txtValid.value);
		req.add("KeyLength", lstKeyLength.value);
		req.send();
		delete req;
	}
}

function generateSSLCfgRes(arg)
{
	if (arg.HAPI_STATUS != 0) {
		var errstr = eLang.getString("common", "STR_CONF_SSL_GNRT_CERT");
		errstr += (eLang.getString("common", "STR_IPMI_ERROR") + 
			GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	} else {
		restartHTTPSService("STR_CONF_SSL_GNRT_SUCCESS");
	}
}

var SSLCERT_DATA;
function doViewSSL()
{
	clearSSLUI();
	rowViewSSL.className = "visibleRow";
	tabViewSSL.style.fontWeight = "bold";
	reloadHelp();
	viewSSL();
}

function viewSSL()
{
	xmit.get({url:"/rpc/viewssl.asp", onrcv:viewSSLRes, status:""});
}

function viewSSLRes(arg)
{
	if (arg.HAPI_STATUS != 0) {
		var errstr = eLang.getString("common","STR_CONF_SSL_VIEW_CERT");
		errstr += (eLang.getString("common","STR_IPMI_ERROR") + GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	} else {
		SSLCERT_DATA = WEBVAR_JSONVAR_VIEWSSLCERT.WEBVAR_STRUCTNAME_VIEWSSLCERT[0];
		loadSSLCert();
	}
}

function loadSSLCert()
{
	lblVersion.innerHTML = SSLCERT_DATA.Version;
	lblSerialNo.innerHTML = SSLCERT_DATA.SerialNo;
	lblSignAlg.innerHTML = SSLCERT_DATA.SignAlg;
	lblPublicKey.innerHTML = SSLCERT_DATA.PublicKey;

	lblFromCommonName.innerHTML = SSLCERT_DATA.FromCommonName;
	lblFromOrganization.innerHTML = SSLCERT_DATA.FromOrganization;
	lblFromOrgUnit.innerHTML = SSLCERT_DATA.FromOrgUnit;
	lblFromCity.innerHTML = SSLCERT_DATA.FromCity;
	lblFromState.innerHTML = SSLCERT_DATA.FromState;
	lblFromCountry.innerHTML = SSLCERT_DATA.FromCountry;
	lblFromEmailID.innerHTML = SSLCERT_DATA.FromEmailID;

	lblValidFrom.innerHTML = SSLCERT_DATA.ValidFrom;
	lblValidTo.innerHTML = SSLCERT_DATA.ValidTo;

	lblToCommonName.innerHTML = SSLCERT_DATA.ToCommonName;
	lblToOrganization.innerHTML = SSLCERT_DATA.ToOrganization;
	lblToOrgUnit.innerHTML = SSLCERT_DATA.ToOrgUnit;
	lblToCity.innerHTML = SSLCERT_DATA.ToCity;
	lblToState.innerHTML = SSLCERT_DATA.ToState;
	lblToCountry.innerHTML = SSLCERT_DATA.ToCountry;
	lblToEmailID.innerHTML = SSLCERT_DATA.ToEmailID;
}