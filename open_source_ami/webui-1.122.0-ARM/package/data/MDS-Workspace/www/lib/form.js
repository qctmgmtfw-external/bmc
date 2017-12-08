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

/* Module: Dynamic Table Form
 * Version: 1.0
 * Author	: Chandrasekar.R
 * Email 	: chandrasekarr@amiindia.co.in
 * Filename: ./lib/form.js   Comments: Contains logic for Table based HTML Form
 */

// javascript class form

function form(name,method,action,className)        // main paramter name,method,action,className
{
	this.frmObj = document.createElement('form');
	this.tblObj = document.createElement('table');
	this.tblObj.cellspacing = 0;
	this.tblObj.cellpadding = 0;
	this.tblObj.border = 0;
	this.tblObj.width = '100%';
	this.tBdyObj = document.createElement('tbody');
	this.tBdyObj.className = className;

	this.frmObj.id = name;
	this.frmObj.name = name;
	this.frmObj.method = method;
	this.frmObj.action = action;
	this.className = className;
}

form.prototype.addRow = function(tag,content,desc)
{
	var tr = document.createElement('tr');
	tr.style.height = "25px";
	tr.className = this.className;

	//left side
	var td1 = document.createElement('td');
	td1.style.width = "40%"
	td1.className = this.className+'_left';
	td1.innerHTML = tag;

	//right side
	var td2 = document.createElement('td');
	td2.className = this.className+'_right';
	if(typeof content=='string' || typeof content=='number')
		td2.innerHTML = content;
	else
		td2.appendChild(content);

	if(desc!='' && desc!=undefined)
	{
		var small = document.createElement('label');
		small.className = this.className;
		small.innerHTML = "&nbsp;" + desc;
		td2.appendChild(small);
	}

	tr.appendChild(td1);
	tr.appendChild(td2);
	this.tBdyObj.appendChild(tr);
	return tr;
}

form.prototype.addTextField = function(tag,name,value,param,className,extra)  //param as array
{
	var input = document.createElement('input');
	input.name = name;
	input.type = 'text';
	input.id = name;
	input.value = value;

	if (className != undefined)
		input.className = className;
	else
		input.className = this.className;

	for(i in param)
	{
		input[i] = param[i];
	}
	this.addRow(tag,input,extra);
	return input;
}

form.prototype.addPasswordField = function(tag,name,value,param,className,extra)  //param as array
{
	var input = document.createElement('input');
	input.name = name;
	input.type = 'password';
	input.id = name;
	input.value = value;

	if (className != undefined)
		input.className = className;
	else
		input.className = this.className;

	for(i in param)
	{
		input[i] = param[i];
	}
	this.addRow(tag,input,extra);
	return input;
}

form.prototype.addTextArea = function(tag,name,value,cols,rows,param,extra)  //param as array
{
	var textarea = document.createElement('textarea');
	textarea.name = name;
	textarea.cols = cols;
	textarea.rows = rows;
	textarea.id = name;
	textarea.value = value;
	textarea.className = this.className;
	for(i=0; i<param.length; i++)
	{
		var att = document.createAttribute(i);
		if(navigator.appName.indexOf('Microsoft')!=-1)
		{
			att.nodeValue = new Function(param[i]);
		}
		else
		{
			att.nodeValue = param[i];
		}
		textarea.setAttributeNode(att);
	}
	this.addRow(tag,textarea,extra);
	return textarea;
}

isIE = (navigator.appName.indexOf('Microsoft')!=-1);

form.prototype.addSelectBox = function(tag,name,valueary,seldef,param,initval,className,extra)
{
	var select = document.createElement('select');
	select.name = name;
	select.id = name;
	if(className != undefined)
		select.className = className;
	else
		select.className = this.className;
 
	if(param!=undefined)
	{
		for(i=0; i<param.length; i++)
		{
			var att = document.createAttribute(i);
			if(navigator.appName.indexOf('Microsoft')!=-1)
			{
				att.nodeValue = new Function(param[i]);
			}
			else
			{
				att.nodeValue = param[i];
			}
			select.setAttributeNode(att);
		}
	}

	optind = 0;
	if(initval==true)
	{
		select.add(new Option("--Select--",""),isIE?optind++:null);
	}

	for(j in valueary)
	{
		select.add(new Option(valueary[j],j),isIE?optind++:null);
	}

	if(seldef!=undefined)
	{
		select.value = seldef;
	}
	this.addRow(tag,select,extra);
	return select;
}

