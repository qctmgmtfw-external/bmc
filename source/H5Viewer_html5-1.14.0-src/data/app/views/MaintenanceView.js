define(['jquery', 'underscore', 'backbone',
        //data

        //localize
        'i18n!strings/nls/common',
        //template
        'text!templates/maintenance.html'
        //plugins
        // 'moment',
        // 'knob',
        // 'sparkline'
    ],

    function($, _, Backbone, CommonStrings, MaintenanceTemplate) {

        var view = Backbone.View.extend({

            template: _.template(MaintenanceTemplate),

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