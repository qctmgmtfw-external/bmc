--- .pristine/webui-1.122.0-src/data/MDS-Workspace/www/lib/listgrid.js Thu Sep  8 15:23:14 2011
+++ source/webui-1.122.0-src/data/MDS-Workspace/www/lib/listgrid.js Fri Sep  9 17:51:13 2011
@@ -83,7 +83,6 @@
 	lGrid.traceClear();
 	lGrid.trace('Listgrid Constructor starting');
 	lGrid.__constructor(arg);
-
 	return lGrid;
 }
 
@@ -111,30 +110,26 @@
 	tBody = document.createElement('tbody');
 	this.container.appendChild(tBody);
 
+	this.message = arg.msg ? arg.msg : eLang.getString("common", "STR_DATA") + 
+		eLang.getString("common", "STR_NOT_AVAILABLE");
 	this.heightLess = arg.h?false:true;
 	tBody.style.height = arg.h?arg.h:"";
 	this.defaultHeight = arg.h?arg.h:""; //Added by Manoj.C for Bug 11530
-	//alert ("Height::" + tBody.style.height + "Default Height::" + this.defaultHeight);
 
 	this.container.style.width = "100%";
 	this.table.style.width = arg.w?arg.w:"100%";
-	if(tBody.style.height!="")
-	{
-		if(firefox)
-		{
+	if (tBody.style.height != "") {
+		if (firefox) {
 			//this.table.style.height = this.defaultHeight;
-		}
-		else if(ie)
-		{
+		} else if (ie) {
 			this.container.header.style.position = 'absolute';
 			//this.container.header.style.top = (this.container.offsetTop-17)+'px';
 			this.container.style.width = "99%";
 			this.table.style.height = arg.h?arg.h:"";
 			this.table.style.overflowX = 'hidden';
 			this.table.style.overflowY = 'auto';
-		}
-		else if(safari || opera || chrome)
-		{ //safari, opera, chrome and other safari based render engines
+		} else if(safari || opera || chrome) {
+			//safari, opera, chrome and other safari based render engines
 			this.container.header.style.display = 'block';
 			this.container.header.style.width = '100%';
 			tBody.style.display = 'block';
@@ -144,10 +139,10 @@
 
 	lGrid.trace("Within constructor, this.table.style.height " + this.table + " = " + this.table.style.height);
 	lGrid.trace("Listgrid .. ActiveXObject out "+this.table+" = "+this.table.style.height);
-	for(i in arg)
-	{
-		if(i!='w' && i!='h')
+	for (i in arg) {
+		if(i!='w' && i!='h') {
 			lGrid[i] = arg[i];
+		}
 	}
 }
 
@@ -211,6 +206,11 @@
 			alert(e.description);
 		}
 
+		if (rCln.length == 0) {
+			//If there is no data in the listgrid sorting not required.
+			return;
+		}
+
 		/*
 		 * Before sorting process starts, it will call onbeforesort function, 
 		 * only when it is defined in implemenation part.
@@ -226,6 +226,7 @@
 		//prepare for the sorting combat [-O<]
 		showWait(true,"Sorting");
 
+		lGrid.trace ("rCln length::" + rCln.length);
 		//using rCln now;
 		var _srtAry = [];
 		var q = 0;
@@ -349,15 +350,15 @@
 		lGrid.trace("Header height out" + th + " = " + th.style.height);
 	}
 
-  th.style.textAlign = (arg.textAlign && arg.textAlign!=undefined)?arg.textAlign:'left';
-
-  if(safari || opera || chrome)
-  { //safari, opera, chrome and other safari based render engines
-	this.container.header.style.width = this.container.header.offsetWidth+'px';
-  }
-
-  //fire onload event;
-  this.onload();
+	th.style.textAlign = (arg.textAlign && arg.textAlign!=undefined)?arg.textAlign:'left';
+
+	if (safari || opera || chrome) {
+		//safari, opera, chrome and other safari based render engines
+		this.container.header.style.width = this.container.header.offsetWidth+'px';
+	}
+
+	//fire onload event;
+	this.onload();
 }
 
 //this list acts loaded when headers are added
@@ -583,8 +584,8 @@
 		// if 100 rows browser can do it fast without hanging..
 		if (args.rows.length == 0) {	//listgrid having no value.
 			var strMessage = "<div id='divMessage' class='errorDisplay'"
-				+ "style=font-size:20px>" +eLang.getString("common","STR_DATA")
-				+ eLang.getString("common", "STR_NOT_AVAILABLE") + "</div>";
+				+ "style=color:#000;text-transform:none;font-variant:normal>" +
+				this.message + "</div>";
 			this.addSingleRow(strMessage);
 			showWait(false);
 		} else if (args.rows.length < 100) {
@@ -786,7 +787,7 @@
 			gTraceWnd.document.writeln(stack + "<br>");
 			gTraceWnd.scrollTo(0,10000000);
 		} catch(e) {
-			alert (e.description);
+			//alert (e.description);
 		}
 	}
 }
@@ -802,7 +803,7 @@
 				gTraceWnd.document.body.innerHTML = "";
 			}
 		} catch (e) {
-			alert (e.description);
+			//alert (e.description);
 		}
 	}
 }
