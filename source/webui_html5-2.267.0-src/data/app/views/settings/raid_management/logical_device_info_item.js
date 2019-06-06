define(['jquery', 'underscore', 'backbone', 'app',
//localize
'i18n!strings/nls/logical_device_info',
//template
'text!templates/settings/raid_management/logical_device_info_item.html'],

function($, _, Backbone, app, locale, LogicalDeviceInfoTemplate) {

    var view = Backbone.View.extend({

        tagName: "tr",

        template: _.template(LogicalDeviceInfoTemplate),

        initialize: function() {
            //this.model.bind("destroy", this.removeItem, this);
        },

        events: {
            //"click .delete": "deleteItem",			
            "click .help-link": "toggleHelp"
			//"click .view": "navigatePhysicalInfo"
        },

        beforeRender: function() {},

        afterRender: function() {
            this.$('.edit').removeClass('hide');
			this.$('.view').removeClass('hide');
			
            var href = this.$el.find('a.edit').attr('href');            
			this.$el.find('a.edit').attr('href', href.replace(':id', this.model.get('id')));
			
            this.$('.delete').removeClass('hide');
			this.$('.delete').attr("data-devid",this.model.get('dev_id'));

			this.$('.LEDActivation').removeClass('hide');
			this.$('.consistencyCheck').removeClass('hide');
			this.$('.initializeCheck').removeClass('hide');

			this.$('.delete').on('click',{model:this.model,devId:this.model.get('dev_id')},this.deleteItem);
			this.$('.view').on('click',{model:this.model,devId:this.model.get('dev_id')},this.navigatePhysicalInfo);

			this.$('.LEDActivation').on('click',{model:this.model,devId:this.model.get('dev_id')},this.callLocateLogical);
			this.$('.consistencyCheck').on('click',{model:this.model,devId:this.model.get('dev_id')},this.callConsistencyCheck);
			this.$('.initializeCheck').on('click',{model:this.model,devId:this.model.get('dev_id')},this.displayInitialize);
			this.$('.initailize').on('click',{model:this.model,devId:this.model.get('dev_id')},this.callInitializeCheck);

			this.$('.delete').attr("title","Click to Delete Virtual Drive");
			this.$('.LEDActivation').attr("title","Click to Locate Virtual Device");
			this.$('.consistencyCheck').attr("title","Click to Start/Cancel Consistency Check");
			this.$('.initializeCheck').attr("title","Click to view Initialize Type to Start/Cancel Initialize");
			this.$('.edit').attr("title","Click to view Advanced Properties");
			this.$('.view').attr("title","Click to view the Physical Information for selected Virtual Drive");
			this.$('.initailize').attr("title","Click to Start/Cancel Initialize");

			//this.doExpandCall();


			//$('.footable tbody tr:not(".footable-row-detail")').on('click',{model:this.model,devId:this.model.get('dev_id')},this.doExpandCall);

			//this.$('.footable tbody tr.footable-even footable-detail-show').on('click',{model:this.model,devId:this.model.get('dev_id')},this.doExpandCall);
			 /*$('.footable').on('click', '.footable-detail-show', function(ev) {
			 	//var row=$( this ).parent();
	          	
	          	 ev.stopPropagation();
            	ev.preventDefault();
				//alert("in row");
	          	//this.doExpandCall();
			 },this.doExpandCall);*/

			 //$('.footable-detail-show').on('click',{model:this.model,devId:this.model.get('dev_id')},this.doExpandCall);

			/*this.$('.footable tbody tr.footable-detail-show').on("click","td:not(.footable-last-column)",function(e){
	          var row=$( this ).parent();
	          alert("in row");
	          //$(row).trigger('footable_toggle_row');
	          console.log("in clikc");
	        });*/


        },
        doExpandCall:function(){
         var that=this;
         var data = {};
		
		data.CTRLID=that.model.get('controller_id');
		data.DEVICEID=that.model.get('dev_id');

		var object=JSON.stringify(data);
		  
		  $.ajax({
				url: "/api/settings/raid_management/logical_device_progress_cmd",
				type: "PUT",
                dataType: "json",
                data:object,
                processData: false,
                contentType: "application/json",
				success: function(data, status, xhr) {
					var con_status=data.CONSISTENCY_PROGRESS;
					if(con_status==0xFF){
						$('.consistencyCheck').html("<i class='fa fa-circle'></i>");
						$('.consistencyCheck').attr("data-consistency",1); //start
						$('.consistencyCheck').attr("title","Click to Start Consistency Check");
					} else {
						$('.consistencyCheck').html("<i class='fa fa-circle text-success'></i>");
						$('.consistencyCheck').attr("data-consistency",0); //cancel
						$('.consistencyCheck').attr("title","Click to Cancel Consistency Check");
					}
				},
				error : function(xhr, status, err) {
					app.HTTPErrorHandler(xhr, status, err);
				}
			});
        },
        removeItem: function() {
            this.$el.remove();
        },

        deleteItem: function(ev) {
			var that=this;
            ev.stopPropagation();
            ev.preventDefault();
            if (!confirm('Are you sure to perform this operation?')) return;

			var model= ev.data.model;

		  var ctlrid=model.get('controller_id');
		  var ctrlName=$('#idgroup_by_controller_name').val();
		  
		  var devId=$('.delete').data("devid");
		  
		  
		var data = {};
		
		data.CTRLID=ctlrid;
		data.DEVICEID=devId;

		var object=JSON.stringify(data);
		  $.ajax({
				url: "/api/settings/raid_management/delete_virtual_drive",
				type: "DELETE",
				dataType: "json",
				data:object,
				processData: false,
				contentType: "application/json",
				success: function(data, status, xhr) {					
					alert(locale.delete_success);
					Backbone.history.loadUrl(Backbone.history.fragment);
				},
				error : function(xhr, status, err) {
					app.HTTPErrorHandler(xhr, status, err);
				}
			});
        },
		refreshData:function(){
			Backbone.history.loadUrl(Backbone.history.fragment);
			if(localStorage.getItem("ctrl_name") != null){
				var selected = localStorage.getItem("ctrl_name");
				if (selected == 'all') {
					this.$('table.footable tbody tr').show();
					return;
				}
				this.$('table.footable tbody tr').hide();
				this.$('table.footable tbody tr:contains("' + selected + '")').show();
			}
		},
		navigatePhysicalInfo:function(ev){
			var model= ev.data.model;
			var ctrl_id=model.get('controller_id');
			var dev_id=model.get('dev_id');
			var path="settings/raid_management/raid_logical_physical_device_info?ctrl_id=" + ctrl_id +"&dev_id="+dev_id;
			app.router.navigate(path, {
                    trigger: true
            });
		},
		callLocateLogical: function(ev){

			var that=this;
			var cRow= $(this).parent();

            ev.stopPropagation();
            ev.preventDefault();
            if (!confirm('Are you sure to perform this operation?')) return;

            cRow.find('.initailize').addClass('hide');
			cRow.find('#idselect_initialize').addClass('hide');

			var model= ev.data.model;

		  var ctlrid=model.get('controller_id');
		  var ctrlName=$('#idgroup_by_controller_name').val();
		  
		  var devId=ev.data.devId; //$('.delete').data("devid");
		  
		  
		var devId=ev.data.devId;//$('.delete').data("devid");
		  var duration=5;// for now duration made it as constant.
		  var action=1;// for now start locate
		  	
		var data = {};
		data.ACTION=action;
		data.CTRLID=ctlrid;
		data.DEVICEID=devId;
		data.DURATION=duration;

		var object=JSON.stringify(data);
		  
		  $.ajax({
				url: "/api/settings/raid_management/logical_start_stop_locate_cmd",
				type: "PUT",
				dataType: "json",
				data:object,
				processData: false,
				contentType: "application/json",
				success: function(data, status, xhr) {					
					alert(locale.locate_success);
					Backbone.history.loadUrl(Backbone.history.fragment);
				},
				error : function(xhr, status, err) {
					app.HTTPErrorHandler(xhr, status, err);
				}
			});
		},
		callConsistencyCheck:function(ev){
			var that=this;
			var cRow= $(this).parent();

            ev.stopPropagation();
            ev.preventDefault();
            if (!confirm('Are you sure to perform this operation?')) return;

            cRow.find('.initailize').addClass('hide');
			cRow.find('#idselect_initialize').addClass('hide');

			var model= ev.data.model;

		  var ctlrid=model.get('controller_id');
		  var ctrlName=$('#idgroup_by_controller_name').val();
		  
		  var devId=ev.data.devId; //$('.delete').data("devid");
		  
		  
			var devId=ev.data.devId;//$('.delete').data("devid");
			var con_status= cRow.find('.consistencyCheck').data("consistency");
		  	
		var data = {};
		data.ACTION=con_status;
		data.CTRLID=ctlrid;
		data.DEVICEID=devId;

		var object=JSON.stringify(data);
		  
		  $.ajax({
				url: "/api/settings/raid_management/logical_consistency_check_cmd",
				type: "PUT",
				dataType: "json",
				data:object,
				processData: false,
				contentType: "application/json",
				success: function(data, status, xhr) {					
					alert(locale.consistency_success);
					Backbone.history.loadUrl(Backbone.history.fragment);
				},
				error : function(xhr, status, err) {
					app.HTTPErrorHandler(xhr, status, err);
				}
			});
		},
		displayInitialize:function(ev){
			var c= ev.data.model;

             var log=ev.data.logical;
			var cRow= $(this).parent();

			var ctlrid=c.get('controller_id');

			cRow.find('.initailize').removeClass('hide');
			cRow.find('#idselect_initialize').removeClass('hide');
		},
		callInitializeCheck:function(ev){

			var that=this;
			var cRow= $(this).parent();

            ev.stopPropagation();
            ev.preventDefault();
            if (!confirm('Are you sure to perform this operation?')) return;

			var model= ev.data.model;

		  var ctlrid=model.get('controller_id');
		  var ctrlName=$('#idgroup_by_controller_name').val();
		  
		  //var devId=ev.data.devId; //$('.delete').data("devid");
		  
		  
			var devId=ev.data.devId;//$('.delete').data("devid");
		  var initialize_type=cRow.find('#idselect_initialize').val();// for now initialize type made it as constant.
		  var action=1;// for now start locate
		  	
		var data = {};
		data.ACTION=action;
		data.CTRLID=ctlrid;
		data.DEVICEID=devId;
		data.INIT_TYPE=initialize_type;

		var object=JSON.stringify(data);
		  
		  $.ajax({
				url: "/api/settings/raid_management/logical_initialize_check_cmd",
				type: "PUT",
				dataType: "json",
				data:object,
				processData: false,
				contentType: "application/json",
				success: function(data, status, xhr) {					
					alert(locale.initialization_success);
					Backbone.history.loadUrl(Backbone.history.fragment);
				},
				error : function(xhr, status, err) {
					app.HTTPErrorHandler(xhr, status, err);
				}
			});
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

        serialize: function() {
            return {
                locale: locale,
                model: this.model
            };
        }

    });

    return view;

});
