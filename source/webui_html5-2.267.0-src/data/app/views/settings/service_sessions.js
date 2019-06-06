define(['jquery', 'underscore', 'backbone', 'app',
//data
'collection//service_sessions',
'collection//services',
//views
'views/settings/service_sessions_item',
//localize
'i18n!strings/nls/service_sessions',
//template
'text!templates/settings/service_sessions.html',
//plugin
'footable'],

function($, _, Backbone, app, ServiceSessionsCollection,ServicesCollection, ServiceSessionsItemView, locale, ServiceSessionsTemplate) {

    var view = Backbone.View.extend({

        template: _.template(ServiceSessionsTemplate),

        initialize: function() {
            this.service_sessions = ServiceSessionsCollection;
            this.collection = this.service_sessions;
            this.services = ServicesCollection;
            this.service_sessions.bind('add', this.add, this);
            this.service_sessions.bind('reset', this.reset, this);
            this.service_sessions.bind('remove', this.removeModel, this);
            this.service_sessions.bind('change', this.change, this);
            this.service_sessions.bind('change add reset remove', this.affect, this);
        },

        events: {
            "click .help-link": "toggleHelp"
        },

        beforeRender: function() {

        },

        afterRender: function() {
            this.collection.reset();
            this.collection.fetch();
            var _parent = this;
            this.collection.each(function(model) {
                _parent.add(model, this.collection);
                _parent.affect(model, this.collection);
            }, this);
            this.$('.footable').footable();
            /*this.$('.footable').footable({
                grid:{
                    "showEmptyInfo":"true",
                    "showIndex":"true",
                    "activeClass":"info"
                }
            });*/
            this.displayHeader();
        },
        displayHeader:function(){
            var id=parseInt(location.hash.split('/').pop());
            if(this.services.length > 0){
                 this.services.each(function(model) {
                   if(id == model.get("service_id")){
                    result= locale.desc_active_session + " - " + "<label class='classicFont'>"+ model.get("service_name").toUpperCase() + "</label>";
                    $('#lblActiveSessionHeader').html(result);
                   }
                }, this);
            }
        },
        add: function(model, collection, options) {
            var itemView = new ServiceSessionsItemView({
                model: model
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
        toggleHelp: function(e) {
            e.preventDefault();
            var help_items = this.$('.help-item').filter(function() {
                var f = $(this).data('feature');
                var nf = $(this).data('not-feature');
                if (f) {
                    return (app.features.indexOf(f) == -1 ? false : true);
                } else if (nf) {
                    return (app.features.indexOf(nf) == -1 ? true : false);
                } else {
                    return true;
                }
            });
            $(".tooltip").hide();
            if (help_items.hasClass('hide')) {
                help_items.removeClass('hide');
            } else {
                help_items.addClass('hide');
            }
        },
        serialize: function() {
            return {
                locale: locale
            };
        }

    });

    return view;

});
