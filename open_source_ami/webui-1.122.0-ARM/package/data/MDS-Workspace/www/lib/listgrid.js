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

/*
**	Library : listgrid.js
**	author: chandrasekarr@amiindia.co.in
**
********************************************************
    Initial arguments
    -----------------

	  w				  : width %, px  (parseInt(document.body.clientWidth)-20), //+parseInt(vcolsa[2])
	  h				  : height %, px (parseInt(document.body.clientHeight)-180),
	  doAllowNoSelect : true,

    // to-do
	  doShowHeader    : true


	  Column arguments
	  ----------------

	  w         : width  %, px
	  h         : height %, px
	  textAlign : center, left, right

********************************************************
*/

//#define
IMGPATH = top.gOemDir+"res/img";

var lGrid = {};
var gTraceWnd = "";

//debug mode
lGrid.traceOn = false;

//detect browsers
var safari = (navigator.userAgent.toLowerCase().indexOf('safari')!=-1);
var firefox = (navigator.userAgent.toLowerCase().indexOf('firefox')!=-1);
var opera = (navigator.userAgent.toLowerCase().indexOf('opera')!=-1);
var ie = isIEBrowser(); //(window.ActiveXObject); This is not working in latest IE browser
var chrome = (navigator.userAgent.toLowerCase().indexOf('chrome')!=-1);

/*
 * This will open a window to show the debug messages for the listgrid.
 * This can be invoked only when the flag traceOn is set.
 */
if(lGrid.traceOn) {
	gTraceWnd = window.open("", "tracer", 
		"width=800, height=600, scrollbars=yes");
}

rCln = [];
lGrid.selected = [];

var listgrid = function(arg)
{
	lGrid.traceClear();
	lGrid.trace('Listgrid Constructor starting');
	lGrid.__constructor(arg);
	return lGrid;
}

//listgrid constructor object
lGrid.__constructor = function(arg)
{
	this.table = document.createElement('div');
	this.container = document.createElement('table');

	this.table.appendChild(this.container);
	this.container.className = "listgrid";

	//delete all spacings and margins and handle them by css
	cellspacing = document.createAttribute('cellspacing');
	cellspacing.nodeValue = 0;
	cellpadding = document.createAttribute('cellpadding');
	cellpadding.nodeValue = 0;
	this.container.setAttributeNode(cellspacing);
	this.container.setAttributeNode(cellpadding);

	_this_container_header = this.container.createTHead();
	this.container.header = _this_container_header.insertRow(0);
	this.container.header.className = 'head';

	tBody = document.createElement('tbody');
	this.container.appendChild(tBody);

	this.message = arg.msg ? arg.msg : eLang.getString("common", "STR_DATA") + 
		eLang.getString("common", "STR_NOT_AVAILABLE");
	this.heightLess = arg.h?false:true;
	tBody.style.height = arg.h?arg.h:"";
	this.defaultHeight = arg.h?arg.h:""; //Added by Manoj.C for Bug 11530

	this.container.style.width = "100%";
	this.table.style.width = arg.w?arg.w:"100%";
	if (tBody.style.height != "") {
		if (firefox) {
			//this.table.style.height = this.defaultHeight;

			//solved BMC GUI event log table end separate line is poorly aligned in Firefox web browser. (Firefox 7.0)
			this.container.style.width = "99%";
			this.table.style.height = arg.h?arg.h:"";
			this.table.style.overflowX = 'hidden';
			this.table.style.overflowY = 'auto';
		} else if (ie) {
			//this.container.header.style.position = 'absolute';
			tBody.style.height = "";
			//this.container.header.style.top = (this.container.offsetTop-17)+'px';
			this.container.style.width = "99%";
			this.table.style.height = arg.h?arg.h:"";
			this.table.style.overflowX = 'hidden';
			this.table.style.overflowY = 'auto';
		} else if(safari || opera || chrome) {
			//safari, opera, chrome and other safari based render engines
			this.container.header.style.display = 'inline-table';
			this.container.header.style.width = '100%';
			tBody.style.display = 'block';
			tBody.style.width = '100%';
		}
	}

	lGrid.trace("Within constructor, this.table.style.height " + this.table + " = " + this.table.style.height);
	lGrid.trace("Listgrid .. ActiveXObject out "+this.table+" = "+this.table.style.height);
	for (i in arg) {
		if(i!='w' && i!='h') {
			lGrid[i] = arg[i];
		}
	}
}

