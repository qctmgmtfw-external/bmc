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
	Module	: Canvas Graph
	Version	: 1.0
	Author	: Chandrasekar.R
	Email 	: chandrasekarr@amiindia.co.in


 - All functions that start with __ is supposed to be considered as private
 - All functions that start with _ is supposed to be considered as protected
 - All functions that does not start with _ is supposed to be considered as public

*/

var SECS = 1;
var MINS = SECS * 60;
var HOURS = MINS * 60;
var DAYS = HOURS * 24;
var WEEKS = DAYS * 7;
var MONTHS = DAYS * 30; //round to 30
var YEARS = MONTHS * 365;

var graph = {

	/*
		Function _init is constructor to graph object.
		This must be first called on initializing a graph instance.

	Arguments:
		id 			- HTML canvas element id which will be canvas for the graph
		gphSettings - JSON Object type. Canvas settings will given through this
					  argument.
	*/

	_init				: function(id, gphSettings)
	{
		this.canvas = document.getElementById(id);
		this.dataCanvas = this.canvas.cloneNode(true);
		this.dataCanvas.style.position = 'absolute';
		this.dataCanvas.style.left = elmOffset(this.canvas,'Left');
		this.dataCanvas.style.top = elmOffset(this.canvas,'Top');
		document.body.appendChild(this.dataCanvas);
		this.context = this.canvas.getContext('2d');
		this.dataContext = this.dataCanvas.getContext('2d');
		this.settings = gphSettings;

		if(this.settings._gX==undefined)
			this.settings._gX = 70;
		if(this.settings._gY==undefined)
			this.settings._gY = 40;
		this.settings.borderColor = "rgb(1,1,1)";
		if(this.settings.borderWidth==undefined)
			this.settings.borderWidth = 1;
		this.settings.scaleHeight = 5;
		this.settings.legendGap = 15;
		CanvasTextFunctions.enable(this.context);
		CanvasTextFunctions.enable(this.dataContext);
		this.__prepareBG();
		if(events!=undefined)
		{
			events.register("resize",graph.reposition,window);
		}
	},

	/*
		Function __prepareBG is used to prepare the background
		initialization of the graph.

	Arguments:
		None as of now.
	*/

	__prepareBG			: function()
	{
		/* Draw Title */
		var font = "sans";
		var fontsize = 16;
		var y = this.context.fontAscent(font,fontsize);
		this.context.strokeStyle = "rgba(0,0,0,1)";
		this.context.drawTextCenter( font, fontsize, this.canvas.width/2, y,
		this.settings.graphTitle);

		/* Draw BG */
		this.context.fillStyle = this.settings.bgColor;
		this.context.fillRect(this.settings._gX,this.settings._gY,this.settings.width,this.settings.height);

		/* Draw Border - Simulate rectangle as line */
		/* y - axis */
		this.context.fillStyle = this.settings.borderColor;
		this.context.fillRect(this.settings._gX,
								this.settings._gY,
								this.settings.borderWidth,
								this.settings.height);
		/* x - axis */
		this.context.fillStyle = this.settings.borderColor;


		this.context.fillRect(this.settings._gX,
								this.settings._gY+this.settings.height,
								this.settings.width,
								this.settings.borderWidth);


	},

	/* Function drawXScale is used to draw all the x-axis units in the graph

	Arguments:
		 xData - Optionally sent data otherwise sent via settings
		 unit - Optionally sent to set x-axis unit
	*/

	drawXScale			: function()
	{
		var xStep = 0;
		var xData = this.settings.xData;
		var unit = 'x-axis';
		if(arguments.length)
		{
			xData = arguments[0];
			this.settings.xData = xData;
			if(arguments[1]!=undefined)
				unit = arguments[1];
		}


		if(xData.length)
		{
			xStep = Math.ceil(this.settings.width/xData.length);
		}

		this.settings.xStep = xStep;

		for(var i=0; i<xData.length; i++)
		{
			this.context.fillStyle = "rgba(100,100,100,0.1)";
			this.context.fillRect(this.settings._gX+(xStep*i),
									this.settings._gY,
									this.settings.borderWidth,
									this.settings._gY+this.settings.height-(this.settings.scaleHeight));

			this.context.save();
			this.context.strokeStyle = "rgba(0,0,0,1)";

			//var txtLen = this.context.measureText("sans",8,""+xData[i])/2;

			this.context.translate(this.settings._gX+(xStep*i),
									this.settings._gY+this.settings.height+this.settings.legendGap);
			//this.context.rotate(330*Math.PI/180);

			this.context.drawTextCenter("sans",7,0,0,
									xData[i]+"");

			this.context.restore();
		}
		//draw unit
		this.context.save();
		this.context.strokeStyle = "rgba(0,0,0,1)";

		this.context.translate(this.settings._gX+(this.settings.width/2),
								this.settings._gY+this.settings.height+(this.settings.legendGap)+9+3);
		this.context.drawTextCenter("sans",9,0,0,unit);
		this.context.restore();
	},

	/* Function drawYScale is used to draw all the y-axis units in the graph

	Arguments:
		 yData - Optionally sent data otherwise sent via settings
		 unit - Optionally sent to set y-axis unit
	*/

	drawYScale			: function()
	{
		var yStep = 0;
		var yData = this.settings.yData;
		var unit = 'x-axis';
		if(arguments.length)
		{
			yData = arguments[0];
			this.settings.yData = yData;
			if(arguments[1]!=undefined)
				unit = arguments[1];
		}


		if(yData.length)
		{
			yStep = Math.ceil(this.settings.height/yData.length);
		}

		this.settings.yStep = yStep;

		var maxLen = 0;

		for(var i=0;i<yData.length;i++)
		{
			this.context.strokeStyle = "rgba(0,0,0,1)";
			this.context.drawTextRight("sans",7,this.settings._gX-3,
									this.settings._gY+this.settings.height-(yStep*i),
									yData[i]+"");

			var tmpLen = this.context.measureText("sans",7,yData[i]+"");
			if(maxLen<tmpLen) maxLen = tmpLen;

			this.context.fillStyle = "rgba(200,200,200,.5)";
			this.context.fillRect(this.settings._gX-(this.settings.scaleHeight/2),
									this.settings._gY+this.settings.height-(yStep*i),
									this.settings.width+(this.settings.scaleHeight/2),
									this.settings.borderWidth);
		}

		//draw unit
		this.context.save();
		this.context.strokeStyle = "rgba(0,0,0,.5)";

		this.context.translate(this.settings._gX+9+2,
								this.settings._gY+(this.settings.height/2));
		this.context.rotate(270*Math.PI/180);
		this.context.drawTextCenter("sans",9,0,0,unit);
		this.context.restore();
	},

	drawExtLineGraph		: function(gData)
	{
		this.clearDataArea();
		this.dataContext.lineWidth = 1;
		this.dataContext.strokeStyle = "blue"; //"rgba(159, 202, 211,.9)";

        this.dataContext.beginPath();
			
		for(var i=gData.length-1;i>=0;i--)
		{

			var ypos = null;
			var yary = this.findFallY(gData[i][1]);
			if(yary[0]==null && yary[1]==null)
			{
				//alert("Out of bound Y");
				return;
			}else if(yary[0]==null)
			{
				//ypos =yary[1]; wrong
				//lie on top line
				ypos = (this.settings.yData.indexOf(yary[1])*this.settings.yStep);
			}else if(yary[1]==null)
			{
				//ypos =yary[0]; wrong
				//ypos = 0; //lie on the bottom line
				ypos = (this.settings.yData.indexOf(yary[0])*this.settings.yStep);  // should lie on the top line
			}else
			{
				var percent = (gData[i][1]-yary[0])/(yary[1]-yary[0]);
				ypos = (this.settings.yData.indexOf(yary[0])*this.settings.yStep)+
							(this.settings.yStep*percent);
			}

			if((gData[i][0]/MINS)>this.settings.xData.max())
			{
			//	top.focus();
			//	alert(gData[i][0]);
				break; // stop till given mins drawing
			}

			var xpos = null;
			var xary = this.findFallX(gData[i][0]/60);
			if(xary[0]==null && xary[1]==null)
			{
				//alert("Out of bound X");
				return;
			}else if(xary[0]==null)
			{
				// xpos=xary[1]; wrong
				xpos = (this.settings.xData.indexOf(xary[1])*this.settings.xStep)
			}else if(xary[1]==null)
			{
				// xpos = xary[0]; wrong
				xpos = (this.settings.xData.indexOf(xary[0])*this.settings.xStep)
			}else
			{
				var percent = ((gData[i][0]/MINS)-xary[0])/(xary[1]-xary[0]);
				//var percent = ((gData[i][0])-xary[0])/(xary[1]-xary[0]);
				xpos = (this.settings.xData.indexOf(xary[0])*this.settings.xStep)-
						(this.settings.xStep*percent);
			}



			xpos = this.settings._gX+xpos;
			ypos = (this.settings._gY+this.settings.height)-ypos;
			if(i==gData.length-1)
			{
				this.dataContext.moveTo(xpos,ypos);
			}
			//alert(xpos+","+ypos);
			this.dataContext.lineTo(xpos,ypos);
		}
		this.dataContext.stroke();
	},

	findFallY				: function(y)
	{
		yData = this.settings.yData;

		if(y<yData[0])
		{
			return [null,yData[0]];
		}

		for(var i=0; i<yData.length-1; i++)
		{
			if(y>=yData[i] && y<=yData[i+1])
			{
				return [yData[i],yData[i+1]];
			}
		}

		if(y>yData[yData.length-2])
		{
			return [yData[yData.length-1],null];
		}

		return [null,null];
	},

	findFallX				: function(x)
	{
		xData = this.settings.xData;

		if(x>xData[0])
		{
			return [null,xData[0]];
		}

		for(var i=0; i<xData.length-1; i++)
		{
			if(x<=xData[i] && x>=xData[i+1])
			{
				return [xData[i+1],xData[i]];
			}
		}

		if(x<xData[xData.length-2])
		{
			return [xData[xData.length-1],null];
		}

		return [null,null];
	},

	/* Function drawBasicLineGraph is used to draw line graph in the plot with given data
		The values can be string or decimal. Only the meet points of X,Y can be plotted with
		this function. And thats why its basic.

	Arguments:
		gData - Optionally sent data otherwise sent via settings

	*/

	drawBasicLineGraph		: function()
	{
		var gStep = 0;
		var gData = this.settings.gData;
		if(arguments.length)
		{
			gData = arguments[0];
			this.settings.gData = gData;
		}

		//var gdx = new Array(); //this array is used to traverse back the path to complete the line

		_debug("<h4>GDX POSITIONS</h4>");

		if(gData.length)
		{
			this.context.strokeStyle = "blue";

			this.context.beginPath();
			this.context.moveTo(this.settings._gX,this.settings._gY+this.settings.height);

			for(var i=0; i<gData.length; i++)
			{

				_debug("gData :: "+gData[i][0]+","+gData[i][1]);
				_gdx = [this.settings._gX + (_GAP(this.settings.xData, gData[i][0]) * this.settings.xStep),
						(this.settings._gY + this.settings.height)-(_GAP(this.settings.yData, gData[i][1]) * this.settings.yStep)];

				_debug(_gdx[0]+","+_gdx[1]);

				this.context.lineTo(_gdx[0],_gdx[1]);
				//gdx.push(_gdx);
			}

			/*
			while(_gdx=gdx.pop())
			{
				this.context.lineTo(_gdx[0],_gdx[1]-1);
			}
			*/
			//this.context.fill();
			this.context.stroke();
		}
	},

	/* Function drawLineGraph is used to draw line graph in the plot with given data
		The values must be of decimal to calculate the position.

	Arguments:
		gData - Optionally sent data otherwise sent via settings

	*/

	drawLineGraph		: function()
	{
		var gStep = 0;
		var gData = this.settings.gData;
		if(arguments.length)
		{
			gData = arguments[0];
			this.settings.gData = gData;
		}

		var gdx = new Array(); //this array is used to traverse back the path to complete the line

		_debug("<h4>GDX POSITIONS</h4>");



		if(gData.length)
		{
			this.dataContext.strokeStyle = "rgba(0,200,200,1)";

			this.dataContext.beginPath();
			this.dataContext.lineWidth = 2.0;
			this.dataContext.moveTo(this.settings._gX + ( (gData[0][0] * this.settings.xStep)/this.settings.xScale),
						(this.settings._gY+this.settings.height)-((gData[0][1] * this.settings.yStep)/this.settings.yScale));

			for(var i=0; i<gData.length; i++)
			{

				_debug("gData :: "+gData[i][0]+","+gData[i][1]);
				_gdx = [this.settings._gX + ( (gData[i][0] * this.settings.xStep)/this.settings.xScale),
						(this.settings._gY + this.settings.height)-((gData[i][1] * this.settings.yStep)/this.settings.yScale)];

				_debug(_gdx[0]+","+_gdx[1]);

				this.dataContext.lineTo(_gdx[0],_gdx[1]);
			}

			this.dataContext.stroke();
		}
	},

	/* Function drawPositionedLineGraph is used to draw line graph in the plot with given data
		The values must be of decimal to calculate the position.

	Arguments:
		gData - Optionally sent data otherwise sent via settings
		lineColor - Optionally sent color value otherwise default black

	*/

	drawPositionedLineGraph		: function()
	{
		var gStep = 0;
		var gColor = "rgb(0,0,0)";
		var gData;

		if(arguments.length)
		{
			gData = arguments[0];
			this.settings.gData = gData;

			gColor = arguments[1]==undefined?"rgb(0,0,0)":arguments[1];
		}else
		{
			gData = this.settings.gData;
		}

		var gdx = new Array(); //this array is used to traverse back the path to complete the line

		_debug("<h4>GDX POSITIONS</h4>");


		if(gData.length)
		{
			this.dataContext.strokeStyle = gColor;


			this.dataContext.beginPath();
			this.dataContext.lineWidth = 2.0;

			_gdx = [this.settings._gX + ( ((gData[0][0]-this.settings.xData[0]) * this.settings.xStep)/this.settings.xScale),
						(this.settings._gY + this.settings.height)-(((gData[0][1]-this.settings.yData[0]) * this.settings.yStep)/this.settings.yScale)];

			_debug("moved to:"+((_gdx[0]<this.settings._gX?this.settings._gX:_gdx[0]) +","+
										(_gdx[1]>(this.settings._gY + this.settings.height)?(this.settings._gY + this.settings.height):_gdx[1])));

			this.dataContext.moveTo(_gdx[0]<this.settings._gX?this.settings._gX:_gdx[0],
										_gdx[1]>(this.settings._gY + this.settings.height)?(this.settings._gY + this.settings.height):_gdx[1]);


			//this.dataContext.moveTo(this.settings._gX + ( (gData[0][0] * this.settings.xStep)/this.settings.xScale),
			//			(this.settings._gY+this.settings.height)-((gData[0][1] * this.settings.yStep)/this.settings.yScale));

			for(var i=0; i<gData.length; i++)
			{

				_gdx = [this.settings._gX + ( ((gData[i][0]-this.settings.xData[0]) * this.settings.xStep)/this.settings.xScale),
						(this.settings._gY + this.settings.height)-(((gData[i][1]-this.settings.yData[0]) * this.settings.yStep)/this.settings.yScale)];

				_debug(_gdx[0]+","+_gdx[1]);
				_debug("_gX:"+this.settings._gX+", _gY:"+(this.settings._gY + this.settings.height));

				var lineX, lineY;

				if(_gdx[0]<this.settings._gX)
				{
					lineX = this.settings._gX;
				}else if( _gdx[0]>this.settings._gX+this.settings.width )
				{
					lineX = this.settings._gX+this.settings.width;
				}else
				{
					lineX = _gdx[0];
				}

				if(_gdx[1]<this.settings._gY)
				{
					lineY = this.settings._gY;
				}else if(_gdx[1]>this.settings._gY+this.settings.height)
				{
					lineY = this.settings._gY+this.settings.height;
				}else
				{
					lineY = _gdx[1];
				}
				alert(lineX+' , '+lineY);
				this.dataContext.lineTo(lineX,lineY);
			}

			this.dataContext.stroke();
		}
	},


	/*

		Function drawBarChart is used to draw a horizontal bar chart

	*/

	drawBarChart			: function(gData, max_percentage)
	{
		var bh = this.settings.barHeight;
		var bw=0;
		var cw = this.canvas.offsetWidth;
		var ch = this.canvas.offsetHeight;

		var offsetY = this.settings.barOffset;
		var stepX = this.settings._gX;
		var stepY = this.settings.height- 2*offsetY;

		//this.dataContext.moveTo(stepX, stepY);

		max_width = this.settings.width-22;

		for(var i=0; i<gData.length; i++)
		{
			var percent = (gData[i].count/max_percentage);
			percent = (isNaN(percent)) ? 0 : percent;
			this.dataContext.fillStyle = gData[i].color;

			this.dataContext.fillRect(stepX+this.settings.borderWidth
										,stepY,Math.floor(max_width*percent),bh);

			this.dataContext.strokeStyle = "rgba(0,0,0,.2)";
			this.dataContext.strokeRect(stepX+this.settings.borderWidth
										,stepY,Math.floor(max_width*percent),bh);
			this.dataContext.strokeStyle = "rgba(0,0,0,1)";
			this.dataContext.drawText("sans",9,stepX+(max_width*percent)+5,
									(stepY+bh)-3,
									"("+gData[i].count+")");
			this.dataContext.drawTextRight("sans",9,stepX-5,
									(stepY+bh)-3,
									gData[i].type);

			stepY-= this.settings.barHeight+offsetY;
		}
	},


	/*
		Function drawPieChart is used to draw a bisector based pie chart


	*/
	drawPieChart			: function(radius)
	{
		if(radius==undefined) radius = 30;

		var totalPercentage = 0;
		var startangle = 0;
		var circ = 360;

		var gData = this.settings.gData;

		var cw = this.canvas.offsetWidth;
		var ch = this.canvas.offsetHeight;




		this.dataContext.beginPath();
		this.dataContext.strokeStyle='#000';
		this.dataContext.arc(cw/2,ch/2,radius,0,2*Math.PI,false);
		this.dataContext.stroke();

		this.dataContext.moveTo(cw/2,ch/2);
		this.dataContext.lineTo((cw/2)+radius,(ch/2));

		var legends = document.createElement('ul');
		legends.className = 'graphLegends';

		for(var i=0; i<gData.length; i++)
		{
			this.dataContext.beginPath();

			totalPercentage += Number(gData[i].sector_percentage.toFixed(2));

			endangle = (2*Math.PI) * (gData[i].sector_percentage/100);

			this.dataContext.arc(cw/2,ch/2,radius,startangle,startangle+endangle,false);
			this.dataContext.lineTo(cw/2,ch/2);
			this.dataContext.fillStyle = gData[i].sector_color;
			this.dataContext.fill();

			startangle += endangle;
			var legendItem = document.createElement('li');
			if ((gData[i].sector_name.indexOf("Unknown") == 0) && (gData[i].sector_name.length == 7))
				legendItem.innerHTML = getHTMLLegend(gData[i].sector_color,gData[i].sector_name,gData[i].sector_percentage.toFixed(2));
			else
				legendItem.innerHTML = getHTMLLegend(gData[i].sector_color,gData[i].sector_name,gData[i].sector_percentage.toFixed(2),gData[i].sector_link);
			legends.appendChild(legendItem);
		}

		var legendItem = document.createElement('li');
		legendItem.innerHTML = getHTMLLegend("#fff","Free Space",(100-totalPercentage.toFixed(2)));
		legends.appendChild(legendItem);

		this.canvas.parentNode.insertBefore(legends,this.canvas);

	},

	reposition				: function()
	{
		graph.dataCanvas.style.left = elmOffset(graph.canvas,'Left');
		graph.dataCanvas.style.top = elmOffset(graph.canvas,'Top');
	},

	/*
		Function clearDataArea is used to clear the dataCanvas without clearing the background

	Arguments:
		- None as of now
	*/
	clearDataArea			: function()
	{
		this.dataContext.clearRect(0,
									0,
									this.dataCanvas.width,
									this.dataCanvas.height);
	},

	/*
		Function clearGraphBG is used to clear the background canvas without clearing the dataCanvas

	Arguments:
		- None as of now
	*/
	clearGraphBG			: function()
	{
		this.context.clearRect(this.settings._gX,
									this.settings._gY,
									this.settings.width,
									this.settings.height);
	},

	/*
		Function clear is used to clear the graph

	Arguments:
		- None as of now
	*/
	clear					: function()
	{
		this.clearGraphBG();
		this.clearDataArea();
	},

	display					: function(pStyle)
	{
		this.canvas.style.display = pStyle;
		this.dataCanvas.style.display = pStyle;
	},

	loaded					: false

};


