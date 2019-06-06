define(['jquery', 'underscore', 'backbone', 'app',
//localize
'i18n!strings/nls/services',
//template
'text!templates/settings/services_item.html'],

function($, _, Backbone, app, locale, ServicesTemplate) {

    var view = Backbone.View.extend({

        tagName: "tr",

        template: _.template(ServicesTemplate),

        initialize: function() {
            this.model.bind("destroy", this.removeItem, this);
        },

        events: {
            "click .help-link": "toggleHelp"
        },

        beforeRender: function() {},

        afterRender: function() {
            if(sessionStorage.privilege_id < CONSTANTS["ADMIN"]){
                $(".content .edit").addClass("disable_a_href");
            }
            
            this.$('.edit').removeClass('hide');
            var href = this.$el.find('a.edit').attr('href');
            this.$el.find('a.edit').attr('href', href.replace(':id', this.model.get('id')));

            if(app.features.indexOf("SESSION_MANAGEMENT") != -1){
                this.$('.action-1').removeClass('hide');
                var href = this.$el.find('a.action-1').attr('href');
                this.$el.find('a.action-1').attr('href', href.replace(':id', this.model.get('service_id')));    
            }
        },

        removeItem: function() {
            this.$el.remove();
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
