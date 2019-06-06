define(['jquery', 'underscore', 'backbone', 'app',
//data
'collection//raid_logical_physical_device_info',
'collection//logical_device_info',
//views
'views/settings/raid_management/raid_logical_physical_device_info_item',
//localize
'i18n!strings/nls/raid_logical_physical_device_info',
//template
'text!templates/settings/raid_management/raid_logical_physical_device_info.html',
//plugin
'footable'],

function($, _, Backbone, app, RaidLogicalPhysicalDeviceInfoCollection,RaidLogicalInfoCollection,RaidLogicalPhysicalDeviceInfoItemView, locale, RaidLogicalPhysicalDeviceInfoTemplate) {

    var view = Backbone.View.extend({

        template: _.template(RaidLogicalPhysicalDeviceInfoTemplate),
		PHYSICAL_INFO:{},
		LOGICAL_INFO:{},
		selectedElementsData:"",
        initialize: function() {
			var id = location.hash.split('?').pop();
			var da=id.split("&");			
			this.ctrl_id=da[0].split("=")[1];
			this.dev_id=da[1].split("=")[1];
			RaidLogicalInfoCollection.fetch({
                async: false
            });
            RaidLogicalPhysicalDeviceInfoCollection.fetch({
                async: false
            });
            this.raid_logical_physical_device_info = RaidLogicalPhysicalDeviceInfoCollection;
            this.collection = this.raid_logical_physical_device_info;
            this.raid_logical_physical_device_info.bind('add', this.add, this);
            this.raid_logical_physical_device_info.bind('reset', this.reset, this);
            this.raid_logical_physical_device_info.bind('remove', this.removeModel, this);
            this.raid_logical_physical_device_info.bind('change', this.change, this);
            this.raid_logical_physical_device_info.bind('change add reset remove', this.affect, this);
        },

        events: {
        	"click .help-link": "toggleHelp"
        },

        beforeRender: function() {

        },

        afterRender: function() {
           
			$('table.footable tbody tr').hide();
			var that=this;
			//RaidLogicalInfoCollection.fetch({success: function(){ 
				//this.LOGICAL_INFO=RaidLogicalInfoCollection;				
				//console.log("first");
				//console.log("RaidLogicalInfoCollection"+ RaidLogicalInfoCollection.length);
				if(RaidLogicalInfoCollection.length){
				RaidLogicalInfoCollection.each(function(model){					
					if(that.ctrl_id==model.get("controller_id") && that.dev_id== model.get("dev_id")){
						//console.log("inside selectedElementsData");
						that.selectedElementsData= model.get("elements").split('~');						
					}
				}, this);
				}
				//that.bindLogicalPhysicalPopupData.call(that);
			//}});
				that.bindLogicalPhysicalPopupData.call(that);
			
			/*
            var _parent = this;
            this.collection.each(function(model) {
                _parent.add(model, this.collection);
                _parent.affect(model, this.collection);
            }, this);
            this.$('.footable').footable();*/
        },
		
		bindLogicalPhysicalPopupData:function() {
		var that=this;
			//RaidLogicalPhysicalDeviceInfoCollection.fetch({success: function(){ 
				//that.PHYSICAL_INFO=RaidLogicalPhysicalDeviceInfoCollection;	
				//console.log("RaidLogicalPhysicalDeviceInfoCollection"+RaidLogicalPhysicalDeviceInfoCollection);
				if(RaidLogicalPhysicalDeviceInfoCollection.length > 0){
				RaidLogicalPhysicalDeviceInfoCollection.each(function(model){	
					//console.log("seleccted data" +that.selectedElementsData);
					var tagdata=that.selectedElementsData;					
					for(var k=0;k<tagdata.length-1;k++) {
						if(tagdata[k] != "") {
							var elData=tagdata[k].split(',');
							var dev_id=elData[0].replace("(","");
							var element_type=elData[1].replace(")","");							
							
							if(that.ctrl_id==model.get("controller_id") && dev_id== model.get("dev_id")){
								var selected=dev_id;
								$('table.footable tbody tr:contains("' + selected + '")').show();
								var _parent = that;
								_parent.add(model, RaidLogicalPhysicalDeviceInfoCollection);
								_parent.affect(model, RaidLogicalPhysicalDeviceInfoCollection);
							}
						}
					}
				}, this);
				}
			//}});			
			
			this.$('.footable').footable();
		},
        add: function(model, collection, options) {
            var itemView = new RaidLogicalPhysicalDeviceInfoItemView({
                model: model
            });
            itemView.$el.appendTo(".table-body");
            //itemView.$el.insertBefore(this.$el.find(".list .list-add-item"));
            itemView.render();
			//console.log("this is in add");
			/*	var selected=model.get("dev_id");
				//console.log("in add table " +selected);
				$('table.footable tbody tr').hide();
				$('table.footable tbody tr:contains("' + selected + '")').show();*/

            this.$('.footable').trigger('footable_redraw');
        },

        reset: function(model, collection, options) {

            this.$('.footable').trigger('footable_redraw');
        },

        removeModel: function(model, collection, options) {

            this.$('.footable').trigger('footable_redraw');
        },

        change: function(model, collection, options) {

            this.$('.footable').trigger('footable_redraw');
        },

        affect: function(model, collection, options) {

        },

        reload: function(model, value) {

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
                locale: locale
            };
        }

    });

    return view;

});
