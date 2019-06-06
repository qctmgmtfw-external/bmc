define([
        'jquery', 'underscore', 'backbone',
        'models/log'
    ],
    function($, _, Backbone, EventLogModel) {

        var collection = Backbone.Collection.extend({

            url: "/api/logs/event",

            model: EventLogModel,

            comparator: function(model) {
                return ((new Date().getTime() / 1000) - model.get('timestamp'));
            },

            byDateRange: function(start, end) {

                if (!end) {
                    end = Math.floor(new Date().getTime()/1000);
                }

                //make end date till midnight
                end = end + 86399; //one day seconds count

                var filtered = this.filter(function(model) {
                    var timestamp = parseInt(model.get("timestamp"), 10);
                    return (timestamp >= start && timestamp <= end);
                });

                return new collection(filtered);

            },

            bySensor: function(sensor_id,limit){

                var filtered = this.filter(function(model) {
                	var sid = parseInt(model.get("sensor_number"), 10);
                	if (sid == sensor_id)
                            return (sid == sensor_id);
                });

		        if(limit) {
			        filtered = filtered.slice(0,limit);
		        }

                return new collection(filtered);
           },
           
           bydeassertion: function(){
               var filtered = this.filter(function(model) {
                   var event_direction = model.get("event_direction");
                   return (event_direction == "deasserted");
                   
               });
               return new collection(filtered);
          }

        });

        return new collection();

    });
