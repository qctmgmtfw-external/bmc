define(["jquery", "underscore", "backbone", "models//lmedia_cfg"],

function($, _, Backbone, LmediaCfgModel) {

    var collection = Backbone.Collection.extend({

        url: function() {
            return "/api/settings/media/local/configurations"
        },

        model: LmediaCfgModel

    });

    return new collection();

});
