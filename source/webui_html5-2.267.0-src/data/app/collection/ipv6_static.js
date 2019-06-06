define(["jquery", "underscore", "backbone", "models//ipv6_static"],

function($, _, Backbone, Ipv6StaticModel) {

    var collection = Backbone.Collection.extend({

        url: function() {
            return "/api/settings/static-ipv6"
        },

        model: Ipv6StaticModel

    });

    return new collection();

});
