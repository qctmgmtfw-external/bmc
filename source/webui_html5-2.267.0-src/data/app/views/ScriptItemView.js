define(['jquery', 'underscore', 'backbone', 'app',
        //data

        //template
        'text!templates/script-item.html',
        //localize
        'i18n!strings/nls/common',
        //plugins
        // 'moment',
        // 'knob',
        // 'sparkline'
        'tooltip'
    ],

    function($, _, Backbone, app, ScriptItemTpl, CommonStrings) {

        var view = Backbone.View.extend({

            template: _.template(ScriptItemTpl),

            initialize: function() {
                this.model.bind("change:id", this.updateID, this);
                this.model.bind("change:author", this.updateAuthor, this);
                this.model.bind("change:name", this.updateScriptName, this);
                this.model.bind("remove", this.removeItem, this);
            },

            events: {
                //"click .script-edit": "openItem",
                "click .script-delete": "confirmDelete"
            },

            updateID: function(model, value) {
                this.$el.find(".script-id", value).data("id", value);
            },

            updateAuthor: function(model, value) {
                this.$el.find(".script-author", value);
            },

            updateScriptName: function(model, value) {
                this.$el.find(".script-title", value);
            },

            getTimeInfo: function() {
                var str = "Created " + moment(this.model.get('timestamp'), 'X').fromNow();
                return str;
            },

            beforeRender: function() {
                //this.$el.addClass("row");
            },

            afterRender: function() {
                this.$el.find(".script-delete").tooltip({
                    placement: 'top',
                    title: CommonStrings.click_confirm_deletion,
                    trigger: 'click'
                });
            },

            openItem: function() {
                app.router.navigate("script/" + this.model.get('id'), {
                    trigger: true
                });
            },

            confirmDelete: function(ev) {
                var btn = $(ev.currentTarget);

                if (btn.hasClass("btn-danger")) {
                    btn.tooltip('destroy');
                    this.model.destroy();
                }
                btn.removeClass("btn-default").addClass("btn-danger").tooltip('show');
                setTimeout(function() {
                    btn.removeClass("btn-danger").addClass("btn-default");
                    btn.tooltip('hide');
                }, 3500);
            },

            removeItem: function() {
                this.$el.remove();
            },

            serialize: function() {
                return {
                    locale: CommonStrings,
                    id: this.model.get('id'),
                    author: this.model.get('author'),
                    title: this.model.get('name'),
                    time_info: this.getTimeInfo()
                };
            }

        });

        return view;

    });