/*
	General purpose function to return the position of a value in array.

	Arguments:
		 ary - The array in which value might be located
		 value - The value to be located in the array
*/

function _GAP(ary, value)
{
	//_debug("<h4>GAP DATA</h4>");
	for(var i=0; i<ary.length; i++)
	{
		if(value==ary[i])
		{
			//_debug("ary pos : "+i+" - "+value);
			return i;
		}
	}

	return 0;
}

function getHTMLLegend(color,name,percentage,link)
{
	var htmllegend = '';
	if(link==undefined)
		htmllegend = "<span class='smallColor' style='background:"+color+";cursor:default'>&nbsp;&nbsp;&nbsp;</span> "+name+" ("+(Math.round(percentage*100)/100)+"%)";
	else
		htmllegend = "<a href='"+link+"' class='smallColor' style='background:"+color+";cursor:pointer'>&nbsp;&nbsp;&nbsp;</a> "+name+" ("+(Math.round(percentage*100)/100)+"%)";
	return htmllegend;
}

/* ---------- My Debugger --------------------- */
/*
DEBUGMODE = false;

function _debug(string)
{
	if(DEBUGMODE)
	{
		document.getElementById('debug').innerHTML += "<p>"+string+"</p>";
	}
}
*/


/* ---------- Canvas Text Functionalities ------- */

