define([
        "jquery", "underscore", "backbone",
        "models/video-log"
    ],

    function($, _, Backbone, VideoLogModel) {

        var collection = Backbone.Collection.extend({

            url: "/api/logs/video",

            model: VideoLogModel,

            comparator: function(model) {
                return ((new Date().getTime() / 1000) - model.get('timestamp'));
            },

            byDateRange: function(start, end) {

                if (!end) {
                    end = start;
                }

                //make end date till midnight
                end = end + 86399; //one day seconds count

                var filtered = this.filter(function(model) {
                    var timestamp = parseInt(model.get("timestamp"), 10);
                    //console.log(timestamp, start, end);
                    return (timestamp >= start && timestamp <= end);
                });

                return new collection(filtered);

            }

        });

        return new collection();

    });