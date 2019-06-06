/* H5Viewer SOC plugin version
** Note: this version should be updated according to the H5viewer_SOC package version */
SOC_VERSION = "2.42.0";
SOC_NAME = "AST"; // SOC name

VideoEngineConfigs ={
		differential_setting: 0,
		dct_quant_quality:0,
		dct_quant_tbl_select:0,
		sharp_mode_selection:0,
		sharp_quant_quality:0,
		sharp_quant_tbl_select:0,
		compression_mode:0,
		vga_dac:0
};

SOC_IVTP = {
	IVTP_HW_CURSOR: 0x1002,
	IVTP_GET_VIDEO_ENGINE_CONFIGS: 0x1003,
	IVTP_SET_VIDEO_ENGINE_CONFIGS: 0x1004
};

var Video = function(canvas, ip) {
    //initialize code
    this.compression_menu_id = new Array(/* compression menu */"#soc_YUV_420","#soc_YUV_444","#soc_YUV_444_2color","#soc_YUV_444_4color");
    this.dct_menu_id= new Array(/* DCT Quantization menu*/"#soc_dct_zero","#soc_dct_one","#soc_dct_two","#soc_dct_three","#soc_dct_four","#soc_dct_five","#soc_dct_six","#soc_dct_seven");
    this.soc_compression_name = ["YUV 420","YUV 444","YUV 444 + 2 color VQ","YUV 444 + 4 color VQ"];
    this.soc_dct_quant_name = ["0 Best Quality","1","2","3","4","5","6","7"];
    this.VideoEngineConfigsBuffer = new Uint8Array(8);
   // this.screenBuffer = [];
  //  this.screenBuffer[0] = new Int16Array(this.MAX_X_RESOLUTION * this.MAX_Y_RESOLUTION);
  //  this.screenBuffer[1] = new Int16Array(this.MAX_X_RESOLUTION * this.MAX_Y_RESOLUTION);
    this.soc_menu_list = this.compression_menu_id.concat(this.dct_menu_id);

   // this.screen1 = null;
   // this.screen2 = null;

    hardwareCursor();
    this.fullScreen = false;
    this.running = false;

    this.canvas = document.getElementById(canvas);
    this.textbox = document.getElementById('kvm_textbox');
    this.$canvas = $(this.canvas);
    this.context = this.canvas.getContext("2d");
    //this.context.globalCompositeOperation = "destination-in";
    this.paintBlankScreen();
    this.imageBuffer = this.context.getImageData(0, 0, this.$canvas.width(), this.$canvas.height());
    this.imageBufferData = this.imageBuffer.data;


    //this.emptyBuffer = new Int16Array(this.MAX_X_RESOLUTION * this.MAX_Y_RESOLUTION);


    var sock = new VideoSocket(ip, this);

    this.sock = sock;

    this.videoRecDuration = 20; //default 20 seconds
    this.videoRecDurationLeft = 0;
    this.partNo =1;
    this.videoRecordDurationTimer = null;
    this.decodeWorker = new Worker('./libs/kvm/ast/decode_worker.js');
   // var bloburl = window.URL.createObjectURL(new Blob(['(',decode_worker.toString(),')()'],{type: 'application/javascript'}));
    //this.decodeWorker = new Worker(bloburl);

    var _video = this;
    window.frames_received = 0*1;
    window.frames_processed = 0*1;
    var oldtimer = new Date().getTime(),
        newtimer = oldtimer;

    this.decodeWorker.onmessage = function(e) {
        switch (e.data.cmd) {
            case 'draw':
                _video.imageBuffer = e.data.ibuf;
		updateCanvasData();
                _video.repaint();
                window.frames_processed++;
                newtimer = new Date().getTime();
                test = "Time: " + (newtimer - oldtimer) / 1000 + " secs";
                //static_log(test);
                oldtimer = newtimer;
                break;
            case 'debug':
                //test2 = e.data.args;
                //static_log.call(window, test+" "+test2);
                break;
            case 'time':
                document.getElementById(e.data.id).innerHTML = e.data.msg;
                break;
            case 'exception':
                //console.log(e.data.ex);
                break;
            case 'resolution_change':
                _video.onResolutionChange();
                window.frames_processed++;
                break;
        }
    };

    this.decodeWorker.postMessage({
        cmd: 'imageBuffer',
        imageBuffer: this.imageBuffer
    });

    if ('createTouch' in document) {
        touchEventListener(this);
    } else {
        eventListener(this);
    }

};

