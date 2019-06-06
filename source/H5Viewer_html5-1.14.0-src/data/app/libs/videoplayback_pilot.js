var VideoPlayback = function(canvas) {
    this.num_frames = 0;

    //this.running = false;

    this.canvas = document.getElementById(canvas);
    this.$canvas = $(this.canvas);
    this.context = this.canvas.getContext("2d");

    this.paintBlankScreen();
    this.imageBuffer = this.context.getImageData(0, 0, this.$canvas.width(),this.$canvas.height());
    this.imageBufferData = this.imageBuffer.data;

    //var crle_bloburl = window.URL.createObjectURL(new Blob(['(',compress_worker.toString(),')()'],{type: 'application/javascript'}));
    //this.compressWorker = new Worker(crle_bloburl);
    this.compressWorker = new Worker('./libs/kvm/p3/compressionRLE.js');
    //var fh_bloburl = window.URL.createObjectURL(new Blob(['(',handler_worker.toString(),')()'],{type: 'application/javascript'}));
    //this.handlerWorker = new Worker(fh_bloburl);
    this.handlerWorker = new Worker('./libs/kvm/p3/frameHandler.js');

    var _video = this;

    this.handlerWorker.postMessage({cmd: 'imageBuffer', imageBuffer: this.imageBuffer});

    this.handlerWorker.onmessage = function(e) {
        switch(e.data.cmd) {
            case 'draw':
            _video.imageBuffer = e.data.ibuf;
            _video.repaint();
            break;
            case 'resolution_change':
            _video.onResolutionChange();
            break;
        }
    }

    this.compressWorker.onmessage = function(e) {
        //console.log("decompressing");
        if(e.data.cmd=='data') {
            //_video.handlerWorker.postMessage({buffer: e.data.ob, cmd:'data', header: e.data.header},[e.data.ob]);
            _video.handlerWorker.postMessage({buffer: e.data.ob, cmd:'data', header: e.data.header});
        }
    }
};

var Compression = {
    SOFT_COMPRESSION_UNKNOWN : -1,
    SOFT_COMPRESSION_NONE : 0x00,
    SOFT_COMPRESSION_MINILZO : 0x01,
    SOFT_COMPRESSION_GZIP : 0x02,
    SOFT_COMPRESSION_BZIP2 : 0x03,
    SOFT_COMPRESSION_QLZW : 0x04,
    SOFT_COMPRESSION_QLZW_MINILZO : 0x05,
    SOFT_COMPRESSION_RLE : 0x06,
    SOFT_COMPRESSION_RLE_QLZW : 0x07,
    SOFT_COMPRESSION_RLE_PIII : 0x08,
    SOFT_COMPRESSION_NONE_PIII : 0x0A
}

var SOCFrameHdr = {
    HDR_SIZE : 34,

    FRAME_TYPE_UNKNOWN : -1,
    FRAME_TYPE_ASCII_ONLY : 0,
    FRAME_TYPE_ASCII_ATTR : 1,
    FRAME_TYPE_ASCII_ATTR_FONT : 2,
    FRAME_TYPE_VGA_4BPP : 3,
    FRAME_TYPE_VGA_8BPP : 4,
    FRAME_TYPE_VESA_8BPP : 5,
    FRAME_TYPE_VESA_16BPP : 6,
    FRAME_TYPE_VESA_32BPP : 7,
    FRAME_TYPE_VESA_15BPP : 8,
    FRAME_TYPE_VESA_24BPP : 9
}

