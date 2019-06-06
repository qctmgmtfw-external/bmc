define(["jquery", "underscore", "backbone", "app", "i18n!strings/nls/raid_event_log"],

function($, _, Backbone, app, locale) {

    var model = Backbone.Model.extend({

        defaults: {},

        validation: {

        }
    });

    return model;

});
