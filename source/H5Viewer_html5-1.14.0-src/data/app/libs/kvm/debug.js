/* debug.js - For logging debug messages
** usage: debug.log(obj); */
var debug = new function(){
	/* For logging real time (forwards to console.log)
	** set this flag to 'true' if log messages needs to be printed.
	** it can be enabled / disabled in runtime by invoking Ctrl+Shift+Y key combinations */
	this.isDebugEnabled = false;
	/* Function which redirects log messages to console.log() */
	this.log = function (obj){
		if (this.isDebugEnabled) console.log(obj);
	}
}