define(['jquery', 'underscore', 'backbone', 'app',
//localize
'i18n!strings/nls/sasit_topology_info',
//template
'text!templates/settings/sasit_management/sasit_topology_info_item.html'],

function($, _, Backbone, app, locale, SasitTopologyInfoTemplate) {

    var view = Backbone.View.extend({

        tagName: "tr",

        template: _.template(SasitTopologyInfoTemplate),

        initialize: function() {
            this.model.bind("destroy", this.removeItem, this);
        },

        events: {
            "click .delete": "deleteItem",
            "click .help-link": "toggleHelp"
        },

        beforeRender: function() {},

        afterRender: function() {
            this.$('.edit').removeClass('hide');
            var href = this.$el.find('a.edit').attr('href');
            this.$el.find('a.edit').attr('href', href.replace(':id', this.model.get('id')));
            this.$('.delete').removeClass('hide');
        },

        removeItem: function() {
            this.$el.remove();
        },

        deleteItem: function(ev) {
            ev.stopPropagation();
            ev.preventDefault();
            if (!confirm('Are you sure to perform this operation?')) return;
            this.model.destroy({
                url: this.model.collection.url().split('?')[0] + '/' + this.model.get('id')
            });
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
