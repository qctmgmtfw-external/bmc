define(["jquery", "underscore", "backbone", "app", "i18n!strings/nls/sol_trigger_settings"],

function($, _, Backbone, app, locale) {

    var model = Backbone.Model.extend({

        defaults: {},

        validation: {
            date_time_to_record: {
                fn: function(value, attr, computedState) {
                    if (computedState.date_time_events_enable == 1) {
                        return value <= computedState.NTPtimestamp;
                    } else {
                        return false;
                    }
                },
                msg: locale.recording_time_must_be_in_future
            }
        },

        url: function() {
            return "/api/settings/video/sol-triggers"
        }
    });

    return new model();

});
