define(['jquery', 'underscore', 'backbone', 'app',
//data
'models//fwimage_location',
//localize
'i18n!strings/nls/bios_update',
//template
'text!templates/maintenance/bios_upgrade.html',
//plugins
'iCheck', 'tooltip', 'alert'],

function($, _, Backbone, app, BiosLocationModel, locale, BiosUpgradeTemplate) {
	var tmpProgressVal = 0;//Quanta
    var view = Backbone.View.extend({

        template: _.template(BiosUpgradeTemplate),
        protocol_type:"http",
        verify_fw :0,
        PROTOCOL_HTTP:0,
        PROTOCOL_TFTP:1,
        initialize: function() {
            this.model = BiosLocationModel;
            this.httpsFlag = false;
            this.httpsInterval;
            this.model.bind('change:protocol_type', this.updateProtocolType, this);
            this.model.bind('change:server_address', this.updateServerAddress, this);
            this.model.bind('change:image_name', this.updateImageName, this);
            this.model.bind('change:retry_count', this.updateRetryCount, this);

            // this.model.bind('validated:valid', this.validData, this);
            // this.model.bind('validated:invalid', this.invalidData, this);

        },

        events: {
            "click #start": "preCheck",
        },

        beforeRender: function() {

        },

        afterRender: function() {


            this.model.fetch();
            var context = this;
            var _parent = this;
            _.each(this.model.attributes, function(value, attr, list) {
                _parent.model.trigger('change:' + attr, _parent.model, value);
            });

            /*var protocol_type=this.model.get("protocol_type");
            this.protocol_type=protocol_type;
            $('#protocol_type').show();
            $('#protocol_type').addClass("show");
            $('#protocol_type').html(this.model.get("protocol_type"));

            var server_address=this.model.get("server_address");
            $('#server_address').show();
            $('#server_address').addClass("show");
            $('#server_address').html(this.model.get("server_address"));

            var image_name=this.model.get("image_name");
            $('#image_name').show();
            $('#image_name').addClass("show");
            $('#image_name').html(this.model.get("image_name"));

            var retry_count=this.model.get("retry_count");
            $('#retry_count').show();
            $('#retry_count').addClass("show");
            $('#retry_count').html(this.model.get("retry_count"));*/


        },

        load: function(model, collection, xhr) {

        },

        reload: function(model, value) {

        },

	updateProtocolType: function(model, value) {
            this.protocol_type =value;
            $('#protocol_type').html(value);
            if(value == "tftp"){
                console.log("inside if" +value);
                $('#iddivtftp').show();
                $('#iddivhttp').hide();
            } else {
                $('#iddivtftp').hide();
                $('#iddivhttp').show();
            }
	},
        updateServerAddress: function(model, value) {
            $('#server_address').html(value);
        },
        updateImageName: function(model, value) {
            $('#image_name').html(value);
        },
        updateRetryCount: function(model, value) {
            $('#retry_count').html(value);
        },

        /*validData: function(model) {
        },

        errorFields: [],

        invalidData: function(model, errors) {
        },*/

        downloadstart: function() {
                $("#save-icon").removeClass().addClass("ion-loading-c");
                $("#start").addClass("disabled").html("Preparing to download...");
                window.fw_upgrade = true;
                app.blockAnchors();
                var that = this;

                $.ajax({
                    url: "/api/maintenance/biosflash",
                    type: "PUT",
                    dataType: "json",
                    data: {},
                    contentType: "application/json",
                    success: function(data, status, xhr) {
                    	that.downloadFWImage.call(that);
                    },
                    error : function(xhr, status, err) {
                        window.fw_upgrade = false;
                        //that.reset.call(that);
                        app.HTTPErrorHandler(xhr, status, err);
                    }
                });
        },

        downloadFWImage:function(){
        	var that=this;
        	 var data={};
        	 data.PROTO_TYPE=that.PROTOCOL_TFTP;

        	 var object= JSON.stringify(data);

        	 $.ajax({
                 url: "/api/maintenance/bios/dwldfwimg",
                 type: "PUT",
                 dataType: "json",
                 data: object,
                 contentType: "application/json",
                 success: function(data, status, xhr) {
 	                that.downloadFWStatus.call(that);
                 },
                 error : function(xhr, status, err) {
                     window.fw_upgrade = false;
                     //that.reset.call(that);
                     app.HTTPErrorHandler(xhr, status, err);
                 }
             });

        },

        downloadFWStatus:function(){
            var that = this;

            $.ajax({
                url: "/api/maintenance/bios/dwldfwstatus-progress",
                type: "GET",
                success: function(data, status, xhr) {
                    var progress = parseInt(data.progress, 10);
                    if(data.state >= 0x1 && data.state <= 0x5){
                    	that.downloadProgress.call(that,data.progress,data.action);
                    }
                },
                error : function(xhr, status, err) {
                    //that.reset.call(that);
                    app.HTTPErrorHandler(xhr, status, err);
                }
            });
        },
        preCheck: function(){
        	var that = this;
            $.ajax({
                type: "GET",
                url: "/api/chassis-status",
                success:  function (response, status, xhr) {
                	if(response.power_status == 1) {
                		alert("Please power off the server befor BIOS update");
                		return;
                	} else {
                		//Sytem is power off , continue to update.
                		that.start.call(that);
                	}
                },
            	error: app.HTTPErrorHandler
            });
        },
        start: function() {
        	var that=this;
        	if($('#filebios_image')[0].files[0])
        	{
        		var name =$('#filebios_image')[0].files[0].name;
                var ext = name.split('.').pop().toLowerCase();
            	if(ext != "bin") {
                    alert("Invalid Firmware image. Only BIN file is supported");
                    return;
                }
        	}
        	else
        	{
        		$("#filebios_image").notify("Please choose the bios file to upgrade", {"className": "error"});
        		return;
        	}
        	if(this.verify_fw == 0){
        		if(this.protocol_type == "tftp"){
        		 		if(!confirm("Attention! \nPlease power Off the system when you want to do BIOS update or you will fail.\n Do you want to continue?")) return;
        		 		that.downloadstart.call(that);
        		} else {
        			if(!$('#filebios_image')[0].files[0]){
                		$("#filebios_image").notify("Please choose the bios file to upgrade", {"className": "error"});
                		return;
                	}
        		}
        	}
            if($("#start").html() == locale.start_fw_update) {
                $("#save-icon").removeClass().addClass("ion-loading-c");

                //if(!confirm("Attention! \nPlease power Off the system when you want to do BIOS update or you will fail.\n Do you want to continue?")) return;

                $("#start").addClass("disabled").html("Preparing to flash...");
                window.fw_upgrade = true;
                //app.blockAnchors();
                $("#iddivhttp").addClass("disable_a_href");
		        $("#refreshPage").addClass("disable_a_href");
                var that = this;

                $.ajax({
                    url: "/api/maintenance/biosflash",
                    type: "PUT",
                    dataType: "json",
                    data: {},
                    contentType: "application/json",
                    success: function(data, status, xhr) {
                        that.upload.call(that);
                        that.httpsInterval = setInterval(function(){ that.upload.call(that); }, 90000);
                    },
                    error : function(xhr, status, err) {
                        window.fw_upgrade = false;
                        //that.reset.call(that);
                        app.HTTPErrorHandler(xhr, status, err);
                    }
                });
            }
            else if($("#start").html() == locale.cont_update) {
                this.section_names = [];
                this.section_count = 0;
                var that = this;
                $("#section_options input[name=section]").each(function()
                {
                    if($(this).prop("checked")) {
                        that.section_names.push($(this).val());
                        that.section_count += 1;
                    }
                });
                $("#section_dialog").hide();
                $("#start").addClass("disabled").html("Starting flash...");
                that.startUpgrade.call(this);
            }

        },

        upload: function() {
            var that = this;
            if(!that.httpsFlag){
                var data = new FormData();
                data.append('fwimage', $('#filebios_image')[0].files[0]);

                $("#start").html("Processing...");

                var that = this;

                $.ajax({
                    url: "/api/maintenance/bios",
                    type: "POST",
                    contentType: false,
                    processData: false,
                    dataType: "json",
                    cache: false,
                    data: data,
                    xhr: function() {
                        var cxhr = $.ajaxSettings.xhr();
                        if(cxhr.upload) {
                          console.log("Upload");
                            cxhr.upload.addEventListener('progress', that.uploadProgress, false);
                        }
                        return cxhr;
                    },
                    success: function(data, status, xhr) {
                            that.httpsFlag = true;
                            //$(".progress-info").html("Uploading "+100+"%");//Quanta--
                            $("#upload_progress").attr({"aria-valuenow": 100, "aria-valuemax": 100});//Quanta
                            $("#upload_progress").html("Uploading "+100+"%");//Quanta
                        that.verifyBios.call(that);
                    },
                    error: app.HTTPErrorHandler
                });
            }
        },

        uploadProgress: function(e) {
            if(e.lengthComputable) {
                $("#upload_progress").attr({"aria-valuenow": e.loaded, "aria-valuemax": e.total});
                $("#upload_progress").css("width", ((e.loaded/e.total)*50)+"%");
                var uploadedContent = ( (Math.ceil((e.loaded/e.total)*100)-1) > 99) ? 99 : Math.ceil((e.loaded/e.total)*100)-1;
                //$(".progress-info").html("Uploading "+uploadedContent+"%");//Quanta--
                $("#start").html("Uploading "+uploadedContent+"%");
            }
        },
        downloadProgress: function(progValue,action) {

            var that=this;
            var progress = parseInt(progValue, 10);
            $("#upload_progress").attr({"aria-valuenow": progress, "aria-valuemax": 100});
            $("#upload_progress").css("width", (progress/2) + "%");
            //$(".progress-info").html("Downloading "+" ("+progValue+")");//Quanta--
            $("#start").html("Downloading "+" ("+progValue+")");//Quanta
            if(progress<100) {
                that.downloadFWStatus.call(that);
            }
            else if(progress == 100 || isNaN(progress)) {
            	that.verifyBios.call(that);
            }
        },

	 verifyBios: function() {
            var that = this;
            clearInterval(that.httpsInterval);
            this.verify_fw=1;

            $("#start").html("Verifying...");
            //console.log("/api/maintenance/bios/verification");
            $.ajax({
                url: "/api/maintenance/bios/verification",
                type: "GET",
                dataType: "json",
                success: function(data, status, xhr) {
                     {
                        that.total_sections = 0;
                        var sections = "<table class=\"table\" id=\"section_table\">";
												sections += "<tr>";
												sections += "<td><center><input type=\"checkbox\" name=\"section\" value = \"bios\"></center></td>";
												sections += "<td>Preserve BIOS NVRAM Region</td>";
												sections += "</tr>";
												sections += "<tr>";
												/*
												sections += "<td><center><input type=\"checkbox\" name=\"section\" value = \"me\"></center></td>";
												sections += "<td>Preserve ME Region</td>";*/
												sections += "</tr>";
                        sections += "</table>";
                        $("#section_options").html(sections);

                        $("#section_table").find('input').iCheck({
                            checkboxClass: 'icheckbox_square-blue',
                            increaseArea: '20%'
                        });

                        $("#section_table").find('input').on('ifUnchecked', function(event) {
                            $("#select").iCheck("uncheck");
                        });

                        $('#select').on('ifClicked', function(event){
                            if($("#select").prop("checked")) {
                                $("#section_options input[name=section]").each(function()
                                {
                                    if(!$(this).is(":disabled")) {
                                        $(this).iCheck("uncheck");
                                    }
                                });
                            }
                            else {
                                $("#section_options input[name=section]").each(function()
                                {
                                    if(!$(this).is(":disabled")) {
                                        $(this).iCheck("check");
                                    }
                                });
                            }
                        });

                        $("#start").removeClass("disabled").html(locale.cont_update);
                        $("#upgrade_options").show();
                    }
                },
                error : function(xhr, status, err) {
                    //that.reset.call(that);
                    app.HTTPErrorHandler(xhr, status, err);
                }
            });
	},

	startUpgrade: function() {
		var that = this;
		var data = {};
		data.section_names = this.section_names;

		var object=JSON.stringify(data);
		console.log("/api/maintenance/bios/upgrade");
            $.ajax({
                url: "/api/maintenance/bios/upgrade",
                type: "PUT",
                dataType: "json",
                data: object,
                contentType: "application/json",
                success: function(data, status, xhr) {
                    that.flashProgress.call(that);
                },
                error : function(xhr, status, err) {
                    //that.reset.call(that);
                    app.HTTPErrorHandler(xhr, status, err);
                }
            });
	},

        flashProgress: function() {
            var that = this;
            var data = {};
            $.ajax({
                url: "/api/maintenance/bios/flash-progress",
                type: "GET",
                success: function(data, status, xhr) {
                	if(data.status== "undefined" || data.status==undefined){
                		that.flashProgress.call(that);
                	}
					else if(data.status<0)
					{
						app.HTTPErrorHandler(xhr, status, err);
					}
					else {
	                    var percentage = parseInt(data.percentage, 10);
	                    //$("#flash_progress").attr({"aria-valuenow": percentage, "aria-valuemax": 100});//Quanta--
	                    //$("#flash_progress").css("width", (percentage/2) + "%");
	                    //$("#flash_progress").html("flashing..."+"  "+data.percentage+"%");//Quanta--
	                    if(!isNaN(percentage)) {//Quanta
	                    	$("#start").html("Flashing..."+"  ("+percentage+"% done)");//Quanta
	                    	$("#flash_progress").attr({"aria-valuenow": percentage, "aria-valuemax": 100});//Quanta
	                    	$("#flash_progress").css("width", (percentage/2) + "%");//Quanta
	                    }
	                    if(data.status !="Complete")//((percentage<100) || isNaN(percentage))
                        {
	                        that.flashProgress.call(that);
	                    } else if(data.status=="Complete") {
	                        //that.reset.call(that);
                            $("#refreshPage").removeClass("disable_a_href");
                            $("#start").addClass("disabled").html("BIOS is Updated done");
                            $("#flash_progress").html("Flashing 100%");//Quanta
                            that.flashDone.call(that);
                            window.fw_upgrade = false;
                            console.log("Clear flag");
                            alert("BIOS Updated Complete");
	                    }
                	}
                },
                error : function(xhr, status, err) {
                    //that.reset.call(that);
                    app.HTTPErrorHandler(xhr, status, err);
                }
            });

        },
        flashDone: function() {
            var that = this;
            var data = {};
            $.ajax({
                url: "/api/maintenance/bios/flash-done",
                type: "PUT",
                success: function(data, status, xhr) {
                        console.log("/api/maintenance/bios/flash-done");
                },
                error : function(xhr, status, err) {
                    //that.reset.call(that);
                    app.HTTPErrorHandler(xhr, status, err);
                }
            });
        },
	reset: function() {
		var that = this;
        $("#start").html("Resetting...");
	    $.ajax({
                url: "/api/maintenance/reset",
                type: "POST",
                dataType: "json",
                data: {},
                contentType: "application/json",
                success: function(data, status, xhr) {
                    alert(locale.reset);
                    $("body").addClass("disable_a_href");
                    app.cancelConfirm();
                    window.fw_upgrade = false;
		            $("#refreshPage").removeClass("disable_a_href");
                    //setTimeout(function() {console.log("refreshing..."); location.reload;}, 2 * 60 * 1000);
                    setTimeout(function(){
                            app.logout();
                            document.cookie = "QSESSIONID=1;expires=Thu, 01 Jan 1970 00:00:00 GMT";
                            app.router.navigate("login");
                            location.reload();
                        },2 * 60 * 1000);
                },
                error : app.HTTPErrorHandler
            });

	},


        serialize: function() {
            return _.extend({
                locale: locale
            }, this.model.toJSON());
        }

    });

    return view;

});
