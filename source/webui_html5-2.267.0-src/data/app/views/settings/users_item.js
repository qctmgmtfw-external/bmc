define(['jquery', 'underscore', 'backbone', 'app',
//localize
'i18n!strings/nls/users',
//template
'text!templates/settings/users_item.html'],

function($, _, Backbone, app, locale, UsersTemplate) {

    var view = Backbone.View.extend({

        template: _.template(UsersTemplate),

        initialize: function() {},

        events: {
            "click .help-link": "toggleHelp"
        },

        beforeRender: function() {},

        afterRender: function() {
            if (this.model.get('kvm') > 0) this.$el.find('.kvm').show();
            else this.$el.find('.kvm').hide();
            if (this.model.get('vmedia') > 0) this.$el.find('.vmedia').show();
            else this.$el.find('.vmedia').hide();
            if (this.model.get('snmp') > 0) this.$el.find('.snmp').show();
            else this.$el.find('.snmp').hide();
            if(sessionStorage.privilege == "Administrator" || sessionStorage.id==this.model.get('id')){
                var href = this.$el.find('a').attr('href');
                this.$el.find('a').attr('href', href.replace(':id', this.model.get('id')));
            }
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
                locale: locale,
                model: this.model
            };
        }

    });

    return view;

});
