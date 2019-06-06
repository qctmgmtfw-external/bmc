define(["jquery", "underscore", "backbone", "models//sol_recorded_video"],

function($, _, Backbone, SolRecordedVideoModel) {

    var collection = Backbone.Collection.extend({

        url: function() {
            return "/api/settings/sol-recorded-video"
        },

        model: SolRecordedVideoModel

    });

    return new collection();

});
