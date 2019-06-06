define(['jquery', 'underscore', 'backbone', 'app',
//data
'collection//event_filters',
//views
'views/settings/pef/event_filters_item',
//localize
'i18n!strings/nls/event_filters',
//template
'text!templates/settings/pef/event_filters.html'],

function($, _, Backbone, app, EventFiltersCollection, EventFiltersItemView, locale, EventFiltersTemplate) {

    var view = Backbone.View.extend({

        template: _.template(EventFiltersTemplate),

        initialize: function() {
            this.event_filters = EventFiltersCollection;
            this.collection = this.event_filters;
            this.event_filters.bind('add', this.add, this);
            this.event_filters.bind('reset', this.reset, this);
            this.event_filters.bind('remove', this.remove, this);
            this.event_filters.bind('change', this.change, this);
            this.event_filters.bind('change add reset remove', this.affect, this);
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
            }, this);
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
        add: function(model, collection, options) {
            var itemView = new EventFiltersItemView({
                model: model
            });
            //itemView.$el.appendTo(".list");
            itemView.$el.insertBefore(this.$el.find(".list .list-add-item"));
            itemView.render();


        },

        reset: function(model, collection, options) {

        },

        remove: function(model, collection, options) {

        },

        change: function(model, collection, options) {

        },

        affect: function(model, collection, options) {

        },

        reload: function(model, value) {

        },



        serialize: function() {
            return {
                locale: locale
            };
        }

    });

    return view;

});
