define(["jquery", "underscore", "backbone", "models//network_interface"],

function($, _, Backbone, NetworkInterfaceModel) {

    var collection = Backbone.Collection.extend({

        url: function() {
            return "/api/settings/network-interfaces"
        },

        model: NetworkInterfaceModel

    });

    return new collection();

});