//add heading columns to the listgrid
lGrid.addCol = function(arg)
{
	//var th = this.container.header.insertCell(-1);
	var th = document.createElement('th');
	this.container.header.appendChild(th);

	arg.value = arg.value?arg.value:arg.text;
	arg.sort = (arg.sort == false) ? arg.sort : true;
	lGrid.trace("arg.sort::" + arg.sort);
	if (arg.sort) {
		applyInnerHTML(th, "&nbsp; "+arg.value + " &nbsp; <img src='"+IMGPATH+"/sortup.gif' title='Ascending' alt='Ascending' />");
	} else {
		applyInnerHTML(th, arg.value);
	}

	th.image = th.getElementsByTagName('img')[0];
	th.idno = this.container.header.cells.length-1;
	th.fieldType = arg.fieldType;
	th.fieldName = arg.fieldName;

	//fitColumnsToHeader() fixes column width issues for chrome & safari - Manoj 12thJan2010 (Bug 11177)
	if(opera)
	{
		if(arg.w.toString().indexOf('%')==-1)
		{
			th.style.width = arg.w+'px';
		}else
		{
			var pw = parseInt(arg.w);
			var w = (pw/100) * this.container.header.offsetWidth;
			th.style.width = w+'px';
		}
	}else
	{
		th.style.width = arg.w.toString().indexOf('%')==-1?arg.w+'px':arg.w;
	}

	th.className = 'head';
	th.style.whiteSpace = 'nowrap';

	th.onmouseover = function()
	{
		this.className = 'headHover';
	}

	th.onmouseout = function()
	{
		this.className = 'head';
	}

	var thParent = this; //This reference is needed so that we can call this.fitColumnsToHeader() from inside th.onclick - Manoj (Bug 11177)
	th.onclick = function()
	{
		try {
			lGrid.table.scrollTop = "0";
		} catch(e) {
			alert(e.description);
		}

		if (rCln.length == 0) {
			//If there is no data in the listgrid sorting not required.
			return;
		}

		/*
		 * Before sorting process starts, it will call onbeforesort function, 
		 * only when it is defined in implemenation part.
		 */
		if (lGrid.onbeforesort) {
			lGrid.onbeforesort();
		}

		lGrid.traceClear();
		this.image.src = (this.image.src.indexOf("sortup") == -1) ? 
			IMGPATH+"/sortup.gif" : IMGPATH+"/sortdown.gif";

		//prepare for the sorting combat [-O<]
		showWait(true,"Sorting");

		lGrid.trace ("rCln length::" + rCln.length);
		//using rCln now;
		var _srtAry = [];
		var q = 0;
		for(i = 0; i<rCln.length; i++) {
			_srtAry[i] = rCln[i].cells[this.idno].innerHTML.replace(/<wbr\/?>/ig,'');
			//lGrid.trace(this.idno);
		}

		//now sort
		lGrid.trace("Field data::" + this.innerHTML);
		lGrid.trace("Field type::" + this.fieldType);
		lGrid.trace("Before Sorting::" + _srtAry);
		_srtAry = (this.fieldType == 2) ? _srtAry.sort(sortNumber) : 
			_srtAry.sort();
		lGrid.trace("After Sorting::" + _srtAry);

		//now remove previous fields and add current
		var fSrtAry = [];
		var disableAry = [];
		var bkCln = rCln;

		//take single sorted array
		if(this.image.src.indexOf("sortup") != -1) {
			this.image.title = "Ascending";
			this.image.alt = "Ascending";
	
			for(i = 0; i <_srtAry.length; i++) {
				lGrid.trace("i = "+i);
				//check which row the element was resided
				for(rs = 0; rs < bkCln.length; rs++) {
					if(_srtAry[i] == bkCln[rs].cells[this.idno].innerHTML.replace(/<wbr\/?>/ig, "")) {
						//check if row is disabled? if so store it to disable again after sorting
						disableAry[i] = bkCln[rs].disabled;

						fSrtAry[i] = [];
						//copy that row content to an array
						for (h = 0; h<lGrid.container.header.cells.length; h++) {
							fSrtAry[i][h] = bkCln[rs].cells[h].innerHTML.replace(/<wbr\/?>/ig, "");
	
							//in order to handle LDAP Search BN string (contain ",")
							fSrtAry[i][h]=fSrtAry[i][h].toString().replace(/,/g, ":COMMA:");
						}

						lGrid.trace("fSrtAry within if ascending[" + i + "] = " + fSrtAry[i]);

						//remove the cell entry
						bkCln[rs].cells[this.idno].innerHTML = "";
						break;
					}
				}
				lGrid.trace("Disabled array::" + disableAry);
			}
		} else {
			this.image.title="Descending";
			this.image.alt="Descending";

			for(i = _srtAry.length-1; i >= 0; i--) {
				lGrid.trace("i = "+i);
				//check which row the element was resided
				for(rs = 0; rs < bkCln.length; rs++) {
					if(_srtAry[i] == bkCln[rs].cells[this.idno].innerHTML.replace(/<wbr\/?>/ig,"")) {
						disableAry[(_srtAry.length-1)-i] = bkCln[rs].disabled;
						fSrtAry[(_srtAry.length-1)-i] = [];

						//copy that row content to an array
						for(h = 0; h<lGrid.container.header.cells.length; h++) {
							fSrtAry[(_srtAry.length-1)-i][h] = bkCln[rs].cells[h].innerHTML.replace(/<wbr\/?>/ig, "");

							//in order to handle LDAP Search BN string (contain ",")
							fSrtAry[(_srtAry.length-1)-i][h]=fSrtAry[(_srtAry.length-1)-i][h].toString().replace(/,/g, ":COMMA:");
						}
						lGrid.trace("fSrtAry within if descending[" + ((_srtAry.length-1)-i) + "] = " + fSrtAry[(_srtAry.length-1)-i]);

						//remove the cell entry to stop any further comparison
						bkCln[rs].cells[this.idno].innerHTML = "";
						break;
					}
				}
				lGrid.trace("Disabled array in Descending::" + disableAry);
			}
		}

		//delete allData
		lGrid.clear(false);

		//now add all data to the table.
		for (i=0; i<fSrtAry.length; i++) {
			var curtr = lGrid.addRow(fSrtAry[i].join(','));
			curtr.setEnabled(!disableAry[i]);
		}
		if(safari || chrome) thParent.fitColumnsToHeader();
		//GC and mem leaks
		delete bkCln;
		delete _srtAry;
		delete fSrtAry;
		showWait(false);

		/*
		 * After sorting process completed, it will call onaftersort function, 
		 * only when it is defined in implemenation part.
		 */
		if (lGrid.onaftersort) {
			lGrid.onaftersort();
		}
		// Thank you ! god!!!
	}

	/*
	 * In some cases, listgrid header doesnot need the sorting process. So 
	 * using the sort flag in the argument we can make use of it.
	 */
	if (!arg.sort) {
		th.onclick = function (){};
		th.onmouseover = function(){};
		th.onmouseout = function(){};
	}

	if(arg.h)
	{
		th.style.height = (arg.h.toString().indexOf('%')==-1?arg.h+'px':arg.h);
		lGrid.trace("Header height out" + th + " = " + th.style.height);
	}

	th.style.textAlign = (arg.textAlign && arg.textAlign!=undefined)?arg.textAlign:'left';

	if (safari || opera || chrome) {
		//safari, opera, chrome and other safari based render engines
		this.container.header.style.width = this.container.header.offsetWidth+'px';
	}

	//fire onload event;
	this.onload();
}

