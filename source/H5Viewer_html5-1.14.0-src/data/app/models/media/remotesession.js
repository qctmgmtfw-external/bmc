define(["jquery", "underscore", "backbone", "app", "i18n!strings/nls/remotesession"],

function($, _, Backbone, app, locale) {

    var model = Backbone.Model.extend({

        defaults: {},

        validation: {
            retry_count: {
                pattern: /^\d+$/,
                min: 1,
                max: 6,
                msg: locale.retry_count_must_be_a_number_from_1_to_6
            },
            retry_time_interval: {
                pattern: /^\d+$/,
                min: 5,
                max: 30,
                msg: locale.retry_time_interval_must_be_a_number_from_5_to_30
            }
        },

        url: function() {
            return "/api/settings/media/remotesession"
        }
    });

    return new model();

});
