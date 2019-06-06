define(['jquery', 'underscore', 'backbone', 'app',
        //data
        'models/script',

        //template
        'text!templates/script-edit-item.html',
        //localize
        'i18n!strings/nls/common',
        //plugins
        // 'moment',
        // 'knob',
        // 'sparkline'
        'tooltip'
    ],

    function($, _, Backbone, app, ScriptModel, ScriptItemTpl, CommonStrings) {

        var view = Backbone.View.extend({

            //tagName: 'li',

            template: _.template(ScriptItemTpl),

            initialize: function() {
                this.model = new ScriptModel(this.script);
                this.model.bind("change:name", this.updateScriptName, this);
                this.model.bind("destory", this.removeItem, this);
            },

            events: {
                "click .script-edit": "editScript",
                "click .script-delete": "confirmDelete"
            },

            updateID: function(model, value) {
                this.$el.data("id", value);
            },

            updateScriptName: function(model, value) {
                this.$(".script-title").html(value);
            },


            beforeRender: function() {
                //this.$el.addClass("row");
            },

            afterRender: function() {
                this.$(".script-delete").tooltip({
                    placement: 'top',
                    title: CommonStrings.click_confirm_deletion,
                    trigger: 'click'
                });
            },

            confirmDelete: function(ev) {
                var btn = $(ev.currentTarget);
                var context = this;

                if (btn.hasClass("btn-danger")) {
                    btn.tooltip('destroy');
                    this.remove();
                }
                btn.removeClass("btn-default").addClass("btn-danger").tooltip('show');
                setTimeout(function() {
                    btn.removeClass("btn-danger").addClass("btn-default");
                    btn.tooltip('hide');
                }, 3500);
            },

            editScript: function() {
                this.trigger("edit-script", {model: this.model});
            },

            deleteScript: function() {
                //this.model.destroy({silent: true});
            },

            removeItem: function() {
                console.log("removed item");
                this.trigger("removed");
                this.$el.remove();
            },

            serialize: function() {
                return {
                    locale: CommonStrings,
                    title: this.model.get('name')
                };
            }

        });

        return view;

    });