//this list acts loaded when headers are added
lGrid.onload = function()
{
	//IE fix
	if(window.ActiveXObject){
		if(!this.heightLess)
		   this.container.header.style.top = (elmOffset(this.container,'Top')-this.container.header.offsetHeight/2)+'px';
		if(document.body.offsetWidth<this.container.header.offsetWidth)
			this.table.style.width = (this.container.header.offsetWidth+20)+'px';
	}
}

/*
 * This function will merge all the columns of the listgrid and will draw the
 * passed string in the center of this newly added single row.
 * @param : arg string, string to be drawn. 
 * [Ex : When any list grid has no records, it will
 * display "Data not available" message using this function. ]
 */
 lGrid.addSingleRow = function(arg) {
	var tr = document.createElement("tr");
	this.container.tBodies[0].appendChild(tr);
	if (firefox) {
		tBody.style.height = this.defaultHeight;
	}
	var td = tr.insertCell(0);
	td.colSpan = this.container.header.cells.length;
	td.width = this.table.offsetWidth + "px";
	td.height = (this.table.style.height == "") ? 
		"100px" : this.table.style.height;
	td.align = "center";
	td.vAlign = "middle";
	applyInnerHTML(td, arg);
}

//add a new row
// arg - comma separated values
lGrid.addRow = function(arg)
{
	//var tr = this.container.insertRow(this.container.rows.length);
	var tr = document.createElement("tr");
	this.container.tBodies[0].appendChild(tr);
	rCln.push(tr);

	tr.className = "normal";
	if ((window.ActiveXObject == undefined) && 
		(navigator.appName.indexOf("Mozilla") == -1)) {
		tr.style.width = this.container.header.offsetWidth + "px";
	}
	sArgs = arg.split(",");

	for (s = 0; s < sArgs.length; s++) {
		var td = tr.insertCell(s);
		var contentString = sArgs[s].toString().replace(/:COMMA:/g, ",");
		if(contentString == "") {
			contentString = "~";
		}

		applyInnerHTML(td, contentString);
		//planning to use this in really long words having an optional argument
		//.wordWrap(10,"<wbr/>",true);
		try {
			td.style.textAlign = lGrid.container.header.cells[s].style.textAlign;
		} catch(e) {
			//not a problem set it as left
			td.style.textAlign = 'left';
		}

		td.style.paddingRight = '1em';
		td.style.paddingLeft = '1em';
		//safari, opera, chrome and other safari related render engines
		if (!this.heightLess) {
			if (safari || opera || chrome) {
				//alert(this.container.header.cells[s].offsetWidth);
				td.style.width = this.container.header.cells[s].offsetWidth+'px';
				this.container.header.cells[s].style.width = td.offsetWidth+'px';
				/*if (!opera) {
					this.container.header.cells[s].style.width = td.offsetWidth+'px';
				}*/
			}
		}
	}

	if (!this.doAllowNoSelect) {
		tr.onclick = function()
		{
			//allow multi selection later
			if (this.disabled != true) {
				lGrid.selectRow(this);
			}
		}

		tr.ondblclick = function()
		{
			if (this.disabled != true) {
				lGrid.selectRow(this);
				if (lGrid.ondblclick) {
					lGrid.ondblclick(this);
				}
			}
		}

		tr.ondisabledclick = tr.onclick;

		//hook document keydown event for listgrid
		//support for mozilla firefox
		//IE can support directly from lGrid.table
		//But this is cross-browser override
	
		document.onkeydown = function(e)
		{
			if (lGrid.selected.length==0) {
				return;
			}
			if ($('descFrame')) {
				return;
			}
	
			if (!e) {
				e = window.event;
			}
			if (e.keyCode == 38) {
				//do move up
				var prevSibling = lGrid.selected[0].previousSibling;
				while (prevSibling) {
					if(prevSibling.disabled != true) {
						lGrid.selectRow(prevSibling);
						break;
					} else {
						prevSibling = prevSibling.previousSibling;
					}
				}
			}
	
			if(e.keyCode==40) {
				//do move down
				var nxtSibling = lGrid.selected[0].nextSibling;
				while (nxtSibling) {
					if(nxtSibling.disabled != true) {
						lGrid.selectRow(nxtSibling);
						break;
					} else {
						nxtSibling = nxtSibling.nextSibling;
					}
				}
			}
		}
	} else {
		tr.ondisabledclick = doNothing;
	}

	tr.setEnabled = function(bool)
	{
		if (!bool) {
			this.onclick = doNothing;
			this.style.color = "#999";
		} else {
			this.onclick = this.ondisabledclick;
			this.style.color = "#000";
		}
		this.disabled = !bool;
	}

	//fire onload event;
	this.onload();

	return tr;
}


