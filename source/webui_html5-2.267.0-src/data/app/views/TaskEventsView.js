define(['jquery', 'underscore', 'backbone',
        //data
        'collection/tasks',
        'views/TaskItemView',
        //localize
        'i18n!strings/nls/tasks',
        //template
        'text!templates/tasks.html'
        //plugins
        // 'moment',
        // 'knob',
        // 'sparkline'
    ],

    function($, _, Backbone, TaskCollection, TaskItemView, CommonStrings, TaskTemplate) {

        var view = Backbone.View.extend({

            template: _.template(TaskTemplate),

            initialize: function() {
                this.collection = TaskCollection;

                this.collection.bind("add", this.load, this);
            },

            events: {},

            beforeRender: function() {

            },

            afterRender: function() {
                console.log("after render task events");
                this.collection.fetch({
                    success: function(){
                        console.log("loaded tasks");
                    }
                });

                this.collection.each(function(model){
                    this.load(model);
                }, this);
            },

            load: function(model, collection, xhr) {
                console.log(model);
                this.insertView(".tasks-list", new TaskItemView({model: model})).render();
            },

            serialize: function() {
                return {
                    locale: CommonStrings
                };
            }

        });

        return view;

    });