define(["jquery", "underscore", "backbone", "i18n!strings/nls/common"],

    function($, _, Backbone, locale) {

        var model = Backbone.Model.extend({

            url: "/api/settings/media/adviser",

            defaults: {},

            validation: {

            }
        });

        return model;

    });