Video.prototype = {

    NO_SCREEN_CHANGE: 0xaa,

    BLANK_SCREEN: 0x66,

    SCREEN_CHANGE: 0x55,

    CAPTURE_FRAME_BIT: 0x0001,

    CAPTURE_EVEN_FRAME: 0x0000,

    CAPTURE_ODD_FRAME: 0x0001,

    CAPTURE_CLEAR_FRAME: 0x0002,

    FRAME_SPEED: 10, //ms

    MAX_X_RESOLUTION: 1800,

    MAX_Y_RESOLUTION: 1800,

    parseRecordedFrameHeader: function(stream) {

        if (stream.isEof()) {
            var rt = 0,
                pt = 0;
            for (var i = 0; i < this.profiler.length; i++) {
                rt += this.profiler[i].runtime;
                pt += this.profiler2[i].paintruntime;
            }
            //console.log("profile avg time", (rt / this.profiler.length) + "ms");
            //console.log("paint avg time", (pt / this.profiler2.length) + "ms");
            return;
        }

        var ts = stream.readUint32();
        var code = stream.readUint8();

        //console.log('rec frame header',ts, code);

        //skip padding
        stream.readUint16();
        stream.readUint8();


        var _video = this;

        switch (code) {
            case this.NO_SCREEN_CHANGE:
                setTimeout(function() {
                    _video.parseRecordedFrameHeader.call(_video, stream);
                }, this.FRAME_SPEED);
                break;
            case this.BLANK_SCREEN:
                this.paintBlankScreen();
                setTimeout(function() {
                    _video.parseRecordedFrameHeader.call(_video, stream);
                }, this.FRAME_SPEED);
                break;
            case this.SCREEN_CHANGE:
                this.parseFrameHeader(stream);
                break;
        }


    },

    parseFrameHeader: function(stream) {
        this.currentFrameHeader = {
            iEngineVersion: stream.readUint16(),
            wHeaderLen: stream.readUint16(),
            SourceMode: {
                X: stream.readUint16(),
                Y: stream.readUint16(),
                ColorDepth: stream.readUint16(),
                RefreshRate: stream.readUint16(),
                ModeIndex: stream.readUint8()
            },
            DestinationMode: {
                X: stream.readUint16(),
                Y: stream.readUint16(),
                ColorDepth: stream.readUint16(),
                RefreshRate: stream.readUint16(),
                ModeIndex: stream.readUint8()
            },
            FrameHdr: {
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

        //console.log('frame header', this.currentFrameHeader);
        //console.log(stream.readUint8Array());
        this.parseFrame(stream);
    },

    parseFrame: function(stream) {

        if (this.currentFrameHeader.SourceModeInfo.X != this.$canvas.width() ||
            this.currentFrameHeader.SourceModeInfo.Y != this.$canvas.height()) {
            this.onResolutionChange();
        }

        var compressedBuffer = stream.readInt32Array(this.currentFrameHeader.CompressData.CompressSize / 4);

        //var cb2 = this.makeIntArray(compressedBuffer);

        //var cb = new Int32Array(compressedBuffer.buffer);


        //console.log(compressedBuffer, cb);

        this.decodeWorker.postMessage({
            header: this.currentFrameHeader,
            buffer: compressedBuffer
        });

        //this.repaint();

    },

    makeIntArray: function(buffer) {

        var btemp, pos = 0;

        if (buffer.length % 4 == 3) {
            btemp = new Int8Array(buffer.length + 1);
            btemp.set(buffer.subarray(0));
            btemp[btemp.length - 1] = 0;
        } else if (buffer.length % 4 == 2) {
            btemp = new Int8Array(buffer.length + 2);
            btemp.set(buffer.subarray(0));
            btemp[btemp.length - 2] = 0;
            btemp[btemp.length - 1] = 0;
        } else if (buffer.length % 4 == 1) {
            btemp = new Int8Array(buffer.length + 3);
            btemp.set(buffer.subarray(0));
            btemp[btemp.length - 3] = 0;
            btemp[btemp.length - 2] = 0;
            btemp[btemp.length - 1] = 0;
        } else {
            btemp = new Int8Array(buffer.length);
            btemp.set(buffer.subarray(0));
        }

        var iarr = new Int32Array(btemp.length / 4);
        for (i = 0; i < btemp.length; i++) {
            iarr[i] = (btemp[pos + 3] & 0xff) << 24;
            iarr[i] |= (btemp[pos + 2] & 0xff) << 16;
            iarr[i] |= (btemp[pos + 1] & 0xff) << 8;
            iarr[i] |= (btemp[pos + 0] & 0xff);
            pos += 4;
        }
	delete btemp;
	btemp = null;
        return iarr;

    },

    paintBlankScreen: function() {
        this.context.beginPath();
        this.context.rect(0, 0, this.$canvas.width(), this.$canvas.height());
        this.context.fillStyle = 'black';
        this.context.fill();
	if(window._video != null && window._video.sock != null)
	{
		// send power control request on blank screen
		var powerStatus = window._video.sock.createHeader(IVTP.CMD_POWER_STATUS, 0, 0);
		window._video.sock.send(powerStatus.buffer);
	}
        if(this.blank_count >= 1) {
            this.context.font = 'bold 48pt Source Sans Pro';
            this.context.textAlign="center";
            this.context.fillStyle ="white";
            if(this.power_status == 1) {
                this.context.fillText("No Signal",this.$canvas.width() / 2,this.$canvas.height() / 2);
            }
            else {
                this.context.fillText("Powered Off",this.$canvas.width() / 2,this.$canvas.height() / 2);
            }
        }
        this.imageBuffer = this.context.getImageData(0, 0, this.$canvas.width(), this.$canvas.height());
        this.imageBufferData = this.imageBuffer.data;
        if (this.decodeWorker) {
            this.decodeWorker.postMessage({
                cmd: 'resolution_changed',
                imageBuffer: this.imageBuffer
            });
        }
    },
    
    paintWhiteScreen: function() {
        this.context.beginPath();
        this.context.rect(0, 0, this.$canvas.width(), this.$canvas.height());
        this.context.fillStyle = 'white';
        this.context.fill();
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
        //console.log("On Resolution Change", this.canvas.width, this.canvas.height, hdr.width, hdr.height);

        this.prev_width = this.canvas.width;
        this.prev_height = this.canvas.height;

        this.$canvas.width(hdr.SourceModeInfo.X).attr("width", hdr.SourceModeInfo.X);
        this.$canvas.height(hdr.SourceModeInfo.Y).attr("height", hdr.SourceModeInfo.Y);
        this.paintBlankScreen();
        this.decodeWorker.postMessage({
                cmd: 'resolution_changed',
                imageBuffer: this.imageBuffer
        });
        if(window.recordVideo && (this.prev_width != this.canvas.width || this.prev_height != this.canvas.height)) {
            this.stopRecording();
            this.startRecording();
        }
    },

    validateNewFrame: function(stream) {

    },

    repaint: function() {
        this.context.putImageData(this.imageBuffer, 0, 0);
        delete this.currentFrame;
	this.currentFrame = null;
	delete this.imageBuffer;
	this.imageBuffer = null;
     //   var _parent = this;
       // setTimeout(function() {
         //   _parent.sock.startRead.call(_parent.sock);
        //}, 1);

    },

    stop: function() {
        this.sock.close();
	delete this.VideoEngineConfigsBuffer;
	this.VideoEngineConfigsBuffer = null;
    	this.decodeWorker.terminate();
    },
	cmdOnVideoPackets: function(ivtp_hdr) {

			running = true;
                        var fragbuf = this.sock.nwBuffer.buffer.slice(this.sock.pos, this.sock.pos + 2);
                        delete fb;
			fb = null;
                        var fb = new DataStream(fragbuf, 0, DataStream.LITTLE_ENDIAN);
                        var frag_num = fb.readUint16();
                        this.sock.pos += 2;
                        delete fb;
			fb = null;

                        var u8vpkt = null;
                        if (this.sock.prev_complete) {
                            //ivtp_hdr is taken
                            //now fetch video pkt hdr after a short int (fragment number)

                            var ab = this.sock.nwBuffer.buffer.slice(this.sock.pos, this.sock.pos + 86); //video pkt hdr
                            this.sock.pos += 86;
                            var stream = new DataStream(ab);
                            //first frame with header
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
                            //console.log("frame hdr", this.video.currentFrameHeader);
                            delete this.sock.currentFrame;
                            this.sock.currentFrame = null;
                            this.sock.currentFrame = new DataStream();
                            u8vpkt = new Uint8Array(this.sock.nwBuffer.buffer.slice(this.sock.pos, this.sock.pos + ivtp_hdr.pkt_size - 88));
                            this.sock.pos += ivtp_hdr.pkt_size - 88;
                            delete stream;
                            stream = null;
                        if(this.sock.firstPacket  == true/*|| this.hdr.SourceModeInfo.X != this.currentFrameHeader.X || this.hdr.SourceModeInfo.Y != this.currentFrameHeader.Y*/) {
                            //this.sock.hdr.SourceModeInfo.X = this.currentFrameHeader.X;
                            //this.sock.hdr.SourceModeInfo.Y = this.currentFrameHeader.Y;
                            //this.currentFrameHeader = this.sock.cfh;
                            this.onResolutionChange();
                            this.sock.firstPacket = false;
                        }
                        } else {
                            //successive frames
                            u8vpkt = new Uint8Array(this.sock.nwBuffer.buffer.slice(this.sock.pos, this.sock.pos + ivtp_hdr.pkt_size - 2));
                            this.sock.pos += ivtp_hdr.pkt_size - 2;
                        }

			for (var i = 0; i < u8vpkt.length; i++) {
				this.sock.currentFrame.writeUint8(u8vpkt[i], i);
			}

                        if (this.sock.currentFrame.byteLength == this.currentFrameHeader.CompressData.CompressSize) {
                            window.frames_received ++;
                            this.sock.prev_complete = true;
                            // console.log("video frame load complete");
                            this.sock.currentFrame.seek(0);
                            //this.video.validateNewFrame();
                            this.parseFrame(this.sock.currentFrame);
                        } else {
                            // console.log("video frame load pending. frag_num="+frag_num);
                            this.sock.prev_complete = false;
                        if(this.running == true);
                          //  this.sock.startRead();
                        }
                        delete u8vpkt;
                        u8vpkt = null;
                        this.sock.blank_count = 0;
    	},

	cmdOnColorPalette: function (){
    },

	cmdOnAttribute: function () {
    },
    //get Video Engine Configuration
    getVideoEngineConfigs: function() {
        Object.getOwnPropertyNames(VideoEngineConfigs).forEach(function(val, idx, array) {
           window._video.VideoEngineConfigsBuffer[idx]=VideoEngineConfigs[val];
        });
    },
    //send VideoConfig Packet to adviser
    sendPacket: function() {
        var hdr=window._video.sock.createHeader(SOC_IVTP.IVTP_SET_VIDEO_ENGINE_CONFIGS,8,0,this.VideoEngineConfigsBuffer);
        window._video.sock.send(hdr.buffer);
    },
    //handle soc specific cmd
    onSocControlMessage : function(ivtp_hdr,m_ctrlMsg) {
        switch (ivtp_hdr.type){
        case SOC_IVTP.IVTP_GET_VIDEO_ENGINE_CONFIGS:
            window._video.setVideoEngineConfigs(m_ctrlMsg);
            delete m_ctrlMsg;
            m_ctrlMsg=null;
	    break;
	case SOC_IVTP.IVTP_HW_CURSOR: 
 	    updateHardwareCursor(m_ctrlMsg); 
 	    m_ctrlMsg=null; 
 	    break;

        default:
            console.log("invalid header type");
        }
    },
    //set Video Engine Configuration from adviser
    setVideoEngineConfigs: function(m_ctrlMsg) {
     //read the video engine configuration
        for(i=0;i<8;i++){
            VideoEngineConfigs[i]=m_ctrlMsg.readUint8();
        }
        //compression mode value from adviser
        compression_mode_value=VideoEngineConfigs[6];
        for(i=0;i<this.compression_menu_id.length;i++){
            //remove tick icon from all the compression menu option
            if($(this.compression_menu_id[i]).children("i").length !=0) {
                $(this.compression_menu_id[i]).children("i").remove();
            }
            //if compression mode value(0 t0 4) match the condition, add tick mark to that option
            if(compression_mode_value == i ){
                $(this.compression_menu_id[i]).append("<i class=\"fa fa-check\" id=\"cm-select\"></i>");
           }
        }
        //DCT qunatization table value from adviser
        dct_quant_tbl_select_value = VideoEngineConfigs[2];
        for(i=0;i<this.dct_menu_id.length;i++){
            //remove the existing tick mark from all dct quantization menu
            if($(this.dct_menu_id[i]).children("i").length !=0) {
                $(this.dct_menu_id[i]).children("i").remove();
            }
            //if dct quantization value(0 t0 7) match the condition, add tick mark to that option
            if(dct_quant_tbl_select_value == i ){
                $(this.dct_menu_id[i]).append("<i class=\"fa fa-check\" id=\"dq-select\"></i>");
            }
        }
    },
    //compression menu Event Handler
    compressionMenu : function(ev){
        ev.preventDefault();
        window._video.getVideoEngineConfigs();
        $("#cm-select").appendTo($(ev.currentTarget));
        // set compression mode value
        switch(ev.target.id){
            case "soc_YUV_420":
                window._video.VideoEngineConfigsBuffer[6]=0;
                break;
            case "soc_YUV_444":
                window._video.VideoEngineConfigsBuffer[6]=1;
                break;
            case "soc_YUV_444_2color":
                window._video.VideoEngineConfigsBuffer[6]=2;
                break;
            case "soc_YUV_444_4color":
                window._video.VideoEngineConfigsBuffer[6]=3;
                break;
        }
        window._video.sendPacket();
    },
    //Dct quantization table menu Event Handler
    dctQuantizationTableMenu : function (ev) {
        ev.preventDefault();
        window._video.getVideoEngineConfigs();
        $("#dq-select").appendTo($(ev.currentTarget));
        // set DCT_Quantization_table
        switch(ev.target.id){
            case "soc_dct_zero":
                window._video.VideoEngineConfigsBuffer[2]=0;
                break;
            case "soc_dct_one":
                 window._video.VideoEngineConfigsBuffer[2]=1;
                 break;
            case "soc_dct_two":
                 window._video.VideoEngineConfigsBuffer[2]=2;
                 break;
            case "soc_dct_three":
                 window._video.VideoEngineConfigsBuffer[2]=3;
                 break;
            case "soc_dct_four":
                 window._video.VideoEngineConfigsBuffer[2]=4;
                 break;
            case "soc_dct_five":
                 window._video.VideoEngineConfigsBuffer[2]=5;
                 break;
             case "soc_dct_six":
                 window._video.VideoEngineConfigsBuffer[2]=6;
                 break;
             case "soc_dct_seven":
                 window._video.VideoEngineConfigsBuffer[2]=7;
                 break;
            }
            window._video.sendPacket();
    },

    updateSocMenu: function() {
        this.parentId="id_option_menu";
        this.index=-1;
        window._video.sock.createMenuItem(this.compression_menu_id,this.soc_compression_name,this.parentId,this.index,window._video.compressionMenu, true, "compression_menu");
        $(this.compression_menu_id[0]).append("<i id= \"cm-select\" class=\"fa fa-check\"></i>");
        window._video.sock.createMenuItem(this.dct_menu_id,this.soc_dct_quant_name,this.parentId,this.index,window._video.dctQuantizationTableMenu, true, "dct_menu");
        $(this.dct_menu_id[0]).append("<i id= \"dq-select\" class=\"fa fa-check\"></i>");
    },

    initializeBuilder: function() {
        var obj = new movbuilder.MotionJPEGBuilder();
        //set up canvas width, height and fps
        obj.setup(this.$canvas.width(),
            this.$canvas.height(),
            30);
        return obj;
    },

    startRecording: function() {
        if (!this.running) {
            return;
        }

	var that = this;
        window.builder1 = window._video.initializeBuilder();
        window.builder1.isActive = true;

        //todo: change it to request animation frame
	//Start timer
		
	window._video.videoRecDurationLeft  = window._video.videoRecDuration;
	// append string to show record time status
	$("#record_progress").append(" ( "+ window._video.videoRecDuration +" ) ");
	$("#record_progress").show();
	
	//  timer to change record duration recursively and to update record status 	
        window.videoRecordDurationTimer = setInterval(function() {		
		var str = document.getElementById("record_progress").innerHTML; 
		if(window._video.videoRecDurationLeft != 0)
		{
			var txt = str.replace(window._video.videoRecDurationLeft,window._video.videoRecDurationLeft-1);
		}
		$("#record_progress").html(txt);			
	
		if ((window.recordVideo == true) &&
			(window._video.videoRecDurationLeft <= 0) ){
			$("#stop").click();
		}

		if( window.builder1 != null) {
	            /* Once builder1 object stream size reaches around 40MB, builder2 object will be initialized and start to add canvas to the data.*/
	            if( window.builder1.movieDesc.videoStreamSize > 40000000) { //around 40MB
	                if( window.builder1.isActive == true){
	                    window.builder1.movieDesc.videoStreamSize =0;
	                    window.builder2 = window._video.initializeBuilder();
	                    window.builder2.isActive = true;
	                    window._video.endVideoRecording(window.builder1);
	                    delete window.builder1;
	                    window.builder1 = null;
	                    window._video.partNo++;
	                }
	            }
	        }
	        else if( window.builder2 != null ) {
	            /* Once builder2 object stream size reaches around 40MB, builder1 object will be initialized and start to add canvas to the data.*/
	            if( window.builder2.movieDesc.videoStreamSize > 40000000) { //around 40MB
	                if( window.builder2.isActive == true) {
	                    window.builder2.movieDesc.videoStreamSize =0;
	                    window.builder1 = window._video.initializeBuilder();
	                    window.builder1.isActive = true;
	                    window._video.endVideoRecording(window.builder2);
	                    delete window.builder2;
	                    window.builder2 = null;
	                    window._video.partNo++;
	                }
	            }
	        }

 		window._video.videoRecDurationLeft = window._video.videoRecDurationLeft - 1;
	}, (1000));

	window.recordVideo = true;
        window.recordVideoTimer = setInterval(function() {
            if ( window.recordVideo == true ){
                if (window.builder1 != null ){
                    if( window.builder1.isActive == true )
                        window.builder1.addCanvasFrame(that.canvas);
                }
                else if ( window.builder2 != null ) {
                    if( window.builder2.isActive == true) {
                        window.builder2.addCanvasFrame(that.canvas);
                    }
                }
            }
        }, 33);

    },

    endVideoRecording: function(obj) { 
	console.log(" endVideoRecording...."); 
 	obj.finish(function(generatedURL) { 
 	// checks whether video file is already downloaded or not 
 	if( $("#download_Video_file").css('visibility') != 'hidden' ) { 
 		open(generatedURL); 
 	} 
 	}); 
    },

    stopRecording: function() {
        clearInterval(window.recordVideoTimer);
        clearInterval(window.videoRecordDurationTimer);
        if( window.builder1 != null ) {
            if( window.builder1.isActive == true) {
                window._video.endVideoRecording(window.builder1);
                window.builder1.isActive = false;
                window.builder1= null;
            }
        }
        else if( window.builder2 != null ) {
            if( window.builder2.isActive == true) {
                window._video.endVideoRecording(window.builder2);
                window.builder2.isActive = false;
                window.builder2 = null;
            }
        }
        window.recordVideo = false;
        window._video.partNo =1;
    } 
   
};

if(typeof define=="function" && define.amd) {
	define(function(){
		return Video;
	});
}
