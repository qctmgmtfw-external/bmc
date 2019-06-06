define(["jquery", "underscore", "backbone", "models//user"],

function($, _, Backbone, UserModel) {

    var collection = Backbone.Collection.extend({

        url: function() {
            return "/api/settings/users"
        },

        model: UserModel

    });

    return new collection();

});
