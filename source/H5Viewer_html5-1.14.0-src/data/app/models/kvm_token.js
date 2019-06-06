define(["jquery", "underscore", "backbone", "i18n!strings/nls/common"],

    function($, _, Backbone, locale) {

        var model = Backbone.Model.extend({

            url: "/api/kvm/token",

            defaults: {},

            validation: {

            }
        });

        return model;

    });