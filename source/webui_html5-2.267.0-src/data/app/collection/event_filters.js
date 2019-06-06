define(["jquery", "underscore", "backbone", "models//event_filter"],

function($, _, Backbone, EventFilterModel) {

    var collection = Backbone.Collection.extend({

        url: function() {
            return "/api/settings/pef/event_filters"
        },

        model: EventFilterModel

    });

    return new collection();

});
