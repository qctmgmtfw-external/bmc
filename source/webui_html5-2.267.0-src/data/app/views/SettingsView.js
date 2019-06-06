define(['jquery', 'underscore', 'backbone',
        //data

        //localize
        'i18n!strings/nls/settings',
        //template
        'text!templates/settings.html'
        //plugins
        // 'moment',
        // 'knob',
        // 'sparkline'
    ],

    function($, _, Backbone, CommonStrings, SettingsTemplate) {

        var view = Backbone.View.extend({

            template: _.template(SettingsTemplate),

            initialize: function() {

            },

            events: {},

            beforeRender: function() {

            },

            afterRender: function() {

            },

            load: function(model, collection, xhr) {

            },

            serialize: function() {
                return {
                    locale: CommonStrings
                };
            }

        });

        return view;

    });