form.prototype.addSubmitForm = function(tag,name,value,param,extra)  //param as array
{
	var input = document.createElement('input');
	input.name = name;
	input.type = 'submit';
	input.id = name;
	input.value = value;
	input.className = this.className;
	for(i=0; i<param.length; i++)
	{
		var att = document.createAttribute(i);
		if(navigator.appName.indexOf('Microsoft')!=-1)
		{
			att.nodeValue = new Function(param[i]);
		}
		else
		{
			att.nodeValue = param[i];
		}
		input.setAttributeNode(att);
	}
	this.addRow(tag,input,extra);
	return input;
}

form.prototype.addButtonField = function(tag,name,value,param,extra)  //param as array
{
	var input = document.createElement('input');
	input.name = name;
	input.type = 'button';
	input.id = name;
	input.value = value;
	input.className = this.className;

	for(i=0; i<param.length; i++)
	{
		var att = document.createAttribute(i);
		if(navigator.appName.indexOf('Microsoft')!=-1)
		{
			att.nodeValue = new Function(param[i]);
		}
		else
		{
			att.nodeValue = param[i];
		}
		input.setAttributeNode(att);
	}
	this.addRow(tag,input,extra);
	return input;
}

form.prototype.addHiddenField = function(name,value)  //param as array
{
	var input = document.createElement('input');
	input.name = name;
	input.type = 'hidden';
	input.id = name;
	input.value = value;
	this.frmObj.appendChild(input);
	return input;
}

form.prototype.addRadioField = function(tag, name, valueary, linebreak, chked, 
	param, extra) 	//param as array
{
	var span = document.createElement('span');
	span.id = name + 'Span';
	for (j in valueary) {
		var input = "<input ";
		input += " name='"+name+"'";
		input += " type='radio'";
		input += " id='"+j+"'";
		input += " value='"+valueary[j]+"'";
		input += " class='"+this.className+"'";
		
		if (chked==j) {
			input += " checked";
		}
		
		for (i in param) {
			input += i+"='"+param[i]+"'";
		}
		
		input += "/>";
		var lbl = "<label ";
		lbl += " for ='"+j+"'";
		lbl += ">";
		lbl += valueary[j];
		lbl += "</label>";
		span.innerHTML += input + lbl;
		if (linebreak) {
			span.innerHTML += "<br/>";
		}
	}
	this.addRow(tag,span,extra);
	return span.firstChild; 
}

form.prototype.addCheckBox = function(tag,name,valueary,linebreak,chkedary,param,extra)
{
	var checkBoxes={};
	var span = document.createElement('span');
	var chked = false;
	span.id = name + 'Span';

	for(j in valueary)
	{
		var input = document.createElement('input');
		input.name = j;
		input.type = 'checkbox';
		input.id = j;
		input.value = valueary[j];
		input.className = this.className;

		if (chkedary != undefined || chkedary != null){
			chked = ((chkedary.indexOf(j) != -1) ? true : false);
		}
		if(chked == true)
		{
			input.checked = true;
		}			// end checks

		for(i in param)		// add all param
		{
			input[i] = param[i];
		}
		var lbl = document.createElement('label');
		lbl.innerHTML = valueary[j];
		lbl.htmlFor = j;

		span.appendChild(input);
		span.appendChild(lbl);
		if(linebreak)
		{
			var br = document.createElement('br');
			span.appendChild(br);
		}

		checkBoxes[input.name] = input;
	}
	this.addRow(tag,span,extra);
	return checkBoxes;
}

form.prototype.display = function()
{
	this.tblObj.appendChild(this.tBdyObj);
	this.frmObj.appendChild(this.tblObj);
	return this.frmObj;
}
