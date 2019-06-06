define(['jquery', 'underscore', 'backbone', 'app',
//data
'collection//rmedia_cfg',
'collection/rmedia_images',
//views
'views/image_redirection/rmedia_item',
//localize
'i18n!strings/nls/rmedia',
//template
'text!templates/image_redirection/rmedia.html',
//plugin
'footable'],

function($, _, Backbone, app, RmediaCfgCollection, RMediaImageCollection, RmediaItemView, locale, RmediaTemplate) {

    var view = Backbone.View.extend({

        template: _.template(RmediaTemplate),

        initialize: function() {
            this.rmedia_cfg = RmediaCfgCollection;
            this.collection = this.rmedia_cfg;
            this.rmedia_cfg.bind('add', this.add, this);
            this.rmedia_cfg.bind('reset', this.reset, this);
            this.rmedia_cfg.bind('remove', this.removeModel, this);
            this.rmedia_cfg.bind('change', this.change, this);
            this.rmedia_cfg.bind('change add reset remove', this.affect, this);
        },

        events: {

        },

        beforeRender: function() {

        },

        afterRender: function() {
        	if(sessionStorage.privilege_id < CONSTANTS["ADMIN"]){
                app.disableAllbutton();  
                app.disableAllinput();  
             }
            this.collection.reset();
            this.collection.fetch();
            var _parent = this;
            this.collection.each(function(model) {
                _parent.add(model, this.collection);
                _parent.affect(model, this.collection);
            }, this);
            RMediaImageCollection.fetch();
            this.$('.footable').footable({pageSize: 12});
            $(".footable").attr("data-sort", false);
            $(".footable-sort-indicator").hide();
            this.poll.call(this);
        },

        add: function(model, collection, options) {
            var itemView = new RmediaItemView({
                model: model,
                collection: RMediaImageCollection
            });
            itemView.$el.appendTo(".table-body");
            //itemView.$el.insertBefore(this.$el.find(".list .list-add-item"));
            itemView.render();



            this.$('.footable').trigger('footable_redraw');
        },

        reset: function(model, collection, options) {

            this.$('.footable').trigger('footable_redraw');
        },

        removeModel: function(model, collection, options) {

            this.$('.footable').trigger('footable_redraw');
        },

        change: function(model, collection, options) {

            this.$('.footable').trigger('footable_redraw');
        },

        affect: function(model, collection, options) {

        },

        poll: function() {
            var context = this;
            this.timer = setInterval(function(){
                // passing parameter global:false to avoid loading screen 
                //RMediaImageCollection.fetch({global:false});
                context.collection.fetch({
                    global:false,
                    success: function() {
                        console.log("success");
                        if(!$('.alert-danger').hasClass("hide")){
                            console.log("add class");
                            $('.alert-danger').addClass("hide");
                        }
                        //context.afterRender();
                        if($(".table-body").html().length == 0){
                            var _parent = context;
                            context.collection.each(function(model) {
                                _parent.add(model, context.collection);
                                _parent.affect(model, context.collection);
                            }, context);
                        }
                        RMediaImageCollection.fetch({global:false});
                    },
                    error: function(){
                        $(".table-body").html("");
                    }
                });
            }, 5000);
        },

        stopPoll: function() {
            clearInterval(this.timer);
        },

        serialize: function() {
            return {
                locale: locale
            };
        }

    });

    return view;

});
