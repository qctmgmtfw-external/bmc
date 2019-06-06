define(["jquery", "underscore", "backbone", "app", "i18n!strings/nls/bond"],

function($, _, Backbone, app, locale) {

    var model = Backbone.Model.extend({

        defaults: {},

        validation: {

        },

        url: "/api/settings/network-bond"
    });

    return new model();

});
