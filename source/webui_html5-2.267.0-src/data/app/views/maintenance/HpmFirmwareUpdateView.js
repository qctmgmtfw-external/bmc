define(['jquery', 'underscore', 'backbone', 'app',
//data
'models//fwimage_location',
'models//preserve_configuration',
//localize
'i18n!strings/nls/hpm_firmware_update',
//template
'text!templates/maintenance/hpm_image_upgrade.html',
//plugins
'iCheck', 'tooltip', 'alert'],


/* ------------------ TODO: -------------------


    Add more robust error handling and improve
    information shown by UI

    Solve MMC+BOOT+APP on one cycle issue

    Test functionality with BIOS upgrades

    possible improvements: 
    - do some uploads/flashing in parallel?
    - change HPM component info to array of objects,
        (rather than many arrays), would allow ordering
        logic to be more intuitive

*/

function($, _, Backbone, app, FirmwareLocationModel, PreserveConfigurationModel, locale, HpmImageUpgradeTemplate) {

    var view = Backbone.View.extend({

        template: _.template(HpmImageUpgradeTemplate),

        HPM_HEADER: [],
        HPM_COMPONENT_DATA_NAME: [],
        HPM_COMPONENT_DATA_VERSION: [],
        HPM_COMPONENT_DATA_START_END : [],
        HPM_COMPONENT_HPM_ID: [], //id from HPM spec
        HPM_COMPONENT_VERS_ID: [], //id used by component manager
        HPM_COMPONENT_CID: [],
        HPM_SECTION_FLASH: [],
        HPM_FLASH_ORDER: [-1,-1,-1,-1],
        headerStartOffBit:0,
        headerEndOffBit:34,
        TOTAL_HPM_SIZE:0,
        NoOfComponents:0,
        sbit:0,
        oemlength:0,
        ebit:0,
        HPM_FILE_OBJ:null,  
        HPM_COMPONENT_COUNT:0,

        HPM_BOOT_COMP: 1,// as per HPM spec
        HPM_APP_COMP: 2,// as per HPM spec
        HPM_BIOS_COMP: 4,// as per HPM spec
        HPM_MMC_COMP: 8,// as per HPM spec

        HPM_BIOS_CID: 2,// as used by component manager
        HPM_MMC_CID: 1,// as used by component manager

        HPM_BOOT_VID: 0,// as used by IPMI version check
        HPM_APP_VID: 1,// as used by IPMI version check
        HPM_BIOS_VID: 2,// as used by IPMI version check
        HPM_MMC_VID: 3,// as used by IPMI version check
        // create preferred flashing order, currently set to BIOS > MMC > BOOT > APP
        BIOS_ORDER: 0,
        MMC_ORDER: 1,
        BOOT_ORDER: 2,
        APP_ORDER: 3,
        HPM_UPLOAD_SIZE:10240,// Total length of conversion Hex format data and comes size as 20KB.
        needReset: false,
        bootAppSeenOnce: false,
        flashAreaPrepared: false,
        HPM_FLASH_QUEUE: [],

        initialize: function() {
            this.model = FirmwareLocationModel;
            this.preserveconfig = PreserveConfigurationModel;
            // this.model.bind('change:protocol_type', this.updateProtocolType, this);
            // this.model.bind('change:server_address', this.updateServerAddress, this);
            // this.model.bind('change:image_name', this.updateImageName, this);
            // this.model.bind('change:retry_count', this.updateRetryCount, this);

            // this.model.bind('validated:valid', this.validData, this);
            // this.model.bind('validated:invalid', this.invalidData, this);

        },

        events: {
            "click #start": "start",
            "click #proceed": "proceedHPMFlash",
            "click #cancelFlashing": "cancelHPMWizard",
			"ifChecked #chkFullFlash": "checkedCheckall",
            "ifUnchecked #chkFullFlash": "uncheckedCheckall",
            "click .help-link": "toggleHelp"
        },

        beforeRender: function() {
            this.support = {
                sectionflash : app.configurations.isFeatureAvailable("SECTIONFLASH"),
                version_cmp_flash : app.configurations.isFeatureAvailable("VERSION_CMP_FLASH"),
                fw_image : app.configurations.isFeatureAvailable("FW_IMAGE"),
                dual_image : app.configurations.isFeatureAvailable("DUAL_IMAGE")
            }

        },

        afterRender: function() {

            $('#chkVersionFlash, #chkFullFlash').iCheck({
                checkboxClass: 'icheckbox_square-blue',
                radioClass: 'iradio_square-blue',
                increaseArea: '20%'
            });

            // version check flash doesn't do anything for now
            $('#chkVersionFlash').iCheck('disable');

            this.model.fetch();
            var context = this;
            var _parent = this;
            _.each(this.model.attributes, function(value, attr, list) {
                _parent.model.trigger('change:' + attr, this.model, value);
            });
			
            context.preserveconfig.fetch({
                success: function(){
                    context.preserveconfig_update();
                }
            });

			//making default values to arrays.
			 this.HPM_HEADER= [];
			 this.HPM_COMPONENT_DATA_NAME= [];
			 this.HPM_COMPONENT_DATA_VERSION= [];
			 this.HPM_COMPONENT_DATA_START_END = [];
			 this.HPM_COMPONENT_HPM_ID= []; //id from HPM spec
			 this.HPM_COMPONENT_VERS_ID= []; //id used by component manager
			 this.HPM_COMPONENT_CID= [];
			 this.HPM_SECTION_FLASH= [];
			 this.HPM_FLASH_ORDER= [-1,-1,-1,-1];
			 this.HPM_FLASH_QUEUE=[];
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
                    var data_feature = (config_name == 'SNMP')? 'data-feature="SNMP"':'';
                    $(".table-body").append('<tr style="display: table-row;" '+data_feature+' class="footable-'+odd_or_even+'"><td><label class="presrv_config_item"> '+i+' </label></td><td><div class="config_name" '+data_feature+ '> '+config_name+' </div></td><td><div class="config_status" >'+value+'</div></td></tr>');  
                    i++;
                }
            }
            var selector = '.table-body';
            app.featureFilter(selector);
        },
/*
        updateProtocolType: function(model, value) {
            $('#idprotocol_type').val(value);
        },
        updateServerAddress: function(model, value) {
            $('#idserver_address').val(value);
        },
        updateImageName: function(model, value) {
            $('#idimage_name').val(value);
        },
        updateRetryCount: function(model, value) {
            $('#idretry_count').val(value);
        },

        validData: function(model) {
        },

        errorFields: [],

        invalidData: function(model, errors) {
        },
*/
		checkedCheckall: function(ev) {            
			$('.cmpnt:checkbox').iCheck('check');
        },
        uncheckedCheckall: function(ev) {            
			$('.cmpnt:checkbox').iCheck('uncheck');
        },
        start: function() {

            var filepath;
            var fileExtension;
            var file;
            
            $("#save-icon").removeClass().addClass("ion-loading-c");
            if($('#filefirmware_image')[0].files[0]) {
                filepath = $('#filefirmware_image')[0].files[0].name;
                fileExtension;
                if(filepath){
                    fileExtension=filepath.split(".").pop(); 
                    if(fileExtension != "hpm"){
                        $("#filefirmware_image").notify("Please choose the .hpm extension firmware file to upgrade", {"className": "error"});
                        return;
                    }
                }
                var file= $('#filefirmware_image')[0].files[0];
                if(file.size==0){
                    $("#filefirmware_image").notify("Please choose the valid hpm firmware file to upgrade", {"className": "error"});
                    return;
                }
            }  else {
                $("#filefirmware_image").notify("Please choose the firmware file to upgrade", {"className": "error"});
                return;
            }
	    $("#refreshPage").addClass("disable_a_href");

            HPM_HEADER= new Array();
            HPM_COMPONENT_DATA_NAME= new Array();
            HPM_COMPONENT_DATA_VERSION= new Array();
            HPM_COMPONENT_DATA_START_END= new Array();
            HPM_COMPONENT_HPM_ID= new Array();
            HPM_COMPONENT_VERS_ID= new Array();
            HPM_COMPONENT_CID= new Array();
            HPM_SECTION_FLASH=new Array();
            
            this.readHPMHeaderBlob.call(this,this.headerStartOffBit,this.headerEndOffBit,1,0);

            /*console.log(this.HPM_HEADER);
            console.log(this.HPM_COMPONENT_DATA_NAME);
            console.log(this.HPM_COMPONENT_DATA_VERSION);
            console.log(this.HPM_COMPONENT_DATA_START_END);
            console.log(this.HPM_COMPONENT_HPM_ID);
            console.log(this.HPM_COMPONENT_VERS_ID);*/

            /*if(!confirm("We will start the firmware upgrade now. You will not be able to access BMC until it flashes and restarts. Do you want to continue?")) return;

            var that = this;

            $.ajax({
                url: "/api/maintenance/flash",
                type: "PUT",
                dataType: "json",
                data: {},
                contentType: "application/json",
                success: function(data, status, xhr) {
                    that.upload.call(that);
                },
                error : app.HTTPErrorHandler
            });*/

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

        readHPMHeaderBlob: function(opt_startByte, opt_stopByte,type,i) {

            var that = this;

            var result;
            if (window.File && window.FileReader && window.FileList && window.Blob) {
            } else {
                alert('The File APIs are not fully supported in this browser.');
            }
            
            var files = $('#filefirmware_image')[0].files;
            this.HPM_FILE_OBJ=$('#filefirmware_image')[0].files;
            var file = files[0];
            
            var start = parseInt(opt_startByte) || 0;
            var stop = parseInt(opt_stopByte) || file.size - 1;
            
            this.TOTAL_HPM_SIZE=file.size - 16;
            
            var reader= new FileReader();
            reader.onloadend = function (evt) {
                if (evt.target.readyState == FileReader.DONE) { // DONE == 2
                    result = that.base64ToHex(that._arrayBufferToBase64(evt.target.result));
                    
                    that.HPM_HEADER.push(result);
                    
                    var totalSize=result.length;
                    
                    that.NoOfComponents=result.substring(40,42);// components
                    
                    var totalOEMLength=result.substring(64,68);//OEM Data Length
                    
                    that.sbit = 34 + parseInt(totalOEMLength,16) + 1;
                    
                    that.ebit=that.sbit+3;
                    
                    that.readHPMUpgradeAction.call(that,that.sbit,that.ebit,2,0);
                }
            };

            var blob = file.slice(start, stop);   
            reader.readAsArrayBuffer(blob);
        },

        readHPMUpgradeAction: function(opt_startByte, opt_stopByte,type,i) {

            var that = this;
            var result;
            var files = this.HPM_FILE_OBJ;
            var file = files[0];
            var start =parseInt(opt_startByte) || 0;
            var stop = parseInt(opt_stopByte) || file.size - 1;
            
            if(!(this.TOTAL_HPM_SIZE > this.ebit)) {
                

                $('#_parsingImage').removeClass("hide");
                $('#_parsingImage').css("color", "#000");
                $('#_parsingImage:first-child').attr("src", "../res/prg_success.png");
                
                $('#_prepareDevice').removeClass("hide");
                $('#_prepareDevice').css("color", "#000");

                if(this.HPM_COMPONENT_HPM_ID.length > 2){ // ot three components.
                    
                    $('#_ulUploadHPM').removeClass("hide");
                    $('#_ulFlashHPM').removeClass("hide");
                    
                    $('#_ulUploadHPM').addClass("wizard");
                    $('#_ulFlashHPM').addClass("wizard");
                    $('#_verifyImage').addClass("hide");
                    
                    //console.log(HPM_COMPONENT_HPM_ID);
                    
                    for(k=0;k<this.HPM_COMPONENT_HPM_ID.length;k++){
                        if(parseInt(this.HPM_COMPONENT_HPM_ID[k])==this.HPM_BIOS_COMP){
                            
                            $('#_uploadBIOSImage').removeClass("hide");
                            $('#_flashBIOSImage').removeClass("hide");
                            
                        } else if((parseInt(this.HPM_COMPONENT_HPM_ID[k])==this.HPM_BOOT_COMP) ||
                                (parseInt(this.HPM_COMPONENT_HPM_ID[k])==this.HPM_APP_COMP)){
                             
                            $('#_uploadBOOTAPPImage').removeClass("hide");
                            $('#_flashBOOTAPPImage').removeClass("hide");
                            
                            $('#_uploadBOOTImage').removeClass("hide");
                            $('#_flashBOOTImage').removeClass("hide");
        
                        }/*else if(parseInt(HPM_COMPONENT_HPM_ID[k])==HPM_APP_COMP){
                            
                            uploadAPPImage.className="visibleRow";
                            flashAPPImage.className="visibleRow";
                        }*/
                    }
                }
                
                //clearSection();
                this.verifyHPMFlashImage.call(this);
            }
            
            var reader = new FileReader(); 

            reader.onloadend = function (evt) {
                if (evt.target.readyState == FileReader.DONE) { // DONE == 2
                     
                    //result=binaryToHex(ABC.toBinary(evt.target.result, 0)).result;
                    result = that.base64ToHex.call(that,that._arrayBufferToBase64.call(that,evt.target.result));
                    
                    that.HPM_HEADER.push(result);
                    
                    var Size=result.length;
                    
                    var startUpgrade=start * 2;
                    
                    var endUpgrade=startUpgrade + 2;
                    
                    var upgradeActionType=result.substring(0,2);// components
                    
                    startUpgrade=endUpgrade;
                    endUpgrade=startUpgrade + 2;
                    
                    var componentId=parseInt(result.substring(2,4));
                    
                    if(upgradeActionType==0x00 || upgradeActionType==0x01) { //Backup components,Prepare components
                        that.sbit=that.ebit;
                        that.ebit=that.sbit+3;
                        that.readHPMUpgradeAction.call(that,that.sbit,that.ebit,3,0);
                    } else if(upgradeActionType==0x02) { //Upload image
                        that.sbit=that.ebit;
                        that.ebit=that.sbit + 31+16; //16 bytes added for OEM Header
                        that.HPM_COMPONENT_HPM_ID.push(componentId);
                        that.doComponentIdVersions.call(that,componentId);
                        that.readHPMUpgradeComponentData.call(that,that.sbit,that.ebit,4,0,componentId);
                    }
                 }
            };
            var blob = file.slice(start, stop);
            //reader.readAsBinaryString(blob);
            reader.readAsArrayBuffer(blob);

        },

        doComponentIdVersions: function(cid) {
            //var returnCId;
            switch(cid){
                case this.HPM_BOOT_COMP:
                    this.HPM_COMPONENT_VERS_ID.push(this.HPM_BOOT_VID);
                    this.HPM_COMPONENT_CID.push(-1);
                    break;
                case this.HPM_APP_COMP: 
                    this.HPM_COMPONENT_VERS_ID.push(this.HPM_APP_VID);
                    this.HPM_COMPONENT_CID.push(-1);
                    break;
                case this.HPM_BIOS_COMP:
                    this.HPM_COMPONENT_VERS_ID.push(this.HPM_BIOS_VID);
                    this.HPM_COMPONENT_CID.push(this.HPM_BIOS_CID);
                    break;
                case this.HPM_MMC_COMP:
                    this.HPM_COMPONENT_VERS_ID.push(this.HPM_MMC_VID);
                    this.HPM_COMPONENT_CID.push(this.HPM_MMC_CID);
                    break;
            }
            //return returnCId;
        },

        verifyHPMFlashImage: function () {
            console.log(this.HPM_HEADER);
            console.log(this.HPM_COMPONENT_DATA_NAME);
            console.log(this.HPM_COMPONENT_DATA_VERSION);
            console.log(this.HPM_COMPONENT_DATA_START_END);
            console.log(this.HPM_COMPONENT_HPM_ID);
            console.log(this.HPM_COMPONENT_VERS_ID);
            console.log(this.HPM_COMPONENT_CID);

            // disable start button
            $("#start").addClass("disabled").html(locale.prep_flash);

            $('#wizardSectionFlash').removeClass('hide');
            $('#verifyHPMFlashImage').removeClass('hide');

            this.loadSectionCfgHPM();

            // #proceed button in this element is already bound to proceedHPMFlash()
            // #cancelFlashing button in this element is already bound to cancelHPMWizard()

            $('#chkVersionFlash, #chkFullFlash').iCheck({
                checkboxClass: 'icheckbox_square-blue',
                radioClass: 'iradio_square-blue',
                increaseArea: '20%'
            }).removeClass("hide");

            // version check flash doesn't do anything for now
            $('#chkVersionFlash').iCheck('disable');
        },

        createButton: function(name, value, fnPtr) {

            var btn = document.createElement("input");
            btn.type = "button";
            btn.id = name;
            btn.name = name;
            btn.value = value;
            btn.onclick= fnPtr;
            return btn;
        },

        proceedHPMFlash: function() {
            // if update all is selected, check all other boxes
            /*if($('#chkFullFlash').prop('checked') == true) {
                $('.cmpnt:checkbox').iCheck('check');
            } else{
				 $('.cmpnt:checkbox').iCheck('uncheck');
			}*/

            // DISALLOW MMC AND BOOT+APP ON SAME CYCLE FOR NOW
            var bootbox = $(".cmpnt:checkbox[data-vid="+this.HPM_BOOT_VID+"]");
            var appbox = $(".cmpnt:checkbox[data-vid="+this.HPM_APP_VID+"]");
            var mmcbox = $(".cmpnt:checkbox[data-vid="+this.HPM_MMC_VID+"]");

           /*if(mmcbox.is(':checked') && (appbox.is(':checked') || bootbox.is(':checked'))) {
                alert(locale.either_bmc_mmc);
                return;
            }*/

            // if there are components selected, start the upgrade process
            if($('.cmpnt:checkbox').is(':checked')) {

                if(!confirm(locale.update_confirm)) {
                    return;
                }
                // disable component selection UI and enable update progress UI
                $('#cancelFlashing').attr('disabled','disabled');
                $('#proceed').attr('disabled','disabled');
                $('.cmpnt:checkbox').addClass('disabled');
                $('#cmpnt_options tr th:nth-child(6), td:nth-child(6)').show();
                $('.cmpnt:checkbox:checked').closest('tr').find('td:nth-child(6)').html(locale.up_queue);
                // create the upgrade queue based on selected components
                this.HPM_FLASH_QUEUE = $('.cmpnt:checkbox:checked').map( function() { return parseInt(this.dataset.order); } ).get();
                this.bootappseenonce = false;
                // start the upgrade!
                this.prepareDeviceShutdown();
                if(!this.proceedNextComponent.call(this)) {
                    alert(locale.no_upgrade_done);
                    $('#cancelFlashing').removeAttr('disabled');
                    $('#proceed').removeAttr('disabled');
                }

            } else {
                // why didnt you select anything??
                alert(locale.none_checked);
            }
        },

        proceedNextComponent: function() {
            var timeout = 0;
            while(timeout < 1000) {
                // try to upgrade something, return 0 if the flash order queue is empty
                if(!this.HPM_FLASH_QUEUE.length) {
                    return 0;
                } 
                if(this.proceedHPMComponents.call(this)){
                    $("#upload_progress").attr({"aria-valuenow": 0, "aria-valuemax": 100});
                    $("#flash_progress").attr({"aria-valuenow": 0, "aria-valuemax": 100});
                    $(".progress-info").html("");
                    return 1;
                }
            }
            // if we timeout, return 0
            return 0;
        },

        proceedHPMComponents: function() {
            // traverse the flash queue and do an upgrade
            /* return values:
                    0 : no upgrade started
                    1 : an upgrade was started
            */

            var next, vid;
            // if there are components left to be upgraded
            if(this.HPM_FLASH_QUEUE.length) {
                next = this.HPM_FLASH_QUEUE.shift();
                // if the next component in line is actually available
                if(next > -1) {
                    // check for BOOT+APP components (must be upgraded together)
                    if((next==this.BOOT_ORDER) || (next==this.APP_ORDER)) {
                        if(this.bootAppSeenOnce) {
                            // if yes, start upgrading the component!
                            this.startApp.call(this);

                            console.log("Starting rom.ima flash! : " + this.HPM_COMPONENT_DATA_NAME[this.HPM_FLASH_ORDER[next]]);
                            return 1;
                        } else {
                            // ignore this call the first time BOOT or APP components are seen (we will probably see the other component on next call)
                            // make a note when we see a BOOT or APP component,
                            // but, make sure both BOOT+APP are queued
                            var bootPresent = (next == this.BOOT_ORDER) || (this.HPM_FLASH_QUEUE.indexOf(this.BOOT_ORDER) > -1);
                            var appPresent = (next == this.APP_ORDER) || (this.HPM_FLASH_QUEUE.indexOf(this.APP_ORDER) > -1);
                            
                            console.log(this.HPM_COMPONENT_DATA_NAME[this.HPM_FLASH_ORDER[next]] + " spotted, rom.ima will flash when other boot/app component is seen.");
                            if(bootPresent && appPresent) {
                                this.bootAppSeenOnce = true;
                            }    
                            return 0;
                        }

                    // else, do normal HPM component upgrades
                    } else {
                        // pass along which component we are currently upgrading
                        var component_index = this.HPM_FLASH_ORDER[next];
                        this.prepareHPMFlash.call(this, component_index);

                        console.log("Starting update for component: " + this.HPM_COMPONENT_DATA_NAME[component_index]);
                        return 1;
                    }
                } else {
                    return 0;
                }
            } else {
                return 0;
            }
        },

        cancelHPMWizard: function() {

            // remove component table
            this.unloadSectionCfgHPM.call(this);

            // hide verification elements
            $('#wizardSectionFlash').addClass('hide');
            $('#verifyHPMFlashImage').addClass('hide');

            // clear component arrays

            this.HPM_HEADER = [];
            this.HPM_COMPONENT_DATA_NAME = [];
            this.HPM_COMPONENT_DATA_VERSION = [];
            this.HPM_COMPONENT_DATA_START_END = [];
            this.HPM_COMPONENT_HPM_ID = [];
            this.HPM_COMPONENT_VERS_ID = [];
            this.HPM_COMPONENT_CID = [];
            this.HPM_SECTION_FLASH = [];
            this.HPM_FLASH_ORDER = [-1,-1,-1,-1]; // mapping of preferred flashing order to component order within .hpm file
            this.HPM_FLASH_QUEUE = [];

            // hide any progress indicators
            $('#_parsingImage').addClass("hide");
            $('#_prepareDevice').addClass("hide");
            $('#_ulUploadHPM').addClass("hide");
            $('#_ulFlashHPM').addClass("hide");
            $('#_uploadBIOSImage').addClass("hide");
            $('#_flashBIOSImage').addClass("hide");
            $('#_uploadBOOTAPPImage').addClass("hide");
            $('#_flashBOOTAPPImage').addClass("hide");
            $('#_uploadBOOTImage').addClass("hide");
            $('#_flashBOOTImage').addClass("hide");

            // reinstate start button
            $('#start').html(locale.start_fw_update);
            $('#start').removeClass("disabled");
			
			
			$('#filefirmware_image').removeAttr("disabled");
			$("#refreshPage").removeClass("disable_a_href");
            // let user leave page
            window.fw_upgrade = false;
        },

        prepareDeviceShutdown: function() {
            window.fw_upgrade = true;
            app.blockAnchors();
        },

        startApp: function() {

            $(".cmpnt:checkbox:checked[data-order="+this.BOOT_ORDER+"]").closest('tr').find('td:nth-child(6)').html(locale.up_ongoing);
            $(".cmpnt:checkbox:checked[data-order="+this.APP_ORDER+"]").closest('tr').find('td:nth-child(6)').html(locale.up_ongoing);

            var that = this;
        
            if(this.flashAreaPrepared) {
                console.log("Flash preparation already done, starting upload.");
                that.doBootPlusApp.call(that);
            } else {

                    $.ajax({
                        url: "/api/maintenance/flash",
                        type: "PUT",
                        dataType: "json",
                        data: {},
                        contentType: "application/json",
                        success: function(data, status, xhr) {
                            console.log("Flash prep successful, uploading image.");                 
                            that.BIOS_FLASH=false;
                            that.doBootPlusApp.call(that);

                            that.flashAreaPrepared = true;
                        },
                        error : app.HTTPErrorHandler
                    });
            }
        },

        startHPM: function() {

            var that = this;
            
                $.ajax({
                    url: "/api/maintenance/flash",
                    type: "PUT",
                    dataType: "json",
                    data: {},
                    contentType: "application/json",
                    success: function(data, status, xhr) {              
                        that.BIOS_FLASH=true;
                        that.doBootPlusApp.call(that);
                    },
                    error : app.HTTPErrorHandler
                });
        },

        doBootPlusApp:function() {

            var files = this.HPM_FILE_OBJ;// $("brwsUpld").files;
            var file = files[0];
            
            var start;
            var stop;
            var start1;
            var stop1;
            var i,j;
            
            for(i=0;i<this.HPM_COMPONENT_DATA_START_END.length;i++){
                
                var compId=parseInt(this.HPM_COMPONENT_DATA_START_END[i].split("-")[0]);
                
                if(this.HPM_BOOT_COMP==compId){
                    start=parseInt(this.HPM_COMPONENT_DATA_START_END[i].split("-")[1]);
                    stop=parseInt(this.HPM_COMPONENT_DATA_START_END[i].split("-")[2]);
                    continue;
                }
                if(this.HPM_APP_COMP==compId){
                    start1=parseInt(this.HPM_COMPONENT_DATA_START_END[i].split("-")[1]);
                    stop1=parseInt(this.HPM_COMPONENT_DATA_START_END[i].split("-")[2]);
                    continue;
                }
                
            }                
            var blob = file.slice(start,stop);
            var blob1 = file.slice(start1, stop1);
            
/*                var myBlobBuilder = new MyBlobBuilder();
            myBlobBuilder.append(blob);
            myBlobBuilder.append(blob1);
            
            
            var string=myBlobBuilder.getBlob();*/

            var BlobParts = [blob, blob1];
            var fwimageBlob = new Blob(BlobParts);

            var that = this;

            var data = new FormData();
            data.append('fwimage', fwimageBlob);

            console.log("Image prepared, starting upload.");

                $.ajax({
                    url: "/api/maintenance/firmware",
                    type: "POST",
                    contentType: false,
                    processData: false,
                    dataType: "json",
                    cache: false,
                    data: data,
                    xhr: function() {
                        var cxhr = $.ajaxSettings.xhr();
                        if(cxhr.upload) {
                            cxhr.upload.addEventListener('progress', that.uploadProgress, false);
                        }
                        return cxhr;
                    },
                    success: function(data, status, xhr) {
                    console.log("Upload successful, verifying firmware.");
                        that.verifyFirmware.call(that);
                    },
                    error: app.HTTPErrorHandler
                });
        
        },

        prepareHPMFlash: function(comp_index) { 

            var that = this;

                $.ajax({
                    url: "/api/maintenance/hpm/updatemode",
                    type: "PUT",
                    dataType: "json",
                    data: {},
                    contentType: "application/json",
                    success: function(data, status, xhr) {
                        console.log("Entered update mode, preparing component: "+that.HPM_COMPONENT_DATA_NAME[comp_index]+" for update.");
                        $(".cmpnt:checkbox:checked[data-vid="+that.HPM_COMPONENT_VERS_ID[comp_index]+"]").closest('tr').find('td:nth-child(6)').html(locale.up_ongoing);
                        that.prepareHPMComponent.call(that,data.unique_id,comp_index);
                    },
                    error : app.HTTPErrorHandler
                    // add a better error handler
                });
        },

        prepareHPMComponent: function(uId,cNdx) {       

            if(this.HPM_COMPONENT_HPM_ID== null) return;

            var cid = this.HPM_COMPONENT_CID[cNdx];
            var start = parseInt(this.HPM_COMPONENT_DATA_START_END[cNdx].split("-")[1]);
            var end = parseInt(this.HPM_COMPONENT_DATA_START_END[cNdx].split("-")[2]);
            var len = end - start;

            var that = this;
            var data = {};
            data.FWUPDATEID = uId;
            var objectErr = JSON.stringify(data);
            data.COMPONENT_ID = cid;
            data.COMPONENT_DATA_LEN = len;
            if(cid == this.HPM_MMC_CID) {
                data.IS_MMC = 1;
            } else {
                data.IS_MMC = 0;
            }

            var hpm_id = this.HPM_COMPONENT_HPM_ID[cNdx];
            var needsPrep = (hpm_id == that.HPM_MMC_COMP) || (hpm_id == that.HPM_BOOT_COMP) || (hpm_id == that.HPM_APP_COMP);

            var object = JSON.stringify(data);
        
            if(needsPrep && this.flashAreaPrepared) {
                console.log("Component: "+that.HPM_COMPONENT_DATA_NAME[cNdx]+" preparation already done, starting upload.");
                that.doHPMComponentUpload.call(that,cNdx,data.COMPONENT_ID,data.COMPONENT_DATA_LEN,data.FWUPDATEID);        
            } else {

                    $.ajax({
                        url: "/api/maintenance/hpm/preparecomponents",
                        type: "PUT",
                        dataType: "json",
                        data: object,
                        contentType: "application/json",
                        success: function(data, status, xhr) {
                            console.log("Component: "+that.HPM_COMPONENT_DATA_NAME[cNdx]+" prepared for update, starting upload.");
                            that.doHPMComponentUpload.call(that,cNdx,data.COMPONENT_ID,data.COMPONENT_DATA_LEN,data.FWUPDATEID);

                            if(needsPrep) {
                                that.flashAreaPrepared = true;
                            }
                        },
                        error : [
                            function () {

                                $.ajax({
                                url: "/api/maintenance/hpm/exitupdatemode",
                                type: "PUT",
                                dataType: "json",
                                data: objectErr,
                                contentType: "application/json",
                                success: function(data, status, xhr) {
                                    alert("Failed to prepare HPM components, update canceled.");
                                },
                                error: app.HTTPErrorHandler
                                });

                            },
                            app.HTTPErrorHandler
                        ]
                    });

            }
        },

        doHPMComponentUpload:function(cNdx,cid,len,uId) {

            var start;
            var stop;
            
            var files = this.HPM_FILE_OBJ;// $("brwsUpld").files;
            var file = files[0];
                                    
            compId=parseInt(this.HPM_COMPONENT_DATA_START_END[cNdx].split("-")[0]);
            start=parseInt(this.HPM_COMPONENT_DATA_START_END[cNdx].split("-")[1]);
            stop=parseInt(this.HPM_COMPONENT_DATA_START_END[cNdx].split("-")[2]);
            
            var blob = file.slice(start, stop);

            var data = new FormData();
            var name = "";
            var urlbase = "/api/maintenance/hpm/";
            var endpoint = "";

            if(cid == this.HPM_MMC_CID) {
                name = "mmc";
                endpoint = urlbase + "mmcfw";
            }
            if(cid == this.HPM_BIOS_CID) {
                name = "hpmbios";
                endpoint = urlbase + "biosfw";
            }

            data.append(name, blob);

            var that = this;

                $.ajax({
                    url: endpoint,
                    type: "POST",
                    contentType: false,
                    processData: false,
                    dataType: "json",
                    cache: false,
                    data: data,
                    xhr: function() {
                        var cxhr = $.ajaxSettings.xhr();
                        if(cxhr.upload) {
                            cxhr.upload.addEventListener('progress', that.uploadProgress, false);
                        }
                        return cxhr;
                    },
                    success: function(data, status, xhr) {    
                        console.log("Component: "+that.HPM_COMPONENT_DATA_NAME[cNdx]+" upload successful, starting upgrade.");                
                        that.doHPMStartFlash.call(that,cNdx,cid,len,uId);
                    },
                    error: app.HTTPErrorHandler
                });
        },

        doHPMStartFlash:function(cNdx,cId,complen,uId) {      

            var hpm_section_flash;

            var compid=parseInt(this.HPM_SECTION_FLASH[cNdx].split("-")[0]);
            if((compid == this.HPM_BIOS_COMP) || (compid == this.HPM_MMC_COMP)) {
                hpm_section_flash = parseInt(this.HPM_SECTION_FLASH[cNdx].split("-")[1]);
            } else {
                hpm_section_flash =0;

            }

            var that = this;
            var data = {};
            data.COMPONENT_ID = cId;
            data.COMPONENT_DATA_LEN = complen;
            data.FWUPDATEID = uId;
            data.SECTION_FLASH = hpm_section_flash;

            var object = JSON.stringify(data);
        
                $.ajax({
                    url: "/api/maintenance/hpm/flash",
                    type: "PUT",
                    dataType: "json",
                    data: object,
                    contentType: "application/json",
                    success: function(data, status, xhr) {
                        console.log("Component: "+that.HPM_COMPONENT_DATA_NAME[cNdx]+" upgrade started...");
                        that.HPMFlashProgress.call(that,cNdx,data.COMPONENT_ID,data.COMPONENT_DATA_LEN,data.FWUPDATEID);
                    },
                    error : app.HTTPErrorHandler
                });
        },

        HPMFlashProgress:function(cNdx,cid,len,uId) {

            var that = this;
            var data = {};
            data.COMPONENT_ID=cid;

                $.ajax({
                    url: "/api/maintenance/hpm/upgradestatus",
                    type: "GET",
                    dataType: "json",
                    data: data,
                    contentType: "application/json",
                    success: function(data, status, xhr) {
                        var progress = parseInt(data.PROGRESS, 10);
                        $("#flash_progress").attr({"aria-valuenow": progress, "aria-valuemax": 100});
                        $("#flash_progress").css("width", (progress/2) + "%");
                        $(".progress-info").html("Flashing "+"("+data.PROGRESS+"%)");
                        if(progress<100) {
                            that.HPMFlashProgress.call(that,cNdx,data.COMPONENT_ID,len,uId);
                        } else if(progress == 100) {
                            //that.reset.call(that);
                            console.log("Component: "+that.HPM_COMPONENT_DATA_NAME[cNdx]+" upgrade successful, verifying upgrade.");
                            that.verifyHPMStatus.call(that,cNdx,data.COMPONENT_ID,len,uId);
                        }
                    },
                    error: app.HTTPErrorHandler
                });
        },

        verifyHPMStatus:function(cNdx,cid,len,uId) {
            
            var that = this;
            var data = {};
            data.COMPONENT_ID=cid;
            data.COMPONENT_DATA_LEN=len;
            data.FWUPDATEID=uId;

            var object=JSON.stringify(data);
        
            $.ajax({
                url: "/api/maintenance/hpm/verifyimage",
                type: "PUT",
                dataType: "json",
                data: object,
                contentType: "application/json",
                success: function(data, status, xhr) {                    
                    //that.HPMFlashProgress.call(that,data.COMPONENT_ID);
                    console.log("Component: "+that.HPM_COMPONENT_DATA_NAME[cNdx]+" verification started...");
                    that.getHPMVerifyFlashStatus.call(that,cNdx,data.COMPONENT_ID,uId);
                },
                error : app.HTTPErrorHandler
            });
        },

        getHPMVerifyFlashStatus:function(cNdx,cid,uId){
            
            var that = this;
            var data = {};
            data.COMPONENT_ID=cid;

            $.ajax({
                url: "/api/maintenance/hpm/verifyimagestatus",
                type: "GET",
                dataType: "json",
                data: data,
                contentType: "application/json",
                success: function(data, status, xhr) {
                    var progress = parseInt(data.PROGRESS, 10);
                    $("#flash_progress").attr({"aria-valuenow": progress, "aria-valuemax": 100});
                    $("#flash_progress").css("width", (progress/2) + "%");
                    $(".progress-info").html("Flashing "+"("+data.PROGRESS+"%)");
                    if(progress<100) {
                        that.getHPMVerifyFlashStatus.call(that,cNdx,data.COMPONENT_ID,uId);
                    } else if(progress == 100) {
                    //that.reset.call(that);
                    //that.verifyHPMStatus.call(that,data.COMPONENT_ID);
                        console.log("Component: "+that.HPM_COMPONENT_DATA_NAME[cNdx]+" verification successful, activating upgrade.");
                        that.ActivateHPMComponents.call(that,cNdx,data.COMPONENT_ID,uId);
                    }
                },
                error: app.HTTPErrorHandler
            });
        },

        ActivateHPMComponents: function(cNdx,cid,uId) {
            
            var that = this;
            var data = {};
            data.COMPONENT_ID=cid;
            var object=JSON.stringify(data);
            var activatecomp = true;


            // skip activation if it requires a reboot (e.g. MMC upgrade) and there are pending components
            if((cid == this.HPM_MMC_CID) && this.HPM_FLASH_QUEUE.length) {
                activatecomp = false;
            }

            if(activatecomp) { 

                    $.ajax({
                        url: "/api/maintenance/hpm/activatecomponents",
                        type: "PUT",
                        dataType: "json",
                        data: object,
                        contentType: "application/json",
                        success: function(data, status, xhr) {
                            console.log("Component: "+that.HPM_COMPONENT_DATA_NAME[cNdx]+" activation successful, exitting upgrade mode.");
                            if(cid == that.HPM_MMC_CID) {
                                alert(locale.reset);
                                window.fw_upgrade = false;
                            }
                            that.doExitComponent.call(that,cNdx,uId);
                        },
                        error: app.HTTPErrorHandler
                    });

            } else {
                console.log("Component: "+that.HPM_COMPONENT_DATA_NAME[cNdx]+" activation skipped for now, exitting upgrade mode.");
                that.doExitComponent.call(that,cNdx,uId);
            }
        },

        doExitComponent: function(cNdx,uId){
            
            var that = this;
            var data = {};
            data.FWUPDATEID = uId;
            var object = JSON.stringify(data);
            var hpm_id = this.HPM_COMPONENT_HPM_ID[cNdx];

                $.ajax({
                    url: "/api/maintenance/hpm/exitupdatemode",
                    type: "PUT",
                    dataType: "json",
                    data: object,
                    contentType: "application/json",
                    success: function(data, status, xhr) {
                    
                        console.log("Component: "+that.HPM_COMPONENT_DATA_NAME[cNdx]+" upgrade complete!");

                        if((hpm_id == that.HPM_MMC_COMP) || (hpm_id == that.HPM_BOOT_COMP) || (hpm_id == that.HPM_APP_COMP)) {
                            that.needReset = true;
                        }

                        $(".cmpnt:checkbox:checked[data-vid="+that.HPM_COMPONENT_VERS_ID[cNdx]+"]").closest('tr').find('td:nth-child(6)').html(locale.up_success);

                        if(hpm_id== that.HPM_BIOS_COMP && that.HPM_FLASH_QUEUE.length ==0){
                            alert(locale.bios_update);
                            return;
                        }
                        if(that.proceedNextComponent.call(that)) {
                            // Next update was started
                        } else {
                            // if there's nothing left, attempt reset
                            $('#start').html(locale.stop_fw_update);
                            that.reset.call(that);
                        }
                    },
                    error: app.HTTPErrorHandler
                });
        },

        readHPMUpgradeComponentData: function(opt_startByte,opt_stopByte,type,i,compId) {
            
            var files = this.HPM_FILE_OBJ;// $("brwsUpld").files;
            var file = files[0];
            var start = parseInt(opt_startByte) || 0;
            var stop = parseInt(opt_stopByte) || file.size - 1;
            var that = this;

            var reader= new FileReader();
            reader.onloadend = function (evt) {
                if (evt.target.readyState == FileReader.DONE) { // DONE == 2
                    //result=binaryToHex(ABC.toBinary(evt.target.result, 0)).result;
                    result = that.base64ToHex.call(that,that._arrayBufferToBase64.call(that,evt.target.result));
                    
                    startUpgrade=start * 2;
                    endUpgrade=startUpgrade + (6*2);
                    
                    var component_version = "";
                    var componentVersion=result.substring(0,12);
                    
                    component_version = component_version+parseInt(componentVersion.substring(0,2),16);
                    component_version = component_version+"."+parseInt(componentVersion.substring(2,4),16);
                    
                    var aux_version = "";
                    var auxVersion = componentVersion.substring(4,12);

                    var j = 0;
                    
                    for(i=auxVersion.length-1;i>=0;i--){
                        if(j%2==1){
                            aux_version=aux_version + auxVersion[i] + auxVersion[i+1];
                        }
                        j++;
                    }
                    component_version = component_version+"."+parseInt(aux_version,16);
                    
                    that.HPM_COMPONENT_DATA_VERSION.push(component_version);
                    
                    startUpgrade=endUpgrade;
                    endUpgrade=startUpgrade + (21*2);
                    
                    var componentName=that.hex2a.call(that,result.substring(12,54));//Component Desc/Name
                    
                    
                    that.HPM_COMPONENT_DATA_NAME.push(componentName +"-"+compId);
                    
                    startUpgrade=endUpgrade;
                    endUpgrade=startUpgrade + (4*2);
                    
                    var componentLength=result.substring(54,62);//Component length
                    componentLength=componentLength.split("");
                    
                    var comLength='';
                    var j=0;
                    
                    for(i=componentLength.length-1;i>=0;i--){
                        if(j%2==1){
                            comLength=comLength + componentLength[i] + componentLength[i+1];
                        }
                        j++;
                    }
                    var oemHeaderLength= result.substring(62,94);//OEM Header Length 16 bytes
                    var oemsig= that.hex2a.call(that,result.substring(62,70));//OEM Signature; 4 bytes
                    var oemsectionflashvalue=result.substring(70,78);//OEM Signature;  4 bytes
                    oemsectionflashvalue=oemsectionflashvalue.split("");
                        var sectionflashLength='';
                        var j=0;
                        for(i=oemsectionflashvalue.length-1;i>=0;i--){
                            if(j%2==1){
                                sectionflashLength=sectionflashLength + oemsectionflashvalue[i] + oemsectionflashvalue[i+1];
                            }
                            j++;
                        }
                    var sectionflash= parseInt(sectionflashLength,16);
                    //console.log(sectionflash);
                
                    var comLen;
                    if(oemsig=="OEM") {
                        that.sbit=that.ebit;
                        comLen= parseInt(comLength,16) - 16;
                    } else {
                        that.sbit=that.ebit -16;
                        sectionflash =0;
                        comLen= parseInt(comLength,16);
                    }
                    that.ebit=that.sbit + comLen;
                    
                    that.HPM_COMPONENT_DATA_START_END.push(compId+'-'+that.sbit+'-'+that.ebit);
                    that.HPM_SECTION_FLASH.push(compId+'-'+sectionflash);
                    that.sbit=that.ebit;
                    that.ebit=that.sbit+3;
                    that.readHPMUpgradeAction.call(that,that.sbit,that.ebit,3,0);
                }
            };
                 
            var blob = file.slice(start, stop);
            //reader.readAsBinaryString(blob);
            reader.readAsArrayBuffer(blob);

        },

        hex2a:function (hex) {

            var str = '';
            for (var i = 0; i < hex.length; i += 2) {
                var v = parseInt(hex.substr(i, 2), 16);
                if (v) str += String.fromCharCode(v);
            }
            return str;
        },

        updateSection: function(title,content,btnAry) {

            $('#section').className = "wizardsection";
            // $('#section').style.display = "";
            $('#section').css("display", "");
            var hldrTitle = document.createElement("h3");
            hldrTitle.className = "title";
            hldrTitle.innerHTML = title;
            var hldrBtn = document.createElement("div");
            hldrBtn.className = "btnTray";

            for(var i=0; i<btnAry.length; i++) {
                hldrBtn.appendChild(btnAry[i]);
            }

            $('#section').append(hldrTitle);
            $('#section').append(content);
            $('#section').append(hldrBtn);
        },

        base64ToHex: function (str) {

            for (var i = 0, bin = atob(str.replace(/[ \r\n]+$/, "")), hex = []; i < bin.length; ++i) {
                var tmp = bin.charCodeAt(i).toString(16);
                if (tmp.length === 1) tmp = "0" + tmp;
                hex[hex.length] = tmp;
            }
            return hex.join("");
        },

        _arrayBufferToBase64: function(buffer) {
            var binary = '';
            var bytes = new Uint8Array(buffer);
            var len = bytes.byteLength;
            for (var i = 0; i < len; i++) {
                binary += String.fromCharCode(bytes[i]);
            }
            return window.btoa(binary);
        },

        upload: function() {

            var data = new FormData();
            data.append('fwimage', $('#filefirmware_image')[0].files[0]);

            var that = this;

            $.ajax({
                url: "/api/maintenance/firmware",
                type: "POST",
                contentType: false,
                processData: false,
                dataType: "json",
                cache: false,
                data: data,
                xhr: function() {
                    var cxhr = $.ajaxSettings.xhr();
                    if(cxhr.upload) {
                        cxhr.upload.addEventListener('progress', that.uploadProgress, false);
                    }
                    return cxhr;
                },
                success: function(data, status, xhr) {
                    that.verifyFirmware.call(that);
                },
                error: app.HTTPErrorHandler
            });
        },

        uploadProgress: function(e) {
            if(e.lengthComputable) {
                $("#upload_progress").attr({"aria-valuenow": e.loaded, "aria-valuemax": e.total});
                $("#upload_progress").css("width", (e.loaded/e.total)*50+"%");
                $(".progress-info").html("Uploading "+Math.ceil((e.loaded/e.total)*100)+"%");
            }
        },

        verifyFirmware: function() {
            var that = this;

                $.ajax({
                    url: "/api/maintenance/firmware/verification",
                    type: "GET",
                    success: function(data, status, xhr) {
                            console.log("Verification succesful, starting flash upgrade.");
                            that.startUpgrade.call(that);
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
    		data.preserve_config=0;
    		data.flash_status=1;
    		var object=JSON.stringify(data);
    		
                $.ajax({
                    url: "/api/maintenance/firmware/upgrade",
                    type: "PUT",
                    dataType: "json",
                    data: object,
                    contentType: "application/json",
                    success: function(data, status, xhr) {
                        console.log("Flash upgrade started...");
                        that.flashProgress.call(that);
                    },
                    error : app.HTTPErrorHandler
                });
    	},

        flashProgress: function() {

            var that = this;

            $.ajax({
                url: "/api/maintenance/firmware/flash-progress",
                type: "GET",
				success: function(data, status, xhr) {
					//console.log("flash progress state"+data.state);
					if(data.state == undefined || data.state == "undefined") {
						 that.flashProgress.call(that);
					} else {
                    var progress = parseInt(data.progress, 10);
                    $("#flash_progress").attr({"aria-valuenow": progress, "aria-valuemax": 100});
                    $("#flash_progress").css("width", (progress/2) + "%");
                    $(".progress-info").html("Flashing "+" ("+data.progress+")");
					
						if(progress<100) {
							that.flashProgress.call(that);
						} else if(progress == 100) {
							that.needReset = true;
							console.log("Flash upgrade finished, resetting BMC");
							$(".cmpnt:checkbox:checked[data-order="+that.BOOT_ORDER+"]").closest('tr').find('td:nth-child(6)').html(locale.up_success);
							$(".cmpnt:checkbox:checked[data-order="+that.APP_ORDER+"]").closest('tr').find('td:nth-child(6)').html(locale.up_success);

							if(that.proceedNextComponent.call(that)) {
								// Next update was started
							} else {
								// if there's nothing left, attempt reset
								$('#start').html(locale.stop_fw_update);
								that.reset.call(that);
							}
						}
					}
                },
                error : function(xhr, status, err) {
                    that.reset.call(that);
                    app.HTTPErrorHandler(xhr, status, err);
                }
            });

        },

        loadSectionCfgHPM: function() {

            var that = this;
            var urldata = {};
            var vid;
            urldata.VERS_ID = this.HPM_COMPONENT_VERS_ID.join(",");

            $.ajax({
                url: "/api/maintenance/hpm/componentversions",
                type: "GET",
                dataType: "json",
                data: urldata,
                success: function(data, status, xhr) {

                    that.reorderHPMComponents.call(that);
                    that.total_cmpnts = 0;
                    var name = "";
                    var rowIndex = 1;
                    var cmpnts = "<table class=\"table\" id=\"cmpnt_table\">";
                    cmpnts += "<thead><tr>";
                    cmpnts += "<th>" + locale.str_hash + "</th>";
                    cmpnts += "<th>" + locale.str_component_name + "</th>";
                    cmpnts += "<th>" + locale.str_fw_existing_version + "</th>";
                    cmpnts += "<th>" + locale.str_fw_uploaded_version + "</th>";
                    cmpnts += "<th>" + locale.str_upgrade + "</th>";
                    cmpnts += "<th>" + locale.up_status + "</th>";
                    cmpnts += "</tr></thead>";
                    for(i = 0; i < that.HPM_FLASH_ORDER.length; i++) {
                        if(that.HPM_FLASH_ORDER[i] > -1) {
                            name = that.HPM_COMPONENT_DATA_NAME[that.HPM_FLASH_ORDER[i]].split("-")[0];
                            cmpnts += "<tr>";
                            cmpnts += "<td>" + rowIndex + "</td>";
                            cmpnts += "<td>" + name + "</td>";
                            cmpnts += "<td>" + data[that.HPM_FLASH_ORDER[i]].current_version + "</td>";
                            cmpnts += "<td>" + that.HPM_COMPONENT_DATA_VERSION[that.HPM_FLASH_ORDER[i]] + "</td>";
                            vid = that.HPM_COMPONENT_VERS_ID[that.HPM_FLASH_ORDER[i]];
                            cmpnts += "<td><center><input class=\"cmpnt\" type=\"checkbox\" name=\"cmpnt\" data-vid=\""+vid+"\" data-order=\""+i+"\" checked></center></td>";
                            cmpnts += "<td>&nbsp;</td>";
                            cmpnts += "</tr>";
                            rowIndex++;
                            that.total_cmpnts++;
                        }
                    }
                    cmpnts += "</table>";
                    $("#cmpnt_options").html(cmpnts);
                    $('#cmpnt_options tr th:nth-child(6), td:nth-child(6)').hide();

                    $("#cmpnt_table").find('input').iCheck({
                        checkboxClass: 'icheckbox_square-blue',
                        increaseArea: '20%'
                    });

                    // if BOOT and APP are available, connect their checkboxes so that they are only flashed in tandem
                    var bootbox = $(".cmpnt:checkbox[data-vid="+that.HPM_BOOT_VID+"]");
                    var appbox = $(".cmpnt:checkbox[data-vid="+that.HPM_APP_VID+"]");

                    if(bootbox.length && appbox.length) {
                        bootbox.on('ifToggled', function(event) {
                            if(bootbox.is(':checked') != appbox.is(':checked')) {
                                appbox.iCheck('toggle');
                            }
                        });
                        appbox.on('ifToggled', function(event) {
                            if(bootbox.is(':checked') != appbox.is(':checked')) {
                                bootbox.iCheck('toggle');
                            }
                        });
                    }
            
                    // $("#start").removeClass("disabled").html(locale.cont_update);
                    $("#cmpnt_options").removeClass("hide");
		    $('#filefirmware_image').attr("disabled","disabled");
                },
                error : function(xhr, status, err) {
                    // that.reset.call(that);
                    app.HTTPErrorHandler(xhr, status, err);
                }
            });
        },

        unloadSectionCfgHPM: function() {
            $("#cmpnt_options").html("");
            $("#cmpnt_options").addClass("hide");
        },

        reorderHPMComponents: function(){
            
            // console.log(this.HPM_COMPONENT_DATA_NAME);
            // create a mapping of preferred flashing order -> component array order
            // preferred order is BIOS > MMC > BOOT > APP
            
            if(this.HPM_COMPONENT_DATA_NAME.length > 0){
                for(i=0;i<this.HPM_COMPONENT_DATA_NAME.length;i++){
                    var id = parseInt(this.HPM_COMPONENT_DATA_NAME[i].split("-")[1]);
                    switch (id) {
                        case this.HPM_BIOS_COMP:
                            this.HPM_FLASH_ORDER[this.BIOS_ORDER] = i;
                        break;
                        case this.HPM_MMC_COMP:
                            this.HPM_FLASH_ORDER[this.MMC_ORDER] = i;
                        break;
                        case this.HPM_BOOT_COMP:
                            this.HPM_FLASH_ORDER[this.BOOT_ORDER] = i;
                        break;
                        case this.HPM_APP_COMP:
                            this.HPM_FLASH_ORDER[this.APP_ORDER] = i;
                        break;
                        default:
                    }
                }
                //console.log(this.HPM_COMPONENT_HPM_ID)
            }
        },

        reset: function() {

    		var that = this;
            if(this.needReset) {
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
                         setTimeout(function(){
                            app.logout();
                            document.cookie = "QSESSIONID=1;expires=Thu, 01 Jan 1970 00:00:00 GMT";
                            app.router.navigate("login");
                            location.reload(); 
                        },2 * 60 * 1000);
						//setTimeout(function() {console.log("refreshing..."); location.reload;}, 2 * 60 * 1000);
                    },
                    error : app.HTTPErrorHandler
                });
            }
    	},

        serialize: function() {

            return _.extend({
                locale: locale
            }, this.model.toJSON());
        }

    });

    return view;

});

