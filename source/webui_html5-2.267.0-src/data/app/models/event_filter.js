define(["jquery", "underscore", "backbone", "app", "i18n!strings/nls/event_filters"],

function($, _, Backbone, app, locale) {

    var model = Backbone.Model.extend({

        defaults: {},

        validation: {
        	policy_group: {
                required: true,
                msg: locale.required_policy_group
            },
            sensor_type: {
                required: true,
                msg: locale.required_sensor_type
            }
        }
    });

    return model;

});
