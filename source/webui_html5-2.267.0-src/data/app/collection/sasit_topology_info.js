define(["jquery", "underscore", "backbone", "models//sasit_topology_info"],

function($, _, Backbone, SasitTopologyInfoModel) {

    var collection = Backbone.Collection.extend({

        url: function() {
            return "/api/settings/sasit_management/sasit_topology_info"
        },

        model: SasitTopologyInfoModel

    });

    return new collection();

});
