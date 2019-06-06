define(["jquery", "underscore", "backbone", "models//lan_user"],

function($, _, Backbone, LanUserModel) {

    var collection = Backbone.Collection.extend({

        url: function() {
            return "/api/settings/users_read_only"
        },

        model: LanUserModel

    });

    return new collection();

});