//select a given row in the listgrid
lGrid.selectRow = function(tRow)
{
	for (i = 0; i < rCln.length; i++) {
		rCln[i].className = "normal";
	}

	tRow.className = "selected";
	tRow.focus();

	lGrid.selected = [];
	lGrid.selected[0] = tRow;
	lGrid.trace(tRow + " clicked ");

	//handle following event if any
	if (lGrid.ontableselect) {
		lGrid.ontableselect();
	}
	if (lGrid.onrowselect != undefined) {
		lGrid.onrowselect(tRow);
	}
}

//load json object elements - compatible with DOMAPI 4
lGrid.loadFromJson = function(args)
{
	if (this.table.offsetHeight != 0) {
		this.previousHeight = this.table.offsetHeight;
	} else {
		this.previousHeight = this.table.style.height;
	}
	lGrid.trace("this.table.offsetHeight" + this.table + " = " + this.table.offsetHeight);
	lGrid.trace("within loadfromJson, this.table.style.height " + this.table + " = " + this.table.style.height);
	lGrid.trace("previousHeight out " + this + " = " + this.previousHeight);
	this.clear(true);

	for(c=0; c<args.cols.length; c++)
	{
		this.addCol(args.cols[c]);
	}
	if(opera)
	{
		//Opera added the columns headers one below another (instead of side by side), which it corrected
		//by itself while clearing the table. This fix forces a clear before rows are loaded. - Manoj
		this.addRow("dummy");
		this.clear(false);
	}

	if(args.rows)
	{
		// if 100 rows browser can do it fast without hanging..
		if (args.rows.length == 0) {	//listgrid having no value.
			var strMessage = "<div id='divMessage' class='errorDisplay'"
				+ "style=color:#000;text-transform:none;font-variant:normal>" +
				this.message + "</div>";
			this.addSingleRow(strMessage);
			showWait(false);
		} else if (args.rows.length <= 100) {
			showWait(true,"Populating");
			for (r = 0; r < args.rows.length; r++) {
				lGrid.trace("Row " + r + "disabled::" + args.rows[r].disabled);
				var tCell = [];
				for (cell = 0; cell < args.rows[r].cells.length; cell++) {
					if(args.rows[r].cells[cell]) {
						var txt = args.rows[r].cells[cell].text;
						txt = txt.toString().replace(/,/g, ":COMMA:");
						tCell.push(txt);
					} else {
						tCell.push("~");
					}
				}
				this.addRow(tCell.join(",")).setEnabled(!args.rows[r].disabled);
			}
			showWait(false);

			if(firefox) 
				this.adjustRowHeights();
			if(safari || chrome) 
				this.fitColumnsToHeader();
		} else {
			// if more than 1000 rows span and do it
			this.spanAndLoad(args, 0);
		}
	}
}

