--- .pristine/webui-1.122.0-src/data/MDS-Workspace/www/lib/widget.js Wed Aug 29 13:49:44 2012
+++ source/webui-1.122.0-src/data/MDS-Workspace/www/lib/widget.js Wed Aug 29 13:58:05 2012
@@ -45,7 +45,7 @@
 		return false;
 	}
 
-	if(widgets[id]!=undefined && widgets[id]!=null)
+	if (widgets[id] != undefined && widgets[id] != null)
 		return document.getElementById('widget_'+id);
 
 	this.container = document.createElement('iframe');
@@ -84,14 +84,14 @@
 
 widget.prototype.close = function()
 {
-	document.body.removeChild(this.container);
 	//force unload events;
 	this.container.src = '../page/blank.html';
-
+	document.body.removeChild(this.container);
+	delete this.container;
 
 	for(var j=0; j<widgets.length; j++)
 	{
-		if(widgets[j]!=null && widgets[j]!="")
+		if (widgets[j] != undefined && widgets[j] != null)
 		{
 			if(widgets[j]==this)
 			{
@@ -103,10 +103,6 @@
 
 	try{ pageFrame.widgetListener(j); }catch(e){}
 
-	delete this.container;
-	try{
-		delete this;
-	}catch(e){}
 	reposition_all_widgets();
 }
 
@@ -114,7 +110,7 @@
 {
 	for(var i=0; i<widgets.length; i++)
 	{
-		if(widgets[i]!=null && widgets[i]!="")
+		if (widgets[i] != undefined && widgets[i] != null)
 			widgets[i].close();
 	}
 }
@@ -125,11 +121,11 @@
 	for(var j=0; j<widgets.length; j++)
 	{
 		try{
-			if(widgets[j]!=null && widgets[j]!="")
-			{
-				widgets[j].container.style.left = ((count++) * (widgets[j].container.offsetWidth+10))+'px';
-				widgets[j].container.style.top = (document.body.clientHeight - widgets[j].container.offsetHeight)+'px';
-			}
+			if (widgets[j] == undefined || 
+				widgets[j] == null) continue;
+
+			widgets[j].container.style.left = ((count++) * (widgets[j].container.offsetWidth+10))+'px';
+			widgets[j].container.style.top = (document.body.clientHeight - widgets[j].container.offsetHeight)+'px';
 		}catch(e)
 		{
 			delete widgets[j];
