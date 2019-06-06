define(['jquery', 'underscore', 'backbone', 'app',
//localize
'i18n!strings/nls/raid_logical_physical_device_info',
//template
'text!templates/settings/raid_management/raid_logical_physical_device_info_item.html'],

function($, _, Backbone, app, locale, RaidLogicalPhysicalDeviceInfoTemplate) {

    var view = Backbone.View.extend({

        tagName: "tr",

        template: _.template(RaidLogicalPhysicalDeviceInfoTemplate),

        initialize: function() {
            this.model.bind("destroy", this.removeItem, this);
        },

        events: {
            //"click .delete": "deleteItem",
            "click .help-link": "toggleHelp",
			"click .status": "doOfflineAndOnline"
        },
		doOfflineAndOnline:function(ev){
			
			  var ctlrid=this.model.get('controller_id');
			  //var ctrlName=$('#idgroup_by_controller_name').val();
			  
			  var devId=this.$('.status').data("devid");
			  var cmdpram=this.$('.status').data("status");
			  
			  ev.stopPropagation();
			  ev.preventDefault();
			  if (!confirm('Are you sure to perform this operation?')) return;
			  
			  
				var data = {};
				data.CMDPARAM=cmdpram;
				data.CTRLID=ctlrid;
				data.DEVICEID=devId;

			var object=JSON.stringify(data);
			  
			  //localStorage.setItem("ctrl_name", ctrlName);
			  
			  //console.log("cmdpram" +cmdpram);
			  //console.log("CTRLID" +ctlrid);
			  //console.log("DEVICEID" +devId);
			  
			  $.ajax({
					url: "/api/settings/raid_management/physical_onlineoffline_cmd",
					type: "PUT",
					dataType: "json",
					data:object,
					processData: false,
					contentType: "application/json",
					success: function(data, status, xhr) {					
						alert("Successfully done the Offline/Online of Physical Device");
						var path="settings/raid_management/logical_device_info";
						app.router.navigate(path, {
                                            trigger: true
                                       });
					},
					error : function(xhr, status, err) {
						app.HTTPErrorHandler(xhr, status, err);
					}
				});
		},
        beforeRender: function() {},

        afterRender: function() {
            //this.$('.edit').removeClass('hide');
            //var href = this.$el.find('a.edit').attr('href');
            //this.$el.find('a.edit').attr('href', href.replace(':id', this.model.get('id')));
            this.$('.status').removeClass('hide');
			this.$('.status').attr("data-devid",this.model.get('dev_id'));
			
			var state= this.model.get('state');
			if(state==4){
				this.$('.status').attr("data-status",1);
				this.$('.status').html("<i class='fa fa-circle text-success'></i>");
				this.$('.status').attr("title","Click to make Online");
			} else if(state==32){
				this.$('.status').attr("data-status",0);
				this.$('.status').html("<i class='fa fa-circle'></i>");
				this.$('.status').attr("title","Click to make Offline");
			} else{
                //this.$('.status').attr("data-status",0);
                this.$('.status').addClass("hide");
                //this.$('.status').attr("title","Click to make Offline");
            }
        },

        removeItem: function() {
            this.$el.remove();
        },

        deleteItem: function(ev) {
            ev.stopPropagation();
            ev.preventDefault();
            if (!confirm('Are you sure to perform this operation?')) return;
            this.model.destroy({
                url: this.model.collection.url().split('?')[0] + '/' + this.model.get('id')
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