lGrid.spanAndLoad = function(args, index)
{
	if(index<args.rows.length)
	{
		r = index;
		var tCell = [];
		for(cell=0; cell<args.rows[r].cells.length; cell++)
		{
			if(args.rows[r].cells[cell])
			{
				var txt = args.rows[r].cells[cell].text;
				txt = txt.toString().replace(/,/g,':COMMA:');
				tCell.push(txt);
			}
			else
			{
				tCell.push('~');
			}
		}
		this.addRow(tCell.join(','));
		setTimeout(function(){lGrid.spanAndLoad(args, ++index)},3);
		showWait(true,'Populating');
	}
	else
	{
		showWait(false);
	}

	if(firefox) 
		this.adjustRowHeights();

	if(safari || chrome) 
		this.fitColumnsToHeader();
}

lGrid.adjustRowHeights = function()
{
	//This function adjusts the height of the tBody so that
	//the table height is shrinked to fit instead of rows getting stretched.
	if(!this.heightLess)
	{
		lGrid.trace("within adjust row heights");
		lGrid.trace("adjustRowHeights just entering, this.table.style.height " + this.table + " = " + this.table.style.height);
		this.container.tBodies[0].style.height = '0px';
		//if row length less than 50, calculation of the actual offsetHeight must be fast.
		lGrid.trace("adjustRowHeights after init, this.table.style.height " + this.table + " = " + this.table.style.height);
		if(this.container.tBodies[0].rows.length < 50)
		{
			lGrid.trace("adjustRowHeights this.container.tBodies[0].rows.length = " + this.container.tBodies[0].rows.length);
			var calculatedRowHeight = 3; //initial bordering 3 pixels top, bottom
			for(var r=0; r<this.container.tBodies[0].rows.length; r++)
			{
				if (this.container.tBodies[0].rows[r].offsetHeight != 0) {
					calculatedRowHeight += this.container.tBodies[0].rows[r].offsetHeight;
				} else {
					calculatedRowHeight += 20;
				}
				lGrid.trace("adjustRowHeights for loop, this.container.tBodies[0].rows[r].offsetHeight = " + this.container.tBodies[0].rows[r].offsetHeight);
				lGrid.trace("adjustRowHeights for loop, this.table.style.height " + this.table + " = " + this.table.style.height);
			}

			if((calculatedRowHeight < parseInt(this.defaultHeight)) && (calculatedRowHeight != 3))
			{
				this.table.style.height = this.previousHeight+'px';
				this.container.tBodies[0].style.height = calculatedRowHeight+'px';
				lGrid.trace("adjustRowHeights if check, this.previousHeight = " + this.previousHeight);
				lGrid.trace("adjustRowHeights if check, this.container.tBodies[0].style.height = " + this.container.tBodies[0].style.height);
				lGrid.trace("adjustRowHeights if check, this.table.style.height " + this.table + " = " + this.table.style.height);
				lGrid.trace("adjustRowHeights if check, calculatedRowHeight = " + calculatedRowHeight);
			}
			else
			{
				this.container.tBodies[0].style.height = this.defaultHeight;
				lGrid.trace("adjustRowHeights some else, this.table.style.height " + this.table + " = " + this.table.style.height);
			}
		}
		else
		{
			lGrid.trace("adjustRowHeights else part, this.table.style.height " + this.table + " = " + this.table.style.height);
			this.container.tBodies[0].style.height = this.defaultHeight;
		}
		lGrid.trace("within adjustRowHeights, this.table.style.height " + this.table + " = " + this.table.style.height);
		lGrid.trace("this.container.tBodies[0].style.height " + this.container + " = " + this.container.tBodies[0].style.height);
	}
}

