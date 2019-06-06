define(["jquery", "underscore", "backbone", "models//ip"],

function($, _, Backbone, IpModel) {

    var collection = Backbone.Collection.extend({

        url: function() {
            return "/api/settings/network"
        },

        model: IpModel

    });

    return new collection();

});
