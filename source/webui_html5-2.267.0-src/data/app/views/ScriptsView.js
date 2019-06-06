define(['jquery', 'underscore', 'backbone',
        //data
        'collection/scripts',
        'views/ScriptItemView',

        //localize
        'i18n!strings/nls/common',
        //template
        'text!templates/scripts.html'
        //plugins
        // 'moment',
        // 'knob',
        // 'sparkline'
    ],

    function($, _, Backbone, ScriptCollection, ScriptItemView, CommonStrings, ScriptsTemplate) {

        var view = Backbone.View.extend({

            template: _.template(ScriptsTemplate),

            initialize: function() {
                this.collection = ScriptCollection;
                this.collection.bind("add", this.load, this);
            },

            events: {},

            beforeRender: function() {

            },

            afterRender: function() {
                this.collection.fetch();

                this.collection.each(function(model){
                    this.load(model);
                }, this);
            },

            load: function(model, collection, xhr) {
                this.insertView(".scripts-list", new ScriptItemView({model: model})).render();
            },

            serialize: function() {
                return {
                    locale: CommonStrings
                };
            }

        });

        return view;

    });