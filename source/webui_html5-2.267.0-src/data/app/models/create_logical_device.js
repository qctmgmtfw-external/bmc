define(["jquery", "underscore", "backbone", "app", "i18n!strings/nls/create_logical_device"],

function($, _, Backbone, app, locale) {

    var model = Backbone.Model.extend({

        defaults: {},

        validation: {

        },

        url: "/api/settings/raid_management/create_virtual_drive"
    });

    return new model();

});
