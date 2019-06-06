define(['jquery', 'underscore', 'backbone', 'app',
//data
'collection//lan_destinations',
//views
'views/settings/pef/lan_destinations_item',
//localize
'i18n!strings/nls/lan_destinations',
//template
'text!templates/settings/pef/lan_destinations.html'],

function($, _, Backbone, app, LanDestinationsCollection, LanDestinationsItemView, locale, LanDestinationsTemplate) {

    var view = Backbone.View.extend({

        template: _.template(LanDestinationsTemplate),

        initialize: function() {
            this.lan_destinations = LanDestinationsCollection;
            this.collection = this.lan_destinations;
            this.lan_destinations.bind('add', this.add, this);
            this.lan_destinations.bind('reset', this.reset, this);
            this.lan_destinations.bind('remove', this.removeModel, this);
            this.lan_destinations.bind('change', this.change, this);
            this.lan_destinations.bind('change add reset remove', this.affect, this);
        },

        events: {
            "change #idgroup_by_lan_channel": "groupChanged_group_by_lan_channel",
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
            var itemView = new LanDestinationsItemView({
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
            var group_models = this.collection.groupBy(function(model) {
                return model.get('lan_channel');
            });
            this.$('#idgroup_by_lan_channel').html('');
            for (i in group_models) {
                this.$('#idgroup_by_lan_channel').append("<option value='" + i + "'>" + i + "</option>");
                this.$('#idgroup_by_lan_channel').change();
            }
        },

        reload: function(model, value) {

        },

        groupChanged_group_by_lan_channel: function(e) {
            var selected = $(e.currentTarget).val();
            if (selected == 'all') {
                this.$('div.list-item').show();
                return;
            }
            this.$('div.list-item').hide();
            $('.list-item').each(function() {
                var label_val = $(this).find('.lan_channel').text();
                if (label_val != null && label_val != undefined && label_val != '') {
                    if (label_val.indexOf(selected) > -1) {
                        $(this).show();
                    }
                }
            });
        },
       
        serialize: function() {
            return {
                locale: locale
            };
        }

    });

    return view;

});
