define(['jquery', 'underscore', 'backbone', 'app',
//data
'collection//lmedia_cfg',
'collection/lmedia_images',
//views
'views/image_redirection/lmedia_item',
//localize
'i18n!strings/nls/lmedia',
//template
'text!templates/image_redirection/lmedia.html',
//plugin
'footable'],

function($, _, Backbone, app, LmediaCfgCollection, LMediaImageCollection, LmediaItemView, locale, LmediaTemplate) {

    var view = Backbone.View.extend({

        template: _.template(LmediaTemplate),

        initialize: function() {
            this.lmedia_cfg = LmediaCfgCollection;
            this.collection = this.lmedia_cfg;
            this.lmedia_cfg.bind('add', this.add, this);
            this.lmedia_cfg.bind('reset', this.reset, this);
            this.lmedia_cfg.bind('remove', this.removeModel, this);
            this.lmedia_cfg.bind('change', this.change, this);
            this.lmedia_cfg.bind('change add reset remove', this.affect, this);
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
            LMediaImageCollection.fetch();
            this.$('.footable').footable({pageSize: 12});
            $(".footable").attr("data-sort", false);
            $(".footable-sort-indicator").hide();
            this.poll.call(this);
        },

        add: function(model, collection, options) {
            var itemView = new LmediaItemView({
                model: model,
                collection: LMediaImageCollection,
                customObj: this
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

        reload: function(model, value) {

        },

        poll: function() {
            var context = this;
            this.timer = setInterval(function(){
                // passing parameter global:false to avoid loading screen 
                //LMediaImageCollection.fetch({global:false});
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
                        LMediaImageCollection.fetch({global:false});
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