var CanvasTextFunctions = { };

CanvasTextFunctions.letters = {
    ' ': { width: 16, points: [] },
    '!': { width: 10, points: [[5,21],[5,7],[-1,-1],[5,2],[4,1],[5,0],[6,1],[5,2]] },
    '"': { width: 16, points: [[4,21],[4,14],[-1,-1],[12,21],[12,14]] },
    '#': { width: 21, points: [[11,25],[4,-7],[-1,-1],[17,25],[10,-7],[-1,-1],[4,12],[18,12],[-1,-1],[3,6],[17,6]] },
    '$': { width: 20, points: [[8,25],[8,-4],[-1,-1],[12,25],[12,-4],[-1,-1],[17,18],[15,20],[12,21],[8,21],[5,20],[3,18],[3,16],[4,14],[5,13],[7,12],[13,10],[15,9],[16,8],[17,6],[17,3],[15,1],[12,0],[8,0],[5,1],[3,3]] },
    '%': { width: 24, points: [[21,21],[3,0],[-1,-1],[8,21],[10,19],[10,17],[9,15],[7,14],[5,14],[3,16],[3,18],[4,20],[6,21],[8,21],[10,20],[13,19],[16,19],[19,20],[21,21],[-1,-1],[17,7],[15,6],[14,4],[14,2],[16,0],[18,0],[20,1],[21,3],[21,5],[19,7],[17,7]] },
    '&': { width: 26, points: [[23,12],[23,13],[22,14],[21,14],[20,13],[19,11],[17,6],[15,3],[13,1],[11,0],[7,0],[5,1],[4,2],[3,4],[3,6],[4,8],[5,9],[12,13],[13,14],[14,16],[14,18],[13,20],[11,21],[9,20],[8,18],[8,16],[9,13],[11,10],[16,3],[18,1],[20,0],[22,0],[23,1],[23,2]] },
    '\'': { width: 10, points: [[5,19],[4,20],[5,21],[6,20],[6,18],[5,16],[4,15]] },
    '(': { width: 14, points: [[11,25],[9,23],[7,20],[5,16],[4,11],[4,7],[5,2],[7,-2],[9,-5],[11,-7]] },
    ')': { width: 14, points: [[3,25],[5,23],[7,20],[9,16],[10,11],[10,7],[9,2],[7,-2],[5,-5],[3,-7]] },
    '*': { width: 16, points: [[8,21],[8,9],[-1,-1],[3,18],[13,12],[-1,-1],[13,18],[3,12]] },
    '+': { width: 26, points: [[13,18],[13,0],[-1,-1],[4,9],[22,9]] },
    ',': { width: 10, points: [[6,1],[5,0],[4,1],[5,2],[6,1],[6,-1],[5,-3],[4,-4]] },
    '-': { width: 26, points: [[4,9],[22,9]] },
    '.': { width: 10, points: [[5,2],[4,1],[5,0],[6,1],[5,2]] },
    '/': { width: 22, points: [[20,25],[2,-7]] },
    '0': { width: 20, points: [[9,21],[6,20],[4,17],[3,12],[3,9],[4,4],[6,1],[9,0],[11,0],[14,1],[16,4],[17,9],[17,12],[16,17],[14,20],[11,21],[9,21]] },
    '1': { width: 20, points: [[6,17],[8,18],[11,21],[11,0]] },
    '2': { width: 20, points: [[4,16],[4,17],[5,19],[6,20],[8,21],[12,21],[14,20],[15,19],[16,17],[16,15],[15,13],[13,10],[3,0],[17,0]] },
    '3': { width: 20, points: [[5,21],[16,21],[10,13],[13,13],[15,12],[16,11],[17,8],[17,6],[16,3],[14,1],[11,0],[8,0],[5,1],[4,2],[3,4]] },
    '4': { width: 20, points: [[13,21],[3,7],[18,7],[-1,-1],[13,21],[13,0]] },
    '5': { width: 20, points: [[15,21],[5,21],[4,12],[5,13],[8,14],[11,14],[14,13],[16,11],[17,8],[17,6],[16,3],[14,1],[11,0],[8,0],[5,1],[4,2],[3,4]] },
    '6': { width: 20, points: [[16,18],[15,20],[12,21],[10,21],[7,20],[5,17],[4,12],[4,7],[5,3],[7,1],[10,0],[11,0],[14,1],[16,3],[17,6],[17,7],[16,10],[14,12],[11,13],[10,13],[7,12],[5,10],[4,7]] },
    '7': { width: 20, points: [[17,21],[7,0],[-1,-1],[3,21],[17,21]] },
    '8': { width: 20, points: [[8,21],[5,20],[4,18],[4,16],[5,14],[7,13],[11,12],[14,11],[16,9],[17,7],[17,4],[16,2],[15,1],[12,0],[8,0],[5,1],[4,2],[3,4],[3,7],[4,9],[6,11],[9,12],[13,13],[15,14],[16,16],[16,18],[15,20],[12,21],[8,21]] },
    '9': { width: 20, points: [[16,14],[15,11],[13,9],[10,8],[9,8],[6,9],[4,11],[3,14],[3,15],[4,18],[6,20],[9,21],[10,21],[13,20],[15,18],[16,14],[16,9],[15,4],[13,1],[10,0],[8,0],[5,1],[4,3]] },
    ':': { width: 10, points: [[5,14],[4,13],[5,12],[6,13],[5,14],[-1,-1],[5,2],[4,1],[5,0],[6,1],[5,2]] },
    ',': { width: 10, points: [[5,14],[4,13],[5,12],[6,13],[5,14],[-1,-1],[6,1],[5,0],[4,1],[5,2],[6,1],[6,-1],[5,-3],[4,-4]] },
    '<': { width: 24, points: [[20,18],[4,9],[20,0]] },
    '=': { width: 26, points: [[4,12],[22,12],[-1,-1],[4,6],[22,6]] },
    '>': { width: 24, points: [[4,18],[20,9],[4,0]] },
    '?': { width: 18, points: [[3,16],[3,17],[4,19],[5,20],[7,21],[11,21],[13,20],[14,19],[15,17],[15,15],[14,13],[13,12],[9,10],[9,7],[-1,-1],[9,2],[8,1],[9,0],[10,1],[9,2]] },
    '@': { width: 27, points: [[18,13],[17,15],[15,16],[12,16],[10,15],[9,14],[8,11],[8,8],[9,6],[11,5],[14,5],[16,6],[17,8],[-1,-1],[12,16],[10,14],[9,11],[9,8],[10,6],[11,5],[-1,-1],[18,16],[17,8],[17,6],[19,5],[21,5],[23,7],[24,10],[24,12],[23,15],[22,17],[20,19],[18,20],[15,21],[12,21],[9,20],[7,19],[5,17],[4,15],[3,12],[3,9],[4,6],[5,4],[7,2],[9,1],[12,0],[15,0],[18,1],[20,2],[21,3],[-1,-1],[19,16],[18,8],[18,6],[19,5]] },
    'A': { width: 18, points: [[9,21],[1,0],[-1,-1],[9,21],[17,0],[-1,-1],[4,7],[14,7]] },
    'B': { width: 21, points: [[4,21],[4,0],[-1,-1],[4,21],[13,21],[16,20],[17,19],[18,17],[18,15],[17,13],[16,12],[13,11],[-1,-1],[4,11],[13,11],[16,10],[17,9],[18,7],[18,4],[17,2],[16,1],[13,0],[4,0]] },
    'C': { width: 21, points: [[18,16],[17,18],[15,20],[13,21],[9,21],[7,20],[5,18],[4,16],[3,13],[3,8],[4,5],[5,3],[7,1],[9,0],[13,0],[15,1],[17,3],[18,5]] },
    'D': { width: 21, points: [[4,21],[4,0],[-1,-1],[4,21],[11,21],[14,20],[16,18],[17,16],[18,13],[18,8],[17,5],[16,3],[14,1],[11,0],[4,0]] },
    'E': { width: 19, points: [[4,21],[4,0],[-1,-1],[4,21],[17,21],[-1,-1],[4,11],[12,11],[-1,-1],[4,0],[17,0]] },
    'F': { width: 18, points: [[4,21],[4,0],[-1,-1],[4,21],[17,21],[-1,-1],[4,11],[12,11]] },
    'G': { width: 21, points: [[18,16],[17,18],[15,20],[13,21],[9,21],[7,20],[5,18],[4,16],[3,13],[3,8],[4,5],[5,3],[7,1],[9,0],[13,0],[15,1],[17,3],[18,5],[18,8],[-1,-1],[13,8],[18,8]] },
    'H': { width: 22, points: [[4,21],[4,0],[-1,-1],[18,21],[18,0],[-1,-1],[4,11],[18,11]] },
    'I': { width: 8, points: [[4,21],[4,0]] },
    'J': { width: 16, points: [[12,21],[12,5],[11,2],[10,1],[8,0],[6,0],[4,1],[3,2],[2,5],[2,7]] },
    'K': { width: 21, points: [[4,21],[4,0],[-1,-1],[18,21],[4,7],[-1,-1],[9,12],[18,0]] },
    'L': { width: 17, points: [[4,21],[4,0],[-1,-1],[4,0],[16,0]] },
    'M': { width: 24, points: [[4,21],[4,0],[-1,-1],[4,21],[12,0],[-1,-1],[20,21],[12,0],[-1,-1],[20,21],[20,0]] },
    'N': { width: 22, points: [[4,21],[4,0],[-1,-1],[4,21],[18,0],[-1,-1],[18,21],[18,0]] },
    'O': { width: 22, points: [[9,21],[7,20],[5,18],[4,16],[3,13],[3,8],[4,5],[5,3],[7,1],[9,0],[13,0],[15,1],[17,3],[18,5],[19,8],[19,13],[18,16],[17,18],[15,20],[13,21],[9,21]] },
    'P': { width: 21, points: [[4,21],[4,0],[-1,-1],[4,21],[13,21],[16,20],[17,19],[18,17],[18,14],[17,12],[16,11],[13,10],[4,10]] },
    'Q': { width: 22, points: [[9,21],[7,20],[5,18],[4,16],[3,13],[3,8],[4,5],[5,3],[7,1],[9,0],[13,0],[15,1],[17,3],[18,5],[19,8],[19,13],[18,16],[17,18],[15,20],[13,21],[9,21],[-1,-1],[12,4],[18,-2]] },
    'R': { width: 21, points: [[4,21],[4,0],[-1,-1],[4,21],[13,21],[16,20],[17,19],[18,17],[18,15],[17,13],[16,12],[13,11],[4,11],[-1,-1],[11,11],[18,0]] },
    'S': { width: 20, points: [[17,18],[15,20],[12,21],[8,21],[5,20],[3,18],[3,16],[4,14],[5,13],[7,12],[13,10],[15,9],[16,8],[17,6],[17,3],[15,1],[12,0],[8,0],[5,1],[3,3]] },
    'T': { width: 16, points: [[8,21],[8,0],[-1,-1],[1,21],[15,21]] },
    'U': { width: 22, points: [[4,21],[4,6],[5,3],[7,1],[10,0],[12,0],[15,1],[17,3],[18,6],[18,21]] },
    'V': { width: 18, points: [[1,21],[9,0],[-1,-1],[17,21],[9,0]] },
    'W': { width: 24, points: [[2,21],[7,0],[-1,-1],[12,21],[7,0],[-1,-1],[12,21],[17,0],[-1,-1],[22,21],[17,0]] },
    'X': { width: 20, points: [[3,21],[17,0],[-1,-1],[17,21],[3,0]] },
    'Y': { width: 18, points: [[1,21],[9,11],[9,0],[-1,-1],[17,21],[9,11]] },
    'Z': { width: 20, points: [[17,21],[3,0],[-1,-1],[3,21],[17,21],[-1,-1],[3,0],[17,0]] },
    '[': { width: 14, points: [[4,25],[4,-7],[-1,-1],[5,25],[5,-7],[-1,-1],[4,25],[11,25],[-1,-1],[4,-7],[11,-7]] },
    '\\': { width: 14, points: [[0,21],[14,-3]] },
    ']': { width: 14, points: [[9,25],[9,-7],[-1,-1],[10,25],[10,-7],[-1,-1],[3,25],[10,25],[-1,-1],[3,-7],[10,-7]] },
    '^': { width: 16, points: [[6,15],[8,18],[10,15],[-1,-1],[3,12],[8,17],[13,12],[-1,-1],[8,17],[8,0]] },
    '_': { width: 16, points: [[0,-2],[16,-2]] },
    '`': { width: 10, points: [[6,21],[5,20],[4,18],[4,16],[5,15],[6,16],[5,17]] },
    'a': { width: 19, points: [[15,14],[15,0],[-1,-1],[15,11],[13,13],[11,14],[8,14],[6,13],[4,11],[3,8],[3,6],[4,3],[6,1],[8,0],[11,0],[13,1],[15,3]] },
    'b': { width: 19, points: [[4,21],[4,0],[-1,-1],[4,11],[6,13],[8,14],[11,14],[13,13],[15,11],[16,8],[16,6],[15,3],[13,1],[11,0],[8,0],[6,1],[4,3]] },
    'c': { width: 18, points: [[15,11],[13,13],[11,14],[8,14],[6,13],[4,11],[3,8],[3,6],[4,3],[6,1],[8,0],[11,0],[13,1],[15,3]] },
    'd': { width: 19, points: [[15,21],[15,0],[-1,-1],[15,11],[13,13],[11,14],[8,14],[6,13],[4,11],[3,8],[3,6],[4,3],[6,1],[8,0],[11,0],[13,1],[15,3]] },
    'e': { width: 18, points: [[3,8],[15,8],[15,10],[14,12],[13,13],[11,14],[8,14],[6,13],[4,11],[3,8],[3,6],[4,3],[6,1],[8,0],[11,0],[13,1],[15,3]] },
    'f': { width: 12, points: [[10,21],[8,21],[6,20],[5,17],[5,0],[-1,-1],[2,14],[9,14]] },
    'g': { width: 19, points: [[15,14],[15,-2],[14,-5],[13,-6],[11,-7],[8,-7],[6,-6],[-1,-1],[15,11],[13,13],[11,14],[8,14],[6,13],[4,11],[3,8],[3,6],[4,3],[6,1],[8,0],[11,0],[13,1],[15,3]] },
    'h': { width: 19, points: [[4,21],[4,0],[-1,-1],[4,10],[7,13],[9,14],[12,14],[14,13],[15,10],[15,0]] },
    'i': { width: 8, points: [[3,21],[4,20],[5,21],[4,22],[3,21],[-1,-1],[4,14],[4,0]] },
    'j': { width: 10, points: [[5,21],[6,20],[7,21],[6,22],[5,21],[-1,-1],[6,14],[6,-3],[5,-6],[3,-7],[1,-7]] },
    'k': { width: 17, points: [[4,21],[4,0],[-1,-1],[14,14],[4,4],[-1,-1],[8,8],[15,0]] },
    'l': { width: 8, points: [[4,21],[4,0]] },
    'm': { width: 30, points: [[4,14],[4,0],[-1,-1],[4,10],[7,13],[9,14],[12,14],[14,13],[15,10],[15,0],[-1,-1],[15,10],[18,13],[20,14],[23,14],[25,13],[26,10],[26,0]] },
    'n': { width: 19, points: [[4,14],[4,0],[-1,-1],[4,10],[7,13],[9,14],[12,14],[14,13],[15,10],[15,0]] },
    'o': { width: 19, points: [[8,14],[6,13],[4,11],[3,8],[3,6],[4,3],[6,1],[8,0],[11,0],[13,1],[15,3],[16,6],[16,8],[15,11],[13,13],[11,14],[8,14]] },
    'p': { width: 19, points: [[4,14],[4,-7],[-1,-1],[4,11],[6,13],[8,14],[11,14],[13,13],[15,11],[16,8],[16,6],[15,3],[13,1],[11,0],[8,0],[6,1],[4,3]] },
    'q': { width: 19, points: [[15,14],[15,-7],[-1,-1],[15,11],[13,13],[11,14],[8,14],[6,13],[4,11],[3,8],[3,6],[4,3],[6,1],[8,0],[11,0],[13,1],[15,3]] },
    'r': { width: 13, points: [[4,14],[4,0],[-1,-1],[4,8],[5,11],[7,13],[9,14],[12,14]] },
    's': { width: 17, points: [[14,11],[13,13],[10,14],[7,14],[4,13],[3,11],[4,9],[6,8],[11,7],[13,6],[14,4],[14,3],[13,1],[10,0],[7,0],[4,1],[3,3]] },
    't': { width: 12, points: [[5,21],[5,4],[6,1],[8,0],[10,0],[-1,-1],[2,14],[9,14]] },
    'u': { width: 19, points: [[4,14],[4,4],[5,1],[7,0],[10,0],[12,1],[15,4],[-1,-1],[15,14],[15,0]] },
    'v': { width: 16, points: [[2,14],[8,0],[-1,-1],[14,14],[8,0]] },
    'w': { width: 22, points: [[3,14],[7,0],[-1,-1],[11,14],[7,0],[-1,-1],[11,14],[15,0],[-1,-1],[19,14],[15,0]] },
    'x': { width: 17, points: [[3,14],[14,0],[-1,-1],[14,14],[3,0]] },
    'y': { width: 16, points: [[2,14],[8,0],[-1,-1],[14,14],[8,0],[6,-4],[4,-6],[2,-7],[1,-7]] },
    'z': { width: 17, points: [[14,14],[3,0],[-1,-1],[3,14],[14,14],[-1,-1],[3,0],[14,0]] },
    '{': { width: 14, points: [[9,25],[7,24],[6,23],[5,21],[5,19],[6,17],[7,16],[8,14],[8,12],[6,10],[-1,-1],[7,24],[6,22],[6,20],[7,18],[8,17],[9,15],[9,13],[8,11],[4,9],[8,7],[9,5],[9,3],[8,1],[7,0],[6,-2],[6,-4],[7,-6],[-1,-1],[6,8],[8,6],[8,4],[7,2],[6,1],[5,-1],[5,-3],[6,-5],[7,-6],[9,-7]] },
    '|': { width: 8, points: [[4,25],[4,-7]] },
    '}': { width: 14, points: [[5,25],[7,24],[8,23],[9,21],[9,19],[8,17],[7,16],[6,14],[6,12],[8,10],[-1,-1],[7,24],[8,22],[8,20],[7,18],[6,17],[5,15],[5,13],[6,11],[10,9],[6,7],[5,5],[5,3],[6,1],[7,0],[8,-2],[8,-4],[7,-6],[-1,-1],[8,8],[6,6],[6,4],[7,2],[8,1],[9,-1],[9,-3],[8,-5],[7,-6],[5,-7]] },
    '~': { width: 24, points: [[3,6],[3,8],[4,11],[6,12],[8,12],[10,11],[14,8],[16,7],[18,7],[20,8],[21,10],[-1,-1],[3,8],[4,10],[6,11],[8,11],[10,10],[14,7],[16,6],[18,6],[20,7],[21,10],[21,12]] }
};

