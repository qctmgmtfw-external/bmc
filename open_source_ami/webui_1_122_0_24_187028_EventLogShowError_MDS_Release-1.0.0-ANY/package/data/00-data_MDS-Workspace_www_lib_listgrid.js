--- .pristine/webui-1.122.0-src/data/MDS-Workspace/www/lib/listgrid.js Fri Aug 29 18:02:13 2014
+++ source/webui-1.122.0-src/data/MDS-Workspace/www/lib/listgrid.js Fri Oct  3 14:15:15 2014
@@ -63,9 +63,7 @@
 var safari = (navigator.userAgent.toLowerCase().indexOf('safari')!=-1);
 var firefox = (navigator.userAgent.toLowerCase().indexOf('firefox')!=-1);
 var opera = (navigator.userAgent.toLowerCase().indexOf('opera')!=-1);
-var ie = (window.ActiveXObject);
-//IE6-9 won¡¦t recognize this cc on statement, and we want IE10 without compatible mode
-var ie10 = !!(ie && /*@cc_on!@*/false && (document.documentMode === 10)); 
+var ie = isIEBrowser(); //(window.ActiveXObject); This is not working in latest IE browser
 var chrome = (navigator.userAgent.toLowerCase().indexOf('chrome')!=-1);
 
 /*
@@ -123,16 +121,23 @@
 	if (tBody.style.height != "") {
 		if (firefox) {
 			//this.table.style.height = this.defaultHeight;
-		} else if (ie && !ie10) {
-			this.container.header.style.position = 'absolute';
+
+			//solved BMC GUI event log table end separate line is poorly aligned in Firefox web browser. (Firefox 7.0)
+			this.container.style.width = "99%";
+			this.table.style.height = arg.h?arg.h:"";
+			this.table.style.overflowX = 'hidden';
+			this.table.style.overflowY = 'auto';
+		} else if (ie) {
+			//this.container.header.style.position = 'absolute';
+			tBody.style.height = "";
 			//this.container.header.style.top = (this.container.offsetTop-17)+'px';
 			this.container.style.width = "99%";
 			this.table.style.height = arg.h?arg.h:"";
 			this.table.style.overflowX = 'hidden';
 			this.table.style.overflowY = 'auto';
-		} else if(safari || opera || chrome || ie10) {
+		} else if(safari || opera || chrome) {
 			//safari, opera, chrome and other safari based render engines
-			this.container.header.style.display = 'block';
+			this.container.header.style.display = 'inline-table';
 			this.container.header.style.width = '100%';
 			tBody.style.display = 'block';
 			tBody.style.width = '100%';
@@ -435,7 +440,7 @@
 		td.style.paddingLeft = '1em';
 		//safari, opera, chrome and other safari related render engines
 		if (!this.heightLess) {
-			if (safari || opera || chrome || ie10) {
+			if (safari || opera || chrome) {
 				//alert(this.container.header.cells[s].offsetWidth);
 				td.style.width = this.container.header.cells[s].offsetWidth+'px';
 				this.container.header.cells[s].style.width = td.offsetWidth+'px';
@@ -887,6 +892,17 @@
     }
 }
 
+/* 
+ * This method is used to identify the client browser
+ * @return true, if its IE, else false.
+ */
+function isIEBrowser() {
+	var ua = window.navigator.userAgent;
+	var msie = ua.indexOf("MSIE ");
+	var trident = ua.indexOf("Trident/");
+	return ((msie > 0) || (trident > 0)) ? true : false;
+}
+
 var doNothing = function(){return false;}
 
 listgrid.prototype.loaded = true;
