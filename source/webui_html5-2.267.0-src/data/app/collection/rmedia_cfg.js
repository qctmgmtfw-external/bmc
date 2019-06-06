define(["jquery", "underscore", "backbone", "models//rmedia_cfg"],

function($, _, Backbone, RmediaCfgModel) {

    var collection = Backbone.Collection.extend({

        url: function() {
            return "/api/settings/media/remote/configurations"
        },

        model: RmediaCfgModel

    });

    return new collection();

});
