define(["jquery", "underscore", "backbone", "models//lan_destination"],

function($, _, Backbone, LanDestinationModel) {

    var collection = Backbone.Collection.extend({

        url: function() {
            return "/api/settings/pef/lan_destinations"
        },

        model: LanDestinationModel

    });

    return new collection();

});
