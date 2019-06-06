define(['jquery', 'underscore', 'backbone',
        'collection/messages',
        'views/MessageView',
        'i18n!strings/nls/common',
        'text!templates/navbar-messages.html',
        //plugins
        "dropdown", "slimscroll"
    ],
    function($, _, Backbone, MessageCollection, MessageView, CommonStrings, HolderTemplate) {

        var view = Backbone.View.extend({

        	tagName: "li",

            template: _.template(HolderTemplate),
            //el: '.messages-menu',

            initialize: function() {

                this.collection = MessageCollection;

                this.collection.on("add", this.addMessage, this);
                this.collection.on("all add reset remove", this.updateCount, this);

            },

            beforeRender: function() {
                this.$el.addClass("dropdown messages-menu");
            	//this.collection.fetch();
                //this.$(".label-warning").hide();
            },

            afterRender: function() {
                this.$("[data-toggle='dropdown']").dropdown();
            	this.collection.reset();
            },

            addMessage: function(model) {
                // this.insertView(".menu", new MessageView({
                //     model: model
                // })).render(); //.$el.appendTo(this.$(".menu"));
        		(new MessageView({model: model})).render().$el.prependTo(this.$(".menu"));
            },

            updateCount: function(model, collection, options) {
                this.$(".count").html(this.collection.length);
                //console.log("messages", this.collection.length);
                if (this.collection.length > 0) {
                    this.$(".label-success").show();
                } else {
                    this.$(".label-success").hide();
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
