define(['jquery', 'underscore', 'backbone', 'app',
//data
'models//create_logical_device',
'collection//physical_device_info',
//localize
'i18n!strings/nls/create_logical_device',
//template
'text!templates/settings/raid_management/create_logical_device.html',
//plugins
'tooltip', 'alert'],

function($, _, Backbone, app, CreateLogicalDeviceModel,PhysicalDeviceInfoCollection, locale, CreateLogicalDeviceTemplate) {

    var view = Backbone.View.extend({

        template: _.template(CreateLogicalDeviceTemplate),
        RAID_LEVELS_PHYSICAL_DEVICE_COUNT:null,
        RAID_LEVELS_STRIP_SIZE_DATA:null,
        RAID_VALID_SPAN_RAID_LEVELS:[6,7,8,9,10],
        IsSpanCreation:false,
        initialize: function() {
            this.model = CreateLogicalDeviceModel;
			this.physical_device_info = PhysicalDeviceInfoCollection;
			
			this.collection = this.physical_device_info;
            this.model.bind('change:VDNAME', this.updateVDNAME, this);
            this.model.bind('change:StripeSize', this.updateStripeSize, this);
            this.model.bind('change:initState', this.updateInitState, this);
            this.model.bind('change:diskCachePolicy', this.updateDiskCachePolicy, this);
            this.model.bind('change:VDSize', this.updateVDSize, this);
            this.model.bind('change:readpolicy', this.updateReadpolicy, this);
            this.model.bind('change:writepolicy', this.updateWritepolicy, this);
            this.model.bind('change:accesspolicy', this.updateAccesspolicy, this);
            this.model.bind('change:deviceId', this.updateDeviceId, this);
            this.model.bind('change:raid_level', this.updateRaidLevel, this);
            this.model.bind('change:iopolicy', this.updateIopolicy, this);


        },

        events: {
            "click #save": "save",
            "click .help-link": "toggleHelp",
			"change #idraid_name":"changeUnConfigDetails",
            "change #idraid_level":"doCreateSpans",
            "click #btnCreateSpan":"doSpanMap",
            "change #iddevice_id":"calVirtualCapacity"
        },

        beforeRender: function() {

        },

        afterRender: function() {
			var context = this;
			
			  this.collection.fetch({
                success: function() {
                    //console.log("call load");
                    context.load.call(context);
                },
                failure: function() {

                }
            });
        },

        load: function(model, collection, xhr) {
			var physical_data = this.collection;
			
			var group_models = this.collection.groupBy(function(model) {				
                return model.get('controller_name');
            });
            this.$('#idraid_name').html('');
            for (i in group_models) {
				//console.log("ctrol name" +i);
			    //var temp1= i.substr(0,i.indexOf("("));
				var val=i.substr(i.indexOf("("),i.indexOf(")")).replace("(","").replace(")","");
				//console.log("temp" +val);
                this.$('#idraid_name').append("<option value='" + val + "'>" + i + "</option>");
                //this.$('#idraid_name').change();
            }
			//this.bindControllerNames();			
			//this.bindUnConfigDevices();
			//console.log("length"+this.collection.length);
			
			this.collection.each(function(model) {
				//console.log(model);
				//console.log("isnide each model" +$('#idraid_name').val());
				if($('#idraid_name').val()==model.get("controller_id")){
				 if(model.get("state")==0 || model.get("state")==1){
					//console.log("inside if bindUnConfigDevices")
					var val= model.get("dev_id") + " - " + this.GetPhysicalType(model.get("type")) + " - " + model.get("size");
					var text=model.get("dev_id");
					$('#iddevice_id').append(new Option(val,text));
				 }
				}
			},this);

            this.getVirualRAIDLevelsAndStripSize($('#idraid_name').val().trim());
        },

        reload: function(model, value) {

        },
		changeUnConfigDetails:function(){
            $('#div_span').html('');
            $('#divbuttonCreateSpan').addClass("hide");
            $('#div_span_data').html('');
			this.bindUnConfigDevices();
		},
		bindControllerNames:function(){
			var that=this;
			
			var raid_names= this.collection;
			
			raid_names.each(function(model) {
				var val=model.get("controller_id");
				var text=model.get("controller_name");
				$('#idraid_name').append(new Option(val,text));
			},this);
		},
		
		bindUnConfigDevices:function(){
		   //console.log("inside function");
			var optind = 0;
			$('#iddevice_id').html('');
			var that=this;
			this.collection.each(function(model) {
				if($('#idraid_name').val().trim()==model.get("controller_id")){
				 if(model.get("state")==0 || model.get("state")==1){
					var val= model.get("dev_id") + " - " + this.GetPhysicalType(model.get("type")) + " - " + model.get("size");
					var text=model.get("dev_id");
					$('#iddevice_id').append(new Option(val,text));
				 }
				}
			},this);
            this.getVirualRAIDLevelsAndStripSize($('#idraid_name').val().trim());
		},
        getVirualRAIDLevelsAndStripSize:function(ctrlId){
            var that = this;
            var data = {};
            data.CTRLID = ctrlId;
            var object = JSON.stringify(data);
            
            $.ajax({
                url: "/api/settings/raid_management/raid_levels",
                type: "PUT",
                dataType: "json",
                data:object,
                processData: false,
                contentType: "application/json",
                success: function(data, status, xhr) {
                    that.RAID_LEVELS_STRIP_SIZE_DATA=data;
                    that.getRAIDLevelsPDCount.call(that,ctrlId);
                },
                error : app.HTTPErrorHandler
            });
        },
        getRAIDLevelsPDCount:function(ctrlId){
            var that = this;
            var data = {};
            data.CTRLID = ctrlId;
            var object = JSON.stringify(data);

            $.ajax({
                url: "/api/settings/raid_management/raid_pd_count",
                type: "PUT",
                dataType: "json",
                data:object,
                processData: false,
                contentType: "application/json",
                success: function(data, status, xhr) {
                    that.RAID_LEVELS_PHYSICAL_DEVICE_COUNT=data;
                    that.bindRAIDLevels.call(that);
                },
                error : app.HTTPErrorHandler
            });
        },
        bindRAIDLevels:function(){
            $('#idraid_level').html('');
            var min=0,max=0;
            var raid0=this.RAID_LEVELS_STRIP_SIZE_DATA.RAID0;
            var raid1=this.RAID_LEVELS_STRIP_SIZE_DATA.RAID1;
            var raid5=this.RAID_LEVELS_STRIP_SIZE_DATA.RAID5;
            var raid6=this.RAID_LEVELS_STRIP_SIZE_DATA.RAID6;
            var raid1E=this.RAID_LEVELS_STRIP_SIZE_DATA.RAID1E;
            var raid10=this.RAID_LEVELS_STRIP_SIZE_DATA.RAID10;
            var raid50=this.RAID_LEVELS_STRIP_SIZE_DATA.RAID50;
            var raid60=this.RAID_LEVELS_STRIP_SIZE_DATA.RAID60;
            var raid00=this.RAID_LEVELS_STRIP_SIZE_DATA.RAID00;
            var raid1E_RLQ0=this.RAID_LEVELS_STRIP_SIZE_DATA.RAID1E_RLQ0;
            var raid1E0_RLQ0=this.RAID_LEVELS_STRIP_SIZE_DATA.RAID1E0_RLQ0;
            var name,val;
            
            var totalUnConfigCount= $('#iddevice_id > option').length;

            if(raid0){
                min=parseInt(this.RAID_LEVELS_PHYSICAL_DEVICE_COUNT.RAID0_MIN);
                max=parseInt(this.RAID_LEVELS_PHYSICAL_DEVICE_COUNT.RAID0_MAX);

                if(totalUnConfigCount >= min){
                    name="RAID0";
                    val=0;
                    $('#idraid_level').append(new Option(name,val));
                }
            }

            if(raid1){
                min=parseInt(this.RAID_LEVELS_PHYSICAL_DEVICE_COUNT.RAID1_MIN);
                max=parseInt(this.RAID_LEVELS_PHYSICAL_DEVICE_COUNT.RAID1_MAX);
                if(totalUnConfigCount >= min){
                    name="RAID1";val=1;
                    $('#idraid_level').append(new Option(name,val));
                }
            }

            if(raid5){
                min=parseInt(this.RAID_LEVELS_PHYSICAL_DEVICE_COUNT.RAID5_MIN);
                max=parseInt(this.RAID_LEVELS_PHYSICAL_DEVICE_COUNT.RAID5_MAX);
                if(totalUnConfigCount >= min){
                    name="RAID5";val=2;
                    $('#idraid_level').append(new Option(name,val));
                }
            }

            if(raid6){
                min=parseInt(this.RAID_LEVELS_PHYSICAL_DEVICE_COUNT.RAID6_MIN);
                max=parseInt(this.RAID_LEVELS_PHYSICAL_DEVICE_COUNT.RAID6_MAX);
                if(totalUnConfigCount >= min){
                    name="RAID6";val=3;
                    $('#idraid_level').append(new Option(name,val));
                }
            }

            if(raid10){
                min=parseInt(this.RAID_LEVELS_PHYSICAL_DEVICE_COUNT.RAID10_MIN);
                if(totalUnConfigCount >= min){
                    name="RAID10";val=8;
                    $('#idraid_level').append(new Option(name,val));
                }
            }

            if(raid50){
                min=parseInt(this.RAID_LEVELS_PHYSICAL_DEVICE_COUNT.RAID50_MIN);
                if(totalUnConfigCount >= min){
                    name="RAID50";val=9;
                    $('#idraid_level').append(new Option(name,val));
                }
            }

            if(raid60){
                min=parseInt(this.RAID_LEVELS_PHYSICAL_DEVICE_COUNT.RAID60_MIN);
                if(totalUnConfigCount >= min ){
                    name="RAID60";val=10;
                    $('#idraid_level').append(new Option(name,val));
                }
            }

            if(raid1E){
                min=parseInt(this.RAID_LEVELS_PHYSICAL_DEVICE_COUNT.RAID1E_MIN);
                if(totalUnConfigCount >= min){
                    name="RAID1E";val=4;
                    $('#idraid_level').append(new Option(name,val));
                }
            }

            if(raid1E_RLQ0){
                min=parseInt(this.RAID_LEVELS_PHYSICAL_DEVICE_COUNT.RAID1E_RLQ0_MIN);
                if(totalUnConfigCount >= min){
                    name="RAID1E_RLQ0";val=5;
                    $('#idraid_level').append(new Option(name,val));
                }
            }

            if(raid1E0_RLQ0){
                min=parseInt(this.RAID_LEVELS_PHYSICAL_DEVICE_COUNT.RAID1E0_RLQ0_MIN);
                if(totalUnConfigCount >= min){
                    name="RAID1E0_RLQ0";val=6;
                    $('#idraid_level').append(new Option(name,val));
                }
            }

            if(raid00){
                min=parseInt(this.RAID_LEVELS_PHYSICAL_DEVICE_COUNT.RAID0_MIN);
                if(totalUnConfigCount >= min){
                    name="RAID00";val=7;
                    $('#idraid_level').append(new Option(name,val));
                }
            }
        },
        doCreateSpans:function(){
            //console.log("inside doCreateSpans");
            var selectedLevel=parseInt($('#idraid_level').val());           
            
            var defaltSpanDepth=0;
            var defaltDrivesPerSpan=0;
            
            
            if(this.RAID_VALID_SPAN_RAID_LEVELS.indexOf(selectedLevel) != -1){
                               
                $('#div_span').html('');
                $('#div_span_data').html('');
                //console.log("inside if doCreateSpans");
                $('#iddevice_id').attr("disabled","disabled");
                $('#divbuttonCreateSpan').addClass("hide");
                
                if(selectedLevel== 9 || selectedLevel==10){
                    defaltSpanDepth=2;
                    defaltDrivesPerSpan=3;
                } else if(selectedLevel==8){
                    defaltSpanDepth=2;
                    defaltDrivesPerSpan=2;
                }else if(selectedLevel==7){
                    defaltSpanDepth=2;
                    defaltDrivesPerSpan=1;
                }
                //show span logic
                this.createSpan(defaltSpanDepth,defaltDrivesPerSpan);
                this.IsSpanCreation=true;
            } else {
                //hide span logic
                if(this.IsSpanCreation==true){
                    $('#div_span').html('');
                    $('#divbuttonCreateSpan').addClass("hide");
                    $('#div_span_data').html('');
                }
                
                $('#iddevice_id').removeAttr("disabled");

                this.IsSpanCreation=false;
            }
        },
        createSpan:function(sDepth,drivesPerSpan){
            
            //console.log("inside createSpan");

            var innerSpan="<div class='alert alert-info help-item hide' role='alert'>"+locale.create_logical_device_enter_span_depth +"</div>"+
                "<label for='spandepth'>"+locale.span_label1 +"</label>"+    
                "<input name='txtSpanDepth' maxlength='3' id='txtSpanDepth' value="+ sDepth +" type='text' class='form-control'>"+
                "<div class='alert alert-info help-item hide' role='alert' style='margin-top:10px'>"+ locale.create_logical_device_enter_drives_per_span +"</div>"+
                "<label for='drivesspan'>"+locale.span_label2 +"</label>"+
               "<input name='txtSpanMax' maxlength='2' id='txtSpanMax' type='text' value="+ drivesPerSpan +" class='form-control'>"+
                "<div class='alert alert-info help-item hide' role='alert' style='margin-top:10px'>"+ locale.create_logical_device_select_un_config_devices +"</div>"+
                "<label for='drivesspan'>"+locale.span_label3 +"</label>"+
               "<select name='lstSpanUnconfigPhyDrives' id='lstSpanUnconfigPhyDrives' multiple='' class='form-control'>";
                
            $('#div_span').html(innerSpan);

            $('#divbuttonCreateSpan').removeClass("hide");
            
            //console.log("before _lstSpanUnconfigPhyDrives");
            var obj=document.getElementById("lstSpanUnconfigPhyDrives");
            this.collection.each(function(model) {
                if($('#idraid_name').val().trim()==model.get("controller_id")){
                 if(model.get("state")==0 || model.get("state")==1){
                    var val= model.get("dev_id") + " - " + this.GetPhysicalType(model.get("type")) + " - " + model.get("size");
                    var text=model.get("dev_id");
                    $('#lstSpanUnconfigPhyDrives').append(new Option(val,text));
                 }
                }
            },this);
            //console.log("after _lstSpanUnconfigPhyDrives");
        },
        doSpanMap:function(){
            var depth= document.getElementById("txtSpanDepth");
            var spanmax=document.getElementById("txtSpanMax");
            var noOfSelected=0;
            
            var lstSpanUnconfigPhyDrives= document.getElementById("lstSpanUnconfigPhyDrives");
            
            if (!this.isnumstr(depth.value, 1, 255)) {
                $("#txtSpanDepth").notify(locale.span_depth_err0, {"className": "error"});
                depth.focus();
                return;
            }
            
            if (!this.isnumstr(spanmax.value, 1, 99)) {
                $("#txtSpanMax").notify(locale.span_depth_err1, {"className": "error"});
                spanmax.focus();
                return;
            }
            
            var multiple=parseInt(depth.value*spanmax.value);
            
            for(var i=0;i<lstSpanUnconfigPhyDrives.options.length;i++){
                if(lstSpanUnconfigPhyDrives.options[i].selected){
                    noOfSelected++;
                }
            }
            
            if(multiple!=noOfSelected){
                alert(locale.create_span_err1);
                return;
            }
            
            $('#div_span_data').html('');

            var tab=document.createElement("TABLE");
            tab.width="100%";
            tab.id="tabSpan";
            
            var trheader1=document.createElement("TR");
            var td1=document.createElement("TD");
            td1.width="40%";
            
            td1.innerHTML="<label>"+locale.span_label4+"</label>";

            var td2=document.createElement("TD");
            td2.innerHTML="<label>"+locale.span_label5+"</label>";
            
            trheader1.appendChild(td1);
            trheader1.appendChild(td2);
            tab.appendChild(trheader1);
            
            for(var i=0;i<lstSpanUnconfigPhyDrives.options.length;i++){
                if(lstSpanUnconfigPhyDrives.options[i].selected){
                    
                    var tr=document.createElement("TR");
                    tr.height="25px";
                    var td=document.createElement("TD");
                    td.width="40%";
                    td.innerHTML="<label>"+lstSpanUnconfigPhyDrives.options[i].text+"</label>";
                    td.setAttribute("devId",lstSpanUnconfigPhyDrives.options[i].value)
                    tr.appendChild(td);
                    
                    var td1=document.createElement("TD");
                    var selec="<select name='lstSpan' id='lstSpan"+i+"' class='form-control'></select>";
                    td1.innerHTML=selec;
                    
                    tr.appendChild(td1);
                    tab.appendChild(tr);
                }
            }
            $('#div_span_data').append(tab);

            var spanOptions=parseInt(depth.value);
            
            var temp=0;
            var count=0;
            
            for(var j=1;j<tab.rows.length;j++){
                var lst=tab.rows[j].getElementsByTagName("SELECT")[0];
                for(i=0;i<spanOptions;i++){ 
                    lst.add(new Option(i,i));
                }
            }
            
            for(var j=1;j<tab.rows.length;j++){
                var lst=tab.rows[j].getElementsByTagName("SELECT")[0];
                 if(j%spanmax.value>0){
                     lst.value=temp;
                 }else{
                     lst.value=temp;
                     temp++;
                }
            }
        },
		GetPhysicalType:function(type){
			var result;
				switch	(type) {
				case 0 :
					result=locale.type_0;
					break;
				case 1 :
					result=locale.type_1;
					break;
				case 2 :
					result=locale.type_2;
					break;
				default:
					result=type;
					break;
				}
				return result;
		},
        isnumstr : function(str, min, max)
        {
            min = (min) ? min : 0;
            max = (max) ? max : 300;
            var patt = /[^0-9]/;
            if(patt.test(str))          //Checks for other than numbers
                return false;

            var v = parseInt(str, 10);
            if (v < min || v > max)     //Check for min & max range
                return false;

            return typeof v == 'number' && isFinite(v); //Check for Number
        },
        isblank : function(str)
        {
            if(0 == str.length)
                return true;
            return false;
        },
        calVirtualCapacity:function(){
            var cap=0;
            var lstUnconfigPhyDrives=document.getElementById("iddevice_id");
            for(var j=0;j< lstUnconfigPhyDrives.options.length;j++){
                if(lstUnconfigPhyDrives.options[j].selected==true){
                    var size=parseInt(lstUnconfigPhyDrives.options[j].text.split("-")[2]);
                    cap= cap + size;
                }
            }
            //console.log("inside calVirtualCapacity");
            $('#idVDSize').val(cap);
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
            $(".tooltip").hide();
            if (help_items.hasClass('hide')) {
                help_items.removeClass('hide');
            } else {
                help_items.addClass('hide');
            }
        },
        updateVDNAME: function(model, value) {},
        updateStripeSize: function(model, value) {
            $('#idStripeSize').val(value);
        },
        updateInitState: function(model, value) {
            $('#idinitState').val(value);
        },
        updateDiskCachePolicy: function(model, value) {},
        updateVDSize: function(model, value) {
            $('#idVDSize').val(value);
        },
        updateReadpolicy: function(model, value) {},
        updateWritepolicy: function(model, value) {},
        updateAccesspolicy: function(model, value) {},
        updateDeviceId: function(model, value) {},
        updateRaidLevel: function(model, value) {
            $('#idraid_level').val(value);
        },
        updateIopolicy: function(model, value) {},

        validData: function(model) {
            var field = null;
            while ((field = this.errorFields.pop()) !== undefined) {
                field.tooltip('hide');
            }
        },

        errorFields: [],

        invalidData: function(model, errors) {
            //console.log('invalid', errors);
            var elMap = [],
                jel;

            var field = null;
            while ((field = this.errorFields.pop()) !== undefined) {
                field.tooltip('hide');
            }

            for (var e in errors) {
                jel = this.$el.find("#" + elMap[e]);
                jel.tooltip({
                    animation: false,
                    title: errors[e],
                    trigger: 'manual',
                    placement: 'top'
                });
                jel.tooltip('show');
                this.errorFields.push(jel);


            }
            $("#save-icon").removeClass().addClass("fa fa-save");
        },

        ValidateData:function(){
            var result=true;
            var noOfSelected=0;
            var min,max;

            if (this.isblank($("#idVDName").val())) {
                $("#idVDName").notify(locale.create_span_err3, {"className": "error"});
                $("#idVDName").focus();
                result=false;
                return result;
            }
            if (this.isblank($("#idVDSize").val())) {
                $("#idVDSize").notify(locale.create_span_err4, {"className": "error"});
                $("#idVDSize").focus();
                result=false;
                return result;
            }
            if(this.IsSpanCreation){
                var spanDepth=document.getElementById("txtSpanDepth");
                var spanMax=document.getElementById("txtSpanMax");
                var lstSpanUnconfigPhyDrives= document.getElementById("lstSpanUnconfigPhyDrives");
                
                
                if (!this.isnumstr(spanDepth.value, 1, 255)) {
                    $("#txtSpanDepth").notify(locale.span_depth_err0, {"className": "error"});
                    spanDepth.focus();
                    result=false;
                    return result;
                }
                if (!this.isnumstr(spanMax.value, 1, 10)) {
                    $("#txtSpanMax").notify(locale.span_depth_err1, {"className": "error"});
                    spanMax.focus();
                    result=false;
                    return result;
                }

                var multiple=parseInt(spanDepth.value*spanMax.value);
        
                for(var i=0;i<lstSpanUnconfigPhyDrives.options.length;i++){
                    if(lstSpanUnconfigPhyDrives.options[i].selected){
                        noOfSelected++;
                    }
                }
                
                if(multiple!=noOfSelected){
                    alert(locale.create_span_err1);
                    result=false;
                    return result;
                }
                
                var tdspan=document.getElementById("div_span_data");
                var tab=tdspan.getElementsByTagName("TABLE")[0];
                var spanOptions=parseInt(spanDepth.value);
                var counter=0;
                for(var i=0;i<spanOptions;i++){
                    for(var j=1;j<tab.rows.length;j++){
                        var lst=tab.rows[j].getElementsByTagName("SELECT")[0];
                        if(i==lst.value){
                            counter++;
                            continue;
                        }
                    }
                    if(counter <= spanMax.value){
                        counter=0;
                    } else{
                        break;  
                    }
                }
                if(counter > spanMax.value){
                    alert(locale.create_span_err2);
                    result=false;
                    return result;
                }
            } else{
                
                var lstUnconfigPhyDrives=document.getElementById("iddevice_id");
                for(var i=0;i<lstUnconfigPhyDrives.options.length;i++){
                    if(lstUnconfigPhyDrives.options[i].selected){
                        noOfSelected++;
                    }
                }

                if($('#idraid_level').val()==0){                
                min=parseInt(this.RAID_LEVELS_PHYSICAL_DEVICE_COUNT.RAID0_MIN);
                max=parseInt(this.RAID_LEVELS_PHYSICAL_DEVICE_COUNT.RAID0_MAX);
                
                if(min > noOfSelected){
                    alert("Minimum "+ min +" "+locale.create_span_err6);
                    $('#iddevice_id').focus();
                    result=false;
                    return result;
                } else if(max < noOfSelected){
                    alert("Maximum "+ max +" "+locale.create_span_err6);
                    $('#iddevice_id').focus();
                    result=false;
                    return result;
                }
                } else  if($('#idraid_level').val()==1){
                    min=parseInt(this.RAID_LEVELS_PHYSICAL_DEVICE_COUNT.RAID1_MIN);
                    max=parseInt(this.RAID_LEVELS_PHYSICAL_DEVICE_COUNT.RAID1_MAX);
                    if(min > noOfSelected){
                        alert("Minimum "+ min +" "+locale.create_span_err6);
                        $('#iddevice_id').focus();
                        result=false;
                        return result;
                    } else if(max < noOfSelected){
                        alert("Maximum "+ max +" "+locale.create_span_err6);
                        $('#iddevice_id').focus();
                        result=false;
                        return result;
                    }
                } else if($('#idraid_level').val()==2){
                    min=parseInt(this.RAID_LEVELS_PHYSICAL_DEVICE_COUNT.RAID5_MIN);
                    max=parseInt(this.RAID_LEVELS_PHYSICAL_DEVICE_COUNT.RAID5_MAX);
                    if(min > noOfSelected){
                        alert("Minimum "+ min +" "+locale.create_span_err6);
                        $('#iddevice_id').focus();
                        result=false;
                        return result;
                    } else if(max < noOfSelected){
                        alert("Maximum "+ max +" "+locale.create_span_err6);
                        $('#iddevice_id').focus();
                        result=false;
                        return result;
                    }
                } else if($('#idraid_level').val()==3){
                    min=parseInt(this.RAID_LEVELS_PHYSICAL_DEVICE_COUNT.RAID6_MIN);
                    max=parseInt(this.RAID_LEVELS_PHYSICAL_DEVICE_COUNT.RAID6_MAX);
                    if(min > noOfSelected){
                        alert("Minimum "+ min +" "+locale.create_span_err6);
                        $('#iddevice_id').focus();
                        result=false;
                        return result;
                    } else if(max < noOfSelected){
                        alert("Maximum "+ max +" "+locale.create_span_err6);
                        $('#iddevice_id').focus();
                        result=false;
                        return result;
                    }
                } else if($('#idraid_level').val()==4){
                    min=parseInt(this.RAID_LEVELS_PHYSICAL_DEVICE_COUNT.RAID1E_MIN);
                    if(min > noOfSelected){
                        alert("Minimum "+ min +" "+locale.create_span_err6);
                        $('#iddevice_id').focus();
                        result=false;
                        return result;
                    }
                } else if($('#idraid_level').val()==5){
                    min=parseInt(this.RAID_LEVELS_PHYSICAL_DEVICE_COUNT.RAID1E_RLQ0_MIN);
                    if(min > noOfSelected){
                        alert("Minimum "+ min +" "+locale.create_span_err6);
                        $('#iddevice_id').focus();
                        result=false;
                        return result;
                    }
                }
            }
            return result;
        },

        save: function() {

            var valid= this.ValidateData();
            //console.log("validatat "+valid);
            if(!valid) return;

            var selPhyDri="";
            var spanDepth=1;
            var spanMax=0;
            var spanId=0;
            
            var lstUnconfigPhyDrives=document.getElementById("iddevice_id");
            if(this.IsSpanCreation){
                spanDepth=document.getElementById("txtSpanDepth").value;
                spanMax=document.getElementById("txtSpanMax").value;
                
                var divpsan=document.getElementById("div_span_data");
                var tab=divpsan.getElementsByTagName("TABLE")[0];
                for(var i=1;i<tab.rows.length;i++){
                    
                    var devId= tab.rows[i].getElementsByTagName("TD")[0].getAttribute("devId");
                    spanId= tab.rows[i].getElementsByTagName("SELECT")[0].value;
                    selPhyDri = selPhyDri + "~" + "("+devId +"," + spanId +")";
                }
                
            } else {
                for(var i=0;i< lstUnconfigPhyDrives.options.length;i++){
                    if(lstUnconfigPhyDrives.options[i].selected==true){
                        selPhyDri = selPhyDri + "~" + "("+ lstUnconfigPhyDrives.options[i].value +"," + spanId +")";
                        spanMax++;
                    }
                }
            }

            $("#save-icon").removeClass().addClass("ion-loading-c");
            this.$(".alert-success,.alert-danger").addClass("hide");


            var context = this;
            try {
                context = that || this;
            } catch (e) {
                context = this;
            }
			
            context.model.save({
				'CTRLID':$('#idraid_name').val().trim(),
                'VDNAME': $('#idVDName').val(),
                'StripeSize': parseInt($('#idStripeSize').val(), 10),
                'initState': parseInt($('#idinitState').val(), 10),
                'diskCachePolicy': parseInt($('#iddisk_policy').val(), 10),
                'VDSize': parseInt($('#idVDSize').val(), 10),
                'readpolicy': parseInt($('#idread_policy').val(), 10),
                'writepolicy': parseInt($('#idwrite_policy').val(), 10),
                'accesspolicy': parseInt($('#idaccess_policy').val(), 10),
                'deviceId': selPhyDri,
                'raid_level': parseInt($('#idraid_level').val(), 10),
                'iopolicy': parseInt($('#idio_policy').val(), 10),
                'spandepth': spanDepth,
                'numdrives': spanMax
            }, {
                success: function() {

                    context.$("#save-icon").removeClass().addClass("fa fa-save");
                    app.events.trigger('save_success', context);
					
					var path="settings/raid_management/logical_device_info";
					app.router.navigate(path, {
                                            trigger: true
                                       });
                },

                error: function() {
                    context.$("#save-icon").removeClass().addClass("fa fa-save");
                    app.events.trigger('save_error', context);
                }

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
