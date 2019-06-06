define(['jquery', 'underscore', 'backbone', 'app',
//localize
'i18n!strings/nls/lan_destinations',
//template
'text!templates/settings/pef/lan_destinations_item.html'],

function($, _, Backbone, app, locale, LanDestinationsTemplate) {

    var view = Backbone.View.extend({

        template: _.template(LanDestinationsTemplate),

        initialize: function() {},

        events: {
            "click .help-link": "toggleHelp"
        },

        beforeRender: function() {},

        afterRender: function() {
        	var context = this;
            var href = this.$el.find('a').attr('href');
            this.$el.find('a').attr('href', href.replace(':id', this.model.get('id')));
            this.$el.find('button').attr('id', "idtest_alert"+this.model.get('id'));
            if( this.model.get('destination_address') != "" || this.model.get('name') != "")
            {
                $("#idtest_alert"+this.model.get('id')).removeClass("disable_a_href");
                $("#idtest_alert"+this.model.get('id')).click(function(){
                    context.clickTestAlert();
             })
                 }
            else
            {
                $("#idtest_alert"+this.model.get('id')).addClass("disable_a_href");
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

        clickTestAlert: function(ev) {
        	$.ajax({
                url: "/api/settings/pef/send_test_alert",
                type: "POST",
                data: JSON.stringify({
                    lan_channel: this.model.get('lan_channel'),
                    channel_id: this.model.get('channel_id'),
                    id: this.model.get('id'),
                    destination_type: this.model.get('destination_type'),
                    destination_address: this.model.get('destination_address'),
                    username: this.model.get('name'),
                    subject: this.model.get('subject'),
                    message: this.model.get('message')
                }),
                processData: false,
                contentType: "application/json",
                success: function(data) {
                    alert(locale.test_email_sent_successfully);
                },
                error: function(xhr, response, options) {
                	app.events.trigger('save_error', this);
                }

            });
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
