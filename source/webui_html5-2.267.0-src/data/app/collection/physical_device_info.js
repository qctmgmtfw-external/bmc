define(["jquery", "underscore", "backbone", "models//physical_device_info"],

function($, _, Backbone, PhysicalDeviceInfoModel) {

    var collection = Backbone.Collection.extend({

        url: function() {
            return "/api/settings/raid_management/physical_device_info"
        },
        byRAIDName: function(name,limit){
                var filtered = this.filter(function(model) {
                	var sname=model.get("controller_name");
					if(sname== name)
                		return (sname== name);
                });
		        if(limit) {
			        filtered = filtered.slice(0,limit);
		        }
                return new collection(filtered);
           },

        model: PhysicalDeviceInfoModel

    });

    return new collection();

});