VideoPlayback.prototype = {

    NO_SCREEN_CHANGE: 0xaa, //170

    BLANK_SCREEN: 0x66, //102

    SCREEN_CHANGE: 0x55, //85

    CAPTURE_FRAME_BIT: 0x0001,

    CAPTURE_EVEN_FRAME: 0x0000,

    CAPTURE_ODD_FRAME: 0x0001,

    CAPTURE_CLEAR_FRAME: 0x0002,

    CAPTURE_CLEAR_BUFFER: 0x0002,

    //FRAME_SPEED: 10, //ms

    MAX_X_RESOLUTION: 1800,

    MAX_Y_RESOLUTION: 1800,

    BIT0 : (1<<0),
    BIT1 : (1<<1),
    BIT2 : (1<<2),
    BIT3 : (1<<3),
    BIT4 : (1<<4),
    BIT5 : (1<<5),

    findFrameSpeed: function(stream) {
        var num_frames = 0;
        var start_time = 0;
        var ts = 0;

        while(!stream.isEof()) {
            ts = stream.readUint32();
            var code = stream.readUint8();
            num_frames++;
            if(start_time == 0) {
                start_time = ts;
            }
            stream.readUint16();
            stream.readUint8();

            if(code == this.SCREEN_CHANGE) {
                stream.seek(stream.position + 5);
                var compress_size = stream.readUint32();
                stream.seek(stream.position + 25 + compress_size);
            }
        }
        stream.seek(0);
        this.fps = num_frames / (ts - start_time);
        this.frame_delay = 1000 / this.fps;
    },

    parseRecordedFrameHeader: function(stream) {

        if(!this.playing) {
            return;
        }

        if (stream.isEof()) {
            this.playing = false;
            //$(".dl").removeClass("disabled");
            if(this.recordVideo) {
                this.stopRecording();
            }
            return;
        }

        var ts = stream.readUint32();
        var code = stream.readUint8();
        this.num_frames++;

        //skip padding
        stream.readUint16();
        stream.readUint8();

        var _video = this;

        switch (code) {
            case this.NO_SCREEN_CHANGE:
                setTimeout(function() {
                    _video.parseRecordedFrameHeader.call(_video, stream);
                }, _video.frame_delay);
                break;
            case this.BLANK_SCREEN:
                this.paintBlankScreen();
                setTimeout(function() {
                    _video.parseRecordedFrameHeader.call(_video, stream);
                }, _video.frame_delay);
                break;
            case this.SCREEN_CHANGE:
                this.parseFrameHeader(stream);

                setTimeout(function() {
                    _video.parseRecordedFrameHeader.call(_video, stream);
                }, _video.frame_delay);
                break;
        }


    },

    getFrameType    : function() {

        var hdr = this.currentFrameHeader;

        if((hdr.videoFlags & this.BIT5) > 0) {
            this.nonMatrox = 1;
        }

        if((hdr.videoFlags & this.BIT0) > 0) {
            if((hdr.videoFlags & this.BIT3) > 0) {
                return SOCFrameHdr.FRAME_TYPE_VESA_15BPP;
            } else if(hdr.bytesPP == 1) {
                return SOCFrameHdr.FRAME_TYPE_VESA_8BPP;
            } else if(hdr.bytesPP == 2) {
                return SOCFrameHdr.FRAME_TYPE_VESA_16BPP;
            } else if(hdr.bytesPP == 4) {
                return SOCFrameHdr.FRAME_TYPE_VESA_32BPP;
            } else if(hdr.bytesPP == 5) {
                return SOCFrameHdr.FRAME_TYPE_VGA_4BPP;
            } else if(hdr.bytesPP == 3) {
                return SOCFrameHdr.FRAME_TYPE_VESA_24BPP;
            }
        } else if((hdr.videoFlags & this.BIT1) > 0) {
            if((hdr.textFlags & this.BIT2) > 0) {
                if((hdr.textFlags & this.BIT3) > 0) {
                    if((hdr.textFlags & this.BIT4) > 0) {
                        return SOCFrameHdr.FRAME_TYPE_ASCII_ATTR_FONT;
                    }
                    return SOCFrameHdr.FRAME_TYPE_ASCII_ATTR;
                }
                return SOCFrameHdr.FRAME_TYPE_ASCII_ONLY;
            }
        } else if((hdr.videoFlags & this.BIT3) > 0) {
            return SOCFrameHdr.FRAME_TYPE_VGA_8BPP;
        } else if((hdr.videoFlags & this.BIT4) > 4) {
            return SOCFrameHdr.FRAME_TYPE_VGA_4BPP;
        }
        return SOCFrameHdr.FRAME_TYPE_UNKNOWN;
    },

    isTextFrame : function() {
        var fType = this.getFrameType();
        if((fType == SOCFrameHdr.FRAME_TYPE_ASCII_ONLY) ||
            (fType == SOCFrameHdr.FRAME_TYPE_ASCII_ATTR) ||
            (fType == SOCFrameHdr.FRAME_TYPE_ASCII_ATTR_FONT)) {
            return true;
        }
        return false;
    },

    parseFrameHeader: function(stream) {
        this.currentFrameHeader = {
            flags   :   stream.readUint32(),
            compressionType: stream.readUint8(),
            size    :   stream.readUint32(),
            res_x   :   stream.readUint16(),
            res_y   :   stream.readUint16(),

            width   :   stream.readUint16(),
            height  :   stream.readUint16(),
            syncLossInvalid : stream.readUint8(),
            modeChange : stream.readUint8(),
            tileColSize : stream.readUint8(),
            tileRowSize : stream.readUint8(),
            textOffset : stream.readUint8(),

            bytesPP : stream.readUint8(),
            videoFlags : stream.readUint8(),
            charHeight : stream.readUint8(),
            left    : stream.readUint8(),
            right   : stream.readUint8(),
            top     : stream.readUint8(),
            bottom  : stream.readUint8(),
            textFlags : stream.readUint8(),
            act_bpp : stream.readUint8(),
            tileCapMode : stream.readUint16(),
            bandwidthMode : stream.readUint8()
            
        };

        var scale = Math.min(window.innerWidth / this.currentFrameHeader.res_x, window.innerHeight / this.currentFrameHeader.res_y) * 0.75;
        this.$canvas.css({"-webkit-transform": "scaleX(" + scale + ") scaleY(" + scale + ")"});
        this.$canvas.trigger("align_elements");

        this.parseFrame(stream);
    },

    parseFrame: function(stream) {
        var compressedBuffer = stream.readUint8Array(this.currentFrameHeader.size);
        this.decompressedBuffer = new DataStream(SOCFrameHdr.HDR_SIZE, 0, DataStream.LITTLE_ENDIAN);
        
        switch(this.currentFrameHeader.compressionType) {
            case Compression.SOFT_COMPRESSION_NONE:
            case Compression.SOFT_COMPRESSION_NONE_PIII: 
                /*this.decompressedBuffer = new DataStream(compressedBuffer);
                var handler = this.frameHandler.getFrameHandler(this.getFrameType(this.currentFrameHeader));

                var vid = this;
                handler.handle(vid.decompressedBuffer, vid.currentFrameHeader, vid.imageBufferData, vid);
                this.repaint();*/
                if(compressedBuffer.length)
                    this.handlerWorker.postMessage({buffer: compressedBuffer.buffer, cmd:'data', header: this.currentFrameHeader});
                    //this.handlerWorker.postMessage({buffer: compressedBuffer.buffer, cmd:'data', header: this.currentFrameHeader},[compressedBuffer.buffer]);
                //console.log("framebuffer", compressedBuffer);
            break;

            case Compression.SOFT_COMPRESSION_RLE_PIII: 
                
                //this.compressWorker.postMessage({inbuffer: compressedBuffer.buffer, offset:0, header:this.currentFrameHeader},[compressedBuffer.buffer]);
                this.compressWorker.postMessage({inbuffer: compressedBuffer.buffer, offset:0, header:this.currentFrameHeader});

            break;
        }
    },

    paintBlankScreen : function(){
        this.context.beginPath();
        this.context.rect(0, 0, this.$canvas.width(), this.$canvas.height());
        this.context.fillStyle = 'black';
        this.context.fill();
        if(this.blank_count > 1) {
            this.context.font = 'bold 48pt Source Sans Pro';
            this.context.textAlign="center";
            this.context.fillStyle ="white";
            this.context.fillText("No Signal",this.$canvas.width() / 2,this.$canvas.height() / 2);
        }
        this.imageBuffer = this.context.getImageData(0, 0, this.$canvas.width(),this.$canvas.height());
        this.imageBufferData = this.imageBuffer.data;
    },

    onResolutionChange : function(hdr){
        if(!hdr) hdr = this.currentFrameHeader;

        var res_x = hdr.res_x;
        if(this.isTextFrame()) {
            res_x *= 8; //char Width;
        }

        this.prev_width = this.canvas.width;
        this.prev_height = this.canvas.height;
        this.$canvas.width(res_x).attr("width", res_x);
        this.$canvas.height(hdr.res_y).attr("height", hdr.res_y);
        this.paintBlankScreen();
        this.handlerWorker.postMessage({cmd:'resolution_changed', imageBuffer: this.imageBuffer});
        if(this.recordVideo && (this.prev_width != this.canvas.width || this.prev_height != this.canvas.height)) {
            this.stopRecording();
            this.startRecording();
        }
    },

    repaint: function() {
        this.context.putImageData(this.imageBuffer, 0, 0);
        delete this.currentFrame;
    },

    startRecording: function() {
        window.builder = new movbuilder.MotionJPEGBuilder();
        window.builder.setup(this.$canvas.width(),
            this.$canvas.height(),
            Math.ceil(this.fps));
        this.recordVideo = true;
        var that = this;
        //todo: change it to request animation frame
        window.recordVideoTimer = setInterval(function() {
            if (that.recordVideo) {
                window.builder.addCanvasFrame(that.canvas);
            }
        }, 1000 / Math.ceil(this.fps));
    },

    stopRecording: function() {
        clearInterval(window.recordVideoTimer);
        var that = this;
        window.builder.finish(function(generatedURL) {
            if(!that.playing) {
                that.dl_url = generatedURL;
                if(that.dl) {
                    open(that.dl_url);
                }
            }
        });
        this.recordVideo = false;
    },

};

if(typeof define=="function" && define.amd) {
	define(function(){
		return VideoPlayback;
	});
}