CanvasTextFunctions.letter = function (ch)
{
    return CanvasTextFunctions.letters[ch];
}

CanvasTextFunctions.ascent = function( font, size)
{
    return size;
}

CanvasTextFunctions.descent = function( font, size)
{
    return 7.0*size/25.0;
}

CanvasTextFunctions.measure = function( font, size, str)
{
    var total = 0;
    var len = str.length;

    for ( i = 0; i < len; i++) {
	var c = CanvasTextFunctions.letter( str.charAt(i));
	if ( c) total += c.width * size / 25.0;
    }
    return total;
}

CanvasTextFunctions.draw = function(ctx,font,size,x,y,str)
{
    var total = 0;
    var len = str.length;
    var mag = size / 25.0;

    ctx.save();
    ctx.lineCap = "round";
    ctx.lineWidth = 2.0 * mag;

    for ( i = 0; i < len; i++) {
	var c = CanvasTextFunctions.letter( str.charAt(i));
	if ( !c) continue;

	ctx.beginPath();

	var penUp = 1;
	var needStroke = 0;
	for ( j = 0; j < c.points.length; j++) {
	    var a = c.points[j];
	    if ( a[0] == -1 && a[1] == -1) {
		penUp = 1;
		continue;
	    }
	    if ( penUp) {
		ctx.moveTo( x + a[0]*mag, y - a[1]*mag);
		penUp = false;
	    } else {
		ctx.lineTo( x + a[0]*mag, y - a[1]*mag);
	    }
	}
	ctx.stroke();
	x += c.width*mag;
    }
    ctx.restore();
    return total;
}

CanvasTextFunctions.enable = function( ctx)
{
    ctx.drawText = function(font,size,x,y,text) { return CanvasTextFunctions.draw( ctx, font,size,x,y,text); };
    ctx.measureText = function(font,size,text) { return CanvasTextFunctions.measure( font,size,text); };
    ctx.fontAscent = function(font,size) { return CanvasTextFunctions.ascent(font,size); }
    ctx.fontDescent = function(font,size) { return CanvasTextFunctions.descent(font,size); }

    ctx.drawTextRight = function(font,size,x,y,text) {
	var w = CanvasTextFunctions.measure(font,size,text);
	return CanvasTextFunctions.draw( ctx, font,size,x-w,y,text);
    };
    ctx.drawTextCenter = function(font,size,x,y,text) {
	var w = CanvasTextFunctions.measure(font,size,text);
	return CanvasTextFunctions.draw( ctx, font,size,x-w/2,y,text);
    };
}


graph.loaded = true;
