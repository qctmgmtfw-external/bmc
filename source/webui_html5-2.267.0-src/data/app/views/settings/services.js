define(['jquery', 'underscore', 'backbone', 'app',
        //data
        'collection//services',
        //views
        'views/settings/services_item',
        //localize
        'i18n!strings/nls/services',
        //template
        'text!templates/settings/services.html',
        //plugin
        'footable'
    ],

    function($, _, Backbone, app, ServicesCollection, ServicesItemView, locale, ServicesTemplate) {

        var view = Backbone.View.extend({

            template: _.template(ServicesTemplate),

            initialize: function() {
                this.services = ServicesCollection;
                this.collection = this.services;
                this.services.bind('add', this.add, this);
                this.services.bind('reset', this.reset, this);
                this.services.bind('remove', this.removeModel, this);
                this.services.bind('change', this.change, this);
                this.services.bind('change add reset remove', this.affect, this);
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

                if (app.isFeatureEnabled("SESSION_MANAGEMENT") == false) {
                    $('.footable').find("#active_header").removeAttr("data-hide");
                    $('.footable').find("#active_header").removeAttr("data-ignore");
                }

                this.$('.footable').footable();
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
                var itemView = new ServicesItemView({
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



            serialize: function() {
                return {
                    locale: locale
                };
            }

        });

        return view;

    });
