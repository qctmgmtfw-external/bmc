define(['jquery', 'underscore', 'backbone',
        //data

        //localize
        'i18n!strings/nls/store',
        //template
        'text!templates/store.html'
        //plugins
        // 'moment',
        // 'knob',
        // 'sparkline'
    ],

    function($, _, Backbone, CommonStrings, StoreTemplate) {

        var view = Backbone.View.extend({

            template: _.template(StoreTemplate),

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