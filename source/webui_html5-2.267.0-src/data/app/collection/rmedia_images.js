define(["jquery", "underscore", "backbone", "models//rmedia_img"],

function($, _, Backbone, RmediaImgModel) {

    var collection = Backbone.Collection.extend({

        url: function() {
            return "/api/settings/media/remote/images"
        },

        model: RmediaImgModel

    });

    return new collection();

});
