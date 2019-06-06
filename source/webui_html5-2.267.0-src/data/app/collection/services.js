define(["jquery", "underscore", "backbone", "models//service"],

function($, _, Backbone, ServiceModel) {

    var collection = Backbone.Collection.extend({

        url: function() {
            return "/api/settings/services"
        },

        model: ServiceModel

    });

    return new collection();

});
