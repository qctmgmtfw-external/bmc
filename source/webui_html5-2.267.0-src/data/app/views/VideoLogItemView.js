define([
    "jquery",
    "underscore",
    "backbone",
    'app',
    "i18n!strings/nls/common",
    //template
    'text!templates/videolog-item.html',
    'videoplayback'

], function($, _, Backbone,app, CommonStrings, VLITemplate) {

    var view = Backbone.View.extend({

        tagName: "li",

        template: _.template(VLITemplate),

        initialize: function() {
            this.model.bind("destroy", this.removeView, this);
        },

        events: {
            "click h3": "toggleDetails",
            "click .del": "deleteVideo",
            "click .close-video": "closeVideo",
            "click .dl": "downloadVideo",
            "align_elements canvas": "alignElements"
        },

        beforeRender: function() {
            //this.$el.addClass("animated fadeInDown");
        },

        CANVAS_PORTRAIT_LEFT: -460,
        CANVAS_PORTRAIT_TOP: -200,

        CANVAS_LANDSCAPE_LEFT: -350,
        CANVAS_LANDSCAPE_TOP: -335,

        afterRender: function() {
            this.id = this.model.id;
            this.status_bar_timer = null;
            var that = this;
            if( /Android|webOS|iPhone|iPad|iPod|BlackBerry|IEMobile|Opera Mini/i.test(navigator.userAgent) ) {
                if(window.orientation == 90 || window.orientation == -90) {
                    //landscape
                    this.canvas_left = this.CANVAS_LANDSCAPE_LEFT;
                    this.canvas_top = this.CANVAS_LANDSCAPE_TOP;
                }
                else {
                    //portrait
                    this.canvas_left = this.CANVAS_PORTRAIT_LEFT;
                    this.canvas_top = this.CANVAS_PORTRAIT_TOP;
                }

                $(window).on("orientationchange",function(event){
                    if(window.orientation == 90 || window.orientation == -90) {
                        that.canvas_left = that.CANVAS_LANDSCAPE_LEFT;
                        that.canvas_top = that.CANVAS_LANDSCAPE_TOP;
                        if(typeof that.$canvas != 'undefined' && that.$canvas.is(":visible")) {
                            that.$canvas.css({"left": that.CANVAS_LANDSCAPE_LEFT, "top": that.CANVAS_LANDSCAPE_TOP});
                            that.$canvas.trigger("align_elements");
                        }
                    }
                    else {
                        that.canvas_left = that.CANVAS_PORTRAIT_LEFT;
                        that.canvas_top = that.CANVAS_PORTRAIT_TOP;
                        if(typeof that.$canvas != 'undefined' && that.$canvas.is(":visible")) {
                            that.$canvas.css({"left": that.CANVAS_PORTRAIT_LEFT, "top": that.CANVAS_PORTRAIT_TOP});
                            that.$canvas.trigger("align_elements");
                        }
                    }
                });
            }

            this.$canvas = this.$el.find("canvas");
            this.$del_btn = this.$el.find(".del");
            this.$dl_btn = this.$el.find(".dl");
            this.$close_btn = this.$el.find(".close-video");
            this.$progress_div = this.$el.find(".progress");
            this.$progress_bar = this.$el.find(".progress-bar");
            this.$progress_info = this.$el.find(".progress-info");
            if(!app.isAdmin()){
                this.$el.find(".del").addClass("disabled");
            }
            //this.$el.addClass("timeline-item-holder sensor_" + this.model.get('sensor_number') + " " + this.model.get('record_type'));
        },

        toggleDetails: function() {
            if($("canvas").is(":visible")) {
                return;
            }
            if(!app.isKVM()){
                $('.alert-danger').removeClass("hide");
                $('.alert-danger').html(locale.kvm_disabled);
                return false;
            }
            if(!app.isAdmin() && !app.isOperator()){
                alert(locale.video_rigths);
                return false;
            }
            if($('.alert-danger').length > 0){
                $('.alert-danger').html("");
                $('.alert-danger').addClass("hide");
            }
            
            this.heading = this.$el;
            //$(".progress").hide();

            if (this.$el.find(".timeline-header").hasClass("expanded")) {
                this.$el.find(".timeline-header, .timeline-body").removeClass("expanded");
                if(this.video.recordVideo) {
                    this.video.stopRecording();
                }
                this.video.playing = false;
            }
            else {
                if( /Android|webOS|iPhone|iPad|iPod|BlackBerry|IEMobile|Opera Mini/i.test(navigator.userAgent) ) {
                    this.$canvas.css({"left": this.canvas_left, "top": this.canvas_top});
                }
                this.$progress_info.hide();
                this.$progress_div.hide();
                this.$el.find(".timeline-header, .timeline-body").addClass("expanded");
                //this.video = new Video(this.model.get('file'));
                if (typeof this.video == 'undefined') {
                    var file = this.model.get('file');

                    var oReq = new XMLHttpRequest();
                    oReq.open("GET", "/api/logs/video-data/?file=" + file, true);
                    oReq.responseType = "arraybuffer";
                    oReq.setRequestHeader("X-CSRFTOKEN", window.sessionStorage.getItem("garc"));
                    var that = this;
                    oReq.onload = function (oEvent) {
                        var arrayBuffer = oReq.response; // Note: not oReq.responseText
                        if (arrayBuffer) {
                            if (file.indexOf("video_dump") != -1) { //post event type 
                                  arrayBuffer=arrayBuffer.slice(20);
                            }
                            that.stream = new DataStream(arrayBuffer);
                            that.video = new VideoPlayback("canvas_" + that.id);
                            window._video = that.video;
                            that.video.playing = true;
                            that.video.findFrameSpeed(that.stream);
                            that.video.parseRecordedFrameHeader(that.stream);
                            if(typeof that.video.dl_url == 'undefined') {
                                that.$progress_bar.attr({"aria-valuenow": 0, "aria-valuemax": 1});
                                that.$progress_bar.css("width", "0%");
                                that.video.startRecording();
                                that.startProgressBar.call(that);
                            }
                        }
                    };

                    oReq.send(null);
                }
                else {
                    this.video.paintBlankScreen();
                    this.video.playing = true;
                    this.stream.seek(0);
                    this.video.parseRecordedFrameHeader(this.stream);
                    this.$progress_bar.attr({"aria-valuenow": 0, "aria-valuemax": 1});
                    this.$progress_bar.css("width", "0%");
                    this.$progress_info.html("");
                    this.startProgressBar.call(this);
                    if(typeof this.video.dl_url == 'undefined') {
                        this.$progress_bar.attr({"aria-valuenow": 0, "aria-valuemax": 1});
                        this.$progress_bar.css("width", "0%");
                        this.$progress_info.html("");
                        this.video.startRecording();
                        //this.startProgressBar.call(this);
                    }
                }
            }
        },

        deleteVideo: function(ev) {
            var that = this;
            ev.stopPropagation();
            ev.preventDefault();
            if (!confirm('Are you sure to perform this operation?')) return;
            var data = {};
            var file_name = that.model.get('file');
           //var path = that.model.collection.url().split('?')[0] + '/' + that.model.get('file_name');
            data.file_name = file_name;
            var object = JSON.stringify(data);
            $.ajax({
                url: "api/logs/video-log/delete",
                type: 'DELETE',
                dataType: 'json',
                data: object,
                processData: false,
                contentType: 'application/json',
                success: function(data, status, xhr) {
                    alert('Deleted Successfully');
                    Backbone.history.loadUrl(Backbone.history.fragment);
                },
                error: function(xhr, status, err) {
                    app.HTTPErrorHandler(xhr, status, err);
                }
            });
        },
        closeVideo: function() {
            this.heading.find(".timeline-header, .timeline-body").removeClass("expanded");
            if(this.video.recordVideo) {
                this.video.stopRecording();
                clearInterval(this.status_bar_timer);
                this.status_bar_timer = null;
            }
            this.video.playing = false;
        },

        downloadVideo: function() {
            if(this.status_bar_timer != null) {
                return;
            }

			window.builder.finish(function(generatedURL) {
            /*if(!that.playing) {
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
            }*/
			//this.closeVideo();
       	 });
		this.closeVideo();
            /*if(typeof this.video.dl_url == 'undefined') {
                this.video.dl = true;
                this.$progress_bar.attr({"aria-valuenow": 0, "aria-valuemax": 1});
                this.$progress_bar.css("width", "0%");
                this.$progress_info.html("");
                this.startProgressBar.call(this);
            }
            else {
               // open(this.video.dl_url);
			   var downloadURL =this.video.dl_url; //url.createObjectURL(blob);
              // console.log("ffff"+this.video.dl_url);
               var a = document.createElement("a");
               var d= new Date();
               a.setAttribute("download", "video_" + d.getDate()+"-"+(d.getMonth()+1)+"-"+d.getFullYear()+"_"+ d.getHours()+"-"+d.getMinutes()+"-"+d.getSeconds()+".avi");
               a.setAttribute("href", downloadURL);
               a.setAttribute("id","download_Video_file");
               a.style.visibility = 'hidden';
               document.body.appendChild(a);
               a.click();
            }*/
        },

        alignElements: function() {
            var canvas_pos = this.$canvas[0].getBoundingClientRect();
            //console.log("inside alignElements:");

            if( !/Android|webOS|iPhone|iPad|iPod|BlackBerry|IEMobile|Opera Mini/i.test(navigator.userAgent) ) {
                if(Math.abs(canvas_pos.top - 100) > 5) {
                    var top = parseInt(this.$canvas.css("top")) + (100 - canvas_pos.top);
                    this.$canvas.css({"top": top});
                }

                if(Math.abs(canvas_pos.left - 300) > 5) {
                    var left = parseInt(this.$canvas.css("left")) + (300 - canvas_pos.left);
                    this.$canvas.css({"left": left});
                }
            }
            
            this.$progress_info.css({"top": canvas_pos.top - 30, "left": canvas_pos.right - 410});
            this.$progress_div.css({"top": canvas_pos.top - 30, "left": canvas_pos.right - 260});
            this.$del_btn.css({"top": canvas_pos.top - 30, "left": canvas_pos.right - 80});
            this.$del_btn.show();
            this.$dl_btn.css({"top": canvas_pos.top - 30, "left": canvas_pos.right - 50});
            this.$dl_btn.show();
            this.$close_btn.css({"top": canvas_pos.top - 30, "left": canvas_pos.right - 20});
            this.$close_btn.show();
        },

        startProgressBar: function() {
            this.$progress_div.show();
            this.$progress_info.show();
            var that = this;
            var start = this.stream.position;
            this.status_bar_timer = setInterval( function() {
                that.$progress_bar.attr({"aria-valuenow": that.stream.position, "aria-valuemax": that.stream.byteLength});
                that.$progress_bar.css("width", Math.ceil(((that.stream.position - start) / (that.stream.byteLength - start)) * 100) + "%");
                if(that.video.playing==false){
                that.$progress_info.html("Processing Video (" + Math.ceil(((that.stream.position - start) / (that.stream.byteLength - start)) * 100) + "%)");
                } else {
                    that.$progress_info.html("Playing Video (" + Math.ceil(((that.stream.position - start) / (that.stream.byteLength - start)) * 100) + "%)");
                }
                if(that.stream.position == that.stream.byteLength) {
                    that.$progress_div.hide();
                    that.$progress_info.hide();
                    clearInterval(that.status_bar_timer);
                    that.status_bar_timer = null;
                }
            }, that.video.frame_delay);
        },

        removeView: function() {
            this.remove();
        },


        serialize: function() {
            return {
                locale: CommonStrings,
                id: this.model.get('id'),
                fromnow: moment(this.model.get('timestamp'), 'X').fromNow(),
                file: this.model.get('file'),
                event_type: (this.model.get('file').indexOf("video_dump") != -1) ? "Post Event" : "Pre Event",
                event_message: "Recorded on " + moment(this.model.get('timestamp'), 'X').format("dddd, MMMM Do YYYY, h:mm:ss a")
            };
        }

    });

    return view;

});