lGrid.fitColumnsToHeader = function()
{
	//This function resets the widths of columns to match the header row columns
	//Needed because some browsers automatically size columns when new rows are added - Manoj (Bug 11177)
	if(this.container.rows.length > 1 && this.container.header)
	{
		var numCols = this.container.header.cells.length;
		for(var i = 0; i < numCols; i++)
		{
			this.container.rows[1].cells[i].style.width = this.container.header.cells[i].style.width;
		}
	}
}

//Delete all rows in the listgrid
lGrid.clear = function(headDelete)
{
	if(headDelete)
	{
		while(lGrid.container.header.cells.length)
			lGrid.container.header.deleteCell(lGrid.container.header.cells.length-1);
	}

	while(lGrid.container.rows.length>1)
	{
		//remove all rows
		lGrid.container.deleteRow(lGrid.container.rows.length-1);
	}

	while(rCln.length) rCln.pop();
	delete rCln;
	rCln = [];
}

//get any selected row
lGrid.getRow = function(tr)
{
	return (tr);
}

//find function
lGrid.getRowByValue = function(fieldName,value)
{
	var headers = lGrid.container.header.cells;
	var colno = false;
	for(hd=0;hd<headers.length;hd++)
	{
		if(headers[hd].fieldName==fieldName)
		{
			colno = headers[hd].idno;
			break;
		}
	}

	if(colno!=false)
	{
		for(rw=0;rw<lGrid.container.rows.length;rw++)
		{
			if(lGrid.container.rows[rw].cells[colno].innerHTML.replace(/<wbr\/?>/ig,'')==value)
			{
				return lGrid.container.rows[rw];
			}
		}
	}else
	{
		alert("Column not found!");
	}
}

