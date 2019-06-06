define(["jquery", "underscore", "backbone", "models//smtp"],

function($, _, Backbone, SmtpModel) {

    var collection = Backbone.Collection.extend({

        url: function() {
            return "/api/settings/smtp"
        },

        model: SmtpModel

    });

    return new collection();

});
