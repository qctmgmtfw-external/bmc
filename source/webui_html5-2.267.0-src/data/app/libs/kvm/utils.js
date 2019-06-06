
if(!ArrayBuffer.prototype.slice) {
	ArrayBuffer.prototype.slice = function(start, end) {
		//var prof = new Date().getTime();
		if(start>this.byteLength) {
			start = end = this.byteLength;
		}
		if(end>this.byteLength) {
			end = this.byteLength;
		}
		var u8 = new Uint8Array(end-start),
			su8 = new Uint8Array(this);
		for(var i=start, j=0; i<end; i++, j++) {
			u8[j] = su8[i];
		}
		//var eprof = new Date().getTime();
		//if(eprof-prof)
		//console.log((eprof-prof)/1000 + ' secs');
		return u8.buffer;

		/*
		var arr = new ArrayBuffer(end-start);
		var ds = new DataView(arr);
		var dv = new DataView(this);
		for(var i=start, j=0; i<end; i++, j++) {
			ds.setUint8(j, dv.getUint8(i));
		}
		//console.log(ds.byteLength);
		return ds.buffer;
		*/
		/*
		var u8 = new Uint8Array(this);
		var nu8 = u8.subarray(start, end);
		console.log(start,' ', end,' ', end-start,' ', nu8.buffer.byteLength, ' ',u8.buffer.byteLength);
		return nu8.buffer;
		*/
	};
}

var getPlatform = function(){
	if(['iPad','iPod','iPhone'].indexOf(navigator.platform)!=-1) {
		return 'iOS';
	}
	if(navigator.userAgent.indexOf('Android')!=-1) {
		return 'Android';
	}
	if(navigator.platform.indexOf('Windows')!=-1) {
		return 'Windows';
	}
	if(navigator.platform.indexOf('Linux')!=-1) {
		return 'Linux';
	}
};

function is_touch_device() {
  return 'createTouch' in document;
}

if(typeof define=="function" && define.amd) {

define(function(){return {is_touch_device: is_touch_device, getPlatform: getPlatform}});

}
