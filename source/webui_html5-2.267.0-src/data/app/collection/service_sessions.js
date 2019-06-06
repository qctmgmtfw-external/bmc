define(["jquery", "underscore", "backbone", "models//service_session"],

function($, _, Backbone, ServiceSessionModel) {

    var collection = Backbone.Collection.extend({

        url: function() {
			
            return "/api/settings/service-sessions?service_id=" + location.hash.split('/').pop()
        },

        model: ServiceSessionModel

    });

    return new collection();

});
