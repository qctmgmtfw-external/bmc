define(['jquery', 'underscore', 'backbone', 'app',
        //localize
        'i18n!strings/nls/service_sessions',
        //template
        'text!templates/settings/service_sessions_item.html'
    ],

    function($, _, Backbone, app, locale, ServiceSessionsTemplate) {

        var view = Backbone.View.extend({

            tagName: "tr",
            client_sessionID: -1,

            template: _.template(ServiceSessionsTemplate),

            initialize: function() {
                this.model.bind("destroy", this.removeItem, this);
            },

            events: {
                "click .delete": "deleteItem",
                "click .help-link": "toggleHelp"
            },

            beforeRender: function() {},

            afterRender: function() {

                this.$('.delete').removeClass('hide');
                var sess_id = this.model.get("session_id");
                var IsEnable = app.features.indexOf("SESSION_MANAGEMENT");
                if (IsEnable != -1) {
                    this.client_sessionID = sessionStorage.session_id;
                }
                if (sessionStorage.session_id == sess_id) {
                    this.$el.find(".session_id").html(this.model.get("session_id") + "*")
                }
                if (sessionStorage.privilege_id < CONSTANTS["ADMIN"]) {
                    app.disableAllbutton();
                }
            },

            removeItem: function() {
                this.$el.remove();
            },

            deleteItem: function(ev) {
                ev.stopPropagation();
                ev.preventDefault();

                if (this.client_sessionID == this.model.get('id')) {
                    if (!confirm(locale.active_session_confirm)) {
                        return;
                    }
                } else if (!confirm(locale.active_session_confirm1)) {
                    return;
                }
                this.model.destroy({
                    url: this.model.collection.url().split('?')[0] + '/' + this.model.get('id')
                });
                if (this.client_sessionID == this.model.get('id')) {
                    $("body").addClass("disable_a_href");
                    document.cookie = "QSESSIONID=1;expires=Thu, 01 Jan 1970 00:00:00 GMT";
                    app.router.navigate("login");
                    location.reload();
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
