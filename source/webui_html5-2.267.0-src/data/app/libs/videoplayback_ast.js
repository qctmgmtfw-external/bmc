var VideoPlayback = function(canvas) {
    this.num_frames = 0;
	this.blank_count=0;

    this.canvas = document.getElementById(canvas);
    this.$canvas = $(this.canvas);
    this.context = this.canvas.getContext("2d");

    this.alignmentTimer;

    this.timer();

    this.paintBlankScreen();

    this.emptyBuffer = new Int16Array(this.MAX_X_RESOLUTION * this.MAX_Y_RESOLUTION);
    
    this.decodeWorker = new Worker("./libs/kvm/ast/decode_worker.js");
    var _video = this;

    this.decodeWorker.onmessage = function(e) {
        switch (e.data.cmd) {
            case 'draw':
                _video.imageBuffer = e.data.ibuf;
                _video.repaint();
                break;
            case 'debug':
                //test2 = e.data.args;
                //static_log.call(window, test+" "+test2);
                break;
            case 'time':
                document.getElementById(e.data.id).innerHTML = e.data.msg;
                break;
            case 'exception':
                console.log(e.data.ex);
                break;
        }
    };

    this.decodeWorker.postMessage({
        cmd: 'imageBuffer',
        imageBuffer: this.imageBuffer
    });
};

