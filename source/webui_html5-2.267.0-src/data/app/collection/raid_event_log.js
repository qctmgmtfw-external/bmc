define(["jquery", "underscore", "backbone", "models//raid_event_log"],

function($, _, Backbone, RaidEventLogModel) {

    var collection = Backbone.Collection.extend({

        url: function() {
            return "/api/settings/raid_management/raid_event_log"
        },
        byRAIDName: function(name,limit){
                var filtered = this.filter(function(model) {
                	var sname=model.get("controller_name");
					if(sname== name)
                		return (sname== name);
                });
		        /*if(limit) {
			        filtered = filtered.slice(0,limit);
		        }*/
                return new collection(filtered);
        },
        byRAIDNameAndEventType: function(name,event_type,limit){
                var filtered = this.filter(function(model) {
                    var sname=model.get("controller_name");
                    var eve_type=model.get("event_type");
                    if(eve_type=='all'){
                        if(sname== name)
                        return (sname== name);    
                    } else {
                        if(sname== name && eve_type== event_type)
                        return (sname== name && eve_type== event_type);    
                    }
                });
                /*if(limit) {
                    filtered = filtered.slice(0,limit);
                }*/
                return new collection(filtered);
        },
        model: RaidEventLogModel

    });

    return new collection();

});