/*
 * This will send the contents(with newline) to the debug window.
 * @param stack string, content to be displayed in debug window.
 */
lGrid.trace = function(stack)
{
	if(lGrid.traceOn) {
		if (gTraceWnd == null) {
			gTraceWnd = window.open("", "tracer", "width=800, height=600, scrollbars=yes");
		}

		try {
			gTraceWnd.document.writeln(stack + "<br>");
			gTraceWnd.scrollTo(0,10000000);
		} catch(e) {
			//alert (e.description);
		}
	}
}

/*
 * This will clear the contents of the Debug window.
 */
lGrid.traceClear = function()
{
	if(lGrid.traceOn) {
		try {
			if(gTraceWnd != null) {
				gTraceWnd.document.body.innerHTML = "";
			}
		} catch (e) {
			//alert (e.description);
		}
	}
}

//do a wordwrap with <wbr> to all strings inside table to prevent long text
// override

String.prototype.wordWrap = function(m, b, c){
	var i, j, s, r = this.split("\n");
	if(m > 0) for(i in r){
		for(s = r[i], r[i] = ""; s.length > m;
			j = c ? m : (j = s.substr(0, m).match(/\S*$/)).input.length - j[0].length
			|| j.input.length + (j = s.substr(m).match(/^\S*/)).input.length + j[0].length,
			r[i] += s.substr(0, j) + ((s = s.substr(j)).length ? b : "")
		);
		r[i] += s;
	}
	return r.join("\n");
};


/* happily and safely deprecated
 *
if(window.ActiveXObject){
	window.onresize = function()
	{
		try{
			lGrid.container.header.style.top = (lGrid.table.parentNode.offsetTop - 17)+"px";
		}catch(e){
			// before table load event..
		}
	}

	top.window.document.body.onresize = function()
	{
		lGrid.container.header.style.top = (lGrid.table.parentNode.offsetTop - 17)+"px";
			//handle custom events

			if(lGrid.onpageresize)
			{
				lGrid.onpageresize();
			}
	}
}

*/

//for sorting number array
function sortNumber(a,b)
{
	//lGrid.trace("a::" + a + ", b::" + b);
	return a-b;
}

//for working around IE8 innerHTML limitation
function applyInnerHTML(element, innerHTML){
    if(ie){
        if((innerHTML.indexOf("<") >= 0) || (innerHTML.indexOf("&deg;") != -1))
			{
            //"<" indicates it's not a safe string, so put it in a span and add it to element
            var spanElement = document.createElement("span");
            spanElement.innerHTML = innerHTML;
            element.appendChild(spanElement);
        } else {
            //plain text, add directly
            //replace common entities
            if(innerHTML.indexOf("&") > 0){
                innerHTML.replace("&quot;", "\"");
                innerHTML.replace("&apos;", "'");
                innerHTML.replace("&lt;", "<");
                innerHTML.replace("&gt;", ">");
                innerHTML.replace("&nbsp;", "\u00a0");
                innerHTML.replace("&amp;", "&");
            }
            element.appendChild(document.createTextNode(innerHTML));
        }
    } else {
        element.innerHTML = innerHTML;
    }
}

/* 
 * This method is used to identify the client browser
 * @return true, if its IE, else false.
 */
function isIEBrowser() {
	var ua = window.navigator.userAgent;
	var msie = ua.indexOf("MSIE ");
	var trident = ua.indexOf("Trident/");
	return ((msie > 0) || (trident > 0)) ? true : false;
}

var doNothing = function(){return false;}

listgrid.prototype.loaded = true;