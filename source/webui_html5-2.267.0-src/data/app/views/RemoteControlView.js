define(['jquery', 'underscore', 'backbone', 'app',
        //data
        'i18n!strings/nls/remote_control',
        //template
        'text!templates/remote-blank.html',

    ],

    function($, _, Backbone, app, CommonStrings, RemoteTemplate) {

        var view = Backbone.View.extend({

            template: _.template(RemoteTemplate),

            initialize: function() {
                
            },

            events: {
            },

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