define(['jquery', 'underscore', 'backbone', 'app',
//localize
'i18n!strings/nls/maintenance',
//template
'text!templates//maintenance.html'],

function($, _, Backbone, app, locale, MaintenanceTemplate) {

    var view = Backbone.View.extend({

        template: _.template(MaintenanceTemplate),

        initialize: function() {},

        events: {
            "click .help-link": "toggleHelp"
        },

        beforeRender: function() {},

        afterRender: function() {

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
            $('.tooltip').hide();
            if (help_items.hasClass('hide')) {
                help_items.removeClass('hide');
            } else {
                help_items.addClass('hide');
            }
        },
        serialize: {
            locale: locale
        }

    });

    return view;

});
