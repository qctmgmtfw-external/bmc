define(["jquery", "underscore", "backbone", "app", "i18n!strings/nls/dns"],

function($, _, Backbone, app, locale) {

    var model = Backbone.Model.extend({

        defaults: {},

        validation: {
            domain_name: {
                pattern: '/^[a-zA-Z0-9]+(\.[a-zA-Z0-9]+)+$/',
                maxLength: 256,
                msg: locale.domain_name_must_consist_of_two_or_more_alphanumeric_labels_separated_by_periods_eg_subdomainexamplecom_the_total_length_of_the_domain_name_string_must_be_less_than_256_characters
            }
        }
    });

    return model;

});
