define(['jquery', 'underscore', 'backbone', 'app',
//localize
'i18n!strings/nls/raid_event_log',
//template
'text!templates/settings/raid_management/raid_event_log_item.html'],

function($, _, Backbone, app, locale, RaidEventLogTemplate) {

    var view = Backbone.View.extend({

        tagName: "tr",

        template: _.template(RaidEventLogTemplate),

        initialize: function() {
            this.model.bind("destroy", this.removeItem, this);
        },

        events: {
            "click .help-link": "toggleHelp",
            "click .description":"displayEventDescription"
        },

        beforeRender: function() {},

        afterRender: function() {
            //this.$('.description').removeClass('hide');
        },

        removeItem: function() {
            this.$el.remove();
        },

        displayEventDescription:function(ev){
            var classes = this.$el.closest("tr").prop("class"); 
            var nRow="<tr class='footable-even' style='display: table-row;'><td>this is for testing</td></tr>";

            var expand=this.$('.description i').prop("class");
            if(expand=='ion-arrow-down-a'){
                this.$('.description i').addClass("ion-arrow-up-a");
            } else if(expand=='ion-arrow-up-a'){
                this.$('.description i').addClass("ion-arrow-down-a");
                $(nRow).insertAfter(this.$el.parent);
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
