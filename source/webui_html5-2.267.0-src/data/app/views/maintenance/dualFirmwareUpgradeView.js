define(['jquery', 'underscore', 'backbone', 'app',
//data
'models//fwimage_location',
'models//preserve_configuration',
'models//dual_image_config',
//localize
'i18n!strings/nls/dual_firmware_update',
//template
'text!templates/maintenance/dual_fwimage_upgrade.html',
//plugins
'iCheck', 'tooltip', 'alert'],

function($, _, Backbone, app, FirmwareLocationModel, PreserveConfigurationModel,DualImageConfigModel, locale, FirmwareUpgradeTemplate) {
    
    var view = Backbone.View.extend({

        template: _.template(FirmwareUpgradeTemplate),
        protocol_type:"http",
        verify_fw :0,
        PROTOCOL_HTTP:0,
        PROTOCOL_TFTP:1,
        initialize: function() {
            this.model = FirmwareLocationModel;
            this.preserveconfig = PreserveConfigurationModel;
            this.httpsFlag = false;
            this.httpsInterval;
            this.model.bind('change:protocol_type', this.updateProtocolType, this);
            this.model.bind('change:server_address', this.updateServerAddress, this);
            this.model.bind('change:image_name', this.updateImageName, this);
            this.model.bind('change:retry_count', this.updateRetryCount, this);
            this.preserve_config = 0 ;
            this.dual_image_config = DualImageConfigModel;
            //this.dual_image_config.bind('change:current_active_image', this.updateActiveImage, this);
            this.dual_image_config.bind('add', this.addDualImageconfig, this);

            // this.model.bind('validated:valid', this.validData, this);
            // this.model.bind('validated:invalid', this.invalidData, this);

        },

        events: {
            "click #start": "start",
            "ifChecked #idpreserveAll": "checkedPreserveAll",
            "ifUnchecked #idpreserveAll": "uncheckedPreserveAll",
            "ifChecked #idreboot_bmc": "checkedRebootBmc",
            "ifUnchecked #idreboot_bmc": "uncheckedRebootBmc",
            "click .help-link": "toggleHelp"
        },

        beforeRender: function() {
            
        },

        afterRender: function() {

            this.$el.find('#idpreserveAll').iCheck({
                checkboxClass: 'icheckbox_square-blue',
                radioClass: 'iradio_square-blue',
                increaseArea: '20%'
            });

            this.model.fetch();
          
            var context = this;
            var _parent = this;
            _.each(this.model.attributes, function(value, attr, list) {
                _parent.model.trigger('change:' + attr, _parent.model, value);
            });
            context.dual_image_config.fetch({
                success: function(){
                    context.updateActiveImage(); }
             });

            context.preserveconfig.fetch({
                success: function(){
                    context.preserveconfig_update();
                }
            });
        },

        load: function(model, collection, xhr) {

        },

        reload: function(model, value) {

        },
        preserveconfig_update : function(){
            var t = this.preserveconfig.attributes;
            var i = 1;
            for(var val in t) {
                if(val != 'id'){
                    var value = (t[val] == 0) ? 'Overwrite' : 'Preserve';   
                    var config_name = val.toUpperCase();  
                    var odd_or_even = (i%2 != 0)? 'even' : 'odd';           
                    $(".table-body").append('<tr style="display: table-row;" class="footable-'+odd_or_even+'"><td><label class="presrv_config_item"> '+i+' </label></td><td><div class="config_name"> '+config_name+' </div></td><td><div class="config_status" >'+value+'</div></td></tr>');  
                    i++;
                }
            }
        },
        checkedPreserveAll : function(){
            $('#idpreserveAll').val(1);
            this.preserve_config = 1 ;
            if($(".table-list").hasClass("disable_a_href")){
                $(".table-list").removeClass("disable_a_href");
            }
            $(".table-list").addClass("disable_a_href");
        },
        uncheckedPreserveAll : function(){
            $('#idpreserveAll').val(0);
            this.preserve_config = 0 ;
            if($(".table-list").hasClass("disable_a_href")){
                $(".table-list").removeClass("disable_a_href");
            }
        },
        updateProtocolType: function(model, value) {
                this.protocol_type =value;
                if(value == "tftp"){
                	 $('#protocol_type').html(locale.tftp);
                    $('#iddivtftp').show();
                    $('#iddivhttp').hide();
                } else {
                    $('#protocol_type').html(locale.http);
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
        updateActiveImage: function() {
        	var value = this.dual_image_config.get("current_active_image");
        	console.log(this.dual_image_config);
        	console.log(this.dual_image_config.get("current_active_image"));
        	$('#idcurrent_active_image').html(value);
        },
        toggleHelp: function(e) {
            e.preventDefault();
            var help_items = this.$('.help-item').filter(function() {
                var f = $(this).data('feature');
                var nf = $(this).data('not-feature');
                if (f) {
                    return (app.features.indexOf(f) == -1 ? false : true);
                } else if (nf) {
                    return (app.features.indexOf(nf) == -1 ? true : false);
                } else {
                    return true;
                }
            });
            if (help_items.hasClass('hide')) {
                help_items.removeClass('hide');
            } else {
                help_items.addClass('hide');
            }
        },
        /*validData: function(model) {
        },

        errorFields: [],

        invalidData: function(model, errors) {
        },*/
        checkedRebootBmc: function(ev) {
            $('#idreboot_bmc').val(1);
        },
        uncheckedRebootBmc: function(ev) {
            $('#idreboot_bmc').val(0);
        },
        imageconfig: function() {
       	 var that = this;
            var data={};
           data.image_update= $('#idimage_update').val();
           data.reboot_bmc= $('#idreboot_bmc').val();
           var object= JSON.stringify(data);
           
           $.ajax({
               url: "/api/maintenance/flash_image_config",
               type: "PUT",
               dataType: "json",
               data: object,
               contentType: "application/json",
               success: function(data, status, xhr) {},
               error: function(data, status, xhr) {
                   window.fw_upgrade = false;
                   that.reset.call(that);
                   app.HTTPErrorHandler(xhr, status, err);
               }
           });
      },
        downloadstart: function() {     	
                $("#save-icon").removeClass().addClass("ion-loading-c");
                $("#start").addClass("disabled").html("Preparing to download...");
                window.fw_upgrade = true;
                app.blockAnchors();
                var that = this;

                $.ajax({
                    url: "/api/maintenance/flash",
                    type: "PUT",
                    dataType: "json",
                    data: {},
                    contentType: "application/json",
                    success: function(data, status, xhr) {
                    	that.downloadFWImage.call(that);
                    },
                    error : function(xhr, status, err) {
                        window.fw_upgrade = false;
                        that.reset.call(that);
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
                 url: "/api/maintenance/firmware/dwldfwimg",
                 type: "PUT",
                 dataType: "json",
                 data: object,
                 contentType: "application/json",
                 success: function(data, status, xhr) {                	
 	                that.downloadFWStatus.call(that);                    
                 },
                 error : function(xhr, status, err) {
                     window.fw_upgrade = false;
                     that.reset.call(that);
                     app.HTTPErrorHandler(xhr, status, err);
                 }
             });       	 
        	 
        },
        
        downloadFWStatus:function(){
            var that = this;

            $.ajax({
                url: "/api/maintenance/firmware/dwldfwstatus-progress",
                type: "GET",
                success: function(data, status, xhr) {
                    var progress = parseInt(data.progress, 10);
                    if(data.state >= 0x1 && data.state <= 0x5){
                    	that.downloadProgress.call(that,data.progress,data.action);
                    } 
                },
                error : function(xhr, status, err) {
                    that.reset.call(that);
                    app.HTTPErrorHandler(xhr, status, err);
                }
            });
        },
        
        start: function() {
        	var that=this;
        	
        	that.imageconfig.call(that);
        	
        	if(this.verify_fw == 0){
        		if(this.protocol_type == "tftp"){
        		 		if(!confirm("We will start the firmware upgrade now. You will not be able to access BMC until it flashes and restarts. Do you want to continue?")) return;
        		 		that.downloadstart.call(that);
        		} else {
        			if(!$('#filefirmware_image')[0].files[0]){
                		$("#filefirmware_image").notify("Please choose the firmware file to upgrade", {"className": "error"});
                		return; 
                	}
        		}
        	}
            if($("#start").html() == locale.start_fw_update) {
                $("#save-icon").removeClass().addClass("ion-loading-c");

                if(!confirm("We will start the firmware upgrade now. You will not be able to access BMC until it flashes and restarts. Do you want to continue?")) return;   
                $("#start").addClass("disabled").html("Preparing to flash...");
                window.fw_upgrade = true;
                app.blockAnchors();
                $("#iddivhttp").addClass("disable_a_href");
		        $("#refreshPage").addClass("disable_a_href");
                var that = this;

                $.ajax({
                    url: "/api/maintenance/flash",
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
                        that.reset.call(that);
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
                if(!this.section_count){
                    $("#section_options").notify("Please select at least 1 section to upgrade", {"className": "error"});
                    return;
                }

                if(this.section_count == this.total_sections) {
                    this.flash_status = 1;
                }
                else {
                    this.flash_status = 2;
                }
                $("#section_dialog").hide();
                $("#start").addClass("disabled").html("Starting flash...");
                that.startUpgrade.call(this);
            }

        },

        upload: function() { var that = this;
        if(!that.httpsFlag){
            var data = new FormData();
            data.append('fwimage', $('#filefirmware_image')[0].files[0]);

            $("#start").html("Processing...");

            //var that = this;

            $.ajax({
                url: "/api/maintenance/firmware",
                type: "POST",
                data: data,
                contentType: false,
                //contentType:"application/json; charset=utf-8",
                processData: false,
                dataType: "json",
                cache: false,
                xhr: function() {
                    var cxhr = $.ajaxSettings.xhr();
                    if(cxhr.upload) {
                        cxhr.upload.addEventListener('progress', that.uploadProgress, false);
                    }
                    return cxhr;
                },
                success: function(data, status, xhr) {
                    that.httpsFlag = true;
                    that.verifyFirmware.call(that);
                },
                error: app.HTTPErrorHandler
            });
        }
        },

        uploadProgress: function(e) {
            if(e.lengthComputable) {
                $("#upload_progress").attr({"aria-valuenow": e.loaded, "aria-valuemax": e.total});
                $("#upload_progress").css("width", (e.loaded/e.total)*50+"%");
                var uploadedContent = (Math.ceil((e.loaded/e.total)*100) > 100) ? 100 : Math.ceil((e.loaded/e.total)*100);
                $(".progress-info").html("Uploading "+uploadedContent+"%");
            }
        },
        downloadProgress: function(progValue,action) {

            var that=this;   	
            var progress = parseInt(progValue, 10);
            $("#upload_progress").attr({"aria-valuenow": progress, "aria-valuemax": 100});
            $("#upload_progress").css("width", (progress/2) + "%");
            $(".progress-info").html("Downloading "+" ("+progValue+")");

            if(progress<100) {
                that.downloadFWStatus.call(that);
            } 
            else if(progress == 100 || isNaN(progress)) { 
            	that.verifyFirmware.call(that);
            }
        },
        
	 verifyFirmware: function() {		 
            var that = this;
            clearInterval(that.httpsInterval);
            this.verify_fw=1;
            
            $("#start").html("Verifying...");

            $.ajax({
                url: "/api/maintenance/firmware/verification",
                type: "GET",
                dataType: "json",
                success: function(data, status, xhr) {
                    if(!app.configurations.isFeatureAvailable("SECTIONFLASH")) {
                        that.flash_status = 1;
                        that.startUpgrade.call(that);
                    }
                    else {
                        that.total_sections = 0;
                        var sections = "<table class=\"table\" id=\"section_table\">";
                        sections += "<thead><tr><th>" + locale.section_name + "</th><th>Existing version</th><th>Uploaded version</th><th><center><input type=\"checkbox\" id=\"select\" checked><center></th></tr></thead>";
                        for(i = 0; i < data.length; i++) {
                            sections += "<tr>";
                            sections += "<td>" + data[i].current_image_name + "</td>";
                            sections += "<td>" + data[i].current_image_version1 + "</td>";
                            sections += "<td>" + data[i].new_image_version + "</td>";
                            if(0x10 & data[0].verification_status)
                            	{
                            	sections += "<td><center><input type=\"checkbox\" name=\"section\" value=\"" + data[i].current_image_name + "\" checked disabled></center></td>";
                            	that.total_sections += 1;
                            	}
                            else if(data[i].section_status) {
                                sections += "<td><center><input type=\"checkbox\" name=\"section\" value=\"" + data[i].current_image_name + "\" disabled></center></td>";
                            }
                            else {
                                sections += "<td><center><input type=\"checkbox\" name=\"section\" value=\"" + data[i].current_image_name + "\" checked></center></td>";
                                that.total_sections += 1;
                            }
                            sections += "</tr>";
                        }
                        sections += "</table>";
                        $("#section_options").html(sections);

                        $("#section_table").find('input').iCheck({
                            checkboxClass: 'icheckbox_square-blue',
                            increaseArea: '20%'
                        });

                        $("#section_table").find('input[name=section]').on('ifUnchecked', function(event) {
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
                    that.reset.call(that);
                    app.HTTPErrorHandler(xhr, status, err);
                }
            });
	},

	startUpgrade: function() {
		var that = this;
		var data = {};
      		data.preserve_config = this.preserve_config;
		data.flash_status=this.flash_status;

        if(data.flash_status == 2) {
            data.section_count = this.section_count;
            data.section_names = this.section_names;
        }

		var object=JSON.stringify(data);
		
            $.ajax({
                url: "/api/maintenance/firmware/upgrade",
                type: "PUT",
                dataType: "json",
                data: object,
                contentType: "application/json",
                success: function(data, status, xhr) {
                    that.flashProgress.call(that);
                },
                error : function(xhr, status, err) {
                    that.reset.call(that);
                    app.HTTPErrorHandler(xhr, status, err);
                }
            });
	},

        flashProgress: function() {
            var that = this;
            var data = {};

            $.ajax({
                url: "/api/maintenance/firmware/flash-progress",
                type: "GET",
                success: function(data, status, xhr) {
                	if(data.state== "undefined" || data.state==undefined){
                		that.flashProgress.call(that);
                	} else {
	                    var progress = parseInt(data.progress, 10);
	                    $("#flash_progress").attr({"aria-valuenow": progress, "aria-valuemax": 100});
	                    $("#flash_progress").css("width", (progress/2) + "%");
	                    $(".progress-info").html(data.action+" ("+data.progress+")");
	                    if(progress<100) {
	                        that.flashProgress.call(that);
	                    } else if(progress == 100 || isNaN(progress)) {
	                        that.reset.call(that);
	                    }
                	}
                },
                error : function(xhr, status, err) {
                    that.reset.call(that);
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
