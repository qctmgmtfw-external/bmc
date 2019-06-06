define(['jquery', 'underscore', 'backbone',
        'collection/usertasks',
        'views/UserTaskView',
        'i18n!strings/nls/common',
        'text!templates/navbar-usertasks.html',
        //plugins
        "dropdown","slimscroll"
    ],
    function($, _, Backbone, UserTaskCollection, UserTaskView, CommonStrings, HolderTemplate) {

        var view = Backbone.View.extend({

        	tagName: "li",

            template: _.template(HolderTemplate),

            initialize: function() {

                this.collection = UserTaskCollection;

                this.collection.on("add", this.addUserTask, this);
                this.collection.on("all add reset remove", this.updateCount, this);

            },

            beforeRender: function() {
                this.$el.addClass("dropdown tasks-menu");
            	//this.collection.fetch();
                //this.$(".label-warning").hide();
            },

            afterRender: function() {
                this.$("[data-toggle='dropdown']").dropdown();
            	this.collection.reset();
            },

            addUserTask: function(model) {
                this.insertView(".menu", new UserTaskView({
                    model: model
                })).render(); //.$el.appendTo(this.$(".menu"));
            },

            updateCount: function(model, collection, options) {
                this.$(".count").html(this.collection.length);
                if (this.collection.length > 0) {
                    this.$(".label-danger").show();
                } else {
                    this.$(".label-danger").hide();
                }
                this.$(".menu").slimscroll({
                    height: "200px",
                    alwaysVisible: false,
                    size: "3px"
                }).css("width", "100%");
            },

            serialize: {
                locale: CommonStrings
            }

        });

        return view;

    });
