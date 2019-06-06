define(["jquery", "underscore", "backbone", "app", "i18n!strings/nls/dual_image_config"],

function($, _, Backbone, app, locale) {

    var model = Backbone.Model.extend({

        defaults: {},

        validation: {

        },

        url: function() {
            return "/api/maintenance/dual_image_config"
        }
    });

    return new model();

});
