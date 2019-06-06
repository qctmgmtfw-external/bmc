define(['jquery', 'underscore', 'backbone', 'app',
//localize
'i18n!strings/nls/event_filters',
//template
'text!templates/settings/pef/event_filters_item.html'],

function($, _, Backbone, app, locale, EventFiltersTemplate) {

    var view = Backbone.View.extend({

        template: _.template(EventFiltersTemplate),

        initialize: function() {},

        events: {
            "click .help-link": "toggleHelp"
        },

        beforeRender: function() {},

        afterRender: function() {
            var href = this.$el.find('a').attr('href');
            this.$el.find('a').attr('href', href.replace(':id', this.model.get('id')));
        },

        toggleHelp: function(e) {
            e.preventDefault();
            var help_items = this.$('.help-item').filter(function() {
                var f = $(this).data('feature');
                if (f) {
                    return (app.features.indexOf(f) == -1 ? false : true);
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
                locale: locale,
                model: this.model
            };
        }

    });

    return view;

});
