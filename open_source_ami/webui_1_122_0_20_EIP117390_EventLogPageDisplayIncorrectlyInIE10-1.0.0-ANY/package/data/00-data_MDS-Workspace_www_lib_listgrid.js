--- .pristine/webui-1.122.0-src/data/MDS-Workspace/www/lib/listgrid.js	Tue Apr 16 15:27:09 2013
+++ source/webui-1.122.0-src/data/MDS-Workspace/www/lib/listgrid.js	Tue Apr 16 15:49:21 2013
@@ -64,6 +64,8 @@
 var firefox = (navigator.userAgent.toLowerCase().indexOf('firefox')!=-1);
 var opera = (navigator.userAgent.toLowerCase().indexOf('opera')!=-1);
 var ie = (window.ActiveXObject);
+//IE6-9 won¡¦t recognize this cc on statement, and we want IE10 without compatible mode
+var ie10 = !!(ie && /*@cc_on!@*/false && (document.documentMode === 10)); 
 var chrome = (navigator.userAgent.toLowerCase().indexOf('chrome')!=-1);
 
 /*
@@ -121,14 +123,14 @@
 	if (tBody.style.height != "") {
 		if (firefox) {
 			//this.table.style.height = this.defaultHeight;
-		} else if (ie) {
+		} else if (ie && !ie10) {
 			this.container.header.style.position = 'absolute';
 			//this.container.header.style.top = (this.container.offsetTop-17)+'px';
 			this.container.style.width = "99%";
 			this.table.style.height = arg.h?arg.h:"";
 			this.table.style.overflowX = 'hidden';
 			this.table.style.overflowY = 'auto';
-		} else if(safari || opera || chrome) {
+		} else if(safari || opera || chrome || ie10) {
 			//safari, opera, chrome and other safari based render engines
 			this.container.header.style.display = 'block';
 			this.container.header.style.width = '100%';
@@ -433,7 +435,7 @@
 		td.style.paddingLeft = '1em';
 		//safari, opera, chrome and other safari related render engines
 		if (!this.heightLess) {
-			if (safari || opera || chrome) {
+			if (safari || opera || chrome || ie10) {
 				//alert(this.container.header.cells[s].offsetWidth);
 				td.style.width = this.container.header.cells[s].offsetWidth+'px';
 				this.container.header.cells[s].style.width = td.offsetWidth+'px';
