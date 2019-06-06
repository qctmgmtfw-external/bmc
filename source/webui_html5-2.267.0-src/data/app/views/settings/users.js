define(['jquery', 'underscore', 'backbone', 'app',
//data
'collection/notifications',
'models/notification',
'collection//users',
//views
'views/settings/users_item',
//localize
'i18n!strings/nls/users',
//template
'text!templates/settings/users.html'],

function($, _, Backbone, app, NotificationCollection, NotificationModel, UsersCollection, UsersItemView, locale, UsersTemplate) {

    var view = Backbone.View.extend({

        template: _.template(UsersTemplate),

        initialize: function() {

            if(sessionStorage.privilege_id < CONSTANTS["OPERATOR"]){
                NotificationCollection.add(new NotificationModel({
                    id: 8000+(new Date()).getTime(),
                    message_group: "failure",
                    message: "Insufficient Privilege \n [code:8000]",
                    severity: "danger"
                }));
                app.hidePage();
            }
            
            this.users = UsersCollection;
            this.collection = this.users;
            this.users.bind('add', this.add, this);
            this.users.bind('reset', this.reset, this);
            this.users.bind('remove', this.removeModel, this);
            this.users.bind('change', this.change, this);
            this.users.bind('change add reset remove', this.affect, this);
        },

        events: {
            "click .help-link": "toggleHelp"
        },

        beforeRender: function() {

        },

        afterRender: function() {

            

            this.collection.reset();
            if(!(sessionStorage.privilege_id < CONSTANTS["OPERATOR"])){
                this.collection.fetch();
            }
            var _parent = this;
            this.collection.each(function(model) {
                _parent.add(model, this.collection);
            }, this);
        },

        add: function(model, collection, options) {
            var itemView = new UsersItemView({
                model: model
            });
            //itemView.$el.appendTo(".list");
            itemView.$el.insertBefore(this.$el.find(".list .list-add-item"));
            itemView.render();


        },

        reset: function(model, collection, options) {

        },

        removeModel: function(model, collection, options) {

        },

        change: function(model, collection, options) {

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
