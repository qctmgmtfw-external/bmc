define(['jquery', 'underscore', 'backbone',
        //data

        //localize
        'i18n!strings/nls/updates',
        //template
        'text!templates/updates.html'
        //plugins
        // 'moment',
        // 'knob',
        // 'sparkline'
    ],

    function($, _, Backbone, CommonStrings, UpdatesTemplate) {

        var view = Backbone.View.extend({

            template: _.template(UpdatesTemplate),

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