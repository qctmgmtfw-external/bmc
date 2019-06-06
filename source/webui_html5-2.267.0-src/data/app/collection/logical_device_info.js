define(["jquery", "underscore", "backbone", "models//logical_device_info"],

function($, _, Backbone, LogicalDeviceInfoModel) {

    var collection = Backbone.Collection.extend({

        url: function() {
            return "/api/settings/raid_management/logical_device_info"
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
        model: LogicalDeviceInfoModel

    });

    return new collection();

});