VideoPlayback.prototype = {

    NO_SCREEN_CHANGE: 0xaa, //170

    BLANK_SCREEN: 0x66, //102

    SCREEN_CHANGE: 0x55, //85

    CAPTURE_FRAME_BIT: 0x0001,

    CAPTURE_EVEN_FRAME: 0x0000,

    CAPTURE_ODD_FRAME: 0x0001,

    CAPTURE_CLEAR_FRAME: 0x0002,

    //FRAME_SPEED: 10, //ms

    MAX_X_RESOLUTION: 1800,

    MAX_Y_RESOLUTION: 1800,

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
                stream.seek(stream.position + 69);
                var compress_size = stream.readUint32();
                stream.seek(stream.position + 13 + compress_size);
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

		if(code== this.BLANK_SCREEN){
            this.blank_count++;
        }

        var _video = this;

        switch (code) {
            case this.NO_SCREEN_CHANGE:
                setTimeout(function() {
                    _video.parseRecordedFrameHeader.call(_video, stream);
                }, _video.frame_delay);
                break;
            case this.BLANK_SCREEN:
                this.alignmentTimer=null;
                clearInterval(this.alignmentTimer);
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

    parseFrameHeader: function(stream) {
        this.currentFrameHeader = {
            iEngineVersion: stream.readUint16(),
            wHeaderLen: stream.readUint16(),
            SourceModeInfo: {
                X: stream.readUint16(),
                Y: stream.readUint16(),
                ColorDepth: stream.readUint16(),
                RefreshRate: stream.readUint16(),
                ModeIndex: stream.readUint8()
            },
            DestinationModeInfo: {
                X: stream.readUint16(),
                Y: stream.readUint16(),
                ColorDepth: stream.readUint16(),
                RefreshRate: stream.readUint16(),
                ModeIndex: stream.readUint8()
            },
            FrameHeader: {
                StartCode: stream.readUint32(),
                FrameNumber: stream.readUint32(),
                HSize: stream.readUint16(),
                VSize: stream.readUint16(),
                reserved: [stream.readUint32(), stream.readUint32()],
                CompressionMode: stream.readUint8(),
                JPEGScaleFactor: stream.readUint8(),
                JPEGTableSelector: stream.readUint8(),
                JPEGYUVTableMapping: stream.readUint8(),
                SharpModeSelection: stream.readUint8(),
                AdvanceTableSelector: stream.readUint8(),
                AdvanceScaleFactor: stream.readUint8(),
                NumberOfMB: stream.readUint32(),
                RC4Enable: stream.readUint8(),
                RC4Reset: stream.readUint8()
            },
            Mode420: stream.readUint8(),
            INFData: {
                DownScalingMethod: stream.readUint8(),
                DifferentialSetting: stream.readUint8(),
                AnalogDifferentialThreshold: stream.readUint16(),
                DigitalDifferentialThreshold: stream.readUint16(),
                ExternalSignalEnable: stream.readUint8(),
                AutoMode: stream.readUint8(),
                VQMode: stream.readUint8()
            },
            CompressData: {
                SourceFrameSize: stream.readUint32(),
                CompressSize: stream.readUint32(),
                HDebug: stream.readUint32(),
                VDebug: stream.readUint32()
            },
            InputSignal: stream.readUint8(),
            Cursor: {
                XPos: stream.readUint16(),
                YPos: stream.readUint16()
            }
        };

        var scale = Math.min(window.innerWidth / this.currentFrameHeader.SourceModeInfo.X, window.innerHeight / this.currentFrameHeader.SourceModeInfo.Y) * 0.75;
        this.$canvas.css({"-webkit-transform": "scaleX(" + scale + ") scaleY(" + scale + ")"});
        //this.$canvas.trigger("align_elements");

        this.parseFrame(stream);
    },

    parseFrame: function(stream) {
        if (this.currentFrameHeader.SourceModeInfo.X != this.$canvas.width() ||
            this.currentFrameHeader.SourceModeInfo.Y != this.$canvas.height()) {
            this.onResolutionChange();
        }

        var compressedBuffer = stream.readInt32Array(this.currentFrameHeader.CompressData.CompressSize / 4);

        this.decodeWorker.postMessage({
            header: this.currentFrameHeader,
            buffer: compressedBuffer
        });
    },

    paintBlankScreen: function() {
        this.context.beginPath();
        this.context.rect(0, 0, this.$canvas.width(), this.$canvas.height());
        this.context.fillStyle = 'black';
        this.context.fill();
        if(this.blank_count >= 1) {
            this.context.font = 'bold 48pt Source Sans Pro';
            this.context.textAlign="center";
            this.context.fillStyle ="white";
            this.context.fillText("No Signal",this.$canvas.width() / 2,this.$canvas.height() / 2);
        }
        //this.$canvas.trigger("align_elements");
        this.imageBuffer = this.context.getImageData(0, 0, this.$canvas.width(), this.$canvas.height());
        this.imageBufferData = this.imageBuffer.data;
        if (this.decodeWorker) {
            this.decodeWorker.postMessage({
                cmd: 'resolution_changed',
                imageBuffer: this.imageBuffer
            });
        }
    },

    onResolutionChange: function(hdr) {
        if (!hdr) hdr = this.currentFrameHeader;

        this.prev_width = this.canvas.width;
        this.prev_height = this.canvas.height;

        this.$canvas.width(hdr.SourceModeInfo.X).attr("width", hdr.SourceModeInfo.X);
        this.$canvas.height(hdr.SourceModeInfo.Y).attr("height", hdr.SourceModeInfo.Y);
        this.paintBlankScreen();

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
        this.alignmentTimer=null;
        clearInterval(this.alignmentTimer);
        var that = this;
        /*window.builder.finish(function(generatedURL) {
            if(!that.playing) {
                that.dl_url = generatedURL;
                if(that.dl) {
                    //open(that.dl_url);
                    var downloadURL = that.dl_url;
                    var a = document.createElement("a");
                    var d= new Date();
                    a.setAttribute("download", "video_" + d.getDate()+"-"+(d.getMonth()+1)+"-"+d.getFullYear()+"_"+ d.getHours()+"-"+d.getMinutes()+"-"+d.getSeconds()+".avi");
                    a.setAttribute("href", downloadURL);
                    a.setAttribute("id","download_Video_file");
                    a.style.visibility = 'hidden';
                    document.body.appendChild(a);
                    a.click();
                }
            }
        });*/
        this.recordVideo = false;
    },

    timer: function(){
        var that = this;
        that.alignmentTimer = window.setInterval(function(){that.$canvas.trigger("align_elements");}, 1);
    }

};

if(typeof define=="function" && define.amd) {
	define(function(){
		return VideoPlayback;
	});
}
