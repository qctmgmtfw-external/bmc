define(["jquery", "underscore", "backbone", "app", "i18n!strings/nls/pre_event"],

function($, _, Backbone, app, locale) {

    var model = Backbone.Model.extend({

        defaults: {},

        validation: {

        },

        url: function() {
            return "/api/settings/video/pre-event"
        }
    });

    return new model();

});
