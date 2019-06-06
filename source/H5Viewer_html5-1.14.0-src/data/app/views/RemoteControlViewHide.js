define(['jquery', 'underscore', 'backbone', 'app',
        //data
        'models/kvm_token',
        //localize
        'i18n!strings/nls/remote_control',
        //template
        'text!templates/remote.html',
        //plugins
        // 'moment',
        // 'knob',
        // 'sparkline'
        //libs
        'video',
        'cdrom'
    ],

    function($, _, Backbone, app, KVMTokenModel, CommonStrings, RemoteTemplate, Video, CDROM) {

        var view = Backbone.View.extend({

            template: _.template(RemoteTemplate),

            initialize: function() {
                this.model = new KVMTokenModel();
            },

            events: {
                "click #toggle": "startKVM",
                "click #record": "recordVideo",
                "click #stop": "stopRecording",
                "click #css": "animateVideo",
                "click #start": "startMedia",
                "change #files": "chooseISO"
            },

            beforeRender: function() {

            },

            afterRender: function() {
                console.log(Video);
                console.log(CDROM);
                this.$el.find("#kbd").hide();
            },

            startKVM: function(ev) {
                var kvm_el = $(ev.currentTarget);

                this.model.fetch({
                    success: function(data) {

                        window.KVM_TOKEN = data.token;

                        //Why? Because, document.cookie will not be set for SESSION_COOKIE when customer prefers HTTP-only mode. 
                        //So get the session during KVM token call for now. Ideally, this data must be removed - Chandru
                        window.SESSION_COOKIE = data.session;

                        if ($(kvm_el).html() == "Start KVM") {
                            $(document).click(function() {
                                $("#kvm_textbox").focus();
                            });
                            $(kvm_el).html("Stop KVM");
                            $("#css").show();
                            $("#kvm").addClass("alive");
                            window._video = new Video("kvm", location.host);
                        } else {
                            window._video.sock.onclose = function() {
                                $(kvm_el).html("Start KVM");
                            };

                            window._video.stop();
                            $("#kvm").removeClass("alive");
                        }
                    }
                });

                
            },

            recordVideo: function(ev) {
                var rec_el = $(ev.currentTarget);
                if (!window._video.running) {
                    return;
                }
                $("#stop").removeAttr("disabled");
                $(rec_el).attr("disabled", "disabled");
                window.builder = new movbuilder.MotionJPEGBuilder();
                window.builder.setup($("#kvm").width(),
                    $("#kvm").height(),
                    30);
                window.recordVideo = true;
                //todo: change it to request animation frame
                window.recordVideoTimer = setInterval(function() {
                    if (window.recordVideo) {
                        window.builder.addCanvasFrame(window._video.canvas);
                    }
                }, 33);

            },

            stopRecording: function() {
                clearInterval(window.recordVideoTimer);
                $("#record").removeAttr("disabled");
                $("#stop").attr("disabled", "disabled");
                window.builder.finish(function(generatedURL) {
                    open(generatedURL);
                });
                window.recordVideo = false;
            },

            startMedia: function() {
                if (window.File && window.FileReader && window.FileList && window.Blob) {
                    // Great success! All the File APIs are supported.
                } else {
                    alert(CommonStrings.alert_apinotsupported);
                }

                if (!window.cdrom) {
                    if (!file) {
                        alert(CommonStrings.select_file);
                    }


                    window.cdrom = new CDROM();
                    cdrom.startRedirection(location.host, 'file', window.KVM_TOKEN, 9999, false);
                    $("#media").html(CommonStrings.stop_media_redirection);

                    window.onmediaclose = function() {
                        window.cdrom = null;
                        $("#media").html(CommonStrings.start_media_redirection);
                    };
                } else {
                    window.cdrom.sock.close();
                }
            },

            chooseISO: function(ev) {
                var files = ev.target.files;

                if (files.length > 1) {
                    alert(CommonStrings.iso_image_redirection);
                    return;
                }

                window.file = files[0];

                if (!file) {
                    //console.log("File selection not valid");
                    return;
                }

                //console.log("size", file.size);

                window.reader = new FileReader();

                window.reader.onloadend = function(e) {
                    if (e.target.readyState == FileReader.DONE) {
                        ////console.log(new Uint8Array(e.target.result));
                        $("#log").innerHTML += ("<br/>read bytes at " + (new Date()));
                    }
                };

                var blob = file.slice(0, 500);
                window.reader.readAsArrayBuffer(blob);
            },

            animateVideo: function() {},

            load: function(model, collection, xhr) {

            },

            serialize: function() {
                return {
                    locale: CommonStrings
                };
            }

        });

        return view;

    });